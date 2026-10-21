// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

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
