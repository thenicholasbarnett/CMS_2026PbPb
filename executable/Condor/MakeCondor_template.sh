#!/bin/bash
set -euo pipefail

if [[ $# -ne 4 ]]; then
  echo "Usage: $0 JOBNAME EXECUTABLE FILELIST OUTPUT_DIR" >&2
  exit 1
fi

jobname="$1"
executable="$2"
filelist="$3"
output_base="$4"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
today=$(date +"%Y-%m-%d_%H-%M-%S")

exe_name="$(basename "$executable")"
filelist_name="$(basename "$filelist")"

echo "Copying executable and filelist..."

cp "$executable" .
cp "$filelist" .

run_script="run_job.sh"
chmod +x "$run_script"

if [[ "$exe_name" == *.sh ]]; then
  chmod +x "$exe_name"
fi

logdir="logs"
mkdir -p "$logdir/out" "$logdir/err" "$logdir/log"

final_output_dir="${output_base}/condor_${jobname}_${today}"
mkdir -p "$final_output_dir"

submission_file="submit_${jobname}.condor"

echo "Creating condor submission file..."

cat > "$submission_file" <<EOF
Universe                = vanilla
Executable              = $run_script

+JobFlavour             = "longlunch"

should_transfer_files   = YES
when_to_transfer_output = ON_EXIT

request_cpus            = 4

Transfer_Input_Files    = $(pwd)/$run_script,$(pwd)/$exe_name

EOF

count=0

while IFS= read -r input_file; do

  # skip empty lines
  [[ -z "$input_file" ]] && continue

  output_file="${final_output_dir}/output_${jobname}_${count}.root"

  cat >> "$submission_file" <<EOF
Arguments = $exe_name $input_file $output_file

Output    = $(pwd)/$logdir/out/job_${count}.out
Error     = $(pwd)/$logdir/err/job_${count}.err
Log       = $(pwd)/$logdir/log/job_${count}.log

Queue

EOF

  count=$((count + 1))

done < "$filelist_name"

echo "Submitting $count jobs..."

condor_submit "$submission_file"

echo "Done."
