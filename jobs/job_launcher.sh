#!/bin/bash

set -euo pipefail
IFS=$'\n\t'

#==============================
#  Utility logging function
#==============================

RED='\033[0;31m'
YELLOW='\033[1;33m'
GREEN='\033[0;32m'
CYAN='\033[0;36m'
BLUE='\033[0;34m'
NC='\033[0m'

log() {
    local level="$1"; shift
    local msg="$*"
    local timestamp
    timestamp="$(date '+%Y-%m-%d %H:%M:%S')"

    local level_num=0 color="$NC"
    case "$level" in
        ERROR) level_num=1; color="$RED" ;;
        WARN)  level_num=2; color="$YELLOW" ;;
        INFO)  level_num=3; color="$GREEN" ;;
        DEBUG) level_num=4; color="$CYAN" ;;
        ALL)   level_num=5; color="$BLUE" ;;
        *)     level_num=3 ;;
    esac

    local prefix="${color}[$timestamp][$level]${NC}"

    case "$level" in
        DEBUG|INFO) echo -e "${prefix} $msg" >&1 ;;
        WARN|ERROR) echo -e "${prefix} $msg" >&2 ;;
        ALL)
            echo -e "${prefix} $msg" >&1
            echo -e "${prefix} $msg" >&2
            ;;
        *) echo -e "${prefix} $msg" >&1 ;;
    esac
}

#==============================
# Utility functions
#==============================

HOSTNAME=$(hostname -f 2>/dev/null || hostname)
if [[ "${HOSTNAME}" =~ ^cc.*\.in2p3\.fr$ ]]; then
    # Detect CC-IN2P3 cluster
    CLUSTER="CC-IN2P3"
elif [[ "${HOSTNAME}" =~ ^lxlogin[0-9]+\.ihep\.ac\.cn$ ]]; then
    # Detect IHEP cluster
    CLUSTER="IHEP"
else
    echo "ERROR: Unknown cluster. Hostname: ${HOSTNAME}" >&2
    echo "Expected CC-IN2P3 (cca###) or IHEP (lxlogin###.ihep.ac.cn)" >&2
    exit 1
fi

log INFO "Cluster detected: ${CLUSTER}"

#==============================
# Configuration defaults
#==============================

LIST_BASE="/sps/juno/jdeandre/rtraw_ThomasRaymond/analysis/other/GoodList"

LOWER_BOUND=""
UPPER_BOUND=""

usage() {
    cat <<EOF
Usage: $(basename "$0") [options]

Required:
  --campaign    <str>      

Optional:
  --lower       <int>           Starting run number (inclusive)
  --upper       <int>           Ending run number (inclusive)
  --list-base   <str>           Base directory of the run list
  --help                        Show this help message and exit
EOF
}

parse_args() {
    while [[ $# -gt 0 ]]; do
        case "$1" in
            --campaign)     CAMPAIGN="$2"; shift 2 ;;
            --lower)        LOWER_BOUND="$2"; shift 2 ;;
            --upper)        UPPER_BOUND="$2"; shift 2 ;;
            --list-base)    LIST_BASE="$2"; shift 2 ;;
            --help|-h) usage; exit 0 ;;
            *) log ERROR "Unknown argument: $1"; usage; exit 1 ;;
        esac
    done

    if [[ -z "${CAMPAIGN:-}" ]]; then
        log ERROR "--campaign is required"
        usage
        exit 1
    fi
}

#==============================
# Fetch Run List
#==============================

load_run_list() {
    log INFO "Fetching run lists"

    mapfile -t RUN_LIST < <(cat "${LIST_BASE}/${CAMPAIGN}/physics_good.txt")

    log INFO "Total run entries loaded: ${#RUN_LIST[@]}"
}

#==============================
# Range Filtering
#==============================

filter_runs() {
    local filtered=()

    for entry in "${RUN_LIST[@]}"; do
        run="${entry%%:*}"
        campaign="${entry##*:}"

        [[ "$run" =~ ^[0-9]+$ ]] || continue

        if [[ -n "${LOWER_BOUND}" && "$run" -lt "$LOWER_BOUND" ]]; then
            continue
        fi
        if [[ -n "${UPPER_BOUND}" && "$run" -gt "$UPPER_BOUND" ]]; then
            continue
        fi

        filtered+=("${run}")
    done

    RUN_LIST=("${filtered[@]}")

    if (( ${#RUN_LIST[@]} == 0 )); then
        log WARN "No runs matched the provided range"
        exit 0
    fi

    log INFO "Runs selected:"
    for entry in "${RUN_LIST[@]}"; do
        log INFO "  ${entry}"
    done
}

#==============================
# Launch Jobs
#==============================

launch_jobs() {
    for entry in "${RUN_LIST[@]}"; do
        run="${entry}"

        log INFO ">>> Launching job for run ${run}"

        if sbatch \
            --job-name="jrafneck_${run}" \
            --output="/sps/juno/jdeandre/rtraw_ThomasRaymond/analysis/log/jrafneck_${run}.log" \
            --error="/sps/juno/jdeandre/rtraw_ThomasRaymond/analysis/err/jrafneck_${run}.err" \
            --partition="htc" \
            --ntasks=1 \
            --cpus-per-task=1 \
            --mem="2G" \
            --time="0-01:00:00" \
            --mail-user="thomas.raymond@iphc.cnrs.fr" \
            --mail-type="FAIL" \
            job_worker.sh \
            --run "${run}"
        then
            log INFO "Run ${run} submitted successfully"
        else
            log ERROR "Submission failed for run ${run}"
        fi
    done
}

#==============================
# Main
#==============================

main() {
    parse_args "$@"
    load_run_list
    filter_runs
    launch_jobs
    log INFO "All matching runs processed successfully"
}

main "$@"