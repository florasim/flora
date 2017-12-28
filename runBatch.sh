#!/bin/bash

opp_runall -j1 ./flora -u Cmdenv -c LoRaSim-NoADR -r 0 -n src:simulations:../inet/examples:../inet/src:../inet/tutorials -l ../inet/src/INET simulations/batchTest.ini
