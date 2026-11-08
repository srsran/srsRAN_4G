#!/bin/bash

# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: AGPL-3.0-or-later

# Based on Osmocom's TTCN3 tests

BASEDIR=../ue_testsuite/

gen_links() {
  DIR=$1
  FILES=$*
  for f in $FILES; do
    echo "Linking $f"
    ln -sf $DIR/$f $f
  done
}

DIR=../src
FILES="srsue_conf_testcases.ttcn
       DummyRats.ttcn
       EncodingTest.ttcn
       SS_DUT_TypeDefs.ttcn
       EUTRA_SecurityHelpers.cc
       EUTRA_CommonHelpers.cc
       EUTRA_Types.ttcn
       NAS_Codec_Test.ttcn
       IPL4_Generic_Definitions.ttcn
       IPL4_MTC_SYSTEM_Ut_Definitions.ttcn
       IPL4_EUTRA_SYSTEM_Definitions.ttcn
       IPL4_EUTRA_SYSIND_Definitions.ttcn
       IPL4_EUTRA_DRB_Definitions.ttcn
       IPL4_NASEMU_SYSTEM_Definitions.ttcn
       IPL4_IP_CONTROL_Definitions.ttcn
       IPL4_IP_SOCKET_Definitions.ttcn"
gen_links $DIR $FILES

# Generated from the test suite by get_testsuite.sh
DIR=$BASEDIR/srsRAN
FILES="LTE_EPS_TS_Testcases.ttcn"
gen_links $DIR $FILES

# Security algorithms of srsRAN
DIR=../../../lib/src/common
FILES="liblte_security.cc s3g.cc zuc.cc"
gen_links $DIR $FILES

# External Test Ports
DIR=../TestPorts/titan.TestPorts.IPL4asp/src
FILES="IPL4asp_Types.ttcn IPL4asp_PT.cc IPL4asp_PT.hh IPL4asp_PortType.ttcn"
gen_links $DIR $FILES

DIR=../TestPorts/titan.TestPorts.Common_Components.Socket-API/src
FILES="Socket_API_Definitions.ttcn"
gen_links $DIR $FILES

DIR=../TestPorts/titan.TestPorts.Common_Components.Abstract_Socket/src
FILES="Abstract_Socket.cc Abstract_Socket.hh"
gen_links $DIR $FILES

# 3GPP testcases
DIR=$BASEDIR/6_1
FILES="Idle_CellReselection.ttcn Idle_PLMNSelection.ttcn"
gen_links $DIR $FILES

DIR=$BASEDIR/7_1
FILES="MAC_711.ttcn MAC_712.ttcn MAC_713.ttcn MAC_714.ttcn MAC_716.ttcn MAC_717.ttcn MAC_Functions.ttcn MAC_Templates.ttcn"
gen_links $DIR $FILES

DIR=$BASEDIR/7_2
FILES="RLC_AM_Testcases.ttcn RLC_AM_Templates.ttcn RLC_Common.ttcn RLC_UM_Testcases.ttcn RLC_UM_Templates.ttcn"
gen_links $DIR $FILES

DIR=$BASEDIR/7_3
FILES="PDCP_Testcases.ttcn PDCP_Security.ttcn"
gen_links $DIR $FILES

DIR=$BASEDIR/8_1
FILES="RRC_Paging.ttcn RRC_ConnEst.ttcn RRC_ConnRel.ttcn"
gen_links $DIR $FILES

DIR=$BASEDIR/8_2
FILES="RRC_Handover.ttcn RRC_ConnReconfig.ttcn"
gen_links $DIR $FILES

DIR=$BASEDIR/8_3
FILES="RRC_Measurement.ttcn RRC_Measurement_Functions.ttcn EUTRA_Measurement_Specific_Templates.ttcn"
gen_links $DIR $FILES

DIR=$BASEDIR/8_5
FILES="RRC_Others.ttcn RRC_RadioLinkFailure.ttcn"
gen_links $DIR $FILES

DIR=$BASEDIR/9_1
FILES="NAS_Authentication.ttcn NAS_SecurityMode.ttcn"
gen_links $DIR $FILES

DIR=$BASEDIR/9_2
FILES="NAS_TrackingArea_EOnly.ttcn NAS_Detach.ttcn NAS_Attach_EOnly.ttcn"
gen_links $DIR $FILES

DIR=$BASEDIR/9_3
FILES="NAS_Paging.ttcn NAS_ServiceRequest_EOnly.ttcn"
gen_links $DIR $FILES

DIR=$BASEDIR/9_4
FILES="NAS_Security.ttcn"
gen_links $DIR $FILES

DIR=$BASEDIR/10
FILES="ESM_testcases.ttcn"
gen_links $DIR $FILES

DIR=$BASEDIR/11
FILES="SMS_Templates.ttcn  SMS_Testcases.ttcn"
gen_links $DIR $FILES

DIR=$BASEDIR/12
FILES="DRB_12.ttcn"
gen_links $DIR $FILES

DIR=$BASEDIR/13
FILES="MultiLayer_Procedures.ttcn"
gen_links $DIR $FILES

