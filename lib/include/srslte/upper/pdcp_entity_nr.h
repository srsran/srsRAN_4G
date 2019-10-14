/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

#ifndef SRSLTE_PDCP_ENTITY_NR_H
#define SRSLTE_PDCP_ENTITY_NR_H

#include "pdcp_entity_base.h"
#include "srslte/common/buffer_pool.h"
#include "srslte/common/common.h"
#include "srslte/common/interfaces_common.h"
#include "srslte/common/log.h"
#include "srslte/common/security.h"
#include "srslte/common/threads.h"

namespace srslte {

/****************************************************************************
 * NR PDCP Entity
 * PDCP entity for 5G NR
 ***************************************************************************/
class pdcp_entity_nr : public pdcp_entity_base
{
public:
  pdcp_entity_nr();
  ~pdcp_entity_nr();
  void init(srsue::rlc_interface_pdcp* rlc_,
            srsue::rrc_interface_pdcp* rrc_,
            srsue::gw_interface_pdcp*  gw_,
            srslte::log*               log_,
            uint32_t                   lcid_,
            pdcp_config_t              cfg_);
  void reset();
  void reestablish();

  // RRC interface
  void write_sdu(unique_byte_buffer_t sdu, bool blocking);

  uint32_t get_dl_count();
  uint32_t get_ul_count();

  // RLC interface
  void write_pdu(unique_byte_buffer_t pdu);

private:
  srsue::rlc_interface_pdcp* rlc = nullptr;
  srsue::rrc_interface_pdcp* rrc = nullptr;
  srsue::gw_interface_pdcp*  gw  = nullptr;

  // State variables: 3GPP TS 38.323 v15.2.0, section 7.1
  uint32_t tx_next  = 0; // COUNT value of next SDU to be transmitted.
  uint32_t rx_next  = 0; // COUNT value of next SDU expected to be received.
  uint32_t rx_deliv = 0; // COUNT value of first SDU not delivered to upper layers, but still waited for.
  uint32_t rx_reord = 0; // COUNT value following the COUNT value of PDCP Data PDU which triggered t-Reordering.

  // Constants: 3GPP TS 38.323 v15.2.0, section 7.2
  uint32_t window_size = 0;

  // Packing/Unpacking Helper functions
  uint32_t read_data_header(const unique_byte_buffer_t& sdu);
  void     write_data_header(const unique_byte_buffer_t& sdu, uint32_t sn);
  void     extract_mac(const unique_byte_buffer_t& sdu, uint8_t* mac);
  void     append_mac(const unique_byte_buffer_t& sdu, uint8_t* mac);
};

} // namespace srslte
#endif // SRSLTE_PDCP_ENTITY_NR_H
