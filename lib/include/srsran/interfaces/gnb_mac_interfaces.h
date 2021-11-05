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

#ifndef SRSRAN_GNB_MAC_INTERFACES_H
#define SRSRAN_GNB_MAC_INTERFACES_H

#include "srsgnb/hdr/stack/mac/sched_nr_interface.h"

namespace srsenb {

class mac_interface_rrc_nr
{
public:
  // Provides cell configuration including SIB periodicity, etc.
  virtual int cell_cfg(const std::vector<srsenb::sched_nr_interface::cell_cfg_t>& nr_cells) = 0;

  /// Allocates a new user/RNTI at MAC. Returns RNTI on success or SRSRAN_INVALID_RNTI otherwise.
  virtual uint16_t reserve_rnti(uint32_t enb_cc_idx, const sched_nr_interface::ue_cfg_t& uecfg) = 0;

  virtual int ue_cfg(uint16_t rnti, const sched_nr_interface::ue_cfg_t& ue_cfg) = 0;

  virtual int remove_ue(uint16_t rnti) = 0;
};

// NR interface is identical to EUTRA interface
class mac_interface_rlc_nr : public mac_interface_rlc
{};

} // namespace srsenb

#endif // SRSRAN_GNB_MAC_INTERFACES_H