DIR=$BASEDIR/CommonEUTRA
FILES="EUTRA_CellInfo.ttcn EUTRA_PdcchConfig.ttcn EUTRA_AuxiliaryFunctions.ttcn EUTRA_CellInfoFrequency.ttcn EUTRA_CommonProcedures.ttcn EUTRA_Component.ttcn EUTRA_ConfigurationSteps.ttcn EUTRA_SecurityFunctions.ttcn EUTRA_LoopBack.ttcn EUTRA_IdleMode.ttcn EUTRA_NASSteps.ttcn EUTRA_RRCSteps.ttcn EUTRA_Paging.ttcn EUTRA_SecuritySteps.ttcn EUTRA_Timing.ttcn"
gen_links $DIR $FILES

DIR=$BASEDIR/CommonUTRAN
FILES="UTRAN_ASP_Templates.ttcn UTRAN_CommonDefs.ttcn UTRAN_Component.ttcn UTRAN_Parameters.ttcn UTRAN_RRC_CommonTemplates.ttcn UTRAN_SysInfo_Templates.ttcn  UTRAN_TypeDefs.ttcn UTRAN_ASP_Definitions.asn UTRAN_CellInfo.ttcn UTRAN_CommonFunctions.ttcn UTRAN_ConfigurationSteps.ttcn UTRAN_RRC_ASN1_Definitions.asn UTRAN_RRC_Templates.ttcn UTRAN_Templates.ttcn"
FILES="UTRAN_CommonDefs.ttcn"
gen_links $DIR $FILES

DIR=$BASEDIR/CommonEUTRA_Defs
FILES="EUTRA_RRC_ASN1_Definitions.asn EUTRA_ASP_DrbDefs.ttcn EUTRA_ASP_TypeDefs.ttcn EUTRA_CommonDefs.ttcn EUTRA_Parameters.ttcn EPS_NAS_Constants.ttcn EPS_NAS_MsgContainers.ttcn EPS_NAS_TypeDefs.ttcn EUTRA_ASP_SrbDefs.ttcn EUTRA_LoopBack_TypeDefs.ttcn EUTRA_ASP_NasCtrl.ttcn EUTRA_ExternalSecurityFunctions.ttcn"
gen_links $DIR $FILES

DIR=$BASEDIR/CommonEUTRA_IRAT
FILES="EUTRA_CapabilityFunctions.ttcn EUTRA_IRATFunctions.ttcn"
gen_links $DIR $FILES

DIR=$BASEDIR/CommonIRAT
FILES="NAS_24008Templates.ttcn NAS_24008TypeDefs.ttcn"
gen_links $DIR $FILES

DIR=$BASEDIR/CommonEUTRA_Templates
FILES="EUTRA_AspCommon_Templates.ttcn EUTRA_DRB_Templates.ttcn EUTRA_RRC_Templates.ttcn EUTRA_CellCfg_Templates.ttcn EUTRA_SysInfo_Templates.ttcn EPS_NAS_Templates.ttcn EUTRA_SRB_Templates.ttcn EUTRA_Security_Templates.ttcn EUTRA_Measurement_Templates.ttcn"
gen_links $DIR $FILES

DIR=$BASEDIR/CommonEUTRA_L2
FILES="L2_CommonTemplates.ttcn L2_CommonFunctions.ttcn"
gen_links $DIR $FILES

DIR=$BASEDIR/common
FILES="CommonDefs.ttcn CommonIratDefs.ttcn Parameters.ttcn TestcaseProperties.ttcn CommonIP.ttcn UpperTesterDefs.ttcn UpperTesterFunctions.ttcn"
gen_links $DIR $FILES

DIR=$BASEDIR/Common_NAS
FILES="NAS_CommonTypeDefs.ttcn NAS_AuxiliaryDefsAndFunctions.ttcn NAS_CommonTemplates.ttcn SMS_TypeDefs.ttcn"
gen_links $DIR $FILES

DIR=$BASEDIR/CommonEUTRA_NAS
FILES="EUTRA_NASCommonTemplates.ttcn EUTRA_NASCommonFunctions.ttcn"
gen_links $DIR $FILES

DIR=$BASEDIR/NasEmulation
FILES="NasEmu.ttcn NasEmu_AspTypes.ttcn NasEmu_Component.ttcn NasEmu_Templates.ttcn NasEmu_C2KPreReg.ttcn"
gen_links $DIR $FILES

DIR=$BASEDIR/IP_PTC
FILES="IP_PTC_CtrlMsgs.ttcn IP_ASP_TypeDefs.ttcn IP_PTC_Component.ttcn IP_PTC_Main.ttcn IP_PTC_Routing.ttcn IP_PTC_Templates.ttcn DHCPv4_Functions.ttcn DHCPv4_TypeDefs.ttcn ICMPv6_Functions.ttcn ICMPv6_TypeDefs.ttcn"
gen_links $DIR $FILES

DIR=$BASEDIR/MTC
FILES="MTC_Main.ttcn MTC_Component.ttcn MTC_UpperTester.ttcn"
gen_links $DIR $FILES
