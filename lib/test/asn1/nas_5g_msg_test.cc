/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#include <iostream>
#include <stdio.h>

#include "srsran/asn1/nas_5g_msg.h"
#include "srsran/common/buffer_pool.h"
#include "srsran/common/nas_pcap.h"
#include "srsran/common/test_common.h"
#include "srsran/srslog/srslog.h"

using namespace srsran::nas_5g;

#define HAVE_PCAP 0

inline void print_msg(const srsran::unique_byte_buffer_t& msg)
{
  printf("\t");
  for (uint32_t i = 0; i < msg->N_bytes; i++) {
    printf("0x%02x ", msg->msg[i]);
    if ((i + 1) % 16 == 0) {
      printf("\n\t");
    }
  }
  printf("\n");
}

inline void hex_dump(uint8_t* buf, uint32_t buf_length)
{
  printf("\t");
  for (uint32_t i = 0; i < buf_length; i++) {
    printf("0x%02x ", buf[i]);
    if ((i + 1) % 16 == 0) {
      printf("\n\t");
    }
  }
  printf("\n");
}

int registration_request_unpacking_packing_test(srsran::nas_pcap* pcap)
{
  uint8_t reg_request[] = {0x7e, 0x00, 0x41, 0x79, 0x00, 0x0b, 0xf2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x2e, 0x02, 0xf0, 0xf0, 0x17, 0x07, 0xf0, 0xf0, 0xc0, 0xc0, 0x01, 0x80, 0x30};
  //  Non-Access-Stratum 5GS (NAS)PDU
  //     Plain NAS 5GS Message
  //         Extended protocol discriminator: 5G mobility management messages (126)
  //         0000 .... = Spare Half Octet: 0
  //         .... 0000 = Security header type: Plain NAS message, not security protected (0)
  //         Message type: Registration request (0x41)
  //         5GS registration type
  //             .... 1... = Follow-On Request bit (FOR): Follow-on request pending
  //             .... .001 = 5GS registration type: initial registration (1)
  //         NAS key set identifier
  //             0... .... = Type of security context flag (TSC): Native security context (for KSIAMF)
  //             .111 .... = NAS key set identifier: 7
  //         5GS mobile identity
  //             Length: 11
  //             .... 0... = Odd/even indication: Even number of identity digits
  //             .... .010 = Type of identity: 5G-GUTI (2)
  //             Mobile Country Code (MCC): Unknown (0)
  //             Mobile Network Code (MNC): Unknown (000)
  //             AMF Region ID: 0
  //             0000 0000 00.. .... = AMF Set ID: 0
  //             ..00 0000 = AMF Pointer: 0
  //             5G-TMSI: 0 (0x00000000)
  //             <TMSI/P-TMSI/M-TMSI/5G-TMSI: 0 (0x00000000)>
  //         UE security capability
  //             Element ID: 0x2e
  //             Length: 2
  //             1... .... = 5G-EA0: Supported
  //             .1.. .... = 128-5G-EA1: Supported
  //             ..1. .... = 128-5G-EA2: Supported
  //             ...1 .... = 128-5G-EA3: Supported
  //             .... 0... = 5G-EA4: Not supported
  //             .... .0.. = 5G-EA5: Not supported
  //             .... ..0. = 5G-EA6: Not supported
  //             .... ...0 = 5G-EA7: Not supported
  //             1... .... = 5G-IA0: Supported
  //             .1.. .... = 128-5G-IA1: Supported
  //             ..1. .... = 128-5G-IA2: Supported
  //             ...1 .... = 128-5G-IA3: Supported
  //             .... 0... = 5G-IA4: Not supported
  //             .... .0.. = 5G-IA5: Not supported
  //             .... ..0. = 5G-IA6: Not supported
  //             .... ...0 = 5G-IA7: Not supported
  //         UE network capability
  //             Element ID: 0x17
  //             Length: 7
  //             1... .... = EEA0: Supported
  //             .1.. .... = 128-EEA1: Supported
  //             ..1. .... = 128-EEA2: Supported
  //             ...1 .... = 128-EEA3: Supported
  //             .... 0... = EEA4: Not supported
  //             .... .0.. = EEA5: Not supported
  //             .... ..0. = EEA6: Not supported
  //             .... ...0 = EEA7: Not supported
  //             1... .... = EIA0: Supported
  //             .1.. .... = 128-EIA1: Supported
  //             ..1. .... = 128-EIA2: Supported
  //             ...1 .... = 128-EIA3: Supported
  //             .... 0... = EIA4: Not supported
  //             .... .0.. = EIA5: Not supported
  //             .... ..0. = EIA6: Not supported
  //             .... ...0 = EIA7: Not supported
  //             1... .... = UEA0: Supported
  //             .1.. .... = UEA1: Supported
  //             ..0. .... = UEA2: Not supported
  //             ...0 .... = UEA3: Not supported
  //             .... 0... = UEA4: Not supported
  //             .... .0.. = UEA5: Not supported
  //             .... ..0. = UEA6: Not supported
  //             .... ...0 = UEA7: Not supported
  //             1... .... = UCS2 support (UCS2): The UE has no preference between the use of the default alphabet and
  //             the use of UCS2 .1.. .... = UMTS integrity algorithm UIA1: Supported
  //             ..0. .... = UMTS integrity algorithm UIA2: Not supported
  //             ...0 .... = UMTS integrity algorithm UIA3: Not supported
  //             .... 0... = UMTS integrity algorithm UIA4: Not supported
  //             .... .0.. = UMTS integrity algorithm UIA5: Not supported
  //             .... ..0. = UMTS integrity algorithm UIA6: Not supported
  //             .... ...0 = UMTS integrity algorithm UIA7: Not supported
  //             0... .... = ProSe direct discovery: Not supported
  //             .0.. .... = ProSe: Not supported
  //             ..0. .... = H.245 After SRVCC Handover: Not supported
  //             ...0 .... = Access class control for CSFB: Not supported
  //             .... 0... = LTE Positioning Protocol: Not supported
  //             .... .0.. = Location services (LCS) notification mechanisms: Not supported
  //             .... ..0. = SRVCC from E-UTRAN to cdma2000 1xCS: Not supported
  //             .... ...1 = Notification procedure: Supported
  //             1... .... = Extended protocol configuration options: Supported
  //             .0.. .... = Header compression for control plane CIoT EPS optimization: Not supported
  //             ..0. .... = EMM-REGISTERED w/o PDN connectivity: Not supported
  //             ...0 .... = S1-U data transfer: Not supported
  //             .... 0... = User plane CIoT EPS optimization: Not supported
  //             .... .0.. = Control plane CIoT EPS optimization: Not supported
  //             .... ..0. = ProSe UE-to-network relay: Not supported
  //             .... ...0 = ProSe direct communication: Not supported
  //             0... .... = Signalling for a maximum number of 15 EPS bearer contexts: Not supported
  //             .0.. .... = Service gap control: Not supported
  //             ..1. .... = N1 mode: Supported
  //             ...1 .... = Dual connectivity with NR: Supported
  //             .... 0... = Control plane data backoff: Not supported
  //             .... .0.. = Restriction on use of enhanced coverage: Not supported
  //             .... ..0. = V2X communication over PC5: Not supported
  //             .... ...0 = Multiple DRB: Not supported

  srsran::unique_byte_buffer_t buf;
  copy_msg_to_buffer(buf, reg_request);
#if HAVE_PCAP
  pcap->write_nas(buf.get()->msg, buf.get()->N_bytes);
#endif
  nas_5gs_msg nas_msg;

  TESTASSERT(nas_msg.unpack_outer_hdr(buf) == SRSRAN_SUCCESS);
  TESTASSERT(nas_msg.hdr.extended_protocol_discriminator == nas_5gs_hdr::extended_protocol_discriminator_5gmm);
  TESTASSERT(nas_msg.hdr.security_header_type == nas_5gs_hdr::plain_5gs_nas_message);
  TESTASSERT(nas_msg.hdr.message_type == msg_types::options::registration_request);

  TESTASSERT(nas_msg.unpack(buf) == SRSRAN_SUCCESS);
  TESTASSERT(nas_msg.hdr.message_type == msg_types::options::registration_request);
  printf("%s\n", nas_msg.hdr.message_type.to_string());
  registration_request_t& reg_request_msg = nas_msg.registration_request();
  TESTASSERT(reg_request_msg.registration_type_5gs.registration_type ==
             registration_type_5gs_t::registration_type_type_::options::initial_registration);
  TESTASSERT(reg_request_msg.registration_type_5gs.follow_on_request_bit ==
             registration_type_5gs_t::follow_on_request_bit_type_::options::follow_on_request_pending);
  TESTASSERT(reg_request_msg.ng_ksi.security_context_flag ==
             key_set_identifier_t::security_context_flag_type_::options::native_security_context);
  TESTASSERT(reg_request_msg.ng_ksi.nas_key_set_identifier ==
             key_set_identifier_t::nas_key_set_identifier_type_::options::no_key_is_available_or_reserved);
  TESTASSERT(reg_request_msg.mobile_identity_5gs.type() == mobile_identity_5gs_t::identity_types_::options::guti_5g);
  mobile_identity_5gs_t::guti_5g_s guti_5g_ = reg_request_msg.mobile_identity_5gs.guti_5g();
  TESTASSERT(guti_5g_.amf_pointer == 0x0);
  TESTASSERT(guti_5g_.amf_region_id == 0x0);
  TESTASSERT(guti_5g_.amf_set_id == 0);
  TESTASSERT(guti_5g_.mcc[0] == 0);
  TESTASSERT(guti_5g_.mcc[1] == 0);
  TESTASSERT(guti_5g_.mcc[2] == 0);
  TESTASSERT(guti_5g_.mnc[0] == 0);
  TESTASSERT(guti_5g_.mnc[1] == 0);
  TESTASSERT(guti_5g_.mnc[2] == 0);
  TESTASSERT(reg_request_msg.ue_security_capability_present == true);
  TESTASSERT(reg_request_msg.ue_security_capability.ea0_5g_supported == true);
  TESTASSERT(reg_request_msg.ue_security_capability.ea1_128_5g_supported == true);
  TESTASSERT(reg_request_msg.ue_security_capability.ea2_128_5g_supported == true);
  TESTASSERT(reg_request_msg.ue_security_capability.ea3_128_5g_supported == true);
  TESTASSERT(reg_request_msg.ue_security_capability.ea3_128_5g_supported == true);
  TESTASSERT(reg_request_msg.ue_security_capability.ea4_5g_supported == false);
  TESTASSERT(reg_request_msg.ue_security_capability.ea5_5g_supported == false);
  TESTASSERT(reg_request_msg.ue_security_capability.ea6_5g_supported == false);
  TESTASSERT(reg_request_msg.ue_security_capability.ea7_5g_supported == false);
  TESTASSERT(reg_request_msg.ue_security_capability.ia0_5g_supported == true);
  TESTASSERT(reg_request_msg.ue_security_capability.ia1_128_5g_supported == true);
  TESTASSERT(reg_request_msg.ue_security_capability.ia2_128_5g_supported == true);
  TESTASSERT(reg_request_msg.ue_security_capability.ia3_128_5g_supported == true);
  TESTASSERT(reg_request_msg.ue_security_capability.ia3_128_5g_supported == true);
  TESTASSERT(reg_request_msg.ue_security_capability.ia4_5g_supported == false);
  TESTASSERT(reg_request_msg.ue_security_capability.ia5_5g_supported == false);
  TESTASSERT(reg_request_msg.ue_security_capability.ia6_5g_supported == false);
  TESTASSERT(reg_request_msg.ue_security_capability.ia7_5g_supported == false);
  TESTASSERT(reg_request_msg.ue_security_capability.eps_caps_present == false);

  TESTASSERT(reg_request_msg.s1_ue_network_capability_present == true);

  TESTASSERT(reg_request_msg.s1_ue_network_capability.eea0_supported == true);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.eea1_128_supported == true);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.eea2_128_supported == true);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.eea3_128_supported == true);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.eea4_supported == false);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.eea5_supported == false);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.eea6_supported == false);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.eea7_supported == false);

  TESTASSERT(reg_request_msg.s1_ue_network_capability.eia0_supported == true);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.eia1_128_supported == true);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.eia2_128_supported == true);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.eia3_128_supported == true);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.eia4_supported == false);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.eia5_supported == false);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.eia6_supported == false);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.eia7_supported == false);

  TESTASSERT(reg_request_msg.s1_ue_network_capability.uea0_supported == true);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.uea1_128_supported == true);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.uea2_128_supported == false);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.uea3_128_supported == false);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.uea4_supported == false);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.uea5_supported == false);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.uea6_supported == false);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.uea7_supported == false);

  TESTASSERT(reg_request_msg.s1_ue_network_capability.ucs2_support == true);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.uia1_128_supported == true);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.uia2_128_supported == false);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.uia3_128_supported == false);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.uia4_supported == false);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.uia5_supported == false);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.uia6_supported == false);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.uia7_supported == false);

  TESTASSERT(reg_request_msg.s1_ue_network_capability.pro_se_dd_supported == false);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.pro_se_supported == false);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.h245_ash_supported == false);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.acc_csfb_supported == false);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.llp_supported == false);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.lcs_supported == false);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.srvcc_capability_supported == false);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.nf_capability_supported == true);

  TESTASSERT(reg_request_msg.s1_ue_network_capability.e_pco_supported == true);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.hc_cp_c_io_t_supported == false);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.e_rw_o_pdn_supported == false);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.s1_u_data_supported == false);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.up_c_io_t_supported == false);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.cp_c_io_t_supported == false);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.pro_se_relay_supported == false);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.pro_se_dc_supported == false);

  TESTASSERT(reg_request_msg.s1_ue_network_capability.max_15_eps_bearer_supported == false);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.sgc_supported == false);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.n1mode_supported == true);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.dcnr_supported == true);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.cp_backoff_supported == false);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.restrict_ec_supported == false);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.v2_x_pc5_supported == false);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.multiple_drb_supported == false);

  TESTASSERT(reg_request_msg.s1_ue_network_capability.nr_pc5_supported == false);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.up_mt_edt_supported == false);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.cp_mt_edt_supported == false);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.wus_supported == false);
  TESTASSERT(reg_request_msg.s1_ue_network_capability.racs_supported == false);

  srsran::unique_byte_buffer_t buf_cmp = srsran::make_byte_buffer();
  nas_msg.pack(buf_cmp);

  printf("buf_cmp->N_bytes %d  buf->N_bytes %d\n", buf_cmp->N_bytes, buf->N_bytes);
  printf("\n");
  print_msg(buf);
  printf("\n");
  print_msg(buf_cmp);
  printf("\n");
