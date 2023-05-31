#!/bin/bash

# GEN Script begin
rm -f request_fragment_check.py
wget -q https://raw.githubusercontent.com/cms-sw/genproductions/master/bin/utils/request_fragment_check.py
chmod +x request_fragment_check.py
./request_fragment_check.py --bypass_status --prepid HIG-RunIISummer20UL18wmLHEGEN-02820
GEN_ERR=$?
if [ $GEN_ERR -ne 0 ]; then
  echo "GEN Checking Script returned exit code $GEN_ERR which means there are $GEN_ERR errors"
  echo "Validation WILL NOT RUN"
  echo "Please correct errors in the request and run validation again"
  exit $GEN_ERR
fi
echo "Running VALIDATION. GEN Request Checking Script returned no errors"
# GEN Script end

export SCRAM_ARCH=slc7_amd64_gcc700

source /cvmfs/cms.cern.ch/cmsset_default.sh
if [ -r CMSSW_10_6_28/src ] ; then
  echo release CMSSW_10_6_28 already exists
else
  scram p CMSSW CMSSW_10_6_28
fi
cd CMSSW_10_6_28/src
eval `scram runtime -sh`

# Download fragment from McM
curl -s -k https://cms-pdmv.cern.ch/mcm/public/restapi/requests/get_fragment/HIG-RunIISummer20UL18wmLHEGEN-02820 --retry 3 --create-dirs -o Configuration/GenProduction/python/HIG-RunIISummer20UL18wmLHEGEN-02820-fragment.py
[ -s Configuration/GenProduction/python/HIG-RunIISummer20UL18wmLHEGEN-02820-fragment.py ] || exit $?;

# Check if fragment contais gridpack path ant that it is in cvmfs
if grep -q "gridpacks" Configuration/GenProduction/python/HIG-RunIISummer20UL18wmLHEGEN-02820-fragment.py; then
  if ! grep -q "/cvmfs/cms.cern.ch/phys_generator/gridpacks" Configuration/GenProduction/python/HIG-RunIISummer20UL18wmLHEGEN-02820-fragment.py; then
    echo "Gridpack inside fragment is not in cvmfs."
    exit -1
  fi
fi
scram b
cd ../..

# Maximum validation duration: 28800s
# Margin for validation duration: 30%
# Validation duration with margin: 28800 * (1 - 0.30) = 20160s
# Time per event for each sequence: 50.2891s
# Threads for each sequence: 1
# Time per event for single thread for each sequence: 1 * 50.2891s = 50.2891s
# Which adds up to 50.2891s per event
# Single core events that fit in validation duration: 20160s / 50.2891s = 400
# Produced events limit in McM is 10000
# According to 1.0000 efficiency, validation should run 10000 / 1.0000 = 10000 events to reach the limit of 10000
# Take the minimum of 400 and 10000, but more than 0 -> 400
# It is estimated that this validation will produce: 400 * 1.0000 = 400 events
EVENTS=400


# cmsDriver command
cmsDriver.py Configuration/GenProduction/python/HIG-RunIISummer20UL18wmLHEGEN-02820-fragment.py --python_filename HIG-RunIISummer20UL18wmLHEGEN-02820_1_cfg.py --eventcontent RAWSIM,LHE --customise Configuration/DataProcessing/Utils.addMonitoring --datatier GEN,LHE --fileout file:HIG-RunIISummer20UL18wmLHEGEN-02820.root --conditions 106X_upgrade2018_realistic_v4 --beamspot Realistic25ns13TeVEarly2018Collision --customise_commands process.source.numberEventsInLuminosityBlock="cms.untracked.uint32(100)" --step LHE,GEN --geometry DB:Extended --era Run2_2018 --no_exec --mc -n $EVENTS || exit $? ;

# Run generated config
REPORT_NAME=HIG-RunIISummer20UL18wmLHEGEN-02820_report.xml
# Run the cmsRun
cmsRun -e -j $REPORT_NAME HIG-RunIISummer20UL18wmLHEGEN-02820_1_cfg.py || exit $? ;

