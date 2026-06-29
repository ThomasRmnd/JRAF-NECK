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
    TEMPDIR=${TMPDIR}
elif [[ "${HOSTNAME}" =~ ^lxlogin[0-9]+\.ihep\.ac\.cn$ ]]; then
    # Detect IHEP cluster
    CLUSTER="IHEP"
    TEMPDIR=${TEMP}
else
    echo "ERROR: Unknown cluster. Hostname: ${HOSTNAME}" >&2
    echo "Expected CC-IN2P3 (cc###) or IHEP (lxlogin###.ihep.ac.cn)" >&2
    exit 1
fi

log INFO "Cluster detected: ${CLUSTER}"

#==============================
# Configuration defaults
#==============================

XRD_URL="root://xrootd-archive.cr.cnaf.infn.it:1095/"
XRD_BASEPATH="/production/storm/dirac"

OUTPUT_DIRECTORY="/sps/juno/jdeandre/rtraw_ThomasRaymond/reconstruction/reprod/TT"

usage() {
    cat <<EOF
Usage: $(basename "$0") --campaign <str> --run <int> --list-base <str>

Required:
  --campaign    <str>           Campaign name
  --run         <int>           Run ID
  --list-base   <str>           Base directory of the run list
EOF
}

parse_args() {
    if [[ $# -eq 0 ]]; then
        usage
        exit 1
    fi

    while [[ $# -gt 0 ]]; do
        case "$1" in
            --campaign)     CAMPAIGN="$2"; shift 2 ;;
            --run)          RUN="$2"; shift 2 ;;
            --list-base)    LIST_BASE="$2"; shift 2 ;;
            --help|-h)   usage; exit 0 ;;
            *) log ERROR "Unknown argument: $1"; usage; exit 1 ;;
        esac
    done

    if [[ -z "${CAMPAIGN:-}" ]]; then
        log ERROR "--campaign is required"
        usage
        exit 1
    fi

    if [[ -z "${RUN:-}" ]]; then
        log ERROR "--run is required"
        usage
        exit 1
    fi

    if [[ ! "${RUN}" =~ ^[0-9]+$ ]]; then
        log ERROR "--run must be an integer"
        exit 1
    fi

    if [[ -z "${LIST_BASE:-}" ]]; then
        log ERROR "--list-base is required"
        usage
        exit 1
    fi

    PROXY_PATH="/sps/juno/jdeandre/rtraw_ThomasRaymond/.cert_traymond_juno_user"
    if [[ ! -f "${PROXY_PATH}" ]]; then
        log ERROR "X.509 proxy does not exist: ${PROXY_PATH}"
        exit 1
    fi
    if [[ ! -r "${PROXY_PATH}" ]]; then
        log ERROR "X.509 proxy not readable: ${PROXY_PATH}"
        exit 1
    fi
    export X509_USER_PROXY="${PROXY_PATH}"
}

#==============================
# Main
#==============================

main() {
    parse_args "$@"

    mkdir -p "${OUTPUT_DIRECTORY}" || {
        log ERROR "Failed to create directory: ${OUTPUT_DIRECTORY}"
        exit 1
    }

    local bucket_val=$(( (10#$RUN / 1000) * 1000 ))
    local group_val=$(( (10#$RUN / 100) * 100 ))

    RUN_BUCKET=$(printf "%08d" "${bucket_val}")
    RUN_GROUP=$(printf "%08d" "${group_val}")

    mapfile -t RUN_LIST < <(cat "${LIST_BASE}/${CAMPAIGN}/esd_list/run_${RUN}.txt")

    if (( ${#RUN_LIST[@]} == 0 )); then
        log WARN "No runs matched the provided range"
        exit 0
    fi

    if [[ "${RUN_LIST[0]}" =~ RUN\.([0-9]+)\.[^/]*([0-9]{14})[^/]* ]]; then
        local run="${BASH_REMATCH[1]}"
        local timestamp="${BASH_REMATCH[2]}"
    else
        log ERROR "Unrecognized ReProd path format: ${RUN_LIST[0]}"
        exit 1
    fi    

    if (( RUN >= 9591 && RUN <= 10169 )); then
        local year="${timestamp:0:4}"
        local month="${timestamp:4:2}"
        local day="${timestamp:6:2}"
        tt_chain_filepath="${XRD_BASEPATH}/juno/user/j/jpandre_1/tt_data_auto/${year}/${month}${day}/RUN.${RUN}.*.EDM.user.root"

    elif (( RUN >= 10176 && RUN <= 10479 )); then
        tt_chain_filepath="${XRD_BASEPATH}/juno/user/j/jpandre_1/tt_data_auto/${RUN_BUCKET}/${RUN_GROUP}/${RUN}/RUN.${RUN}.*.EDM.user.root"

    elif (( RUN >= 10480 )); then
        tt_chain_filepath="${XRD_BASEPATH}/juno/juno-reprod/TT25A/J25.4.3-patched/user_rec/${RUN_BUCKET}/${RUN_GROUP}/${RUN}/RUN.${RUN}.*.EDM.user.root"

    else
        log ERROR "No TT reco path rule defined for run ${RUN}"
        exit 1
    fi

    local dir_tt=$(dirname "${tt_chain_filepath}")
    local pattern_tt=$(basename "${tt_chain_filepath}")

    local nfiles=$(
        xrdfs "${XRD_URL}" ls "${dir_tt}" \
        | grep -E "${pattern_tt}" \
        | wc -l
    )
    log DEBUG "Found ${nfiles} TT reconstruction files"

    RECONSTRUCTION_TT_FILEPATH="${XRD_URL}${tt_chain_filepath}"
    OUTPUT_FILEPATH="${OUTPUT_DIRECTORY}/RUN.${RUN}.TT.root"

    source /pbs/home/t/traymond/J25.7.4/git_junosw_load_J25_7_4.sh

    root -l -b -q "tt_reco_extractor.cpp(\"${RECONSTRUCTION_TT_FILEPATH}\", ${RUN}, \"${OUTPUT_FILEPATH}\")"
}

main "$@"