/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
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

#include "srslte/common/pdu.h"
#include "srslte/srslte.h"

// Table 6.1.3.1-1 Buffer size levels for BSR 
static uint32_t btable[64] = {
  0, 1, 10, 12, 14, 17, 19, 22, 26, 31, 36, 42, 49, 57, 67, 78, 91, 107, 125, 146, 171, 200, 234, 274, 321, 376, 440, 515, 603, 706, 826, 967, 1132,
  1326, 1552, 1817, 2127, 2490, 2915, 3413, 3995, 4667, 5476, 6411, 7505, 8787, 10287, 12043, 14099, 16507, 19325, 22624, 26487, 31009, 36304, 
  42502, 49759, 58255, 68201, 79846, 93479, 109439, 128125, 150000};



namespace srslte {

void sch_pdu::fprint(FILE* stream)
{
  fprintf(stream, "MAC SDU for UL/DL-SCH. ");
  pdu::fprint(stream);
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

    int n_sub = pdu_len-read_len-1;

    if (n_sub >= 0) {
      subheaders[nof_subheaders-1].set_payload_size(n_sub);
    } else {
      fprintf(stderr,"Reading MAC PDU: negative payload for last subheader\n");
    }
  }
}

uint8_t* sch_pdu::write_packet() {
  return write_packet(NULL);
}

