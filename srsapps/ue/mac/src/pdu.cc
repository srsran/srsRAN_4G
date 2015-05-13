/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include <strings.h>
#include <string.h>
#include <stdlib.h>

#include "srsapps/ue/mac/pdu.h"
#include "srslte/srslte.h"

namespace srslte {
  namespace ue {
   
    
void sch_pdu::fprint(FILE* stream)
{
  fprintf(stream, "MAC SDU for UL/DL-SCH. ");
  pdu::fprint(stream);
}

void sch_subh::fprint(FILE* stream)
{
  if (is_sdu()) {
    fprintf(stream, "SDU LCHID=%d, SDU nof_bytes=%d\n", lcid, nof_bytes);
  } else {
    switch(lcid) {
      case C_RNTI:
        fprintf(stream, "C-RNTI CE: %d\n", get_c_rnti());
        break;
      case CON_RES_ID:
        fprintf(stream, "Contention Resolution ID CE: %d\n", get_con_res_id());
        break;
      case TA_CMD:
        fprintf(stream, "Time Advance Command CE: %d\n", get_ta_cmd());
        break;
    }    
  }
}
    
// Section 6.1.2
bool sch_pdu::write_packet(uint8_t* ptr)
{
  // Add single or two-byte padding if required
  if (rem_len == 1 || rem_len == 2) {
    sch_subh padding; 
    padding.set_padding(); 
    for (int i=0;i<rem_len;i++) {
      padding.write_subheader(&ptr, false);  
    }
    rem_len = 0;
  }
  // Find last SDU or CE 
  int last_sdu = nof_subheaders-1; 
  while(!subheaders[last_sdu].is_sdu() && last_sdu >= 0) {
    last_sdu--;
  }
  int last_ce = nof_subheaders-1; 
  while(subheaders[last_ce].is_sdu() && last_ce >= 0) {
    last_ce--;
  }
  int last_sh = subheaders[last_sdu].is_sdu()?last_sdu:last_ce;  
  // Write subheaders for MAC CE first
  for (int i=0;i<nof_subheaders;i++) {
    if (!subheaders[i].is_sdu()) {
      subheaders[i].write_subheader(&ptr, i==last_sh);
    }
  }
  // Then for SDUs
  for (int i=0;i<nof_subheaders;i++) {
    if (subheaders[i].is_sdu()) {
      subheaders[i].write_subheader(&ptr, i==last_sh);
    }
  }
  // Write payloads in the same order
  for (int i=0;i<nof_subheaders;i++) {
    if (!subheaders[i].is_sdu()) {
      subheaders[i].write_payload(&ptr);
    }
  }
  for (int i=0;i<nof_subheaders;i++) {
    if (subheaders[i].is_sdu()) {
      subheaders[i].write_payload(&ptr);
    }
  }
  // Set paddint to zeros (if any) 
  bzero(ptr, rem_len*sizeof(uint8_t)*8);
}




uint32_t sch_pdu::size_plus_header_pdu(uint32_t nbytes)
{
  if (nbytes < 128) {
    return nbytes + 2; 
  } else {
    return nbytes + 3; 
  }
}
bool sch_pdu::has_space_ce(uint32_t nbytes)
{
  if (rem_len >= nbytes + 1) {
    return true; 
  } else {
    return false; 
  }
}
bool sch_pdu::has_space_sdu(uint32_t nbytes)
{
  if (rem_len >= size_plus_header_pdu(nbytes)) {
    return true; 
  } else {
    return false; 
  }
}
bool sch_pdu::update_space_ce(uint32_t nbytes)
{
  if (has_space_ce(nbytes)) {
    rem_len -= nbytes + 1; 
  }
}
bool sch_pdu::update_space_sdu(uint32_t nbytes)
{
  if (has_space_sdu(nbytes)) {
    rem_len -= size_plus_header_pdu(nbytes);
  }
}



void sch_subh::init()
{
  lcid            = 0; 
  nof_bytes       = 0; 
  sdu_payload_ptr = NULL;
  bzero(ce_payload, sizeof(uint8_t) * MAX_CE_PAYLOAD_LEN);
}

sch_subh::cetype sch_subh::ce_type()
{
  if (lcid >= PHD_REPORT) {
    return (cetype) lcid;
  } else {
    return SDU;
  }
}

uint32_t sch_subh::sizeof_ce(uint32_t lcid, bool is_ul)
{
  if (is_ul) {
    switch(lcid) {
      case PHD_REPORT: 
        return 1; 
      case C_RNTI: 
        return 2;
      case TRUNC_BSR: 
        return 1;
      case SHORT_BSR: 
        return 1; 
      case LONG_BSR: 
        return 3; 
      case PADDING: 
        return 0; 
    }      
  } else {
    switch(lcid) {
      case CON_RES_ID: 
        return 6;
      case TA_CMD: 
        return 1; 
      case DRX_CMD:
        return 0; 
      case PADDING: 
        return 0; 
    }  
  }
}
bool sch_subh::is_sdu()
{
  return ce_type() == SDU;
}
uint16_t sch_subh::get_c_rnti()
{
  return *((uint16_t*) ce_payload);
}
uint64_t sch_subh::get_con_res_id()
{
  return *((uint64_t*) ce_payload);
}
uint8_t sch_subh::get_phd()
{
  return *((uint8_t*) ce_payload);
}
uint32_t sch_subh::get_sdu_lcid()
{
  return *((uint32_t*) ce_payload);
}
uint32_t sch_subh::get_sdu_nbytes()
{
  return *((uint32_t*) ce_payload);
}
uint8_t* sch_subh::get_sdu_ptr()
{
  return sdu_payload_ptr;
}
uint8_t sch_subh::get_ta_cmd()
{
  return *((uint8_t*) ce_payload);
}
void sch_subh::set_padding()
{
  lcid = PADDING;
}
bool sch_subh::set_c_rnti(uint16_t crnti)
{
  if (((sch_pdu*)parent)->has_space_ce(2)) {
    *((uint16_t*) ce_payload) = crnti;  
    lcid = C_RNTI;
    ((sch_pdu*)parent)->update_space_ce(2);
    return true; 
  } else {
    return false; 
  }
}
bool sch_subh::set_con_res_id(uint64_t con_res_id)
{
  if (((sch_pdu*)parent)->has_space_ce(6)) {
    *((uint64_t*) ce_payload) = con_res_id;  
    lcid = CON_RES_ID;
    ((sch_pdu*)parent)->update_space_ce(6);
    return true; 
  } else {
    return false; 
  }
}
bool sch_subh::set_phd(uint8_t phd)
{
  if (((sch_pdu*)parent)->has_space_ce(1)) {
    *((uint8_t*) ce_payload) = phd;  
    lcid = PHD_REPORT;
    ((sch_pdu*)parent)->update_space_ce(1);
    return true; 
  } else {
    return false; 
  }
}
bool sch_subh::set_sdu(uint32_t lcid_, uint8_t* ptr, uint32_t nof_bytes_)
{
  if (((sch_pdu*)parent)->has_space_sdu(nof_bytes_)) {
    sdu_payload_ptr = ptr; 
    nof_bytes = nof_bytes_;
    lcid = lcid_;
    ((sch_pdu*)parent)->update_space_sdu(nof_bytes_);
    return true; 
  } else {
    return false; 
  }
}
bool sch_subh::set_ta_cmd(uint8_t ta_cmd)
{
  if (((sch_pdu*)parent)->has_space_ce(1)) {
    *((uint8_t*) ce_payload) = ta_cmd;  
    lcid = TA_CMD;
    ((sch_pdu*)parent)->update_space_ce(1);
    return true; 
  } else {
    return false; 
  }
}
// Section 6.2.1
void sch_subh::write_subheader(uint8_t** ptr, bool is_last)
{
  if (is_sdu()) {
    // MAC SDU: R/R/E/LCID/F/L subheader
    srslte_bit_pack(0, ptr, 2);           // R, R    
    srslte_bit_pack(is_last?1:0, ptr, 1); // E
    srslte_bit_pack(lcid, ptr, 5);        // LCID

    // 2nd and 3rd octet
    srslte_bit_pack(F_bit?1:0, ptr, 1); // F
    srslte_bit_pack(nof_bytes, ptr, nof_bytes<128?7:15); // L
  } else {
    // MAC CE: R/R/E/LCID MAC Subheader
    srslte_bit_pack(0, ptr, 2);           // R, R    
    srslte_bit_pack(is_last?1:0, ptr, 1); // E
    srslte_bit_pack(lcid, ptr, 5);        // LCID
  }
}
void sch_subh::write_payload(uint8_t** ptr)
{
  if (is_sdu()) {
    memcpy(*ptr, sdu_payload_ptr, nof_bytes*8*sizeof(uint8_t));    
  } else {
    srslte_bit_pack_vector(ce_payload, *ptr, nof_bytes*8);
  }
  *ptr += nof_bytes*8;
}
bool sch_subh::read_subheader(uint8_t** ptr)
{
  // Skip R
  *ptr += 2; 
  bool e_bit   = srslte_bit_unpack(ptr, 1)?true:false;
  lcid         = srslte_bit_unpack(ptr, 5);
  if (is_sdu()) {
    F_bit      = srslte_bit_unpack(ptr, 1)?true:false;
    nof_bytes  = srslte_bit_unpack(ptr, F_bit?7:15); 
  } else {
    nof_bytes = sizeof_ce(lcid, parent->is_ul()); 
  }
  return e_bit;
}
void sch_subh::read_payload(uint8_t** ptr)
{
  if (is_sdu()) {
    sdu_payload_ptr = *ptr; 
  } else {
    srslte_bit_unpack_vector(ce_payload, *ptr, nof_bytes*8);
  }
  *ptr += nof_bytes*8;
}












void rar_pdu::fprint(FILE* stream)
{
  fprintf(stream, "MAC PDU for RAR. ");
  if (has_backoff_indicator) {
    fprintf(stream, "Backoff Indicator %d. ", backoff_indicator);
  }
  pdu::fprint(stream);  
}

void rar_subh::fprint(FILE* stream)
{
  fprintf(stream, "RAPID: %d, Temp C-RNTI: %d, TA: %d, UL Grant: ", preamble, temp_rnti, ta);
  srslte_vec_fprint_hex(stream, grant, 20);
}

rar_pdu::rar_pdu(uint32_t max_rars_) : pdu(max_rars_)
{
  backoff_indicator = 0; 
  has_backoff_indicator = false; 
}
uint8_t rar_pdu::get_backoff()
{
  return backoff_indicator;
}
bool rar_pdu::has_backoff()
{
  return has_backoff_indicator;
}
void rar_pdu::set_backoff(uint8_t bi)
{
  has_backoff_indicator = true; 
  backoff_indicator = bi; 
}

// Section 6.1.5
bool rar_pdu::write_packet(uint8_t* ptr)
{
  // Write Backoff Indicator, if any 
  if (has_backoff_indicator) {
    if (nof_subheaders > 0) {
      srslte_bit_pack(1, &ptr, 1);                 // E
      srslte_bit_pack(0, &ptr, 1);                 // T
      srslte_bit_pack(0, &ptr, 2);                 // R, R
      srslte_bit_pack(backoff_indicator, &ptr, 4);
    }
  }
  // Write RAR subheaders
  for (int i=0;i<nof_subheaders;i++) {
    subheaders[i].write_subheader(&ptr, i==nof_subheaders-1);
  }
  // Write payload 
  for (int i=0;i<nof_subheaders;i++) {
    subheaders[i].write_payload(&ptr);
  }
  // Set paddint to zeros (if any) 
  bzero(ptr, rem_len*sizeof(uint8_t)*8);
}



void rar_subh::init()
{
  bzero(grant, sizeof(uint8_t) * RAR_GRANT_LEN);
  ta        = 0; 
  temp_rnti = 0; 
}
uint32_t rar_subh::get_rapid()
{
  return preamble; 
}
void rar_subh::get_sched_grant(uint8_t grant_[RAR_GRANT_LEN])
{
  memcpy(grant_, &grant, sizeof(uint8_t)*RAR_GRANT_LEN);
}
uint32_t rar_subh::get_ta_cmd()
{
  return ta; 
}
uint16_t rar_subh::get_temp_crnti()
{
  return temp_rnti;
}
void rar_subh::set_rapid(uint32_t rapid)
{
  preamble = rapid; 
}
void rar_subh::set_sched_grant(uint8_t grant_[RAR_GRANT_LEN])
{
  memcpy(&grant, grant_, sizeof(uint8_t)*RAR_GRANT_LEN);
}
void rar_subh::set_ta_cmd(uint32_t ta_)
{
  ta = ta_; 
}
void rar_subh::set_temp_crnti(uint16_t temp_rnti_)
{
  temp_rnti = temp_rnti_;
}
// Section 6.2.2
void rar_subh::write_subheader(uint8_t** ptr, bool is_last)
{
  srslte_bit_pack(is_last?1:0, ptr, 1); // E
  srslte_bit_pack(1, ptr, 1);           // T
  srslte_bit_pack(preamble, ptr, 6);    // RAPID
}
// Section 6.2.3
void rar_subh::write_payload(uint8_t** ptr)
{
  srslte_bit_pack(0, ptr, 1);             // R
  srslte_bit_pack(ta, ptr, 11);           // Timing Adv Cmd
  memcpy(*ptr, grant, 20*sizeof(uint8_t)); // UL grant
  *ptr += 20; 
  srslte_bit_pack(temp_rnti, ptr, 16);    // Temp C-RNTI
}
void rar_subh::read_payload(uint8_t** ptr)
{
  *ptr += 1; // R
  ta    = srslte_bit_unpack(ptr, 11);     // Timing Adv Cmd
  memcpy(grant, *ptr, 20*sizeof(uint8_t)); // UL Grant
  *ptr += 20; 
  temp_rnti = srslte_bit_unpack(ptr, 16); // Temp C-RNTI  
}
bool rar_subh::read_subheader(uint8_t** ptr)
{
  bool e_bit   = srslte_bit_unpack(ptr, 1); // E
  bool type    = srslte_bit_unpack(ptr, 1); // T
  if (type) {
    preamble   = srslte_bit_unpack(ptr, 6); // RAPID  
  } else {
    // Read Backoff
    *ptr += 2; // R, R
    ((rar_pdu*)parent)->set_backoff((uint8_t) srslte_bit_unpack(ptr, 4));
  }
  return e_bit;
}


  
    
  }
}
