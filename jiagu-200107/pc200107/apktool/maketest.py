__author__ = 'zhr'

import PCflow
import os
from getopt import getopt
import sys

f = open('config.bin', 'r+')
config = f.read().splitlines()
config[0]='0'
print(config)
opts, argv = getopt(sys.argv[1:], 'h', longopts=['max_start=', 'dex=', 'so_encrypt=', 'debug=', 'data='])
for op, value in opts:
    if op == '--max_start':
        config[1] = value
    elif op == '--dex':
        config[2] = value
    elif op == '--so_encrypt':
        config[3] = value
    elif op == '--debug':
        config[4] = value
    elif op == '--data':
        config[5] = value
    else:
        raise RuntimeError('error cmd...')
f.seek(0)
print(config)
f.write('\n'.join(config))
f.flush()
f.close()


foldername = r"test"

for filename in os.listdir(foldername):
    print(filename)
    apkpath = 'test/' + filename
    PCflow.pcworkflow(apkpath)

# apkpath = "test.apk"
#
# PCflow.pcworkflow(apkpath)