/* Writes the MAC PDU in the packet, including the MAC headers and CE payload. Section 6.1.2 */
uint8_t* sch_pdu::write_packet(srslte::log *log_h)
{
  int init_rem_len=rem_len; 
  sch_subh padding; 
  padding.set_padding();

  if (nof_subheaders <= 0 && nof_subheaders < (int)max_subheaders) {
    log_h->error("Trying to write packet with invalid number of subheaders (nof_subheaders=%d).\n", nof_subheaders);
    return NULL;
  }

  if (init_rem_len < 0) {
    log_h->error("init_rem_len=%d\n", init_rem_len);
    return NULL; 
  }

  /* If last SDU has zero payload, remove it. FIXME: Why happens this?? */
  if (subheaders[nof_subheaders-1].get_payload_size() == 0) {
    del_subh();
  }
  
  /* Determine if we are transmitting CEs only. */
  bool ce_only = last_sdu_idx<0?true:false; 
  
  /* Determine if we will need multi-byte padding or 1/2 bytes padding */
  bool multibyte_padding  = false;
  uint32_t onetwo_padding = 0; 
  if (rem_len > 2) {
    multibyte_padding = true; 
    // Add 1 header for padding 
    rem_len--; 
    // Add the header for the last SDU
    if (!ce_only) {
      rem_len -= (subheaders[last_sdu_idx].get_header_size(false)-1); // Becuase we were assuming it was the one
    }
  } else if (rem_len > 0) {
    onetwo_padding = rem_len; 
    rem_len = 0;  
  }


  /* Determine the header size and CE payload size */
  uint32_t header_sz     = 0; 
  uint32_t ce_payload_sz = 0; 
  for (int i=0;i<nof_subheaders;i++) {
    header_sz += subheaders[i].get_header_size(!multibyte_padding && i==last_sdu_idx);
    if (!subheaders[i].is_sdu()) {
      ce_payload_sz += subheaders[i].get_payload_size();
    }
  }
  if (multibyte_padding) {
    header_sz += 1; 
  } else if (onetwo_padding) {
    header_sz += onetwo_padding;
  }
  if (ce_payload_sz + header_sz >= sdu_offset_start) {
    fprintf(stderr, "Writing PDU: header sz + ce_payload_sz >= sdu_offset_start (%d>=%d). pdu_len=%d, total_sdu_len=%d\n",
            header_sz + ce_payload_sz, sdu_offset_start, pdu_len, total_sdu_len);
    return NULL; 
  }

  /* Start writing header and CE payload before the start of the SDU payload */
  uint8_t *ptr = &buffer_tx[sdu_offset_start-header_sz-ce_payload_sz];
  uint8_t *pdu_start_ptr = ptr; 
  
  // Add single/two byte padding first 
  for (uint32_t i=0;i<onetwo_padding;i++) {
    padding.write_subheader(&ptr, false);  
  }
  // Then write subheaders for MAC CE
  for (int i=0;i<nof_subheaders;i++) {
    if (!subheaders[i].is_sdu()) {
      subheaders[i].write_subheader(&ptr, ce_only && !multibyte_padding && i==(nof_subheaders-1));
    }
  }
  // Then for SDUs
  if (!ce_only) {
    for (int i=0;i<nof_subheaders;i++) {
      if (subheaders[i].is_sdu()) {
        subheaders[i].write_subheader(&ptr, !multibyte_padding && i==last_sdu_idx);
      }
    }
  }
  // and finally add multi-byte padding
  if (multibyte_padding) {
    sch_subh padding_multi; 
    padding_multi.set_padding(rem_len); 
    padding_multi.write_subheader(&ptr, true);
  }
  
  // Write CE payloads (SDU payloads already in the buffer)
  for (int i=0;i<nof_subheaders;i++) {
    if (!subheaders[i].is_sdu()) {      
      subheaders[i].write_payload(&ptr);
    }
  }
  // Set padding to zeros (if any) 
  if (rem_len > 0) {
    bzero(&pdu_start_ptr[pdu_len-rem_len], rem_len*sizeof(uint8_t));
  }

  /* Sanity check and print if error */
  if (log_h) {
    log_h->debug("Wrote PDU: pdu_len=%d, header_and_ce=%d (%d+%d), nof_subh=%d, last_sdu=%d, sdu_len=%d, onepad=%d, multi=%d\n", 
         pdu_len, header_sz+ce_payload_sz, header_sz, ce_payload_sz, 
         nof_subheaders, last_sdu_idx, total_sdu_len, onetwo_padding, rem_len);
  } else {
    printf("Wrote PDU: pdu_len=%d, header_and_ce=%d (%d+%d), nof_subh=%d, last_sdu=%d, sdu_len=%d, onepad=%d, multi=%d, init_rem_len=%d\n", 
         pdu_len, header_sz+ce_payload_sz, header_sz, ce_payload_sz, 
         nof_subheaders, last_sdu_idx, total_sdu_len, onetwo_padding, rem_len, init_rem_len);
  }
  
  if (rem_len + header_sz + ce_payload_sz + total_sdu_len != pdu_len) {
    printf("\n------------------------------\n");
    for (int i=0;i<nof_subheaders;i++) {
      printf("SUBH %d is_sdu=%d, payload=%d\n", i, subheaders[i].is_sdu(), subheaders[i].get_payload_size());
    }
    printf("Wrote PDU: pdu_len=%d, header_and_ce=%d (%d+%d), nof_subh=%d, last_sdu=%d, sdu_len=%d, onepad=%d, multi=%d, init_rem_len=%d\n", 
         pdu_len, header_sz+ce_payload_sz, header_sz, ce_payload_sz, 
         nof_subheaders, last_sdu_idx, total_sdu_len, onetwo_padding, rem_len, init_rem_len);
    fprintf(stderr, "Expected PDU len %d bytes but wrote %d\n", pdu_len, rem_len + header_sz + ce_payload_sz + total_sdu_len);
    printf("------------------------------\n");
    
    if (log_h) {
      log_h->error("Wrote PDU: pdu_len=%d, header_and_ce=%d (%d+%d), nof_subh=%d, last_sdu=%d, sdu_len=%d, onepad=%d, multi=%d, init_rem_len=%d\n", 
         pdu_len, header_sz+ce_payload_sz, header_sz, ce_payload_sz, 
         nof_subheaders, last_sdu_idx, total_sdu_len, onetwo_padding, rem_len, init_rem_len);
    
    }
    
    return NULL; 
  }

  if ((int)(header_sz + ce_payload_sz) != (int) (ptr - pdu_start_ptr)) {
    fprintf(stderr, "Expected a header and CE payload of %d bytes but wrote %d\n", 
            header_sz+ce_payload_sz,(int) (ptr - pdu_start_ptr));
    return NULL;
  }
  
  return pdu_start_ptr; 
}

