#!/bin/bash

# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: AGPL-3.0-or-later

set -e

if [ -z "$PARALLEL_MAKE" ]; then
	if [ `which nproc` ]; then
		NPROC=`nproc`
	else
		echo "WARNING: No nproc binary (coreutils) found, disabling parallel build"
		NPROC=1
	fi
	PARALLEL_MAKE="-j$NPROC"
fi

# link all source files to build directory
../gen_links.sh

# generate the test port skeletons and control functions of the IPL4 translation ports
../gen_ipl4_ports.sh

# [re]generate the makefile
../regen-makefile.sh *.ttcn *.asn *.cc

# generate C++ from TTCN-3
make compile

# compile the C++ into executable code
make $PARALLEL_MAKE
