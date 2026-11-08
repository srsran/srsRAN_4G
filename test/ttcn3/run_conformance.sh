#!/bin/bash

# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: AGPL-3.0-or-later

# Runs a TTCN-3 test suite with Titan against srsUE's ttcn3_dut and checks that all test cases passed.
# Paths default to the layout of the container built with test/ttcn3/Dockerfile.

testsuite=${1:-srsue_conf_testcases}
timeout=${2:-3600}
dut=${TTCN3_DUT:-/srsran/build/srsue/src/test/ttcn3/src/ttcn3_dut}
tester_dir=${TTCN3_TESTER_DIR:-/ttcn3/build}
log_dir=${TTCN3_LOG_DIR:-/logs}
ut_port=2222

dut_pid=0
dut_exit=0

fail(){
  echo "Error. $1"
  exit 1
}

# The DUT is a child of this script, so it remains as zombie after exiting until it is waited for
dut_alive(){
  local state=$(ps -o stat= -p $dut_pid 2>/dev/null)
  [ ! -z "$state" ] && [[ "$state" != Z* ]]
}

stop_dut(){
  if [ "$dut_pid" == "0" ]; then
    return
  fi
  if ! dut_alive; then
    wait $dut_pid
    dut_exit=$?
    dut_pid=0
    return
  fi
  # The DUT handles SIGTERM through its event loop and shuts down gracefully
  echo "Stopping DUT"
  kill -SIGTERM $dut_pid
  for i in $(seq 1 20); do
    if ! dut_alive; then
      wait $dut_pid
      dut_exit=$?
      dut_pid=0
      return
    fi
    sleep 0.5
  done
  echo "Killing DUT"
  kill -9 $dut_pid
  wait $dut_pid
  dut_exit=137
  dut_pid=0
}
trap stop_dut EXIT

# ASAN builds write their reports to the log_path of ASAN_OPTIONS, if set
asan_log_path=$(echo "$ASAN_OPTIONS" | grep -oP '(?<=log_path=)[^:]+')

check_asan(){
  if [ -z "$asan_log_path" ]; then
    return
  fi
  local reports=$(ls ${asan_log_path}.* 2>/dev/null)
  if [ ! -z "$reports" ]; then
    for report in $reports; do
      echo "---- $report ----"
      head -n 50 "$report"
    done
    fail "Detected AddressSanitizer reports."
  fi
}

check_verdicts(){
  # e.g. "Verdict statistics: 0 none (0.00 %), 22 pass (100.00 %), 0 inconc (0.00 %), 0 fail (0.00 %), 0 error (0.00 %)."
  local stats=$(grep -m1 "Verdict statistics:" $log_dir/titan_console.log)
  if [ -z "$stats" ]; then
    fail "No verdict statistics found, the test suite didn't complete."
  fi
  echo "$stats"
  local num_pass=$(echo "$stats" | grep -oP '\d+(?= pass)')
  local num_not_pass=0
  for verdict in none inconc fail error; do
    num_not_pass=$((num_not_pass + $(echo "$stats" | grep -oP "\d+(?= $verdict)")))
  done
  if [ "$num_pass" == "0" ]; then
    fail "No test case passed, check that $testsuite is a valid control module."
  fi
  if [ "$num_not_pass" != "0" ]; then
    grep -E "^.*Test case .* finished\. Verdict: (none|inconc|fail|error)" $log_dir/titan_console.log
    fail "$num_not_pass test case(s) didn't pass."
  fi
}

[ -x "$dut" ] || fail "DUT binary $dut not found or not executable."
[ -x "$tester_dir/srslte_ttcn3_titan" ] || fail "Titan executable not found in $tester_dir."
mkdir -p $log_dir
rm -f $log_dir/*_titan.* $log_dir/srslte_ue_test*.log $log_dir/titan_console.log
[ ! -z "$asan_log_path" ] && rm -f ${asan_log_path}.*

echo "Starting DUT"
$dut --loglevel=info --logfilename=$log_dir/ttcn3_ue.log --pcap.filename=$log_dir/ttcn3_ue.pcap \
  > $log_dir/ttcn3_dut_console.log 2>&1 &
dut_pid=$!

echo "Waiting for the DUT to listen on SCTP port $ut_port"
for i in $(seq 1 30); do
  ss -Hln --sctp | grep -q ":$ut_port\b" && break
  dut_alive || { tail -n 30 $log_dir/ttcn3_dut_console.log; fail "DUT exited during startup."; }
  sleep 1
done
ss -Hln --sctp | grep -q ":$ut_port\b" || fail "DUT didn't open SCTP port $ut_port."

echo "Running test suite $testsuite"
cd $tester_dir
timeout $timeout ttcn3_start srslte_ttcn3_titan ../ue_tests.cfg $testsuite 2>&1 | tee $log_dir/titan_console.log
cd - > /dev/null

stop_dut

# Make Titan's logs readable, one event per line
for file in $log_dir/*_titan.*; do
  [ -f "$file" ] || continue
  ttcn3_logformat "$file" -i 2 -o "$file.formatted" && mv "$file.formatted" "$file"
done

check_asan
if [ "$dut_exit" != "0" ]; then
  tail -n 30 $log_dir/ttcn3_dut_console.log
  fail "DUT exited with code $dut_exit."
fi
check_verdicts

echo "All tests passed!"
exit 0
