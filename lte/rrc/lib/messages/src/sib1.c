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

#include "liblte/rrc/common/rrc_common.h"
#include "liblte/rrc/messages/bcch.h"
#include "liblte/rrc/messages/sib1.h"
#include "liblte/phy/utils/bit.h"
#include "rrc_asn.h"

void read_plmn(PLMN_IdentityInfo_t *msg, plmn_identity_t *plmn) {
  MCC_t *mcc = msg->plmn_Identity.mcc;
  plmn->mcc =  *((int*) mcc->list.array[0])*100+*((int*)mcc->list.array[1])*10+*((int*)mcc->list.array[2]);
  plmn->mnc = *((int*) msg->plmn_Identity.mnc.list.array[0])*10+*((int*) msg->plmn_Identity.mnc.list.array[1]);
}

uint32_t bcch_dlsch_sib1_get_freq_num(void *bcch_dlsch_msg) {
  BCCH_DL_SCH_Message_t *msg = (BCCH_DL_SCH_Message_t*) bcch_dlsch_msg; 
  SystemInformationBlockType1_t *sib1 = &(msg->message.choice.c1.choice.systemInformationBlockType1);
  return (uint32_t) sib1->freqBandIndicator;
}

void bcch_dlsch_sib1_get_plmns(void *bcch_dlsch_msg, plmn_identity_t *plmns, uint32_t max_plmn_identities) {
  int i; 
  BCCH_DL_SCH_Message_t *msg = (BCCH_DL_SCH_Message_t*) bcch_dlsch_msg; 
  SystemInformationBlockType1_t *sib1 = &(msg->message.choice.c1.choice.systemInformationBlockType1);
  for (i=0;i<MIN(max_plmn_identities,sib1->cellAccessRelatedInfo.plmn_IdentityList.list.count);i++) {
    read_plmn(sib1->cellAccessRelatedInfo.plmn_IdentityList.list.array[i], &plmns[i]);
  }
}

void bcch_dlsch_sib1_get_cell_access_info(void *bcch_dlsch_msg, cell_access_info_t *info) {
  BCCH_DL_SCH_Message_t *msg = (BCCH_DL_SCH_Message_t*) bcch_dlsch_msg; 
  SystemInformationBlockType1_t *sib1 = &(msg->message.choice.c1.choice.systemInformationBlockType1);
  info->tracking_area_code = sib1->cellAccessRelatedInfo.trackingAreaCode.buf[0]<<8|
    sib1->cellAccessRelatedInfo.trackingAreaCode.buf[1];
  info->cell_id = (sib1->cellAccessRelatedInfo.cellIdentity.buf[0]<<24)|
    (sib1->cellAccessRelatedInfo.cellIdentity.buf[1]<<16)|
    (sib1->cellAccessRelatedInfo.cellIdentity.buf[2]<<8)|
    (sib1->cellAccessRelatedInfo.cellIdentity.buf[3]&0xF0);
  info->cell_id >>= 4;
  info->cellBarred = 
    sib1->cellAccessRelatedInfo.cellBarred == 
    SystemInformationBlockType1__cellAccessRelatedInfo__cellBarred_barred; 
  info->intraFreqReselection = 
    sib1->cellAccessRelatedInfo.intraFreqReselection == 
    SystemInformationBlockType1__cellAccessRelatedInfo__intraFreqReselection_allowed;
}

uint32_t get_si_period(long int period) {
  return 8<<(period);     
}

sib_type_t get_si_type(SIB_Type_t *type) {
  switch(*type) {
    case SIB_Type_sibType3: return SIB3;
    case SIB_Type_sibType4: return SIB4;
    case SIB_Type_sibType5: return SIB5;
    case SIB_Type_sibType6: return SIB6;
    case SIB_Type_sibType7: return SIB7;
    case SIB_Type_sibType8: return SIB8;
    case SIB_Type_sibType9: return SIB9;
    default: return SIB_ERROR;
  }
}

uint32_t get_window(long int window_length) {
  switch(window_length) {
    case SystemInformationBlockType1__si_WindowLength_ms1: return 1;
    case SystemInformationBlockType1__si_WindowLength_ms2: return 2;
    case SystemInformationBlockType1__si_WindowLength_ms5: return 5;
    case SystemInformationBlockType1__si_WindowLength_ms10: return 10;
    case SystemInformationBlockType1__si_WindowLength_ms15: return 15;
    case SystemInformationBlockType1__si_WindowLength_ms20: return 20;
    case SystemInformationBlockType1__si_WindowLength_ms40: return 40;
    default: return 0; 
  }
}
int bcch_dlsch_sib1_get_scheduling_info(void *bcch_dlsch_msg, 
                                        uint32_t *si_window_length,
                                        scheduling_info_t *info, 
                                        uint32_t max_elems) 
{
  BCCH_DL_SCH_Message_t *msg = (BCCH_DL_SCH_Message_t*) bcch_dlsch_msg; 
  SystemInformationBlockType1_t *sib1 = &(msg->message.choice.c1.choice.systemInformationBlockType1);
  
  uint32_t nelems = 0; 
  
  if (max_elems > 0 && info != NULL) {
    /* First is always SIB2 */
    info[0].type = SIB2; 
    info[0].n = 0; 
    info[0].period = get_si_period(sib1->schedulingInfoList.list.array[0]->si_Periodicity);
    nelems++; 
    for (int i=0;i<sib1->schedulingInfoList.list.count;i++) {
      SchedulingInfo_t *s = sib1->schedulingInfoList.list.array[i];
      
      for (int j=0;j<s->sib_MappingInfo.list.count;j++) {
        if (nelems < max_elems) {
          info[nelems].type = get_si_type(s->sib_MappingInfo.list.array[j]);
          info[nelems].n = i;
          info[nelems].period = get_si_period(s->si_Periodicity);
          nelems++;
        }
      }      
    }
    
  }
  if (si_window_length) {
    *si_window_length = get_window(sib1->si_WindowLength);    
  }
  return nelems;
}

