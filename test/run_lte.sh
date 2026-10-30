#!/bin/bash

# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

epc_pid=0
enb_pid=0
ue_pid=0

print_use(){
  echo "Please call script with srsRAN build path as first argument and number of PRBs as second (number of component carriers, ciphering algorithm and transmission mode are optional)"
  echo "E.g. ./run_lte.sh [build_path] [nof_prb] [num_cc] [EEA0|EEA1|EEA2|EEA3] [TM1|TM2|TM3|TM4]"
  exit -1
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
  # Allow to gracefully exit
  stop_proc "$ue_pid" UE
  stop_proc "$enb_pid" eNB
  stop_proc "$epc_pid" EPC

  if [ -f ./srsRAN.backtrace.crash ]; then
    echo "Rename backtrace"
    mv ./srsRAN.backtrace.crash srsRAN.backtrace.log
  fi

  # Delete netns
  ip netns delete $ue_netns

  # Don't exit with return code ..
}

print_log()
{
  echo "---- Last lines of $1 ----"
  tail -n 30 "$1" 2>/dev/null
  echo "----"
}

valid_num_prb()
{
    case "$1" in
    "6"|"15"|"25"|"50"|"75"|"100")
        return 0;;
    *)
        echo "Invalid number of PRBs. Possible values are [6, 15, 25, 50, 75, 100]."
        return 1;;
    esac
}

# Generates the eNB cell configuration from the example, with one cell per component carrier, all of them being
# SCells of each other
write_rr_conf()
{
  sed '/^cell_list =/,$d' $build_path/../srsenb/rr.conf.example > ./${nof_prb}prb_rr.conf
  echo "cell_list = (" >> ./${nof_prb}prb_rr.conf
  for cc in $(seq 0 $((num_cc - 1))); do
    local scells=""
    for scc in $(seq 0 $((num_cc - 1))); do
      if [ "$scc" != "$cc" ]; then
        scells="$scells${scells:+, }{ cell_id = $scc; cross_carrier_scheduling = false; scheduling_cell_id = $scc; ul_allowed = true; }"
      fi
    done
    [ "$cc" != "0" ] && echo "," >> ./${nof_prb}prb_rr.conf
    cat >> ./${nof_prb}prb_rr.conf <<EOF
  {
    rf_port = $cc;
    cell_id = $cc;
    tac = 0x0007;
    pci = $((cc + 1));
    root_seq_idx = $((204 + cc));
    dl_earfcn = ${dl_earfcn[$cc]};
    ho_active = false;
    scell_list = ($scells);
    meas_cell_list = ();
    meas_report_desc = ();
  }
EOF
  done
  echo ");" >> ./${nof_prb}prb_rr.conf
  echo "nr_cell_list = ();" >> ./${nof_prb}prb_rr.conf
}

# Generates the ZMQ device arguments of the eNB ($1=enb) or UE ($1=ue), with one channel per carrier and antenna port
zmq_args()
{
  local args="id=$1,base_srate=${base_srate}"
  [ "$1" == "enb" ] && args="fail_on_disconnect=true,$args"
  for cc in $(seq 0 $((num_cc - 1))); do
    for port in $(seq 0 $((nof_ports - 1))); do
      local ch=$((cc * nof_ports + port))
      local dl_port=$((2000 + 2 * ch))
      local ul_port=$((2001 + 2 * ch))
      if [ "$1" == "enb" ]; then
        args="$args,tx_port$ch=tcp://*:$dl_port,rx_port$ch=tcp://localhost:$ul_port,tx_freq$ch=${dl_freq[$cc]},rx_freq$ch=${ul_freq[$cc]}"
      else
        args="$args,tx_port$ch=tcp://*:$ul_port,rx_port$ch=tcp://localhost:$dl_port,tx_freq$ch=${ul_freq[$cc]},rx_freq$ch=${dl_freq[$cc]}"
      fi
    done
  done
  echo $args
}

check_ping()
{
  loss_rate=$(cat ./${nof_prb}prb_screenlog_ping_ul.log | grep -oP '\d+(?=% packet loss)')
  if [ "$loss_rate" != "0" ] 2>/dev/null; then
    echo "Error. Detected $loss_rate per-cent loss rate in UL ping."
    exit 1
  fi

  loss_rate=$(cat ./${nof_prb}prb_screenlog_ping_dl.log | grep -oP '\d+(?=% packet loss)')
  if [ "$loss_rate" != "0" ] 2>/dev/null; then
    echo "Error. Detected $loss_rate per-cent loss rate in DL ping."
    exit 1
  fi
}

