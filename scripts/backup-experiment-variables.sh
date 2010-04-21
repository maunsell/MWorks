#!/bin/sh

DATESTR=`date +%y%m%d`

OUTDIR="/Library/MonkeyWorks/Experiment-backups/$DATESTR"

mkdir -p $OUTDIR
rsync -av --exclude=*.wav --exclude=*.png /Library/MonkeyWorks/Experiments/* $OUTDIR


