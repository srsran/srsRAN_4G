/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The libLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the libLTE library.
 *
 * libLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "liblte/rrc/rrc_msg/bcch.h"
#include "liblte/phy/utils/bit.h"
#include "rrc_asn.h"


void sib1_create_default(SystemInformationBlockType1_t *sib1, 
                            MCC_MNC_Digit_t mcc_val[3], 
                            MCC_MNC_Digit_t mnc_val[2], 
                            uint8_t tac_val[2],
                            uint8_t cid_val[4], 
                            int freq_band) 
{
  

  PLMN_IdentityInfo_t PLMN_identity_info;
  asn_enc_rval_t enc_rval;
  SchedulingInfo_t schedulingInfo;
  SIB_Type_t sib_type;
  struct MCC mcc; 

  bzero(sib1, sizeof(SystemInformationBlockType1_t));
  bzero(&PLMN_identity_info, sizeof(PLMN_IdentityInfo_t));
  bzero(&schedulingInfo, sizeof(SchedulingInfo_t));
  bzero(&sib_type, sizeof(SIB_Type_t));
  bzero(&mcc, sizeof(struct MCC));

  PLMN_identity_info.plmn_Identity.mcc = &mcc;
  asn_set_empty(&mcc);
  for (int i=0;i<3;i++) {
    ASN_SEQUENCE_ADD(&mcc.list,&mcc_val[i]);    
  }
  printf("MCC set len: %d\n", mcc.list.count);
  for (int i=0;i<2;i++) {
    ASN_SEQUENCE_ADD(&PLMN_identity_info.plmn_Identity.mnc.list,&mnc_val[i]);    
  }
  PLMN_identity_info.cellReservedForOperatorUse=cellReservedForOperatorUse_reserved;

  ASN_SEQUENCE_ADD(&sib1->cellAccessRelatedInfo.plmn_IdentityList.list,&PLMN_identity_info);

  sib1->cellAccessRelatedInfo.trackingAreaCode.buf=tac_val;    
  sib1->cellAccessRelatedInfo.trackingAreaCode.size=2;
  sib1->cellAccessRelatedInfo.trackingAreaCode.bits_unused=0;

  sib1->cellAccessRelatedInfo.cellIdentity.buf=cid_val;
  sib1->cellAccessRelatedInfo.cellIdentity.size=4;
  sib1->cellAccessRelatedInfo.cellIdentity.bits_unused=4;

  sib1->cellAccessRelatedInfo.cellBarred=cellBarred_notBarred;  

  sib1->cellAccessRelatedInfo.intraFreqReselection=intraFreqReselection_allowed;
  sib1->cellAccessRelatedInfo.csg_Indication=0;

  sib1->cellSelectionInfo.q_RxLevMin=-70;
  sib1->cellSelectionInfo.q_RxLevMinOffset=NULL;

  sib1->freqBandIndicator = (long int) freq_band;

  schedulingInfo.si_Periodicity=si_Periodicity_rf8;

  //  assign_enum(&sib_type,SIB_Type_sibType3);
  sib_type=SIB_Type_sibType3;

  ASN_SEQUENCE_ADD(&schedulingInfo.sib_MappingInfo.list,&sib_type);
  ASN_SEQUENCE_ADD(&sib1->schedulingInfoList.list,&schedulingInfo);
 
  sib1->tdd_Config = NULL;
 
  sib1->si_WindowLength=si_WindowLength_ms10;
  sib1->systemInfoValueTag=0;

  asn_fprint(stdout, &asn_DEF_SystemInformationBlockType1, (void*)sib1);
}