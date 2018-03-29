#!/bin/bash
cd simulations

FILES=networks/*
for f in $FILES
do
  echo "Started network $f:"
  ../src/flora -u Cmdenv -n ../src:../simulations:../../inet/examples:../../inet/src:../../inet/tutorials -l ../../inet/src/INET -f loraNetwork_skeleton.ini -f $f
done
