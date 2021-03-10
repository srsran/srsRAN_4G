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

#include "srslte/interfaces/ue_mac_interfaces.h"
#include "srslte/srslog/srslog.h"
#include <stdint.h>

/* Scheduling Request procedure as defined in 5.4.4 of 36.321 */

namespace srsue {

class ra_proc;
class phy_interface_mac_lte;
class rrc_interface_mac;

class sr_proc
{
public:
  explicit sr_proc(srslog::basic_logger& logger);
  void init(ra_proc* ra, phy_interface_mac_lte* phy_h, rrc_interface_mac* rrc);
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
  srslog::basic_logger&  logger;

  bool initiated;
};

} // namespace srsue

#endif // SRSUE_PROC_SR_H
