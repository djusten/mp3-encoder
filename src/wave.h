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
  uint32_t chunkId; //chunkID big
  uint32_t chunkSize; // chunkSize little
  uint32_t format; //format big
  uint32_t subChunk1Id; //subshunk1 id big
  uint32_t subChunk1Size; // subshunk1 size little
  uint16_t audioFormat; //audioFormat little
  uint16_t numChannels; //NumChannles little
  uint32_t sampleRatio; //sampleRatio little
  uint32_t byteRate; //byteRate little
  uint16_t blockAlign; //BlockAlign little
  uint16_t bitPerSample; //BitsPerSample little
  uint32_t subChunk2Id; //subchunk2id big
  uint32_t subChunk2Size; //subchunk2size little
} header_t;

// Public Variabels ///////////////////////////////////////////////////////////

// Public Functions ///////////////////////////////////////////////////////////

int wave_read_header(header_t *header, lame_t gf, FILE *music_in);

int wave_converter(header_t *header, lame_t gf, FILE *music_in);

#endif // WAVE_ENCODER_H__
// EOF
