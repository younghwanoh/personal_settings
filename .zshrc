#!/bin/zsh
# -----------------------------------------------------------------------------------
# Internally used env
# -----------------------------------------------------------------------------------

export PATH=~/.local/bin:$PATH

# pythonrc for auto-complete
export PYTHONSTARTUP=~/.pythonrc
export PYTHONPATH=$PYTHONPATH:~/workspace/ep-py:~/.local/mylib

# Select SVN editor
export SVN_EDITOR=vi

# Enable Highlighting of bash shell
export CLICOLOR=1

# Copy current working directory
alias cbwd="pwd | cb"  

# Initial assignment for SSH_HOME
export SSH_HOME=younghwan@`ipconfig getifaddr en0`
myip(){
  ip=younghwan@`ipconfig getifaddr en0`
  cb $ip
}

# Custom instructions
alias lt='ls -alt'
alias l='ls'
alias grep='grep --color'
alias gl="git log --oneline"
alias gls="git ls-tree -r master --name-only"
alias svnup='svn up --depth empty'
alias portscan="/System/Library/CoreServices/Applications/Network\ Utility.app/Contents/Resources/stroke"

# SSH-related command to linux server
alias new="tmux-command new"
alias attach="tmux-command attach"
alias tls="tmux-command ls"

# Zsh-specific
# Try "showkey -a" and press any key for keycodes
bindkey  "^[[H"   beginning-of-line # home
bindkey  "^[[F"   end-of-line
bindkey  "^[[3~"  delete-char

# . ~/.local/anaconda2/etc/profile.d/conda.sh  # commented out by conda initialize
# conda activate base  # commented out by conda initialize

rcp(){
  file=${1##*/}
  echo "Copying $file from server alias ..."
  ssh -t server "scp -r $1 ."
  scp -r server:~/$file $2
}