#if HAVE_PCAP
  pcap->write_nas(buf_cmp.get()->msg, buf_cmp.get()->N_bytes);
#endif
  return SRSRAN_SUCCESS;
}

int registration_request_unpacking_packing_test_2(srsran::nas_pcap* pcap)
{
  // Non-Access-Stratum 5GS (NAS)PDU
  //   Plain NAS 5GS Message
  //       Extended protocol discriminator: 5G mobility management messages (126)
  //       0000 .... = Spare Half Octet: 0
  //       .... 0000 = Security header type: Plain NAS message, not security protected (0)
  //       Message type: Registration request (0x41)
  //       5GS registration type
  //           .... 1... = Follow-On Request bit (FOR): Follow-on request pending
  //           .... .001 = 5GS registration type: initial registration (1)
  //       NAS key set identifier
  //           0... .... = Type of security context flag (TSC): Native security context (for KSIAMF)
  //           .111 .... = NAS key set identifier: 7
  //       5GS mobile identity
  //           Length: 54
  //           0... .... = Spare: 0
  //           .000 .... = SUPI format: IMSI (0)
  //           .... 0... = Spare: 0
  //           .... .001 = Type of identity: SUCI (1)
  //           Mobile Country Code (MCC): Unknown (1)
  //           Mobile Network Code (MNC): Unknown (01)
  //           Routing indicator: 17
  //           .... 0010 = Protection scheme Id: ECIES scheme profile B (2)
  //           Home network public key identifier: 27
  //           Scheme output: 03 99 7e e4 01 2d e3 6c 86 e2 29 97 c8 99 70 4b 0f 61 3a bd 6c 3b 1c 9c …
  //               ECC ephemeral public key: 03 99 7e e4 01 2d e3 6c 86 e2 29 97 c8 99 70 4b 0f 61 3a bd 6c 3b 1c 9c …
  //               Ciphertext: cb bd 5d 27 34
  //               MAC tag: 0x1e8b9e3328184bec
  //       UE security capability
  //           Element ID: 0x2e
  //           Length: 2
  //           1... .... = 5G-EA0: Supported
  //           .0.. .... = 128-5G-EA1: Not supported
  //           ..0. .... = 128-5G-EA2: Not supported
  //           ...0 .... = 128-5G-EA3: Not supported
  //           .... 0... = 5G-EA4: Not supported
  //           .... .0.. = 5G-EA5: Not supported
  //           .... ..0. = 5G-EA6: Not supported
  //           .... ...0 = 5G-EA7: Not supported
  //           0... .... = 5G-IA0: Not supported
  //           .0.. .... = 128-5G-IA1: Not supported
  //           ..1. .... = 128-5G-IA2: Supported
  //           ...0 .... = 128-5G-IA3: Not supported
  //           .... 0... = 5G-IA4: Not supported
  //           .... .0.. = 5G-IA5: Not supported
  //           .... ..0. = 5G-IA6: Not supported
  //           .... ...0 = 5G-IA7: Not supported

  uint8_t reg_request[] = {0x7e, 0x00, 0x41, 0x79, 0x00, 0x36, 0x01, 0x00, 0xf1, 0x10, 0x71, 0xff, 0x02,
                           0x1b, 0x03, 0x99, 0x7e, 0xe4, 0x01, 0x2d, 0xe3, 0x6c, 0x86, 0xe2, 0x29, 0x97,
                           0xc8, 0x99, 0x70, 0x4b, 0x0f, 0x61, 0x3a, 0xbd, 0x6c, 0x3b, 0x1c, 0x9c, 0xa7,
                           0x8a, 0x4b, 0x14, 0x7e, 0x22, 0xaf, 0xb0, 0x64, 0xcb, 0xbd, 0x5d, 0x27, 0x34,
                           0x1e, 0x8b, 0x9e, 0x33, 0x28, 0x18, 0x4b, 0xec, 0x2e, 0x02, 0x80, 0x20};

  srsran::unique_byte_buffer_t buf;
  copy_msg_to_buffer(buf, reg_request);
#if HAVE_PCAP
  pcap->write_nas(buf.get()->msg, buf.get()->N_bytes);
#endif
  nas_5gs_msg nas_msg;

  TESTASSERT(nas_msg.unpack_outer_hdr(buf) == SRSRAN_SUCCESS);
  TESTASSERT(nas_msg.hdr.extended_protocol_discriminator == nas_5gs_hdr::extended_protocol_discriminator_5gmm);
  TESTASSERT(nas_msg.hdr.security_header_type == nas_5gs_hdr::plain_5gs_nas_message);
  TESTASSERT(nas_msg.hdr.message_type == msg_types::options::registration_request);

  TESTASSERT(nas_msg.unpack(buf) == SRSRAN_SUCCESS);
  TESTASSERT(nas_msg.hdr.message_type == msg_types::options::registration_request);
  printf("%s\n", nas_msg.hdr.message_type.to_string());
  registration_request_t& reg_request_msg = nas_msg.registration_request();
  TESTASSERT(reg_request_msg.registration_type_5gs.registration_type ==
             registration_type_5gs_t::registration_type_type_::options::initial_registration);
  TESTASSERT(reg_request_msg.registration_type_5gs.follow_on_request_bit ==
             registration_type_5gs_t::follow_on_request_bit_type_::options::follow_on_request_pending);
  TESTASSERT(reg_request_msg.ng_ksi.security_context_flag ==
             key_set_identifier_t::security_context_flag_type_::options::native_security_context);
  TESTASSERT(reg_request_msg.ng_ksi.nas_key_set_identifier ==
             key_set_identifier_t::nas_key_set_identifier_type_::options::no_key_is_available_or_reserved);
  TESTASSERT(reg_request_msg.mobile_identity_5gs.type() == mobile_identity_5gs_t::identity_types_::options::suci);
  TESTASSERT(reg_request_msg.mobile_identity_5gs.suci().supi_format.value ==
             mobile_identity_5gs_t::suci_s::supi_format_type_::options::imsi);
  TESTASSERT(reg_request_msg.mobile_identity_5gs.suci().mcc[0] == 0);
  TESTASSERT(reg_request_msg.mobile_identity_5gs.suci().mcc[1] == 0);
  TESTASSERT(reg_request_msg.mobile_identity_5gs.suci().mcc[2] == 1);
  TESTASSERT(reg_request_msg.mobile_identity_5gs.suci().mnc[0] == 0);
  TESTASSERT(reg_request_msg.mobile_identity_5gs.suci().mnc[1] == 1);
  TESTASSERT(reg_request_msg.mobile_identity_5gs.suci().mnc[2] == 0xf);

  TESTASSERT(reg_request_msg.mobile_identity_5gs.suci().routing_indicator[0] == 1);
  TESTASSERT(reg_request_msg.mobile_identity_5gs.suci().routing_indicator[1] == 7);
  TESTASSERT(reg_request_msg.mobile_identity_5gs.suci().routing_indicator[2] == 0xf);
  TESTASSERT(reg_request_msg.mobile_identity_5gs.suci().routing_indicator[3] == 0xf);

  TESTASSERT(reg_request_msg.mobile_identity_5gs.suci().protection_scheme_id ==
             mobile_identity_5gs_t::suci_s::protection_scheme_id_type_::options::ecies_scheme_profile_b);
  TESTASSERT(reg_request_msg.mobile_identity_5gs.suci().home_network_public_key_identifier == 27);

  hex_dump(reg_request_msg.mobile_identity_5gs.suci().scheme_output.data(),
           reg_request_msg.mobile_identity_5gs.suci().scheme_output.size());
  srsran::unique_byte_buffer_t buf_cmp = srsran::make_byte_buffer();
  nas_msg.pack(buf_cmp);

  printf("buf_cmp->N_bytes %d  buf->N_bytes %d\n", buf_cmp->N_bytes, buf->N_bytes);
  printf("\n");
  print_msg(buf);
  printf("\n");
  print_msg(buf_cmp);
  printf("\n");
#if HAVE_PCAP
  pcap->write_nas(buf_cmp.get()->msg, buf_cmp.get()->N_bytes);
#endif
  return SRSRAN_SUCCESS;
}

