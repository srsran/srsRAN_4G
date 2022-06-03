#!/bin/bash

#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

#Check for sudo rights
sudo -v || exit 

#Check if outbound interface was specified
if [ ! $# -eq 1 ]
  then
    echo "Usage :'sudo ./srsepc_if_masq.sh <Interface Name>' "
    exit
fi

echo "Masquerading Interface "$1

echo 1 | sudo tee /proc/sys/net/ipv4/ip_forward 1>/dev/null
sudo iptables -t nat -A POSTROUTING -o $1 -j MASQUERADE
