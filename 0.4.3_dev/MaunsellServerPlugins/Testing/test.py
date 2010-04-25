#!/usr/bin/python2.5

import u6
import time

d = u6.U6()

for iR in range(0,100):
    for iP in range(0,8):
        d.getFeedback(u6.BitStateWrite(iP,0))

    time.sleep(1)

    for iP in range(0,8):
        d.getFeedback(u6.BitStateWrite(iP,1))

    time.sleep(1)



    
