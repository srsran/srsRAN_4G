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

/******************************************************************************
 * File:        metrics_stdout.h
 * Description: Metrics class printing to stdout.
 *****************************************************************************/

#ifndef SRSUE_METRICS_STDOUT_H
#define SRSUE_METRICS_STDOUT_H

#include <pthread.h>
#include <stdint.h>
#include <string>

#include "srsran/common/metrics_hub.h"
#include "ue_metrics_interface.h"

namespace srsue {

class metrics_stdout : public srsran::metrics_listener<ue_metrics_t>
{
public:
  metrics_stdout(){};

  void toggle_print(bool b);
  void set_metrics(const ue_metrics_t& m, const uint32_t period_usec);
  void set_ue_handle(ue_metrics_interface* ue_);
  void stop(){};

private:
  static const bool FORCE_NEIGHBOUR_CELL = false; // Set to true for printing always neighbour cells
  void              set_metrics_helper(const phy_metrics_t& phy,
                                       const mac_metrics_t  mac[SRSRAN_MAX_CARRIERS],
                                       const rrc_metrics_t& rrc,
                                       bool                 display_neighbours,
                                       const uint32_t       r,
                                       bool                 is_carrier_nr,
                                       bool                 print_carrier_num);
  std::string       float_to_string(float f, int digits);
  std::string       float_to_eng_string(float f, int digits);
  void              print_table(const bool display_neighbours, const bool is_nr);

  std::atomic<bool>     do_print             = {false};
  bool                  table_has_neighbours = false; ///< state of last table head
  uint8_t               n_reports            = 10;
  ue_metrics_interface* ue                   = nullptr;
  std::mutex            mutex;
};

} // namespace srsue

#endif // SRSUE_METRICS_STDOUT_H
