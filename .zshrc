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
myip(){
  failed=false
  target=('ifconfig.me' 'icanhazip.com' 'ipecho.net/plain' 'ifconfig.co' 'myip.dnsomatic.com')
  for i in "${target[@]}"; do
    export SSH_HOME=younghwan@`curl -s ${i}`
    fail_check="html"
    if test "${SSH_HOME#*$fail_check}" != "$SSH_HOME"; then
      if [ -z "$1" ]; then
        echo "Failed: ${SSH_HOME}, ${i}"
        failed=true
      fi
    else
      if [ -z "$1" ]; then
        echo "Success: ${SSH_HOME}, ${i}"
      fi
      break
    fi
  done

  if $failed; then
    echo "------"
    echo "All of IP identification targets are failed"
  fi
  
  if [ -z "$1" ]; then
      cb $SSH_HOME
  fi
}
myip "initial"

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
bindkey  "^[[F"   end-of-line # end
bindkey  "^[[3~"  delete-char # delete

# . ~/.local/anaconda2/etc/profile.d/conda.sh  # commented out by conda initialize
# conda activate base  # commented out by conda initialize

rcp(){
  file=${1##*/}
  echo "Copying $file from server alias ..."
  ssh -t server "scp -r $1 ."
  scp -r server:~/$file $2
}

