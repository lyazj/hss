#!/bin/bash

set -e

source env_10_6_20.sh
cd build
cmake .. -DROOT_DIR="$(root-config --bindir)/../cmake" -DCMSSW_RLSE=${CMSSW_RLSE}
make -j$(cat /proc/cpuinfo | grep MHz | wc -l)
