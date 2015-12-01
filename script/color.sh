#!/bin/bash

# Associative array works on bash version > 4
# Just retain this for a reference usage of associative array.
declare -A colorTable 2> /dev/null
colorTable=( ["red"]='\\e[0;31m' ["green"]='\\e[0;32m' ["yellow"]='\\e[0;33m' ["blue"]='\\e[0;34m' ["orange"]='\\e[0;35m' ["purple"]='\\e[0;36m' ["reset"]='\\e[0m')

color(){
  format="$1"
  text="$2"
  color="$3"
  reset='\\e[0m'

  case $color in
    [Rr]* ) index="red";    key='\\e[1;31m';;
    [Gg]* ) index="green";  key='\\e[1;32m';;
    [Yy]* ) index="yellow"; key='\\e[1;33m';;
    [Bb]* ) index="blue";   key='\\e[1;34m';;
    [Oo]* ) index="orange"; key='\\e[1;35m';;
    [Pp]* ) index="purple"; key='\\e[1;36m';;
  esac

  # Deprecate associative array for backward compatibility (bash < 3)
  # format=$(echo $format | sed -e "s/\(%-[0-9]*s\|%s\)/"${colorTable[$index]}"\1"${colorTable["reset"]}"/g")
  format=$(echo "$format" | sed -E "s/(%-[0-9]*s|%s)/"$key"\1"$reset"/g")
  printf "$format" "$text"
}

main(){
  color "This is red text: %-10s with format" "red" "r"
  color ", and %s with format\n" "green" "g"
  color "Green test      : %-10s after 10 words\n" "test" "g"
  color "Purple test     : %-10s\n" "test" "p"
  color "Yellow test     : %-10s\n" "test" "y"
  color "Orange test     : %-10s\n" "test" "o"
  color "Blue test       : %-10s\n" "test" "b"
}

if [[ "$BASH_SOURCE" == "$0" ]]; then
  main "$@"
fi