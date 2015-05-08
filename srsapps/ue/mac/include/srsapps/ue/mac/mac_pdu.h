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

#ifndef MACPDU_H
#define MACPDU_H

/* MAC PDU Packing/Unpacking functions */   

namespace srslte {
namespace ue {

class mac_pdu
{
public:
  
  class mac_subh
  {
  public: 
    
    typedef enum {
      C_RNTI = 0,
      CON_RES_ID,
      TA_CMD,
      PHD,
      SDU
    } cetype; 
    
    // Reading functions
    bool     is_sdu();
    cetype   ce_type();
    
    uint32_t get_sdu_lcid();
    uint32_t get_sdu_nbytes();
    uint8_t* get_sdu_ptr();
    
    uint16_t get_c_rnti();
    uint64_t get_con_res_id();
    uint8_t  get_ta_cmd();
    uint8_t  get_phd();
    
    // Writing functions
    bool     set_sdu(uint8_t *ptr, uint32_t nof_bytes);
    bool     set_c_rnti(uint16_t crnti);
    bool     set_con_res_id(uint64_t con_res_id);
    bool     set_ta_cmd(uint8_t ta_cmd);
    bool     set_phd(uint8_t phd);
        
  private: 
    static const int MAX_CE_PAYLOAD_LEN = 8; 
    uint32_t lcid;
    uint32_t nof_bytes; 
    uint8_t* sdu_payload_ptr;   
    uint8_t  ce_payload[MAX_CE_PAYLOAD_LEN];
  };

  mac_pdu(uint32_t max_subheaders);
  
  void      reset();
  void      init(uint32_t pdu_len);

  bool      read_next();
  bool      write_next();
  mac_subh* get();
  
  bool      write_packet(uint8_t *ptr);
  void      parse_packet(uint8_t *ptr); 
  
  
  
protected:  
  mac_subh   *subheaders;
  uint32_t   pdu_len; 
  uint32_t   rem_len; 
  uint32_t   rp, wp;
  uint32_t   nof_subheaders; 
  uint32_t   max_subheaders; 
};

class mac_rar_pdu 
{
public:
  class mac_rar
  {

  public:
  
    static const uint32_t RAR_GRANT_LEN = 20; 

    // Reading functions
    uint32_t get_rapid();
    uint32_t get_ta_cmd();
    uint16_t get_temp_crnti();
    void     get_sched_grant(uint8_t grant[RAR_GRANT_LEN]);
    
    // Writing functoins
    void     set_rapid(uint32_t rapid);
    void     set_ta_cmd(uint32_t ta);
    void     set_temp_crnti(uint16_t temp_rnti);
    void     set_sched_grant(uint8_t grant[RAR_GRANT_LEN]);
  private: 
    uint8_t grant[RAR_GRANT_LEN];
    uint32_t ta; 
    uint16_t temp_rnti; 
  };

  mac_rar_pdu(uint32_t max_rars);
  
  void      reset();
  void      init(uint32_t pdu_len);

  bool      read_next();
  bool      write_next();
  mac_rar*  get();
  
  void      set_backoff(uint8_t bi);
  bool      is_backoff();
  uint8_t   get_backoff();
  
  bool      write_packet(uint8_t *ptr);
  void      parse_packet(uint8_t *ptr); 
  
  
private: 
  mac_rar   *rars;
  uint32_t   pdu_len; 
  uint32_t   rem_len; 
  uint32_t   rp, wp;
  uint32_t   nof_rars; 
  uint32_t   max_rars; 
  
};

}
}

#endif

