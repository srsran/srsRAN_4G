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

#include <assert.h>
#include "srslte/asn1/liblte_rrc.h"
#include "srslte/common/log_filter.h"
#include "srslte/phy/utils/bit.h"

void pack_test()
{
  srslte::log_filter log1("RRC");
  log1.set_level(srslte::LOG_LEVEL_DEBUG);
  log1.set_hex_limit(1024);

  uint32_t known_reference_len  = 30;
  uint8_t  known_reference[256] = {0x0d, 0x8f, 0xdf, 0xff, 0xff, 0xff, 0xe2, 0x2f,
                                   0xfc, 0x38, 0x5e, 0x61, 0xec, 0xa8, 0x00, 0x00,
                                   0x02, 0x02, 0x10, 0x00, 0x20, 0x05, 0xe6, 0x1e,
                                   0xca, 0x80, 0x00, 0x00, 0x40, 0x42};

  LIBLTE_BYTE_MSG_STRUCT     byte_buf;
  LIBLTE_BIT_MSG_STRUCT      bit_buf;
  LIBLTE_RRC_MCCH_MSG_STRUCT mcch_msg;

  mcch_msg.commonsf_allocpatternlist_r9_size = 2;
  mcch_msg.commonsf_allocpatternlist_r9[0].radio_fr_alloc_offset = 4;
  mcch_msg.commonsf_allocpatternlist_r9[0].radio_fr_alloc_period = LIBLTE_RRC_RADIO_FRAME_ALLOCATION_PERIOD_N32;
  mcch_msg.commonsf_allocpatternlist_r9[0].subfr_alloc_num_frames = LIBLTE_RRC_SUBFRAME_ALLOCATION_NUM_FRAMES_ONE;
  mcch_msg.commonsf_allocpatternlist_r9[0].subfr_alloc = 0x3F;
  mcch_msg.commonsf_allocpatternlist_r9[1].radio_fr_alloc_offset = 7;
  mcch_msg.commonsf_allocpatternlist_r9[1].radio_fr_alloc_period = LIBLTE_RRC_RADIO_FRAME_ALLOCATION_PERIOD_N8;
  mcch_msg.commonsf_allocpatternlist_r9[1].subfr_alloc_num_frames = LIBLTE_RRC_SUBFRAME_ALLOCATION_NUM_FRAMES_FOUR;
  mcch_msg.commonsf_allocpatternlist_r9[1].subfr_alloc = 0xFFFFFF;

  mcch_msg.commonsf_allocperiod_r9 = LIBLTE_RRC_MBSFN_COMMON_SF_ALLOC_PERIOD_R9_RF256;

  mcch_msg.pmch_infolist_r9_size = 2;

  mcch_msg.pmch_infolist_r9[0].mbms_sessioninfolist_r9_size = 1;
  mcch_msg.pmch_infolist_r9[0].mbms_sessioninfolist_r9[0].logicalchannelid_r9 = 1;
  mcch_msg.pmch_infolist_r9[0].mbms_sessioninfolist_r9[0].sessionid_r9_present = true;
  mcch_msg.pmch_infolist_r9[0].mbms_sessioninfolist_r9[0].sessionid_r9 = 1;
  mcch_msg.pmch_infolist_r9[0].mbms_sessioninfolist_r9[0].tmgi_r9.plmn_id_explicit = true;
  mcch_msg.pmch_infolist_r9[0].mbms_sessioninfolist_r9[0].tmgi_r9.plmn_id_r9.mcc = 0xF987;
  mcch_msg.pmch_infolist_r9[0].mbms_sessioninfolist_r9[0].tmgi_r9.plmn_id_r9.mnc = 0xF654;
  mcch_msg.pmch_infolist_r9[0].mbms_sessioninfolist_r9[0].tmgi_r9.serviceid_r9 = 1;
  mcch_msg.pmch_infolist_r9[0].pmch_config_r9.datamcs_r9 = 16;
  mcch_msg.pmch_infolist_r9[0].pmch_config_r9.mch_schedulingperiod_r9 = LIBLTE_RRC_MCH_SCHEDULING_PERIOD_R9_RF1024;
  mcch_msg.pmch_infolist_r9[0].pmch_config_r9.sf_alloc_end_r9 = 1535;

  mcch_msg.pmch_infolist_r9[1].mbms_sessioninfolist_r9_size = 1;
  mcch_msg.pmch_infolist_r9[1].mbms_sessioninfolist_r9[0].logicalchannelid_r9 = 2;
  mcch_msg.pmch_infolist_r9[1].mbms_sessioninfolist_r9[0].sessionid_r9_present = true;
  mcch_msg.pmch_infolist_r9[1].mbms_sessioninfolist_r9[0].sessionid_r9 = 2;
  mcch_msg.pmch_infolist_r9[1].mbms_sessioninfolist_r9[0].tmgi_r9.plmn_id_explicit = true;
  mcch_msg.pmch_infolist_r9[1].mbms_sessioninfolist_r9[0].tmgi_r9.plmn_id_r9.mcc = 0xF987;
  mcch_msg.pmch_infolist_r9[1].mbms_sessioninfolist_r9[0].tmgi_r9.plmn_id_r9.mnc = 0xF654;
  mcch_msg.pmch_infolist_r9[1].mbms_sessioninfolist_r9[0].tmgi_r9.serviceid_r9 = 2;
  mcch_msg.pmch_infolist_r9[1].pmch_config_r9.datamcs_r9 = 8;
  mcch_msg.pmch_infolist_r9[1].pmch_config_r9.mch_schedulingperiod_r9 = LIBLTE_RRC_MCH_SCHEDULING_PERIOD_R9_RF8;
  mcch_msg.pmch_infolist_r9[1].pmch_config_r9.sf_alloc_end_r9 = 0;

  liblte_rrc_pack_mcch_msg(&mcch_msg, &bit_buf);
  liblte_pack(bit_buf.msg, bit_buf.N_bits, byte_buf.msg);
  byte_buf.N_bytes = (bit_buf.N_bits+7)/8;

  //log1.info_hex(byte_buf.msg, byte_buf.N_bytes, "MCCH packed message:");

  assert(byte_buf.N_bytes == known_reference_len);
  for(uint32 i=0; i<known_reference_len; i++) {
    assert(byte_buf.msg[i] == known_reference[i]);
  }

}

