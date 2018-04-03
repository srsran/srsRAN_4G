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
#include <assert.h>
#include "srsue/hdr/upper/usim.h"
#include "srsue/hdr/upper/nas.h"
#include "srslte/upper/rlc.h"
#include "srsue/hdr/upper/rrc.h"
#include "srsue/hdr/mac/mac.h"
#include "srslte/common/log_filter.h"
#include "srslte/upper/pdcp_entity.h"
#include "srslte/upper/pdcp.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/common/bcd_helpers.h"


using namespace srsue;

#define LCID 1

uint8_t auth_request_pdu[] = { 0x07, 0x52, 0x01, 0x0c, 0x63, 0xa8, 0x54, 0x13, 0xe6, 0xa4,
                               0xce, 0xd9, 0x86, 0xfb, 0xe5, 0xce, 0x9b, 0x62, 0x5e, 0x10,
                               0x67, 0x57, 0xb3, 0xc2, 0xb9, 0x70, 0x90, 0x01, 0x0c, 0x72,
                               0x8a, 0x67, 0x57, 0x92, 0x52, 0xb8 };

uint8_t sec_mode_command_pdu[] = { 0x37, 0x37, 0xc7, 0x67, 0xae, 0x00, 0x07, 0x5d, 0x02, 0x01,
                                   0x02, 0xe0, 0x60, 0xc1 };

uint16 mcc = 61441;
uint16 mnc = 65281;

using namespace srslte;

namespace srslte {

// fake classes
class pdcp_dummy : public rrc_interface_pdcp
{
public:
      void write_pdu(uint32_t lcid, byte_buffer_t *pdu) {}
      void write_pdu_bcch_bch(byte_buffer_t *pdu) {}
      void write_pdu_bcch_dlsch(byte_buffer_t *pdu) {}
      void write_pdu_pcch(byte_buffer_t *pdu) {}
      std::string get_rb_name(uint32_t lcid) { return std::string("lcid"); }
};

class rrc_dummy : public rrc_interface_nas
{
public:
  void write_sdu(uint32_t lcid, byte_buffer_t *sdu)
  {
    printf("NAS generated SDU (len=%d):\n", sdu->N_bytes);
    last_sdu_len = sdu->N_bytes;
    srslte_vec_fprint_byte(stdout, sdu->msg, sdu->N_bytes);
    byte_buffer_pool::get_instance()->deallocate(sdu);
  }
  std::string get_rb_name(uint32_t lcid) { return std::string("lcid"); }
  uint32_t get_last_sdu_len() { return last_sdu_len; }

  void plmn_search() {};
  void plmn_select(LIBLTE_RRC_PLMN_IDENTITY_STRUCT plmn_id, bool con_req) {};

  uint16_t get_mcc() { return mcc; }
  uint16_t get_mnc() { return mnc; }
  void enable_capabilities() {}

private:
  uint32_t last_sdu_len;
};

class gw_dummy : public gw_interface_nas, public gw_interface_pdcp
{
  error_t setup_if_addr(uint32_t ip_addr, char *err_str) { return ERROR_NONE; }
  void write_pdu(uint32_t lcid, byte_buffer_t *pdu) {}
};

}

int security_command_test()
{
  int ret = SRSLTE_ERROR;
  srslte::log_filter nas_log("NAS");
  srslte::log_filter rrc_log("RRC");
  srslte::log_filter mac_log("MAC");
  srslte::log_filter usim_log("USIM");

  nas_log.set_level(srslte::LOG_LEVEL_DEBUG);
  rrc_log.set_level(srslte::LOG_LEVEL_DEBUG);
  nas_log.set_hex_limit(100000);
  rrc_log.set_hex_limit(100000);

  rrc_dummy rrc_dummy;
  gw_dummy gw;

  usim_args_t args;
  args.algo = "xor";
  args.imei = "353490069873319";
  args.imsi = "001010123456789";
  args.k = "00112233445566778899aabbccddeeff";
  args.op = "63BFA50EE6523365FF14C1F45F88737D";


  // init USIM
  srsue::usim usim;
  bool    net_valid;
  uint8_t res[16];
  usim.init(&args, &usim_log);

  srslte::byte_buffer_pool *pool;
  pool = byte_buffer_pool::get_instance();

  srsue::nas nas;
  srslte_nas_config_t cfg;
  nas.init(&usim, &rrc_dummy, &gw, &nas_log, cfg);

  // push auth request PDU to NAS to generate security context
  byte_buffer_t* tmp = pool->allocate();
  memcpy(tmp->msg, auth_request_pdu, sizeof(auth_request_pdu));
  tmp->N_bytes = sizeof(auth_request_pdu);
  nas.write_pdu(LCID, tmp);

  // TODO: add check for authentication response

  // reuse buffer for security mode command
  memcpy(tmp->msg, sec_mode_command_pdu, sizeof(sec_mode_command_pdu));
  tmp->N_bytes = sizeof(sec_mode_command_pdu);
  nas.write_pdu(LCID, tmp);

  // check length of generated NAS SDU
  if (rrc_dummy.get_last_sdu_len() > 3) {
    ret = SRSLTE_SUCCESS;
  }

  pool->cleanup();

  return ret;
}


int mme_attach_request_test()
{
  int ret = SRSLTE_ERROR;
  srslte::log_filter nas_log("NAS");
  srslte::log_filter rrc_log("RRC");
  srslte::log_filter mac_log("MAC");
  srslte::log_filter usim_log("USIM");

  nas_log.set_level(srslte::LOG_LEVEL_DEBUG);
  rrc_log.set_level(srslte::LOG_LEVEL_DEBUG);
  nas_log.set_hex_limit(100000);
  rrc_log.set_hex_limit(100000);

  rrc_dummy rrc_dummy;
  gw_dummy gw;
  srsue::usim usim;
  usim_args_t args;
  args.algo = "xor";
  args.imei = "353490069873319";
  args.imsi = "001010123456789";
  args.k = "00112233445566778899aabbccddeeff";
  args.op = "63BFA50EE6523365FF14C1F45F88737D";
  usim.init(&args, &usim_log);

  srslte_nas_config_t nas_cfg;
  srsue::nas nas;
  nas.init(&usim, &rrc_dummy, &gw, &nas_log, nas_cfg);

  nas.attach_request();
  nas.notify_connection_setup();

  // check length of generated NAS SDU
  if (rrc_dummy.get_last_sdu_len() > 3) {
    ret = SRSLTE_SUCCESS;
  }

  return ret;
}


int main(int argc, char **argv)
{
  if (security_command_test()) {
    printf("Security command test failed.\n");
    return -1;
  }

  if (mme_attach_request_test()) {
    printf("Attach request test failed.\n");
    return -1;
  }

  return 0;
}
