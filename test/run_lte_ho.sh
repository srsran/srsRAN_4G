#!/bin/bash

#
# Copyright 2013-2023 Software Radio Systems Limited
#
# This file is part of srsRAN
#
# srsRAN is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsRAN is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

#
# Minimal LTE intra-eNB handover test over ZMQ, without an external channel broker:
#  - srsENB has two cells on different EARFCNs, each on its own RF port, that are neighbours of each other (A3 event)
#  - srsUE has two carriers, the second one is used to measure the inter-frequency neighbour. The ZMQ device maps
#    each carrier to the port with the matching frequency, so the UE follows the cell after a handover
#  - The mobility is emulated by alternating the relative DL gain of the cells with the srsENB "cell_gain" command
#  - A ping runs during the handovers and must not lose more than a few packets
#

epc_pid=0
enb_pid=0
ue_pid=0
ue_netns="ue1"
prefix="ho"

print_use(){
  echo "Please call script with srsRAN build path as first argument (number of PRBs and handovers are optional)"
  echo "E.g. ./run_lte_ho.sh [build_path] [nof_prb] [nof_ho]"
  exit 1
}

print_log()
{
  echo "---- Last lines of $1 ----"
  tail -n 30 "$1" 2>/dev/null
  echo "----"
}

stop_proc(){
  # Send SIGTERM and wait for the process to exit before moving on, so that
  # components are torn down in order (the apps force-exit themselves after 5s)
  local pid=$1
  local name=$2
  if [ -z "$pid" ] || [ "$pid" == "0" ] || ! ps -p $pid > /dev/null; then
    return
  fi
  kill -SIGTERM $pid
  for i in $(seq 1 20); do
    if ! ps -p $pid > /dev/null; then
      return
    fi
    sleep 0.5
  done
  echo "Killing $name"
  kill -9 $pid
}

kill_lte(){
  echo "Shutting down LTE network"
  stop_proc "$ue_pid" UE
  stop_proc "$enb_pid" eNB
  stop_proc "$epc_pid" EPC

  if [ -f ./srsRAN.backtrace.crash ]; then
    echo "Rename backtrace"
    mv ./srsRAN.backtrace.crash srsRAN.backtrace.log
  fi

  ip netns delete $ue_netns 2>/dev/null
}

fail(){
  echo "Error. $1"
  exit 1
}

# Starts an app in a detached screen session and returns its PID in start_pid
start_app(){
  local name=$1
  shift
  echo "Starting $name"
  screen -S $name -dm -L -Logfile ./${prefix}_screenlog_${name}.log "$@"
  sleep 2
  start_pid=$(pgrep -x $name)
  if [ -z "$start_pid" ]; then
    echo "Couldn't start $name"
    print_log ./${prefix}_screenlog_${name}.log
    kill_lte
    exit 1
  fi
  echo "$name's PID is $start_pid"
}

# Waits up to $2 seconds until the screen log of app $1 contains the string $3 at least $4 times (default 1)
wait_for_log(){
  for i in $(seq 1 $2); do
    if [ "$(grep -c "$3" ./${prefix}_screenlog_$1.log 2>/dev/null)" -ge "${4:-1}" ]; then
      return 0
    fi
    sleep 1
  done
  return 1
}

# Sets the relative DL gain of eNB cell $1 to $2 dB
set_cell_gain(){
  screen -S srsenb -X stuff "cell_gain $1 $2$(printf \\r)"
}

# Writes the cell configuration with two cells that are neighbours of each other
write_rr_conf(){
  sed '/^cell_list =/,$d' $build_path/../srsenb/rr.conf.example > ./${prefix}_rr.conf
  echo "cell_list = (" >> ./${prefix}_rr.conf
  for cell in 0 1; do
    local nb=$((1 - cell))
    [ "$cell" == "1" ] && echo "," >> ./${prefix}_rr.conf
    cat >> ./${prefix}_rr.conf <<EOF
  {
    rf_port = $cell;
    cell_id = $cell;
    tac = 0x0007;
    pci = ${pci[$cell]};
    root_seq_idx = $((204 + cell));
    dl_earfcn = ${earfcn[$cell]};
    ho_active = true;
    scell_list = ();
    meas_cell_list = (
      { eci = $(printf "0x19B%02X" $nb); dl_earfcn = ${earfcn[$nb]}; pci = ${pci[$nb]}; }
    );
    meas_report_desc = (
      { eventA = 3; a3_offset = 6; hysteresis = 0; time_to_trigger = 480; trigger_quant = "RSRP";
        max_report_cells = 1; report_interv = 120; report_amount = 1; }
    );
    meas_quant_desc = { rsrq_config = 4; rsrp_config = 4; };
  }
EOF
  done
  echo ");" >> ./${prefix}_rr.conf
  echo "nr_cell_list = ();" >> ./${prefix}_rr.conf
}

