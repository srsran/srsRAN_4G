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
    srsran_ssb_pattern_t        ssb_pattern;
    srsran_duplex_mode_t        duplex_mode;
  };

  struct ret_t {
    enum { CELL_FOUND = 1, CELL_NOT_FOUND = 0, ERROR = -1 } result;
    srsran_ssb_search_res_t ssb_res;
  };

  cell_search(srslog::basic_logger& logger);
  ~cell_search();

  bool init(const args_t& args);

  bool  start(const cfg_t& cfg);
  ret_t run_slot(const cf_t* buffer, uint32_t slot_sz);

private:
  srslog::basic_logger&        logger;
  srsran_ssb_t                 ssb    = {};
};
} // namespace nr
} // namespace srsue

#endif // SRSUE_CELL_SEARCH_H
