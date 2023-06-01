#!/bin/bash
# Modified by: <lyazj@github.com>

if [ $# != 3 ]; then
  >&2 echo "usage: $(basename $0) <nevent> <nthread> <seed>"
  exit 1
fi
NEVENT=$1
NTHREAD=$2
SEED=$3

set -ev
export SCRAM_ARCH=slc7_amd64_gcc700
export SCRAM_RLSE=CMSSW_10_6_28
FRAGMENT_NAME=HIG-RunIISummer20UL18wmLHEGEN-02820
FRAGMENT="$(readlink -f ${FRAGMENT_NAME})"  # absolute path
PYTHON_DIR=Configuration/GenProduction/python  # w.r.t. ${SCRAM_RLSE}/src
FRAGMENT_COPY=${PYTHON_DIR}/${FRAGMENT_NAME}_copy.py

source /cvmfs/cms.cern.ch/cmsset_default.sh
if [ -r ${SCRAM_RLSE}/src ] ; then
  echo release ${SCRAM_RLSE} already exists
else
  scram p CMSSW ${SCRAM_RLSE}
fi
pushd ${SCRAM_RLSE}/src  # ******************** $PWD changed ********************
eval `scram runtime -sh`
mkdir -p ${PYTHON_DIR}
sed s/__TO_BE_REPLACED_NEVENT/${NEVENT}/g "${FRAGMENT}" > ${FRAGMENT_COPY}
scram b -j ${NTHREAD}
popd  # ******************** $PWD restored ********************

# 0_HIG-RunIISummer20UL18wmLHEGEN-02820.sh
cmsDriver.py ${FRAGMENT_COPY} --python_filename HIG-RunIISummer20UL18wmLHEGEN-02820.py --eventcontent RAWSIM,LHE --customise Configuration/DataProcessing/Utils.addMonitoring --datatier GEN,LHE --fileout file:HIG-RunIISummer20UL18wmLHEGEN-02820.root --conditions 106X_upgrade2018_realistic_v4 --beamspot Realistic25ns13TeVEarly2018Collision --customise_commands 'process.RandomNumberGeneratorService.externalLHEProducer.initialSeed=int('${SEED}')\nprocess.source.numberEventsInLuminosityBlock=cms.untracked.uint32(100)' --step LHE,GEN --geometry DB:Extended --era Run2_2018 --mc -n ${NEVENT} --nThreads ${NTHREAD}

# 1_HIG-RunIISummer20UL18SIM-02334.sh
cmsDriver.py --python_filename HIG-RunIISummer20UL18SIM-02334.py --eventcontent RAWSIM --customise Configuration/DataProcessing/Utils.addMonitoring --datatier GEN-SIM --fileout file:HIG-RunIISummer20UL18SIM-02334.root --conditions 106X_upgrade2018_realistic_v11_L1v1 --beamspot Realistic25ns13TeVEarly2018Collision --step SIM --geometry DB:Extended --filein file:HIG-RunIISummer20UL18wmLHEGEN-02820.root --era Run2_2018 --runUnscheduled --mc -n ${NEVENT} --nThreads ${NTHREAD}

# 2_HIG-RunIISummer20UL18DIGIPremix-02315.sh
cmsDriver.py --python_filename HIG-RunIISummer20UL18DIGIPremix-02315.py --eventcontent PREMIXRAW --customise Configuration/DataProcessing/Utils.addMonitoring --datatier GEN-SIM-DIGI --fileout file:HIG-RunIISummer20UL18DIGIPremix-02315.root --pileup_input "dbs:/Neutrino_E-10_gun/RunIISummer20ULPrePremix-UL18_106X_upgrade2018_realistic_v11_L1v1-v2/PREMIX" --conditions 106X_upgrade2018_realistic_v11_L1v1 --step DIGI,DATAMIX,L1,DIGI2RAW --procModifiers premix_stage2 --geometry DB:Extended --filein file:HIG-RunIISummer20UL18SIM-02334.root --datamix PreMix --era Run2_2018 --runUnscheduled --mc -n ${NEVENT} --nThreads ${NTHREAD}

# 3_HIG-RunIISummer20UL18HLT-02334.sh
cmsDriver.py --python_filename HIG-RunIISummer20UL18HLT-02334.py --eventcontent RAWSIM --customise Configuration/DataProcessing/Utils.addMonitoring --datatier GEN-SIM-RAW --fileout file:HIG-RunIISummer20UL18HLT-02334.root --conditions 102X_upgrade2018_realistic_v15 --customise_commands 'process.source.bypassVersionCheck = cms.untracked.bool(True)' --step HLT:2018v32 --geometry DB:Extended --filein file:HIG-RunIISummer20UL18DIGIPremix-02315.root --era Run2_2018 --mc -n ${NEVENT} --nThreads ${NTHREAD}

# 4_HIG-RunIISummer20UL18RECO-02334.sh
cmsDriver.py --python_filename HIG-RunIISummer20UL18RECO-02334.py --eventcontent AODSIM --customise Configuration/DataProcessing/Utils.addMonitoring --datatier AODSIM --fileout file:HIG-RunIISummer20UL18RECO-02334.root --conditions 106X_upgrade2018_realistic_v11_L1v1 --step RAW2DIGI,L1Reco,RECO,RECOSIM,EI --geometry DB:Extended --filein file:HIG-RunIISummer20UL18HLT-02334.root --era Run2_2018 --runUnscheduled --mc -n ${NEVENT} --nThreads ${NTHREAD}

# 5_HIG-RunIISummer20UL18MiniAODv2-02334.sh
cmsDriver.py --python_filename HIG-RunIISummer20UL18MiniAODv2-02334.py --eventcontent MINIAODSIM --customise Configuration/DataProcessing/Utils.addMonitoring --datatier MINIAODSIM --fileout file:HIG-RunIISummer20UL18MiniAODv2-02334.root --conditions 106X_upgrade2018_realistic_v16_L1v1 --step PAT --procModifiers run2_miniAOD_UL --geometry DB:Extended --filein "dbs:/WplusH_HToCC_WToLNu_M-125_TuneCP5_13TeV-powheg-pythia8/RunIISummer20UL18RECO-106X_upgrade2018_realistic_v11_L1v1-v1/AODSIM" --era Run2_2018 --runUnscheduled --mc -n ${NEVENT} --nThreads ${NTHREAD}