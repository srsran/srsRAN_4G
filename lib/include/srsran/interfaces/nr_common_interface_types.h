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

#ifndef SRSRAN_COMMON_NR_H
#define SRSRAN_COMMON_NR_H

namespace srsran {

// NR Radio Bearer Id
// maxDRB = 29
enum rb_id_nr_t {
  NR_SRB0,
  NR_SRB1,
  NR_SRB2,
  NR_SRB3,
  NR_DRB1,
  NR_DRB2,
  NR_DRB3,
  NR_DRB4,
  NR_DRB5,
  NR_DRB6,
  NR_DRB7,
  NR_DRB8,
  NR_DRB9,
  NR_DRB10,
  NR_DRB11,
  NR_DRB12,
  NR_DRB13,
  NR_DRB14,
  NR_DRB15,
  NR_DRB16,
  NR_DRB17,
  NR_DRB18,
  NR_DRB19,
  NR_DRB20,
  NR_DRB21,
  NR_DRB22,
  NR_DRB23,
  NR_DRB24,
  NR_DRB25,
  NR_DRB26,
  NR_DRB27,
  NR_DRB28,
  NR_DRB29,
  RB_ID_NR_N_ITEMS
};

inline const char* to_string(rb_id_nr_t rb_id)
{
  const static char* names[] = {
      "SRB0",
      "SRB1",
      "SRB2",
      "SRB3",
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
      "DRB11",
      "DRB13",
      "DRB14",
      "DRB15",
      "DRB16",
      "DRB17",
      "DRB18",
      "DRB19",
      "DRB20",
      "DRB21",
      "DRB22",
      "DRB23",
      "DRB24",
      "DRB25",
      "DRB26",
      "DRB27",
      "DRB28",
      "DRB29",
  };
  return (rb_id < rb_id_nr_t::RB_ID_NR_N_ITEMS) ? names[rb_id] : "invalid bearer id";
}
inline bool is_srb(rb_id_nr_t lcid)
{
  return lcid <= rb_id_nr_t::NR_SRB3;
}
inline bool is_drb(rb_id_nr_t lcid)
{
  return not is_srb(lcid) and lcid <= rb_id_nr_t::RB_ID_NR_N_ITEMS;
}
inline int get_drb_id(rb_id_nr_t rb_id)
{
  return is_drb(rb_id) ? (rb_id - 3) : -1;
}

} // namespace srsran

#endif // SRSRAN_NR_COMMON_INTERFACE_TYPES_H
