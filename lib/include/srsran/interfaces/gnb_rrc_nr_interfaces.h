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
#ifndef SRSRAN_GNB_RRC_NR_INTERFACES_H
#define SRSRAN_GNB_RRC_NR_INTERFACES_H

#include "srsenb/hdr/phy/phy_interfaces.h"
#include "srsran/asn1/ngap.h"
#include "srsran/common/byte_buffer.h"

namespace srsenb {

class rrc_interface_ngap_nr
{
public:
  virtual int ue_set_security_cfg_key(uint16_t rnti, const asn1::fixed_bitstring<256, false, true>& key)            = 0;
  virtual int ue_set_bitrates(uint16_t rnti, const asn1::ngap_nr::ue_aggregate_maximum_bit_rate_s& rates)           = 0;
  virtual int set_aggregate_max_bitrate(uint16_t rnti, const asn1::ngap_nr::ue_aggregate_maximum_bit_rate_s& rates) = 0;
  virtual int ue_set_security_cfg_capabilities(uint16_t rnti, const asn1::ngap_nr::ue_security_cap_s& caps)         = 0;
  virtual int start_security_mode_procedure(uint16_t rnti)                                                          = 0;
  virtual int
  establish_rrc_bearer(uint16_t rnti, uint16_t pdu_session_id, srsran::const_byte_span nas_pdu, uint32_t lcid) = 0;
  virtual int  allocate_lcid(uint16_t rnti)                                                                    = 0;
  virtual int  release_bearers(uint16_t rnti)                                                                  = 0;
  virtual void write_dl_info(uint16_t rnti, srsran::unique_byte_buffer_t sdu)                                  = 0;
};

// Cell/Sector configuration for NR cells
struct rrc_cell_cfg_nr_t {
  phy_cell_cfg_nr_t    phy_cell; // already contains all PHY-related parameters (i.e. RF port, PCI, etc.)
  uint32_t             tac;      // Tracking area code
  uint32_t             dl_arfcn; // DL freq already included in phy_cell
  uint32_t             ul_arfcn; // UL freq also in phy_cell
  uint32_t             dl_absolute_freq_point_a; // derived from DL ARFCN
  uint32_t             ul_absolute_freq_point_a; // derived from UL ARFCN
  uint32_t             ssb_absolute_freq_point;  // derived from DL ARFCN
  uint32_t             band;
  srsran_duplex_mode_t duplex_mode;
  srsran_ssb_cfg_t     ssb_cfg;
};

typedef std::vector<rrc_cell_cfg_nr_t> rrc_cell_list_nr_t;

} // namespace srsenb

#endif // SRSRAN_GNB_RRC_NR_INTERFACES_H