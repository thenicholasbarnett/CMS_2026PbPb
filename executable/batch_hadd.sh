#!/usr/bin/env bash
set -euo pipefail

if [ $# -ne 4 ]; then
    echo "Usage: $0 OUT_FILE "IN_FILES" BATCH_SIZE NJOBS" >&2
    echo "Note: IN_FILES must be quoted to prevent shell expansion" >&2
    exit 1
fi

OUT_FILE="$1"
IN_FILES="$2"
BATCH_SIZE="${3:-10}"
NJOBS="${4:-4}"

EOS_WORKDIR=""
MY_TMPDIR="${EOS_WORKDIR}/hadd_tmp_$$"
eos mkdir -p "$MY_TMPDIR"

mapfile -t FILES < <(find $IN_FILES -type f 2>/dev/null | sort)

if (( ${#FILES[@]} == 0 )); then
    echo "No files matched: $IN_FILES" >&2
    echo "Pattern must be a valid path with files and wildcard(s), e.g. /eos/user/username/files*/*.root" >&2
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

    for ((i=0; i<${#current_files[@]}; i+=BATCH_SIZE)); do
        chunk=("${current_files[@]:i:BATCH_SIZE}")
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

    level_summary+=("level ${level}: ${batch} batches of up to ${BATCH_SIZE}")
    current_files=("${next_files[@]}")
    level=$(( level + 1 ))
done

echo "Writing final output: ${OUT_FILE}"
mv "${current_files[0]}" "$OUT_FILE"

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
