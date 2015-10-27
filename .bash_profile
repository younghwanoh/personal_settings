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

# Aliases / functions leveraging the cb() function
# ------------------------------------------------
# Copy SSH public key
alias cbssh="cbf ~/.ssh/id_rsa.pub"  
# Copy current working directory
alias cbwd="pwd | cb"  
# Copy most recent command in bash history
alias cbhs="cat $HISTFILE | tail -n 1 | cb"
