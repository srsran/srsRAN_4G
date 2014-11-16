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

#include "liblte/rrc/messages/sib1.h"
#include "liblte/rrc/messages/bcch.h"
#include "liblte/phy/utils/bit.h"
#include "rrc_asn.h"




int bcch_bch_pack(lte_cell_t *cell, uint32_t sfn, uint8_t *buffer, uint32_t buffer_size_bytes) {
  
  MasterInformationBlock_t req;
  
  switch (cell->nof_prb) {
    case 6:         
      req.dl_Bandwidth = MasterInformationBlock__dl_Bandwidth_n6;
      break;
    case 15:
      req.dl_Bandwidth = MasterInformationBlock__dl_Bandwidth_n15;    
      break;
    case 25:
      req.dl_Bandwidth = MasterInformationBlock__dl_Bandwidth_n25;    
      break; 
    case 50:
      req.dl_Bandwidth = MasterInformationBlock__dl_Bandwidth_n50;    
      break; 
    case 75:
      req.dl_Bandwidth = MasterInformationBlock__dl_Bandwidth_n75;    
      break; 
    case 100:
      req.dl_Bandwidth = MasterInformationBlock__dl_Bandwidth_n100;
      break;    
    default:
      fprintf(stderr, "Invalid bandwidth %d PRB\n", cell->nof_prb);
      return LIBLTE_ERROR; 
  }
  if (cell->phich_length == PHICH_NORM) {
    req.phich_Config.phich_Duration = PHICH_Config__phich_Duration_normal;    
  } else {
    req.phich_Config.phich_Duration = PHICH_Config__phich_Duration_extended;
  }
  switch(cell->phich_resources) {
    case R_1:
      req.phich_Config.phich_Resource = PHICH_Config__phich_Resource_one;      
      break;
    case R_1_2:
      req.phich_Config.phich_Resource = PHICH_Config__phich_Resource_half;
      break;
    case R_1_6:
      req.phich_Config.phich_Resource = PHICH_Config__phich_Resource_oneSixth;
      break;
    case R_2:
      req.phich_Config.phich_Resource = PHICH_Config__phich_Resource_two;
      break;
  }
  sfn=(sfn>>2);
  req.systemFrameNumber.buf = (uint8_t*) &sfn;
  req.systemFrameNumber.size = 1;
  int spare = 0;
  req.spare.buf = (uint8_t*) &spare;
  req.spare.size = 2;
  req.spare.bits_unused = 6;
  
  asn_enc_rval_t n = uper_encode_to_buffer(&asn_DEF_MasterInformationBlock, (void*) &req, buffer, buffer_size_bytes);
  if (n.encoded == -1) {
    printf("Encoding failed.\n");
    printf("Failed to encode element %s\n", n.failed_type ? n.failed_type->name : "");
    return LIBLTE_ERROR;
  } 
  asn_fprint(stdout, &asn_DEF_MasterInformationBlock, &req); 
  return LIBLTE_SUCCESS;
}

int bcch_bch_unpack(uint8_t *buffer, uint32_t msg_nof_bits, lte_cell_t *cell, uint32_t *sfn) {
  asn_codec_ctx_t *opt_codec_ctx = NULL;
  MasterInformationBlock_t *req = calloc(1, sizeof(MasterInformationBlock_t));
  if (!req) {
    perror("calloc");
    return LIBLTE_ERROR; 
  }
  asn_dec_rval_t n = uper_decode(opt_codec_ctx, &asn_DEF_MasterInformationBlock, 
                                 (void**) &req, buffer, msg_nof_bits/8,0,msg_nof_bits%8);
  if (n.consumed == -1) {
    printf("Decoding failed.\n");
    return LIBLTE_ERROR;
  } 

  switch(req->dl_Bandwidth) {
    case MasterInformationBlock__dl_Bandwidth_n6:
      cell->nof_prb = 6;
      break;
    case MasterInformationBlock__dl_Bandwidth_n15:
      cell->nof_prb = 15;
      break;
    case MasterInformationBlock__dl_Bandwidth_n25:
      cell->nof_prb = 25;
      break;
    case MasterInformationBlock__dl_Bandwidth_n50:
      cell->nof_prb = 50;
      break;
    case MasterInformationBlock__dl_Bandwidth_n75:
      cell->nof_prb = 75;
      break;
  } 
  if (req->phich_Config.phich_Duration == PHICH_Config__phich_Duration_normal) {
    cell->phich_length = PHICH_NORM;    
  } else {
    cell->phich_length = PHICH_EXT;
  } 
  switch(req->phich_Config.phich_Resource) {
    case PHICH_Config__phich_Resource_one:
      cell->phich_resources = R_1;      
      break;
    case PHICH_Config__phich_Resource_half:
      cell->phich_resources = R_1_2;
      break;
    case PHICH_Config__phich_Resource_oneSixth:
      cell->phich_resources = R_1_6;
      break;
    case PHICH_Config__phich_Resource_two:
      cell->phich_resources = R_2;
      break;
  }
  int sfn_i=0;; 
  memcpy(&sfn_i, req->systemFrameNumber.buf, req->systemFrameNumber.size);
  if (sfn) {
    *sfn=(sfn_i<<2);    
  }
  return LIBLTE_SUCCESS;
}


