#!/bin/bash

set -euo pipefail

# Assign default configuration, allow override from file
TEMPLATE_HDR_DEFAULT="header-default"
TEMPLATE_HDR_ALT="header-alt"
TEMPLATE_HDR_PRUNED="header-pruned"
COPYRIGHT_PREFIX="spdx-string-c"
COPYRIGHT_YEAR="$(date +%Y)"
# COPYRIGHT_YEAR="2026-$(date +%Y)" (starting from 2027)
COPYRIGHT_HOLDER="OCUDU contributors"
LICENSE="BSD-3-Clause-Open-MPI"

config_env=".reuse/annotate-env.txt"
match_alt=".reuse/annotate-alt.txt"
match_exclude=".reuse/annotate-exclude.txt"

# Override config from file (skip empty file/lines and commented lines)
while IFS='=' read -r key value; do
  [[ -z "$key" || "$key" =~ ^# ]] && continue
  export "$key=$value"
done < "${config_env}"

# Parse args
PRUNE=false
VERBOSE=false
SCAN=false

while [[ $# -gt 0 ]]; do
  case "$1" in
    -v|--verbose)
      VERBOSE=true
      shift
      ;;
    -p|--prune)
      PRUNE=true
      shift
      ;;
    -s|--scan)
      SCAN=true
      shift
      ;;
    *)
      break
      ;;
  esac
done

# Prepare temporary directory and filenames for lists of files
tmpdir=$(mktemp -d)

files_all="${tmpdir}/files_all.txt"
files_filtered="${tmpdir}/files_filtered.txt"

files_cpp="${tmpdir}/files_cpp.txt"
files_generic="${tmpdir}/files_generic.txt"

files_cpp_default="${tmpdir}/files_cpp_default.txt"
files_cpp_alt="${tmpdir}/files_cpp_alt.txt"

files_generic_default="${tmpdir}/files_generic_default.txt"
files_generic_alt="${tmpdir}/files_generic_alt.txt"

if $SCAN || [ -t 0 ]; then
    find . -type f -printf "%P\n" > "${files_all}"
else
    cat > "${files_all}"
fi

if $VERBOSE; then
  echo "Written to ${files_all}"
fi

# Filter excluded files (if filter is defined)
if [[ -f "${match_exclude}" ]]; then
  grep -v -E -f "${match_exclude}" ${files_all} > "${files_filtered}" || true
else
  cat "${files_all}" > "${files_filtered}"
fi

# Filter C++ files to apply single-line comments for the headers
match_cpp="\.(cc|cpp|c|h)$"
grep    -E ${match_cpp} ${files_filtered} > ${files_cpp} || true
grep -v -E ${match_cpp} ${files_filtered} > ${files_generic} || true

# Filter files that shall use the alternative (alt) header (if filter is defined)
if [[ -f "${match_alt}" ]]; then
  # C++ files
  grep -v -E -f ${match_alt} ${files_cpp} > ${files_cpp_default} || true
  grep    -E -f ${match_alt} ${files_cpp} > ${files_cpp_alt} || true

  # Other files
  grep -v -E -f ${match_alt} ${files_generic} > ${files_generic_default} || true
  grep    -E -f ${match_alt} ${files_generic} > ${files_generic_alt} || true
else
  # C++ files
  cat "${files_cpp}" > "${files_cpp_default}"
  touch "${files_cpp_alt}"

  # Other files
  cat "${files_generic}" > "${files_generic_default}"
  touch "${files_generic_alt}"
fi

# Prepare Reuse application and params
REUSE_APP="reuse"
REUSE_BASE_ARGS=(
  "annotate"
  "--merge-copyrights"
  "--copyright-prefix=${COPYRIGHT_PREFIX}"
  "--year=${COPYRIGHT_YEAR}"
  "--copyright=${COPYRIGHT_HOLDER}"
  "--license=${LICENSE}"
)
REUSE_CPP_ARGS=(
  "--style=cpp"
)
REUSE_GENERIC_ARGS=(
  "--skip-unrecognised"
)
REUSE_HDR_DEFAULT=(
  "--template=${TEMPLATE_HDR_DEFAULT}"
)
REUSE_HDR_ALT=(
  "--template=${TEMPLATE_HDR_ALT}"
)
REUSE_HDR_PRUNED=(
  "--template=${TEMPLATE_HDR_PRUNED}"
)

XARGS_APP="xargs"
XARGS_BATCH_SIZE=100
XARGS_PROCS=$(nproc)
XARGS_BASE_ARGS=(
  "--max-lines=${XARGS_BATCH_SIZE}"
  "--max-procs=${XARGS_PROCS}"
)

# Output lines from reuse that are informational but not actionable
REUSE_OUTPUT_FILTERS=(
  "^Successfully changed header of "
  "^Skipped unrecognised file "
)

# Run xargs+reuse and filter noisy informational output unless verbose
run_annotate() {
  if $VERBOSE; then
    "${XARGS_APP}" "${XARGS_BASE_ARGS[@]}" "$@"
  else
    local pattern tmpout
    pattern=$(printf '%s\n' "${REUSE_OUTPUT_FILTERS[@]}" | paste -sd '|')
    tmpout=$(mktemp "${tmpdir}/annotate_out.XXXXXX")
    "${XARGS_APP}" "${XARGS_BASE_ARGS[@]}" "$@" > "${tmpout}"
    grep -v -E "${pattern}" "${tmpout}" || true
  fi
}

# Prune copyright (if enabled)
if $PRUNE; then
  run_annotate -a "${files_cpp}"     "${REUSE_APP}" "${REUSE_BASE_ARGS[@]}" "${REUSE_CPP_ARGS}"     "${REUSE_HDR_PRUNED}"
  run_annotate -a "${files_generic}" "${REUSE_APP}" "${REUSE_BASE_ARGS[@]}" "${REUSE_GENERIC_ARGS}" "${REUSE_HDR_PRUNED}"
fi

# Apply new headers
run_annotate -a "${files_cpp_default}"     "${REUSE_APP}" "${REUSE_BASE_ARGS[@]}" "${REUSE_CPP_ARGS}"     "${REUSE_HDR_DEFAULT}"
run_annotate -a "${files_cpp_alt}"         "${REUSE_APP}" "${REUSE_BASE_ARGS[@]}" "${REUSE_CPP_ARGS}"     "${REUSE_HDR_ALT}"
run_annotate -a "${files_generic_default}" "${REUSE_APP}" "${REUSE_BASE_ARGS[@]}" "${REUSE_GENERIC_ARGS}" "${REUSE_HDR_DEFAULT}"
run_annotate -a "${files_generic_alt}"     "${REUSE_APP}" "${REUSE_BASE_ARGS[@]}" "${REUSE_GENERIC_ARGS}" "${REUSE_HDR_ALT}"

rm -rf "${tmpdir}"
