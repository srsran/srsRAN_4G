/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2012-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSRAN_LTE_COMMON_H
#define SRSRAN_LTE_COMMON_H

#include <array>
#include <cstdint>

namespace srsran {

// Cell nof PRBs
const std::array<uint32_t, 6> lte_cell_nof_prbs = {6, 15, 25, 50, 75, 100};
inline uint32_t               lte_nof_prb_to_idx(uint32_t nof_prb)
{
  switch (nof_prb) {
    case 6:
      return 0;
    case 15:
      return 1;
    case 25:
      return 2;
    case 50:
      return 3;
    case 75:
      return 4;
    case 100:
      return 5;
    default:
      return -1;
  }
}

// Radio Bearer
enum class lte_rb { srb0, srb1, srb2, drb1, drb2, drb3, drb4, drb5, drb6, drb7, drb8, drb9, drb10, drb11, count };
const size_t       MAX_LTE_DRB_ID = 11;
const size_t       MAX_LTE_SRB_ID = 2;
inline const char* get_rb_name(lte_rb rb_id)
{
  static const char* names[] = {"SRB0",
                                "SRB1",
                                "SRB2",
                                "DRB0",
                                "DRB1",
                                "DRB2",
                                "DRB3",
                                "DRB4",
                                "DRB5",
                                "DRB6",
                                "DRB7",
                                "DRB8",
                                "DRB9",
                                "DRB10",
                                "invalid RB id"};
  return names[rb_id < lte_rb::count ? (size_t)rb_id : (size_t)lte_rb::count];
}

} // namespace srsran

#endif // SRSRAN_LTE_COMMON_H