int deregistration_request_unpacking_packing_test(srsran::nas_pcap* pcap)
{
  // Non-Access-Stratum 5GS (NAS)PDU
  // Security protected NAS 5GS message
  //     Extended protocol discriminator: 5G mobility management messages (126)
  //     0000 .... = Spare Half Octet: 0
  //     .... 0001 = Security header type: Integrity protected (1)
  //     Message authentication code: 0x6f0325f5
  //     Sequence number: 2
  // Plain NAS 5GS Message
  //     Extended protocol discriminator: 5G mobility management messages (126)
  //     0000 .... = Spare Half Octet: 0
  //     .... 0000 = Security header type: Plain NAS message, not security protected (0)
  //     Message type: Deregistration request (UE originating) (0x45)
  //     De-registration type
  //         .... 1... = Switch off: Switch off
  //         .... .0.. = Re-registration required: re-registration not required
  //         .... ..01 = Access type: 3GPP access (1)
  //     NAS key set identifier
  //         0... .... = Type of security context flag (TSC): Native security context (for KSIAMF)
  //         .000 .... = NAS key set identifier: 0
  //     5GS mobile identity
  //         Length: 11
  //         .... 0... = Odd/even indication: Even number of identity digits
  //         .... .010 = Type of identity: 5G-GUTI (2)
  //         Mobile Country Code (MCC): Unknown (1)
  //         Mobile Network Code (MNC): Unknown (01)
  //         AMF Region ID: 202
  //         1111 1110 00.. .... = AMF Set ID: 1016
  //         ..00 0000 = AMF Pointer: 0
  //         5G-TMSI: 1 (0x00000001)
  //         <TMSI/P-TMSI/M-TMSI/5G-TMSI: 1 (0x00000001)>
  uint8_t dereg_request[] = {0x7e, 0x01, 0x6f, 0x03, 0x25, 0xf5, 0x02, 0x7e, 0x00, 0x45, 0x09, 0x00,
                             0x0b, 0x02, 0x00, 0xf1, 0x10, 0xca, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x01};
  srsran::unique_byte_buffer_t buf;
  copy_msg_to_buffer(buf, dereg_request);
#if HAVE_PCAP
  pcap->write_nas(buf.get()->msg, buf.get()->N_bytes);
#endif

  nas_5gs_msg nas_msg;

  TESTASSERT(nas_msg.unpack_outer_hdr(buf) == SRSRAN_SUCCESS);
  TESTASSERT(nas_msg.hdr.extended_protocol_discriminator == nas_5gs_hdr::extended_protocol_discriminator_5gmm);
  TESTASSERT(nas_msg.hdr.security_header_type == nas_5gs_hdr::integrity_protected);
  TESTASSERT(nas_msg.hdr.message_authentication_code == 0x6f0325f5);
  TESTASSERT(nas_msg.hdr.sequence_number == 2);

  TESTASSERT(nas_msg.unpack(buf) == SRSRAN_SUCCESS);
  TESTASSERT(nas_msg.hdr.message_type == msg_types::options::deregistration_request_ue_originating);
  printf("%s\n", nas_msg.hdr.message_type.to_string());

  deregistration_request_ue_originating_t& dereg_request_msg = nas_msg.deregistration_request_ue_originating();
  TESTASSERT(dereg_request_msg.de_registration_type.switch_off ==
             de_registration_type_t::switch_off_type_::options::switch_off);
  TESTASSERT(dereg_request_msg.de_registration_type.re_registration_required ==
             de_registration_type_t::re_registration_required_type_::options::re_registration_not_required);
  TESTASSERT(dereg_request_msg.de_registration_type.access_type ==
             de_registration_type_t::access_type_type_::options::access_3_gpp);
  TESTASSERT(dereg_request_msg.ng_ksi.security_context_flag ==
             key_set_identifier_t::security_context_flag_type_::options::native_security_context);
  TESTASSERT(dereg_request_msg.ng_ksi.nas_key_set_identifier == 0);
  TESTASSERT(dereg_request_msg.mobile_identity_5gs.type() == mobile_identity_5gs_t::identity_types_::options::guti_5g);
  mobile_identity_5gs_t::guti_5g_s guti_5g_ = dereg_request_msg.mobile_identity_5gs.guti_5g();
  TESTASSERT(guti_5g_.amf_pointer == 0x0);
  TESTASSERT(guti_5g_.amf_region_id == 202);
  TESTASSERT(guti_5g_.amf_set_id == 1016);
  TESTASSERT(guti_5g_.mcc[0] == 0);
  TESTASSERT(guti_5g_.mcc[1] == 0);
  TESTASSERT(guti_5g_.mcc[2] == 1);
  TESTASSERT(guti_5g_.mnc[0] == 0);
  TESTASSERT(guti_5g_.mnc[1] == 1);
  TESTASSERT(guti_5g_.mnc[2] == 0xf);
  TESTASSERT(guti_5g_.tmsi_5g == 0x00000001);

  // Packing
  srsran::unique_byte_buffer_t buf_cmp = srsran::make_byte_buffer();
  nas_msg.pack(buf_cmp);
#if HAVE_PCAP
  pcap->write_nas(buf_cmp.get()->msg, buf_cmp.get()->N_bytes);
#endif

  // Compare buffer
  TESTASSERT(buf.get()->N_bytes == buf_cmp.get()->N_bytes);
  TESTASSERT(memcmp(buf.get()->msg, buf_cmp.get()->msg, buf.get()->N_bytes) == 0);
  return SRSRAN_SUCCESS;
}

int authentication_request_unpacking_packing_test(srsran::nas_pcap* pcap)
{
  // Non-Access-Stratum 5GS (NAS)PDU
  // Plain NAS 5GS Message
  //     Extended protocol discriminator: 5G mobility management messages (126)
  //     0000 .... = Spare Half Octet: 0
  //     .... 0000 = Security header type: Plain NAS message, not security protected (0)
  //     Message type: Authentication request (0x56)
  //     0000 .... = Spare Half Octet: 0
  //     NAS key set identifier - ngKSI
  //         .... 0... = Type of security context flag (TSC): Native security context (for KSIAMF)
  //         .... .000 = NAS key set identifier: 0
  //     ABBA
  //         Length: 2
  //         ABBA Contents: 0x0000
  //     Authentication Parameter RAND - 5G authentication challenge
  //         Element ID: 0x21
  //         RAND value: 16 46 24 32 75 b8 b9 c7 18 b6 05 c6 ff 03 96 71
  //     Authentication Parameter AUTN (UMTS and EPS authentication challenge) - 5G authentication challenge
  //         Element ID: 0x20
  //         Length: 16
  //         AUTN value: a3 09 26 e4 2e ea 80 00 f6 87 d5 ba a2 d9 56 ed
  //             SQN xor AK: a3 09 26 e4 2e ea
  //             AMF: 80 00
  //             MAC: f6 87 d5 ba a2 d9 56 ed

  uint8_t auth_request[] = {0x7e, 0x00, 0x56, 0x00, 0x02, 0x00, 0x00, 0x21, 0x16, 0x46, 0x24, 0x32, 0x75, 0xb8,
                            0xb9, 0xc7, 0x18, 0xb6, 0x05, 0xc6, 0xff, 0x03, 0x96, 0x71, 0x20, 0x10, 0xa3, 0x09,
                            0x26, 0xe4, 0x2e, 0xea, 0x80, 0x00, 0xf6, 0x87, 0xd5, 0xba, 0xa2, 0xd9, 0x56, 0xed};
  srsran::unique_byte_buffer_t buf;
  copy_msg_to_buffer(buf, auth_request);

#if HAVE_PCAP
  pcap->write_nas(buf.get()->msg, buf.get()->N_bytes);
#endif

  nas_5gs_msg nas_msg;
  TESTASSERT(nas_msg.unpack_outer_hdr(buf) == SRSRAN_SUCCESS);
  TESTASSERT(nas_msg.hdr.extended_protocol_discriminator == nas_5gs_hdr::extended_protocol_discriminator_5gmm);
  TESTASSERT(nas_msg.hdr.security_header_type == nas_5gs_hdr::plain_5gs_nas_message);

  TESTASSERT(nas_msg.unpack(buf) == SRSRAN_SUCCESS);
  TESTASSERT(nas_msg.hdr.message_type == msg_types::options::authentication_request);
  printf("%s\n", nas_msg.hdr.message_type.to_string());

  authentication_request_t& auth_request_msg = nas_msg.authentication_request();
  TESTASSERT(auth_request_msg.ng_ksi.security_context_flag ==
             key_set_identifier_t::security_context_flag_type_::options::native_security_context);
  TESTASSERT(auth_request_msg.ng_ksi.nas_key_set_identifier == 0);
  TESTASSERT(auth_request_msg.abba.abba_contents.size() == 2);
  TESTASSERT(auth_request_msg.abba.abba_contents[0] == 0x00);
  TESTASSERT(auth_request_msg.abba.abba_contents[1] == 0x00);
  TESTASSERT(auth_request_msg.authentication_parameter_rand_present == true);
  TESTASSERT(auth_request_msg.authentication_parameter_rand.rand[0] == 0x16);
  TESTASSERT(auth_request_msg.authentication_parameter_rand.rand[15] == 0x71);
  TESTASSERT(auth_request_msg.authentication_parameter_autn_present == true);
  TESTASSERT(auth_request_msg.authentication_parameter_autn.autn.size() == 16);
  TESTASSERT(auth_request_msg.authentication_parameter_autn.autn[0] == 0xa3);
  TESTASSERT(auth_request_msg.authentication_parameter_autn.autn[15] == 0xed);

  // Packing
  srsran::unique_byte_buffer_t buf_cmp = srsran::make_byte_buffer();
  nas_msg.pack(buf_cmp);
#if HAVE_PCAP
  pcap->write_nas(buf_cmp.get()->msg, buf_cmp.get()->N_bytes);
#endif

  // Compare buffer
  TESTASSERT(buf.get()->N_bytes == buf_cmp.get()->N_bytes);
  TESTASSERT(memcmp(buf.get()->msg, buf_cmp.get()->msg, buf.get()->N_bytes) == 0);
  return SRSRAN_SUCCESS;
}

int authentication_resp_request_unpacking_packing_test(srsran::nas_pcap* pcap)
{
  // Non-Access-Stratum 5GS (NAS)PDU
  //     Plain NAS 5GS Message
  //         Extended protocol discriminator: 5G mobility management messages (126)
  //         0000 .... = Spare Half Octet: 0
  //         .... 0000 = Security header type: Plain NAS message, not security protected (0)
  //         Message type: Authentication response (0x57)
  //         Authentication response parameter
  //             Element ID: 0x2d
  //             Length: 16
  //             RES: a1 1f 51 a4 1d a9 b5 29 b3 3b 04 3a e1 e2 02 08

  uint8_t                      auth_resp_buf[] = {0x7e, 0x00, 0x57, 0x2d, 0x10, 0xa1, 0x1f, 0x51, 0xa4, 0x1d, 0xa9,
                             0xb5, 0x29, 0xb3, 0x3b, 0x04, 0x3a, 0xe1, 0xe2, 0x02, 0x08};
  srsran::unique_byte_buffer_t buf;
  copy_msg_to_buffer(buf, auth_resp_buf);

#if HAVE_PCAP
  pcap->write_nas(buf.get()->msg, buf.get()->N_bytes);
#endif
  nas_5gs_msg nas_msg;

  TESTASSERT(nas_msg.unpack_outer_hdr(buf) == SRSRAN_SUCCESS);
  TESTASSERT(nas_msg.hdr.extended_protocol_discriminator == nas_5gs_hdr::extended_protocol_discriminator_5gmm);
  TESTASSERT(nas_msg.hdr.security_header_type == nas_5gs_hdr::plain_5gs_nas_message);
  TESTASSERT(nas_msg.hdr.message_type == msg_types::options::authentication_response);

  TESTASSERT(nas_msg.unpack(buf) == SRSRAN_SUCCESS);
  TESTASSERT(nas_msg.hdr.message_type == msg_types::options::authentication_response);
  printf("%s\n", nas_msg.hdr.message_type.to_string());
  authentication_response_t& auth_resp = nas_msg.authentication_response();

  TESTASSERT(auth_resp.authentication_response_parameter_present == true);
  TESTASSERT(auth_resp.authentication_response_parameter.res.size() == 16);
  TESTASSERT(auth_resp.authentication_response_parameter.res[0] == 0xa1);
  TESTASSERT(auth_resp.authentication_response_parameter.res[15] == 0x08);

  // Packing
  srsran::unique_byte_buffer_t buf_cmp = srsran::make_byte_buffer();
  nas_msg.pack(buf_cmp);
#if HAVE_PCAP
  pcap->write_nas(buf_cmp.get()->msg, buf_cmp.get()->N_bytes);
#endif

  // Compare buffer
  TESTASSERT(buf.get()->N_bytes == buf_cmp.get()->N_bytes);
  TESTASSERT(memcmp(buf.get()->msg, buf_cmp.get()->msg, buf.get()->N_bytes) == 0);
  return SRSRAN_SUCCESS;
}

