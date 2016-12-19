#!/bin/bash

. script/color.sh

checkAndCopy(){
  if [ -e $3 ]; then
    printf '\e['1';'32'm%s\e[m' "$1"
    printf ": file conflict detected !\n"
    read -p "Do you want to overwrite it ? [y/n]" ans
    case $ans in
      [Yy]* ) printf '\e['1';'31'm%-6s\e[m\n\n' "Overwritten done !"; rm -rf $3;;
      [Nn]* ) printf '\e['1';'33'm%-6s\e[m %s\n\n' " Skip" "$3"; return;;
		* ) color "%s\n\n" "Please answer yes or no." "r"; exit;;
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
  sudo apt-get install subversion vim-gtk python-numpy python-matplotlib xclip libgnome2-bin g++ openssh-server
fi 

# JDK
if [ "$(uname -s)" == "Linux" ] && [ "$1" == "jdk" ]; then
  sudo add-apt-repository ppa:webupd8team/java
  sudo apt-get update
  sudo apt-get install oracle-java8-installer
fi

# Up-to-date Git
if [ "$(uname -s)" == "Linux" ] && [ "$1" == "git" ]; then
	color "Up-to-date git version will be downloaded from ppa:git-core (16, Sep. 2.9.3)" "g"
	read -p "Do you want to proceed ? [y/n]" ans
	case $ans in
		[Yy]* )	sudo add-apt-repository ppa:git-core/ppa -y;
						sudo apt-get update;
				  	sudo apt-get install git -y;;
		[Nn]* ) ;;
		* ) color "%s\n\n" "Please answer yes or no." "r"; exit;;
	esac
	color "%s\n" "`git --version`" "y"
fi

# CUDA
if [ "$(uname -s)" == "Linux" ] && [ "$1" == "cuda" ]; then
  lspci -vnn | grep VGA -A 12
  color "\n%s\n" "Here is the installation process !!" "g"
  color "%s\n" "- Install NVIDIA Graphic driver" "y"
  echo "\
  1) Download matched driver manually
  2) sudo service lightdm stop
  3) Ctrl + Alt + F1 (CLI mode - virtual terminal?)
  4) Install NVIDIA driver with sudo
  5) add /usr/local/cuda/lib64:/usr/local/cuda/lib64/stubs to LD_LIBRARY_PATH"

  color "\n%s\n" "- Install CUDA driver" "y"
  echo "\
  1) Download CUDA SDK
  2) Install SDK only"

  color "\n%s\n" "- Caution" "y"
  echo "\
  1) Do not install driver via nvidia-current
  2) Do not install driver via CUDA SDK
  "
fi

# Jedi-vim, python code hint
if [ "$(uname -s)" == "Linux" ] && [ "$1" == "vim-bundle" ]; then
  git clone https://github.com/gmarik/vundle.git ~/.vim/bundle/vundle
  ex -sc "1i|syntax on
set nocompatible
filetype off

set rtp+=~/.vim/bundle/vundle/
call vundle#rc()
\" let Vundle manage Vundle
\" \" required!
Bundle 'gmarik/vundle'
\" \" My bundles here:
Bundle 'davidhalter/jedi-vim'

filetype plugin indent on     \" required!

" -cx ~/.vimrc

  vi -c BundleInstall
fi

# Install custom settings
if [ "$(uname -s)" == "Darwin" ] && [ "$1" == "init" ]; then
  echo "Found OS X. Copy custom settings"

elif [ "$(uname -s)" == "Linux" ] && [ "$1" == "init" ]; then
  echo "Found Linux. Copy custom settings"
  if [ -z $HOME ]; then
    echo "Error: Set your home directory path to \$HOME first!"
    exit
  else
    printf 'Your home directory is\e['1';'32'm %s \e[m\n' "$HOME"
  fi

  # DSA key generation
  if [ ! -e ~/.ssh/id_dsa ]; then
    ssh-keygen -t dsa
    eval "$(ssh-agent -s)"
    ssh-add ~/.ssh/id_dsa
  else
    color "Skip DSA key generation due to conflict" "b"
  fi

  # RSA key generation
  if [ ! -e ~/.ssh/id_rsa ]; then
    ssh-keygen -t rsa -b 4096 -C "garion9013@gmail.com"
    eval "$(ssh-agent -s)"
    ssh-add ~/.ssh/id_rsa
  else
    color "Skip RSA key generation due to conflict" "b"
  fi
  mkdir ~/.subversion 2> /dev/null
  mkdir ~/.local 2> /dev/null
  mkdir ~/.ssh 2> /dev/null
	mkdir ~/workspace 2> /dev/null
	mkdir ~/Packages 2> /dev/null

  checkAndCopy "tmux config" ".tmux.conf" "$HOME/.tmux.conf"
  checkAndCopy "ssh config" ".ssh/config" "$HOME/.ssh/config"
  checkAndCopy "svn config" ".subversion/config" "$HOME/.subversion/config"
  checkAndCopy "bin" ".local/bin" "$HOME/.local/bin"
  checkAndCopy "mylib" ".local/mylib" "$HOME/.local/mylib"
  checkAndCopy "bash" ".bashrc" "$HOME/.bashrc"
  checkAndCopy "bash_profile" ".bash_profile" "$HOME/.bash_profile"
  checkAndCopy "vimrc" ".vimrc" "$HOME/.vimrc"
  checkAndCopy "vim" ".vim" "$HOME/.vim"
  checkAndCopy "lib color" "script/color.sh" "$HOME/.local/bin/color.sh"
  checkAndCopy "pythonrc" ".pythonrc" "$HOME/.pythonrc"

	# SSH home variable settings
	sudo echo "SendEnv SSH_HOME" >> ~/.ssh/config
	if [ -f /etc/ssh/sshd_config ]; then
		sudo echo "AcceptEnv SSH_HOME" >> /etc/ssh/sshd_config
		sudo service ssh restart
	else
		color "%s variable cannot be set, install openssh-server.\n" "SSH_HOME" "b"
		color "Then, add %s to " "'AcceptEnv SSH_HOME'" "r"
		color "/etc/ssh/sshd_config" "y"
		exit
	fi

  source ~/.bashrc
fi

if [ -z "$1" ]; then
  echo "Usage: install.sh package"
  echo "       install.sh init"
  echo "       install.sh git"
  echo "       install.sh vim-bundle (jedi-vim: Python code hint, after init)"
  echo "       install.sh cuda"
  echo "       install.sh jdk"
fi