int sch_pdu::rem_size() {
  return rem_len; 
}

int sch_pdu::get_pdu_len()
{
  return pdu_len; 
}

uint32_t sch_pdu::size_header_sdu(uint32_t nbytes)
{
  if (nbytes < 128) {
    return 2; 
  } else {
    return 3; 
  }
}

bool sch_pdu::has_space_ce(uint32_t nbytes, bool var_len)
{
  uint32_t head_len = var_len ? size_header_sdu(nbytes) : 1;
  if (rem_len >= nbytes + head_len) {
    return true; 
  } else {
    return false; 
  }
}

bool sch_pdu::update_space_ce(uint32_t nbytes, bool var_len)
{
  uint32_t head_len = var_len ? size_header_sdu(nbytes) : 1;
  if (has_space_ce(nbytes)) {
    rem_len      -= nbytes + head_len;
    return true; 
  } else {
    return false; 
  }
}

bool sch_pdu::has_space_sdu(uint32_t nbytes)
{
  int s = get_sdu_space();

  if (s < 0) {
    return false;
  } else {
    return (uint32_t)s >= nbytes;
  }
}

bool sch_pdu::update_space_sdu(uint32_t nbytes)
{
  int init_rem = rem_len; 
  if (has_space_sdu(nbytes)) {
    if (last_sdu_idx < 0) {
      rem_len      -= (nbytes+1);
    } else {
      rem_len      -= (nbytes+1 + (size_header_sdu(subheaders[last_sdu_idx].get_payload_size())-1));
    }
    last_sdu_idx = cur_idx;
    return true;
  } else {
    return false;
  }
}

int sch_pdu::get_sdu_space()
{
  int ret; 
  if (last_sdu_idx < 0) {
    ret = rem_len - 1;
  } else {
    ret = rem_len - (size_header_sdu(subheaders[last_sdu_idx].get_payload_size())-1) - 1; 
  }
  return ret; 
}

void sch_subh::init()
{
  lcid             = 0;
  nof_bytes        = 0;
  payload          = NULL;
  nof_mch_sched_ce = 0;
  cur_mch_sched_ce = 0;
}

sch_subh::cetype sch_subh::ce_type()
{
  if (lcid >= PHR_REPORT && type == SCH_SUBH_TYPE) {
    return (cetype)lcid;
  }
  if(lcid >= MCH_SCHED_INFO && type == MCH_SUBH_TYPE) {
    return (cetype)lcid;
  }
  return (cetype)SDU;
}

void sch_subh::set_payload_size(uint32_t size) {
  nof_bytes = size; 
}

uint32_t sch_subh::size_plus_header() {
  if (is_sdu() || is_var_len_ce()) {
    return sch_pdu::size_header_sdu(nof_bytes) + nof_bytes; 
  }
  // All others are 1-byte headers
  return 1 + nof_bytes;
}

