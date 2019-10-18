#!/bin/bash
# Copyright 2013-2019 Software Radio Systems Limited
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
 
# make sure all commands are echoed
#set -x
set -o pipefail

if ([ ! $1 ])
then
  echo "Please call script with target branch name or git hash to perform diff with."
  echo "E.g. ./run-clang-format-diff.sh [HASH]"
  exit 1
fi

# check for apps
app1=$(which clang-format-diff)
app2=$(which git)
app3=$(which colordiff)
if ([ ! -x "$app1" ] || [ ! -x "$app2" ] || [ ! -x "$app3" ])
then
  echo "Please install clang-format-diff, git and colordiff"
  exit 1
fi

target=$1

echo "Running code format check between ${target} and ${commit} .."

# run clang-format
diff="$(git diff -U0 ${target} | clang-format-diff -p1 | python3 -c 'data = open(0).read(); print(data); exit(1 if data else 0)')"

# safe return code
ret=$?

if ([ $ret == "0" ])
then
  echo "Code formatting is correct."
else
  echo "The following code seems to be not formatted correctly:"
  # print colorized version
  echo "${diff}" | colordiff
fi

exit ${ret}
