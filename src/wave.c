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
#include <glib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "wave.h"

// Definitions ////////////////////////////////////////////////////////////////

// Macros /////////////////////////////////////////////////////////////////////

// Datatypes, Structures and Enumerations /////////////////////////////////////

typedef enum {
  QUALITY_0 = 0, /* best */
  QUALITY_1,
  QUALITY_2,
  QUALITY_3,
  QUALITY_4,
  QUALITY_5,     /* good */
  QUALITY_6,
  QUALITY_7,
  QUALITY_8,
  QUALITY_9      /* worst */
} quality_t;

// Private Functions Prototypes //////////////////////////////////////////////

// Public Variables ///////////////////////////////////////////////////////////

// Private Variables //////////////////////////////////////////////////////////

static uint32_t const WAV_ID_RIFF = 0x52494646;
static uint32_t const WAV_ID_WAVE = 0x57415645;
static uint32_t const WAV_ID_FMT = 0x666d7420;
static uint32_t const WAV_ID_DATA = 0x64617461;

// Private Functions //////////////////////////////////////////////////////////

static void swap(header_t *header)
{
  g_assert(header);

  header->chunk_id = __bswap_32(header->chunk_id);
  header->format = __bswap_32(header->format);
  header->sub_chunk1_id = __bswap_32(header->sub_chunk1_id);
  header->sub_chunk2_id = __bswap_32(header->sub_chunk2_id);
}

static int encode_to_file(lame_global_flags *gfp,
                           const short *left_pcm,
                           const short *right_pcm,
                           FILE *out,
                           int num_samples)
{
  g_assert(gfp);
  g_assert(left_pcm);
  g_assert(right_pcm);
  g_assert(out);

  int mp3_buffer_size = num_samples * 5 / 4 + 7200;
  unsigned char mp3_buffer[mp3_buffer_size];

  int mp3_size = lame_encode_buffer(gfp,
                                    left_pcm,
                                    right_pcm,
                                    num_samples,
                                    mp3_buffer,
                                    mp3_buffer_size);
  if (!(mp3_size > 0)) {
    printf("No data was encoded by lame_encode_buffer. Return code:%d \n", mp3_size);
    return -1;
  }

  fwrite((void *)mp3_buffer, sizeof(unsigned char), mp3_size, out);

  int flush_size = lame_encode_flush(gfp, mp3_buffer, mp3_buffer_size);

  fwrite((void *)mp3_buffer, sizeof(unsigned char), flush_size, out);

  lame_mp3_tags_fid(gfp, out);

#ifdef DEBUG
  printf("Wrote %d bytes\n", mp3_size + flush_size);
#endif

  return 0;
}

static int skip_extension(header_t *header, FILE *music_in)
{
  uint16_t skip;
  uint8_t i;
  uint32_t data = 0;

  g_assert(header);
  g_assert(music_in);

  fread(&skip, 1, sizeof(skip), music_in);
  skip = __bswap_16(skip);

  for (i = 0; i < header->sub_chunk1_size; i++) {

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
  g_assert(header);
  g_assert(music_in);

  fread(header, 1, sizeof(header_t), music_in);

  swap(header);

  if (header->sub_chunk2_id != WAV_ID_DATA) {
    if (skip_extension(header, music_in) < 0) {
      printf("Unable skip extented header\n");
      return -1;
    }

    header->sub_chunk2_id = WAV_ID_DATA;
    fread(&header->sub_chunk2_size, 1, sizeof(header->sub_chunk2_size), music_in);
  }

  if (header->chunk_id != WAV_ID_RIFF) {
    printf("Unsupported audio format\n");
    return -1;
  }

  if (header->format != WAV_ID_WAVE) {
    printf("Unsupported audio format\n");
    return -1;
  }

  if (header->sub_chunk1_id != WAV_ID_FMT) {
    printf("Unsupported audio format\n");
    return -1;
  }

  return 0;
}

int wave_converter(header_t *header, lame_t gf, FILE *music_in, char *filename_out)
{
  FILE *outf;
  int idx;

  g_assert(header);
  g_assert(music_in);
  g_assert(filename_out);

  outf = fopen(filename_out, "w+b");
  if (!outf) {
    printf("Unbale opeb destination file: %sd\n", filename_out);
    return -1;
  }

  lame_set_quality(gf, QUALITY_5);
  lame_set_bWriteVbrTag(gf, 0);

  short *left_pcm =
    malloc(header->sub_chunk2_size / header->num_channels * sizeof(short));
  if (!left_pcm) {
    printf("Unable to allocate memory\n");
    return -1;
  }

  short *right_pcm =
    malloc(header->sub_chunk2_size / header->num_channels * sizeof(short));
  if (!right_pcm) {
    printf("Unable to allocate memory\n");
    g_free(left_pcm);
    return -1;
  }

  int num_samples = header->sub_chunk2_size / header->block_align;
  for (idx = 0; idx < num_samples; idx++) {
    fread(&left_pcm[idx], 1, sizeof(short), music_in);
    fread(&right_pcm[idx], 1, sizeof(short), music_in);
  }

  lame_set_num_channels(gf, header->num_channels);
  lame_set_num_samples(gf, header->sub_chunk2_size / header->block_align);
  lame_set_in_samplerate(gf, header->sample_ratio);

  if (header->bit_per_sample <= 0) {
    printf("Unsupported bits per sample: %d\n", header->bit_per_sample);
    return -1;
  }

  if (lame_init_params(gf) != 0) {
    printf("Error init params\n");
    fclose(outf);
    free(left_pcm);
    free(right_pcm);
    return -1;
  }

  if (encode_to_file(gf, left_pcm, right_pcm, outf, num_samples) < 0) {
    printf("Unable encode to file\n");
    fclose(outf);
    free(left_pcm);
    free(right_pcm);
    return -1;
  }

  fclose(outf);
  free(left_pcm);
  free(right_pcm);

  return 0;
}

// EOF
