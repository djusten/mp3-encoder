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
#include "mp3_encoder.h"

// Definitions ////////////////////////////////////////////////////////////////

#define MAX_ARGS 2
#define MAX_FOLDER_SIZE 255

// Macros /////////////////////////////////////////////////////////////////////

// Datatypes, Structures and Enumerations /////////////////////////////////////

// Private Functions Prototypes //////////////////////////////////////////////

// Public Variables ///////////////////////////////////////////////////////////

// Private Variables //////////////////////////////////////////////////////////

// Private Functions //////////////////////////////////////////////////////////

static void usage(void)
{
  printf("mp3_encoder <file>\n");
}

// Public Functions ///////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
  char folder_name[MAX_FOLDER_SIZE];

  if (argc != MAX_ARGS) {
    usage();
    return -1;
  }

  if (strncpy(folder_name, argv[1], sizeof(folder_name)) < 0) {
    printf("Unable get folder name\n");
    return -1;
  }

  if (mp3_encoder_init(folder_name) < 0) {
    printf("Unable init mp3 encoder\n");
    return -1;
  }

  if (mp3_encoder_finish() < 0) {
    printf("Unable finish mp3 encoder\n");
    return -1;
  }

  return 0;
}

// EOF

