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

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include "defines.h"
#include "mp3_encoder.h"

// Definitions ////////////////////////////////////////////////////////////////

#define MAX_ARGS 2

// Macros /////////////////////////////////////////////////////////////////////

// Datatypes, Structures and Enumerations /////////////////////////////////////

// Private Functions Prototypes //////////////////////////////////////////////

// Public Variables ///////////////////////////////////////////////////////////

// Private Variables //////////////////////////////////////////////////////////

// Private Functions //////////////////////////////////////////////////////////

static void show_version(void)
{
  printf("mp3_encoder Version %s\n", APP_VERSION);
  printf("Copyright (C) 2017  Diogo Justen. All rights reserved.\n");
}

static void usage(void)
{
  printf("mp3_encoder <directory>\n");
}

// Public Functions ///////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
  int opt;
  char *folder_name;
  struct timeval tv1, tv2;
  mp3_encoder_t mp3_encoder;

  gettimeofday(&tv1, NULL);

  if (argc != MAX_ARGS) {
    usage();
    return -1;
  }

  opt = getopt(argc, argv, "vh");
  switch (opt) {

    case 'v':
      show_version();
    return 0;

    case 'h':
      usage();
    return 0;
  }

  folder_name = strdup (argv[1]);
  if (!folder_name) {
    printf("Unable get folder name\n");
    return -1;
  }

  if (mp3_encoder_init(&mp3_encoder, folder_name) < 0) {
    printf("Unable init mp3 encoder\n");
    g_free(folder_name);
    return -1;
  }

  g_free(folder_name);

  if (mp3_encoder_process(&mp3_encoder) < 0) {
    printf("Error processing\n");
    return -1;
  }

  if (mp3_encoder_finish(&mp3_encoder) < 0) {
    printf("Unable finish mp3 encoder\n");
    return -1;
  }

  gettimeofday(&tv2, NULL);

  printf ("Total time = %f seconds\n",
      (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
      (double) (tv2.tv_sec - tv1.tv_sec));

  return 0;
}

// EOF
