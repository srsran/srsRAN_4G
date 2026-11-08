#!/bin/bash

# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: AGPL-3.0-or-later

set -e

TESTSUITE_FILE="36523-3-860.zip"
TESTSUITE_URL="https://www.3gpp.org/ftp/Specs/archive/36_series/36.523-3/$TESTSUITE_FILE"
TESTSUITE_SHA256="f9af76797cd5adce0c78ce77c6a2ee91d84b21aabeae114ab85a7e50fa262ddc"
# A local copy of the archive can be used instead of downloading it, e.g. ./get_testsuite.sh ~/Downloads/36523-3-860.zip
TESTSUITE_ARCHIVE="$1"
TESTSUITE_PATCH="ue_testsuite_diff/latest.diff"
TESTSUITE_BASEDIR=./ue_testsuite
TMP_DIR=./tmp/

# Check if folder already exist
if [ -d "$TESTSUITE_BASEDIR" ]; then
  echo "$TESTSUITE_BASEDIR already exist. Not overwriting existing filed. Exiting"
  exit
fi

# Now temp folder
if [ -d "$TMP_DIR" ]; then
  echo "$TMP_DIR already exist. Not overwriting existing filed. Exiting"
  exit
fi

echo "Creating testsuite folder"
mkdir $TESTSUITE_BASEDIR

echo "Creating temporary folder"
mkdir $TMP_DIR

if [ -z "$TESTSUITE_ARCHIVE" ]; then
  echo "Downloading $TESTSUITE_URL"
  curl -fsSL -o $TMP_DIR/$TESTSUITE_FILE $TESTSUITE_URL
else
  echo "Copying archive to temp dir"
  cp $TESTSUITE_ARCHIVE $TMP_DIR/$TESTSUITE_FILE
fi
echo "$TESTSUITE_SHA256  $TMP_DIR/$TESTSUITE_FILE" | sha256sum -c -

# extract archive
echo "Extracting files ..."
unzip $TMP_DIR/$TESTSUITE_FILE -d $TMP_DIR

# Delete downloaded ZIP to only leave extracted one
rm $TMP_DIR/$TESTSUITE_FILE
unzip $TMP_DIR/*.zip -d $TMP_DIR

echo "Copying files ..."
cp -R $TMP_DIR/*/* $TESTSUITE_BASEDIR

echo "Patching files"
patch -d $TESTSUITE_BASEDIR -p1 < $TESTSUITE_PATCH

echo "Generating test cases ..."
mkdir -p $TESTSUITE_BASEDIR/srsRAN
awk -f gen_conf_testcases.awk $TESTSUITE_BASEDIR/LTE_EPS_TS_Testcases.ttcn > $TESTSUITE_BASEDIR/srsRAN/LTE_EPS_TS_Testcases.ttcn

echo "Deleting $TMP_DIR"
rm -rf $TMP_DIR