check_ue()
{
  # Check the selected ciphering algorithm was applied for NAS and RRC
  eea_name=$eea
  if [ "$eea" != "EEA0" ]; then
    eea_name="128-$eea"
  fi
  num_smc=$(cat ./${nof_prb}prb_ue.log | grep "Received Security Mode Command" | grep "eea: $eea_name," | wc -l)
  if [ "$num_smc" != "2" ] 2>/dev/null; then
    echo "Error. Expected NAS and RRC Security Mode Command with $eea_name in UE logs, found $num_smc."
    exit 1
  fi

  # check that app exited correctly
  last_line=$(cat ./${nof_prb}prb_screenlog_srsue.log | tail -n1)
  if [[ "$last_line" != *"---  exiting  ---"* ]]; then
    echo "Error. srsUE didn't seem to have exited properly."
    exit 1
  fi
  
  # Check UE logs don't contain Error from any layer
  num_error=$(cat ./${nof_prb}prb_ue.log | grep "\[E\]" | wc -l)
  if [ "$num_error" != "0" ] 2>/dev/null; then
    echo "Error. Detected $num_error error messages in UE logs. We should finish without any error."
    exit 1
  fi

  # Check UE logs don't contain Warning from any layer
  num_warning=$(cat ./${nof_prb}prb_ue.log | grep "\[W\]" | wc -l)
  if [ "$num_warning" != "0" ] 2>/dev/null; then
    echo "Error. Detected $num_warning warning messages in UE logs. We should finish without any."
    exit 1
  fi

  # Check PDSCH
  num_error=$(cat ./${nof_prb}prb_ue.log | grep -E "crc=\{?([A-Z]+/)?KO" | wc -l)
  if [ "$num_error" != "0" ] 2>/dev/null; then
    echo "Error. Detected $num_error PDSCH errors in UE logs. We should finish without any error."
    exit 1
  fi

  # Check PUCCH ack
  num_error=$(cat ./${nof_prb}prb_ue.log | grep "PUCCH" | grep "ack=0" | wc -l)
  if [ "$num_error" != "0" ] 2>/dev/null; then
    echo "Error. Detected $num_error PUCCH errors in UE logs. We should finish without any error."
    exit 1
  fi

  # Check PHICH
  num_error=$(cat ./${nof_prb}prb_ue.log | grep "hi=0" | wc -l)
  if [ "$num_error" != "0" ] 2>/dev/null; then
    echo "Error. Detected $num_error PHICH errors in UE logs. We should finish without any error."
    exit 1
  fi

  # Check CQI is 15
  num_error=$(cat ./${nof_prb}prb_ue.log | grep "PUCCH" | grep "cqi=" | grep -v "cqi=15" | wc -l)
  if [ "$num_error" != "0" ] 2>/dev/null; then
    echo "Error. Detected $num_error CQI != 15 in UE logs. We should finish with all cqi=15."
    exit 1
  fi

  # Check number of PRACHs
  num_prach=$(cat ./${nof_prb}prb_ue.log | grep "preamble=" | wc -l)
  if [ "$num_prach" != "1" ] 2>/dev/null; then
    echo "Error. Detected $num_prach PRACH(s). But should be only 1."
    exit 1
  fi

  # Check the SCell carried user data
  if [ "$num_cc" == "2" ]; then
    num_pdsch=$(grep "PDSCH: cc=1" ./${nof_prb}prb_ue.log | grep -c "crc={OK")
    if [ "$num_pdsch" -lt 10 ] 2>/dev/null; then
      echo "Error. Detected $num_pdsch PDSCH on the SCell in UE logs. Expected at least 10."
      exit 1
    fi
  fi

  # Check the multi-antenna transmission scheme was used, i.e. transmit diversity (TM2) or two TBs in the same PDSCH
  # (spatial multiplexing in TM3 and TM4)
  if [ "$tm" == "TM2" ]; then
    num_pdsch=$(grep "PDSCH:" ./${nof_prb}prb_ue.log | grep -v "rnti=0xffff\|rnti=0x2," | grep "tx=div" | grep -c "crc={OK}")
    if [ "$num_pdsch" -lt 10 ] 2>/dev/null; then
      echo "Error. Detected $num_pdsch PDSCH with transmit diversity in UE logs. Expected at least 10."
      exit 1
    fi
  elif [ "$tm" != "TM1" ]; then
    num_pdsch=$(grep "PDSCH:" ./${nof_prb}prb_ue.log | grep "nof_tb=2" | grep -c "crc={OK/OK}")
    if [ "$num_pdsch" -lt 10 ] 2>/dev/null; then
      echo "Error. Detected $num_pdsch PDSCH with two TBs in UE logs. Expected at least 10."
      exit 1
    fi
  fi

  # Check duplicate TB at MAC
  num_error=$(cat ./${nof_prb}prb_ue.log | grep "duplicate" | wc -l)
  if [ "$num_error" != "0" ] 2>/dev/null; then
    echo "Error. Detected $num_error duplicate TBs in UE logs. We should finish without any duplicate."
    exit 1
  fi
}

