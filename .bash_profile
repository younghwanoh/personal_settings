export REMOTE_LOGIN=1
export REMOTE_HOST=yhmac

alias cb=rcb
alias cbf=rcbf
alias open=ropen

# Load bashrc at ssh login
if [ -f ~/.bashrc ]; then
  . ~/.bashrc
fi