MCC_MNC_Digit_t *dup_digit(MCC_MNC_Digit_t value) {
  MCC_MNC_Digit_t *x = calloc(1, sizeof(MCC_MNC_Digit_t));
  *x = value; 
  return x;
}
void bcch_dlsch_sib1(BCCH_DL_SCH_Message_t *msg, 
                     MCC_MNC_Digit_t mcc_val[3], 
                     MCC_MNC_Digit_t mnc_val[2], 
                     uint8_t tac_val[2], 
                     uint8_t cid_val[4], 
                     int freq_band) 
{

  bzero(msg, sizeof(BCCH_DL_SCH_Message_t));

  msg->message.present = BCCH_DL_SCH_MessageType_PR_c1;
  msg->message.choice.c1.present = BCCH_DL_SCH_MessageType__c1_PR_systemInformationBlockType1;
  SystemInformationBlockType1_t *sib1 = &(msg->message.choice.c1.choice.systemInformationBlockType1);
  
  PLMN_IdentityInfo_t *PLMN_identity_info = calloc(1, sizeof(PLMN_IdentityInfo_t));
  MCC_t *mcc = calloc(1, sizeof(MCC_t));
  PLMN_identity_info->plmn_Identity.mcc = mcc;
  asn_set_empty(&mcc->list);
  for (int i=0;i<3;i++) {
    ASN_SEQUENCE_ADD(&mcc->list,dup_digit(mcc_val[i]));    
  }
  asn_set_empty(&PLMN_identity_info->plmn_Identity.mnc.list);
  for (int i=0;i<2;i++) {
    ASN_SEQUENCE_ADD(&PLMN_identity_info->plmn_Identity.mnc.list,dup_digit(mnc_val[i]));    
  }
  PLMN_identity_info->cellReservedForOperatorUse=PLMN_IdentityInfo__cellReservedForOperatorUse_notReserved;

  ASN_SEQUENCE_ADD(&sib1->cellAccessRelatedInfo.plmn_IdentityList.list,PLMN_identity_info);

  sib1->cellAccessRelatedInfo.trackingAreaCode.buf=malloc(2); 
  for (int i=0;i<2;i++) {
    sib1->cellAccessRelatedInfo.trackingAreaCode.buf[i] = tac_val[i];    
  }
  sib1->cellAccessRelatedInfo.trackingAreaCode.size=2;
  sib1->cellAccessRelatedInfo.trackingAreaCode.bits_unused=0;

  sib1->cellAccessRelatedInfo.cellIdentity.buf=malloc(4); 
  for (int i=0;i<4;i++) {
    sib1->cellAccessRelatedInfo.cellIdentity.buf[i] = cid_val[i];
  }
  sib1->cellAccessRelatedInfo.cellIdentity.size=4;
  sib1->cellAccessRelatedInfo.cellIdentity.bits_unused=4;

  sib1->cellAccessRelatedInfo.cellBarred=SystemInformationBlockType1__cellAccessRelatedInfo__cellBarred_barred;  

  sib1->cellAccessRelatedInfo.intraFreqReselection=SystemInformationBlockType1__cellAccessRelatedInfo__intraFreqReselection_allowed;
  sib1->cellAccessRelatedInfo.csg_Indication=0;

  sib1->cellSelectionInfo.q_RxLevMin=-60;
  sib1->cellSelectionInfo.q_RxLevMinOffset=NULL;

  sib1->freqBandIndicator = (long int) freq_band;


  SchedulingInfo_t *schedulingInfo_3 = calloc(2, sizeof(SchedulingInfo_t));
  SchedulingInfo_t *schedulingInfo_6 = calloc(2, sizeof(SchedulingInfo_t));
  SIB_Type_t *sib_type_3 = calloc(1, sizeof(SIB_Type_t));
  SIB_Type_t *sib_type_6 = calloc(1, sizeof(SIB_Type_t));

  schedulingInfo_3->si_Periodicity=SchedulingInfo__si_Periodicity_rf16;
  *sib_type_3=SIB_Type_sibType3;
  ASN_SEQUENCE_ADD(&schedulingInfo_3->sib_MappingInfo.list,sib_type_3);
  ASN_SEQUENCE_ADD(&sib1->schedulingInfoList.list,schedulingInfo_3);    
  
  schedulingInfo_6->si_Periodicity=SchedulingInfo__si_Periodicity_rf64;
  *sib_type_6=SIB_Type_sibType6;
  ASN_SEQUENCE_ADD(&schedulingInfo_6->sib_MappingInfo.list,sib_type_6);
  ASN_SEQUENCE_ADD(&sib1->schedulingInfoList.list,schedulingInfo_6);    
    
  sib1->si_WindowLength=SystemInformationBlockType1__si_WindowLength_ms40;
  sib1->systemInfoValueTag=8;  
}





