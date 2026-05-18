#!/usr/bin/env bash
set -euo pipefail

OUT="$1"
PATTERN="$2"
GROUP_SIZE="${3:-50}"
NJOBS="${4:-4}"

EOS_WORKDIR="/eos/cms/store/group/phys_heavyions/nbarnett/hadd"
MY_TMPDIR="${EOS_WORKDIR}/hadd_tmp_$$"
eos mkdir -p "$MY_TMPDIR"

mapfile -t FILES < <(ls $PATTERN 2>/dev/null | sort)

if (( ${#FILES[@]} == 0 )); then
    echo "No files matched: $PATTERN"
    eos rm -r "$MY_TMPDIR"
    exit 1
fi

echo "Found ${#FILES[@]} files"
echo "Temporary directory: ${MY_TMPDIR}"

level=0
current_files=("${FILES[@]}")
declare -a level_summary

while (( ${#current_files[@]} > 1 )); do
    echo "Level ${level}: ${#current_files[@]} input files"

    next_files=()
    batch=0
    pids=()

    for ((i=0; i<${#current_files[@]}; i+=GROUP_SIZE)); do
        chunk=("${current_files[@]:i:GROUP_SIZE}")
        partial="${MY_TMPDIR}/level${level}_batch${batch}.root"
        next_files+=("$partial")

        echo "  hadd ${partial} from ${#chunk[@]} files"
        hadd "$partial" "${chunk[@]}" &
        pids+=($!)
        batch=$(( batch + 1 ))

        while (( $(jobs -rp | wc -l) >= NJOBS )); do
            wait -n
        done
    done

    for pid in "${pids[@]}"; do
        wait "$pid"
    done

    level_summary+=("level ${level}: ${batch} batches of up to ${GROUP_SIZE}")
    current_files=("${next_files[@]}")
    level=$(( level + 1 ))
done

echo "Writing final output: ${OUT}"
mv "${current_files[0]}" "$OUT"

echo "Cleaning up ${MY_TMPDIR}"
eos rm -r "$MY_TMPDIR"

echo ""
echo "=== Merge Summary ==="
echo "Input files: ${#FILES[@]}"
echo "Total levels: ${level}"
for entry in "${level_summary[@]}"; do
    echo "  ${entry}"
done
echo "Output: ${OUT}"
echo "====================="
