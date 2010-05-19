#!/usr/bin/env python

from pylab import *
import numpy
import time
import os

figure()

while True: 
    os.system("grep Mean /tmp/testcase2.log | awk '{print $5}' > /tmp/testcase.nums")

    els = numpy.fromfile('/tmp/testcase.nums', sep="\n");
    clf()
    timevals = transpose(range(len(els)))
    semilogy(timevals / 6.0, els/1000) # 10s intervals
    xlabel('time - min')
    ylabel('elapsed command-response (ms)')
    title('worst-case latency over 1000 periods')
    draw()

    savefig('/tmp/plot-fig.png', format='png')

    #time.sleep(5)
    canvas = gcf().canvas
    canvas.start_event_loop(timeout=10)
