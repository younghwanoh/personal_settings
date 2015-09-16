#!/usr/bin/python

from subprocess import PIPE, Popen
import epic as ep
import os
import re

source = "test.cpp"
target_list = ["origin","invert"]
tile_list = [8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096]
target_tiled_list = ["tiled","invert-tiled-in","invert-tiled-out"]
target_opt_list = [ i + "-opt" for i in target_list ]
iteration = 5
verbose = True

def evaluate():
    # modify tile size of source code
    with open(source) as rsrc:
	content = rsrc.read()
	content = re.sub(r'#define _T \d+', "#define _T %d" % num_tile, content)
    with open(source, "w") as wsrc:
	wsrc.write(content)

    # build
    make_process = Popen("make", stdout=PIPE, stderr=PIPE)
    out, err = make_process.communicate()
    print "Tile:%d - Make complete!! Start Test......" % (num_tile)

    # run and log target
    for target in all_list:
	# get average
	accum = 0.0
	for i in range(iteration):
	    target_process = Popen("./"+target, stdout=PIPE, stderr=PIPE)
	    out, err = target_process.communicate()
	    accum += float(out)

	    # if verbose -> print all iteration
	    if verbose:
		with open(target+".log", "a+") as logfile:
		    logfile.write("%s-%d: %s" % (target, num_tile, out))

	    print str(i) + "th iteration"

	accum = float(accum) / float(iteration)
	print "Average: " + str(accum)

	# write result to file
	with open(target+".log", "a+") as logfile:
	    logfile.write("%s-%d-avg: %f\n" % (target, num_tile, accum))

	print "Complete %s-%d" % (target, num_tile)

# no tile test
for all_list in [target_list, target_opt_list]:
    num_tile = 1
    evaluate()

# tile test
for all_list in [target_tiled_list]:
    for num_tile in tile_list:
        evaluate()

