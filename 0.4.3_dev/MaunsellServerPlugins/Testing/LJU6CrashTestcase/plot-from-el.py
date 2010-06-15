#!/usr/bin/env python

from pylab import *
import numpy as np
import time
import os

figure()

while True: 
    # get data
    regexp = r"Mean *([\d\.]+) *max *([\d\.]+) *totalElapsedUs ([-\d]+)"

    els = np.fromregex('/tmp/testcase2.log', regexp,
                       [('meanUs', np.float64), ('maxUs', np.float64), ('totalUs', np.int64)])

    # fix display overflow
    totalUs = []
    lastEl = 0
    addUs = 0
    addStep = 4294211800
    for tEl in els['totalUs']: #range(0,len(els['totalUs'])):
        if (tEl - lastEl) < 0:
            addUs = addUs+addStep
        totalUs.append(tEl + addUs)
        lastEl = tEl
    totalUs = np.array(totalUs, np.float64)

    clf()

    #semilogx
    pH1 = plot(totalUs / 1000 / 1000 / 60 / 60, els['maxUs']/1000) # 10s intervals
    pH2 = plot(totalUs / 1000 / 1000 / 60 / 60, els['meanUs']/1000, 'r') # 10s intervals
    xlabel('time (hr)')
    ylabel('elapsed command-response (ms)')
    legH = legend((pH1,pH2), ('worst-case', 'mean'))
    draw()

    savefig('/tmp/plot-fig.png', format='png')

    #import pdb; pdb.set_trace()

    #time.sleep(5)
    canvas = gcf().canvas
    canvas.start_event_loop(timeout=10)


