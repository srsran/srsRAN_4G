#!/bin/sh

# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: AGPL-3.0-or-later

#
# Wrapper around the Titan makefile generator, based on Osmocom's regen-makefile.
# Needs TTCN3_DIR to point to the Titan installation.
#

test -x "$(which ttcn3_makefilegen 2>/dev/null)" || { echo "ERROR: ttcn3_makefilegen not in PATH"; exit 1; }
test -n "$TTCN3_DIR" || { echo "ERROR: TTCN3_DIR not set"; exit 1; }

ttcn3_makefilegen -l -f $*

# Name of the executable, by default it's named after the first module
sed -i -e 's/^EXECUTABLE = .*/EXECUTABLE = srslte_ttcn3_titan/' Makefile

# The IPL4 test port is used with SCTP, the security algorithms come from srsRAN
sed -i -e 's/^CPPFLAGS = \(.*\)$/CPPFLAGS = \1 -DUSE_SCTP -I..\/..\/..\/lib\/include/' Makefile

# mbedTLS for the security functions in EUTRA_SecurityHelpers.cc
sed -i -e 's/-lcrypto/-lcrypto -lmbedcrypto/g' Makefile
