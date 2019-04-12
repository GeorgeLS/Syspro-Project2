#!/bin/bash
mkdir "common"
CLIENT_N=$1
for (( I = 0; I < ${CLIENT_N}; ++I )); do
    ./Client -n ${I} -c ./common -i ./inputs -m ./${I}_mirror -b 1000 -l ${I}_log.log &
done
