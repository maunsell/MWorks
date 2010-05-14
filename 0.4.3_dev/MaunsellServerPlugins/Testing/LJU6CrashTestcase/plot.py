#!/usr/bin/env python

from pylab import *
import numpy
import time
import os

figure()

while True: 
    os.system("grep Mean /tmp/testcase2.log | awk '{print $3}' > /tmp/testcase.nums")

    els = numpy.fromfile('/tmp/testcase.nums', sep="\n");
    clf()
    timevals = transpose(range(len(els)))
    plot(timevals / 6.0, els/1000) # 10s intervals
    xlabel('time - min')
    ylabel('elapsed command-response (ms)')
    draw()

    savefig('/tmp/plot-fig.png', format='png')

    time.sleep(5)
