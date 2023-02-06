
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

#ifndef SRSRAN_COMMON_NR_H
#define SRSRAN_COMMON_NR_H

#include <array>
#include <cstdint>

namespace srsran {

// Radio Bearers
enum class nr_srb { srb0, srb1, srb2, srb3, count };
const uint32_t MAX_NR_SRB_ID = 3;
enum class nr_drb {
  drb1 = 1,
  drb2,
  drb3,
  drb4,
  drb5,
  drb6,
  drb7,
  drb8,
  drb9,
  drb10,
  drb11,
  drb12,
  drb13,
  drb14,
  drb15,
  drb16,
  drb17,
  drb18,
  drb19,
  drb20,
  drb21,
  drb22,
  drb23,
  drb24,
  drb25,
  drb26,
  drb27,
  drb28,
  drb29,
  invalid
};
const uint32_t MAX_NR_DRB_ID      = 29;
const uint32_t MAX_NR_NOF_BEARERS = MAX_NR_DRB_ID + MAX_NR_SRB_ID; // 32

constexpr bool is_nr_lcid(uint32_t lcid)
{
  return lcid < MAX_NR_NOF_BEARERS;
}
constexpr bool is_nr_srb(uint32_t srib)
{
  return srib <= MAX_NR_SRB_ID;
}
inline const char* get_srb_name(nr_srb srb_id)
{
  static const char* names[] = {"SRB0", "SRB1", "SRB2", "SRB3", "invalid SRB id"};
  return names[(uint32_t)(srb_id < nr_srb::count ? srb_id : nr_srb::count)];
}
constexpr uint32_t srb_to_lcid(nr_srb srb_id)
{
  return static_cast<uint32_t>(srb_id);
}
constexpr nr_srb nr_lcid_to_srb(uint32_t lcid)
{
  return static_cast<nr_srb>(lcid);
}
constexpr nr_drb nr_drb_id_to_drb(uint32_t drb_id)
{
  return static_cast<nr_drb>(drb_id);
}
constexpr bool is_nr_drb(uint32_t drib)
{
  return drib > MAX_NR_SRB_ID and is_nr_lcid(drib);
}
inline const char* get_drb_name(nr_drb drb_id)
{
  static const char* names[] = {"DRB1",  "DRB2",  "DRB3",  "DRB4",  "DRB5",  "DRB6",          "DRB7",  "DRB8",
                                "DRB9",  "DRB10", "DRB11", "DRB12", "DRB13", "DRB14",         "DRB15", "DRB16",
                                "DRB17", "DRB18", "DRB19", "DRB20", "DRB21", "DRB22",         "DRB23", "DRB24",
                                "DRB25", "DRB26", "DRB27", "DRB28", "DRB29", "invalid DRB id"};
  return names[(uint32_t)(drb_id < nr_drb::invalid ? drb_id : nr_drb::invalid) - 1];
}

inline const char* get_nr_rb_name(uint32_t lcid)
{
  if (is_nr_srb(lcid)) {
    return get_srb_name(static_cast<nr_srb>(lcid));
  }
  return get_drb_name(static_cast<nr_drb>(lcid - MAX_NR_SRB_ID));
}

} // namespace srsran

#endif // SRSRAN_COMMON_NR_H