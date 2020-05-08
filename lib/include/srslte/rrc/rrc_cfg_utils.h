/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSLTE_RRC_CFG_UTILS_H
#define SRSLTE_RRC_CFG_UTILS_H

#include "srslte/asn1/rrc_asn1.h"

namespace srslte {

//! convenience function overload to extract rrc fields Id
constexpr uint8_t get_id(const asn1::rrc::srb_to_add_mod_s& obj)
{
  return obj.srb_id;
}
constexpr uint8_t get_id(const asn1::rrc::drb_to_add_mod_s& obj)
{
  return obj.drb_id;
}

//! Functor to compare RRC config elements (e.g. SRB/measObj/Rep) based on ID
template <typename T>
struct field_id_cmp {
  bool operator()(const T& lhs, const T& rhs) const { return get_id(lhs) < get_id(rhs); }
  template <typename IdType>
  bool operator()(const T& lhs, IdType id) const
  {
    return get_id(lhs) < id;
  }
  template <typename IdType>
  bool operator()(IdType id, const T& rhs) const
  {
    return id < get_id(rhs);
  }
};
using srb_id_cmp = field_id_cmp<asn1::rrc::srb_to_add_mod_s>;
using drb_id_cmp = field_id_cmp<asn1::rrc::drb_to_add_mod_s>;

//! Search of rrc cfg element based on ID. Assumes sorted list.
template <typename Container, typename IdType>
typename Container::iterator binary_find(Container& c, IdType id)
{
  auto it = std::lower_bound(c.begin(), c.end(), id, field_id_cmp<decltype(*c.begin())>{});
  return (it == c.end() or get_id(*it) != id) ? c.end() : it;
}
template <typename Container, typename IdType>
typename Container::const_iterator binary_find(const Container& c, IdType id)
{
  auto it = std::lower_bound(c.begin(), c.end(), id, field_id_cmp<decltype(*c.begin())>{});
  return (it == c.end() or get_id(*it) != id) ? c.end() : it;
}

//! Update RRC field toAddModList
template <typename List, typename RemFunctor, typename AddFunctor, typename ModFunctor>
void apply_cfg_list_updates(List&      src_list,
                            List&      target_list,
                            RemFunctor rem_func,
                            AddFunctor add_func,
                            ModFunctor mod_func)
{
  // Sort by ID
  std::sort(src_list.begin(), src_list.end(), field_id_cmp<decltype(*src_list.begin())>{});
  std::sort(target_list.begin(), target_list.end(), field_id_cmp<decltype(*target_list.begin())>{});

  auto src_it    = src_list.begin();
  auto target_it = target_list.begin();

  bool src_left    = src_it != src_list.end();
  bool target_left = target_it != target_list.end();
  while (src_left or target_left) {
    if (not target_left or (src_left and get_id(*src_it) < get_id(*target_it))) {
      rem_func(src_it++);
    } else if (not src_left or (target_left and get_id(*src_it) > get_id(*target_it))) {
      // a new object has been added to target
      add_func(target_it++);
    } else {
      mod_func(src_it++, target_it++);
    }
    src_left    = src_it != src_list.end();
    target_left = target_it != target_list.end();
  }
}

} // namespace srslte

#endif // SRSLTE_RRC_CFG_UTILS_H
