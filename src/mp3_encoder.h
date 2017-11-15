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

// Definitions ////////////////////////////////////////////////////////////////

#define MAX_FILENAME_SIZE 255

// Macros /////////////////////////////////////////////////////////////////////

// Datatypes, Structures and Enumerations /////////////////////////////////////

// Public Variabels ///////////////////////////////////////////////////////////

// Public Functions ///////////////////////////////////////////////////////////

int mp3_encoder_init(char *folder_name);

int mp3_encoder_finish(void);

#endif // __MP3_ENCODER_H__
// EOF