int bcch_dlsch_pack(void *bcch_dlsch_msg, uint8_t *buffer, uint32_t buffer_size_bytes) {
  BCCH_DL_SCH_Message_t *msg = (BCCH_DL_SCH_Message_t*) bcch_dlsch_msg; 
  if (!msg) {
    msg = calloc(1, sizeof(BCCH_DL_SCH_Message_t));
  }
  int ret; 
  
  MCC_MNC_Digit_t mcc[3] = {2,1,4};
  MCC_MNC_Digit_t mnc[2] = {0,1};
  uint8_t tac_val[2] = {0x2,0x0b};
  uint8_t cid_val[4] = {0x14,0x4a,0x30,0x10};
  
  bcch_dlsch_sib1(msg, mcc, mnc, tac_val, cid_val, 3);
  asn_enc_rval_t n = uper_encode_to_buffer(&asn_DEF_BCCH_DL_SCH_Message, 
                                           (void*) msg, buffer, 
                                           buffer_size_bytes);
  if (n.encoded == -1) {
    fprintf(stderr, "Encoding failed. Element %s\n", n.failed_type ? n.failed_type->name : "");
    return LIBLTE_ERROR;
  } else {
    return n.encoded;
  }
  
  bcch_dlsch_fprint(&msg, stdout);
  bcch_dlsch_free(&msg);
  
  return ret;
}

void* bcch_dlsch_unpack(uint8_t *buffer, uint32_t msg_nof_bits) {
  BCCH_DL_SCH_Message_t *msg = calloc(1, sizeof(BCCH_DL_SCH_Message_t));  
  asn_dec_rval_t n = uper_decode(NULL, &asn_DEF_BCCH_DL_SCH_Message, 
                                 (void**) &msg, buffer, msg_nof_bits/8,0,msg_nof_bits%8);
  if (n.consumed == -1) {
    fprintf(stderr, "Decoding failed.\n");
    return NULL;
  } else {
    bcch_dlsch_fprint(msg, stdout);        
    return msg;
  }
}

bcch_dlsch_sib_type_t bcch_dlsch_get_type(void *bcch_dlsch_msg) {
  BCCH_DL_SCH_Message_t *msg = (BCCH_DL_SCH_Message_t*) bcch_dlsch_msg; 
  switch(msg->message.present) {
    case BCCH_DL_SCH_MessageType_PR_c1:
      switch (msg->message.choice.c1.present) {
        case BCCH_DL_SCH_MessageType__c1_PR_systemInformationBlockType1: 
          return BCCH_DLSCH_SIB1;
        break;
        default: 
          fprintf(stderr, "Message type not supported\n");
          bcch_dlsch_fprint(bcch_dlsch_msg, stderr);
        break;
      }
    default:
      fprintf(stderr, "Message type not supported\n");
      bcch_dlsch_fprint(bcch_dlsch_msg, stderr);
    break;
  }
  return BCCH_DLSCH_UNKNOWN;
}

void bcch_dlsch_fprint(void *bcch_dlsch_msg, FILE *stream) {
  xer_fprint(stdout, &asn_DEF_BCCH_DL_SCH_Message, (BCCH_DL_SCH_Message_t*) bcch_dlsch_msg); 
}

void bcch_dlsch_free(void *bcch_dlsch_msg) {
    asn_DEF_BCCH_DL_SCH_Message.free_struct(&asn_DEF_BCCH_DL_SCH_Message, 
                                            (BCCH_DL_SCH_Message_t*) bcch_dlsch_msg, 0);
}