int security_command_unpacking_packing_test(srsran::nas_pcap* pcap)
{
  // Non-Access-Stratum 5GS (NAS)PDU
  //   Security protected NAS 5GS message
  //   Plain NAS 5GS Message
  //       Extended protocol discriminator: 5G mobility management messages (126)
  //       0000 .... = Spare Half Octet: 0
  //       .... 0000 = Security header type: Plain NAS message, not security protected (0)
  //       Message type: Security mode command (0x5d)
  //       NAS security algorithms
  //           0000 .... = Type of ciphering algorithm: 5G-EA0 (null ciphering algorithm) (0)
  //           .... 0010 = Type of integrity protection algorithm: 128-5G-IA2 (2)
  //       0000 .... = Spare Half Octet: 0
  //       NAS key set identifier - ngKSI
  //           .... 0... = Type of security context flag (TSC): Native security context (for KSIAMF)
  //           .... .000 = NAS key set identifier: 0
  //       UE security capability - Replayed UE security capabilities
  //           Length: 4
  //           1... .... = 5G-EA0: Supported
  //           .1.. .... = 128-5G-EA1: Supported
  //           ..1. .... = 128-5G-EA2: Supported
  //           ...1 .... = 128-5G-EA3: Supported
  //           .... 0... = 5G-EA4: Not supported
  //           .... .0.. = 5G-EA5: Not supported
  //           .... ..0. = 5G-EA6: Not supported
  //           .... ...0 = 5G-EA7: Not supported
  //           0... .... = 5G-IA0: Not supported
  //           .1.. .... = 128-5G-IA1: Supported
  //           ..1. .... = 128-5G-IA2: Supported
  //           ...1 .... = 128-5G-IA3: Supported
  //           .... 0... = 5G-IA4: Not supported
  //           .... .0.. = 5G-IA5: Not supported
  //           .... ..0. = 5G-IA6: Not supported
  //           .... ...0 = 5G-IA7: Not supported
  //           1... .... = EEA0: Supported
  //           .1.. .... = 128-EEA1: Supported
  //           ..1. .... = 128-EEA2: Supported
  //           ...1 .... = 128-EEA3: Supported
  //           .... 0... = EEA4: Not supported
  //           .... .0.. = EEA5: Not supported
  //           .... ..0. = EEA6: Not supported
  //           .... ...0 = EEA7: Not supported
  //           0... .... = EIA0: Not supported
  //           .1.. .... = 128-EIA1: Supported
  //           ..1. .... = 128-EIA2: Supported
  //           ...1 .... = 128-EIA3: Supported
  //           .... 0... = EIA4: Not supported
  //           .... .0.. = EIA5: Not supported
  //           .... ..0. = EIA6: Not supported
  //           .... ...0 = EIA7: Not supported
  //       IMEISV request
  //           1110 .... = Element ID: 0xe-
  //           .... 0... = Spare bit(s): 0x00
  //           .... .001 = IMEISV request: IMEISV requested (1)
  //       Additional 5G security information
  //           Element ID: 0x36
  //           Length: 1
  //           .... 0... = Spare: 0
  //           .... .0.. = Spare: 0
  //           .... ..0. = Retransmission of initial NAS message request(RINMR): Not Requested
  //           .... ...0 = Horizontal derivation parameter (HDP): Not required

  uint8_t                      sec_command[] = {0x7e, 0x03, 0x53, 0x3f, 0xcb, 0x29, 0x00, 0x7e, 0x00, 0x5d, 0x02,
                           0x00, 0x04, 0xf0, 0x70, 0xf0, 0x70, 0xe1, 0x36, 0x01, 0x00};
  srsran::unique_byte_buffer_t buf;
  copy_msg_to_buffer(buf, sec_command);

#if HAVE_PCAP
  pcap->write_nas(buf.get()->msg, buf.get()->N_bytes);
#endif
  nas_5gs_msg nas_msg;

  TESTASSERT(nas_msg.unpack_outer_hdr(buf) == SRSRAN_SUCCESS);
  TESTASSERT(nas_msg.hdr.extended_protocol_discriminator == nas_5gs_hdr::extended_protocol_discriminator_5gmm);
  TESTASSERT(nas_msg.hdr.security_header_type == nas_5gs_hdr::integrity_protected_with_new_5G_nas_context);
  TESTASSERT(nas_msg.hdr.message_authentication_code == 0x533fcb29);
  TESTASSERT(nas_msg.hdr.sequence_number == 0);

  TESTASSERT(nas_msg.unpack(buf) == SRSRAN_SUCCESS);
  TESTASSERT(nas_msg.hdr.inner_extended_protocol_discriminator == nas_5gs_hdr::extended_protocol_discriminator_5gmm);
  TESTASSERT(nas_msg.hdr.inner_security_header_type == nas_5gs_hdr::plain_5gs_nas_message);
  TESTASSERT(nas_msg.hdr.message_type == msg_types::options::security_mode_command);
  printf("%s\n", nas_msg.hdr.message_type.to_string());

  security_mode_command_t& sec_command_msg = nas_msg.security_mode_command();
  TESTASSERT(sec_command_msg.ng_ksi.security_context_flag ==
             key_set_identifier_t::security_context_flag_type_::options::native_security_context);
  TESTASSERT(sec_command_msg.ng_ksi.nas_key_set_identifier == 0);

  TESTASSERT(sec_command_msg.selected_nas_security_algorithms.ciphering_algorithm ==
             security_algorithms_t::ciphering_algorithm_type_::options::ea0_5g);
  TESTASSERT(sec_command_msg.selected_nas_security_algorithms.integrity_protection_algorithm ==
             security_algorithms_t::integrity_protection_algorithm_type_::options::ia2_128_5g);

  TESTASSERT(sec_command_msg.replayed_ue_security_capabilities.ea0_5g_supported == true);
  TESTASSERT(sec_command_msg.replayed_ue_security_capabilities.ea1_128_5g_supported == true);
  TESTASSERT(sec_command_msg.replayed_ue_security_capabilities.ea2_128_5g_supported == true);
  TESTASSERT(sec_command_msg.replayed_ue_security_capabilities.ea3_128_5g_supported == true);
  TESTASSERT(sec_command_msg.replayed_ue_security_capabilities.ea4_5g_supported == false);
  TESTASSERT(sec_command_msg.replayed_ue_security_capabilities.ea5_5g_supported == false);
  TESTASSERT(sec_command_msg.replayed_ue_security_capabilities.ea6_5g_supported == false);
  TESTASSERT(sec_command_msg.replayed_ue_security_capabilities.ea7_5g_supported == false);

  TESTASSERT(sec_command_msg.replayed_ue_security_capabilities.ia0_5g_supported == false);
  TESTASSERT(sec_command_msg.replayed_ue_security_capabilities.ia1_128_5g_supported == true);
  TESTASSERT(sec_command_msg.replayed_ue_security_capabilities.ia2_128_5g_supported == true);
  TESTASSERT(sec_command_msg.replayed_ue_security_capabilities.ia3_128_5g_supported == true);
  TESTASSERT(sec_command_msg.replayed_ue_security_capabilities.ia4_5g_supported == false);
  TESTASSERT(sec_command_msg.replayed_ue_security_capabilities.ia5_5g_supported == false);
  TESTASSERT(sec_command_msg.replayed_ue_security_capabilities.ia6_5g_supported == false);
  TESTASSERT(sec_command_msg.replayed_ue_security_capabilities.ia7_5g_supported == false);

  TESTASSERT(sec_command_msg.replayed_ue_security_capabilities.eps_caps_present == true);

  TESTASSERT(sec_command_msg.replayed_ue_security_capabilities.eea0_supported == true);
  TESTASSERT(sec_command_msg.replayed_ue_security_capabilities.eea1_128_supported == true);
  TESTASSERT(sec_command_msg.replayed_ue_security_capabilities.eea2_128_supported == true);
  TESTASSERT(sec_command_msg.replayed_ue_security_capabilities.eea3_128_supported == true);
  TESTASSERT(sec_command_msg.replayed_ue_security_capabilities.eea4_supported == false);
  TESTASSERT(sec_command_msg.replayed_ue_security_capabilities.eea5_supported == false);
  TESTASSERT(sec_command_msg.replayed_ue_security_capabilities.eea6_supported == false);
  TESTASSERT(sec_command_msg.replayed_ue_security_capabilities.eea7_supported == false);

  TESTASSERT(sec_command_msg.replayed_ue_security_capabilities.eia0_supported == false);
  TESTASSERT(sec_command_msg.replayed_ue_security_capabilities.eia1_128_supported == true);
  TESTASSERT(sec_command_msg.replayed_ue_security_capabilities.eia2_128_supported == true);
  TESTASSERT(sec_command_msg.replayed_ue_security_capabilities.eia3_128_supported == true);
  TESTASSERT(sec_command_msg.replayed_ue_security_capabilities.eia4_supported == false);
  TESTASSERT(sec_command_msg.replayed_ue_security_capabilities.eia5_supported == false);
  TESTASSERT(sec_command_msg.replayed_ue_security_capabilities.eia6_supported == false);
  TESTASSERT(sec_command_msg.replayed_ue_security_capabilities.eia7_supported == false);

  TESTASSERT(sec_command_msg.imeisv_request_present == true);
  TESTASSERT(sec_command_msg.imeisv_request.imeisv_request == true);

  TESTASSERT(sec_command_msg.additional_5g_security_information_present == true);
  TESTASSERT(sec_command_msg.additional_5g_security_information.rinmr == false);
  TESTASSERT(sec_command_msg.additional_5g_security_information.hdp == false);

  // Packing
  srsran::unique_byte_buffer_t buf_cmp = srsran::make_byte_buffer();
  nas_msg.pack(buf_cmp);

  printf("buf_cmp->N_bytes %d  buf->N_bytes %d\n", buf_cmp->N_bytes, buf->N_bytes);
  printf("\n");
  print_msg(buf);
  printf("\n");
  print_msg(buf_cmp);
  printf("\n");
#if HAVE_PCAP
  pcap->write_nas(buf_cmp.get()->msg, buf_cmp.get()->N_bytes);
#endif

  // Compare buffer
  TESTASSERT(buf.get()->N_bytes == buf_cmp.get()->N_bytes);
  TESTASSERT(memcmp(buf.get()->msg, buf_cmp.get()->msg, buf.get()->N_bytes) == 0);
  return SRSRAN_SUCCESS;
}

