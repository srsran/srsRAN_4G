/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#ifndef SRSRAN_RLC_TEST_COMMON_H
#define SRSRAN_RLC_TEST_COMMON_H

#include "srsran/common/byte_buffer.h"
#include "srsran/interfaces/ue_pdcp_interfaces.h"
#include "srsran/interfaces/ue_rrc_interfaces.h"
#include <vector>

namespace srsran {

class rlc_um_tester : public srsue::pdcp_interface_rlc, public srsue::rrc_interface_rlc
{
public:
  rlc_um_tester() {}

  // PDCP interface
  void write_pdu(uint32_t lcid, unique_byte_buffer_t sdu)
  {
    // check length
    if (lcid != 3 && sdu->N_bytes != expected_sdu_len) {
      printf("Received PDU with size %d, expected %d. Exiting.\n", sdu->N_bytes, expected_sdu_len);
      exit(-1);
    }

    // check content
    uint8_t first_byte = *sdu->msg;
    for (uint32_t i = 0; i < sdu->N_bytes; i++) {
      if (sdu->msg[i] != first_byte) {
        printf("Received corrupted SDU with size %d. Exiting.\n", sdu->N_bytes);
        srsran_vec_fprint_byte(stdout, sdu->msg, sdu->N_bytes);
        exit(-1);
      }
    }

    // srsran_vec_fprint_byte(stdout, sdu->msg, sdu->N_bytes);
    sdus.push_back(std::move(sdu));
  }
  void write_pdu_bcch_bch(unique_byte_buffer_t sdu) {}
  void write_pdu_bcch_dlsch(unique_byte_buffer_t sdu) {}
  void write_pdu_pcch(unique_byte_buffer_t sdu) {}
  void write_pdu_mch(uint32_t lcid, srsran::unique_byte_buffer_t sdu) { sdus.push_back(std::move(sdu)); }
  void notify_delivery(uint32_t lcid, const srsran::pdcp_sn_vector_t& pdcp_sns) {}
  void notify_failure(uint32_t lcid, const srsran::pdcp_sn_vector_t& pdcp_sns) {}

  // RRC interface
  void        max_retx_attempted() {}
  const char* get_rb_name(uint32_t lcid) { return ""; }
  void        set_expected_sdu_len(uint32_t len) { expected_sdu_len = len; }

  uint32_t get_num_sdus() { return sdus.size(); }

  // TODO: this should be private
  std::vector<unique_byte_buffer_t> sdus;
  uint32_t                          expected_sdu_len = 0;
};

} // namespace srsran

#endif // SRSRAN_RLC_TEST_COMMON_H