check_enb()
{
  # check that app exited correctly
  last_line=$(cat ./${nof_prb}prb_screenlog_srsenb.log | tail -n1)
  if [[ "$last_line" != *"---  exiting  ---"* ]]; then
    echo "Error. srsENB didn't seem to have exited properly."
    exit 1
  fi

  # Check PRACH results
  num_prach=$(cat ./${nof_prb}prb_screenlog_srsenb.log | grep RACH: | wc -l)
  if [ "$num_prach" != "1" ] 2>/dev/null; then
    echo "Error. Detected $num_prach PRACH(s). But should be only 1."
    exit 1
  fi

  # check [E]
  num_error=$(cat ./${nof_prb}prb_enb.log | grep "\[E\]" | wc -l)
  if [ "$num_error" != "0" ] 2>/dev/null; then
    echo "Error. Detected $num_error error messages in eNB logs. We should finish without any error."
    exit 1
  fi

  # Check [W]
  num_warning=$(cat ./${nof_prb}prb_enb.log | grep "\[W\]" | wc -l)
  if [ "$num_warning" != "0" ] 2>/dev/null; then
    echo "Error. Detected $num_warning warning messages in eNB logs. We should finish without any."
    exit 1
  fi

  # Check for PUSCH errors
  num_error=$(cat ./${nof_prb}prb_enb.log | grep -E "crc=\{?([A-Z]+/)?KO" | wc -l)
  if [ "$num_error" != "0" ] 2>/dev/null; then
    echo "Error. Detected $num_error PUSCH errors in eNB logs. We should finish without any error."
    exit 1
  fi

  # Check PUCCH ack
  num_error=$(cat ./${nof_prb}prb_enb.log | grep "PUCCH" | grep "ack=0" | wc -l)
  if [ "$num_error" != "0" ] 2>/dev/null; then
    echo "Error. Detected $num_error PUCCH errors in eNB logs. We should finish without any error."
    exit 1
  fi

  # Check CQI is 15 (ignore PUCCH occasions where nothing was detected, e.g. right after connection setup)
  num_error=$(cat ./${nof_prb}prb_enb.log | grep "PUCCH" | grep "cqi=" | grep -v "corr=0.000" | grep -v "cqi=15" | wc -l)
  if [ "$num_error" != "0" ] 2>/dev/null; then
    echo "Error. Detected $num_error CQI != 15 in eNB logs. We should finish with all qci=15."
    exit 1
  fi
}

check_epc()
{
  # check that app exited correctly
  last_line=$(cat ./${nof_prb}prb_screenlog_srsepc.log | tail -n1)
  if [[ "$last_line" != *"---  exiting  ---"* ]]; then
    echo "Error. srsEPC didn't seem to have exited properly."
    exit 1
  fi

  # check [E]
  num_error=$(cat ./${nof_prb}prb_epc.log | grep "\[E\]" | wc -l)
  if [ "$num_error" != "0" ] 2>/dev/null; then
    echo "Error. Detected $num_error error messages in EPC logs. We should finish without any error."
    exit 1
  fi
}

# check if build path has been passed
if ([ ! $1 ])
then
  print_use
  exit
fi
build_path="$1"

# check number of PRBs
if ([ ! $2 ])
then
  print_use
  exit
fi
nof_prb="$2"

# check number of CC
num_cc="1"
if ([ $3 ])
then
  num_cc="$3"
fi
echo "Using $num_cc component carrier(s) in srsENB"

