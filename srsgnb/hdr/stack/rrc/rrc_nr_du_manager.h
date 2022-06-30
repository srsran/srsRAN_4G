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

#ifndef SRSRAN_RRC_NR_DU_MANAGER_H
#define SRSRAN_RRC_NR_DU_MANAGER_H

#include "rrc_nr_config.h"
#include "srsgnb/hdr/stack/mac/sched_nr_interface.h"
#include "srsran/asn1/rrc_nr.h"

namespace srsenb {

class du_cell_config
{
public:
  uint32_t cc;
  uint32_t pci;

  asn1::rrc_nr::mib_s          mib;
  srsran::unique_byte_buffer_t packed_mib;

  asn1::rrc_nr::sib1_s         sib1;
  srsran::unique_byte_buffer_t packed_sib1;

  asn1::rrc_nr::subcarrier_spacing_e ssb_scs;
  srsran_ssb_pattern_t               ssb_pattern;
  double                             ssb_center_freq_hz;
  double                             dl_freq_hz;
  bool                               is_standalone;

  const asn1::rrc_nr::serving_cell_cfg_common_sib_s& serv_cell_cfg_common() const
  {
    return sib1.serving_cell_cfg_common;
  }

  /// SI messages (index=0 for SIB1)
  srsran::const_byte_span packed_si_msg(uint32_t idx) { return srsran::make_span(packed_sib1); }
  size_t                  nof_si_msgs() const { return 1; }
};

class du_config_manager
{
public:
  explicit du_config_manager(const rrc_nr_cfg_t& cfg);
  ~du_config_manager();

  const rrc_nr_cfg_t& cfg;

  int add_cell();

  const du_cell_config& cell(uint32_t cc) const
  {
    srsran_assert(cc < cells.size(), "Unknown DU Cell Index=%d", cc);
    return *cells[cc];
  }

private:
  srslog::basic_logger& logger;

  std::vector<std::unique_ptr<du_cell_config> > cells;
};

void fill_phy_pdcch_cfg_common(const du_cell_config& cell, srsran_pdcch_cfg_nr_t* pdcch);

} // namespace srsenb

#endif // SRSRAN_RRC_NR_DU_MANAGER_H
