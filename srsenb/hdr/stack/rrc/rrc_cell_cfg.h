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

#ifndef SRSLTE_RRC_CELL_CFG_H
#define SRSLTE_RRC_CELL_CFG_H

#include "rrc_config.h"

namespace srsenb {

struct cell_ctxt_common {
  uint32_t                           enb_cc_idx = 0;
  asn1::rrc::mib_s                   mib;
  asn1::rrc::sib_type1_s             sib1;
  asn1::rrc::sib_type2_s             sib2;
  const cell_cfg_t&                  cell_cfg;
  std::vector<std::vector<uint8_t> > sib_buffer; ///< Packed SIBs for given CC

  cell_ctxt_common(uint32_t idx_, const cell_cfg_t& cfg) : enb_cc_idx(idx_), cell_cfg(cfg) {}
};

class cell_ctxt_common_list
{
public:
  explicit cell_ctxt_common_list(const rrc_cfg_t& cfg_);

  cell_ctxt_common* get_cc_idx(uint32_t enb_cc_idx) { return &cell_list[enb_cc_idx]; }
  cell_ctxt_common* get_cell_id(uint32_t cell_id);
  cell_ctxt_common* get_pci(uint32_t pci);
  size_t            nof_cells() const { return cell_list.size(); }

private:
  const rrc_cfg_t&              cfg;
  std::vector<cell_ctxt_common> cell_list;
};

} // namespace srsenb

#endif // SRSLTE_RRC_CELL_CFG_H
