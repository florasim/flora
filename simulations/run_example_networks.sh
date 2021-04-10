#!/bin/bash

FILES=examples/*
for f in $FILES
do
  echo "Started network $f:"
  $(dirname $0)/../src/run_flora -u Cmdenv -f $f
done