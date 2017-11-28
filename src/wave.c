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

#define         MAX_U_32_NUM            0xFFFFFFFF

// Macros /////////////////////////////////////////////////////////////////////

// Datatypes, Structures and Enumerations /////////////////////////////////////

// Private Functions Prototypes //////////////////////////////////////////////

// Public Variables ///////////////////////////////////////////////////////////

// Private Variables //////////////////////////////////////////////////////////

static int const WAV_ID_RIFF = 0x52494646; /* "RIFF" */
static int const WAV_ID_WAVE = 0x57415645; /* "WAVE" */
static int const WAV_ID_FMT = 0x666d7420; /* "fmt " */
static int const WAV_ID_DATA = 0x64617461; /* "data" */

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
  int mp3BufferSize = numSamples * 5 / 4 + 7200; // worst case estimate
  unsigned char mp3Buffer[mp3BufferSize];

  int mp3size = lame_encode_buffer(gfp, leftPcm, rightPcm, numSamples, mp3Buffer, mp3BufferSize);
  if (!(mp3size > 0)) {
    printf("No data was encoded by lame_encode_buffer. Return code:%d \n", mp3size);
    return -1;
  }

  printf("%d %d\n", numSamples, mp3BufferSize);

  fwrite((void *)mp3Buffer, sizeof(unsigned char), mp3size, out);

  int flushSize = lame_encode_flush(gfp, mp3Buffer, mp3BufferSize);

  fwrite((void *)mp3Buffer, sizeof(unsigned char), flushSize, out);

  lame_mp3_tags_fid(gfp, out);

  printf("Wrote %d bytes\n", mp3size + flushSize);

  return 0;
}

// Public Functions ///////////////////////////////////////////////////////////

int wave_read_header(header_t *header, lame_t gf, FILE *music_in)
{
  uint16_t skip;
  int i;

  fread(header, 1, sizeof(header_t), music_in);

  swap(header);

  if (header->subChunk2Id != WAV_ID_DATA) {
    /* skip extension */
    for (i = 0; i < header->subChunk1Size; i++) {
      fread(&skip, 1, sizeof(skip), music_in);
      if (skip != 0x6164) {
        continue;
      }
      fread(&skip, 1, sizeof(skip), music_in);
      if (skip != 0x6174) {
        continue;
      }

      header->subChunk2Id = WAV_ID_DATA;
      fread(&header->subChunk2Size, 1, sizeof(header->subChunk2Size), music_in);
      break;
    }
  }

  if (header->chunkId != WAV_ID_RIFF) {
    printf("Unsupported audio format\n");
    return -1;
  }

  if (header->format != WAV_ID_WAVE) {
    return -1;
  }

  if (header->subChunk1Id == WAV_ID_FMT) {
//    printf("FMT\n");
  }
  else if(header->subChunk1Id == WAV_ID_DATA) {
    printf("ID\n");
  }
  else {
    printf("ELSE\n");
  }

//  lame_set_num_samples(gf, MAX_U_32_NUM);

  return 0;
}

int wave_converter(header_t *header, lame_t gf, FILE *music_in)
{
  FILE *outf;
  char filename[128];
  static int i = 0;
  int idx;

  snprintf(filename, sizeof(filename), "arquivo%d.mp3", i++);
//  printf("gerando: [%s]\n", filename);
  outf = fopen(filename, "w+b");

  lame_set_brate(gf, 192); // increase bitrate
  lame_set_quality(gf, 3);
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
  //  lame_set_num_samples(gf, header->subChunk2Size / (header->numChannels * ((header->bitPerSample + 7) / 8)));

  if (lame_init_params(gf) != 0) {
    printf("error init params\n");
    fclose(outf);
    return -1;
  }

  encode_to_file(gf, leftPcm, rightPcm, outf, numSamples);

  fclose(outf);

  lame_close(gf);

  free(leftPcm);
  free(rightPcm);

  return 0;
}

// EOF
