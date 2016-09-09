#!/bin/bash

sudo apt-get install ncurses-dev libevent-dev
extract ./tmux-2.2.tar.gz .
cd tmux-2.2
./configure
make
sudo make install