# Parse values from HIG-RunIISummer20UL18wmLHEGEN-02820_report.xml report
processedEvents=$(grep -Po "(?<=<Metric Name=\"NumberEvents\" Value=\")(.*)(?=\"/>)" $REPORT_NAME | tail -n 1)
producedEvents=$(grep -Po "(?<=<TotalEvents>)(\d*)(?=</TotalEvents>)" $REPORT_NAME | tail -n 1)
threads=$(grep -Po "(?<=<Metric Name=\"NumberOfThreads\" Value=\")(.*)(?=\"/>)" $REPORT_NAME | tail -n 1)
peakValueRss=$(grep -Po "(?<=<Metric Name=\"PeakValueRss\" Value=\")(.*)(?=\"/>)" $REPORT_NAME | tail -n 1)
peakValueVsize=$(grep -Po "(?<=<Metric Name=\"PeakValueVsize\" Value=\")(.*)(?=\"/>)" $REPORT_NAME | tail -n 1)
totalSize=$(grep -Po "(?<=<Metric Name=\"Timing-tstoragefile-write-totalMegabytes\" Value=\")(.*)(?=\"/>)" $REPORT_NAME | tail -n 1)
totalSizeAlt=$(grep -Po "(?<=<Metric Name=\"Timing-file-write-totalMegabytes\" Value=\")(.*)(?=\"/>)" $REPORT_NAME | tail -n 1)
totalJobTime=$(grep -Po "(?<=<Metric Name=\"TotalJobTime\" Value=\")(.*)(?=\"/>)" $REPORT_NAME | tail -n 1)
totalJobCPU=$(grep -Po "(?<=<Metric Name=\"TotalJobCPU\" Value=\")(.*)(?=\"/>)" $REPORT_NAME | tail -n 1)
eventThroughput=$(grep -Po "(?<=<Metric Name=\"EventThroughput\" Value=\")(.*)(?=\"/>)" $REPORT_NAME | tail -n 1)
avgEventTime=$(grep -Po "(?<=<Metric Name=\"AvgEventTime\" Value=\")(.*)(?=\"/>)" $REPORT_NAME | tail -n 1)
if [ -z "$threads" ]; then
  echo "Could not find NumberOfThreads in report, defaulting to 1"
  threads=1
fi
if [ -z "$eventThroughput" ]; then
  eventThroughput=$(bc -l <<< "scale=4; 1 / ($avgEventTime / $threads)")
fi
if [ -z "$totalSize" ]; then
  totalSize=$totalSizeAlt
fi
if [ -z "$processedEvents" ]; then
  processedEvents=$EVENTS
fi
echo "Validation report of HIG-RunIISummer20UL18wmLHEGEN-02820 sequence 1/1"
echo "Processed events: $processedEvents"
echo "Produced events: $producedEvents"
echo "Threads: $threads"
echo "Peak value RSS: $peakValueRss MB"
echo "Peak value Vsize: $peakValueVsize MB"
echo "Total size: $totalSize MB"
echo "Total job time: $totalJobTime s"
echo "Total CPU time: $totalJobCPU s"
echo "Event throughput: $eventThroughput"
echo "CPU efficiency: "$(bc -l <<< "scale=2; ($totalJobCPU * 100) / ($threads * $totalJobTime)")" %"
echo "Size per event: "$(bc -l <<< "scale=4; ($totalSize * 1024 / $producedEvents)")" kB"
echo "Time per event: "$(bc -l <<< "scale=4; (1 / $eventThroughput)")" s"
echo "Filter efficiency percent: "$(bc -l <<< "scale=8; ($producedEvents * 100) / $processedEvents")" %"
echo "Filter efficiency fraction: "$(bc -l <<< "scale=10; ($producedEvents) / $processedEvents")
