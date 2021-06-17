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

#ifndef SRSRAN_PHY_COMMON_INTERFACE_H
#define SRSRAN_PHY_COMMON_INTERFACE_H

#include "../radio/rf_buffer.h"
#include "../radio/rf_timestamp.h"

namespace srsran {

class phy_common_interface
{
public:
  virtual void worker_end(void* h, srsran::rf_buffer_t& buffer, srsran::rf_timestamp_t& tx_time, bool is_nr) = 0;
};

} // namespace srsran

#endif // SRSRAN_PHY_COMMON_INTERFACE_H
