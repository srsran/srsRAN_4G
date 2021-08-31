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

#ifndef SRSUE_CELL_SEARCH_H
#define SRSUE_CELL_SEARCH_H

#include "srsran/interfaces/radio_interfaces.h"
#include "srsran/interfaces/ue_nr_interfaces.h"
#include "srsran/srsran.h"

namespace srsue {
namespace nr {
class cell_search
{
public:
  struct args_t {
    double                      max_srate_hz;
    srsran_subcarrier_spacing_t ssb_min_scs = srsran_subcarrier_spacing_15kHz;
  };

  struct cfg_t {
    double                      srate_hz;
    double                      center_freq_hz;
    double                      ssb_freq_hz;
    srsran_subcarrier_spacing_t ssb_scs;
    srsran_ssb_patern_t         ssb_pattern;
    srsran_duplex_mode_t        duplex_mode;
  };

  cell_search(stack_interface_phy_sa_nr& stack_, srsran::radio_interface_phy& radio_);

  ~cell_search();

  bool init(const args_t& args);

  bool start(const cfg_t& cfg);

  bool run();

  void reset();

private:
  srslog::basic_logger&        logger;
  stack_interface_phy_sa_nr&   stack;
  srsran::radio_interface_phy& radio;
  srsran_ssb_t                 ssb    = {};
  uint32_t                     sf_sz  = 0;       ///< subframe size in samples (1 ms)
  cf_t*                        buffer = nullptr; ///< Receive buffer
};
} // namespace nr
} // namespace srsue

#endif // SRSUE_CELL_SEARCH_H
