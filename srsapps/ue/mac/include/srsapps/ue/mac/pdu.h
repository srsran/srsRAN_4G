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


#include <stdint.h>
#include "srsapps/common/log.h"
#include <vector>
#include <stdio.h>


#ifndef MACPDU_H
#define MACPDU_H

/* MAC PDU Packing/Unpacking functions. Section 6 of 36.321 */   

namespace srslte {
namespace ue {

  
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
    for (int i=0;i<max_subheaders;i++) {
      subheaders[i].parent = this; 
    }
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
    cur_idx = -1; 
    rem_len = pdu_len;
  }

  /* Prepares the PDU for parsing or writing by setting the number of subheaders to 0 and the pdu length */
  void init(uint32_t pdu_len_bytes) {
    init(pdu_len_bytes, false);
  }
  void init(uint32_t pdu_len_bytes, bool is_ulsch) {
    nof_subheaders = 0; 
    pdu_len        = pdu_len_bytes; 
    rem_len        = pdu_len; 
    pdu_is_ul      = is_ulsch; 
    reset();
    for (int i=0;i<max_subheaders;i++) {
      subheaders[i].init();
    }
  }
  
  bool new_subh() {
    if (nof_subheaders < max_subheaders - 1) {
      nof_subheaders++;
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

  SubH* get() {
    if (cur_idx >= 0) {
      return &subheaders[cur_idx];
    }
  }
  
  // Section 6.1.2
  void parse_packet(uint8_t *ptr) {
    nof_subheaders = 0; 
    while(subheaders[nof_subheaders].read_subheader(&ptr)) {
      nof_subheaders++;
    }
    nof_subheaders++;
    for (int i=0;i<nof_subheaders;i++) {
      subheaders[i].read_payload(&ptr);
    }
  }
  bool is_ul() {
    return pdu_is_ul;
  }

  virtual bool write_packet(uint8_t *ptr) = 0;

protected:  
  std::vector<SubH> subheaders;
  uint32_t   pdu_len; 
  uint32_t   rem_len; 
  int        cur_idx;
  int        nof_subheaders; 
  uint32_t   max_subheaders; 
  bool       pdu_is_ul;
};

template<class SubH>
class subh
{
public: 
      
  virtual bool read_subheader(uint8_t** ptr)                 = 0;
  virtual void read_payload(uint8_t **ptr)                   = 0;    
  virtual void write_subheader(uint8_t** ptr, bool is_last)  = 0;
  virtual void write_payload(uint8_t **ptr)                  = 0;
  virtual void fprint(FILE *stream)                          = 0;

  pdu<SubH>* parent; 
  
private: 
  virtual void init() = 0;
};



class sch_subh : public subh<sch_subh>
{

public: 
  
  typedef enum {
    PHD_REPORT = 26,
    C_RNTI     = 27,
    CON_RES_ID = 28,
    TRUNC_BSR  = 28,
    TA_CMD     = 29,
    SHORT_BSR  = 29,
    DRX_CMD    = 30,
    LONG_BSR   = 30,
    PADDING    = 31,
    SDU        = 0
  } cetype; 
  
  // Reading functions
  bool     is_sdu();
  cetype   ce_type();
  
  bool     read_subheader(uint8_t** ptr);
  void     read_payload(uint8_t **ptr);
  uint32_t get_sdu_lcid();
  uint32_t get_sdu_nbytes();
  uint8_t* get_sdu_ptr();
  
  uint16_t get_c_rnti();
  uint64_t get_con_res_id();
  uint8_t  get_ta_cmd();
  uint8_t  get_phd();
  
  // Writing functions
  void     write_subheader(uint8_t** ptr, bool is_last);
  void     write_payload(uint8_t **ptr);
  bool     set_sdu(uint32_t lcid, uint8_t *ptr, uint32_t nof_bytes);
  bool     set_c_rnti(uint16_t crnti);
  bool     set_con_res_id(uint64_t con_res_id);
  bool     set_ta_cmd(uint8_t ta_cmd);
  bool     set_phd(uint8_t phd);
  void     set_padding();

  void     init();
  void     fprint(FILE *stream);
  
private: 
  static const int MAX_CE_PAYLOAD_LEN = 8; 
  uint32_t lcid;
  uint32_t nof_bytes; 
  uint8_t* sdu_payload_ptr; 
  bool     F_bit;    
  uint8_t  ce_payload[MAX_CE_PAYLOAD_LEN];
  uint32_t sizeof_ce(uint32_t lcid, bool is_ul);
};

class sch_pdu : public pdu<sch_subh>
{
public:
  
  sch_pdu(uint32_t max_rars) : pdu(max_rars) {}

  bool      write_packet(uint8_t *ptr);
  bool      has_space_ce(uint32_t nbytes);  
  bool      has_space_sdu(uint32_t nbytes);  
  static uint32_t size_plus_header_pdu(uint32_t nbytes);
  bool      update_space_ce(uint32_t nbytes);  
  bool      update_space_sdu(uint32_t nbytes);  
  void     fprint(FILE *stream);

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
  
  rar_pdu(uint32_t max_rars);
    
  void     set_backoff(uint8_t bi);
  bool     has_backoff();
  uint8_t  get_backoff();
  
  bool     write_packet(uint8_t* ptr);
  void     fprint(FILE *stream);

private: 
  bool       has_backoff_indicator;
  uint8_t    backoff_indicator; 
};

}
}

#endif

