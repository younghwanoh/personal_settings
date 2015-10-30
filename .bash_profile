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

# Show segment of file
show(){
    path="$1"
    X=$2
    Y=$3
    tail -n +$X $path | head -n $((Y-X+1))
}

# Remote access 
export MY_SSH_REMOTE_HOST="yhmac"
export MY_SSH_REMOTE_STARTDIR="/Users/younghwan/"

# Change remote host to arguments
rh(){
    if [ -z "$1" ]; then
        echo "Usage: changeRemoteHost [start dir] <remote host> "
        printf "\nCurrent settings =====================\n"
        echo "MY_SSHH_REMOTE_HOST = " $MY_SSH_REMOTE_HOST
        echo "MY_SSHH_REMOTE_STARTDIR = " $MY_SSH_REMOTE_STARTDIR
        return
    else
        if [ "reset" -e "$1" ]; then
            echo "export MY_SSH_REMOTE_HOST=\"yhmac\"" > ~/.ssh/tempRemoteEnv.sh 
            echo "export MY_SSH_REMOTE_STARTDIR=\"/Users/younghwan/\"" >> ~/.ssh/tempRemoteEnv.sh 
            source ~/.ssh/tempRemoteEnv.sh
        else
            if [ -z "$2" ]; then
                dir="/"
            else
                dir="$1"
            fi
            echo "export MY_SSH_REMOTE_HOST=\"$2\"" > ~/.ssh/tempRemoteEnv.sh 
            echo "export MY_SSH_REMOTE_STARTDIR=\"$dir\"" >> ~/.ssh/tempRemoteEnv.sh 
            source ~/.ssh/tempRemoteEnv.sh

            printf "\nUpdated settings =====================\n"
            echo "MY_SSHH_REMOTE_HOST = " $MY_SSH_REMOTE_HOST
            echo "MY_SSHH_REMOTE_STARTDIR = " $MY_SSH_REMOTE_STARTDIR
        fi
    fi
}

# Change remote host to arguments
rls(){
    ssh $MY_SSH_REMOTE_HOST ls -l $MY_SSH_REMOTE_STARTDIR"$1"
}
