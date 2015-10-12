#!/bin/bash

if [ "$(uname -s)" == "Darwin" ] & [ "$1" == "init" ]; then
  echo "Install initial dependencies (OSX - brew)"
  brew install vim --with-lua
elif [ "$(uname -s)" == "Linux" ] & [ "$1" == "init" ]; then
  echo "Install initial dependencies (Linux - apt)"
  apt-get install vim-gtk
fi 

if [ "$(uname -s)" == "Darwin" ]; then
  echo "Found OS X: Copy custom settings"
  
elif [ "$(uname -s)" == "Linux" ]; then
  echo "Found Linux: Copy custom settings"
fi
