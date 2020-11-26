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

#ifndef SRSLTE_ENB_COMMAND_INTERFACE_H
#define SRSLTE_ENB_COMMAND_INTERFACE_H

#include <cstdint>

namespace srsenb {
class enb_command_interface
{
public:
  /**
   * Sets the relative gain of a cell from it's index (following rr.conf) order.
   * @param cell_id Provides a cell identifier
   * @param gain Relative gain
   */
  virtual void cmd_cell_gain(uint32_t cell_id, float gain) = 0;
};
} // namespace srsenb

#endif // SRSLTE_ENB_COMMAND_INTERFACE_H
