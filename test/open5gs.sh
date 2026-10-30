#!/bin/bash

# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

#
# Helpers to run Open5GS as EPC or 5GC in the end-to-end tests, sourced by the test scripts.
# The caller provides fail(), print_log() and stop_proc().
#

open5gs_gw="10.45.0.1"
open5gs_epc_nfs="nrf scp mme sgwc sgwu smf upf hss pcrf"
open5gs_5gc_nfs="nrf scp amf smf upf ausf udm udr pcf nssf bsf"
open5gs_pids=""
open5gs_db_dir=""

# Starts MongoDB and the given Open5GS network functions, with PLMN 00101, TAC 7 and the default srsUE USIM as subscriber
# $1: prefix of the configuration and log files
# $2: network function whose log signals the core is ready (mme or amf)
# $3: string in that log that signals the core is ready
# $4: list of network functions
start_open5gs(){
  local prefix=$1
  local ready_nf=$2
  local ready_str=$3
  local nfs=$4
  echo "Starting Open5GS"
  local cfg_dir=$PWD/${prefix}_open5gs
  mkdir -p $cfg_dir
  # Keep the database out of the log directory, MongoDB creates its files only readable by the user running the container,
  # which differs from the user uploading the logs in CI
  open5gs_db_dir=$(mktemp -d)
  cp /etc/open5gs/*.yaml $cfg_dir/
  sed -i -e 's/mcc: 999/mcc: 001/' -e 's/mnc: 70/mnc: 01/' -e 's/tac: 1$/tac: 7/' \
         -e "s|/var/log/open5gs/\(.*\)\.log|$PWD/${prefix}_open5gs_\\1.log|" $cfg_dir/*.yaml

  mongod --fork --quiet --bind_ip 127.0.0.1 --dbpath $open5gs_db_dir --logpath ./${prefix}_mongod.log > /dev/null || fail "Couldn't start MongoDB"
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
  ip addr add $open5gs_gw/16 dev ogstun
  ip link set ogstun up

  for nf in $nfs; do
    open5gs-${nf}d -c $cfg_dir/$nf.yaml > /dev/null 2>&1 &
    open5gs_pids="$open5gs_pids $!"
    if [ "$nf" == "nrf" ] || [ "$nf" == "scp" ]; then
      sleep 1
    fi
  done

  for i in $(seq 1 20); do
    if grep -q "$ready_str" ./${prefix}_open5gs_${ready_nf}.log 2>/dev/null; then
      return
    fi
    sleep 1
  done
  print_log ./${prefix}_open5gs_${ready_nf}.log
  stop_open5gs $prefix
  fail "Open5GS $ready_nf didn't start"
}

# Stops the Open5GS network functions and MongoDB
# $1: prefix of the configuration and log files
stop_open5gs(){
  for pid in $open5gs_pids; do
    stop_proc $pid open5gs
  done
  open5gs_pids=""
  if [ -n "$open5gs_db_dir" ]; then
    mongod --shutdown --dbpath $open5gs_db_dir > /dev/null 2>&1
    rm -rf $open5gs_db_dir
    open5gs_db_dir=""
  fi
  chmod a+r ./${1}_mongod.log 2>/dev/null
  ip link delete ogstun 2>/dev/null
}
