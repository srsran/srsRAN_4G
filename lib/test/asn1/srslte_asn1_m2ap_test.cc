/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
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

#include "srslte/asn1/liblte_m2ap.h"
#include "srslte/common/log_filter.h"
#include <iostream>
#include <memory>
#include <srslte/srslte.h>

#define TESTASSERT(cond)                                                                                               \
  {                                                                                                                    \
    if (!(cond)) {                                                                                                     \
      std::cout << "[" << __FUNCTION__ << "][Line " << __LINE__ << "]: FAIL at " << (#cond) << std::endl;              \
      return -1;                                                                                                       \
    }                                                                                                                  \
  }

int m2_setup_request_test()
{
  srslte::log_filter log1("M2AP");
  log1.set_level(srslte::LOG_LEVEL_DEBUG);
  log1.set_hex_limit(128);

  LIBLTE_BYTE_MSG_STRUCT                       tst_msg = {};
  LIBLTE_BYTE_MSG_STRUCT                       out_msg = {};
  std::unique_ptr<LIBLTE_M2AP_M2AP_PDU_STRUCT> m2ap_pdu(new LIBLTE_M2AP_M2AP_PDU_STRUCT);

  uint32_t m2ap_message_len = 59;
  uint8_t  m2ap_message[]   = {0x00, 0x05, 0x00, 0x37, 0x00, 0x00, 0x03, 0x00, 0x0d, 0x00, 0x08, 0x00, 0x00, 0xf1, 0x10,
                            0x00, 0x1a, 0x2d, 0x00, 0x00, 0x0e, 0x40, 0x0a, 0x03, 0x80, 0x65, 0x6e, 0x62, 0x31, 0x61,
                            0x32, 0x64, 0x30, 0x00, 0x0f, 0x00, 0x16, 0x00, 0x00, 0x10, 0x00, 0x11, 0x00, 0x00, 0xf1,
                            0x10, 0x1a, 0x2d, 0x00, 0x10, 0x27, 0x10, 0x01, 0x02, 0x00, 0x01, 0x02, 0x00, 0x02};

  /*M2AP Setup Request Unpack Test*/
  tst_msg.N_bytes = m2ap_message_len;
  memcpy(tst_msg.msg, m2ap_message, m2ap_message_len);
  log1.info_hex(tst_msg.msg, tst_msg.N_bytes, "M2 Setup Request original message\n");

  liblte_m2ap_unpack_m2ap_pdu(&tst_msg, m2ap_pdu.get());
  TESTASSERT(m2ap_pdu->choice_type == LIBLTE_M2AP_M2AP_PDU_CHOICE_INITIATINGMESSAGE);

  LIBLTE_M2AP_INITIATINGMESSAGE_STRUCT* in_msg = &m2ap_pdu->choice.initiatingMessage;
  TESTASSERT(in_msg->choice_type == LIBLTE_M2AP_INITIATINGMESSAGE_CHOICE_M2SETUPREQUEST);

  LIBLTE_M2AP_MESSAGE_M2SETUPREQUEST_STRUCT* m2_setup = &in_msg->choice.M2SetupRequest;
  TESTASSERT(m2_setup->ext == false);

  /*Global eNB-Id*/
  // PLMN Identity (MCC = 001 , MNC = 01)
  uint8_t* plmn_id = m2_setup->Global_ENB_ID.pLMNidentity.buffer;
  TESTASSERT((plmn_id[0] & 0x0F) == 0 && (plmn_id[0] & 0xF0) >> 4 == 0 && (plmn_id[1] & 0x0F) == 1);        // MCC
  TESTASSERT((plmn_id[1] & 0xF0) >> 4 == 0xF && (plmn_id[2] & 0x0F) == 0 && (plmn_id[2] & 0xF0) >> 4 == 1); // MNC

  // eNB Identity
  uint8_t  enb_id_bits[32];
  uint32_t enb_id;
  bzero(enb_id_bits, sizeof(enb_id_bits));
  memcpy(&enb_id_bits[32 - LIBLTE_M2AP_ENBID_BIT_STRING_LEN],
         m2_setup->Global_ENB_ID.eNB_ID.buffer,
         LIBLTE_M2AP_ENBID_BIT_STRING_LEN);
  liblte_pack(enb_id_bits, 32, (uint8_t*)&enb_id);
  enb_id = ntohl(enb_id);
  TESTASSERT(enb_id == 0x1a2d0);

  // eNB Name
  TESTASSERT(m2_setup->eNBname_present == true);
  TESTASSERT(m2_setup->eNBname.n_octets == 8);
  TESTASSERT(strncmp((const char*)m2_setup->eNBname.buffer, "enb1a2d0", m2_setup->eNBname.n_octets) == 0);

  // eNB MBMS Configuration Data List
  TESTASSERT(m2_setup->configurationDataList.len == 1);

  // eNB MBMS Configuration Data Item
  LIBLTE_M2AP_ENB_MBMS_CONFIGURATION_DATA_ITEM_STRUCT* conf_item = &m2_setup->configurationDataList.buffer[0];

  // eCGI
  plmn_id = conf_item->eCGI.pLMN_Identity.buffer;
  TESTASSERT((plmn_id[0] & 0x0F) == 0 && (plmn_id[0] & 0xF0) >> 4 == 0 && (plmn_id[1] & 0x0F) == 1);        // MCC
  TESTASSERT((plmn_id[1] & 0xF0) >> 4 == 0xF && (plmn_id[2] & 0x0F) == 0 && (plmn_id[2] & 0xF0) >> 4 == 1); // MNC

  // E-UTRAN Cell Identifier
  TESTASSERT(conf_item->eCGI.EUTRANCellIdentifier.eUTRANCellIdentifier == 27447297);

  // MBSFN Synchronization Area
  TESTASSERT(conf_item->mbsfnSynchronisationArea.mbsfn_synchronisation_area_id == 10000);

  // MBMS Service Area
  TESTASSERT(conf_item->mbmsServiceAreaList.len == 2);
  TESTASSERT(conf_item->mbmsServiceAreaList.buffer[0].n_octets == 2); // Service Area 1
  TESTASSERT((conf_item->mbmsServiceAreaList.buffer[0].buffer[0] == 0) &&
             (conf_item->mbmsServiceAreaList.buffer[0].buffer[1] == 1)); // Service Area 1
  TESTASSERT(conf_item->mbmsServiceAreaList.buffer[1].n_octets == 2);    // Service Area 2
  TESTASSERT((conf_item->mbmsServiceAreaList.buffer[1].buffer[0] == 0) &&
             (conf_item->mbmsServiceAreaList.buffer[1].buffer[1] == 2)); // Service Area 2

  /*M2AP Setup Request Pack Test*/
  liblte_m2ap_pack_m2ap_pdu(m2ap_pdu.get(), &out_msg);
  log1.info_hex(out_msg.msg, out_msg.N_bytes, "M2 Setup Request Packed message\n");

  for (uint32_t i = 0; i < m2ap_message_len; i++) {
    TESTASSERT(tst_msg.msg[i] == out_msg.msg[i]);
  }

  printf("Test M2SetupRequest successfull\n");

  return 0;
}

int m2_setup_response_test()
{
  srslte::log_filter log1("M2AP");
  log1.set_level(srslte::LOG_LEVEL_DEBUG);
  log1.set_hex_limit(128);

  LIBLTE_BYTE_MSG_STRUCT                       tst_msg = {};
  LIBLTE_BYTE_MSG_STRUCT                       out_msg = {};
  std::unique_ptr<LIBLTE_M2AP_M2AP_PDU_STRUCT> m2ap_pdu(new LIBLTE_M2AP_M2AP_PDU_STRUCT);

  uint32_t m2ap_message_len = 40;
  uint8_t  m2ap_message[]   = {0x20, 0x05, 0x00, 0x24, 0x00, 0x00, 0x02, 0x00, 0x11, 0x00, 0x06, 0x00, 0x00, 0xf1,
                            0x10, 0x00, 0x50, 0x00, 0x13, 0x00, 0x13, 0x00, 0x00, 0x14, 0x00, 0x0e, 0x40, 0x01,
                            0x50, 0x40, 0x00, 0x00, 0x00, 0x00, 0xf1, 0x10, 0x1a, 0x2d, 0x00, 0x14};

  /*M2AP Setup Response Unpack Test*/
  tst_msg.N_bytes = m2ap_message_len;
  memcpy(tst_msg.msg, m2ap_message, m2ap_message_len);

  LIBLTE_ERROR_ENUM err = liblte_m2ap_unpack_m2ap_pdu(&tst_msg, m2ap_pdu.get());
  TESTASSERT(err == LIBLTE_SUCCESS);
  TESTASSERT(m2ap_pdu->choice_type == LIBLTE_M2AP_M2AP_PDU_CHOICE_SUCCESSFULOUTCOME);

  LIBLTE_M2AP_SUCCESSFULOUTCOME_STRUCT* succ_out = &m2ap_pdu->choice.successfulOutcome;
  TESTASSERT(succ_out->choice_type == LIBLTE_M2AP_SUCCESSFULOUTCOME_CHOICE_M2SETUPRESPONSE);

  LIBLTE_M2AP_MESSAGE_M2SETUPRESPONSE_STRUCT* m2_setup = &succ_out->choice.M2SetupResponse;
  TESTASSERT(m2_setup->ext == false);

  /*Global MCE-Id*/
  // PLMN Identity (MCC = 001 , MNC = 01)
  uint8_t* plmn_id = m2_setup->Global_MCE_ID.pLMN_Identity.buffer;
  TESTASSERT((plmn_id[0] & 0x0F) == 0 && (plmn_id[0] & 0xF0) >> 4 == 0 && (plmn_id[1] & 0x0F) == 1);        // MCC
  TESTASSERT((plmn_id[1] & 0xF0) >> 4 == 0xF && (plmn_id[2] & 0x0F) == 0 && (plmn_id[2] & 0xF0) >> 4 == 1); // MNC

  // MCE Identity
  uint16_t mce_id;
  memcpy(&mce_id, m2_setup->Global_MCE_ID.mCE_ID.buffer, LIBLTE_M2AP_MCEID_OCTET_STRING_LEN);
  mce_id = ntohs(mce_id);
  TESTASSERT(mce_id == 0x0050);

  /*MCE Name*/
  TESTASSERT(m2_setup->MCEname_present == false); // TODO Test with MCE name

  /*MCCHrelatedBCCH-ConfigPerMBSFNArea*/
  // Length
  TESTASSERT(m2_setup->MCCHrelatedBCCHConfigPerMBSFNArea.len == 1);

  // MCCH Related BCCH Config Per MBSFN Area Configuration Item
  LIBLTE_M2AP_MCCH_RELATED_BCCH_CONFIG_PER_MBSFN_AREA_ITEM_STRUCT* conf_item =
      &m2_setup->MCCHrelatedBCCHConfigPerMBSFNArea.buffer[0];

  // MBSFN Area
  TESTASSERT(conf_item->mbsfnArea.mbsfn_area_id == 1);

  // PDCCH Length
  TESTASSERT(conf_item->pdcchLength.ext == false);
  TESTASSERT(conf_item->pdcchLength.pdcchLength == LIBLTE_M2AP_PDCCH_LENGTH_S2);

  // Repetition Period
  TESTASSERT(conf_item->repetitionPeriod.repetitionPeriod == LIBLTE_M2AP_REPETITION_PERIOD_RF64);

  // Offset
  TESTASSERT(conf_item->offset.offset == 0);

  // Modification Period
  TESTASSERT(conf_item->modificationPeriod.modificationPeriod == LIBLTE_M2AP_MODIFICATION_PERIOD_RF512);

  // Subframe Allocation Info
  TESTASSERT(conf_item->subframeAllocationInfo.buffer[0] == 1);
  TESTASSERT(conf_item->subframeAllocationInfo.buffer[1] == 0);
  TESTASSERT(conf_item->subframeAllocationInfo.buffer[2] == 0);
  TESTASSERT(conf_item->subframeAllocationInfo.buffer[3] == 0);
  TESTASSERT(conf_item->subframeAllocationInfo.buffer[4] == 0);
  TESTASSERT(conf_item->subframeAllocationInfo.buffer[5] == 0);

  // Modulation and Coding Scheme
  TESTASSERT(conf_item->modulationAndCodingScheme.mcs == LIBLTE_M2AP_MODULATION_AND_CODING_SCHEME_N2);

  /*Cell Information List*/
  TESTASSERT(conf_item->cellInformationList_present);
  TESTASSERT(conf_item->cellInformationList.len == 1);

  /*Cell Information*/
  LIBLTE_M2AP_CELL_INFORMATION_STRUCT* cell_info = &conf_item->cellInformationList.buffer[0];

  // eCGI
  plmn_id = cell_info->eCGI.pLMN_Identity.buffer;
  TESTASSERT((plmn_id[0] & 0x0F) == 0 && (plmn_id[0] & 0xF0) >> 4 == 0 && (plmn_id[1] & 0x0F) == 1);        // MCC
  TESTASSERT((plmn_id[1] & 0xF0) >> 4 == 0xF && (plmn_id[2] & 0x0F) == 0 && (plmn_id[2] & 0xF0) >> 4 == 1); // MNC
  // E-UTRAN Cell Identifier
  TESTASSERT(cell_info->eCGI.EUTRANCellIdentifier.eUTRANCellIdentifier == 27447297);
  // Cell Reservation
  TESTASSERT(cell_info->cellReservationInfo.e == LIBLTE_M2AP_CELL_RESERVATION_INFO_NON_RESERVED_CELL);

  /*M2AP Setup Request Pack Test*/
  err = liblte_m2ap_pack_m2ap_pdu(m2ap_pdu.get(), &out_msg);
  log1.info_hex(tst_msg.msg, tst_msg.N_bytes, "M2 Setup Request original message\n");
  log1.info_hex(out_msg.msg, out_msg.N_bytes, "M2 Setup Request Packed message\n");

  TESTASSERT(err == LIBLTE_SUCCESS);
  for (uint32_t i = 0; i < m2ap_message_len; i++) {
    TESTASSERT(tst_msg.msg[i] == out_msg.msg[i]);
  }
  printf("Test M2SetupResponse successfull\n");

  return 0;
}

int mbms_session_start_request_test()
{
  srslte::log_filter log1("M2AP");
  log1.set_level(srslte::LOG_LEVEL_DEBUG);
  log1.set_hex_limit(128);

  LIBLTE_BYTE_MSG_STRUCT                       tst_msg = {};
  LIBLTE_BYTE_MSG_STRUCT                       out_msg = {};
  std::unique_ptr<LIBLTE_M2AP_M2AP_PDU_STRUCT> m2ap_pdu(new LIBLTE_M2AP_M2AP_PDU_STRUCT);

  uint32_t m2ap_message_len = 49;
  uint8_t  m2ap_message[]   = {0x00, 0x00, 0x00, 0x2d, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00,
                            0x00, 0x02, 0x00, 0x07, 0x00, 0x00, 0xf1, 0x10, 0x00, 0x00, 0x01, 0x00, 0x06,
                            0x00, 0x03, 0x02, 0x00, 0x01, 0x00, 0x07, 0x00, 0x0e, 0x00, 0x7f, 0x00, 0x02,
                            0x01, 0x00, 0x7f, 0x00, 0x01, 0xc8, 0x00, 0x00, 0x00, 0x01};

  tst_msg.N_bytes = m2ap_message_len;
  memcpy(tst_msg.msg, m2ap_message, m2ap_message_len);

  /*M2AP MBMS Session Start Request Unpack Test*/
  LIBLTE_ERROR_ENUM err = liblte_m2ap_unpack_m2ap_pdu(&tst_msg, m2ap_pdu.get());
  TESTASSERT(err == LIBLTE_SUCCESS);
  TESTASSERT(m2ap_pdu->choice_type == LIBLTE_M2AP_M2AP_PDU_CHOICE_INITIATINGMESSAGE);

  LIBLTE_M2AP_INITIATINGMESSAGE_STRUCT* in_msg = &m2ap_pdu->choice.initiatingMessage;
  TESTASSERT(in_msg->choice_type == LIBLTE_M2AP_INITIATINGMESSAGE_CHOICE_MBMSSESSIONSTARTREQUEST);

  LIBLTE_M2AP_MESSAGE_MBMSSESSIONSTARTREQUEST_STRUCT* mbms_sess = &in_msg->choice.MbmsSessionStartRequest;

  /*ProtocolIE MCE-MBMS-M2AP-ID*/
  TESTASSERT(mbms_sess->MceMbmsM2apId.mce_mbms_m2ap_id == 0);

  /*ProtocolIE TMGI*/
  uint8_t* plmn_id = mbms_sess->Tmgi.pLMN_Identity.buffer;
  TESTASSERT((plmn_id[0] & 0x0F) == 0 && (plmn_id[0] & 0xF0) >> 4 == 0 && (plmn_id[1] & 0x0F) == 1);        // MCC
  TESTASSERT((plmn_id[1] & 0xF0) >> 4 == 0xF && (plmn_id[2] & 0x0F) == 0 && (plmn_id[2] & 0xF0) >> 4 == 1); // MNC
  TESTASSERT(mbms_sess->Tmgi.serviceID.buffer[0] == 0);
  TESTASSERT(mbms_sess->Tmgi.serviceID.buffer[1] == 0);
  TESTASSERT(mbms_sess->Tmgi.serviceID.buffer[2] == 1);

  /*Service Area*/
  TESTASSERT(mbms_sess->MbmsServiceArea.n_octets == 2);
  TESTASSERT(mbms_sess->MbmsServiceArea.buffer[0] == 0);
  TESTASSERT(mbms_sess->MbmsServiceArea.buffer[1] == 1);

  /* TNL Information */
  // IPMC Address
  TESTASSERT(mbms_sess->TnlInformation.iPMCAddress.len == 4);
  int32_t addr;
  memcpy(&addr, mbms_sess->TnlInformation.iPMCAddress.buffer, 4);
  TESTASSERT(ntohl(addr) == 0x7F000201);
  // Source Address
  TESTASSERT(mbms_sess->TnlInformation.iPSourceAddress.len == 4);
  memcpy(&addr, mbms_sess->TnlInformation.iPSourceAddress.buffer, 4);
  TESTASSERT(ntohl(addr) == 0x7F0001C8);

  // TEID
  int32_t teid;
  memcpy(&teid, mbms_sess->TnlInformation.gtpTeid.buffer, 4);
  TESTASSERT(ntohl(teid) == 0x00000001);

  /*M2AP Setup Request Pack Test*/
  err = liblte_m2ap_pack_m2ap_pdu(m2ap_pdu.get(), &out_msg);
  log1.info_hex(tst_msg.msg, tst_msg.N_bytes, "MBMS Session Start Request original message\n");
  log1.info_hex(out_msg.msg, out_msg.N_bytes, "MBMS Session Start Request Packed message\n");

  TESTASSERT(err == LIBLTE_SUCCESS);
  for (uint32_t i = 0; i < m2ap_message_len; i++) {
    TESTASSERT(tst_msg.msg[i] == out_msg.msg[i]);
  }
  printf("Test MBMSSessionStartRequest successfull\n");

  return 0;
}

int mbms_session_start_response_test()
{
  srslte::log_filter log1("M2AP");
  log1.set_level(srslte::LOG_LEVEL_DEBUG);
  log1.set_hex_limit(128);

  LIBLTE_BYTE_MSG_STRUCT                       tst_msg = {};
  LIBLTE_BYTE_MSG_STRUCT                       out_msg = {};
  std::unique_ptr<LIBLTE_M2AP_M2AP_PDU_STRUCT> m2ap_pdu(new LIBLTE_M2AP_M2AP_PDU_STRUCT);

  uint32_t m2ap_message_len = 19;
  uint8_t  m2ap_message[]   = {
      0x20, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x00, 0x00};

  tst_msg.N_bytes = m2ap_message_len;
  memcpy(tst_msg.msg, m2ap_message, m2ap_message_len);
  log1.info_hex(tst_msg.msg, tst_msg.N_bytes, "MBMS Session Start Response original message\n");

  /*M2AP MBMS Session Start Request Unpack Test*/
  LIBLTE_ERROR_ENUM err = liblte_m2ap_unpack_m2ap_pdu(&tst_msg, m2ap_pdu.get());
  TESTASSERT(err == LIBLTE_SUCCESS);
  TESTASSERT(m2ap_pdu->choice_type == LIBLTE_M2AP_M2AP_PDU_CHOICE_SUCCESSFULOUTCOME);

  LIBLTE_M2AP_SUCCESSFULOUTCOME_STRUCT* succ_msg = &m2ap_pdu->choice.successfulOutcome;
  TESTASSERT(succ_msg->choice_type == LIBLTE_M2AP_SUCCESSFULOUTCOME_CHOICE_MBMSSESSIONSTARTRESPONSE);

  LIBLTE_M2AP_MESSAGE_MBMSSESSIONSTARTRESPONSE_STRUCT* mbms_sess = &succ_msg->choice.MbmsSessionStartResponse;

  /*ProtocolIE MCE-MBMS-M2AP-ID*/
  TESTASSERT(mbms_sess->MceMbmsM2apId.mce_mbms_m2ap_id == 0);

  /*ProtocolIE ENB-MBMS-M2AP-ID*/
  TESTASSERT(mbms_sess->EnbMbmsM2apId.enb_mbms_m2ap_id == 0);

  /*M2AP Setup Request Pack Test*/
  err = liblte_m2ap_pack_m2ap_pdu(m2ap_pdu.get(), &out_msg);
  log1.info_hex(tst_msg.msg, tst_msg.N_bytes, "MBMS Session Start Response original message\n");
  log1.info_hex(out_msg.msg, out_msg.N_bytes, "MBMS Session Start Response Packed message\n");

  TESTASSERT(err == LIBLTE_SUCCESS);
  for (uint32_t i = 0; i < m2ap_message_len; i++) {
    TESTASSERT(tst_msg.msg[i] == out_msg.msg[i]);
  }
  printf("Test MBMSSessionStartRequest successfull\n");

  return 0;
}

int mbms_scheduling_information_test()
{
  srslte::log_filter log1("M2AP");
  log1.set_level(srslte::LOG_LEVEL_DEBUG);
  log1.set_hex_limit(128);

  LIBLTE_BYTE_MSG_STRUCT                       tst_msg = {};
  LIBLTE_BYTE_MSG_STRUCT                       out_msg = {};
  std::unique_ptr<LIBLTE_M2AP_M2AP_PDU_STRUCT> m2ap_pdu(new LIBLTE_M2AP_M2AP_PDU_STRUCT);

  uint32_t m2ap_message_len = 62;
  uint8_t  m2ap_message[]   = {0x00, 0x02, 0x00, 0x3a, 0x00, 0x00, 0x02, 0x00, 0x19, 0x00, 0x01, 0x00, 0x00,
                            0x0a, 0x00, 0x2e, 0x00, 0x00, 0x04, 0x00, 0x0b, 0x00, 0x12, 0x10, 0x00, 0x0c,
                            0x00, 0x0d, 0x00, 0x00, 0x3f, 0x13, 0x00, 0x00, 0x00, 0xf1, 0x10, 0x00, 0x00,
                            0x01, 0x08, 0x00, 0x16, 0x00, 0x07, 0x00, 0x00, 0x17, 0x00, 0x02, 0x00, 0x40,
                            0x00, 0x18, 0x00, 0x01, 0x80, 0x00, 0x1d, 0x00, 0x01, 0x01};

  tst_msg.N_bytes = m2ap_message_len;
  memcpy(tst_msg.msg, m2ap_message, m2ap_message_len);
  log1.info_hex(tst_msg.msg, tst_msg.N_bytes, "MBMS Scheduling Information message\n");

  /*M2AP MBMS Scheduling Information Unpack Test*/
  LIBLTE_ERROR_ENUM err = liblte_m2ap_unpack_m2ap_pdu(&tst_msg, m2ap_pdu.get());
  TESTASSERT(err == LIBLTE_SUCCESS);
  TESTASSERT(m2ap_pdu->choice_type == LIBLTE_M2AP_M2AP_PDU_CHOICE_INITIATINGMESSAGE);

  LIBLTE_M2AP_INITIATINGMESSAGE_STRUCT* in_msg = &m2ap_pdu->choice.initiatingMessage;
  TESTASSERT(in_msg->choice_type == LIBLTE_M2AP_INITIATINGMESSAGE_CHOICE_MBMSSCHEDULINGINFORMATION);

  LIBLTE_M2AP_MESSAGE_MBMSSCHEDULINGINFORMATION_STRUCT* sched_info = &in_msg->choice.MbmsSchedulingInformation;

  /*ProtocolIE MCCH-Update-Time*/
  TESTASSERT(sched_info->MCCHUpdateTime.mcchUpdateTime == 0);

  /*ProtocolIE MBSFN-Area-Configuration-Item*/
  TESTASSERT(sched_info->MbsfnAreaConfigurationList.len == 1);
  LIBLTE_M2AP_MBSFN_AREA_CONFIGURATION_ITEM_STRUCT* area_conf = &sched_info->MbsfnAreaConfigurationList.buffer[0];

  // PMCH Configuration List/Item
  TESTASSERT(area_conf->PMCHConfigurationList.len == 1);
  LIBLTE_M2AP_PMCH_CONFIGURATION_ITEM_STRUCT* pmch_conf_item = &area_conf->PMCHConfigurationList.buffer[0];

  // PMCH Configuration
  LIBLTE_M2AP_PMCH_CONFIGURATION_STRUCT* pmch_conf = &area_conf->PMCHConfigurationList.buffer[0].PMCHConfiguration;

  // Allocated Subframes End
  TESTASSERT(pmch_conf->allocatedSubframesEnd.allocated_subframes_end == 63);

  // Data MCS
  TESTASSERT(pmch_conf->dataMCS.dataMCS == 2);

  // MCH Scheduling Period
  TESTASSERT(pmch_conf->mchSchedulingPeriod.e == LIBLTE_M2AP_MCH_SCHEDULING_PERIOD_RF64);

  // MBMS Session List Per PMCH Item
  TESTASSERT(pmch_conf_item->MBMSSessionListPerPMCHItem.len == 1);
  LIBLTE_M2AP_MBMS_SESSION_LIST_PER_PMCH_ITEM_STRUCT* mbms_sess = &pmch_conf_item->MBMSSessionListPerPMCHItem;

  // TMGI
  uint8_t* plmn_id = mbms_sess->buffer[0].Tmgi.pLMN_Identity.buffer;
  TESTASSERT((plmn_id[0] & 0x0F) == 0 && (plmn_id[0] & 0xF0) >> 4 == 0 && (plmn_id[1] & 0x0F) == 1);        // MCC
  TESTASSERT((plmn_id[1] & 0xF0) >> 4 == 0xF && (plmn_id[2] & 0x0F) == 0 && (plmn_id[2] & 0xF0) >> 4 == 1); // MNC
  TESTASSERT(mbms_sess->buffer[0].Tmgi.serviceID.buffer[0] == 0);
  TESTASSERT(mbms_sess->buffer[0].Tmgi.serviceID.buffer[1] == 0);
  TESTASSERT(mbms_sess->buffer[0].Tmgi.serviceID.buffer[2] == 1);

  // LCID
  TESTASSERT(mbms_sess->buffer[0].Lcid.lcid == 1);

  /*MBSFN Subframe Configuration List*/
  TESTASSERT(area_conf->MBSFNSubframeConfigurationList.len == 1);
  LIBLTE_M2AP_MBSFN_SUBFRAME_CONFIGURATION_STRUCT* sub_conf = &area_conf->MBSFNSubframeConfigurationList.buffer[0];

  /*MBSFN Subframe configuration*/
  // Radioframe Allocation period
  TESTASSERT(sub_conf->radioFrameAllocationPeriod.e == LIBLTE_M2AP_RADIOFRAME_ALLOCATION_PERIOD_N1);
  // Radioframe Allocation Offset
  TESTASSERT(sub_conf->radioFrameAllocationOffset.radioframeAllocationOffset == 0);
  // Subframe Allocation
  TESTASSERT(sub_conf->subframeAllocation.choice_type == LIBLTE_M2AP_SUBFRAME_ALLOCATION_ONE_FRAME);
  TESTASSERT(sub_conf->subframeAllocation.choice.oneFrame[0] == 1);
  TESTASSERT(sub_conf->subframeAllocation.choice.oneFrame[1] == 0);
  TESTASSERT(sub_conf->subframeAllocation.choice.oneFrame[2] == 0);
  TESTASSERT(sub_conf->subframeAllocation.choice.oneFrame[3] == 0);
  TESTASSERT(sub_conf->subframeAllocation.choice.oneFrame[4] == 0);
  TESTASSERT(sub_conf->subframeAllocation.choice.oneFrame[5] == 0);

  /* Common Subframe Allocation Period*/
  TESTASSERT(area_conf->CommonSubframeAllocationPeriod.e == LIBLTE_M2AP_COMMON_SUBFRAME_ALLOCATION_PERIOD_RF64);

  /*MBSFN Area Id*/
  TESTASSERT(area_conf->MBSFNAreaId.mbsfn_area_id == 1);

  /*M2AP Setup Request Pack Test*/
  err = liblte_m2ap_pack_m2ap_pdu(m2ap_pdu.get(), &out_msg);
  log1.info_hex(out_msg.msg, out_msg.N_bytes, "MBMS Scheduling Information message\n");

  TESTASSERT(err == LIBLTE_SUCCESS);
  for (uint32_t i = 0; i < m2ap_message_len; i++) {
    TESTASSERT(tst_msg.msg[i] == out_msg.msg[i]);
  }
  printf("Test MBMS Scheduling Information successfull\n");

  return 0;
}

int mbms_scheduling_information_response_test()
{
  srslte::log_filter log1("M2AP");
  log1.set_level(srslte::LOG_LEVEL_DEBUG);
  log1.set_hex_limit(128);

  LIBLTE_BYTE_MSG_STRUCT                       tst_msg = {};
  LIBLTE_BYTE_MSG_STRUCT                       out_msg = {};
  std::unique_ptr<LIBLTE_M2AP_M2AP_PDU_STRUCT> m2ap_pdu(new LIBLTE_M2AP_M2AP_PDU_STRUCT);

  uint32_t m2ap_message_len = 7;
  uint8_t  m2ap_message[]   = {0x20, 0x02, 0x00, 0x03, 0x00, 0x00, 0x00};
  tst_msg.N_bytes           = m2ap_message_len;
  memcpy(tst_msg.msg, m2ap_message, m2ap_message_len);
  log1.info_hex(tst_msg.msg, tst_msg.N_bytes, "MBMS Scheduling Information Response message\n");

  /*M2AP MBMS Scheduling Information Unpack Test*/
  LIBLTE_ERROR_ENUM err = liblte_m2ap_unpack_m2ap_pdu(&tst_msg, m2ap_pdu.get());
  TESTASSERT(err == LIBLTE_SUCCESS);
  TESTASSERT(m2ap_pdu->choice_type == LIBLTE_M2AP_M2AP_PDU_CHOICE_SUCCESSFULOUTCOME);

  LIBLTE_M2AP_SUCCESSFULOUTCOME_STRUCT* succ_out = &m2ap_pdu->choice.successfulOutcome;
  TESTASSERT(succ_out->choice_type == LIBLTE_M2AP_SUCCESSFULOUTCOME_CHOICE_MBMSSCHEDULINGINFORMATIONRESPONSE);

  LIBLTE_M2AP_MESSAGE_MBMSSCHEDULINGINFORMATIONRESPONSE_STRUCT* sched_info =
      &succ_out->choice.MbmsSchedulingInformationResponse;

  /*M2AP Setup Request Pack Test*/
  err = liblte_m2ap_pack_m2ap_pdu(m2ap_pdu.get(), &out_msg);
  log1.info_hex(out_msg.msg, out_msg.N_bytes, "MBMS Scheduling Information message\n");

  TESTASSERT(err == LIBLTE_SUCCESS);
  for (uint32_t i = 0; i < m2ap_message_len; i++) {
    TESTASSERT(tst_msg.msg[i] == out_msg.msg[i]);
  }
  printf("Test MBMS Scheduling Information successfull\n");

  return 0;
}

int main(int argc, char** argv)
{
  if (m2_setup_request_test()) {
    return -1;
  }

  if (m2_setup_response_test()) {
    return -1;
  }

  if (mbms_session_start_request_test()) {
    return -1;
  }

  if (mbms_session_start_response_test()) {
    return -1;
  }

  if (mbms_scheduling_information_test()) {
    return -1;
  }

  if (mbms_scheduling_information_response_test()) {
    return -1;
  }
}
