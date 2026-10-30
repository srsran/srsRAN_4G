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
# Minimal 5G end-to-end test using srsENB and srsUE over ZMQ:
#  - nsa_fdd: EN-DC with LTE anchor (EARFCN 300) and NR FDD cell in band 3, core is srsEPC
#  - nsa_tdd: EN-DC with LTE anchor (EARFCN 300) and NR TDD cell in band 78, core is srsEPC
#  - sa_fdd:  NR standalone FDD cell in band 3, core is Open5GS (5GC)
#

core_pid=0
enb_pid=0
ue_pid=0
ue_netns="ue1"

# Open5GS (SA only)
amf_addr="127.0.0.5"
sa_gw="10.45.0.1"
open5gs_nfs="nrf scp amf smf upf ausf udm udr pcf nssf bsf"
open5gs_pids=""

print_use(){
  echo "Please call script with srsRAN build path as first argument and the test mode as second"
  echo "E.g. ./run_5g.sh [build_path] [nsa_fdd|nsa_tdd|sa_fdd]"
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

kill_5g(){
  echo "Shutting down 5G network"
  stop_proc "$ue_pid" UE
  stop_proc "$enb_pid" eNB
  if [ "$is_sa" == "1" ]; then
    stop_5gc
  else
    stop_proc "$core_pid" EPC
  fi

  if [ -f ./srsRAN.backtrace.crash ]; then
    echo "Rename backtrace"
    mv ./srsRAN.backtrace.crash srsRAN.backtrace.log
  fi

  ip netns delete $ue_netns 2>/dev/null
}

# Starts MongoDB and the Open5GS 5GC network functions, the subscriber is the default srsUE USIM
start_5gc(){
  echo "Starting Open5GS 5GC"
  cfg_dir=$PWD/${mode}_open5gs
  mkdir -p $cfg_dir
  # Keep the database out of the log directory, MongoDB creates its files only readable by the user running the container,
  # which differs from the user uploading the logs in CI
  db_dir=$(mktemp -d)
  cp /etc/open5gs/*.yaml $cfg_dir/
  sed -i -e 's/mcc: 999/mcc: 001/' -e 's/mnc: 70/mnc: 01/' -e 's/tac: 1$/tac: 7/' \
         -e "s|/var/log/open5gs/\(.*\)\.log|$PWD/${mode}_open5gs_\\1.log|" $cfg_dir/*.yaml

  mongod --fork --quiet --bind_ip 127.0.0.1 --dbpath $db_dir --logpath ./${mode}_mongod.log > /dev/null || fail "Couldn't start MongoDB"
  cat > $cfg_dir/subscriber.json <<EOF
{"imsi": "001010123456780", "msisdn": [], "imeisv": [], "mme_host": [], "mme_realm": [], "purge_flag": [],
 "security": {"k": "00112233445566778899AABBCCDDEEFF", "op": null, "opc": "63BFA50EE6523365FF14C1F45F88737D",
              "amf": "8000", "sqn": {"\$numberLong": "64"}},
 "ambr": {"downlink": {"value": 1, "unit": 3}, "uplink": {"value": 1, "unit": 3}},
 "slice": [{"sst": 1, "default_indicator": true,
            "session": [{"name": "internet", "type": 3, "pcc_rule": [],
                         "qos": {"index": 9, "arp": {"priority_level": 8, "pre_emption_capability": 1,
                                                     "pre_emption_vulnerability": 1}},
                         "ambr": {"downlink": {"value": 1, "unit": 3}, "uplink": {"value": 1, "unit": 3}}}]}],
 "access_restriction_data": 32, "subscriber_status": 0, "operator_determined_barring": 0,
 "network_access_mode": 0, "subscribed_rau_tau_timer": 12, "__v": 0}
EOF
  mongoimport --quiet --db open5gs --collection subscribers --file $cfg_dir/subscriber.json || fail "Couldn't add subscriber"

  # Data network interface of the UPF
  ip tuntap add name ogstun mode tun
  ip addr add $sa_gw/16 dev ogstun
  ip link set ogstun up

  for nf in $open5gs_nfs; do
    open5gs-${nf}d -c $cfg_dir/$nf.yaml > /dev/null 2>&1 &
    open5gs_pids="$open5gs_pids $!"
    if [ "$nf" == "nrf" ] || [ "$nf" == "scp" ]; then
      sleep 1
    fi
  done

  for i in $(seq 1 20); do
    if grep -q "ngap_server()" ./${mode}_open5gs_amf.log 2>/dev/null; then
      return
    fi
    sleep 1
  done
  print_log ./${mode}_open5gs_amf.log
  stop_5gc
  fail "AMF didn't start"
}

stop_5gc(){
  for pid in $open5gs_pids; do
    stop_proc $pid open5gs
  done
  open5gs_pids=""
  mongod --shutdown --dbpath $db_dir > /dev/null 2>&1
  rm -rf $db_dir
  chmod a+r ./${mode}_mongod.log
  ip link delete ogstun 2>/dev/null
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
  screen -S $name -dm -L -Logfile ./${mode}_screenlog_${name}.log "$@"
  sleep 2
  start_pid=$(pgrep -x $name)
  if [ -z "$start_pid" ]; then
    echo "Couldn't start $name"
    print_log ./${mode}_screenlog_${name}.log
    kill_5g
    exit 1
  fi
  echo "$name's PID is $start_pid"
}

# Waits up to $2 seconds until the screen log of app $1 contains the string $3
wait_for_log(){
  for i in $(seq 1 $2); do
    if grep -q "$3" ./${mode}_screenlog_$1.log 2>/dev/null; then
      return 0
    fi
    sleep 1
  done
  return 1
}

# Generates the eNB cell configuration from the example, replacing the cell lists
write_rr_conf(){
  sed '/^cell_list =/,$d' $build_path/../srsenb/rr.conf.example > ./${mode}_rr.conf
  if [ "$is_sa" == "1" ]; then
    echo "cell_list = ();" >> ./${mode}_rr.conf
  else
    cat >> ./${mode}_rr.conf <<EOF
cell_list =
(
  {
    rf_port = 0;
    cell_id = 0x01;
    tac = 0x0007;
    pci = 1;
    root_seq_idx = 204;
    dl_earfcn = 300;
    ho_active = false;
    scell_list = ();
    meas_cell_list = ();
    meas_report_desc = ();
  }
);
EOF
  fi
  cat >> ./${mode}_rr.conf <<EOF
nr_cell_list =
(
  {
    rf_port = $nr_rf_port;
    cell_id = 0x02;
    tac = 0x0007;
    pci = 500;
    root_seq_idx = 1;
    dl_arfcn = $nr_dl_arfcn;
    coreset0_idx = $nr_coreset0_idx;
    band = $nr_band;
  }
);
EOF
}

check_ping(){
  loss_rate=$(cat ./${mode}_ping_$1.log | grep -oP '\d+(?=% packet loss)')
  if [ "$loss_rate" != "0" ]; then
    print_log ./${mode}_ping_$1.log
    fail "Detected ${loss_rate:-100} per-cent loss rate in $1 ping."
  fi
}

check_app_exit(){
  last_line=$(cat ./${mode}_screenlog_$1.log | tail -n1)
  if [[ "$last_line" != *"---  exiting  ---"* ]]; then
    print_log ./${mode}_screenlog_$1.log
    fail "$1 didn't seem to have exited properly."
  fi
}

# Checks NR carried user traffic in DL ($1=ue, PDSCH) or UL ($1=enb, PUSCH) without CRC errors
check_nr_traffic(){
  nr_lines=$(grep -E "\[PHY[0-9]*-NR" ./${mode}_$1.log | grep "$2:" | grep "c-rnti")
  num_ok=$(echo "$nr_lines" | grep -c "CRC=OK")
  num_ko=$(echo "$nr_lines" | grep -c "CRC=KO")
  if [ "$num_ok" -lt 5 ] || [ "$num_ko" != "0" ]; then
    fail "Expected NR $2 traffic without errors in $1 logs, found $num_ok OK and $num_ko KO."
  fi
}

check_no_errors(){
  num_error=$(grep "\[E\]" $1 | wc -l)
  if [ "$num_error" != "0" ]; then
    grep -m 10 "\[E\]" $1
    fail "Detected $num_error error messages in $1."
  fi
}

# check arguments
if [ -z "$1" ] || [ -z "$2" ]; then
  print_use
fi
build_path="$1"
mode="$2"

case "$mode" in
  "nsa_fdd")
    is_sa=0; nr_rf_port=1; nr_band=3;  nr_dl_arfcn=368500; nr_coreset0_idx=7;;
  "nsa_tdd")
    is_sa=0; nr_rf_port=1; nr_band=78; nr_dl_arfcn=634240; nr_coreset0_idx=3;;
  "sa_fdd")
    is_sa=1; nr_rf_port=0; nr_band=3;  nr_dl_arfcn=368500; nr_coreset0_idx=6;;
  *)
    echo "Invalid mode $mode."
    print_use;;
esac
echo "Running 5G test in $mode mode"

for app in srsenb/src/srsenb srsue/src/srsue; do
  if [ ! -x "$build_path/$app" ]; then
    fail "$app binary not found or not executable!"
  fi
done

ip netns add $ue_netns || fail "Couldn't add netns. Missing permissions? Network namespace $ue_netns already exists?"

rm -f ./${mode}_*
write_rr_conf

# ZMQ ports, the NR carrier in NSA uses the second channel
base_srate="11.52e6"
if [ "$is_sa" == "1" ]; then
  enb_rf_args="fail_on_disconnect=true,id=enb,base_srate=$base_srate,tx_port0=tcp://*:2000,rx_port0=tcp://localhost:2001"
  ue_rf_args="id=ue,base_srate=$base_srate,tx_port0=tcp://*:2001,rx_port0=tcp://localhost:2000"
else
  enb_rf_args="fail_on_disconnect=true,id=enb,base_srate=$base_srate,tx_port0=tcp://*:2000,rx_port0=tcp://localhost:2001,tx_port1=tcp://*:2100,rx_port1=tcp://localhost:2101"
  ue_rf_args="id=ue,base_srate=$base_srate,tx_port0=tcp://*:2001,rx_port0=tcp://localhost:2000,tx_port1=tcp://*:2101,rx_port1=tcp://localhost:2100"
fi

enb_args="$build_path/../srsenb/enb.conf.example \
          --enb_files.sib_config=$build_path/../srsenb/sib.conf.example \
          --enb_files.rb_config=$build_path/../srsenb/rb.conf.example \
          --enb_files.rr_config=./${mode}_rr.conf \
          --enb.n_prb=50 \
          --rf.device_name=zmq \
          --rf.device_args=$enb_rf_args \
          --log.all_level=info \
          --log.filename=./${mode}_enb.log"

ue_args="$build_path/../srsue/ue.conf.example \
         --rf.device_name=zmq \
         --rf.device_args=$ue_rf_args \
         --rf.srate=$base_srate \
         --rat.nr.nof_carriers=1 \
         --rat.nr.bands=3,78 \
         --rrc.release=15 \
         --stack.have_tti_time_stats=false \
         --gw.netns=$ue_netns \
         --nas.apn=internet \
         --nas.apn_protocol=ipv4 \
         --log.all_level=info \
         --log.filename=./${mode}_ue.log"

if [ "$is_sa" == "1" ]; then
  # srsENB uses the MME address for the AMF
  enb_args="$enb_args --enb.mme_addr=$amf_addr"
  ue_args="$ue_args --rat.eutra.nof_carriers=0"
else
  ue_args="$ue_args --rat.eutra.dl_earfcn=300 --rat.eutra.nof_carriers=1"
fi

# Start the core network
if [ "$is_sa" == "1" ]; then
  start_5gc
  core_gw="$sa_gw"
else
  if [ ! -x "$build_path/srsepc/src/srsepc" ]; then
    fail "srsepc binary not found or not executable!"
  fi
  start_app srsepc $build_path/srsepc/src/srsepc $build_path/../srsepc/epc.conf.example \
            --hss.db_file=$build_path/../srsepc/user_db.csv.example \
            --log.filename=./${mode}_epc.log
  core_pid=$start_pid
  core_gw="172.16.0.1"
fi

start_app srsenb $build_path/srsenb/src/srsenb $enb_args
enb_pid=$start_pid

start_app srsue $build_path/srsue/src/srsue $ue_args
ue_pid=$start_pid

# Wait for the UE to get connected
if [ "$is_sa" == "1" ]; then
  wait_for_log srsue 30 "PDU Session Establishment successful" || { print_log ./${mode}_screenlog_srsue.log; kill_5g; fail "UE didn't establish a PDU session"; }
else
  wait_for_log srsue 30 "Network attach successful" || { print_log ./${mode}_screenlog_srsue.log; kill_5g; fail "UE didn't attach"; }
  wait_for_log srsue 10 "RRC NR reconfiguration successful" || { print_log ./${mode}_screenlog_srsue.log; kill_5g; fail "UE didn't add the NR carrier"; }
fi
ue_ip=$(nsenter --net=/run/netns/$ue_netns ip -4 addr show tun_srsue | grep -oP '(?<=inet )[\d.]+')
echo "UE is connected with IP $ue_ip"
sleep 1

# Run ping tests, the default bearer is mapped to the NR carrier in NSA (QCI 7)
echo "Run UL ping"
nsenter --net=/run/netns/$ue_netns ping -c 5 -i 0.5 $core_gw > ./${mode}_ping_ul.log 2>&1
echo "Run DL ping"
ping -c 5 -i 0.5 $ue_ip > ./${mode}_ping_dl.log 2>&1

kill_5g

# check results
check_ping ul
check_ping dl
check_nr_traffic ue PDSCH
check_nr_traffic enb PUSCH
check_app_exit srsue
check_app_exit srsenb
check_no_errors ./${mode}_ue.log
check_no_errors ./${mode}_enb.log

echo "All tests passed!"
exit 0
