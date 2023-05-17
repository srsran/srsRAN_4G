#!/bin/bash
#
# Copyright 2013-2023 Software Radio Systems Limited
#
# This file is part of srsRAN
#
# srsRAN is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsRAN is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

RELEASE=19.06
DISTRO=disco
COMMIT=eda7ca69a09933526e9318bcf553af0dc0b81598
REL_FLAG=releases

echo 'Packaging srsRAN release' $RELEASE 'for Ubuntu' $DISTRO 

# Make build dir for the package
BUILD_DIR=~/build-area/srsran_$RELEASE/$REL_FLAG/$DISTRO
mkdir -p $BUILD_DIR

# Make tarball of the package source
pushd ~/srsRAN
git archive $COMMIT -o $BUILD_DIR/srsran_$DISTRO.tar.gz
popd

# Copy original tarball
cp ~/build-area/srsran_$RELEASE/$REL_FLAG/srsran_$RELEASE.orig.tar.gz $BUILD_DIR

mkdir $BUILD_DIR/srsRAN
pushd $BUILD_DIR/srsRAN
tar -vxzf ../srsran_$DISTRO.tar.gz
popd