void unpack_test()
{
  uint32_t known_reference_len  = 30;
  uint8_t  known_reference[256] = {0x0d, 0x8f, 0xdf, 0xff, 0xff, 0xff, 0xe2, 0x2f,
                                   0xfc, 0x38, 0x5e, 0x61, 0xec, 0xa8, 0x00, 0x00,
                                   0x02, 0x02, 0x10, 0x00, 0x20, 0x05, 0xe6, 0x1e,
                                   0xca, 0x80, 0x00, 0x00, 0x40, 0x42};

  LIBLTE_BYTE_MSG_STRUCT     byte_buf;
  LIBLTE_BIT_MSG_STRUCT      bit_buf;
  LIBLTE_RRC_MCCH_MSG_STRUCT mcch_msg;

  liblte_unpack(known_reference, known_reference_len, bit_buf.msg);
  bit_buf.N_bits = known_reference_len*8;
  liblte_rrc_unpack_mcch_msg(&bit_buf, &mcch_msg);

  assert(mcch_msg.commonsf_allocpatternlist_r9_size == 2);
  assert(mcch_msg.commonsf_allocpatternlist_r9[0].radio_fr_alloc_offset == 4);
  assert(mcch_msg.commonsf_allocpatternlist_r9[0].radio_fr_alloc_period == LIBLTE_RRC_RADIO_FRAME_ALLOCATION_PERIOD_N32);
  assert(mcch_msg.commonsf_allocpatternlist_r9[0].subfr_alloc_num_frames == LIBLTE_RRC_SUBFRAME_ALLOCATION_NUM_FRAMES_ONE);
  assert(mcch_msg.commonsf_allocpatternlist_r9[0].subfr_alloc == 0x3F);
  assert(mcch_msg.commonsf_allocpatternlist_r9[1].radio_fr_alloc_offset == 7);
  assert(mcch_msg.commonsf_allocpatternlist_r9[1].radio_fr_alloc_period == LIBLTE_RRC_RADIO_FRAME_ALLOCATION_PERIOD_N8);
  assert(mcch_msg.commonsf_allocpatternlist_r9[1].subfr_alloc_num_frames == LIBLTE_RRC_SUBFRAME_ALLOCATION_NUM_FRAMES_FOUR);
  assert(mcch_msg.commonsf_allocpatternlist_r9[1].subfr_alloc == 0xFFFFFF);

  assert(mcch_msg.commonsf_allocperiod_r9 == LIBLTE_RRC_MBSFN_COMMON_SF_ALLOC_PERIOD_R9_RF256);

  assert(mcch_msg.pmch_infolist_r9_size == 2);

  assert(mcch_msg.pmch_infolist_r9[0].mbms_sessioninfolist_r9_size == 1);
  assert(mcch_msg.pmch_infolist_r9[0].mbms_sessioninfolist_r9[0].logicalchannelid_r9 == 1);
  assert(mcch_msg.pmch_infolist_r9[0].mbms_sessioninfolist_r9[0].sessionid_r9_present == true);
  assert(mcch_msg.pmch_infolist_r9[0].mbms_sessioninfolist_r9[0].sessionid_r9 == 1);
  assert(mcch_msg.pmch_infolist_r9[0].mbms_sessioninfolist_r9[0].tmgi_r9.plmn_id_explicit == true);
  assert(mcch_msg.pmch_infolist_r9[0].mbms_sessioninfolist_r9[0].tmgi_r9.plmn_id_r9.mcc == 0xF987);
  assert(mcch_msg.pmch_infolist_r9[0].mbms_sessioninfolist_r9[0].tmgi_r9.plmn_id_r9.mnc == 0xF654);
  assert(mcch_msg.pmch_infolist_r9[0].mbms_sessioninfolist_r9[0].tmgi_r9.serviceid_r9 == 1);
  assert(mcch_msg.pmch_infolist_r9[0].pmch_config_r9.datamcs_r9 == 16);
  assert(mcch_msg.pmch_infolist_r9[0].pmch_config_r9.mch_schedulingperiod_r9 == LIBLTE_RRC_MCH_SCHEDULING_PERIOD_R9_RF1024);
  assert(mcch_msg.pmch_infolist_r9[0].pmch_config_r9.sf_alloc_end_r9 == 1535);

  assert(mcch_msg.pmch_infolist_r9[1].mbms_sessioninfolist_r9_size == 1);
  assert(mcch_msg.pmch_infolist_r9[1].mbms_sessioninfolist_r9[0].logicalchannelid_r9 == 2);
  assert(mcch_msg.pmch_infolist_r9[1].mbms_sessioninfolist_r9[0].sessionid_r9_present == true);
  assert(mcch_msg.pmch_infolist_r9[1].mbms_sessioninfolist_r9[0].sessionid_r9 == 2);
  assert(mcch_msg.pmch_infolist_r9[1].mbms_sessioninfolist_r9[0].tmgi_r9.plmn_id_explicit == true);
  assert(mcch_msg.pmch_infolist_r9[1].mbms_sessioninfolist_r9[0].tmgi_r9.plmn_id_r9.mcc == 0xF987);
  assert(mcch_msg.pmch_infolist_r9[1].mbms_sessioninfolist_r9[0].tmgi_r9.plmn_id_r9.mnc == 0xF654);
  assert(mcch_msg.pmch_infolist_r9[1].mbms_sessioninfolist_r9[0].tmgi_r9.serviceid_r9 == 2);
  assert(mcch_msg.pmch_infolist_r9[1].pmch_config_r9.datamcs_r9 == 8);
  assert(mcch_msg.pmch_infolist_r9[1].pmch_config_r9.mch_schedulingperiod_r9 == LIBLTE_RRC_MCH_SCHEDULING_PERIOD_R9_RF8);
  assert(mcch_msg.pmch_infolist_r9[1].pmch_config_r9.sf_alloc_end_r9 == 0);

}

int main(int argc, char **argv)
{
  pack_test();
  unpack_test();
}

