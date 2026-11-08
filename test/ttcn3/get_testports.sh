#!/bin/bash

# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: AGPL-3.0-or-later

# Fetches the upstream Eclipse Titan test ports used by the tester, pinned to known good commits

set -e

TESTPORTS_DIR=./TestPorts
TESTPORTS_URL=https://gitlab.eclipse.org/eclipse/titan
TESTPORTS="titan.TestPorts.IPL4asp:a4d5c2992368bc7b702759887e54b64a44c54de0
           titan.TestPorts.Common_Components.Socket-API:37f8b1f04c0d6627bf5dbaf3b533bc5606f19b9c
           titan.TestPorts.Common_Components.Abstract_Socket:f6b2d2ba3f486dea778e108ce60e02a3fcae9ff8"

mkdir -p $TESTPORTS_DIR
for testport in $TESTPORTS; do
  name=${testport%%:*}
  commit=${testport##*:}
  if [ -d "$TESTPORTS_DIR/$name" ]; then
    echo "$TESTPORTS_DIR/$name already exists, not fetching it again"
    continue
  fi
  echo "Fetching $name at $commit"
  git init -q $TESTPORTS_DIR/$name
  git -C $TESTPORTS_DIR/$name fetch -q --depth 1 $TESTPORTS_URL/$name.git $commit
  git -C $TESTPORTS_DIR/$name checkout -q FETCH_HEAD
done
