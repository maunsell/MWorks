#!/bin/sh

mkdir $1
buildbot create-slave $1 dicarlo3.mit.edu:9990 i386-OSX-10.5 labuser
echo "Ben <bkennedy@mit.edu>" > ${1}/info/admin
echo "dicarlo10.mit.edu - 1.5 GHz Core Solo MacMini - OSX 10.5" > ${1}/info/host
buildbot start ${1}

