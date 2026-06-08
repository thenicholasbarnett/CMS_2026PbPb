#!/bin/bash -l
set -euo pipefail

if [[ $# -ne 4 ]]; then
  echo "Usage: $0 EXECUTABLE INPUT OUTPUT OUTPUT_TXT" >&2
  exit 1
fi

START_DIR="$(pwd)"
EXECUTABLE="$1"
INPUT="$2"
OUTPUT="$3"
OUTPUT_TXT="$4"

CMSSW_SRC=""

if [[ -z "${CMSSW_SRC}" ]]; then
  echo "ERROR: CMSSW_SRC is not set in runtime_wrapper.sh" >&2
  exit 1
fi

echo "Running in CMSSW environment: $(basename "$(dirname "${CMSSW_SRC}")")"
source /cvmfs/cms.cern.ch/cmsset_default.sh
cd "${CMSSW_SRC}"
eval "$(scramv1 runtime -sh)"
cd "${START_DIR}"

root -l -b -q "${EXECUTABLE}(\"${INPUT}\", \"${OUTPUT}\", \"${OUTPUT_TXT}\", false)"