int security_complete_unpacking_packing_test(srsran::nas_pcap* pcap)
{
  // Non-Access-Stratum 5GS (NAS)PDU
  //   Security protected NAS 5GS message
  //       Extended protocol discriminator: 5G mobility management messages (126)
  //       0000 .... = Spare Half Octet: 0
  //       .... 0100 = Security header type: Integrity protected and ciphered with new 5GS security context (4)
  //       Message authentication code: 0x4088e4e4
  //       Sequence number: 0
  //   Plain NAS 5GS Message
  //       Extended protocol discriminator: 5G mobility management messages (126)
  //       0000 .... = Spare Half Octet: 0
  //       .... 0000 = Security header type: Plain NAS message, not security protected (0)
  //       Message type: Security mode complete (0x5e)
  //       5GS mobile identity
  //           Element ID: 0x77
  //           Length: 9
  //           .... 0... = Odd/even indication: Even number of identity digits
  //           .... .101 = Type of identity: IMEISV (5)
  //           IMEISV: 8651160458202125
  //       NAS message container
  //           Element ID: 0x71
  //           Length: 87
  //           Non-Access-Stratum 5GS (NAS)PDU
  //               Plain NAS 5GS Message
  //                   Extended protocol discriminator: 5G mobility management messages (126)
  //                   0000 .... = Spare Half Octet: 0
  //                   .... 0000 = Security header type: Plain NAS message, not security protected (0)
  //                   Message type: Registration request (0x41)
  //                   5GS registration type
  //                       .... 1... = Follow-On Request bit (FOR): Follow-on request pending
  //                       .... .001 = 5GS registration type: initial registration (1)
  //                   NAS key set identifier
  //                       0... .... = Type of security context flag (TSC): Native security context (for KSIAMF)
  //                       .111 .... = NAS key set identifier: 7
  //                   5GS mobile identity
  //                       Length: 54
  //                       0... .... = Spare: 0
  //                       .000 .... = SUPI format: IMSI (0)
  //                       .... 0... = Spare: 0
  //                       .... .001 = Type of identity: SUCI (1)
  //                       Mobile Country Code (MCC): Unknown (1)
  //                       Mobile Network Code (MNC): Unknown (01)
  //                       Routing indicator: 17
  //                       .... 0010 = Protection scheme Id: ECIES scheme profile B (2)
  //                       Home network public key identifier: 27
  //                       Scheme output: 03 e3 42 42 99 67 4b 24 bc 8c 8a 54 e2 f9 06 5b f6 92 09 63 b0 9e 37 26 …
  //                           ECC ephemeral public key: 03 e3 42 42 99 67 4b 24 bc 8c 8a 54 e2 f9 06 5b f6 92 09 63 b0
  //                           9e 37 26 … Ciphertext: 00 f9 6d 57 82 MAC tag: 0xbf257ecba4d6ce2d
  //                   5GMM capability
  //                       Element ID: 0x10
  //                       Length: 1
  //                       0... .... = Spare: 0
  //                       .0.. .... = Spare: 0
  //                       ..0. .... = Spare: 0
  //                       ...0 .... = Spare: 0
  //                       .... 0... = Spare: 0
  //                       .... .0.. = LTE Positioning Protocol (LPP) capability: Not Requested
  //                       .... ..1. = HO attach: Supported
  //                       .... ...1 = S1 mode: Requested
  //                   UE security capability
  //                       Element ID: 0x2e
  //                       Length: 4
  //                       1... .... = 5G-EA0: Supported
  //                       .1.. .... = 128-5G-EA1: Supported
  //                       ..1. .... = 128-5G-EA2: Supported
  //                       ...1 .... = 128-5G-EA3: Supported
  //                       .... 0... = 5G-EA4: Not supported
  //                       .... .0.. = 5G-EA5: Not supported
  //                       .... ..0. = 5G-EA6: Not supported
  //                       .... ...0 = 5G-EA7: Not supported
  //                       0... .... = 5G-IA0: Not supported
  //                       .1.. .... = 128-5G-IA1: Supported
  //                       ..1. .... = 128-5G-IA2: Supported
  //                       ...1 .... = 128-5G-IA3: Supported
  //                       .... 0... = 5G-IA4: Not supported
  //                       .... .0.. = 5G-IA5: Not supported
  //                       .... ..0. = 5G-IA6: Not supported
  //                       .... ...0 = 5G-IA7: Not supported
  //                       1... .... = EEA0: Supported
  //                       .1.. .... = 128-EEA1: Supported
  //                       ..1. .... = 128-EEA2: Supported
  //                       ...1 .... = 128-EEA3: Supported
  //                       .... 0... = EEA4: Not supported
  //                       .... .0.. = EEA5: Not supported
  //                       .... ..0. = EEA6: Not supported
  //                       .... ...0 = EEA7: Not supported
  //                       0... .... = EIA0: Not supported
  //                       .1.. .... = 128-EIA1: Supported
  //                       ..1. .... = 128-EIA2: Supported
  //                       ...1 .... = 128-EIA3: Supported
  //                       .... 0... = EIA4: Not supported
  //                       .... .0.. = EIA5: Not supported
  //                       .... ..0. = EIA6: Not supported
  //                       .... ...0 = EIA7: Not supported
  //                   UE network capability
  //                       Element ID: 0x17
  //                       Length: 7
  //                       1... .... = EEA0: Supported
  //                       .1.. .... = 128-EEA1: Supported
  //                       ..1. .... = 128-EEA2: Supported
  //                       ...1 .... = 128-EEA3: Supported
  //                       .... 0... = EEA4: Not supported
  //                       .... .0.. = EEA5: Not supported
  //                       .... ..0. = EEA6: Not supported
  //                       .... ...0 = EEA7: Not supported
  //                       0... .... = EIA0: Not supported
  //                       .1.. .... = 128-EIA1: Supported
  //                       ..1. .... = 128-EIA2: Supported
  //                       ...1 .... = 128-EIA3: Supported
  //                       .... 0... = EIA4: Not supported
  //                       .... .0.. = EIA5: Not supported
  //                       .... ..0. = EIA6: Not supported
  //                       .... ...0 = EIA7: Not supported
  //                       1... .... = UEA0: Supported
  //                       .1.. .... = UEA1: Supported
  //                       ..0. .... = UEA2: Not supported
  //                       ...0 .... = UEA3: Not supported
  //                       .... 0... = UEA4: Not supported
  //                       .... .0.. = UEA5: Not supported
  //                       .... ..0. = UEA6: Not supported
  //                       .... ...0 = UEA7: Not supported
  //                       0... .... = UCS2 support (UCS2): The UE has a preference for the default alphabet
  //                       .1.. .... = UMTS integrity algorithm UIA1: Supported
  //                       ..0. .... = UMTS integrity algorithm UIA2: Not supported
  //                       ...0 .... = UMTS integrity algorithm UIA3: Not supported
  //                       .... 0... = UMTS integrity algorithm UIA4: Not supported
  //                       .... .0.. = UMTS integrity algorithm UIA5: Not supported
  //                       .... ..0. = UMTS integrity algorithm UIA6: Not supported
  //                       .... ...0 = UMTS integrity algorithm UIA7: Not supported
  //                       0... .... = ProSe direct discovery: Not supported
  //                       .0.. .... = ProSe: Not supported
  //                       ..0. .... = H.245 After SRVCC Handover: Not supported
  //                       ...1 .... = Access class control for CSFB: Supported
  //                       .... 0... = LTE Positioning Protocol: Not supported
  //                       .... .0.. = Location services (LCS) notification mechanisms: Not supported
  //                       .... ..0. = SRVCC from E-UTRAN to cdma2000 1xCS: Not supported
  //                       .... ...1 = Notification procedure: Supported
  //                       1... .... = Extended protocol configuration options: Supported
  //                       .0.. .... = Header compression for control plane CIoT EPS optimization: Not supported
  //                       ..0. .... = EMM-REGISTERED w/o PDN connectivity: Not supported
  //                       ...0 .... = S1-U data transfer: Not supported
  //                       .... 0... = User plane CIoT EPS optimization: Not supported
  //                       .... .0.. = Control plane CIoT EPS optimization: Not supported
  //                       .... ..0. = ProSe UE-to-network relay: Not supported
  //                       .... ...0 = ProSe direct communication: Not supported
  //                       1... .... = Signalling for a maximum number of 15 EPS bearer contexts: Supported
  //                       .0.. .... = Service gap control: Not supported
  //                       ..1. .... = N1 mode: Supported
  //                       ...1 .... = Dual connectivity with NR: Supported
  //                       .... 0... = Control plane data backoff: Not supported
  //                       .... .0.. = Restriction on use of enhanced coverage: Not supported
  //                       .... ..0. = V2X communication over PC5: Not supported
  //                       .... ...0 = Multiple DRB: Not supported
  //                   UE's usage setting
  //                       Element ID: 0x18
  //                       Length: 1
  //                       .... 0... = Spare: 0
  //                       .... .0.. = Spare: 0
  //                       .... ..0. = Spare: 0
  //                       .... ...1 = UE's usage setting: Data centric
  //                   LADN indication
  //                       Element ID: 0x74
  //                       Length: 0
  //                   5GS update type
  //                       Element ID: 0x53
  //                       Length: 1
  //                       .... 0... = Spare: 0
  //                       .... .0.. = Spare: 0
  //                       .... ..0. = NG-RAN Radio Capability Update (NG-RAN-RCU): Not Needed
  //                       .... ...1 = SMS requested: SMS over NAS supported

  uint8_t sec_complete[] = {
      0x7e, 0x04, 0x40, 0x88, 0xe4, 0xe4, 0x00, 0x7e, 0x00, 0x5e, 0x77, 0x00, 0x09, 0x85, 0x56, 0x11, 0x06, 0x54, 0x28,
      0x20, 0x21, 0xf5, 0x71, 0x00, 0x57, 0x7e, 0x00, 0x41, 0x79, 0x00, 0x36, 0x01, 0x00, 0xf1, 0x10, 0x71, 0xff, 0x02,
      0x1b, 0x03, 0xe3, 0x42, 0x42, 0x99, 0x67, 0x4b, 0x24, 0xbc, 0x8c, 0x8a, 0x54, 0xe2, 0xf9, 0x06, 0x5b, 0xf6, 0x92,
      0x09, 0x63, 0xb0, 0x9e, 0x37, 0x26, 0x13, 0x48, 0xf5, 0xfe, 0xdc, 0xa2, 0x42, 0x07, 0x91, 0x00, 0xf9, 0x6d, 0x57,
      0x82, 0xbf, 0x25, 0x7e, 0xcb, 0xa4, 0xd6, 0xce, 0x2d, 0x10, 0x01, 0x03, 0x2e, 0x04, 0xf0, 0x70, 0xf0, 0x70, 0x17,
      0x07, 0xf0, 0x70, 0xc0, 0x40, 0x11, 0x80, 0xb0, 0x18, 0x01, 0x01, 0x74, 0x00, 0x00, 0x53, 0x01, 0x01};
  srsran::unique_byte_buffer_t buf;
  copy_msg_to_buffer(buf, sec_complete);

#if HAVE_PCAP
  pcap->write_nas(buf.get()->msg, buf.get()->N_bytes);
#endif
  nas_5gs_msg nas_msg;

  TESTASSERT(nas_msg.unpack_outer_hdr(buf) == SRSRAN_SUCCESS);
  TESTASSERT(nas_msg.hdr.extended_protocol_discriminator == nas_5gs_hdr::extended_protocol_discriminator_5gmm);
  TESTASSERT(nas_msg.hdr.security_header_type == nas_5gs_hdr::integrity_protected_and_ciphered_with_new_5G_nas_context);
  TESTASSERT(nas_msg.hdr.message_authentication_code == 0x4088e4e4);
  TESTASSERT(nas_msg.hdr.sequence_number == 0);

  TESTASSERT(nas_msg.unpack(buf) == SRSRAN_SUCCESS);
  TESTASSERT(nas_msg.hdr.inner_extended_protocol_discriminator == nas_5gs_hdr::extended_protocol_discriminator_5gmm);
  TESTASSERT(nas_msg.hdr.inner_security_header_type == nas_5gs_hdr::plain_5gs_nas_message);
  TESTASSERT(nas_msg.hdr.message_type == msg_types::options::security_mode_complete);
  printf("%s\n", nas_msg.hdr.message_type.to_string());

  security_mode_complete_t& sec_complete_msg = nas_msg.security_mode_complete();

  TESTASSERT(sec_complete_msg.imeisv_present == true);
  TESTASSERT(sec_complete_msg.imeisv.type() == mobile_identity_5gs_t::identity_types_::options::imeisv);
  mobile_identity_5gs_t::imeisv_s imeisv = sec_complete_msg.imeisv.imeisv();

  TESTASSERT(imeisv.odd_even_indicator == false);
  TESTASSERT(imeisv.imeisv[0] == 8);
  TESTASSERT(imeisv.imeisv[1] == 6);
  TESTASSERT(imeisv.imeisv[2] == 5);
  TESTASSERT(imeisv.imeisv[3] == 1);
  TESTASSERT(imeisv.imeisv[4] == 1);
  TESTASSERT(imeisv.imeisv[5] == 6);
  TESTASSERT(imeisv.imeisv[6] == 0);
  TESTASSERT(imeisv.imeisv[7] == 4);
  TESTASSERT(imeisv.imeisv[8] == 5);
  TESTASSERT(imeisv.imeisv[9] == 8);
  TESTASSERT(imeisv.imeisv[10] == 2);
  TESTASSERT(imeisv.imeisv[11] == 0);
  TESTASSERT(imeisv.imeisv[12] == 2);
  TESTASSERT(imeisv.imeisv[13] == 1);
  TESTASSERT(imeisv.imeisv[14] == 2);
  TESTASSERT(imeisv.imeisv[15] == 5);

  TESTASSERT(sec_complete_msg.nas_message_container_present == true);
  TESTASSERT(sec_complete_msg.nas_message_container.nas_message_container.size() == 87);
  nas_5gs_msg inner_message;
  TESTASSERT(inner_message.unpack(sec_complete_msg.nas_message_container.nas_message_container) == SRSRAN_SUCCESS);
  // TODO check content of inner message

  // Packing
  srsran::unique_byte_buffer_t buf_cmp = srsran::make_byte_buffer();
  nas_msg.pack(buf_cmp);
#if HAVE_PCAP
  pcap->write_nas(buf_cmp.get()->msg, buf_cmp.get()->N_bytes);
#endif

  // Compare buffer
  TESTASSERT(buf.get()->N_bytes == buf_cmp.get()->N_bytes);
  TESTASSERT(memcmp(buf.get()->msg, buf_cmp.get()->msg, buf.get()->N_bytes) == 0);
  return SRSRAN_SUCCESS;
}

