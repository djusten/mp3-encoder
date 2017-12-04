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

#include <ctype.h>
#include <dirent.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "mp3_encoder.h"
#include "wave.h"

// Definitions ////////////////////////////////////////////////////////////////

//#define DEBUG
#define WAV_EXTENSION ".wav"
#define MP3_EXTENSION ".mp3"
#define EXTENSION_SIZE 4
#define FILENAME_LEN 128

// Macros /////////////////////////////////////////////////////////////////////

// Datatypes, Structures and Enumerations /////////////////////////////////////

// Private Functions Prototypes //////////////////////////////////////////////

// Public Variables ///////////////////////////////////////////////////////////

// Private Variables //////////////////////////////////////////////////////////

// Private Functions //////////////////////////////////////////////////////////

static int check_wav_extension(char *filename)
{
  char *p;
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
    if (tolower(p[i]) != WAV_EXTENSION[i]) {
      return -1;
    }
  }

  return 0;
}

static int get_wav_list(GList **filename_list, char *folder_name)
{
  struct dirent **namelist;
  int num_files;
  int i;
  char filename_complete[PATH_MAX];

  g_assert(folder_name);

  num_files = scandir(folder_name, &namelist, NULL, alphasort);
  if (num_files < 0) {
    printf("Error scandir: %s\n", folder_name);
    return -1;
  }

  for (i = 0; i < num_files; i++) {
    if (check_wav_extension(namelist[i]->d_name) == 0) {
      snprintf(filename_complete, sizeof(filename_complete), "%s/%s", folder_name, namelist[i]->d_name);
      *filename_list = g_list_append(*filename_list, g_strdup(filename_complete));
    }
  }

  return 0;
}

static int get_num_cores(void)
{
  int num;

  num = sysconf(_SC_NPROCESSORS_ONLN);
  if (num > MAX_THREAD) {
    num = MAX_THREAD;
  }

  return num;
}

static int convert(lame_t gf, char *filename_in, char *filename_out)
{
  header_t header;
  FILE *music_in;

  g_assert(filename_in);
  g_assert(filename_out);

  printf("Converting %s to %s\n", filename_in, filename_out);

  music_in = fopen(filename_in, "rb");
  if (!music_in) {
    printf("Unable open input file\n");
    return -1;
  }

  if (wave_read_header(&header, music_in) < 0) {
    printf("Unable read header\n");
    fclose(music_in);
    return -1;
  }

  if (wave_converter(&header, gf, music_in, filename_out) < 0) {
    printf("Unable converter\n");
    fclose(music_in);
    return -1;
  }

  fclose(music_in);

  return 0;
}

static void *thread_func(void *arg)
{
  g_assert(arg);

  mp3_encoder_t *mp3_encoder = (mp3_encoder_t *) arg;
  gpointer *p;
  char filename_in[FILENAME_LEN];
  char filename_out[FILENAME_LEN];
  int8_t pos;
  lame_t gf;

  g_assert(mp3_encoder);

  while (1) {

    pthread_mutex_lock(&mp3_encoder->lock);
    pos = mp3_encoder->process_pos;
    mp3_encoder->process_pos++;
    pthread_mutex_unlock(&mp3_encoder->lock);

    if (pos >= mp3_encoder->num_files) {
      break;
    }

    gf = lame_init();
    if (!gf) {
      printf("Error during initialization\n");
    }

    p = g_list_nth_data(mp3_encoder->filename_list, pos);
    if (p) {

      memset(filename_in, '\0', sizeof(filename_in));
      memset(filename_out, '\0', sizeof(filename_out));

      strncpy(filename_in, (const char *)p, sizeof(filename_in));

      strncpy(filename_out, filename_in, strlen(filename_in) - EXTENSION_SIZE);
      strncat(filename_out, MP3_EXTENSION, strlen(MP3_EXTENSION));

      convert(gf, filename_in, filename_out);
    }

    lame_close(gf);
  }

  return 0;
}

// Public Functions ///////////////////////////////////////////////////////////

int mp3_encoder_init(mp3_encoder_t *mp3_encoder, char *folder_name)
{
  g_assert(mp3_encoder);
  g_assert(folder_name);

  mp3_encoder->filename_list = NULL;
  mp3_encoder->num_cores = 0;
  mp3_encoder->process_pos = 0;

  if (get_wav_list(&mp3_encoder->filename_list, folder_name) < 0) {
    printf("Unable get wav files\n");
    return -1;
  }

  mp3_encoder->num_files = g_list_length(mp3_encoder->filename_list);
  if (mp3_encoder->num_files == 0) {
    printf("Folder empty\n");
    return 0;
  }

#ifdef DEBUG
  GList *elem;
  for (elem = g_list_first(mp3_encoder->filename_list); elem != NULL ; elem = g_list_next(elem)) {
    printf("%s\n", (char *) elem->data);
  }
#endif

  if (pthread_mutex_init(&mp3_encoder->lock, NULL) != 0) {
    printf("\n Mutex init failed\n");
    return 1;
  }

  mp3_encoder->num_cores = get_num_cores();

  return 0;
}

int mp3_encoder_process(mp3_encoder_t *mp3_encoder)
{
  int i;

  g_assert(mp3_encoder);

  for (i = 0; i < mp3_encoder->num_cores; i++) {
    if (pthread_create(&mp3_encoder->thread[i], NULL, thread_func, mp3_encoder) != 0) {
      printf("Unable create thread%d\n", i);
    }
  }

  return 0;
}

int mp3_encoder_finish(mp3_encoder_t *mp3_encoder)
{
  int i;

  for (i = 0; i < mp3_encoder->num_cores; i++) {
    pthread_join(mp3_encoder->thread[i], NULL);
  }

  g_list_free(mp3_encoder->filename_list);

  pthread_mutex_destroy(&mp3_encoder->lock);

  return 0;
}

// EOF
