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
    if (parent->is_ul()) {
      switch(lcid) {
        case C_RNTI:
          fprintf(stream, "C-RNTI CE: %d\n", get_c_rnti());
          break;
        case PHD_REPORT:
          fprintf(stream, "C-RNTI CE: %d\n", get_c_rnti());
          break;
        case TRUNC_BSR:
          fprintf(stream, "Truncated BSR CE\n");
          break;
        case SHORT_BSR:
          fprintf(stream, "Short BSR CE\n");
          break;
        case LONG_BSR:
          fprintf(stream, "Long BSR CE\n");
          break;
        case PADDING:
          fprintf(stream, "PADDING\n");
      }
    } else {
      switch(lcid) {
        case CON_RES_ID:
          fprintf(stream, "Contention Resolution ID CE: 0x%lx\n", get_con_res_id());
          break;
        case TA_CMD:
          fprintf(stream, "Time Advance Command CE: %d\n", get_ta_cmd());
          break;
        case DRX_CMD:
          fprintf(stream, "DRX Command CE: Not implemented\n");
          break;
        case PADDING:
          fprintf(stream, "PADDING\n");
      }      
    }
  }
}

void sch_pdu::parse_packet(uint8_t *ptr)
{
  pdu::parse_packet(ptr);
  
  // Correct size for last SDU 
  if (nof_subheaders > 0) {
    uint32_t read_len = 0; 
    for (int i=0;i<nof_subheaders-1;i++) {
      read_len += subheaders[i].size_plus_header();
    }
    if (pdu_len-read_len-1 >= 0) {
      subheaders[nof_subheaders-1].set_payload_size(pdu_len-read_len-1);
    } else {
      fprintf(stderr,"Reading MAC PDU: negative payload for last subheader\n");
    }
  }
}
    
// Section 6.1.2
bool sch_pdu::write_packet(uint8_t* ptr, rlc_interface_mac *rlc)
{
  uint8_t *init_ptr = ptr; 
  bool last_is_padding = false; 

  // Find last SDU or CE 
  int last_sh;
  int last_sdu = nof_subheaders-1; 
  while(!subheaders[last_sdu].is_sdu() && last_sdu >= 0) {
    last_sdu--;
  }
  int last_ce = nof_subheaders-1; 
  while(subheaders[last_ce].is_sdu() && last_ce >= 0) {
    last_ce--;
  }
  last_sh = subheaders[last_sdu].is_sdu()?last_sdu:last_ce;  
  
  // Add multi-byte padding if there are more than 2 bytes or there are 2 bytes 
  // and there is at least one SDU 
  if (rem_len > 2) {
    last_is_padding = true; 
  } else if (rem_len > 0) {
    // Add single or two-byte padding if required
    if (rem_len == 1 || rem_len == 2) {
      sch_subh padding; 
      padding.set_padding(); 
      for (int i=0;i<rem_len;i++) {
        padding.write_subheader(&ptr, false);  
      }
      rem_len = 0;
    }     
  }
  if (last_is_padding) {
    last_sh = -1;
  }
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
  if (last_is_padding) {
    sch_subh padding; 
    padding.set_padding(rem_len); 
    padding.write_subheader(&ptr, true);
  }
  // Write payloads in the same order
  for (int i=0;i<nof_subheaders;i++) {
    if (!subheaders[i].is_sdu()) {
      subheaders[i].write_payload(&ptr, rlc);
    }
  }
  for (int i=0;i<nof_subheaders;i++) {
    if (subheaders[i].is_sdu()) {
      subheaders[i].write_payload(&ptr, rlc);
    }
  }
  // Set paddint to zeros (if any) 
  bzero(ptr, rem_len*sizeof(uint8_t));
 
  return true; 
}

uint32_t sch_pdu::rem_size() {
  return rem_len; 
}

uint32_t sch_pdu::size()
{
  return pdu_len - rem_len; 
}

uint32_t sch_pdu::size_plus_header_sdu(uint32_t nbytes)
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
  return has_space_sdu(nbytes, false);
}