uint32_t sch_subh::sizeof_ce(uint32_t lcid, bool is_ul)
{
  if (type == SCH_SUBH_TYPE) {
    if (is_ul) {
      switch(lcid) {
        case PHR_REPORT:
          return 1;
        case CRNTI:
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
  if (type == MCH_SUBH_TYPE) {
    switch (lcid) {
    case MCH_SCHED_INFO:
      return nof_mch_sched_ce*2;
    case PADDING:
      return 0;
    }
  }
  return 0;
}

bool sch_subh::is_sdu()
{
  return ce_type() == SDU;
}

bool sch_subh::is_var_len_ce()
{
  return (MCH_SCHED_INFO == ce_type()) && (MCH_SUBH_TYPE == type);
}

uint16_t sch_subh::get_c_rnti()
{
  if (payload) {
    return (uint16_t) payload[0]<<8 | payload[1];
  } else {
    return (uint16_t) w_payload_ce[0]<<8 | w_payload_ce[1];
  }
}

uint64_t sch_subh::get_con_res_id()
{
  if (payload) {
    return ((uint64_t) payload[5]) | (((uint64_t) payload[4])<<8) | (((uint64_t) payload[3])<<16) | (((uint64_t) payload[2])<<24) |
           (((uint64_t) payload[1])<<32) | (((uint64_t) payload[0])<<40);                
  } else {
    return ((uint64_t) w_payload_ce[5]) | (((uint64_t) w_payload_ce[4])<<8) | (((uint64_t) w_payload_ce[3])<<16) | (((uint64_t) w_payload_ce[2])<<24) |
           (((uint64_t) w_payload_ce[1])<<32) | (((uint64_t) w_payload_ce[0])<<40);                
    return 0; 
  }
}

float sch_subh::get_phr()
{
  if (payload) {
    return (float) (payload[0]&0x3f) - 23;
  } else {
    return (float) (w_payload_ce[0]&0x3f) - 23;
  }
}

int sch_subh::get_bsr(uint32_t buff_size[4])
{
  if (payload) {
    uint32_t nonzero_lcg = 0;
    if (ce_type()==LONG_BSR) {
      buff_size[0] = (payload[0]&0xFC) >> 2;
      buff_size[1] = (payload[0]&0x03) << 4 | (payload[1]&0xF0) >> 4;
      buff_size[2] = (payload[1]&0x0F) << 4 | (payload[1]&0xC0) >> 6;
      buff_size[3] = (payload[2]&0x3F); 
    } else {
      nonzero_lcg              = (payload[0]&0xc0) >> 6;
      buff_size[nonzero_lcg%4] =  payload[0]&0x3f;
    }
    for (int i=0;i<4;i++) {
      if (buff_size[i]) {
        if (buff_size[i]<63) {
          buff_size[i] = btable[1+buff_size[i]];
        } else {
          buff_size[i] = btable[63];
        }
      }
    }
    return nonzero_lcg;
  } else {
    return -1; 
  }
}

bool sch_subh::get_next_mch_sched_info(uint8_t *lcid_, uint16_t *mtch_stop)
{
  if(payload) {
    nof_mch_sched_ce = nof_bytes/2;
    if(cur_mch_sched_ce < nof_mch_sched_ce) {
      *lcid_ = (payload[cur_mch_sched_ce*2]&0xF8) >> 3;
      *mtch_stop = ((uint16_t)(payload[cur_mch_sched_ce*2]&0x07)) << 8;
      *mtch_stop += payload[cur_mch_sched_ce*2+1];
      cur_mch_sched_ce++;
      return true;
    }
  }
  return false;
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


uint32_t sch_subh::get_payload_size()

{
  return nof_bytes;
}

uint32_t sch_subh::get_header_size(bool is_last) {
  if (!is_last) {
    if (is_sdu()) {
      return sch_pdu::size_header_sdu(nof_bytes);
    }
    if (lcid == MCH_SCHED_INFO && type == MCH_SUBH_TYPE) {
      return sch_pdu::size_header_sdu(nof_bytes);
    }
    return 1; // All others are 1-byte
  } else {
    return 1; // Last subheader (CE or SDU) has always 1 byte header
  }
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

bool sch_subh::set_bsr(uint32_t buff_size[4], sch_subh::cetype format)
{
  uint32_t nonzero_lcg=0;
  for (int i=0;i<4;i++) {
    if (buff_size[i]) {
      nonzero_lcg=i;
    }
  }
  uint32_t ce_size = format==LONG_BSR?3:1;
  if (((sch_pdu*)parent)->has_space_ce(ce_size)) {
    if (format==LONG_BSR) {
      w_payload_ce[0] = (buff_size_table(buff_size[0])&0x3f) << 2 | (buff_size_table(buff_size[1])&0xc0)>>6;
      w_payload_ce[1] = (buff_size_table(buff_size[1])&0xf)  << 4 | (buff_size_table(buff_size[2])&0xf0)>>4;
      w_payload_ce[2] = (buff_size_table(buff_size[2])&0x3)  << 6 | (buff_size_table(buff_size[3])&0x3f);
    } else {
      w_payload_ce[0] = (nonzero_lcg&0x3)<<6 | (buff_size_table(buff_size[nonzero_lcg])&0x3f);
    }
    lcid = format;
    ((sch_pdu*)parent)->update_space_ce(ce_size);
    nof_bytes = ce_size;
    return true; 
  } else {
    return false; 
  }  
}

bool sch_subh::set_c_rnti(uint16_t crnti)
{
  if (((sch_pdu*)parent)->has_space_ce(2)) {
    w_payload_ce[0] = (uint8_t) ((crnti&0xff00)>>8);
    w_payload_ce[1] = (uint8_t) ((crnti&0x00ff));
    lcid = CRNTI;
    ((sch_pdu*)parent)->update_space_ce(2);
    nof_bytes = 2; 
    return true; 
  } else {
    return false; 
  }
}
bool sch_subh::set_con_res_id(uint64_t con_res_id)
{
  if (((sch_pdu*)parent)->has_space_ce(6)) {
    w_payload_ce[0] = (uint8_t) ((con_res_id&0xff0000000000)>>40);
    w_payload_ce[1] = (uint8_t) ((con_res_id&0x00ff00000000)>>32);
    w_payload_ce[2] = (uint8_t) ((con_res_id&0x0000ff000000)>>24);
    w_payload_ce[3] = (uint8_t) ((con_res_id&0x000000ff0000)>>16);
    w_payload_ce[4] = (uint8_t) ((con_res_id&0x00000000ff00)>>8);
    w_payload_ce[5] = (uint8_t) ((con_res_id&0x0000000000ff));
    lcid = CON_RES_ID;
    ((sch_pdu*)parent)->update_space_ce(6);
    nof_bytes = 6; 
    return true; 
  } else {
    return false; 
  }
}
bool sch_subh::set_phr(float phr)
{
  if (((sch_pdu*)parent)->has_space_ce(1)) {
    w_payload_ce[0] = phr_report_table(phr)&0x3f; 
    lcid = PHR_REPORT;
    ((sch_pdu*)parent)->update_space_ce(1);
    nof_bytes = 1; 
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
    nof_bytes = 1; 
    return true; 
  } else {
    return false; 
  }
}

bool sch_subh::set_next_mch_sched_info(uint8_t lcid_, uint16_t mtch_stop)
{
  if (((sch_pdu*)parent)->has_space_ce(2, true)) {
    w_payload_ce[nof_mch_sched_ce*2] = (lcid_&0x1F) << 3 | (uint8_t) ((mtch_stop&0x0700)>>8);
    w_payload_ce[nof_mch_sched_ce*2+1] = (uint8_t) (mtch_stop&0xff);
    nof_mch_sched_ce++;
    lcid = MCH_SCHED_INFO;
    ((sch_pdu*)parent)->update_space_ce(2, true);
    nof_bytes += 2;
    return true;
  }
  return false;
}

int sch_subh::set_sdu(uint32_t lcid_, uint32_t requested_bytes, read_pdu_interface *sdu_itf)
{
  if (((sch_pdu*)parent)->has_space_sdu(requested_bytes)) {
    lcid = lcid_;
    
    payload = ((sch_pdu*)parent)->get_current_sdu_ptr();
    // Copy data and get final number of bytes written to the MAC PDU 
    int sdu_sz = sdu_itf->read_pdu(lcid, payload, requested_bytes);
    
    if (sdu_sz < 0) {
      return -1;
    } 
    if (sdu_sz == 0) {
      return 0; 
    }
    else {
      // Save final number of written bytes
      nof_bytes = sdu_sz;

      if(nof_bytes > (int32_t)requested_bytes) {
         return -1;
      }
    }

    ((sch_pdu*)parent)->add_sdu(nof_bytes);
    ((sch_pdu*)parent)->update_space_sdu(nof_bytes);
    return nof_bytes; 
  } else {
    return -1; 
  }
}

int sch_subh::set_sdu(uint32_t lcid_, uint32_t nof_bytes_, uint8_t *payload)
{
  if (((sch_pdu*)parent)->has_space_sdu(nof_bytes_)) {
    lcid = lcid_;
    
    memcpy(((sch_pdu*)parent)->get_current_sdu_ptr(), payload, nof_bytes_);
    
    ((sch_pdu*)parent)->add_sdu(nof_bytes_);
    ((sch_pdu*)parent)->update_space_sdu(nof_bytes_);
    nof_bytes = nof_bytes_;

    return (int) nof_bytes; 
  } else {
    return -1; 
  }
}


// Section 6.2.1
void sch_subh::write_subheader(uint8_t** ptr, bool is_last)
{
  *(*ptr)   = (uint8_t) (is_last?0:(1<<5)) | ((uint8_t) lcid & 0x1f);
  *ptr += 1;
  if (is_sdu() || is_var_len_ce()) {
    // MAC SDU: R/R/E/LCID/F/L subheader
    // 2nd and 3rd octet
    if (!is_last) {
      if (nof_bytes >= 128) {
        *(*ptr) = (uint8_t) 1<<7 | ((nof_bytes & 0x7f00) >> 8);
        *ptr += 1;
        *(*ptr) = (uint8_t) (nof_bytes & 0xff);
        *ptr += 1;
      } else {
       *(*ptr) = (uint8_t) (nof_bytes & 0x7f); 
       *ptr += 1;
     }      
    }
  } 
}

void sch_subh::write_payload(uint8_t** ptr)
{
  if (is_sdu()) {
    // SDU is written directly during subheader creation
  } else {
    nof_bytes = sizeof_ce(lcid, parent->is_ul());
    memcpy(*ptr, w_payload_ce, nof_bytes*sizeof(uint8_t));
  }
  *ptr += nof_bytes;
}

bool sch_subh::read_subheader(uint8_t** ptr)
{
  // Skip R
  bool e_bit    = (bool)    (*(*ptr) & 0x20)?true:false;
  lcid          = (uint8_t) *(*ptr) & 0x1f;
  *ptr += 1;
  if (is_sdu() || is_var_len_ce()) {
    if (e_bit) {
      F_bit     = (bool)    (*(*ptr) & 0x80)?true:false;
      nof_bytes = (uint32_t)*(*ptr) & 0x7f;
      *ptr += 1;
      if (F_bit) {
        nof_bytes = nof_bytes<<8 | ((uint32_t) *(*ptr) & 0xff);
        *ptr += 1;
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

void sch_subh::fprint(FILE* stream)
{
  if (is_sdu()) {
    fprintf(stream, "SDU LCHID=%d, SDU nof_bytes=%d\n", lcid, nof_bytes);
  } else if (type == SCH_SUBH_TYPE) {
    if (parent->is_ul()) {
      switch(lcid) {
        case CRNTI:
          fprintf(stream, "C-RNTI CE\n");
          break;
        case PHR_REPORT:
          fprintf(stream, "PHR\n");
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
  } else if (type == MCH_SUBH_TYPE) {
    switch(lcid) {
      case MCH_SCHED_INFO:
        fprintf(stream, "MCH Scheduling Info CE\n");
        break;
      case PADDING:
        fprintf(stream, "PADDING\n");
    }
  }
}

uint8_t sch_subh::buff_size_table(uint32_t buffer_size) {
  if (buffer_size == 0) {
    return 0; 
  } else if (buffer_size > 150000) {
    return 63;
  } else {
    for (int i=0;i<61;i++) {
      if (buffer_size < btable[i+2]) {
        return 1+i; 
      }      
    }
    return 62; 
  }
}
  
// Implements Table 9.1.8.4-1 Power headroom report mapping (36.133)
uint8_t sch_subh::phr_report_table(float phr_value)
{
  if (phr_value < -23) {
    phr_value = -23; 
  }
  if (phr_value > 40) {
    phr_value = 40;
  }
  return (uint8_t) floor(phr_value+23);
}

void rar_pdu::fprint(FILE* stream)
{
  fprintf(stream, "MAC PDU for RAR. ");
  if (has_backoff_indicator) {
    fprintf(stream, "Backoff Indicator %d. ", backoff_indicator);
  }
  pdu::fprint(stream);  
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
    *(ptr) = backoff_indicator&0xf;
    if (nof_subheaders > 0) {
      *(ptr) = 1<<7;
    }
    ptr++;
  }

  // Write RAR subheaders
  for (int i=0;i<nof_subheaders;i++) {
    subheaders[i].write_subheader(&ptr, i==nof_subheaders-1);
  }
  // Write payload 
  for (int i=0;i<nof_subheaders;i++) {
    subheaders[i].write_payload(&ptr);
  }
  // Set padding to zeros (if any) 
  bzero(ptr, rem_len*sizeof(uint8_t));
  
  return true; 
}

void rar_subh::fprint(FILE* stream)
{
  fprintf(stream, "RAPID: %d, Temp C-RNTI: %d, TA: %d, UL Grant: ", preamble, temp_rnti, ta);
  srslte_vec_fprint_hex(stream, grant, 20);
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
  memcpy(grant_, grant, sizeof(uint8_t)*RAR_GRANT_LEN);
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
  memcpy(grant, grant_, sizeof(uint8_t)*RAR_GRANT_LEN);
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
  *(*ptr) = (uint8_t) (!is_last<<7 | 1<<6 | (preamble & 0x3f));
  *ptr += 1;
}
// Section 6.2.3
void rar_subh::write_payload(uint8_t** ptr)
{
  *(*ptr + 0) = (uint8_t)  ((ta&0x7f0)>>4);
  *(*ptr + 1) = (uint8_t)  ((ta&0xf)  <<4) | (grant[0]<<3) | (grant[1]<<2) | (grant[2]<<1) | grant[3];
  uint8_t *x = &grant[4];
  *(*ptr + 2) = (uint8_t) srslte_bit_pack(&x, 8);
  *(*ptr + 3) = (uint8_t) srslte_bit_pack(&x, 8);
  *(*ptr + 4) = (uint8_t) ((temp_rnti&0xff00) >> 8);
  *(*ptr + 5) = (uint8_t)  (temp_rnti&0x00ff);
  *ptr += 6;
}

void rar_subh::read_payload(uint8_t** ptr)
{
  ta = ((uint32_t) *(*ptr + 0)&0x7f)<<4 | (*(*ptr + 1)&0xf0)>>4;
  grant[0] = *(*ptr + 1)&0x8?1:0;
  grant[1] = *(*ptr + 1)&0x4?1:0;
  grant[2] = *(*ptr + 1)&0x2?1:0;
  grant[3] = *(*ptr + 1)&0x1?1:0;
  uint8_t *x = &grant[4];
  srslte_bit_unpack(*(*ptr+2), &x, 8);
  srslte_bit_unpack(*(*ptr+3), &x, 8);
  temp_rnti = ((uint16_t) *(*ptr + 4))<<8 | *(*ptr + 5);    
  *ptr += 6;
}

bool rar_subh::read_subheader(uint8_t** ptr)
{
  bool e_bit = *(*ptr) & 0x80?true:false;
  bool type  = *(*ptr) & 0x40?true:false;
  if (type) {
    preamble = *(*ptr) & 0x3f;
  } else {
    ((rar_pdu*)parent)->set_backoff(*(*ptr) & 0xf);
  }
  *ptr += 1; 
  return e_bit;
}
    
}
