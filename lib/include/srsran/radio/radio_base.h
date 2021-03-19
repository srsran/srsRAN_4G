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

/**
 * @file radio_base.h
 * @brief Base class for all eNB/UE radios.
 */

#ifndef SRSRAN_RADIO_BASE_H
#define SRSRAN_RADIO_BASE_H

#include "srsran/common/interfaces_common.h"
#include "srsran/radio/radio_metrics.h"

namespace srsran {

class phy_interface_radio;

class radio_base
{
public:
  virtual ~radio_base() = default;

  virtual std::string get_type() = 0;

  virtual int  init(const rf_args_t& args_, phy_interface_radio* phy_) = 0;
  virtual void stop()                                                  = 0;

  virtual bool get_metrics(rf_metrics_t* metrics) = 0;
};

} // namespace srsran

#endif // SRSRAN_RADIO_BASE_H
