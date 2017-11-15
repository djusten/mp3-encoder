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

#include <dirent.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <glib.h>
#include <lame/lame.h>
#include "mp3_encoder.h"

// Definitions ////////////////////////////////////////////////////////////////

#define DEBUG

#define LOG_IDENTIFICATION      "[MP3-ENCODER]"
#define LOG_FACILITY            LOG_LOCAL1
#define WAV_EXTENSION           ".wav"
#define EXTENSION_SIZE          4

// Macros /////////////////////////////////////////////////////////////////////

// Datatypes, Structures and Enumerations /////////////////////////////////////

// Private Functions Prototypes //////////////////////////////////////////////

// Public Variables ///////////////////////////////////////////////////////////

// Private Variables //////////////////////////////////////////////////////////

static GList *filename_list = NULL;

// Private Functions //////////////////////////////////////////////////////////

static int check_wav_extension(char *filename)
{
  int len;
  char *p;
  char extension[EXTENSION_SIZE];
  int i;

  g_assert(filename);

  p = strrchr(filename, '.');
  if (!p || p == filename) {
    return -1;
  }

  if (strlen(p) != EXTENSION_SIZE) {
    return -1;
  }

  for (i = 0; i < EXTENSION_SIZE; i++) {
    if (p[i] != WAV_EXTENSION[i]) {
      return -1;
    }
  }

  return 0;
}

static int get_wav_list(char *folder_name)
{
  struct dirent **namelist;
  int num_files;
  int i;

  g_assert(folder_name);

  num_files = scandir(folder_name, &namelist, NULL, alphasort);
  if (num_files < 0) {
    printf("Error scandir: %s\n", folder_name);
    return -1;
  }

  for (i = 0; i < num_files; i++) {
    if (check_wav_extension(namelist[i]->d_name) == 0) {
      filename_list = g_list_append(filename_list, namelist[i]->d_name);
    }
  }

  return 0;
}

// Public Functions ///////////////////////////////////////////////////////////

int mp3_encoder_init(char *folder_name)
{
  if (get_wav_list(folder_name) < 0) {
    printf("Unable get wav files\n");
    return -1;
  }

#ifdef DEBUG
  GList *elem;
  for (elem = g_list_first(filename_list); elem != NULL ; elem = g_list_next(elem)) {
    printf("%s\n", (char *) elem->data);
  }
#endif

  return 0;
}

int mp3_encoder_finish(void)
{

  g_list_free(filename_list);

  return 0;
}

// EOF
