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

#ifndef __MP3_ENCODER_H__
#define __MP3_ENCODER_H__

// Includes ///////////////////////////////////////////////////////////////////

#include <glib.h>
#include <pthread.h>
#include <lame/lame.h>

// Definitions ////////////////////////////////////////////////////////////////

#define MAX_THREAD 16

// Macros /////////////////////////////////////////////////////////////////////

// Datatypes, Structures and Enumerations /////////////////////////////////////

typedef struct {
  GList *filename_list;
  int num_files;
  int num_cores;
  int process_pos;
  pthread_t thread[MAX_THREAD];
  pthread_mutex_t lock;
} mp3_encoder_t;

// Public Variabels ///////////////////////////////////////////////////////////

// Public Functions ///////////////////////////////////////////////////////////

int mp3_encoder_init(mp3_encoder_t *mp3_encoder, char *folder_name);

int mp3_encoder_process(mp3_encoder_t *mp3_encoder);

int mp3_encoder_finish(mp3_encoder_t *mp3_encoder);

#endif // __MP3_ENCODER_H__
// EOF

