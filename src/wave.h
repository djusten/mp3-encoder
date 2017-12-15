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

#ifndef WAVE_ENCODER_H__
#define WAVE_ENCODER_H__

// Includes ///////////////////////////////////////////////////////////////////

#include <lame/lame.h>

// Definitions ////////////////////////////////////////////////////////////////

// Macros /////////////////////////////////////////////////////////////////////

// Datatypes, Structures and Enumerations /////////////////////////////////////

typedef struct {
  uint32_t chunk_id;
  uint32_t chunk_size;
  uint32_t format;
  uint32_t sub_chunk1_id;
  uint32_t sub_chunk1_size;
  uint16_t audio_format;
  uint16_t num_channels;
  uint32_t sample_ratio;
  uint32_t byte_rate;
  uint16_t block_align;
  uint16_t bit_per_sample;
  uint32_t sub_chunk2_id;
  uint32_t sub_chunk2_size;
} header_t;

// Public Variabels ///////////////////////////////////////////////////////////

// Public Functions ///////////////////////////////////////////////////////////

int wave_read_header(header_t *header, FILE *music_in);

int wave_converter(header_t *header, lame_t gf, FILE *music_in, char *filename_out);

#endif // WAVE_ENCODER_H__
// EOF
