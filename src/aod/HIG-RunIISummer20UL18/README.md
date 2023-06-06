# Hss sample generating I: MiniAOD

Directory in our git-repo: https://github.com/lyazj/hss/tree/main/src/aod/HIG-RunIISummer20UL18

Official routines for $H \to cc$ process to be adjusted for our $H \to ss$ use:

* [mcm retrieval result](https://cms-pdmv.cern.ch/mcm/requests?prepid=*20UL18*&dataset_name=WplusH_HToCC_WToLNu_*&page=0&shown=127) of our interest, which is restricted that:
  * `prepid=20UL18 (latest)`
  * `dataset_name=WplusH_HToCC_WToLNu_*`

* choose any of the 7 chained entries to reach to the chain: [HIG-RunIISummer20UL18DIGIPremix-02315](https://cms-pdmv.cern.ch/mcm/chained_requests?contains=HIG-RunIISummer20UL18DIGIPremix-02315)
* dataset: [WplusH_HToCC_WToLNu_M-125_TuneCP5_13TeV-powheg-pythia8](https://cms-pdmv.cern.ch/mcm/requests?dataset_name=WplusH_HToCC_WToLNu_M-125_TuneCP5_13TeV-powheg-pythia8) (contains also `20UL16` and `20UL17` which are considered to be obsolete)

Fetched official test scripts illustrating 7 key procedures ([auto-fetching script](https://github.com/lyazj/hss/blob/main/src/aod/HIG-RunIISummer20UL18/origin/fetch-scripts)):

```
0_HIG-RunIISummer20UL18wmLHEGEN-02820.sh
1_HIG-RunIISummer20UL18SIM-02334.sh
2_HIG-RunIISummer20UL18DIGIPremix-02315.sh
3_HIG-RunIISummer20UL18HLT-02334.sh
4_HIG-RunIISummer20UL18RECO-02334.sh
5_HIG-RunIISummer20UL18MiniAODv2-02334.sh
6_HIG-RunIISummer20UL18NanoAODv9-03236.sh
```

**NOTE: For our purpose, NanoAOD running is suspended, waiting for the future Hss tagger.**

For condor batch system, a single executable is expected.

So Combining the first 6 scripts together, we got:

```bash
#!/bin/bash
# Modified by: <lyazj@github.com>

if [ $# != 4 ] && [ $# != 5 ]; then
    >&2 echo "usage: $(basename $0) <nevent> <nthread> <seed-begin> <seed-offset> [<x509up>]"
    exit 1
fi
NEVENT=$1
NTHREAD=$2
SEED=$[$3 + $4]
if [ ! -z "$5" ]; then
    export X509_USER_PROXY="$5"
fi

set -ev
FRAGMENT_NAME=HIG-RunIISummer20UL18wmLHEGEN-02820
FRAGMENT="$(readlink -f ${FRAGMENT_NAME})"  # absolute path

voms-proxy-info  # early stop on proxy error
source /cvmfs/cms.cern.ch/cmsset_default.sh

# setup scram environment with fragment installed
scram_setup() {
    # scram configuration
    # NOTE: external environment variable ${SCRAM_ARCH} is required to be set
    if [ $# != 2 ]; then
        >&2 echo "usage: $(basename $0) <arch> <rlse>"
        exit 1
    fi
    export SCRAM_ARCH=$1
    export SCRAM_RLSE=$2

    # fragment configuration
    # NOTE: the procedure sets global environment variable ${FRAGMENT_COPY}
    local PYTHON_DIR=Configuration/GenProduction/python  # w.r.t. ${SCRAM_RLSE}/src
    FRAGMENT_COPY=${PYTHON_DIR}/${FRAGMENT_NAME}_copy.py

    # scram creation
    if [ -r ${SCRAM_RLSE}/src ] ; then
        echo release ${SCRAM_RLSE} already exists
    else
        scram p CMSSW ${SCRAM_RLSE}
    fi

    # scram initialization
    # NOTE: copy fragment in before building scram
    pushd ${SCRAM_RLSE}/src  # ******************** $PWD changed ********************
    eval `scram runtime -sh`
    mkdir -p ${PYTHON_DIR}
    sed s/__TO_BE_REPLACED_NEVENT/${NEVENT}/g "${FRAGMENT}" > ${FRAGMENT_COPY}
    scram b -j ${NTHREAD}
    popd  # ******************** $PWD restored ********************
}

# 0_HIG-RunIISummer20UL18wmLHEGEN-02820.sh
scram_setup slc7_amd64_gcc700 CMSSW_10_6_28
cmsDriver.py ${FRAGMENT_COPY} --python_filename HIG-RunIISummer20UL18wmLHEGEN-02820.py --eventcontent RAWSIM,LHE --customise Configuration/DataProcessing/Utils.addMonitoring --datatier GEN,LHE --fileout file:HIG-RunIISummer20UL18wmLHEGEN-02820.root --conditions 106X_upgrade2018_realistic_v4 --beamspot Realistic25ns13TeVEarly2018Collision --customise_commands 'process.RandomNumberGeneratorService.externalLHEProducer.initialSeed=int('${SEED}')\nprocess.source.numberEventsInLuminosityBlock=cms.untracked.uint32(100)' --step LHE,GEN --geometry DB:Extended --era Run2_2018 --mc -n ${NEVENT} --nThreads ${NTHREAD}

# 1_HIG-RunIISummer20UL18SIM-02334.sh
scram_setup slc7_amd64_gcc700 CMSSW_10_6_17_patch1
cmsDriver.py --python_filename HIG-RunIISummer20UL18SIM-02334.py --eventcontent RAWSIM --customise Configuration/DataProcessing/Utils.addMonitoring --datatier GEN-SIM --fileout file:HIG-RunIISummer20UL18SIM-02334.root --conditions 106X_upgrade2018_realistic_v11_L1v1 --beamspot Realistic25ns13TeVEarly2018Collision --step SIM --geometry DB:Extended --filein file:HIG-RunIISummer20UL18wmLHEGEN-02820.root --era Run2_2018 --runUnscheduled --mc -n ${NEVENT} --nThreads ${NTHREAD}

# 2_HIG-RunIISummer20UL18DIGIPremix-02315.sh
scram_setup slc7_amd64_gcc700 CMSSW_10_6_17_patch1
cmsDriver.py --python_filename HIG-RunIISummer20UL18DIGIPremix-02315.py --eventcontent PREMIXRAW --customise Configuration/DataProcessing/Utils.addMonitoring --datatier GEN-SIM-DIGI --fileout file:HIG-RunIISummer20UL18DIGIPremix-02315.root --pileup_input "dbs:/Neutrino_E-10_gun/RunIISummer20ULPrePremix-UL18_106X_upgrade2018_realistic_v11_L1v1-v2/PREMIX" --conditions 106X_upgrade2018_realistic_v11_L1v1 --step DIGI,DATAMIX,L1,DIGI2RAW --procModifiers premix_stage2 --geometry DB:Extended --filein file:HIG-RunIISummer20UL18SIM-02334.root --datamix PreMix --era Run2_2018 --runUnscheduled --mc -n ${NEVENT} --nThreads ${NTHREAD}

# 3_HIG-RunIISummer20UL18HLT-02334.sh
scram_setup slc7_amd64_gcc700 CMSSW_10_2_16_UL
cmsDriver.py --python_filename HIG-RunIISummer20UL18HLT-02334.py --eventcontent RAWSIM --customise Configuration/DataProcessing/Utils.addMonitoring --datatier GEN-SIM-RAW --fileout file:HIG-RunIISummer20UL18HLT-02334.root --conditions 102X_upgrade2018_realistic_v15 --customise_commands 'process.source.bypassVersionCheck = cms.untracked.bool(True)' --step HLT:2018v32 --geometry DB:Extended --filein file:HIG-RunIISummer20UL18DIGIPremix-02315.root --era Run2_2018 --mc -n ${NEVENT} --nThreads ${NTHREAD}

# 4_HIG-RunIISummer20UL18RECO-02334.sh
scram_setup slc7_amd64_gcc700 CMSSW_10_6_17_patch1
cmsDriver.py --python_filename HIG-RunIISummer20UL18RECO-02334.py --eventcontent AODSIM --customise Configuration/DataProcessing/Utils.addMonitoring --datatier AODSIM --fileout file:HIG-RunIISummer20UL18RECO-02334.root --conditions 106X_upgrade2018_realistic_v11_L1v1 --step RAW2DIGI,L1Reco,RECO,RECOSIM,EI --geometry DB:Extended --filein file:HIG-RunIISummer20UL18HLT-02334.root --era Run2_2018 --runUnscheduled --mc -n ${NEVENT} --nThreads ${NTHREAD}

# 5_HIG-RunIISummer20UL18MiniAODv2-02334.sh
scram_setup slc7_amd64_gcc700 CMSSW_10_6_20
cmsDriver.py --python_filename HIG-RunIISummer20UL18MiniAODv2-02334.py --eventcontent MINIAODSIM --customise Configuration/DataProcessing/Utils.addMonitoring --datatier MINIAODSIM --fileout file:HIG-RunIISummer20UL18MiniAODv2-02334.root --conditions 106X_upgrade2018_realistic_v16_L1v1 --step PAT --procModifiers run2_miniAOD_UL --geometry DB:Extended --filein file:HIG-RunIISummer20UL18RECO-02334.root --era Run2_2018 --runUnscheduled --mc -n ${NEVENT} --nThreads ${NTHREAD}
```

Remarks (most of which come from my painful experience):

* The 6 procedures use different CMSSW releases in similar initialization routines, which advocates coding of a shell function to handle this minor difference. In my experience, astonishingly, **a wrong third version number crashes the program.**
* **Python fragment must be installed (i.e. copied) to the target directory before `scram b`.**
* **A valid x509 user proxy is required since procedure #2** and must not be omitted even with pre-generated Python configure file provided (which lists the pile-up resources, etc., but gives empty results without an x509up).
* [highly error-prone and very hard to debug] The x509 user proxy file stored in `/tmp` is by definition transient, and will not be seen on remote working hosts, so it is not a good idea to use the default path. To be compatible to CMS Connect, [using x509 proxy and shipping it with the job](https://batchdocs.web.cern.ch/tutorial/exercise2e_proxy.html#using-x509-proxy-and-shipping-it-with-the-job) is recommended. Also, according to [relevant condor documentation](https://htcondor.readthedocs.io/en/latest/man-pages/condor_submit.html), `x509userproxy` should be set to the user-specified x509 path, in addition to including the path in `transfer_input_files`.
* Generating and downloading a certification directly from [CERN](https://ca.cern.ch/ca/user/Request.aspx) is much faster than requesting one from https://cagrid.ihep.ac.cn/. Note also that [a VOMS membership request](https://voms24.cern.ch:8443/voms/cms/user/home.action) is required before one can activating a CMS x509 user proxy, and only try visiting this page after the p12 certificate got from CERN or IHEP is properly installed to the browser. For me, firefox rejects p12 certificates with empty secret string, so google chrome is used instead.

Then the JDL file is easy to write:

```
Universe = vanilla
Executable = HIG-RunIISummer20UL18MiniAODv2.sh

+ProjectName="cms.org.cern"

NJOB = 1000
NEVENT = 100
NTHREAD = 4
SEED_BEGIN = 6001
SEED_OFFSET = $(Process)
X509UP = x509up
Arguments = $(NEVENT) $(NTHREAD) $(SEED_BEGIN) $(SEED_OFFSET) $(X509UP)

requirements = (OpSysAndVer =?= "CentOS7")
request_cpus = 4
request_memory = 4096
x509userproxy = $(X509UP)

+JobFlavour = "tomorrow"

Log    = log/HIG-RunIISummer20UL18MiniAODv2_$(Cluster)_system.log
Output = log/HIG-RunIISummer20UL18MiniAODv2_$(Cluster)_$(SEED_BEGIN)_$(SEED_OFFSET)_1.log
Error  = log/HIG-RunIISummer20UL18MiniAODv2_$(Cluster)_$(SEED_BEGIN)_$(SEED_OFFSET)_2.log

should_transfer_files   = YES
when_to_transfer_output = ON_EXIT_OR_EVICT
transfer_input_files    = $(X509UP),HIG-RunIISummer20UL18wmLHEGEN-02820
transfer_output_files   = HIG-RunIISummer20UL18MiniAODv2-02334.root
transfer_output_remaps  = "HIG-RunIISummer20UL18MiniAODv2-02334.root = output/HIG-RunIISummer20UL18MiniAODv2-02334_$(Cluster)_$(SEED_BEGIN)_$(SEED_OFFSET).root"

Queue $(NJOB)
```
