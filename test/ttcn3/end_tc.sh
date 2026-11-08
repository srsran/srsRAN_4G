#!/bin/sh

# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: AGPL-3.0-or-later

sleep 1
echo "Finished TC $1 with result $2"
echo "{ \"Cmd\": {\"TC_END\": { \"Name\": \"$1\", \"Result\" : \"$2\" } } }" | ncat --sctp localhost 2222
sleep 1