int registration_accept_unpacking_packing_test(srsran::nas_pcap* pcap)
{
  // Non-Access-Stratum 5GS (NAS)PDU
  //   Security protected NAS 5GS message
  //       Extended protocol discriminator: 5G mobility management messages (126)
  //       0000 .... = Spare Half Octet: 0
  //       .... 0010 = Security header type: Integrity protected and ciphered (2)
  //       Message authentication code: 0xd2b078f7
  //       Sequence number: 1
  //   Plain NAS 5GS Message
  //       Extended protocol discriminator: 5G mobility management messages (126)
  //       0000 .... = Spare Half Octet: 0
  //       .... 0000 = Security header type: Plain NAS message, not security protected (0)
  //       Message type: Registration accept (0x42)
  //       5GS registration result
  //           Length: 1
  //           ...0 .... = NSSAA Performed: False
  //           .... 0... = SMS over NAS: Not Allowed
  //           .... .001 = 5GS registration result: 3GPP access (1)
  //       5GS mobile identity - 5G-GUTI
  //           Element ID: 0x77
  //           Length: 11
  //           .... 0... = Odd/even indication: Even number of identity digits
  //           .... .010 = Type of identity: 5G-GUTI (2)
  //           Mobile Country Code (MCC): Unknown (1)
  //           Mobile Network Code (MNC): Unknown (01)
  //           AMF Region ID: 202
  //           1111 1110 00.. .... = AMF Set ID: 1016
  //           ..00 0000 = AMF Pointer: 0
  //           5G-TMSI: 3 (0x00000003)
  //           <TMSI/P-TMSI/M-TMSI/5G-TMSI: 3 (0x00000003)>
  //       5GS tracking area identity list
  //           Element ID: 0x54
  //           Length: 7
  //           Partial tracking area list  1
  //               .00. .... = Type of list: list of TACs belonging to one PLMN, with non-consecutive TAC values (0)
  //               ...0 0000 = Number of elements: 1 element (0)
  //               Mobile Country Code (MCC): Unknown (1)
  //               Mobile Network Code (MNC): Unknown (01)
  //               TAC: 1
  //       NSSAI - Allowed NSSAI
  //           Element ID: 0x15
  //           Length: 10
  //           S-NSSAI 1
  //               Length: 4
  //               Slice/service type (SST): 1
  //               Slice differentiator (SD): 66051
  //           S-NSSAI 2
  //               Length: 4
  //               Slice/service type (SST): 1
  //               Slice differentiator (SD): 1122867
  //       GPRS Timer 3 - T3512 value
  //           Element ID: 0x5e
  //           Length: 1
  //           GPRS Timer: 60 min
  //               000. .... = Unit: value is incremented in multiples of 10 minutes (0)
  //               ...0 0110 = Timer value: 6
  //       GPRS Timer 2 - T3502 value
  //           Element ID: 0x16
  //           Length: 1
  //           GPRS Timer: 12 min
  //               001. .... = Unit: value is incremented in multiples of 1 minute (1)
  //               ...0 1100 = Timer value: 12

  uint8_t reg_accept[] = {0x7e, 0x02, 0xd2, 0xb0, 0x78, 0xf7, 0x01, 0x7e, 0x00, 0x42, 0x01, 0x01, 0x77, 0x00,
                          0x0b, 0xf2, 0x00, 0xf1, 0x10, 0xca, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x03, 0x54, 0x07,
                          0x00, 0x00, 0xf1, 0x10, 0x00, 0x00, 0x01, 0x15, 0x0a, 0x04, 0x01, 0x01, 0x02, 0x03,
                          0x04, 0x01, 0x11, 0x22, 0x33, 0x5e, 0x01, 0x06, 0x16, 0x01, 0x2c};
  srsran::unique_byte_buffer_t buf;
  copy_msg_to_buffer(buf, reg_accept);

#if HAVE_PCAP
  pcap->write_nas(buf.get()->msg, buf.get()->N_bytes);
#endif
  nas_5gs_msg nas_msg;

  // Unpacking
  TESTASSERT(nas_msg.unpack_outer_hdr(buf) == SRSRAN_SUCCESS);
  TESTASSERT(nas_msg.hdr.extended_protocol_discriminator == nas_5gs_hdr::extended_protocol_discriminator_5gmm);
  TESTASSERT(nas_msg.hdr.security_header_type == nas_5gs_hdr::integrity_protected_and_ciphered);
  TESTASSERT(nas_msg.hdr.message_authentication_code == 0xd2b078f7);
  TESTASSERT(nas_msg.hdr.sequence_number == 1);

  TESTASSERT(nas_msg.unpack(buf) == SRSRAN_SUCCESS);
  TESTASSERT(nas_msg.hdr.inner_extended_protocol_discriminator == nas_5gs_hdr::extended_protocol_discriminator_5gmm);
  TESTASSERT(nas_msg.hdr.inner_security_header_type == nas_5gs_hdr::plain_5gs_nas_message);
  TESTASSERT(nas_msg.hdr.message_type == msg_types::options::registration_accept);
  printf("%s\n", nas_msg.hdr.message_type.to_string());

  // Packing
  srsran::unique_byte_buffer_t buf_cmp = srsran::make_byte_buffer();
  nas_msg.pack(buf_cmp);
#if HAVE_PCAP
  pcap->write_nas(buf_cmp.get()->msg, buf_cmp.get()->N_bytes);
#endif

  return SRSRAN_SUCCESS;
}

int registration_complete_unpacking_packing_test(srsran::nas_pcap* pcap)
{
  // Non-Access-Stratum 5GS (NAS)PDU
  //   Security protected NAS 5GS message
  //       Extended protocol discriminator: 5G mobility management messages (126)
  //       0000 .... = Spare Half Octet: 0
  //       .... 0010 = Security header type: Integrity protected and ciphered (2)
  //       Message authentication code: 0xa0b88817
  //       Sequence number: 1
  //   Plain NAS 5GS Message
  //       Extended protocol discriminator: 5G mobility management messages (126)
  //       0000 .... = Spare Half Octet: 0
  //       .... 0000 = Security header type: Plain NAS message, not security protected (0)
  //       Message type: Registration complete (0x43)

  uint8_t                      reg_complete[] = {0x7e, 0x02, 0xa0, 0xb8, 0x88, 0x17, 0x01, 0x7e, 0x00, 0x43};
  srsran::unique_byte_buffer_t buf;
  copy_msg_to_buffer(buf, reg_complete);

#if HAVE_PCAP
  pcap->write_nas(buf.get()->msg, buf.get()->N_bytes);
#endif
  nas_5gs_msg nas_msg;

  // Unpacking
  TESTASSERT(nas_msg.unpack_outer_hdr(buf) == SRSRAN_SUCCESS);
  TESTASSERT(nas_msg.hdr.extended_protocol_discriminator == nas_5gs_hdr::extended_protocol_discriminator_5gmm);
  TESTASSERT(nas_msg.hdr.security_header_type == nas_5gs_hdr::integrity_protected_and_ciphered);
  TESTASSERT(nas_msg.hdr.message_authentication_code == 0xa0b88817);
  TESTASSERT(nas_msg.hdr.sequence_number == 1);

  TESTASSERT(nas_msg.unpack(buf) == SRSRAN_SUCCESS);
  TESTASSERT(nas_msg.hdr.inner_extended_protocol_discriminator == nas_5gs_hdr::extended_protocol_discriminator_5gmm);
  TESTASSERT(nas_msg.hdr.inner_security_header_type == nas_5gs_hdr::plain_5gs_nas_message);
  TESTASSERT(nas_msg.hdr.message_type == msg_types::options::registration_complete);
  printf("%s\n", nas_msg.hdr.message_type.to_string());

  // Packing
  srsran::unique_byte_buffer_t buf_cmp = srsran::make_byte_buffer();
  nas_msg.pack(buf_cmp);
#if HAVE_PCAP
  pcap->write_nas(buf_cmp.get()->msg, buf_cmp.get()->N_bytes);
#endif

  // Compare buffer
  TESTASSERT(buf.get()->N_bytes == buf_cmp.get()->N_bytes);
  TESTASSERT(memcmp(buf.get()->msg, buf_cmp.get()->msg, buf.get()->N_bytes) == 0);

  return SRSRAN_SUCCESS;
}

