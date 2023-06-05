#!/bin/bash

export CMSSW_RLSE=CMSSW_10_6_20

mkdir -p build
pushd build >/dev/null

source /cvmfs/cms.cern.ch/cmsset_default.sh

[ -r ${CMSSW_RLSE}/src ] || scram p CMSSW ${CMSSW_RLSE}
pushd ${CMSSW_RLSE}/src >/dev/null
eval `scram runtime -sh`
popd >/dev/null

popd >/dev/null
