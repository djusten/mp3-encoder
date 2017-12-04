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
  uint32_t chunkId;
  uint32_t chunkSize;
  uint32_t format;
  uint32_t subChunk1Id;
  uint32_t subChunk1Size;
  uint16_t audioFormat;
  uint16_t numChannels;
  uint32_t sampleRatio;
  uint32_t byteRate;
  uint16_t blockAlign;
  uint16_t bitPerSample;
  uint32_t subChunk2Id;
  uint32_t subChunk2Size;
} header_t;

// Public Variabels ///////////////////////////////////////////////////////////

// Public Functions ///////////////////////////////////////////////////////////

int wave_read_header(header_t *header, FILE *music_in);

int wave_converter(header_t *header, lame_t gf, FILE *music_in, char *filename_out);

#endif // WAVE_ENCODER_H__
// EOF