check_app_exit(){
  last_line=$(cat ./${prefix}_screenlog_$1.log | tail -n1)
  if [[ "$last_line" != *"---  exiting  ---"* ]]; then
    print_log ./${prefix}_screenlog_$1.log
    fail "$1 didn't seem to have exited properly."
  fi
}

check_no_errors(){
  num_error=$(grep "\[E\]" $1 | wc -l)
  if [ "$num_error" != "0" ]; then
    grep -m 10 "\[E\]" $1
    fail "Detected $num_error error messages in $1."
  fi
}

check_ping(){
  num_tx=$(grep -oP '\d+(?= packets transmitted)' ./${prefix}_ping_$1.log)
  num_rx=$(grep -oP '\d+(?= received)' ./${prefix}_ping_$1.log)
  if [ -z "$num_rx" ] || [ $((num_tx - num_rx)) -gt $2 ]; then
    print_log ./${prefix}_ping_$1.log
    fail "Lost more than $2 of ${num_tx:-0} packets in $1 ping."
  fi
  echo "$1 ping: $num_rx of $num_tx packets received"
}

check_ho(){
  num_ho=$(grep -c "HO successful" ./${prefix}_screenlog_srsue.log)
  num_ho_cmd=$(grep -c "Received HO command" ./${prefix}_screenlog_srsue.log)
  if [ "$num_ho" != "$nof_ho" ] || [ "$num_ho_cmd" != "$nof_ho" ]; then
    fail "Expected $nof_ho successful handovers, UE received $num_ho_cmd HO commands and completed $num_ho."
  fi

  # The UE must alternate between both cells, i.e. the HO commands target PCI 2 and 1
  for i in $(seq 1 $nof_ho); do
    expected_pci=${pci[$((i % 2))]}
    target_pci=$(grep "Received HO command" ./${prefix}_screenlog_srsue.log | sed -n "${i}p" | grep -oP '(?<=PCell=)\d+')
    if [ "$target_pci" != "$expected_pci" ]; then
      fail "Handover $i went to PCI ${target_pci:-none}, expected PCI $expected_pci."
    fi
  done

  # Only the initial access and the handovers should need a random access
  num_prach=$(grep -c "RACH:" ./${prefix}_screenlog_srsenb.log)
  if [ "$num_prach" != "$((nof_ho + 1))" ]; then
    fail "Detected $num_prach PRACH(s) at the eNB, expected $((nof_ho + 1))."
  fi

  # No radio link failure or connection re-establishment, i.e. the UE stayed connected
  if grep -q -i -E "re-?establishment|radio link failure" ./${prefix}_screenlog_srsue.log; then
    grep -i -E "re-?establishment|radio link failure" ./${prefix}_screenlog_srsue.log
    fail "UE lost the connection during the handovers."
  fi
}

# check arguments
if [ -z "$1" ]; then
  print_use
fi
build_path="$1"
nof_prb="${2:-25}"
nof_ho="${3:-4}"

case "$nof_prb" in
  "6"|"15"|"25"|"50") ;;
  *)
    echo "Invalid number of PRBs $nof_prb. Possible values are [6, 15, 25, 50]."
    exit 1;;
esac
echo "Running LTE handover test with $nof_prb PRB and $nof_ho handovers"

for app in srsepc/src/srsepc srsenb/src/srsenb srsue/src/srsue; do
  if [ ! -x "$build_path/$app" ]; then
    fail "$app binary not found or not executable!"
  fi
done

# Cell 0 is the initial serving cell, the UE moves to cell 1 and back
earfcn=(2850 3050)
pci=(1 2)
dl_freq=(2630e6 2650e6)
ul_freq=(2510e6 2530e6)
# Relative gain of the weaker cell, needs to be well beyond the A3 offset
ho_gain_db=-12