int deregistration_request_unpacking_packing_test_2(srsran::nas_pcap* pcap)
{
  // NAS-PDU: 7e 02 b1 b8 76 98 02 7e 00 45 09 00 0b f2 00 f1 10 ca fe 00 00 00 00 03
  //   Non-Access-Stratum 5GS (NAS)PDU
  //       Security protected NAS 5GS message
  //           Extended protocol discriminator: 5G mobility management messages (126)
  //           0000 .... = Spare Half Octet: 0
  //           .... 0010 = Security header type: Integrity protected and ciphered (2)
  //           Message authentication code: 0xb1b87698
  //           Sequence number: 2
  //       Plain NAS 5GS Message
  //           Extended protocol discriminator: 5G mobility management messages (126)
  //           0000 .... = Spare Half Octet: 0
  //           .... 0000 = Security header type: Plain NAS message, not security protected (0)
  //           Message type: Deregistration request (UE originating) (0x45)
  //           De-registration type
  //               .... 1... = Switch off: Switch off
  //               .... .0.. = Re-registration required: re-registration not required
  //               .... ..01 = Access type: 3GPP access (1)
  //           NAS key set identifier
  //               0... .... = Type of security context flag (TSC): Native security context (for KSIAMF)
  //               .000 .... = NAS key set identifier: 0
  //           5GS mobile identity
  //               Length: 11
  //               .... 0... = Odd/even indication: Even number of identity digits
  //               .... .010 = Type of identity: 5G-GUTI (2)
  //               Mobile Country Code (MCC): Unknown (1)
  //               Mobile Network Code (MNC): Unknown (01)
  //               AMF Region ID: 202
  //               1111 1110 00.. .... = AMF Set ID: 1016
  //               ..00 0000 = AMF Pointer: 0
  //               5G-TMSI: 3 (0x00000003)
  //               <TMSI/P-TMSI/M-TMSI/5G-TMSI: 3 (0x00000003)>

  uint8_t deregistration_req[] = {0x7e, 0x02, 0xb1, 0xb8, 0x76, 0x98, 0x02, 0x7e, 0x00, 0x45, 0x09, 0x00,
                                  0x0b, 0x02, 0x00, 0xf1, 0x10, 0xca, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x03};

  srsran::unique_byte_buffer_t buf;
  copy_msg_to_buffer(buf, deregistration_req);
#if HAVE_PCAP
  pcap->write_nas(buf.get()->msg, buf.get()->N_bytes);
#endif

  nas_5gs_msg nas_msg;
  TESTASSERT(nas_msg.unpack_outer_hdr(buf) == SRSRAN_SUCCESS);
  TESTASSERT(nas_msg.hdr.extended_protocol_discriminator == nas_5gs_hdr::extended_protocol_discriminator_5gmm);
  TESTASSERT(nas_msg.hdr.security_header_type == nas_5gs_hdr::integrity_protected_and_ciphered);
  TESTASSERT(nas_msg.hdr.message_authentication_code == 0xb1b87698);
  TESTASSERT(nas_msg.hdr.sequence_number == 2);

  TESTASSERT(nas_msg.unpack(buf) == SRSRAN_SUCCESS);
  TESTASSERT(nas_msg.hdr.inner_extended_protocol_discriminator == nas_5gs_hdr::extended_protocol_discriminator_5gmm);
  TESTASSERT(nas_msg.hdr.inner_security_header_type == nas_5gs_hdr::plain_5gs_nas_message);
  TESTASSERT(nas_msg.hdr.message_type == msg_types::options::deregistration_request_ue_originating);
  printf("%s\n", nas_msg.hdr.message_type.to_string());

  deregistration_request_ue_originating_t& dereg_req_ue_o = nas_msg.deregistration_request_ue_originating();

  TESTASSERT(dereg_req_ue_o.de_registration_type.switch_off ==
             de_registration_type_t::switch_off_type_::options::switch_off);
  TESTASSERT(dereg_req_ue_o.de_registration_type.access_type ==
             de_registration_type_t::access_type_type_::options::access_3_gpp);
  TESTASSERT(dereg_req_ue_o.ng_ksi.security_context_flag ==
             key_set_identifier_t::security_context_flag_type_::options::native_security_context);
  TESTASSERT(dereg_req_ue_o.ng_ksi.nas_key_set_identifier == 0);
  TESTASSERT(dereg_req_ue_o.mobile_identity_5gs.type() == mobile_identity_5gs_t::identity_types_::options::guti_5g);
  mobile_identity_5gs_t::guti_5g_s guti_5g = dereg_req_ue_o.mobile_identity_5gs.guti_5g();

  TESTASSERT(guti_5g.mcc[0] == 0);
  TESTASSERT(guti_5g.mcc[1] == 0);
  TESTASSERT(guti_5g.mcc[2] == 1);
  TESTASSERT(guti_5g.mnc[0] == 0);
  TESTASSERT(guti_5g.mnc[1] == 1);
  TESTASSERT(guti_5g.mnc[2] == 0xf);
  TESTASSERT(guti_5g.amf_region_id == 202);
  TESTASSERT(guti_5g.amf_set_id == 1016);
  TESTASSERT(guti_5g.amf_pointer == 0);
  TESTASSERT(guti_5g.tmsi_5g == 0x00000003);

  // Packing
  srsran::unique_byte_buffer_t buf_cmp = srsran::make_byte_buffer();
  nas_msg.pack(buf_cmp);

#if HAVE_PCAP
  pcap->write_nas(buf_cmp.get()->msg, buf_cmp.get()->N_bytes);
#endif

  // Compare buffer
  TESTASSERT(buf.get()->N_bytes == buf_cmp.get()->N_bytes);
  TESTASSERT(memcmp(buf.get()->msg, buf_cmp.get()->msg, buf.get()->N_bytes) == 0);
  return SRSRAN_SUCCESS;
}

int pdu_session_establishment_request_unpacking_packing_test(srsran::nas_pcap* pcap)
{
  // NAS-PDU: 7e 02 dc f9 1d 1b 02 7e 00 67 01 00 06 2e 0a 00 c1 ff ff 12 0a 81 22 04 …
  //   Non-Access-Stratum 5GS (NAS)PDU
  //       Security protected NAS 5GS message
  //           Extended protocol discriminator: 5G mobility management messages (126)
  //           0000 .... = Spare Half Octet: 0
  //           .... 0010 = Security header type: Integrity protected and ciphered (2)
  //           Message authentication code: 0xdcf91d1b
  //           Sequence number: 2
  //       Plain NAS 5GS Message
  //           Extended protocol discriminator: 5G mobility management messages (126)
  //           0000 .... = Spare Half Octet: 0
  //           .... 0000 = Security header type: Plain NAS message, not security protected (0)
  //           Message type: UL NAS transport (0x67)
  //           0000 .... = Spare Half Octet: 0
  //           Payload container type
  //               .... 0001 = Payload container type: N1 SM information (1)
  //           Payload container
  //               Length: 6
  //               Plain NAS 5GS Message
  //                   Extended protocol discriminator: 5G session management messages (46)
  //                   PDU session identity: PDU session identity value 10 (10)
  //                   Procedure transaction identity: 0
  //                   Message type: PDU session establishment request (0xc1)
  //                   Integrity protection maximum data rate
  //                       Integrity protection maximum data rate for uplink: Full data rate (255)
  //                       Integrity protection maximum data rate for downlink: Full data rate (255)
  //           PDU session identity 2 - PDU session ID
  //               Element ID: 0x12
  //               PDU session identity: PDU session identity value 10 (10)
  //           Request type
  //               1000 .... = Element ID: 0x8-
  //               .... .001 = Request type: Initial request (1)
  //           S-NSSAI
  //               Element ID: 0x22
  //               Length: 4
  //               Slice/service type (SST): 1
  //               Slice differentiator (SD): 66051
  //           DNN
  //               Element ID: 0x25
  //               Length: 9
  //               DNN: internet

  uint8_t pdu_session_bytes[] = {0x7e, 0x02, 0xdc, 0xf9, 0x1d, 0x1b, 0x02, 0x7e, 0x00, 0x67, 0x01, 0x00, 0x06,
                                 0x2e, 0x0a, 0x00, 0xc1, 0xff, 0xff, 0x12, 0x0a, 0x81, 0x22, 0x04, 0x01, 0x01,
                                 0x02, 0x03, 0x25, 0x09, 0x08, 0x69, 0x6e, 0x74, 0x65, 0x72, 0x6e, 0x65, 0x74};
  srsran::unique_byte_buffer_t buf;
  copy_msg_to_buffer(buf, pdu_session_bytes);
#if HAVE_PCAP
  pcap->write_nas(buf.get()->msg, buf.get()->N_bytes);
#endif

  nas_5gs_msg nas_msg;

  // Unpacking
  TESTASSERT(nas_msg.unpack_outer_hdr(buf) == SRSRAN_SUCCESS);
  TESTASSERT(nas_msg.hdr.extended_protocol_discriminator == nas_5gs_hdr::extended_protocol_discriminator_5gmm);
  TESTASSERT(nas_msg.hdr.security_header_type == nas_5gs_hdr::integrity_protected_and_ciphered);
  TESTASSERT(nas_msg.hdr.message_authentication_code == 0xdcf91d1b);
  TESTASSERT(nas_msg.hdr.sequence_number == 2);

  TESTASSERT(nas_msg.unpack(buf) == SRSRAN_SUCCESS);
  TESTASSERT(nas_msg.hdr.inner_extended_protocol_discriminator == nas_5gs_hdr::extended_protocol_discriminator_5gmm);
  TESTASSERT(nas_msg.hdr.inner_security_header_type == nas_5gs_hdr::plain_5gs_nas_message);
  TESTASSERT(nas_msg.hdr.message_type == msg_types::options::ul_nas_transport);
  printf("%s\n", nas_msg.hdr.message_type.to_string());

  ul_nas_transport_t& ul_nas = nas_msg.ul_nas_transport();
  TESTASSERT(ul_nas.payload_container_type.payload_container_type ==
             payload_container_type_t::Payload_container_type_type_::options::n1_sm_information);
  TESTASSERT(ul_nas.payload_container.payload_container_contents.size() == 6);
  TESTASSERT(ul_nas.payload_container.payload_container_contents[0] == 0x2e);
  TESTASSERT(ul_nas.payload_container.payload_container_contents[5] == 0xff);

  // Unpack inner nas
  nas_5gs_msg inner_nas;
  TESTASSERT(inner_nas.unpack_outer_hdr(ul_nas.payload_container.payload_container_contents) == SRSRAN_SUCCESS);
  TESTASSERT(inner_nas.hdr.extended_protocol_discriminator == nas_5gs_hdr::extended_protocol_discriminator_5gsm);
  TESTASSERT(inner_nas.hdr.pdu_session_identity == 10);
  TESTASSERT(inner_nas.hdr.procedure_transaction_identity == 0);
  TESTASSERT(inner_nas.hdr.message_type == msg_types::options::pdu_session_establishment_request);

  TESTASSERT(inner_nas.unpack(ul_nas.payload_container.payload_container_contents) == SRSRAN_SUCCESS);
  pdu_session_establishment_request_t pdu_sess_est_req = inner_nas.pdu_session_establishment_request();
  TESTASSERT(pdu_sess_est_req.integrity_protection_maximum_data_rate.max_data_rate_upip_uplink ==
             integrity_protection_maximum_data_rate_t::max_data_rate_UPIP_uplink_type_::options::full_data_rate);
  TESTASSERT(pdu_sess_est_req.integrity_protection_maximum_data_rate.max_data_rate_upip_downlink ==
             integrity_protection_maximum_data_rate_t::max_data_rate_UPIP_downlink_type_::options::full_data_rate);

  // Packing inner nas buffer
  srsran::unique_byte_buffer_t buf_cmp_inner = srsran::make_byte_buffer();
  inner_nas.pack(buf_cmp_inner);

  // Compare inner nas buffer
  TESTASSERT(ul_nas.payload_container.payload_container_contents.size() == buf_cmp_inner.get()->N_bytes);
  TESTASSERT(memcmp(ul_nas.payload_container.payload_container_contents.data(),
                    buf_cmp_inner.get()->msg,
                    buf_cmp_inner.get()->N_bytes) == 0);

  // Outer again
  TESTASSERT(ul_nas.pdu_session_id_present == true);
  TESTASSERT(ul_nas.pdu_session_id.pdu_session_identity_2_value == 10);
  TESTASSERT(ul_nas.request_type_present == true);
  TESTASSERT(ul_nas.request_type.request_type_value ==
             request_type_t::Request_type_value_type_::options::initial_request);
  TESTASSERT(ul_nas.s_nssai_present == true);
  TESTASSERT(ul_nas.s_nssai.sst == 1);
  TESTASSERT(ul_nas.s_nssai.sd == 66051);
  TESTASSERT(ul_nas.dnn_present == true);
  TESTASSERT(ul_nas.dnn.dnn_value.size() == 9);
  TESTASSERT(ul_nas.dnn.dnn_value[0] == 0x08);
  TESTASSERT(ul_nas.dnn.dnn_value[1] == 0x69);
  TESTASSERT(ul_nas.dnn.dnn_value[8] == 0x74);

  // Packing
  srsran::unique_byte_buffer_t buf_cmp = srsran::make_byte_buffer();
  nas_msg.pack(buf_cmp);

#if HAVE_PCAP
  pcap->write_nas(buf_cmp.get()->msg, buf_cmp.get()->N_bytes);
#endif

  // Compare buffer
  TESTASSERT(buf.get()->N_bytes == buf_cmp.get()->N_bytes);
  TESTASSERT(memcmp(buf.get()->msg, buf_cmp.get()->msg, buf.get()->N_bytes) == 0);
  return SRSRAN_SUCCESS;
}

