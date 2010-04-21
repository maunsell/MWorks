#!/bin/sh

#kill  X first
echo Killing X processes
ps aux | grep /X | awk '{print $2'} | xargs -t kill

echo Removing X locks in /tmp
rm -rf /tmp/.X?-lock

echo Killing matlab processes
ps aux | grep -i matlab 
ps aux | grep -i matlab | awk '{print $2}' | xargs -t kill
