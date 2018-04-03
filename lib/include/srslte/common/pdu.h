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

#ifndef SRSLTE_PDU_H
#define SRSLTE_PDU_H

#include <stdint.h>
#include "srslte/common/log.h"
#include "srslte/common/interfaces_common.h"
#include <vector>
#include <stdio.h>

/* MAC PDU Packing/Unpacking functions. Section 6 of 36.321 */   


namespace srslte {  
  
template<class SubH>
class pdu
{
public:
  
  pdu(uint32_t max_subheaders_) : subheaders(max_subheaders_) {
    max_subheaders = max_subheaders_; 
    nof_subheaders = 0; 
    cur_idx        = -1; 
    pdu_len        = 0; 
    rem_len        = 0;   
    last_sdu_idx   = -1;
    pdu_is_ul      = false; 
    buffer_tx      = NULL; 
    total_sdu_len  = 0; 
  }
  
  void fprint(FILE *stream) {
    fprintf(stream, "Number of Subheaders: %d\n", nof_subheaders);
    for (int i=0;i<nof_subheaders;i++) {
      fprintf(stream, " -- Subheader %d: ", i);
      subheaders[i].fprint(stream);
    }
  }
  
  /* Resets the Read/Write position and remaining PDU length */
  void reset() {
    cur_idx      = -1; 
    last_sdu_idx = -1; 
    rem_len = pdu_len;
  }

  void init_rx(uint32_t pdu_len_bytes, bool is_ulsch = false) {
    init_(NULL, pdu_len_bytes, is_ulsch);
  }

  void init_tx(uint8_t *payload, uint32_t pdu_len_bytes, bool is_ulsch = false) {
    init_(payload, pdu_len_bytes, is_ulsch);
  }

  uint32_t nof_subh() {
    return nof_subheaders;
  }
  
  bool new_subh() {
    if (nof_subheaders < (int)max_subheaders - 1 && rem_len > 0) {
      nof_subheaders++;
      next();
      return true; 
    } else {
      return false; 
    }
  }

  bool next() {
    if (cur_idx < nof_subheaders - 1) {
      cur_idx++;
      return true; 
    } else {
      return false; 
    }
  }
  
  void del_subh() {
    if (nof_subheaders > 0) {
      nof_subheaders--;
    }
    if (cur_idx > 0) {
      cur_idx--;
    }
  }

  SubH* get() {
    if (cur_idx >= 0) {
      return &subheaders[cur_idx];
    } else {
      return NULL; 
    }
  }
  
  bool is_ul() {
    return pdu_is_ul;
  }
  
  uint8_t* get_current_sdu_ptr() {
    return &buffer_tx[total_sdu_len+sdu_offset_start];    
  }
  
  void add_sdu(uint32_t sdu_sz) {
    total_sdu_len += sdu_sz; 
  }

  // Section 6.1.2
  void parse_packet(uint8_t *ptr) {
    uint8_t *init_ptr = ptr; 
    nof_subheaders = 0;
    bool ret = false;
    do {
      if (nof_subheaders < (int)max_subheaders) {
        ret = subheaders[nof_subheaders].read_subheader(&ptr);
        nof_subheaders++;
      }
    } while (ret && (nof_subheaders + 1) < (int)max_subheaders);

    for (int i=0;i<nof_subheaders;i++) {
      subheaders[i].read_payload(&ptr);
    }
  }

protected:  
  std::vector<SubH> subheaders;
  uint32_t   pdu_len; 
  uint32_t   rem_len; 
  int        cur_idx;
  int        nof_subheaders;
  uint32_t   max_subheaders; 
  bool       pdu_is_ul;
  uint8_t*   buffer_tx; 
  uint32_t   total_sdu_len; 
  uint32_t   sdu_offset_start; 
  int        last_sdu_idx;
  
private: 
  
  /* Prepares the PDU for parsing or writing by setting the number of subheaders to 0 and the pdu length */
  void init_(uint8_t *buffer_tx_ptr, uint32_t pdu_len_bytes, bool is_ulsch) {
    nof_subheaders = 0; 
    pdu_len        = pdu_len_bytes; 
    rem_len        = pdu_len; 
    pdu_is_ul      = is_ulsch; 
    buffer_tx      = buffer_tx_ptr; 
    sdu_offset_start = max_subheaders*2 + 13; // Assuming worst-case 2 bytes per sdu subheader + all possible CE
    total_sdu_len  = 0; 
    last_sdu_idx   = -1;
    reset();
    for (uint32_t i=0;i<max_subheaders;i++) {
      subheaders[i].parent = this; 
      subheaders[i].init();
    }
  }
};

template<class SubH>
class subh
{
public: 
      
  virtual bool read_subheader(uint8_t** ptr)                        = 0;
  virtual void read_payload(uint8_t **ptr)                          = 0;    
  virtual void write_subheader(uint8_t** ptr, bool is_last)         = 0;
  virtual void write_payload(uint8_t **ptr)                         = 0;
  virtual void fprint(FILE *stream)                                 = 0;

