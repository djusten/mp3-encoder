# Copyright (C) 2017  Diogo Justen. All rights reserved.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required (VERSION 3.0)
project (mp3encoder C)
find_package (PkgConfig REQUIRED)

message(STATUS "We are on a ${CMAKE_SYSTEM_NAME} system")

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  add_definitions(-DSYSTEM_LINUX)
endif()
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  add_definitions(-DSYSTEM_WINDOWS)
  SET(CMAKE_MAKE_PROGRAM /usr/bin/make.exe FORCE)
endif()

message(STATUS "The host processor is ${CMAKE_HOST_SYSTEM_PROCESSOR}")

set(CMAKE_C_FLAGS "-Wall -Werror -Wextra -pedantic")

#### glib
pkg_check_modules (GLIB glib-2.0)

link_directories (${GLIB_LIBRARY_DIRS})
include_directories (${GLIB_INCLUDE_DIRS})

#### MP3 encoder
add_executable (mp3_encoder
  src/main.c
  src/mp3_encoder.c
  src/wave.c
  )

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  target_link_libraries (mp3_encoder ${GLIB_LIBRARIES} libmp3lame.a m pthread)
else()
  target_link_libraries (mp3_encoder ${GLIB_LIBRARIES} libmp3lame m pthread)
endif()

include_directories(
  ${PROJECT_SOURCE_DIR}/src
  )
