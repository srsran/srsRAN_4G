/**
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSRAN_PHY_COMMON_INTERFACE_H
#define SRSRAN_PHY_COMMON_INTERFACE_H

#include "../radio/rf_buffer.h"
#include "../radio/rf_timestamp.h"

namespace srsran {

class phy_common_interface
{
public:
  /**
   * @brief Common PHY interface for workers to indicate they ended
   * @param h Worker pointer used as unique identifier for synchronising Tx
   * @param tx_enable Indicates whether the buffer has baseband samples to transmit
   * @param buffer Baseband buffer
   * @param tx_time Transmit timestamp
   * @param is_nr Indicates whether the worker is NR or not
   */
  virtual void
  worker_end(void* h, bool tx_enable, srsran::rf_buffer_t& buffer, srsran::rf_timestamp_t& tx_time, bool is_nr) = 0;
};

} // namespace srsran

#endif // SRSRAN_PHY_COMMON_INTERFACE_H