int pdu_session_est_req_accecpt(srsran::nas_pcap* pcap)
{
  // pDUSessionNAS-PDU: 7e 02 1a ca a2 92 02 7e 00 68 01 00 1d 2e 0a 00 c2 11 00 08 01 06 31 31 …
  //     Non-Access-Stratum 5GS (NAS)PDU
  //         Security protected NAS 5GS message
  //             Extended protocol discriminator: 5G mobility management messages (126)
  //             0000 .... = Spare Half Octet: 0
  //             .... 0010 = Security header type: Integrity protected and ciphered (2)
  //             Message authentication code: 0x1acaa292
  //             Sequence number: 2
  //         Plain NAS 5GS Message
  //             Extended protocol discriminator: 5G mobility management messages (126)
  //             0000 .... = Spare Half Octet: 0
  //             .... 0000 = Security header type: Plain NAS message, not security protected (0)
  //             Message type: DL NAS transport (0x68)
  //             0000 .... = Spare Half Octet: 0
  //             Payload container type
  //                 .... 0001 = Payload container type: N1 SM information (1)
  //             Payload container
  //                 Length: 29
  //                 Plain NAS 5GS Message
  //                     Extended protocol discriminator: 5G session management messages (46)
  //                     PDU session identity: PDU session identity value 10 (10)
  //                     Procedure transaction identity: 0
  //                     Message type: PDU session establishment accept (0xc2)
  //                     .001 .... = Selected SSC mode: SSC mode 1 (1)
  //                     PDU session type - Selected PDU session type
  //                         .... .001 = PDU session type: IPv4 (1)
  //                     QoS rules - Authorized QoS rules
  //                         Length: 8
  //                         QoS rule 1
  //                             QoS rule identifier: 1
  //                             Length: 1585
  //                             001. .... = Rule operation code: Create new QoS rule (1)
  //                             ...1 .... = DQR: The QoS rule is the default QoS rule
  //                             .... 0001 = Number of packet filters: 1
  //                             Packet filter 1
  //                                 ..00 .... = Packet filter direction: Reserved (0)
  //                                 .... 0001 = Packet filter identifier: 1
  //                                 Length: 1
  //                                 Packet filter component 1
  //                                     Packet filter component type: Unknown (0)
  //                                     Not dissected yet
  //                                         [Expert Info (Note/Protocol): Not dissected yet]
  //                                             [Not dissected yet]
  //                                             <Message: Not dissected yet>
  //                                             [Severity level: Note]
  //                                             [Group: Protocol]
  //                             QoS rule precedence: 9
  //                             0... .... = Spare: 0
  //                             .0.. .... = Spare: 0
  //                             ..00 0110 = Qos flow identifier: 6
  //                     Session-AMBR
  //                         Length: 6
  //                         Unit for Session-AMBR for downlink: value is incremented in multiples of 1 Kbps (1)
  //                         Session-AMBR for downlink: 59395 Kbps (59395)
  //                         Unit for Session-AMBR for uplink: value is incremented in multiples of 1 Kbps (1)
  //                         Session-AMBR for uplink: 59395 Kbps (59395)
  //                     PDU address
  //                         Element ID: 0x29
  //                         Length: 5
  //                         .... .001 = PDU session type: IPv4 (1)
  //                         PDU address information: 60.60.0.1
  //             PDU session identity 2 - PDU session ID
  //                 Element ID: 0x12
  //                 PDU session identity: PDU session identity value 10 (10)

  uint8_t                      pdu_session_bytes[] = {0x7e, 0x02, 0x1a, 0xca, 0xa2, 0x92, 0x02, 0x7e, 0x00, 0x68, 0x01,
                                 0x00, 0x1d, 0x2e, 0x0a, 0x00, 0xc2, 0x11, 0x00, 0x08, 0x01, 0x06,
                                 0x31, 0x31, 0x01, 0x01, 0x00, 0x09, 0x06, 0x01, 0xe8, 0x03, 0x01,
                                 0xe8, 0x03, 0x29, 0x05, 0x01, 0x3c, 0x3c, 0x00, 0x01, 0x12, 0x0a};
  srsran::unique_byte_buffer_t buf;
  copy_msg_to_buffer(buf, pdu_session_bytes);
#if HAVE_PCAP
  pcap->write_nas(buf.get()->msg, buf.get()->N_bytes);
#endif

  nas_5gs_msg nas_msg;

  // Unpacking
  TESTASSERT(nas_msg.unpack_outer_hdr(buf) == SRSRAN_SUCCESS);
  TESTASSERT(nas_msg.hdr.extended_protocol_discriminator == nas_5gs_hdr::extended_protocol_discriminator_5gmm);
  TESTASSERT(nas_msg.hdr.security_header_type == nas_5gs_hdr::integrity_protected_and_ciphered);
  TESTASSERT(nas_msg.hdr.message_authentication_code == 0x1acaa292);
  TESTASSERT(nas_msg.hdr.sequence_number == 2);

  TESTASSERT(nas_msg.unpack(buf) == SRSRAN_SUCCESS);
  TESTASSERT(nas_msg.hdr.inner_extended_protocol_discriminator == nas_5gs_hdr::extended_protocol_discriminator_5gmm);
  TESTASSERT(nas_msg.hdr.inner_security_header_type == nas_5gs_hdr::plain_5gs_nas_message);
  TESTASSERT(nas_msg.hdr.message_type == msg_types::options::dl_nas_transport);
  printf("%s\n", nas_msg.hdr.message_type.to_string());

  dl_nas_transport_t& dl_nas = nas_msg.dl_nas_transport();
  TESTASSERT(dl_nas.payload_container_type.payload_container_type ==
             payload_container_type_t::Payload_container_type_type_::options::n1_sm_information);
  TESTASSERT(dl_nas.payload_container.payload_container_contents.size() == 29);
  TESTASSERT(dl_nas.payload_container.payload_container_contents[0] == 0x2e);
  TESTASSERT(dl_nas.payload_container.payload_container_contents[28] == 0x01);

  // Unpack inner nas
  nas_5gs_msg inner_nas;
  TESTASSERT(inner_nas.unpack_outer_hdr(dl_nas.payload_container.payload_container_contents) == SRSRAN_SUCCESS);
  TESTASSERT(inner_nas.hdr.extended_protocol_discriminator == nas_5gs_hdr::extended_protocol_discriminator_5gsm);
  TESTASSERT(inner_nas.hdr.pdu_session_identity == 10);
  TESTASSERT(inner_nas.hdr.procedure_transaction_identity == 0);
  TESTASSERT(inner_nas.hdr.message_type == msg_types::options::pdu_session_establishment_accept);

  TESTASSERT(inner_nas.unpack(dl_nas.payload_container.payload_container_contents) == SRSRAN_SUCCESS);
  pdu_session_establishment_accept_t pdu_sess_est_acc = inner_nas.pdu_session_establishment_accept();
  TESTASSERT(pdu_sess_est_acc.selected_ssc_mode.ssc_mode_value ==
             ssc_mode_t::SSC_mode_value_type_::options::ssc_mode_1);
  TESTASSERT(pdu_sess_est_acc.selected_pdu_session_type.pdu_session_type_value ==
             pdu_session_type_t::PDU_session_type_value_type_::options::ipv4);

  TESTASSERT(pdu_sess_est_acc.session_ambr.unit_session_ambr_for_downlink ==
             session_ambr_t::unit_session_AMBR_type_::options::inc_by_1_kbps);
  TESTASSERT(pdu_sess_est_acc.session_ambr.session_ambr_for_downlink == 59395);
  TESTASSERT(pdu_sess_est_acc.session_ambr.unit_session_ambr_for_uplink ==
             session_ambr_t::unit_session_AMBR_type_::options::inc_by_1_kbps);
  TESTASSERT(pdu_sess_est_acc.session_ambr.session_ambr_for_uplink == 59395);
  TESTASSERT(pdu_sess_est_acc.pdu_address_present == true);
  TESTASSERT(pdu_sess_est_acc.pdu_address.si6_lla == false);
  TESTASSERT(pdu_sess_est_acc.pdu_address.pdu_session_type_value ==
             pdu_address_t::PDU_session_type_value_type_::options::ipv4);
  TESTASSERT(pdu_sess_est_acc.pdu_address.ipv4[0] == 0x3c);
  TESTASSERT(pdu_sess_est_acc.pdu_address.ipv4[1] == 0x3c);
  TESTASSERT(pdu_sess_est_acc.pdu_address.ipv4[2] == 0x00);
  TESTASSERT(pdu_sess_est_acc.pdu_address.ipv4[3] == 0x01);

  // Packing inner nas buffer
  srsran::unique_byte_buffer_t buf_cmp_inner = srsran::make_byte_buffer();
  inner_nas.pack(buf_cmp_inner);

  // Compare inner nas buffer
  // TESTASSERT(dl_nas.payload_container.payload_container_contents.size() == buf_cmp_inner.get()->N_bytes);
  // TESTASSERT(memcmp(dl_nas.payload_container.payload_container_contents.data(),
  //                   buf_cmp_inner.get()->msg,
  //                   buf_cmp_inner.get()->N_bytes) == 0);

  // outer nas again
  TESTASSERT(dl_nas.pdu_session_id_present == true);
  TESTASSERT(dl_nas.pdu_session_id.pdu_session_identity_2_value == 10);
  // Packing
  srsran::unique_byte_buffer_t buf_cmp = srsran::make_byte_buffer();
  nas_msg.pack(buf_cmp);

#if HAVE_PCAP
  pcap->write_nas(buf_cmp.get()->msg, buf_cmp.get()->N_bytes);
#endif

  // Compare buffer
  TESTASSERT(buf.get()->N_bytes == buf_cmp.get()->N_bytes);
  TESTASSERT(memcmp(buf.get()->msg, buf_cmp.get()->msg, buf.get()->N_bytes) == 0);
  return SRSRAN_SUCCESS;
}

int main()
{
  srslog::init();
  srsran::console("Testing 5G NAS packing and unpacking\n");
#if HAVE_PCAP
  srsran::nas_pcap pcap;
  pcap.open("nas_5g_msg_test.pcap", 0, srsran::srsran_rat_t::nr);
  TESTASSERT(registration_request_unpacking_packing_test(&pcap) == SRSRAN_SUCCESS);
  TESTASSERT(registration_request_unpacking_packing_test_2(&pcap) == SRSRAN_SUCCESS);
  TESTASSERT(deregistration_request_unpacking_packing_test(&pcap) == SRSRAN_SUCCESS);
  TESTASSERT(authentication_request_unpacking_packing_test(&pcap) == SRSRAN_SUCCESS);
  TESTASSERT(authentication_resp_request_unpacking_packing_test(&pcap) == SRSRAN_SUCCESS);
  TESTASSERT(security_command_unpacking_packing_test(&pcap) == SRSRAN_SUCCESS);
  TESTASSERT(security_complete_unpacking_packing_test(&pcap) == SRSRAN_SUCCESS);
  TESTASSERT(registration_accept_unpacking_packing_test(&pcap) == SRSRAN_SUCCESS);
  TESTASSERT(registration_complete_unpacking_packing_test(&pcap) == SRSRAN_SUCCESS);
  TESTASSERT(deregistration_request_unpacking_packing_test_2(&pcap) == SRSRAN_SUCCESS);
  TESTASSERT(pdu_session_establishment_request_unpacking_packing_test(&pcap) == SRSRAN_SUCCESS);
  TESTASSERT(pdu_session_est_req_accecpt(&pcap) == SRSRAN_SUCCESS);
  pcap.close();
#else
  TESTASSERT(registration_request_unpacking_packing_test(nullptr) == SRSRAN_SUCCESS);
  TESTASSERT(registration_request_unpacking_packing_test_2(nullptr) == SRSRAN_SUCCESS);
  TESTASSERT(deregistration_request_unpacking_packing_test(nullptr) == SRSRAN_SUCCESS);
  TESTASSERT(authentication_request_unpacking_packing_test(nullptr) == SRSRAN_SUCCESS);
  TESTASSERT(authentication_resp_request_unpacking_packing_test(nullptr) == SRSRAN_SUCCESS);
  TESTASSERT(security_command_unpacking_packing_test(nullptr) == SRSRAN_SUCCESS);
  TESTASSERT(security_complete_unpacking_packing_test(nullptr) == SRSRAN_SUCCESS);
  TESTASSERT(registration_accept_unpacking_packing_test(nullptr) == SRSRAN_SUCCESS);
  TESTASSERT(registration_complete_unpacking_packing_test(nullptr) == SRSRAN_SUCCESS);
  TESTASSERT(deregistration_request_unpacking_packing_test_2(nullptr) == SRSRAN_SUCCESS);
  TESTASSERT(pdu_session_establishment_request_unpacking_packing_test(nullptr) == SRSRAN_SUCCESS);
  TESTASSERT(pdu_session_est_req_accecpt(nullptr) == SRSRAN_SUCCESS);
#endif
  return SRSRAN_SUCCESS;
}
