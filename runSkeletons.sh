#!/bin/bash

FILES=simulations/networks/*
for f in $FILES
do
  echo "Started network $f:"
  ./flora -u Cmdenv -n src:simulations:../inet/examples:../inet/src:../inet/tutorials -l ../inet/src/INET -f simulations/loraNetwork_skeleton.ini -f $f
done
