#!/bin/bash

# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: AGPL-3.0-or-later

# Generates the code of the IPL4 translation ports (IPL4_<name>_Definitions.ttcn): the test port skeletons with the
# Titan compiler, they don't need any custom code, and the control functions from the template of the upstream IPL4asp
# test port. Needs to run in the build directory after gen_links.sh.

set -e

TEMPLATE_DIR=../TestPorts/titan.TestPorts.IPL4asp/demo/TranslationPortUserCtrl

for defs in IPL4_*_Definitions.ttcn; do
  port=$(sed -n 's/^type port \([A-Za-z0-9_]*\) message map to IPL4asp_PT.*/\1/p' $defs)
  if [ -z "$port" ]; then
    continue
  fi
  defs_module=${defs%.ttcn}
  ctrl_module=${defs_module%_Definitions}_User_CtrlFunct
  echo "Generating $ctrl_module for $port"

  sed -e "s/<user types module>/$defs_module/g" \
      -e "s/<user port type>/$port/g" \
      -e "s/IPL4asp_User_CtrlFunct/$ctrl_module/g" \
      $TEMPLATE_DIR/IPL4asp_User_CtrlFunct.ttcn > $ctrl_module.ttcn

  # Titan doubles the underscores of TTCN-3 names in C++, except in the file names of the include
  sed -e "s/\"<user types module>.hh\"/\"$defs_module.hh\"/" \
      -e "s/<user types module>/${defs_module//_/__}/g" \
      -e "s/<user port type>/${port//_/__}/g" \
      -e "s/IPL4asp__User__CtrlFunct/${ctrl_module//_/__}/g" \
      $TEMPLATE_DIR/IPL4asp_User_CtrlFunctDef.cc > ${ctrl_module}Def.cc
done

# Test port skeletons, only generate code for the IPL4 modules, but the compiler needs all modules for the semantic check
rm -rf skeletons
mkdir skeletons
compiler -t -o skeletons *.ttcn *.asn - IPL4_*_Definitions.ttcn
mv skeletons/*_PT.cc skeletons/*_PT.hh .
rm -rf skeletons
