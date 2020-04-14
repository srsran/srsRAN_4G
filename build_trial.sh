#!/bin/bash

rm -rf sysroot && mkdir sysroot
cmake -DCMAKE_INSTALL_PREFIX="sysroot/" ../
make install
this="$(date +%Y-%m-%d_%H_%M_%S)"
trialdir="srslte_trial_${this}"
tar="srslte.build-${this}.tgz"
mkdir ${trialdir}
tar czf "${trialdir}/$tar" -C "sysroot" .
md5sum "${trialdir}/$tar" >>${trialdir}/checksums.md5

echo "Built ${trialdir}"

