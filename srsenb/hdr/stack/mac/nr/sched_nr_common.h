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

#ifndef SRSRAN_SCHED_NR_COMMON_H
#define SRSRAN_SCHED_NR_COMMON_H

#include "sched_nr_interface.h"
#include "srsran/adt/bounded_bitset.h"

namespace srsenb {

const static size_t SCHED_NR_MAX_USERS     = 4;
const static size_t SCHED_NR_NOF_SUBFRAMES = 10;
const static size_t SCHED_NR_NOF_HARQS     = 16;

namespace sched_nr_impl {

using sched_cfg_t = sched_nr_interface::sched_cfg_t;
using cell_cfg_t  = sched_nr_interface::cell_cfg_t;

struct sched_cell_params {
  const uint32_t     cc;
  const cell_cfg_t   cell_cfg;
  const sched_cfg_t& sched_cfg;

  sched_cell_params(uint32_t cc_, const cell_cfg_t& cell, const sched_cfg_t& sched_cfg_) :
    cc(cc_), cell_cfg(cell), sched_cfg(sched_cfg_)
  {}
};

struct sched_params {
  const sched_cfg_t              sched_cfg;
  std::vector<sched_cell_params> cells;

  explicit sched_params(const sched_cfg_t& sched_cfg_) : sched_cfg(sched_cfg_) {}
};

using pdcchmask_t = srsran::bounded_bitset<SCHED_NR_MAX_NOF_RBGS, true>;
using rbgmask_t   = srsran::bounded_bitset<SCHED_NR_MAX_NOF_RBGS, true>;

struct resource_guard {
public:
  resource_guard()                            = default;
  resource_guard(const resource_guard& other) = delete;
  resource_guard(resource_guard&& other)      = delete;
  resource_guard& operator=(const resource_guard& other) = delete;
  resource_guard& operator=(resource_guard&& other) = delete;
  bool            busy() const { return flag; }

  struct token {
    token() = default;
    token(resource_guard& parent) : flag(parent.busy() ? nullptr : &parent.flag)
    {
      if (flag != nullptr) {
        *flag = true;
      }
    }
    token(token&&) noexcept = default;
    token& operator=(token&&) noexcept = default;
    void   release() { flag.reset(); }
    bool   owns_token() const { return flag != nullptr; }
    bool   empty() const { return flag == nullptr; }

  private:
    struct release_deleter {
      void operator()(bool* ptr)
      {
        if (ptr != nullptr) {
          srsran_assert(*ptr == true, "resource token: detected inconsistency token state");
          *ptr = false;
        }
      }
    };
    std::unique_ptr<bool, release_deleter> flag;
  };

private:
  bool flag = false;
};

} // namespace sched_nr_impl

} // namespace srsenb

#endif // SRSRAN_SCHED_NR_COMMON_H
