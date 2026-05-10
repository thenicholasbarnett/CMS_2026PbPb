#!/bin/bash
set -euo pipefail

if [[ $# -ne 4 ]]; then
  echo "Usage: $0 JOBNAME EXECUTABLE FILELIST OUTPUT_DIR" >&2
  exit 1
fi

jobname="$1"
executable="$2"
filelist="$3"
output_dir="$4"

template="/eos/cms/store/group/phys_heavyions/nbarnett/condor/MakeCondor_template.sh"

today=$(date +"%Y-%m-%d_%H-%M-%S")
workdir="condor_${jobname}_${today}"

mkdir -p "$workdir"
cd "$workdir" || exit 1

cp "$template" "./MakeCondor_${jobname}.sh"
chmod +x "./MakeCondor_${jobname}.sh"

"./MakeCondor_${jobname}.sh" "$jobname" "$executable" "$filelist" "$output_dir"
