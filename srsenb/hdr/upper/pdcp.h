/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2017 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of srsLTE.
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

#include <map>
#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/interfaces/enb_interfaces.h"
#include "srslte/upper/pdcp.h"

#ifndef SRSENB_PDCP_H
#define SRSENB_PDCP_H

namespace srsenb {
  
class pdcp :  public pdcp_interface_rlc, 
              public pdcp_interface_gtpu,
              public pdcp_interface_rrc
{
public:
 
  void init(rlc_interface_pdcp *rlc_, rrc_interface_pdcp *rrc_, gtpu_interface_pdcp *gtpu_, srslte::log *pdcp_log_);
  void stop(); 
  
  // pdcp_interface_rlc
  void write_pdu(uint16_t rnti, uint32_t lcid, srslte::byte_buffer_t *sdu); 
  
  // pdcp_interface_rrc
  void reset(uint16_t rnti);
  void add_user(uint16_t rnti);  
  void rem_user(uint16_t rnti); 
  void write_sdu(uint16_t rnti, uint32_t lcid, srslte::byte_buffer_t *sdu);
  void add_bearer(uint16_t rnti, uint32_t lcid, srslte::srslte_pdcp_config_t cnfg);
  void config_security(uint16_t rnti, 
                       uint32_t lcid,
                       uint8_t *k_rrc_enc_,
                       uint8_t *k_rrc_int_,
                       srslte::CIPHERING_ALGORITHM_ID_ENUM cipher_algo_,
                       srslte::INTEGRITY_ALGORITHM_ID_ENUM integ_algo_);
  
private: 
  
  class user_interface_rlc : public srsue::rlc_interface_pdcp
  {
  public:
    uint16_t rnti; 
    srsenb::rlc_interface_pdcp *rlc; 
    // rlc_interface_pdcp
    void write_sdu(uint32_t lcid,  srslte::byte_buffer_t *sdu);
    bool rb_is_um(uint32_t lcid);
  };
  
  class user_interface_gtpu : public srsue::gw_interface_pdcp
  {
  public: 
    uint16_t rnti; 
    srsenb::gtpu_interface_pdcp  *gtpu;
    // gw_interface_pdcp
    void write_pdu(uint32_t lcid, srslte::byte_buffer_t *pdu); 
  }; 
  
  class user_interface_rrc : public srsue::rrc_interface_pdcp
  {
  public: 
    uint16_t rnti; 
    srsenb::rrc_interface_pdcp *rrc;
    // rrc_interface_pdcp
    void write_pdu(uint32_t lcid, srslte::byte_buffer_t *pdu);
    void write_pdu_bcch_bch(srslte::byte_buffer_t *pdu);
    void write_pdu_bcch_dlsch(srslte::byte_buffer_t *pdu);
    void write_pdu_pcch(srslte::byte_buffer_t *pdu);
    std::string get_rb_name(uint32_t lcid);
  };
  
  class user_interface 
  {
  public: 
    user_interface_rlc  rlc_itf; 
    user_interface_gtpu gtpu_itf;
    user_interface_rrc  rrc_itf; 
    srslte::pdcp        *pdcp; 
  }; 
  
  std::map<uint32_t,user_interface> users; 
  
  rlc_interface_pdcp  *rlc;
  rrc_interface_pdcp  *rrc;
  gtpu_interface_pdcp *gtpu;
  srslte::log         *log_h;
  srslte::byte_buffer_pool *pool;
};

}

#endif // SRSENB_PDCP_H
