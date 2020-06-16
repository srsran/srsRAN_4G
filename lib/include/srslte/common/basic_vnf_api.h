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

#ifndef SRSLTE_BASIC_VNF_API_H
#define SRSLTE_BASIC_VNF_API_H

#include <thread>

namespace basic_vnf_api {

//  PNF (the PHY)          VNF (the L2/L3)
//          |               |
//          |               |
//          | -             |
//          |  \ sf_ind_msg_t
//          |    \          |
//          |      \        |
//          |        \      |
//          |          \    |
//          |            \  |
//          |              \|
//          |               |
//          |               | DL_CONFIG.request
//          |              /|
//          |            /  |
//          |          /    |
//          |        /      |
//          |      /        |
//          |    / dl_conf_msg_t
//          |  /            |
//          |/              | TX.request
//          |              /|
//          |            /  |
//          |          /    |
//          |        /      |
//          |      /        |
//          |    / tx_request_msg_t
//          |  /            |
//          |/              |
//          |               |

// Primitive API messages for basic testing basic VNF/PNF interaction
enum msg_type_t {
  SF_IND,      ///< To signal start of new subframe (later slot) for both UE and gNB
  DL_CONFIG,   ///< To configure the DL for gNB
  TX_REQUEST,  ///< For DL data for gNB
  RX_DATA_IND, ///< For UL Data for gNB
  DL_IND,      ///< For the UE for DL data
  UL_IND,      ///< For the UE for UL Data
  MSG_TYPE_NITEMS
};
static const char* msg_type_text[MSG_TYPE_NITEMS] =
    {"SF Indication", "DL_CONFIG.Request", "TX.Request", "RX_Data.indication", "DL_Indication", "UL_Indication"};
enum pdu_type_t { MAC_PBCH, PHY_PBCH, PDCCH, PDSCH, PUSCH };

struct msg_header_t {
  msg_type_t type;
  uint32_t   msg_len;
};

struct sf_ind_msg_t {
  msg_header_t header;
  uint32_t     t1;     // Timestamp taken at PNF
  uint32_t     tti;    // TTI of requested subframe
  uint32_t     tb_len; // Length of the TB
};

#define MAX_TB_LEN (16 * 1024)
#define MAX_PDU_SIZE (16 * 1024)
#define MAX_NUM_PDUS (1)

struct phy_pbch_pdu_t {
  uint16_t phy_cell_id;    // 0 - 1007
  uint8_t  ss_block_index; // 0-63
  uint8_t  ssb_sc_offset;  // 0-15
  uint8_t  dmrs_pos;       // 0-1
  uint8_t  pdcch_config;   // 0-255
};

struct dl_conf_msg_t {
  msg_header_t header;
  uint32_t     t1;      // Replayed timestamp
  uint32_t     t2;      // Timestamp taken at VNF
  uint32_t     tti;     // TTI
  uint16_t     beam_id; // tx beam id for the slot
};

struct tx_request_pdu_t {
  uint16_t   length;
  uint16_t   index; // index indicated in dl_config
  pdu_type_t type;  // physical chan of pdu/tb
  uint8_t    data[MAX_PDU_SIZE];
};

struct tx_request_msg_t {
  msg_header_t     header;
  uint32_t         tti;    // TTI
  uint32_t         tb_len; // actual TB len
  uint32_t         nof_pdus;
  tx_request_pdu_t pdus[MAX_NUM_PDUS];
};

struct rx_data_ind_pdu_t {
  uint16_t   length;
  pdu_type_t type; // physical chan of pdu/tb
  uint8_t    data[MAX_PDU_SIZE];
};

struct rx_data_ind_msg_t {
  msg_header_t      header;
  uint32_t          t1;       // Timestamp taken at PNF
  uint32_t          sfn;      ///< SFN (0-1023)
  uint32_t          slot;     ///< Slot (0-319)
  uint32_t          tb_len;   ///< actual TB len
  uint32_t          nof_pdus; //
  rx_data_ind_pdu_t pdus[MAX_NUM_PDUS];
};

// UE specific messages
struct dl_ind_pdu_t {
  pdu_type_t type; // physical chan of pdu/tb
  uint16_t   length;
  uint8_t    data[MAX_PDU_SIZE];
};

struct dl_ind_msg_t {
  msg_header_t header;
  uint32_t     t1;  // Timestamp taken at PNF
  uint32_t     tti; // tti or slot?
  uint32_t     nof_pdus;
  dl_ind_pdu_t pdus[MAX_NUM_PDUS];
};

///< Messages for UL (only one PDU)
struct ul_ind_pdu_t {
  pdu_type_t type; // physical chan of pdu/tb
  uint16_t   length;
};

struct ul_ind_msg_t {
  msg_header_t header;
  uint32_t     t1;   // Timestamp taken at PNF
  uint32_t     tti;  // tti or slot?
  uint32_t     rnti; ///< RNTI of this grant
  ul_ind_pdu_t pdus;
};

} // namespace basic_vnf_api

#endif // SRSLTE_BASIC_VNF_API_H
