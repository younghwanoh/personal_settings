#!/usr/bin/python

class a:
    def __init__(self):
        self.b = 1

    def printArgs(self):
        attrs = vars(self)
        print ', '.join("%s: %s" % item for item in attrs.items())

k = a()
k.printArgs()
