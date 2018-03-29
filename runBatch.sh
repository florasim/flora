#!/bin/bash
cd simulations
opp_runall -j1 ../src/flora -u Cmdenv -c LoRaSim-NoADR -r 0 -n ../src:../simulations:../../inet/examples:../../inet/src:../../inet/tutorials -l ../../inet/src/INET batchTest.ini
