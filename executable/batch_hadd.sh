#!/usr/bin/env bash
set -euo pipefail

if [ $# -ne 4 ]; then
    echo "Usage: $0 OUT_FILE \"IN_FILES\" BATCH_SIZE NJOBS" >&2
    echo "Note: IN_FILES must be quoted to prevent shell expansion" >&2
    exit 1
fi

OUT_FILE="$1"
IN_FILES="$2"
BATCH_SIZE="${3:-10}"
NJOBS="${4:-4}"

MY_TMPDIR="$(dirname "$OUT_FILE")/hadd_tmp_$$"
LOG_FILE="$(dirname "$OUT_FILE")/hadd_$$.log"
eos mkdir -p "$MY_TMPDIR"

cleanup() {
    if (( $? == 0 )); then
        eos rm -r "$MY_TMPDIR"
        rm -f "$LOG_FILE"
    else
        if [[ -s "$LOG_FILE" ]]; then
            echo "Failure — log written to ${LOG_FILE}" >&2
        fi
        echo "Leaving ${MY_TMPDIR} intact for inspection" >&2
    fi
}
trap cleanup EXIT

mapfile -t FILES < <(find "$(dirname "$IN_FILES")" -type f -name "$(basename "$IN_FILES")" | sort)

if (( ${#FILES[@]} == 0 )); then
    echo "No files matched: $IN_FILES" >&2
    echo "Pattern must be a valid path with files and wildcard(s), e.g. /eos/user/username/files*/*.root" >&2
    exit 1
fi

echo "Found ${#FILES[@]} files — checking for zombies..."
valid_files=()
zombie_files=()
for f in "${FILES[@]}"; do
    if rootls "$f" &>/dev/null; then
        valid_files+=("$f")
    else
        zombie_files+=("$f")
        echo "  [ZOMBIE] $f"
    fi
done

if (( ${#zombie_files[@]} > 0 )); then
    echo "Warning: skipping ${#zombie_files[@]} zombie/corrupt file(s)" >&2
fi

if (( ${#valid_files[@]} == 0 )); then
    echo "No valid ROOT files remain after zombie check." >&2
    exit 1
fi

TOTAL_FILES=$(( ${#valid_files[@]} + ${#zombie_files[@]} ))
FILES=("${valid_files[@]}")
echo "Proceeding with ${#FILES[@]} valid files"
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
        hadd "$partial" "${chunk[@]}" >>"$LOG_FILE" 2>&1 &
        pids+=($!)
        batch=$(( batch + 1 ))

        while (( $(jobs -rp | wc -l) >= NJOBS )); do
            sleep 0.5
        done
    done

    for pid in "${pids[@]}"; do
        if ! wait "$pid"; then
            echo "A hadd batch failed at level ${level} — check ${LOG_FILE}" >&2
            exit 1
        fi
    done

    level_summary+=("level ${level}: ${batch} batches of up to ${BATCH_SIZE}")
    current_files=("${next_files[@]}")
    level=$(( level + 1 ))
done

echo "Writing final output: ${OUT_FILE}"
mv "${current_files[0]}" "$OUT_FILE"

echo ""
echo "=== Merge Summary ==="
echo "Input files found:  ${TOTAL_FILES}"
echo "Zombies skipped:    ${#zombie_files[@]}"
echo "Files merged:       ${#FILES[@]}"
echo "Total levels:       ${level}"
for entry in "${level_summary[@]}"; do
    echo "  ${entry}"
done
echo "Output: ${OUT_FILE}"
echo "====================="