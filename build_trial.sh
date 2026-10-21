#!/bin/bash

# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

rm -rf sysroot && mkdir sysroot
cmake -DCMAKE_INSTALL_PREFIX="sysroot/" ../
make install
this="$(date +%Y-%m-%d_%H_%M_%S)"
trialdir="srsran_trial_${this}"
tar="srsran.build-${this}.tgz"
mkdir ${trialdir}
tar czf "${trialdir}/$tar" -C "sysroot" .
md5sum "${trialdir}/$tar" >>${trialdir}/checksums.md5

echo "Built ${trialdir}"

