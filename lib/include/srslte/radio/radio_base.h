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

/**
 * @file radio_base.h
 * @brief Base class for all eNB/UE radios.
 */

#ifndef SRSLTE_RADIO_BASE_H
#define SRSLTE_RADIO_BASE_H

#include "srslte/common/interfaces_common.h"
#include "srslte/common/logger.h"
#include "srslte/radio/radio_metrics.h"

namespace srslte {

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

} // namespace srslte

#endif // SRSLTE_RADIO_BASE_H
