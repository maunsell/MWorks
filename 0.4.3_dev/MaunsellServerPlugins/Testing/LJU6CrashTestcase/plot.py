#!/usr/bin/env python

from pylab import *
import numpy
import time
import os

figure()

while True: 
    os.system("grep Mean /tmp/testcase.log | awk '{print $3}' > /tmp/testcase.nums")

    els = numpy.fromfile('/tmp/testcase.nums', sep="\n");
    clf()
    plot(els)
    xlabel('time - 10 second intervals')
    ylabel('elapsed command-response (us)')
    draw()

    savefig('/tmp/plot-fig.png', format='png')

    time.sleep(5)