# check ciphering algorithm (used for NAS and AS)
eea="EEA0"
if ([ $4 ])
then
  eea="$4"
fi
case "$eea" in
  "EEA0"|"EEA1"|"EEA2"|"EEA3")
    echo "Using $eea ciphering";;
  *)
    echo "Invalid ciphering algorithm $eea. Possible values are [EEA0, EEA1, EEA2, EEA3]."
    exit 1;;
esac

# check transmission mode, TM2 to TM4 use two antenna ports at the eNB and two antennas at the UE
tm="TM1"
if ([ $5 ])
then
  tm="$5"
fi
case "$tm" in
  "TM1")
    nof_ports="1";;
  "TM2"|"TM3"|"TM4")
    nof_ports="2";;
  *)
    echo "Invalid transmission mode $tm. Possible values are [TM1, TM2, TM3, TM4]."
    exit 1;;
esac
echo "Using $tm with $nof_ports antenna port(s)"

# Carrier frequencies, the second carrier is used as SCell
dl_earfcn=(2850 2910)
dl_freq=(2630e6 2636e6)
ul_freq=(2510e6 2516e6)
if [ "$num_cc" != "1" ] && [ "$num_cc" != "2" ]; then
  echo "Invalid number of component carriers $num_cc. Possible values are [1, 2]."
  exit 1
fi

base_srate="23.04e6"
if ([ "$nof_prb" == "75" ])
then
  base_srate="15.36e6"
fi

if ([ "$nof_prb" == "15" ])
then
  base_srate="3.84e6"
fi

# Check for LTE binaries in build path
if [ ! -x "$build_path/srsenb/src/srsenb" ]; then
  echo "Error! srsENB binary not found or not executable!"
  exit 1
fi

if [ ! -x "$build_path/srsepc/src/srsepc" ]; then
  echo "Error! srsEPC binary not found or not executable!"
  exit 1
fi

if [ ! -x "$build_path/srsue/src/srsue" ]; then
  echo "Error! srsUE binary not found or not executable!"
  exit 1
fi

# Check PRB parameter
if ! valid_num_prb "$nof_prb"; then
  exit 1
fi

# Adding netns
ue_netns="ue1"
ip netns add $ue_netns
if [ $? -ne 0 ]
then
  echo "Couldn't add netns. Missing permissions? Network namespace $ue_netns already exists?"
  exit 1
fi

epc_args="$build_path/../srsepc/epc.conf.example \
          --hss.db_file=$build_path/../srsepc/user_db.csv.example \
          --mme.encryption_algo=$eea \
          --log.filename=./${nof_prb}prb_epc.log"
enb_args="$build_path/../srsenb/enb.conf.example \
          --enb_files.sib_config=$build_path/../srsenb/sib.conf.example \
          --enb_files.rb_config=$build_path/../srsenb/rb.conf.example \
          --rf.device_name=zmq \
          --log.all_level=info \
          --enb.n_prb=$nof_prb \
          --expert.eea_pref_list=$eea \
          --log.filename=./${nof_prb}prb_enb.log"

ue_args="$build_path/../srsue/ue.conf.example \
         --rf.device_name=zmq \
         --rf.rx_gain 60 \
         --stack.have_tti_time_stats=false \
         --gw.netns=$ue_netns \
         --log.all_level=info \
         --log.filename=./${nof_prb}prb_ue.log \
         --pcap.enable=mac \
         --pcap.mac_filename=./${nof_prb}prb_ue.pcap"

write_rr_conf
enb_args="$enb_args --enb_files.rr_config=./${nof_prb}prb_rr.conf \
          --enb.tm=${tm#TM} --enb.nof_ports=$nof_ports \
          --rf.device_args=$(zmq_args enb)"
ue_args="$ue_args --rat.eutra.dl_earfcn=$(echo ${dl_earfcn[@]:0:$num_cc} | tr ' ' ',') --rat.eutra.nof_carriers=$num_cc \
         --rf.nof_antennas=$nof_ports \
         --rf.device_args=$(zmq_args ue)"
if ([ "$num_cc" == "2" ])
then
  ue_args="$ue_args --rrc.ue_category=7 --rrc.release=10"
fi

# Remove existing log files
log_files=$(ls -l | grep ${nof_prb}prb_)
if [ ! -z "$log_files" ]; then
  echo "Deleting existing log files for ${nof_prb} PRB."
  eval "rm -Rf ${nof_prb}prb_*.log"
