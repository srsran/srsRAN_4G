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

#ifndef SRSUE_PRACH_H
#define SRSUE_PRACH_H

#include "srsran/interfaces/ue_phy_interfaces.h"
#include "srsran/radio/radio.h"
#include "srsran/srslog/srslog.h"
#include "srsran/srsran.h"
#include <bitset>
#include <mutex>

namespace srsue {

class prach
{
public:
  prach(srslog::basic_logger& logger) : logger(logger) {}
  ~prach() { stop(); }

  void  init(uint32_t max_prb);
  void  stop();
  bool  set_cell(srsran_cell_t cell, srsran_prach_cfg_t prach_cfg);
  bool  prepare_to_send(uint32_t preamble_idx, int allowed_subframe = -1, float target_power_dbm = -1);
  bool  is_ready_to_send(uint32_t current_tti, uint32_t current_pci);
  bool  is_pending() const;
  cf_t* generate(float cfo, uint32_t* nof_sf, float* target_power = NULL);

  phy_interface_mac_lte::prach_info_t get_info() const;

private:
  bool generate_buffer(uint32_t f_idx);

private:
  static constexpr unsigned MAX_LEN_SF    = 3;
  static constexpr unsigned max_fs        = 12;
  static constexpr unsigned max_preambles = 64;

  srslog::basic_logger& logger;
  srsran_prach_t        prach_obj        = {};
  srsran_cell_t         cell             = {};
  srsran_cfo_t          cfo_h            = {};
  srsran_prach_cfg_t    cfg              = {};
  cf_t*                 signal_buffer    = nullptr;
  int                   preamble_idx     = -1;
  uint32_t              len              = 0;
  int                   allowed_subframe = 0;
  int                   transmitted_tti  = 0;
  float                 target_power_dbm = 0;
  bool                  mem_initiated    = false;
  bool                  cell_initiated   = false;
  mutable std::mutex    mutex;
};

} // namespace srsue

#endif // SRSUE_PRACH_H
