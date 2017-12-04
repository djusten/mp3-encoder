#!/bin/sh

PROJECT_DIR=`pwd`
BUILD_DIR=${PROJECT_DIR}/build
SUDO=''

prepare () {
  echo ">>> Preparing environment..."

  USER=`id -u`

  if [ "${USER}" -ne 0 ]; then
    echo "! This preparation needs to run as root"
    SUDO=sudo
  fi

  echo "This preparation will install the following packages and its dependencies:"
  echo "- cmake"
  echo "- pkg-config"
  echo "- libglib2.0-dev"
  echo "- libmp3lame-dev"
  echo -n "Do you agree? (Y/n) "

  read opt
  echo

  if [ "${opt}" != "n" ]; then
    ${SUDO} apt-get -y install cmake \
      pkg-config \
      libglib2.0-dev \
      libmp3lame-dev
  fi
}

build () {
  echo ">>> Building..."
  if [ ! -d ${BUILD_DIR} ]; then
    mkdir ${BUILD_DIR}
  fi
  cd ${BUILD_DIR} && \
    cmake .. && \
    make
}

clean () {
  echo ">>> Cleaning..."
  cd ${BUILD_DIR} 2> /dev/null && \
    make clean
}

deepclean () {
  echo ">>> Erasing build dir..."
  if [ ! -d ${BUILD_DIR} ]; then
    echo "! Build dir does not exist."
    return
  fi
  rm -rf ${BUILD_DIR} 2> /dev/null
}

invalid() {
  echo "Invalid option: -$1"
}

param() {
  echo "Option -$1 requires an argument"
}

check() {
  if [ $1 != 0 ]; then
    echo "! Error running last command =("
    exit 1
  fi
}

usage() {
  echo "Usage: $1 [OPTIONS]"
  echo ""
  echo "  -p \t\tPrepare environment (requires sudo)"
  echo "  -b \t\tBuild"
  echo "  -c \t\tClean"
  echo "  -d \t\tErase build dir"
  echo ""
}

RUN=0
while getopts ":p :b :c :d" opt; do
  RUN=1
  case $opt in
    p)
      prepare
      ;;
    b)
      build
      ;;
    c)
      clean
      ;;
    d)
      deepclean
      ;;
    :)
      param $OPTARG
      RUN=0
      ;;
    \?)
      invalid $OPTARG
      RUN=0
      ;;
  esac
  check $?
done

if [ "$RUN" != "1" ]; then
  usage $0
  exit 1
fi
