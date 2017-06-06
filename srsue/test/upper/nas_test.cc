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

#include <iostream>
#include "upper/usim.h"
#include "upper/nas.h"
#include "srslte/upper/rlc.h"
#include "upper/rrc.h"
#include "mac/mac.h"
#include "srslte/upper/pdcp_entity.h"
#include "srslte/upper/pdcp.h"
#include "srslte/common/log_stdout.h"
#include "srslte/interfaces/ue_interfaces.h"

using namespace srsue;



uint8_t pdu1[] = {
0x03, 0x22, 0x16, 0x15, 0xe8 , 0x00 , 0x00 , 0x03 , 0x13 , 0xb0 , 0x00 , 0x02 , 0x90 , 0x08,
0x79, 0xf0, 0x00, 0x00, 0x40 , 0xb5 , 0x01 , 0x25 , 0x40 , 0xcc , 0x1d , 0x08 , 0x04 , 0x3c , 0x18 , 0x00,
0x4c, 0x02, 0x20, 0x0f, 0xa8 , 0x00 , 0x65 , 0x48 , 0x07 , 0x04 , 0x04 , 0x24 , 0x1c , 0x19 , 0x05 , 0x41,
0x39, 0x39, 0x4d, 0x38, 0x14 , 0x04 , 0x28 , 0xd1 , 0x5e , 0x6d , 0x78 , 0x13 , 0xfb , 0xf9 , 0x01 , 0xb1,
0x40, 0x2f, 0xd8, 0x4c, 0x02 , 0x20 , 0x00 , 0x5b , 0x78 , 0x00 , 0x07 , 0xa1 , 0x25 , 0xa9 , 0xc1 , 0x3f,
0xd9, 0x40, 0x41, 0xf5, 0x1b , 0x58 , 0x2f , 0x27 , 0x28 , 0xa0 , 0xed , 0xde , 0x54 , 0x43 , 0x48 , 0xc0,
0x56, 0xcc, 0x00, 0x02, 0x84 , 0x00 , 0x42 , 0x0a , 0xf1 , 0x63 };

uint32_t PDU1_LEN = 104;


#define LCID 3

namespace srsue {

// fake classes
class pdcp_dummy : public rrc_interface_pdcp
{
public:
      void write_pdu(uint32_t lcid, byte_buffer_t *pdu) {}
      void write_pdu_bcch_bch(byte_buffer_t *pdu) {}
      void write_pdu_bcch_dlsch(byte_buffer_t *pdu) {}
      void write_pdu_pcch(byte_buffer_t *pdu) {}
};



class rrc_dummy : public rrc_interface_nas
{
public:
  void write_sdu(uint32_t lcid, byte_buffer_t *sdu)
  {

  }

  uint16_t get_mcc() { return 0x11; }
  uint16_t get_mnc() { return 0xff; }
  void enable_capabilities() {

  }
};

class gw_dummy : public gw_interface_nas, public gw_interface_pdcp
{
  error_t setup_if_addr(uint32_t ip_addr, char *err_str) {}
  void write_pdu(uint32_t lcid, byte_buffer_t *pdu) {}
};

}

class usim_dummy : public usim_interface_nas
{
  void get_imsi_vec(uint8_t* imsi_, uint32_t n){

  }
  void get_imei_vec(uint8_t* imei_, uint32_t n){

  }
  void generate_authentication_response(uint8_t  *rand,
                                                uint8_t  *autn_enb,
                                                uint16_t  mcc,
                                                uint16_t  mnc,
                                                bool     *net_valid,
                                                uint8_t  *res){

  }


  void generate_nas_keys(uint8_t *k_nas_enc,
                                   uint8_t *k_nas_int,
                                   CIPHERING_ALGORITHM_ID_ENUM cipher_algo,
                                   INTEGRITY_ALGORITHM_ID_ENUM integ_algo){

  }
};




int main(int argc, char **argv)
{
  srslte::log_stdout nas_log("NAS");
  srslte::log_stdout pdcp_entity_log("PDCP");
  srslte::log_stdout rrc_log("RRC");
  srslte::log_stdout mac_log("MAC");


  nas_log.set_level(srslte::LOG_LEVEL_DEBUG);
  pdcp_entity_log.set_level(srslte::LOG_LEVEL_DEBUG);
  rrc_log.set_level(srslte::LOG_LEVEL_DEBUG);

  nas_log.set_hex_limit(100000);
  rrc_log.set_hex_limit(100000);

  usim_dummy usim;
  rrc_dummy rrc_dummy;
  gw_dummy gw;

  pdcp_dummy pdcp_dummy;




  buffer_pool        *pool;
  pool = buffer_pool::get_instance();

  srsue::nas nas;
  nas.init(&usim, &rrc_dummy, &gw, &nas_log);




  byte_buffer_t* tmp = pool_allocate;
  memcpy(tmp->msg, &pdu1[0], PDU1_LEN);
  tmp->N_bytes = PDU1_LEN;

  //byte_buffer_t tmp2;
  //memcpy(tmp2.msg, &pdu1[0], PDU1_LEN);
  //tmp2.N_bytes = PDU1_LEN;

  //srsue::mac mac;
  //mac.init(NULL, NULL, NULL, &mac_log);

  srsue::rrc rrc;
  rrc.init(NULL, NULL, NULL, NULL, &nas, NULL, NULL, &rrc_log);
  //rrc.init(&phy, &mac, &rlc, &pdcp, &nas, &usim, &mac, &rrc_log);


  srsue::pdcp_entity pdcp_entity;
  pdcp_entity.init(NULL, &rrc, &gw, &pdcp_entity_log, RB_ID_SRB1, NULL);

  pdcp_entity.write_pdu(tmp);

  //rrc.write_sdu(RB_ID_SRB2, tmp);


  //nas.write_pdu(LCID, tmp);

  pool->cleanup();

}
