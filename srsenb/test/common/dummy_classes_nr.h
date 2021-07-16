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

#ifndef SRSRAN_DUMMY_NR_CLASSES_H
#define SRSRAN_DUMMY_NR_CLASSES_H

#include "srsran/interfaces/gnb_interfaces.h"

namespace srsenb {

class mac_dummy : public mac_interface_rrc_nr
{
public:
  int cell_cfg(srsenb::sched_interface::cell_cfg_t* cell_cfg_)
  {
    cellcfgobj = *cell_cfg_;
    return SRSRAN_SUCCESS;
  }
  uint16_t reserve_rnti() { return 0x4601; }

  srsenb::sched_interface::cell_cfg_t cellcfgobj;
};

} // namespace srsenb

#endif // SRSRAN_DUMMY_NR_CLASSES_H
