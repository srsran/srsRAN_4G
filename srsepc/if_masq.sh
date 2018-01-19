#/bin/bash

###################################################################
#
# This file is part of srsLTE.
#
# srsLTE is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsLTE is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#
###################################################################

#Check for sudo rights
sudo -v || exit 

#Check if outbound interface was specified
if [ ! $# -eq 1 ]
  then
    echo "Usage :'sudo ./if_masq.sh <Interface Name>' "
    exit
fi

echo "Masquerading Interface "$1

echo 1 | sudo tee /proc/sys/net/ipv4/ip_forward 1>/dev/null
sudo iptables -t nat -A POSTROUTING -o $1 -j MASQUERADE

