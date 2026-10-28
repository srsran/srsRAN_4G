/**
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srsran/common/bcd_helpers.h"
#include "srsran/common/test_common.h"
#include "srsran/common/tsan_options.h"
#include "srsran/interfaces/ue_pdcp_interfaces.h"
#include "srsran/srslog/srslog.h"
#include "srsran/test/ue_test_interfaces.h"
#include "srsue/hdr/stack/upper/gw.h"
#include "srsue/hdr/stack/upper/nas_5g.h"
#include "srsue/hdr/stack/upper/test/nas_test_common.h"

using namespace srsue;
using namespace srsran;

#define HAVE_PCAP 0

int amf_attach_request_test(srsran::nas_pcap* pcap)
{
  int ret = SRSRAN_ERROR;

  rrc_nr_dummy rrc_nr_dummy;
  pdcp_dummy   pdcp_dummy;

  srsue::usim usim(srslog::fetch_basic_logger("USIM"));
  usim_args_t args;
  args.mode = "soft";
  args.algo = "xor";
  args.imei = "353490069873319";
  args.imsi = "001010123456789";
  args.k    = "00112233445566778899aabbccddeeff";
  args.op   = "63BFA50EE6523365FF14C1F45F88737D";
  usim.init(&args);

  nas_5g_args_t nas_5g_cfg;
  nas_5g_cfg.force_imsi_attach = true;
  pdu_session_cfg_t pdu_session;
  pdu_session.apn_name = "test123";

  nas_5g_cfg.pdu_session_cfgs.push_back(pdu_session);

  nas_5g_cfg.eia = "0,1,2,3";
  nas_5g_cfg.eea = "0,1,2,3";

  nas_5g_cfg.ia5g = "0,1,2,3";
  nas_5g_cfg.ea5g = "0,1,2,3";

  test_stack_dummy<srsue::nas_5g> stack(&pdcp_dummy);
  srsue::nas_5g                   nas_5g(srslog::fetch_basic_logger("NAS-5G"), &stack.task_sched);
  srsue::gw                       gw(srslog::fetch_basic_logger("GW"));

  if (pcap != nullptr) {
    nas_5g.start_pcap(pcap);
  }

  gw_args_t gw_args;
  gw_args.tun_dev_name     = "tun0";
  gw_args.log.gw_level     = "debug";
  gw_args.log.gw_hex_limit = 100000;

  gw.init(gw_args, &stack);
  stack.init(&nas_5g);

  nas_5g.init(&usim, &rrc_nr_dummy, &gw, nas_5g_cfg);
  rrc_nr_dummy.init(&nas_5g);

  // trigger test
  stack.switch_on();
  stack.stop();

  gw.stop();
  ret = SRSRAN_SUCCESS;

  return ret;
}  

int service_request_mt_test(srsran::nas_pcap* pcap)
{
  rrc_nr_dummy rrc_nr_dummy;
  pdcp_dummy   pdcp_dummy;

  srsue::usim usim(srslog::fetch_basic_logger("USIM"));
  usim_args_t usim_args;
  usim_args.mode = "soft";
  usim_args.algo = "xor";
  usim_args.imei = "353490069873319";
  usim_args.imsi = "001010123456789";
  usim_args.k    = "00112233445566778899aabbccddeeff";
  usim_args.op   = "63BFA50EE6523365FF14C1F45F88737D";
  usim.init(&usim_args);

  nas_5g_args_t nas_5g_cfg;
  nas_5g_cfg.force_imsi_attach = true;
  nas_5g_cfg.ia5g              = "0,1,2,3";
  nas_5g_cfg.ea5g              = "0,1,2,3";

  test_stack_dummy<srsue::nas_5g> stack(&pdcp_dummy);
  srsue::gw     gw(srslog::fetch_basic_logger("GW"));
  srsue::nas_5g nas_5g(srslog::fetch_basic_logger("NAS-5G"), &stack.task_sched);

  if (pcap != nullptr) {
    nas_5g.start_pcap(pcap);
  }

  gw_args_t gw_args;
  gw_args.tun_dev_name     = "tun0";
  gw_args.log.gw_level     = "debug";
  gw_args.log.gw_hex_limit = 100000;
  gw.init(gw_args, &stack);
  stack.init(&nas_5g);

  nas_5g.init(&usim, &rrc_nr_dummy, &gw, nas_5g_cfg);
  rrc_nr_dummy.init(&nas_5g);

  uint8_t ref_k_nas_enc[32] = {0x1b, 0xa5, 0x75, 0xb5, 0x76, 0xd2, 0xd8, 0x70, 0x59, 0x32, 0x60, 0xe8,
                                0xc4, 0x07, 0x88, 0x51, 0x24, 0x9d, 0x9d, 0x15, 0x3f, 0xc0, 0x85, 0xad,
                                0xa4, 0x23, 0xc8, 0xb9, 0x36, 0xfe, 0xa1, 0x80};
  uint8_t ref_k_nas_int[32] = {0x70, 0x84, 0xcf, 0xa1, 0x2d, 0xa5, 0xbd, 0x2d, 0xe3, 0x91, 0x62, 0x56,
                                0xdb, 0x74, 0x58, 0xdd, 0x3e, 0x7b, 0xfa, 0x8a, 0x9d, 0xe3, 0x04, 0xa2,
                                0x94, 0x8b, 0xb0, 0x3e, 0xf1, 0x82, 0x90, 0x76};

  memcpy(nas_5g.ctxt_base.k_nas_enc, ref_k_nas_enc, 32);
  memcpy(nas_5g.ctxt_base.k_nas_int, ref_k_nas_int, 32);
  nas_5g.ctxt_base.cipher_algo = srsran::CIPHERING_ALGORITHM_ID_EEA0;
  nas_5g.ctxt_base.integ_algo  = srsran::INTEGRITY_ALGORITHM_ID_128_EIA2;
  nas_5g.ctxt_base.tx_count    = 4;

  nas_5g.ng_ksi = 5;

  nas_5g.guti_5g.amf_set_id  = 1;
  nas_5g.guti_5g.amf_pointer = 0;
  nas_5g.guti_5g.tmsi_5g     = 0xc00004b1;

  // --- Generate the Service Request as an MT (paging-triggered) request ---
  srsran::unique_byte_buffer_t generated = srsran::make_byte_buffer();
  nas_5g.nr_gen_service_request(generated, srsran::nr_establishment_cause_t::mt_Access);

  uint8_t ref_service_request_pdu[] = {0x7e, 0x01, 0x99, 0xde, 0x56, 0x96, 0x04, 0x7e, 0x00, 0x4c,
                                        0x25, 0x00, 0x07, 0x04, 0x00, 0x40, 0xc0, 0x00, 0x04, 0xb1,
                                        0x71, 0x00, 0x0d, 0x7e, 0x00, 0x4c, 0x25, 0x00, 0x07, 0x04,
                                        0x00, 0x40, 0xc0, 0x00, 0x04, 0xb1};
  
  auto hex_dump = [](const uint8_t* buf, uint32_t len) {
    std::string s;
    char tmp[4];
    for (uint32_t i = 0; i < len; i++) {
      snprintf(tmp, sizeof(tmp), "%02x ", buf[i]);
      s += tmp;  
    }
    return s;
  };

  printf("Generated (%d B): %s\n", generated->N_bytes, hex_dump(generated->msg, generated->N_bytes).c_str());
  printf("Reference (%zu B): %s\n",
         sizeof(ref_service_request_pdu),
         hex_dump(ref_service_request_pdu, sizeof(ref_service_request_pdu)).c_str());

  TESTASSERT(generated->N_bytes == sizeof(ref_service_request_pdu));
  TESTASSERT(memcmp(generated->msg, ref_service_request_pdu, generated->N_bytes) == 0);

  srslog::fetch_basic_logger("NAS-5G")
      .info(generated->msg, generated->N_bytes, "Generated Service Request (%d B)", generated->N_bytes);

  stack.stop();
  gw.stop();
  return SRSRAN_SUCCESS;
}

int service_request_mo_test(srsran::nas_pcap* pcap)  
{  
  rrc_nr_dummy rrc_nr_dummy;  
  pdcp_dummy   pdcp_dummy;  
  
  srsue::usim usim(srslog::fetch_basic_logger("USIM"));  
  usim_args_t usim_args;  
  usim_args.mode = "soft";  
  usim_args.algo = "xor";  
  usim_args.imei = "353490069873319";  
  usim_args.imsi = "001010123456789";  
  usim_args.k    = "00112233445566778899aabbccddeeff";  
  usim_args.op   = "63BFA50EE6523365FF14C1F45F88737D";  
  usim.init(&usim_args);  
  
  nas_5g_args_t nas_5g_cfg;  
  nas_5g_cfg.force_imsi_attach = true;  
  nas_5g_cfg.ia5g              = "0,1,2,3";  
  nas_5g_cfg.ea5g              = "0,1,2,3";  
  
  test_stack_dummy<srsue::nas_5g> stack(&pdcp_dummy);  
  srsue::gw                       gw(srslog::fetch_basic_logger("GW"));  
  srsue::nas_5g                   nas_5g(srslog::fetch_basic_logger("NAS-5G"), &stack.task_sched);  
  
  gw_args_t gw_args;  
  gw_args.tun_dev_name     = "tun0";  
  gw_args.log.gw_level     = "debug";  
  gw_args.log.gw_hex_limit = 100000;  
  
  gw.init(gw_args, &stack);  
  stack.init(&nas_5g);  
  nas_5g.init(&usim, &rrc_nr_dummy, &gw, nas_5g_cfg);  
  rrc_nr_dummy.init(&nas_5g);  
  
  if (pcap != nullptr) {  
    nas_5g.start_pcap(pcap);  
  }  
  
  static const uint8_t ref_k_nas_enc[32] = {0x1b, 0xa5, 0x75, 0xb5, 0x76, 0xd2, 0xd8, 0x70,  
                                             0x59, 0x32, 0x60, 0xe8, 0xc4, 0x07, 0x88, 0x51,  
                                             0x24, 0x9d, 0x9d, 0x15, 0x3f, 0xc0, 0x85, 0xad,  
                                             0xa4, 0x23, 0xc8, 0xb9, 0x36, 0xfe, 0xa1, 0x80};  
  static const uint8_t ref_k_nas_int[32] = {0x70, 0x84, 0xcf, 0xa1, 0x2d, 0xa5, 0xbd, 0x2d,  
                                             0xe3, 0x91, 0x62, 0x56, 0xdb, 0x74, 0x58, 0xdd,  
                                             0x3e, 0x7b, 0xfa, 0x8a, 0x9d, 0xe3, 0x04, 0xa2,  
                                             0x94, 0x8b, 0xb0, 0x3e, 0xf1, 0x82, 0x90, 0x76};  
  
  memcpy(nas_5g.ctxt_base.k_nas_enc, ref_k_nas_enc, 32);  
  memcpy(nas_5g.ctxt_base.k_nas_int, ref_k_nas_int, 32);  
  nas_5g.ctxt_base.cipher_algo = srsran::CIPHERING_ALGORITHM_ID_EEA0;
  nas_5g.ctxt_base.integ_algo  = srsran::INTEGRITY_ALGORITHM_ID_128_EIA2;
  nas_5g.ctxt_base.tx_count    = 6;
  
  nas_5g.ng_ksi = 5;
  
  nas_5g.guti_5g.amf_set_id  = 1;  
  nas_5g.guti_5g.amf_pointer = 0;  
  nas_5g.guti_5g.tmsi_5g     = 0xc000070c;  
  
  nas_5g.pdu_sessions[0].configured 	 = true;  
  nas_5g.pdu_sessions[0].established	 = true;
  nas_5g.pdu_sessions[0].pdu_session_id  = 1;
  
  // --- Generate for MO ---  
  srsran::unique_byte_buffer_t generated = srsran::make_byte_buffer();  
  nas_5g.nr_gen_service_request(generated, srsran::nr_establishment_cause_t::mo_Data);  
  
  static const uint8_t ref_service_request_pdu[] = {  
      0x7e, 0x01, 0xee, 0x01, 0x0f, 0xab, 0x06, 0x7e, 0x00, 0x4c, 0x15, 0x00, 0x07, 0x04, 0x00,  
      0x40, 0xc0, 0x00, 0x07, 0x0c, 0x71, 0x00, 0x15, 0x7e, 0x00, 0x4c, 0x15, 0x00, 0x07, 0x04,  
      0x00, 0x40, 0xc0, 0x00, 0x07, 0x0c, 0x40, 0x02, 0x02, 0x00, 0x50, 0x02, 0x02, 0x00};  
  
  auto hex_dump = [](const uint8_t* buf, uint32_t len) {  
    std::string s;  
    char        tmp[4];  
    for (uint32_t i = 0; i < len; i++) {  
      snprintf(tmp, sizeof(tmp), "%02x ", buf[i]);  
      s += tmp;  
    }  
    return s;  
  };  
  printf("Generated (%d B): %s\n", generated->N_bytes, hex_dump(generated->msg, generated->N_bytes).c_str());  
  printf("Reference (%zu B): %s\n",  
         sizeof(ref_service_request_pdu),  
         hex_dump(ref_service_request_pdu, sizeof(ref_service_request_pdu)).c_str());  
  
  TESTASSERT(generated->N_bytes == sizeof(ref_service_request_pdu));  
  TESTASSERT(memcmp(generated->msg, ref_service_request_pdu, generated->N_bytes) == 0);  
  
  stack.stop();  
  gw.stop();  
  return SRSRAN_SUCCESS;  
}

int main(int argc, char** argv)
{
  // Setup logging.
  auto& rrc_logger = srslog::fetch_basic_logger("RRC", false);
  rrc_logger.set_level(srslog::basic_levels::debug);
  rrc_logger.set_hex_dump_max_size(100000);
  auto& nas_logger = srslog::fetch_basic_logger("NAS", false);
  nas_logger.set_level(srslog::basic_levels::debug);
  nas_logger.set_hex_dump_max_size(100000);
  auto& usim_logger = srslog::fetch_basic_logger("USIM", false);
  usim_logger.set_level(srslog::basic_levels::debug);
  usim_logger.set_hex_dump_max_size(100000);
  auto& gw_logger = srslog::fetch_basic_logger("GW", false);
  gw_logger.set_level(srslog::basic_levels::debug);
  gw_logger.set_hex_dump_max_size(100000);

  // Start the log backend.
  srslog::init();
#if HAVE_PCAP
  srsran::nas_pcap pcap;
  pcap.open("nas_5g_test.pcap", 0, srsran::srsran_rat_t::nr);
  TESTASSERT(amf_attach_request_test(&pcap) == SRSRAN_SUCCESS);
  pcap.close();

  srsran::nas_pcap pcap;  
  pcap.open("nas_5g_service_request_test.pcap", 0, srsran::srsran_rat_t::nr);  
  TESTASSERT(service_request_mt_test(&pcap) == SRSRAN_SUCCESS);  
  pcap.close();

  srsran::nas_pcap pcap;  
  pcap.open("nas_5g_service_request_mo_test.pcap", 0, srsran::srsran_rat_t::nr);  
  TESTASSERT(service_request_mo_test(&pcap) == SRSRAN_SUCCESS);  
  pcap.close();  
#else
  TESTASSERT(amf_attach_request_test(nullptr) == SRSRAN_SUCCESS);
  TESTASSERT(service_request_mt_test(nullptr) == SRSRAN_SUCCESS);  
  TESTASSERT(service_request_mo_test(nullptr) == SRSRAN_SUCCESS);
#endif // HAVE_PCAP

  return SRSRAN_SUCCESS;
}
