#!/bin/bash

set -euo pipefail
IFS=$'\n\t'

#==============================
# Utility functions
#==============================

HOSTNAME=$(hostname -f 2>/dev/null || hostname)
if [[ "${HOSTNAME}" =~ ^cc.*\.in2p3\.fr$ ]]; then
    # Detect CC-IN2P3 cluster
    CLUSTER="CC-IN2P3"
    source /pbs/home/t/traymond/share/bash/logging.sh
    TEMPDIR=${TMPDIR}
    SOURCE_JUNOSW_PATH="/pbs/home/t/traymond/J25.7.4/git_junosw_load_J25_7_4.sh"
elif [[ "${HOSTNAME}" =~ ^lxlogin[0-9]+\.ihep\.ac\.cn$ ]]; then
    # Detect IHEP cluster
    CLUSTER="IHEP"
    source /junofs/users/traymond/bash/logging.sh
    TEMPDIR=${TEMP}
    SOURCE_JUNOSW_PATH="/afs/ihep.ac.cn/users/t/traymond/J25.3.0/git_junosw_J25_load.sh"
else
    echo "ERROR: Unknown cluster. Hostname: ${HOSTNAME}" >&2
    echo "Expected CC-IN2P3 (cc###) or IHEP (lxlogin###.ihep.ac.cn)" >&2
    exit 1
fi

log INFO "Cluster detected: ${CLUSTER}"

#==============================
# Configuration defaults
#==============================

IO_DIRECTORY_AMBER="/sps/juno/jdeandre/rtraw_ThomasRaymond/reconstruction/reprod/Amber_v5.5"
IO_DIRECTORY_EDWIN="/sps/juno/jdeandre/rtraw_ThomasRaymond/reconstruction/reprod/EDWIN"
TREENAME_AMBER="MuonReco"
TREENAME_EDWIN="Edwin_Muon"

usage() {
    cat <<EOF
Usage: $(basename "$0") --method <str> --run <int> [options]

Required:
  --method <str>        Method name {AMBER|EDWIN}
  --run <int>           Run number
  --help                Show this help message and exit
EOF
}

parse_args() {
    while [[ $# -gt 0 ]]; do
        case "$1" in
            --method) METHOD="$2"; shift 2 ;;
            --run)    RUN="$2"; shift 2 ;;
            --help|-h) usage; exit 0 ;;
            *) log ERROR "Unknown argument: $1"; usage; exit 1 ;;
        esac
    done

    if [[ -z "${METHOD:-}" ]]; then
        log ERROR "--method is required {AMBER|EDWIN}"
        usage
        exit 1
    fi

    if [[ -z "${RUN:-}" ]]; then
        log ERROR "--run is required"
        usage
        exit 1
    fi

    case "${METHOD}" in
        AMBER)
            IO_DIRECTORY="${IO_DIRECTORY_AMBER}"
            FILENAME="muonReco_Amber_v5.5_run${RUN}.root"
            TREENAME="${TREENAME_AMBER}"
            ;;
        EDWIN)
            IO_DIRECTORY="${IO_DIRECTORY_EDWIN}"
            FILENAME="RUN${RUN}-user.root"
            TREENAME="${TREENAME_EDWIN}"
            ;;
        *) 
            log ERROR "Invalid --method: ${METHOD} (expected {AMBER|EDWIN})"
            usage
            exit 1 
            ;;
    esac
}

#==============================
# Main
#==============================

main() {
    parse_args "$@"

    INPUT_FILE="${IO_DIRECTORY}/raw/${FILENAME}"
    OUTPUT_FILE="${IO_DIRECTORY}/${FILENAME}"

    if [[ ! -f "${INPUT_FILE}" ]]; then
        log ERROR "Input file not found: ${INPUT_FILE}"
        exit 1
    fi

    source ${SOURCE_JUNOSW_PATH}

    root -l -b -q "timestamp_sorter.cpp(\"${INPUT_FILE}\",\"${TREENAME}\", \"${OUTPUT_FILE}\")"
    root -l -b -q "timestamp_sorted_checker.cpp(\"${OUTPUT_FILE}\",\"${TREENAME}\")"

    log INFO "Job completed successfully"
}

main "$@"