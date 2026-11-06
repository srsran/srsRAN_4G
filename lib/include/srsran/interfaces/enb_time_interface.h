// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef SRSRAN_ENB_TIME_INTERFACE_H
#define SRSRAN_ENB_TIME_INTERFACE_H

namespace srsenb {

// RAT-agnostic interface to provide timing information to upper layers
class enb_time_interface
{
public:
  /**
   * @brief Called for every tick (currently every ms)
   *
   */
  virtual void tti_clock() = 0;
};

} // namespace srsenb

#endif // SRSRAN_ENB_TIME_INTERFACE_H