fi

# Run srsEPC
echo "Starting srsEPC"
screen -S srsepc -dm -L -Logfile ./${nof_prb}prb_screenlog_srsepc.log $build_path/srsepc/src/srsepc $epc_args
sleep 2
epc_pid=$(pgrep srsepc)
if [ -z "$epc_pid" ]
then
  echo "Couldn't start EPC."
  print_log ./${nof_prb}prb_screenlog_srsepc.log
  kill_lte
  exit 1
fi
echo "srsEPC's PID is $epc_pid"

# Run srsENB
echo "Starting srsENB"
screen -S srsenb -dm -L -Logfile ./${nof_prb}prb_screenlog_srsenb.log $build_path/srsenb/src/srsenb $enb_args
sleep 2
enb_pid=$(pgrep srsenb)
if [ -z "$enb_pid" ]
then
  echo "Couldn't start ENB"
  print_log ./${nof_prb}prb_screenlog_srsenb.log
  kill_lte
  exit 1
fi
echo "srsENB's PID is $enb_pid"

# Run srsUE
echo "Starting srsUE"
screen -S srsue -dm -L -Logfile ${nof_prb}prb_screenlog_srsue.log $build_path/srsue/src/srsue $ue_args
sleep 2
ue_pid=$(pgrep srsue)
if [ -z "$ue_pid" ]
then
  echo "Couldn't start UE"
  print_log ./${nof_prb}prb_screenlog_srsue.log
  kill_lte
  exit 1
fi
echo "srsUE's PID is $ue_pid"


# The UE PHY initialization can take more than 20s on slow machines, so wait for it before timing the attach
echo "Waiting for srsUE to initialize .."
for i in $(seq 1 60); do
  grep -q "Attaching UE" ./${nof_prb}prb_screenlog_srsue.log 2>/dev/null && break
  sleep 1
done

# Wait until UE is connected
timeout=20
tun_dev="tun_srsue"

echo "Waiting for $tun_dev to become available for ${timeout}s .."
wait_time=0
ip=$(nsenter --net=/run/netns/$ue_netns ip addr show $tun_dev | grep "inet\b" | awk '{print $2}' | cut -d/ -f1)
while [ -z $ip ] && [ $wait_time -ne $timeout ]
do
  # Wait 1s before next attempt
  sleep 1
  let wait_time++
  ip=$(nsenter --net=/run/netns/$ue_netns ip addr show $tun_dev | grep "inet\b" | awk '{print $2}' | cut -d/ -f1)
done

if [ ! -z $ip ]; then
  echo "$tun_dev is up with IP $ip!"
  sleep 1
else
  echo "$tun_dev didn't get up after ${timeout}s"
  kill_lte
  exit 1
fi

# extract MME IP
baseip=`echo $ip | cut -d"." -f1-3`
mme_ip=$baseip".1"

echo "MME IP is $mme_ip"

# show UE/eNB console trace
screen -S srsue -X stuff "t$(printf \\r)"
screen -S srsenb -X stuff "t$(printf \\r)"

# Run tests now

# run UL ping test
echo "Run UL ping"
nsenter --net=/run/netns/$ue_netns screen -dm -L -Logfile ./${nof_prb}prb_screenlog_ping_ul.log ping $mme_ip -c 3
sleep 4

# sleep 3s to get RRC connection release
echo "Run DL ping"
screen -dm -L -Logfile ./${nof_prb}prb_screenlog_ping_dl.log ping $ip -c 3
sleep 4

# run UDP DL (rate must not be more than max DL rate for 6 PRB). With CA or MIMO the rate must exceed what a single
# carrier and layer carries with one TB, so that the SCell and the second TB get scheduled
dl_rate="1M"
if [ "$num_cc" != "1" ] || [ "$nof_ports" != "1" ]; then
  dl_rate="$((nof_prb * 6 / 10))M"
fi
echo "Run DL iperf with $dl_rate"
screen -dm -L -Logfile ./${nof_prb}prb_screenlog_iperf_dl.log iperf -c $ip -u -t 2 -i 1 -b $dl_rate
sleep 4

# Stop all running LTE components and remove netns
kill_lte

# check results
check_ping
check_ue
check_enb
check_epc

echo "All tests passed!"

exit 0
