#!/bin/sh

#--------------------------------------------------------------------------------------------------
#  JUNO Job Worker
#--------------------------------------------------------------------------------------------------

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
    echo "Expected CC-IN2P3 (cca###) or IHEP (lxlogin###.ihep.ac.cn)" >&2
    exit 1
fi

log INFO "Cluster detected: ${CLUSTER}"

#==============================
# Configuration defaults
#==============================

INPUT_ANALYSIS_SUFFIX_REPROD25C="output.reprod25c.root"
INPUT_ANALYSIS_SUFFIX_REPROD25D="output.reprod25d.root"

INPUT_RECONSTRUCTION_SUFFIX_REPROD25C="output.reprod25c.cca.root"
INPUT_RECONSTRUCTION_SUFFIX_REPROD25D="output.reprod25d.cca.root"

OUTPUT_SUFFIX_REPROD25C="jrafneck.reprod25c.root"
OUTPUT_SUFFIX_REPROD25D="jrafneck.reprod25d.root"

usage() {
    cat <<EOF
Usage: $(basename "$0") --campaign <str> --run <int>

Required:
  --campaign <str>               Reprod name {ReProd25C|ReProd25D}
  --run      <int>               Run ID
EOF
}

parse_args() {
    if [[ $# -eq 0 ]]; then
        usage
        exit 1
    fi

    while [[ $# -gt 0 ]]; do
        case "$1" in
            --run)       RUN="$2"; shift 2 ;;
            --campaign)  CAMPAIGN="$2"; shift 2 ;;
            --help|-h)   usage; exit 0 ;;
            *) log ERROR "Unknown argument: $1"; usage; exit 1 ;;
        esac
    done

    if [[ -z "${RUN:-}" || -z "${CAMPAIGN:-}" ]]; then
        log ERROR "--run and --campaign are required"
        usage
        exit 1
    fi

    if [[ ! "${RUN}" =~ ^[0-9]+$ ]]; then
        log ERROR "--run must be an integer"
        exit 1
    fi

    case "${CAMPAIGN}" in
        ReProd25C)
            INPUT_ANALYSIS_SUFFIX="${INPUT_ANALYSIS_SUFFIX_REPROD25C}"
            INPUT_RECONSTRUCTION_SUFFIX="${INPUT_RECONSTRUCTION_SUFFIX_REPROD25C}"
            OUTPUT_SUFFIX="${OUTPUT_SUFFIX_REPROD25C}"
            ;;
        ReProd25D)
            INPUT_ANALYSIS_SUFFIX="${INPUT_ANALYSIS_SUFFIX_REPROD25D}"
            INPUT_RECONSTRUCTION_SUFFIX="${INPUT_RECONSTRUCTION_SUFFIX_REPROD25D}"
            OUTPUT_SUFFIX="${OUTPUT_SUFFIX_REPROD25D}"
            ;;
        *)
            log ERROR "Invalid --campaign: ${CAMPAIGN} (expected {ReProd25C|ReProd25D})"
            exit 1
            ;;
    esac
}

#==============================
# Main
#==============================

main() {
    SCRIPT_DIR="${SLURM_SUBMIT_DIR:-$(pwd)}"
    SRC_DIR="${SCRIPT_DIR}/../src"
    JRAFNECK_CPP="${SRC_DIR}/jrafneck.cpp"
    if [[ ! -f "${JRAFNECK_CPP}" ]]; then
        log ERROR "Cannot find jrafneck.cpp at: ${JRAFNECK_CPP}"
        exit 1
    fi

    parse_args "$@"

    OUTPUT_DIRECTORY="/sps/juno/jdeandre/rtraw_ThomasRaymond/analysis/ibd/jrafneck"
    # OUTPUT_DIRECTORY="/sps/juno/jdeandre/rtraw_ThomasRaymond/test/jrafneck"
    mkdir -p "${OUTPUT_DIRECTORY}" || {
        log ERROR "Failed to create directory: ${OUTPUT_DIRECTORY}"
        exit 1
    }

    ANALYSIS_FILEPATH="/sps/juno/jdeandre/rtraw_ThomasRaymond/analysis/ibd/summary/RUN.${RUN}.${INPUT_ANALYSIS_SUFFIX}"
    # ANALYSIS_FILEPATH="/sps/juno/jdeandre/rtraw_ThomasRaymond/test/RUN.${RUN}.output.root"
    if [[ ! -f "${ANALYSIS_FILEPATH}" ]]; then
        log ERROR "Missing analysis file: ${ANALYSIS_FILEPATH}"
        exit 1
    fi

    RECONSTRUCTION_FILEPATH="/sps/juno/jdeandre/rtraw_ThomasRaymond/reconstruction/reprod/summary/RUN.${RUN}.${INPUT_RECONSTRUCTION_SUFFIX}"
    if [[ ! -f "${RECONSTRUCTION_FILEPATH}" ]]; then
        log ERROR "Missing reconstruction file: ${RECONSTRUCTION_FILEPATH}"
        exit 1
    fi

    RECONSTRUCTION_EDWIN_FILEPATH="/sps/juno/jdeandre/rtraw_ThomasRaymond/reconstruction/reprod/EDWIN/RUN${RUN}-user.root"
    if [[ ! -f "${RECONSTRUCTION_EDWIN_FILEPATH}" ]]; then
        log ERROR "Missing EDWIN reconstruction file: ${RECONSTRUCTION_EDWIN_FILEPATH}"
        exit 1
    fi

    RECONSTRUCTION_AMBER_FILEPATH="/sps/juno/jdeandre/rtraw_ThomasRaymond/reconstruction/reprod/Amber_v5.5/muonReco_Amber_v5.5_run${RUN}.root"
    if [[ ! -f "${RECONSTRUCTION_AMBER_FILEPATH}" ]]; then
        log ERROR "Missing Amber reconstruction file: ${RECONSTRUCTION_AMBER_FILEPATH}"
        exit 1
    fi

    RECONSTRUCTION_TT_FILEPATH=""

    OUTPUT_FILEPATH="${OUTPUT_DIRECTORY}/RUN.${RUN}.${OUTPUT_SUFFIX}"

    source /pbs/home/t/traymond/J25.7.4/git_junosw_load_J25_7_4.sh

    pushd "${SRC_DIR}" > /dev/null || {
        log ERROR "Failed to enter source directory: ${SRC_DIR}"
        exit 1
    }

    if ! root -l -b -q "jrafneck.cpp(\"${ANALYSIS_FILEPATH}\",\"${RECONSTRUCTION_FILEPATH}\",\"${RECONSTRUCTION_EDWIN_FILEPATH}\",\"${RECONSTRUCTION_AMBER_FILEPATH}\",\"${RECONSTRUCTION_TT_FILEPATH}\",\"${OUTPUT_FILEPATH}\")"; then
        log ERROR "ROOT execution failed for run ${RUN}"
        popd > /dev/null
        exit 1
    fi

    popd > /dev/null
}

main "$@"