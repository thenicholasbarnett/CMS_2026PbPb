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

echo "Started: $(date '+%Y-%m-%d %H:%M:%S')"

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

# progress bar function: draw_bar LABEL CURRENT TOTAL BAR_WIDTH
draw_bar() {
    local label="$1"
    local current="$2"
    local total="$3"
    local width="${4:-40}"
    local filled=$(( current * width / total ))
    local empty=$(( width - filled ))
    local pct=$(( current * 100 / total ))
    local bar
    bar="$(printf '%0.s█' $(seq 1 $filled) 2>/dev/null)$(printf '%0.s░' $(seq 1 $empty) 2>/dev/null)"
    printf "\r  %-20s [%s] %d/%d (%d%%)" "$label" "$bar" "$current" "$total" "$pct"
}

# normalise IN_FILES to a directory + filename pattern
IN_DIR="$(dirname "$IN_FILES")"
IN_PAT="$(basename "$IN_FILES")"

if [[ -d "$IN_FILES" ]]; then
    IN_DIR="$IN_FILES"
    IN_PAT="*.root"
elif [[ "$IN_PAT" == *root && "$IN_PAT" != *.root ]]; then
    IN_PAT="${IN_PAT/%root/.root}"
fi

mapfile -t FILES < <(find "$IN_DIR" -type f -name "$IN_PAT" | sort)

if (( ${#FILES[@]} == 0 )); then
    echo "No files matched: $IN_FILES" >&2
    echo "Pattern must be a valid path with files and wildcard(s), e.g. /eos/user/username/mydir/type1_*.root" >&2
    exit 1
fi

CHECK_JOBS=$(( NJOBS > 4 ? NJOBS : 4 ))
TOTAL_INPUT=${#FILES[@]}
echo "Found ${TOTAL_INPUT} files — checking for zombies..."
ZOMBIE_START=$(date +%s)

valid_files=()
zombie_files=()
pids=()
tmpdir_check=$(mktemp -d)
checked=0

draw_bar "Zombie check:" 0 "$TOTAL_INPUT"

for f in "${FILES[@]}"; do
    while (( $(jobs -rp | wc -l) >= CHECK_JOBS )); do
        sleep 0.1
    done

    (
        if rootls "$f" &>/dev/null; then
            echo "valid:$f" > "${tmpdir_check}/$BASHPID"
        else
            echo "zombie:$f" > "${tmpdir_check}/$BASHPID"
        fi
    ) &
    pids+=($!)

    for p in "${pids[@]}"; do
        if [[ -f "${tmpdir_check}/${p}" ]]; then
            r=$(< "${tmpdir_check}/${p}")
            if [[ "$r" == valid:* ]]; then
                valid_files+=("${r#valid:}")
            else
                zombie_files+=("${r#zombie:}")
            fi
            rm -f "${tmpdir_check}/${p}"
            checked=$(( checked + 1 ))
            draw_bar "Zombie check:" "$checked" "$TOTAL_INPUT"
        fi
    done
done

for pid in "${pids[@]}"; do
    wait "$pid"
    if [[ -f "${tmpdir_check}/${pid}" ]]; then
        r=$(< "${tmpdir_check}/${pid}")
        if [[ "$r" == valid:* ]]; then
            valid_files+=("${r#valid:}")
        else
            zombie_files+=("${r#zombie:}")
        fi
        rm -f "${tmpdir_check}/${pid}"
        checked=$(( checked + 1 ))
        draw_bar "Zombie check:" "$checked" "$TOTAL_INPUT"
    fi
done

rm -rf "$tmpdir_check"
printf "\n"

ZOMBIE_END=$(date +%s)
ZOMBIE_ELAPSED=$(( ZOMBIE_END - ZOMBIE_START ))

if (( ${#zombie_files[@]} > 0 )); then
    for zf in "${zombie_files[@]}"; do
        echo "  [ZOMBIE] $zf"
    done
    echo "Warning: skipping ${#zombie_files[@]} zombie/corrupt file(s)" >&2
fi

echo "Zombie check complete in ${ZOMBIE_ELAPSED}s"

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
    n_batches=$(( (${#current_files[@]} + BATCH_SIZE - 1) / BATCH_SIZE ))
    echo "Level ${level}: ${#current_files[@]} files → ${n_batches} batches"

    next_files=()
    batch=0
    pids=()
    completed=0

    draw_bar "Level ${level}:" 0 "$n_batches"

    for ((i=0; i<${#current_files[@]}; i+=BATCH_SIZE)); do
        chunk=("${current_files[@]:i:BATCH_SIZE}")
        partial="${MY_TMPDIR}/level${level}_batch${batch}.root"
        next_files+=("$partial")

        hadd "$partial" "${chunk[@]}" >>"$LOG_FILE" 2>&1 &
        pids+=($!)
        batch=$(( batch + 1 ))

        while (( $(jobs -rp | wc -l) >= NJOBS )); do
            sleep 0.5
        done
    done

    for pid in "${pids[@]}"; do
        if ! wait "$pid"; then
            printf "\n"
            echo "A hadd batch failed at level ${level} — check ${LOG_FILE}" >&2
            exit 1
        fi
        completed=$(( completed + 1 ))
        draw_bar "Level ${level}:" "$completed" "$n_batches"
    done

    printf "\n"
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
echo "Finished: $(date '+%Y-%m-%d %H:%M:%S')"
echo "====================="