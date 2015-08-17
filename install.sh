#!/bin/bash

if [ "$1" == "-f" ] || [ "$1" == "--force"]; then
    rm -rf ~/.vim ~/.vimrc
    cp -rf .vim .vimrc ~/
fi
cp -rf .bashrc ./bin ~/
