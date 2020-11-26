/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSUE_PROC_SR_H
#define SRSUE_PROC_SR_H

#include "srslte/common/logmap.h"
#include "srslte/interfaces/ue_interfaces.h"
#include <stdint.h>

/* Scheduling Request procedure as defined in 5.4.4 of 36.321 */

namespace srsue {

// Forward-declare ra_proc
class ra_proc;

class sr_proc
{
public:
  sr_proc();
  void init(ra_proc* ra, phy_interface_mac_lte* phy_h, rrc_interface_mac* rrc, srslte::log_ref log_h);
  void step(uint32_t tti);
  void set_config(srslte::sr_cfg_t& cfg);
  void reset();
  void start();

private:
  bool need_tx(uint32_t tti);

  int  sr_counter;
  bool is_pending_sr;

  srslte::sr_cfg_t sr_cfg;

  ra_proc*               ra;
  rrc_interface_mac*     rrc;
  phy_interface_mac_lte* phy_h;
  srslte::log_ref        log_h;

  bool initiated;
};

} // namespace srsue

#endif // SRSUE_PROC_SR_H
