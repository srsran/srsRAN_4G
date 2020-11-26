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

#ifndef SRSLTE_COMMON_NR_H
#define SRSLTE_COMMON_NR_H

namespace srslte {

// NR Radio Bearer Id

enum rb_id_nr_t { NR_SRB0, NR_SRB1, NR_SRB2, NR_SRB3, NR_DRB1, RB_ID_NR_N_ITEMS };
inline const char* to_string(rb_id_nr_t rb_id)
{
  const static char* names[] = {"SRB0", "SRB1", "SRB2", "SRB3", "DRB1"};
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

} // namespace srslte

#endif // SRSLTE_NR_COMMON_INTERFACE_TYPES_H
