#!/bin/bash
#
# Copyright 2013-2020 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

RELEASE=19.06
DISTRO=disco
COMMIT=eda7ca69a09933526e9318bcf553af0dc0b81598
REL_FLAG=releases

echo 'Packaging srsLTE release' $RELEASE 'for Ubuntu' $DISTRO 

# Make build dir for the package
BUILD_DIR=~/build-area/srslte_$RELEASE/$REL_FLAG/$DISTRO
mkdir -p $BUILD_DIR

# Make tarball of the package source
pushd ~/srsLTE
git archive $COMMIT -o $BUILD_DIR/srslte_$DISTRO.tar.gz
popd

# Copy original tarball
cp ~/build-area/srslte_$RELEASE/$REL_FLAG/srslte_$RELEASE.orig.tar.gz $BUILD_DIR

mkdir $BUILD_DIR/srsLTE
pushd $BUILD_DIR/srsLTE
tar -vxzf ../srslte_$DISTRO.tar.gz
popd
