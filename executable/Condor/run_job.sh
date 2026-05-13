#!/bin/bash -l
set -euo pipefail

if [[ $# -ne 3 ]]; then
  echo "Usage: $0 EXECUTABLE INPUT OUTPUT" >&2
  exit 1
fi

start_dir="$(pwd)"

executable="$1"
input_file="$2"
output_file="$3"

# Set this to your CMSSW src directory on lxplus, e.g.:
# /afs/cern.ch/user/x/username/public/CondorWorkArea/CMSSW_X_Y_Z/src
CMSSW_src=""

echo "Setting up CMSSW environment..."
source /cvmfs/cms.cern.ch/cmsset_default.sh
cd "$CMSSW_src"
eval "$(scramv1 runtime -sh)"
cmsenv
cd "$start_dir"

case "$executable" in

  *.py)
    echo "Running CMSSW python job"
    cmsRun "$executable" "$input_file" "$output_file"
    ;;

  *.C)
    echo "Running ROOT macro"
    root -l -b -q "${executable}(\"${input_file}\", \"${output_file}\")"
    ;;

  *.sh)
    echo "Running bash script"
    chmod +x "$executable"
    "$executable" "$input_file" "$output_file"
    ;;

  *)
    echo "Unsupported executable type: $executable" >&2
    exit 1
    ;;
esac
