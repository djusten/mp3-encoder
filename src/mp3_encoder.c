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
#include <unistd.h>
#include "mp3_encoder.h"

// Definitions ////////////////////////////////////////////////////////////////

//#define DEBUG

#define WAV_EXTENSION           ".wav"
#define EXTENSION_SIZE          4
#define         MAX_U_32_NUM            0xFFFFFFFF

static int const WAV_ID_RIFF = 0x52494646; /* "RIFF" */
static int const WAV_ID_WAVE = 0x57415645; /* "WAVE" */
static int const WAV_ID_FMT = 0x666d7420; /* "fmt " */
static int const WAV_ID_DATA = 0x64617461; /* "data" */

// Macros /////////////////////////////////////////////////////////////////////

// Datatypes, Structures and Enumerations /////////////////////////////////////

// Private Functions Prototypes //////////////////////////////////////////////

// Public Variables ///////////////////////////////////////////////////////////

// Private Variables //////////////////////////////////////////////////////////

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
      //*filename_list = g_list_append(*filename_list, namelist[i]->d_name);
      *filename_list = g_list_append(*filename_list, filename_complete);
    }
  }

  return 0;
}

static int get_num_cores(void)
{
  return sysconf(_SC_NPROCESSORS_ONLN);
}

static int
read_32_bits_high_low(FILE * fp)
{
  unsigned char bytes[4] = { 0, 0, 0, 0 };
  fread(bytes, 1, 4, fp);
  {
    int32_t const low = bytes[3];
    int32_t const medl = bytes[2];
    int32_t const medh = bytes[1];
    int32_t const high = (signed char) (bytes[0]);
    return (high << 24) | (medh << 16) | (medl << 8) | low;
  }
}

static int read_header(lame_t gf, char *filename)
{
  FILE   *music_in;

  music_in = fopen(filename, "rb");
  if (!music_in) {
    printf("Unable open input file\n");
    return -1;
  }

  int type = read_32_bits_high_low(music_in);

  if (type == WAV_ID_RIFF) {
    printf("wav\n");
  }

  read_32_bits_high_low(music_in);

  if (type != WAV_ID_WAVE) {
    fclose(music_in);
    return -1;
  }

  lame_set_num_samples(gf, MAX_U_32_NUM);

  fclose(music_in);

  return 0;
}

static int convert(lame_t gf, gpointer *filename)
{
  char name[255];

  strcpy(name, (char *) filename);
  printf("converting %s\n", name);

  read_header(gf, name);

  return 0;
}

static void *thread_func(void *arg)
{
  mp3_encoder_t *mp3_encoder = (mp3_encoder_t *) arg;
  gpointer *filename;
  int pos;
  lame_t gf;

  if (!mp3_encoder) {
    printf("eh nulo\n");
  }

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



    filename = g_list_nth_data(mp3_encoder->filename_list, pos);
    if (filename) {
      convert(gf, filename);
    }

    lame_close(gf);
  }
}

// Public Functions ///////////////////////////////////////////////////////////

int mp3_encoder_init(mp3_encoder_t *mp3_encoder, char *folder_name)
{
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
    printf("\n mutex init failed\n");
    return 1;
  }

  mp3_encoder->num_cores = get_num_cores();

  return 0;
}

int mp3_encoder_process(mp3_encoder_t *mp3_encoder)
{
  int i;

  for (i = 0; i < mp3_encoder->num_cores; i++) {
    pthread_create(&mp3_encoder->thread[i], NULL, thread_func, mp3_encoder);
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
