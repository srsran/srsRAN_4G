#!/bin/sh

# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: AGPL-3.0-or-later

sleep 1
echo "Starting TC $1"
echo "{ \"Cmd\": {\"TC_START\": { \"Name\" : \"$1\" } } }" | ncat --sctp localhost 2222
sleep 1