ip netns add $ue_netns || fail "Couldn't add netns. Missing permissions? Network namespace $ue_netns already exists?"

rm -f ./${prefix}_*
write_rr_conf

base_srate="11.52e6"
enb_rf_args="fail_on_disconnect=true,id=enb,base_srate=$base_srate,\
tx_port0=tcp://*:2000,rx_port0=tcp://localhost:2001,tx_port1=tcp://*:2100,rx_port1=tcp://localhost:2101,\
tx_freq0=${dl_freq[0]},tx_freq1=${dl_freq[1]},rx_freq0=${ul_freq[0]},rx_freq1=${ul_freq[1]}"
ue_rf_args="id=ue,base_srate=$base_srate,\
tx_port0=tcp://*:2001,rx_port0=tcp://localhost:2000,tx_port1=tcp://*:2101,rx_port1=tcp://localhost:2100,\
tx_freq0=${ul_freq[0]},tx_freq1=${ul_freq[1]},rx_freq0=${dl_freq[0]},rx_freq1=${dl_freq[1]}"

start_app srsepc $build_path/srsepc/src/srsepc $build_path/../srsepc/epc.conf.example \
          --hss.db_file=$build_path/../srsepc/user_db.csv.example \
          --log.filename=./${prefix}_epc.log
epc_pid=$start_pid

start_app srsenb $build_path/srsenb/src/srsenb $build_path/../srsenb/enb.conf.example \
          --enb_files.sib_config=$build_path/../srsenb/sib.conf.example \
          --enb_files.rb_config=$build_path/../srsenb/rb.conf.example \
          --enb_files.rr_config=./${prefix}_rr.conf \
          --enb.n_prb=$nof_prb \
          --rf.device_name=zmq \
          --rf.device_args=$enb_rf_args \
          --log.all_level=info \
          --log.filename=./${prefix}_enb.log
enb_pid=$start_pid

# Make cell 0 the strongest cell before the UE starts
set_cell_gain 1 $ho_gain_db

start_app srsue $build_path/srsue/src/srsue $build_path/../srsue/ue.conf.example \
          --rf.device_name=zmq \
          --rf.device_args=$ue_rf_args \
          --rat.eutra.dl_earfcn=${earfcn[0]} \
          --rat.eutra.nof_carriers=2 \
          --stack.have_tti_time_stats=false \
          --gw.netns=$ue_netns \
          --log.all_level=info \
          --log.filename=./${prefix}_ue.log
ue_pid=$start_pid

wait_for_log srsue 30 "Network attach successful" || { print_log ./${prefix}_screenlog_srsue.log; kill_lte; fail "UE didn't attach"; }
ue_ip=$(nsenter --net=/run/netns/$ue_netns ip -4 addr show tun_srsue | grep -oP '(?<=inet )[\d.]+')
echo "UE is connected with IP $ue_ip"
sleep 1

# Keep the UE busy in UL and DL during the handovers
nsenter --net=/run/netns/$ue_netns ping -i 0.2 172.16.0.1 > ./${prefix}_ping_ul.log 2>&1 &
ping_ul_pid=$!
ping -i 0.2 $ue_ip > ./${prefix}_ping_dl.log 2>&1 &
ping_dl_pid=$!
sleep 1

# Alternate the strongest cell, each swap must trigger one handover
for i in $(seq 1 $nof_ho); do
  target=$((i % 2))
  echo "Handover $i to cell $target (PCI ${pci[$target]})"
  set_cell_gain $target 0
  set_cell_gain $((1 - target)) $ho_gain_db
  if ! wait_for_log srsue 10 "HO successful" $i; then
    print_log ./${prefix}_screenlog_srsue.log
    kill $ping_ul_pid $ping_dl_pid
    kill_lte
    fail "Handover $i didn't complete"
  fi
  sleep 2
done

kill -INT $ping_ul_pid $ping_dl_pid
wait $ping_ul_pid $ping_dl_pid
kill_lte

# check results
check_ho
check_ping ul $nof_ho
check_ping dl $nof_ho
check_app_exit srsue
check_app_exit srsenb
check_app_exit srsepc
check_no_errors ./${prefix}_ue.log
check_no_errors ./${prefix}_enb.log
check_no_errors ./${prefix}_epc.log

echo "All tests passed!"
exit 0