  pdu<SubH>* parent; 
private: 
  virtual void init() = 0;
};



class sch_subh : public subh<sch_subh>
{

public: 
  
  typedef enum {
    PHR_REPORT = 26,
    CRNTI      = 27,
    CON_RES_ID = 28,
    TRUNC_BSR  = 28,
    TA_CMD     = 29,
    SHORT_BSR  = 29,
    DRX_CMD    = 30,
    LONG_BSR   = 30,
    PADDING    = 31,
    SDU        = 0
  } cetype; 
  
  // Size of MAC CEs
  const static int MAC_CE_CONTRES_LEN = 6; 

  // Reading functions
  bool     is_sdu();
  cetype   ce_type();
  uint32_t size_plus_header();
  void     set_payload_size(uint32_t size);
  
  bool     read_subheader(uint8_t** ptr);
  void     read_payload(uint8_t **ptr);
  uint32_t get_sdu_lcid();
  int get_payload_size();
  uint32_t get_header_size(bool is_last);
  uint8_t* get_sdu_ptr();
  
  uint16_t get_c_rnti();
  uint64_t get_con_res_id();
  uint8_t  get_ta_cmd();
  float    get_phr();
  int      get_bsr(uint32_t buff_size[4]);
  
  // Writing functions
  void     write_subheader(uint8_t** ptr, bool is_last);
  void     write_payload(uint8_t **ptr);
  int      set_sdu(uint32_t lcid, uint32_t nof_bytes, uint8_t *payload);
  int      set_sdu(uint32_t lcid, uint32_t requested_bytes, read_pdu_interface *sdu_itf);
  bool     set_c_rnti(uint16_t crnti);
  bool     set_bsr(uint32_t buff_size[4], sch_subh::cetype format);
  bool     set_con_res_id(uint64_t con_res_id);
  bool     set_ta_cmd(uint8_t ta_cmd);
  bool     set_phr(float phr);
  void     set_padding();
  void     set_padding(uint32_t padding_len);

  void     init();
  void     fprint(FILE *stream);
  

private: 
  static const int MAX_CE_PAYLOAD_LEN = 8; 
  uint32_t lcid;
  int      nof_bytes; 
  uint8_t* payload; 
  uint8_t  w_payload_ce[8];
  bool     F_bit;    
  uint32_t sizeof_ce(uint32_t lcid, bool is_ul);
  static uint8_t buff_size_table(uint32_t buffer_size);
  static uint8_t phr_report_table(float phr_value);
};

class sch_pdu : public pdu<sch_subh>
{
public:
  
  sch_pdu(uint32_t max_subh) : pdu(max_subh) {}

  void      parse_packet(uint8_t *ptr);
  uint8_t*  write_packet();
  uint8_t*  write_packet(srslte::log *log_h);
  bool      has_space_ce(uint32_t nbytes);  
  bool      has_space_sdu(uint32_t nbytes);  
  int       get_pdu_len();
  int       rem_size(); 
  int       get_sdu_space();
  
  static uint32_t size_header_sdu(uint32_t nbytes);
  bool      update_space_ce(uint32_t nbytes);  
  bool      update_space_sdu(uint32_t nbytes);  
  void      fprint(FILE *stream);
};

class rar_subh : public subh<rar_subh>
{
public:

  static const uint32_t RAR_GRANT_LEN = 20; 
  
  // Reading functions
  bool     read_subheader(uint8_t** ptr);
  void     read_payload(uint8_t** ptr);
  uint32_t get_rapid();
  uint32_t get_ta_cmd();
  uint16_t get_temp_crnti();
  void     get_sched_grant(uint8_t grant[RAR_GRANT_LEN]);
  
  // Writing functoins
  void     write_subheader(uint8_t** ptr, bool is_last);
  void     write_payload(uint8_t** ptr);
  void     set_rapid(uint32_t rapid);
  void     set_ta_cmd(uint32_t ta);
  void     set_temp_crnti(uint16_t temp_rnti);
  void     set_sched_grant(uint8_t grant[RAR_GRANT_LEN]);
  
  void     init();
  void     fprint(FILE *stream);

private: 
  uint8_t  grant[RAR_GRANT_LEN];
  uint32_t ta; 
  uint16_t temp_rnti; 
  uint32_t preamble;
};

class rar_pdu : public pdu<rar_subh>
{
public:
  
  rar_pdu(uint32_t max_rars = 16);
    
  void     set_backoff(uint8_t bi);
  bool     has_backoff();
  uint8_t  get_backoff();
  
  bool     write_packet(uint8_t* ptr);
  void     fprint(FILE *stream);

private: 
  bool       has_backoff_indicator;
  uint8_t    backoff_indicator; 
};

} // namespace srslte

#endif // SRSLTE_PDU_H
