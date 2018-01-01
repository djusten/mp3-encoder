# WAV to MP3 Converter

This is a project to create a converter from WAV to MP3 files. Following requirements from Cinemo.

## Project Build

### Linux:
  * Use ```./generate.sh -p``` to prepare an enviroment, require sudo.
  * Use ```./generate.sh -b``` to build a project, binary will be on ./build/mp3_encoder.
  * Use ```./generate.sh -r``` to run a sample. The result will be on ./buid/training
  * Use ```./generate.sh``` to show help options.

### Windows:
  * Use Cygwin to compile and run this application.
  * It's necessary install Cygwin with support for:
    * cmake,
    * pkg-config,
    * glib2.0
    * libmp3lame,
    * git,
    * make and
    * gcc.
  * Use ```./generate.sh -b``` to build a project, binary will be on ./build/mp3_encoder.
  * Use ```./generate.sh -r``` to run a sample. The result will be on ./buid/training
  * Use ```./generate.sh``` to show help options.
