/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSRAN_RLC_AM_LTE_PACKING_H
#define SRSRAN_RLC_AM_LTE_PACKING_H

#include "srsran/common/string_helpers.h"
#include "srsran/rlc/rlc_am_base.h"
#include "srsran/rlc/rlc_am_data_structs.h" // required for rlc_am_pdu_segment
#include <list>

namespace srsran {

struct rlc_sn_info_t {
  uint32_t sn;
  bool     is_acked;
};

struct rlc_amd_rx_pdu {
  rlc_amd_pdu_header_t header;
  unique_byte_buffer_t buf;
  uint32_t             rlc_sn;

  rlc_amd_rx_pdu() = default;
  explicit rlc_amd_rx_pdu(uint32_t rlc_sn_) : rlc_sn(rlc_sn_) {}
};

struct rlc_amd_rx_pdu_segments_t {
  std::list<rlc_amd_rx_pdu> segments;
};

/****************************************************************************
 * Header pack/unpack helper functions
 * Ref: 3GPP TS 36.322 v10.0.0 Section 6.2.1
 ***************************************************************************/
void rlc_am_read_data_pdu_header(byte_buffer_t* pdu, rlc_amd_pdu_header_t* header);
void rlc_am_read_data_pdu_header(uint8_t** payload, uint32_t* nof_bytes, rlc_amd_pdu_header_t* header);
void rlc_am_write_data_pdu_header(rlc_amd_pdu_header_t* header, byte_buffer_t* pdu);
void rlc_am_write_data_pdu_header(rlc_amd_pdu_header_t* header, uint8_t** payload);
void rlc_am_read_status_pdu(byte_buffer_t* pdu, rlc_status_pdu_t* status);
void rlc_am_read_status_pdu(uint8_t* payload, uint32_t nof_bytes, rlc_status_pdu_t* status);
void rlc_am_write_status_pdu(rlc_status_pdu_t* status, byte_buffer_t* pdu);
int  rlc_am_write_status_pdu(rlc_status_pdu_t* status, uint8_t* payload);

uint32_t rlc_am_packed_length(rlc_amd_pdu_header_t* header);
uint32_t rlc_am_packed_length(rlc_status_pdu_t* status);
uint32_t rlc_am_packed_length(rlc_amd_retx_t retx);
bool     rlc_am_is_pdu_segment(uint8_t* payload);
bool     rlc_am_is_valid_status_pdu(const rlc_status_pdu_t& status, uint32_t rx_win_min = 0);
bool     rlc_am_start_aligned(const uint8_t fi);
bool     rlc_am_end_aligned(const uint8_t fi);
bool     rlc_am_is_unaligned(const uint8_t fi);
bool     rlc_am_not_start_aligned(const uint8_t fi);
std::string
rlc_am_undelivered_sdu_info_to_string(const std::map<uint32_t, pdcp_pdu_info<rlc_amd_pdu_header_t> >& info_queue);
const char* rlc_amd_pdu_header_to_string(const rlc_amd_pdu_header_t& header);
const char* rlc_am_status_pdu_to_string(rlc_status_pdu_t* status);
} // namespace srsran

#endif // SRSRAN_RLC_AM_LTE_PACKING_H