bool sch_pdu::has_space_sdu(uint32_t nbytes, bool is_first)
{
  uint32_t sizeh_first=is_first?1:0;
  if (rem_len >= size_plus_header_sdu(nbytes)-sizeh_first) {
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
bool sch_pdu::update_space_sdu(uint32_t nbytes) {
  return update_space_sdu(nbytes, false);
}

bool sch_pdu::update_space_sdu(uint32_t nbytes, bool is_first)
{
  uint32_t sizeh_first=is_first?1:0;
  if (has_space_sdu(nbytes, is_first)) {
    rem_len -= (size_plus_header_sdu(nbytes)-sizeh_first);
  }
}

void sch_subh::init()
{
  lcid            = 0; 
  nof_bytes       = 0; 
  payload         = NULL;
  bzero(payload, sizeof(uint8_t) * MAX_CE_PAYLOAD_LEN);
}

sch_subh::cetype sch_subh::ce_type()
{
  if (lcid >= PHD_REPORT) {
    return (cetype) lcid;
  } else {
    return SDU;
  }
}

void sch_subh::set_payload_size(uint32_t size) {
  nof_bytes = size; 
}

uint32_t sch_subh::size_plus_header() {
  if (is_sdu()) {
    return sch_pdu::size_plus_header_sdu(nof_bytes); 
  } else {
    return nof_bytes + 1;
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
  if (payload) {
    return (uint16_t) payload[0] | payload[1]<<8;
  } else {
    return 0;
  }
}
uint64_t sch_subh::get_con_res_id()
{
  if (payload) {
    return ((uint64_t) payload[0]) | ((uint64_t) payload[1])<<8 | ((uint64_t) payload[2])<<16 | ((uint64_t) payload[3])<<24 |
                      ((uint64_t) payload[4])<<32 | ((uint64_t) payload[5])<<48;                
  } else {
    return 0; 
  }
}
uint8_t sch_subh::get_phd()
{
  if (payload) {
    return (uint8_t) payload[0]&0x3f;
  } else {
    return 0;
  }
}
uint8_t sch_subh::get_ta_cmd()
{
  if (payload) {
    return (uint8_t) payload[0]&0x3f;
  } else {
    return 0;
  }
}
uint32_t sch_subh::get_sdu_lcid()
{
  return lcid;
}
uint32_t sch_subh::get_sdu_nbytes()
{
  return nof_bytes;
}
uint8_t* sch_subh::get_sdu_ptr()
{
  return payload;
}
void sch_subh::set_padding(uint32_t padding_len)
{
  lcid = PADDING;
  nof_bytes = padding_len; 
}
void sch_subh::set_padding()
{
  set_padding(0);
}


bool sch_subh::set_bsr(uint32_t buff_size[4], sch_subh::cetype format, bool update_size)
{
  uint32_t nonzero_lcg=0;
  for (int i=0;i<4;i++) {
    if (buff_size[i]) {
      nonzero_lcg=i;
    }
  }
  uint32_t ce_size = format==LONG_BSR?3:1;
  if (((sch_pdu*)parent)->has_space_ce(ce_size) || !update_size) {
    if (format==LONG_BSR) {
      w_payload_ce[0] = (buff_size_table(buff_size[0])&0x3f) << 2 | (buff_size_table(buff_size[1])&0xc0)>>6;
      w_payload_ce[1] = (buff_size_table(buff_size[1])&0xf)  << 4 | (buff_size_table(buff_size[2])&0xf0)>>4;
      w_payload_ce[2] = (buff_size_table(buff_size[2])&0x3)  << 6 | (buff_size_table(buff_size[3])&0x3f);
    } else {
      w_payload_ce[0] = (nonzero_lcg&0x3)<<6 | buff_size_table(buff_size[nonzero_lcg])&0x3f;
    }
    lcid = format;
    if (update_size) {
      ((sch_pdu*)parent)->update_space_ce(ce_size);
    }
    return true; 
  } else {
    return false; 
  }  
}

bool sch_subh::set_c_rnti(uint16_t crnti)
{
  if (((sch_pdu*)parent)->has_space_ce(2)) {
    w_payload_ce[0] = (uint8_t) (crnti&0xff00)>>8;
    w_payload_ce[1] = (uint8_t) (crnti&0x00ff);
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
    w_payload_ce[0] = (uint8_t) ((con_res_id&0xff0000000000)>>48);
    w_payload_ce[1] = (uint8_t) ((con_res_id&0x00ff00000000)>>32);
    w_payload_ce[2] = (uint8_t) ((con_res_id&0x0000ff000000)>>24);
    w_payload_ce[3] = (uint8_t) ((con_res_id&0x000000ff0000)>>16);
    w_payload_ce[4] = (uint8_t) ((con_res_id&0x00000000ff00)>>8);
    w_payload_ce[5] = (uint8_t) ((con_res_id&0x0000000000ff));
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
    w_payload_ce[0] = phd&0x3f; 
    lcid = PHD_REPORT;
    ((sch_pdu*)parent)->update_space_ce(1);
    return true; 
  } else {
    return false; 
  }
}

bool sch_subh::set_ta_cmd(uint8_t ta_cmd)
{
  if (((sch_pdu*)parent)->has_space_ce(1)) {
    w_payload_ce[0] = ta_cmd&0x3f; 
    lcid = TA_CMD;
    ((sch_pdu*)parent)->update_space_ce(1);
    return true; 
  } else {
    return false; 
  }
}

bool sch_subh::set_sdu(uint32_t lcid_, uint32_t nof_bytes_)
{
  return set_sdu(lcid_, nof_bytes_, false);
}

bool sch_subh::set_sdu(uint32_t lcid_, uint32_t nof_bytes_, bool is_first)
{
  if (((sch_pdu*)parent)->has_space_sdu(nof_bytes_, is_first)) {
    nof_bytes = nof_bytes_;
    lcid = lcid_;
    ((sch_pdu*)parent)->update_space_sdu(nof_bytes_, is_first);
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
    *(*ptr + 0)   = (uint8_t) is_last<<5 | (lcid & 0x1f);

    // 2nd and 3rd octet
    if (!is_last) {
      if (nof_bytes >= 128) {
        *(*ptr + 1) = (uint8_t) 1<<7 | ((nof_bytes & 0x7f00) >> 8);
        *(*ptr + 2) = (uint8_t) (nof_bytes & 0xff);
      } else {
       *(*ptr + 1) = (uint8_t) (nof_bytes & 0x7f); 
     }      
    }
  } else {
    // MAC CE: R/R/E/LCID MAC Subheader
    *(*ptr + 0)   = (uint8_t) is_last<<5 | (lcid & 0x1f);
  }
}

void sch_subh::write_payload(uint8_t** ptr, rlc_interface_mac *rlc)
{
  if (is_sdu()) {
    // Read data from RLC interface 
    rlc->read_pdu(lcid, *ptr, nof_bytes);
  } else {
    nof_bytes = sizeof_ce(lcid, parent->is_ul()); 
    memcpy(*ptr, w_payload_ce, nof_bytes*sizeof(uint8_t));    
  }
  *ptr += nof_bytes;
}

bool sch_subh::read_subheader(uint8_t** ptr)
{
  // Skip R
  bool e_bit    = (bool)    *(*ptr + 0) & 0x20;
  lcid          = (uint8_t) *(*ptr + 0) & 0x1f;
  if (is_sdu()) {
    if (e_bit) {
      F_bit     = (bool)    *(*ptr + 1) & 0x80;
      nof_bytes = (uint32_t)*(*ptr + 1) & 0x7f;
      if (F_bit) {
        nof_bytes = nof_bytes<<8 | (uint32_t) *(*ptr + 2) & 0xff;
      }
    } else {
      nof_bytes = 0; 
      F_bit = 0; 
    }
  } else {
    nof_bytes = sizeof_ce(lcid, parent->is_ul()); 
  }
  return e_bit;
}
void sch_subh::read_payload(uint8_t** ptr)
{
  payload = *ptr; 
  *ptr += nof_bytes;
}



// Table 6.1.3.1-1 Buffer size levels for BSR 
uint32_t btable[61] = {
  10, 12, 14, 17, 19, 22, 26, 31, 36, 42, 49, 57, 67, 78, 91, 107, 125, 146, 171, 200, 234, 274, 321, 376, 440, 515, 603, 706, 826, 967, 1132, 
  1326, 1552, 1817, 2127, 2490, 2915, 3413, 3995, 4667, 5476, 6411, 7505, 8787, 10287, 12043, 14099, 16507, 19325, 22624, 26487, 31009, 36304, 
  42502, 49759, 58255, 68201, 79846, 93479, 109439, 128125};

uint8_t sch_subh::buff_size_table(uint32_t buffer_size) {
  if (buffer_size == 0) {
    return 0; 
  } else if (buffer_size > 150000) {
    return 63;
  } else {
    for (int i=0;i<61;i++) {
      if (buffer_size < btable[i]) {
        return 1+i; 
      }      
    }
    return 62; 
  }
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
bool rar_pdu::write_packet(uint8_t* ptr, rlc_interface_mac *rlc)
{
  // Write Backoff Indicator, if any 
  if (has_backoff_indicator) {
    if (nof_subheaders > 0) {
      *(ptr) = 1<<7 | backoff_indicator&0xf;
    }
  }
  // Write RAR subheaders
  for (int i=0;i<nof_subheaders;i++) {
    subheaders[i].write_subheader(&ptr, i==nof_subheaders-1);
  }
  // Write payload 
  for (int i=0;i<nof_subheaders;i++) {
    subheaders[i].write_payload(&ptr, rlc);
  }
  // Set paddint to zeros (if any) 
  bzero(ptr, rem_len*sizeof(uint8_t));
  
  return true; 
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
  *(*ptr + 0) = (uint8_t) (is_last<<7 | 1<<6 | preamble & 0x3f);
}
// Section 6.2.3
void rar_subh::write_payload(uint8_t** ptr, rlc_interface_mac *rlc)
{
  *(*ptr + 0) = (uint8_t)  (ta&0x7f0)>>4;
  *(*ptr + 1) = (uint8_t)  (ta&0xf)  <<4 | grant[0]<<3 | grant[1] << 2 | grant[2] << 1 | grant[3];
  uint8_t *x = &grant[4];
  *(*ptr + 2) = (uint8_t) srslte_bit_unpack(&x, 8);
  *(*ptr + 3) = (uint8_t) srslte_bit_unpack(&x, 8);
  *(*ptr + 4) = (uint8_t) ((temp_rnti&0xff00) >> 8);
  *(*ptr + 5) = (uint8_t)  (temp_rnti&0x00ff);
}

void rar_subh::read_payload(uint8_t** ptr)
{
  ta = *(*ptr + 0)&0x7f << 4 | (*(*ptr + 1)&0xf0)>>4;
  grant[0] = *(*ptr + 1)&0x8;
  grant[1] = *(*ptr + 1)&0x4;
  grant[2] = *(*ptr + 1)&0x2;
  grant[3] = *(*ptr + 1)&0x1;
  uint8_t *x = &grant[4];
  srslte_bit_pack(*(*ptr+2), &x, 8);
  srslte_bit_pack(*(*ptr+3), &x, 8);
  temp_rnti = *(*ptr + 4)<<8 | *(*ptr + 5);    
}

bool rar_subh::read_subheader(uint8_t** ptr)
{
  bool e_bit = *(*ptr + 0) & 0x80;
  bool type  = *(*ptr + 0) & 0x40;
  if (type) {
    preamble = *(*ptr + 0) & 0x3f;
  } else {
    ((rar_pdu*)parent)->set_backoff(*(*ptr + 0) & 0xf);
  }
  return e_bit;
}

    
  }
}
