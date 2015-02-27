# PATH setup
export PATH=$PATH:~/bin

# pythonrc for auto-complete
export PYTHONSTARTUP=~/.pythonrc
export PYTHONPATH=$PYTHONPATH:~/workspace/ep-py

# Select SVN editor
export SVN_EDITOR=vi

# Enable Highlighting of bash shell
export CLICOLOR=1

# Homebrew setup
# ------------------------------------------------
# vi -> vim 7.4 (clipboard enabled)
alias vi='/usr/local/Cellar/vim/7.4.488/bin/vim'
alias vim='/usr/local/Cellar/vim/7.4.488/bin/vim'

# Ctags binary 5.8 homebrew version
alias ctags='/usr/local/bin/ctags'

# Custom instructions
alias lt='ls -alt'
alias l='ls'
alias grep='grep --color'
alias gl="git log --oneline"
alias portscan="/System/Library/CoreServices/Applications/Network\ Utility.app/Contents/Resources/stroke"

# A shortcut function that simplifies usage of xclip.
# - Accepts input from either stdin (pipe), or params.
# ------------------------------------------------
cb() {
  local _scs_col='\033[00;32m'; local _wrn_col='\033[01;31m'; local _trn_col='\033[00;33m';
  # Identify OS
  if [[ $OSTYPE == "linux-gnu" ]]; then
    local _clip="xclip";
  elif [[ $OSTYPE == "darwin"* ]]; then
    local _clip="pbcopy";
  fi
  # Check that xclip or pbcopy is installed.
  if ! type $_cp > /dev/null 2>&1; then
    echo -e "$_wrn_col""You must have the '""$_clip""' program installed.\e[0m"
  # Check user is not root (root doesn't have access to user xorg server)
  elif [[ "$USER" == "root" ]]; then
    echo -e "$_wrn_col""Must be regular user (not root) to copy a file to the clipboard.\e[0m"
  else
    # If no tty, data should be available on stdin
    if ! [[ "$( tty )" == /dev/* ]]; then
      input="$(< /dev/stdin)"
    # Else, fetch input from params
    else
      input="$*"
    fi
    if [ -z "$input" ]; then  # If no input, print usage message.
      echo "Copies a string to the clipboard."
      echo "Usage: cb <string>"
      echo "       echo <string> | cb"
    else
      # Copy input to clipboard
      echo -n "$input" | $_clip -selection c
      echo -n "$input" | $_clip
      # Truncate text for status
      if [ ${#input} -gt 80 ]; then input="$(echo $input | cut -c1-80)$_trn_col...\033[0m"; fi
      # Print status.
      echo -e "$_scs_col""Copied to clipboard:\033[0m $input"
    fi
  fi
}

# Aliases / functions leveraging the cb() function
# ------------------------------------------------
# Copy contents of a file
function cbf() { cat "$1" | cb; }  
# Copy SSH public key
alias cbssh="cbf ~/.ssh/id_rsa.pub"  
# Copy current working directory
alias cbwd="pwd | cb"  
# Copy most recent command in bash history
alias cbhs="cat $HISTFILE | tail -n 1 | cb"
