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
project (mp3encoder)
find_package (PkgConfig REQUIRED)

enable_language(C)

message(STATUS "We are on a ${CMAKE_SYSTEM_NAME} system")
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  add_definitions(-DSYSTEM_LINUX)
endif()
if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
  add_definitions(-DSYSTEM_DARWIN)
endif()
if(${CMAKE_SYSTEM_NAME} STREQUAL "AIX")
  add_definitions(-DSYSTEM_AIX)
endif()
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  add_definitions(-DSYSTEM_WINDOWS)
endif()

message(STATUS "The host processor is ${CMAKE_HOST_SYSTEM_PROCESSOR}")

##### glib
pkg_check_modules (GLIB glib-2.0)

link_directories (${GLIB_LIBRARY_DIRS})
include_directories (${GLIB_INCLUDE_DIRS})

#### MP3 encoder
add_executable (mp3_encoder
  src/mp3_encoder_main.c
  src/mp3_encoder.c
  )
target_link_libraries (mp3_encoder ${GLIB_LIBRARIES})
add_compile_options (-Wall -Werror -ggdb)

include_directories(
  ${PROJECT_SOURCE_DIR}/src
  )
