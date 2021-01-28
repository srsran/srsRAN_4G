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

#ifndef SRSUE_PRACH_H
#define SRSUE_PRACH_H

#include "srslte/common/log.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/radio/radio.h"
#include "srslte/srslog/srslog.h"
#include "srslte/srslte.h"
#include <bitset>

namespace srsue {

class prach
{
public:
  prach(srslog::basic_logger& logger) : logger(logger) {}
  ~prach() { stop(); }

  void  init(uint32_t max_prb);
  void  stop();
  bool  set_cell(srslte_cell_t cell, srslte_prach_cfg_t prach_cfg);
  bool  prepare_to_send(uint32_t preamble_idx, int allowed_subframe = -1, float target_power_dbm = -1);
  bool  is_ready_to_send(uint32_t current_tti, uint32_t current_pci);
  bool  is_pending() const;
  cf_t* generate(float cfo, uint32_t* nof_sf, float* target_power = NULL);

  phy_interface_mac_lte::prach_info_t get_info() const;

private:
  bool generate_buffer(uint32_t f_idx);

  bool is_buffer_generated(uint32_t f_idx, uint32_t preamble_index) const
  {
    return buffer_bitmask.test(f_idx * 64 + preamble_index);
  }

  void set_buffer_as_generated(uint32_t f_idx, uint32_t preamble_index)
  {
    buffer_bitmask.set(f_idx * 64 + preamble_index);
  }

private:
  static constexpr unsigned MAX_LEN_SF    = 3;
  static constexpr unsigned max_fs        = 12;
  static constexpr unsigned max_preambles = 64;

  srslog::basic_logger&                                logger;
  srslte_prach_t                                       prach_obj        = {};
  srslte_cell_t                                        cell             = {};
  srslte_cfo_t                                         cfo_h            = {};
  srslte_prach_cfg_t                                   cfg              = {};
  std::array<std::array<cf_t*, max_preambles>, max_fs> buffer           = {};
  cf_t*                                                signal_buffer    = nullptr;
  int                                                  preamble_idx     = -1;
  uint32_t                                             len              = 0;
  int                                                  allowed_subframe = 0;
  int                                                  transmitted_tti  = 0;
  float                                                target_power_dbm = 0;
  bool                                                 mem_initiated    = false;
  bool                                                 cell_initiated   = false;
  std::bitset<max_fs * max_preambles>                  buffer_bitmask;
};

} // namespace srsue

#endif // SRSUE_PRACH_H
