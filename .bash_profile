export REMOTE_LOGIN=1

alias cb=rcb
alias cbf=rcbf

# Load bashrc at ssh login
if [ -f ~/.bashrc ]; then
  . ~/.bashrc
fi