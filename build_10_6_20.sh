#!/bin/bash

CMSSW_RLSE=CMSSW_10_6_20

set -e
mkdir -p build
cd build

[ -r ${CMSSW_RLSE}/src ] || scram p CMSSW ${CMSSW_RLSE}
pushd ${CMSSW_RLSE}/src
eval `scram runtime -sh`
popd

cmake .. -DROOT_DIR="$(root-config --bindir)/../cmake" -DCMSSW_RLSE=${CMSSW_RLSE}
make -j$(cat /proc/cpuinfo | grep MHz | wc -l)
