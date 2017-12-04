/*
 * Copyright (C) 2017  Diogo Justen. All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// Includes ///////////////////////////////////////////////////////////////////

#include <byteswap.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "wave.h"
#include <stdlib.h>

// Definitions ////////////////////////////////////////////////////////////////

// Macros /////////////////////////////////////////////////////////////////////

// Datatypes, Structures and Enumerations /////////////////////////////////////

// Private Functions Prototypes //////////////////////////////////////////////

// Public Variables ///////////////////////////////////////////////////////////

// Private Variables //////////////////////////////////////////////////////////

static uint32_t const WAV_ID_RIFF = 0x52494646; /* "RIFF" */
static uint32_t const WAV_ID_WAVE = 0x57415645; /* "WAVE" */
static uint32_t const WAV_ID_FMT = 0x666d7420; /* "fmt " */
static uint32_t const WAV_ID_DATA = 0x64617461; /* "data" */

// Private Functions //////////////////////////////////////////////////////////

static void swap(header_t *header)
{
  header->chunkId = __bswap_32(header->chunkId);
  header->format = __bswap_32(header->format);
  header->subChunk1Id = __bswap_32(header->subChunk1Id);
  header->subChunk2Id = __bswap_32(header->subChunk2Id);
}

static int encode_to_file(lame_global_flags *gfp, const short *leftPcm, const short *rightPcm, FILE *out, int numSamples)
{
  int mp3BufferSize = numSamples * 5 / 4 + 7200; //FIXME
  unsigned char mp3Buffer[mp3BufferSize];

  int mp3size = lame_encode_buffer(gfp, leftPcm, rightPcm, numSamples, mp3Buffer, mp3BufferSize);
  if (!(mp3size > 0)) {
    printf("No data was encoded by lame_encode_buffer. Return code:%d \n", mp3size);
    return -1;
  }

  fwrite((void *)mp3Buffer, sizeof(unsigned char), mp3size, out);

  int flushSize = lame_encode_flush(gfp, mp3Buffer, mp3BufferSize);

  fwrite((void *)mp3Buffer, sizeof(unsigned char), flushSize, out);

  lame_mp3_tags_fid(gfp, out);

#ifdef DEBUG
  printf("Wrote %d bytes\n", mp3size + flushSize);
#endif

  return 0;
}

static int skip_extension(header_t *header, FILE *music_in)
{
  uint16_t skip;
  uint8_t i;
  uint32_t data = 0;

  fread(&skip, 1, sizeof(skip), music_in);
  skip = __bswap_16(skip);

  for (i = 0; i < header->subChunk1Size; i++) {

    data = data << 16;
    fread(&skip, 1, sizeof(skip), music_in);
    skip = __bswap_16(skip);
    data = (data | skip);

    if (data == WAV_ID_DATA) {
      return 0;
    }
  }

  return -1;
}

// Public Functions ///////////////////////////////////////////////////////////

int wave_read_header(header_t *header, FILE *music_in)
{
  fread(header, 1, sizeof(header_t), music_in);

  swap(header);

  if (header->subChunk2Id != WAV_ID_DATA) {
    if (skip_extension(header, music_in) < 0) {
      printf("Unable skip extented header\n");
      return -1;
    }

    header->subChunk2Id = WAV_ID_DATA;
    fread(&header->subChunk2Size, 1, sizeof(header->subChunk2Size), music_in);
  }

  if (header->chunkId != WAV_ID_RIFF) {
    printf("Unsupported audio format\n");
    return -1;
  }

  if (header->format != WAV_ID_WAVE) {
    return -1;
  }

  if (header->subChunk1Id != WAV_ID_FMT) {
    printf("Unsupported audio format\n");
    return -1;
  }

  return 0;
}

int wave_converter(header_t *header, lame_t gf, FILE *music_in, char *filename_out)
{
  FILE *outf;
  int idx;

  outf = fopen(filename_out, "w+b");

  lame_set_brate(gf, 192); // increase bitrate FIXME
  lame_set_quality(gf, 3); //FIXME
  lame_set_bWriteVbrTag(gf, 0);

  short *leftPcm = malloc(header->subChunk2Size / header->numChannels * sizeof(short));
  short *rightPcm = malloc(header->subChunk2Size / header->numChannels * sizeof(short));

  int numSamples = header->subChunk2Size / header->blockAlign;
  for (idx = 0; idx < numSamples; idx++) {
    fread(&leftPcm[idx], 1, 2, music_in);
    fread(&rightPcm[idx], 1, 2, music_in);
  }

  lame_set_num_channels(gf, header->numChannels);
  lame_set_num_samples(gf, header->subChunk2Size / header->blockAlign);
  lame_set_in_samplerate(gf, header->sampleRatio);

  if (lame_init_params(gf) != 0) {
    printf("Error init params\n");
    fclose(outf);
    free(leftPcm);
    free(rightPcm);
    return -1;
  }

  if (encode_to_file(gf, leftPcm, rightPcm, outf, numSamples) < 0) {
    printf("Unable encode to file\n");
    fclose(outf);
    free(leftPcm);
    free(rightPcm);
    return -1;
  }

  fclose(outf);

  lame_close(gf);

  free(leftPcm);
  free(rightPcm);

  return 0;
}

// EOF
