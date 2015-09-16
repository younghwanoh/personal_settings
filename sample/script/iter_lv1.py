#!/usr/bin/python

import os
import re

command = ""
# command = "rm *.txt *.csv *.log _cuo* *.icnt *.xml *.exe *.config"

# Level 1 dir/file iterator

fileList = os.listdir(".")
for elem in fileList:
    if os.path.isdir(elem): # iterate over directories
        os.chdir(elem)

        # Denote command you'd like to run in directory of Level 1
        os.system(command)

        os.chdir("../")
        subFileList = os.listdir(elem)

        for file in subFileList:
            if os.path.isfile(elem + "/" + file): # iterate over all files in subdir
                # file denotes each file in directory of Level 1
                print file
