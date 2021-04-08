#!/bin/bash

FILES=examples/*
for f in $FILES
do
  echo "Started network $f:"
  ../src/run_flora -u Cmdenv -f $f
done
