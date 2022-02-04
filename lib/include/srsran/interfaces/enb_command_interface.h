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

#ifndef SRSRAN_ENB_COMMAND_INTERFACE_H
#define SRSRAN_ENB_COMMAND_INTERFACE_H

#include <cstdint>

namespace srsenb {
class enb_command_interface
{
public:
  /**
   * Trigger downlink singnal measurements (currently PAPR)
   */
  virtual void cmd_cell_measure() = 0;

  /**
   * Sets the relative gain of a cell from it's index (following rr.conf) order.
   * @param cell_id Provides a cell identifier
   * @param gain Relative gain
   */
  virtual void cmd_cell_gain(uint32_t cell_id, float gain) = 0;

  virtual void toggle_padding() = 0;
};
} // namespace srsenb

#endif // SRSRAN_ENB_COMMAND_INTERFACE_H
