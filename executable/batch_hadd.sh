#!/usr/bin/env bash
set -euo pipefail

# Usage:
#   ./hadd_tree.sh final.root "filename_*.root" 50 4
#
# Args:
#   $1 = final output file
#   $2 = input glob, quoted
#   $3 = files per partial hadd
#   $4 = parallel hadd jobs

OUT="$1"
PATTERN="$2"
GROUP_SIZE="${3:-50}"
NJOBS="${4:-4}"

# Set this to the directory hadd is performed, e.g.:
# /eos/cms/store/group/phys_heavyions/<username>/hadd
WORKDIR="" 
TMPDIR="$WORKDIR/hadd_tmp_$$"
mkdir -p "$TMPDIR"

cleanup_on_error() {
    echo "Script failed. Temp files left in: $TMPDIR"
}
trap cleanup_on_error ERR

mapfile -t FILES < <(ls $PATTERN 2>/dev/null | sort)

if (( ${#FILES[@]} == 0 )); then
    echo "No files matched: $PATTERN"
    exit 1
fi

echo "Found ${#FILES[@]} files"
echo "Temporary directory: $TMPDIR"

level=0
current_files=("${FILES[@]}")

while (( ${#current_files[@]} > 1 )); do
    echo "Level $level: ${#current_files[@]} input files"

    next_files=()
    batch=0

    for ((i=0; i<${#current_files[@]}; i+=GROUP_SIZE)); do
        chunk=("${current_files[@]:i:GROUP_SIZE}")
        partial="$TMPDIR/level${level}_batch${batch}.root"
        next_files+=("$partial")

        (
            cd "$WORKDIR"
	        echo "  hadd $partial from ${#chunk[@]} files"
            hadd "$partial" "${chunk[@]}"
        ) &

        ((batch+=1))

        while (( $(jobs -rp | wc -l) >= NJOBS )); do
            wait -n
        done
    done

    wait
    current_files=("${next_files[@]}")
    ((level+=1))
done

echo "Writing final output: $OUT"
mv "${current_files[0]}" "$OUT"

echo "Cleaning up"
rm -rf "$TMPDIR"
trap - ERR

echo "Done: $OUT"
