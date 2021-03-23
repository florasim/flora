#!/bin/bash

FILES=examples/*
for f in $FILES
do
  echo "Started network $f:"
  ../src/flora -u Cmdenv -n ../src:../simulations:../../inet4.3/examples:../../inet4.3/src:../../inet4.3/tutorials -l ../../inet4.3/src/INET -f $f &
done
