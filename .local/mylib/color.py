#!/usr/bin/python

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

if __name__=="__main__":
    print(bcolors.HEADER+"color test1"+bcolors.ENDC)
    print(bcolors.OKBLUE+"color test2"+bcolors.ENDC)
    print(bcolors.OKGREEN+"color test3"+bcolors.ENDC)
    print(bcolors.WARNING+"color test4"+bcolors.ENDC)
    print(bcolors.FAIL+"color test5"+bcolors.ENDC)
    print(bcolors.BOLD+"color test6"+bcolors.ENDC)
    print(bcolors.UNDERLINE+"color test7"+bcolors.ENDC)
