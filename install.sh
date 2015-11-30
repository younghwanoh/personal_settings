#!/bin/bash

checkAndCopy(){
  if [ -e $3 ]; then
    printf '\e['1';'32'm%s\e[m' $1
    printf ": file conflict detected !\n"
    read -p "Do you want to overwrite it ? [y/n]" ans
    case $ans in
      [Yy]* ) printf '\e['1';'31'm%-6s\e[m\n\n' "Overwritten done !"; rm -rf $3;;
      [Nn]* ) printf '\e['1';'33'm%-6s\e[m %s\n\n' " Skip" "$3"; return;;
      * ) echo "Please answer yes or no.";;
    esac
  else
    printf '\e['1';'32'm%-50s %s\e[m\n' "$3" "Success !"
  fi
  cp -rf "$2" "$3"
}


# Install packages
if [ "$(uname -s)" == "Darwin" ] && [ "$1" == "package" ]; then
  echo "Install initial dependencies (OSX - brew)"
  brew install vim --with-lua
elif [ "$(uname -s)" == "Linux" ] && [ "$1" == "package" ]; then
  echo "Install initial dependencies (Linux - apt)"
  sudo apt-get update
	sudo apt-get install subversion openssh-server vim-gtk python-numpy xclip libgnome2-bin
fi 

# Install custom settings
if [ "$(uname -s)" == "Darwin" ] && [ "$1" == "init" ]; then
  echo "Found OS X. Copy custom settings"

elif [ "$(uname -s)" == "Linux" ] && [ "$1" == "init" ]; then
  echo "Found Linux. Copy custom settings"
  if [ -z $HOME ]; then
    echo "Set your home directory path to \$HOME"
  else
    printf 'Your home directory is\e['1';'32'm %s \e[m\n' "$HOME"
  fi
  if [ ! -e "$HOME/.ssh/id_dsa" ]; then
    ssh-keygen -t dsa
    eval "$(ssh-agent -s)"
    ssh-add ~/.ssh/id_dsa
  fi
  if [ ! -e "$HOME/.ssh/id_rsa" ]; then
    ssh-keygen -t rsa -b 4096 -C "garion9013@gmail.com"
    eval "$(ssh-agent -s)"
    ssh-add ~/.ssh/id_rsa
  fi
  mkdir ~/.subversion 2> /dev/null

  checkAndCopy "ssh config" ".ssh/config" "$HOME/.ssh/config"
  checkAndCopy "svn config" ".subversion/config" "$HOME/.subversion/config"
  checkAndCopy "bin" "bin" "$HOME/bin"
  checkAndCopy "bash" ".bashrc" "$HOME/.bashrc"
  checkAndCopy "bash_profile" ".bash_profile" "$HOME/.bash_profile"
  checkAndCopy "vimrc" ".vimrc" "$HOME/.vimrc"
  checkAndCopy "vim" ".vim" "$HOME/.vim"

  source ~/.bashrc
fi
