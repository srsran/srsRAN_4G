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

#include "srsran/common/common.h"
#include "srsran/common/interfaces_common.h"
#include "srsran/srsran.h"

#include "srsran/interfaces/enb_rrc_interfaces.h"

#ifndef SRSRAN_ENB_INTERFACES_H
#define SRSRAN_ENB_INTERFACES_H

namespace srsenb {

class stack_interface_rrc
{
public:
  virtual void add_eps_bearer(uint16_t rnti, uint8_t eps_bearer_id, srsran::srsran_rat_t rat, uint32_t lcid) = 0;
  virtual void remove_eps_bearer(uint16_t rnti, uint8_t eps_bearer_id)                                       = 0;
  virtual void remove_eps_bearers(uint16_t rnti)                                                             = 0;
};

} // namespace srsenb

#endif // SRSRAN_ENB_INTERFACES_H
