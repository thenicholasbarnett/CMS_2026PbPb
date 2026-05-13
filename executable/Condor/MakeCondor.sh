#!/bin/bash
set -euo pipefail

if [[ $# -ne 4 ]]; then
  echo "Usage: $0 JOBNAME EXECUTABLE FILELIST OUTPUT_DIR" >&2
  exit 1
fi

JOBNAME="$1"
EXECUTABLE="$2"
FILELIST="$3"
OUTPUT_DIR="$4"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TEMPLATE="$SCRIPT_DIR/MakeCondor_template.sh"

TODAY=$(date +"%Y-%m-%d_%H-%M-%S")
WORKDIR="condor_${JOBNAME}_${TODAY}"

mkdir -p "$workdir"
cd "$workdir" || exit 1

cp "$TEMPLATE" "./MakeCondor_${JOBNAME}.sh"
cp "$SCRIPT_DIR/run_job.sh" "./run_job.sh" 
chmod +x "./MakeCondor_${JOBNAME}.sh"

"./MakeCondor_${JOBNAME}.sh" "$JOBNAME" "$EXECUTABLE" "$FILELIST" "$OUTPUT_DIR"
