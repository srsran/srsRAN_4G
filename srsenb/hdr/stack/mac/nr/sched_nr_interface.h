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

#ifndef SRSRAN_SCHED_NR_INTERFACE_H
#define SRSRAN_SCHED_NR_INTERFACE_H

#include "srsran/adt/bounded_vector.h"
#include "srsran/common/tti_point.h"

namespace srsenb {

const static size_t   SCHED_NR_MAX_CARRIERS = 4;
const static uint16_t SCHED_NR_INVALID_RNTI = 0;

struct sched_nr_cell_cfg {};

struct sched_nr_cfg {
  uint32_t nof_concurrent_subframes = 1;

  srsran::bounded_vector<sched_nr_cell_cfg, SCHED_NR_MAX_CARRIERS> cells;
};

struct sched_nr_ue_cc_cfg {
  bool active = false;
};

struct sched_nr_ue_cfg {
  srsran::bounded_vector<sched_nr_ue_cc_cfg, SCHED_NR_MAX_CARRIERS> carriers;
};

struct sched_nr_res_t {
  struct dl_result {};
  struct ul_result {};
};

class sched_nr_interface
{
public:
  virtual ~sched_nr_interface()                                     = default;
  virtual void ue_cfg(uint16_t rnti, const sched_nr_ue_cfg& ue_cfg) = 0;
  virtual void new_tti(tti_point tti_rx)                            = 0;

  virtual void dl_ack_info(tti_point tti_rx, uint16_t rnti, uint32_t cc, uint32_t tb_idx, bool ack) = 0;
  virtual void ul_sr_info(tti_point, uint16_t rnti)                                                 = 0;
};

} // namespace srsenb

#endif // SRSRAN_SCHED_NR_INTERFACE_H
