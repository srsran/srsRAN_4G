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

#include "srslte/asn1/rrc_asn1_utils.h"
#include <algorithm>

namespace srslte {

//! Functor to compare RRC config elements (e.g. SRB/measObj/Rep) based on ID
template <typename T>
struct rrb_obj_id_cmp {
  bool operator()(const T& lhs, const T& rhs) const
  {
    return asn1::rrc::get_rrc_obj_id(lhs) < asn1::rrc::get_rrc_obj_id(rhs);
  }
  template <typename IdType>
  bool operator()(const T& lhs, IdType id) const
  {
    return asn1::rrc::get_rrc_obj_id(lhs) < id;
  }
  template <typename IdType>
  bool operator()(IdType id, const T& rhs) const
  {
    return id < asn1::rrc::get_rrc_obj_id(rhs);
  }
};
template <typename Container>
using rrc_obj_id_list_cmp = rrb_obj_id_cmp<typename Container::value_type>;

template <typename Container, typename IdType>
typename Container::iterator find_rrc_obj_id(Container& c, IdType id)
{
  return std::find_if(
      c.begin(), c.end(), [id](const typename Container::value_type& e) { return asn1::rrc::get_rrc_obj_id(e) == id; });
}
template <typename Container, typename IdType>
typename Container::const_iterator find_rrc_obj_id(const Container& c, IdType id)
{
  return std::find_if(c.begin(), c.end(), [id](const typename Container::value_type& e) {
    return asn1::rrc::get_rrc_obj_id(*e) == id;
  });
}

template <typename Container, typename IdType>
typename Container::iterator add_rrc_obj_id(Container& c, IdType id)
{
  auto it = find_rrc_obj_id(c, id);
  if (it == c.end()) {
    c.push_back({});
    it = c.end() - 1;
    asn1::rrc::set_rrc_obj_id(*it, id);
  }
  return it;
}

//! Search of rrc cfg element based on ID. Assumes sorted list.
template <typename Container, typename IdType>
typename Container::iterator binary_find(Container& c, IdType id)
{
  auto it = std::lower_bound(c.begin(), c.end(), id, rrb_obj_id_cmp<decltype(*c.begin())>{});
  return (it == c.end() or asn1::rrc::get_rrc_obj_id(*it) != id) ? c.end() : it;
}
template <typename Container, typename IdType>
typename Container::const_iterator binary_find(const Container& c, IdType id)
{
  auto it = std::lower_bound(c.begin(), c.end(), id, rrb_obj_id_cmp<decltype(*c.begin())>{});
  return (it == c.end() or asn1::rrc::get_rrc_obj_id(*it) != id) ? c.end() : it;
}

/**
 * Apply toAddModList/toRemoveList changes
 * @param src_list original list of rrc fields
 * @param add_diff_list added/modified elements
 * @param rm_diff_list removed elements
 * @param target_list resulting list. (Can be same as src_list)
 */
template <typename AddModList, typename RemoveList>
void apply_addmodremlist_diff(AddModList& src_list,
                              AddModList& add_diff_list,
                              RemoveList& rm_diff_list,
                              AddModList& target_list)
{
  // Sort Lists by ID
  auto id_cmp_op = rrc_obj_id_list_cmp<AddModList>{};
  std::sort(src_list.begin(), src_list.end(), id_cmp_op);
  std::sort(add_diff_list.begin(), add_diff_list.end(), id_cmp_op);
  std::sort(rm_diff_list.begin(), rm_diff_list.end());

  AddModList tmp_lst;
  // apply remove list
  std::set_difference(src_list.begin(),
                      src_list.end(),
                      rm_diff_list.begin(),
                      rm_diff_list.end(),
                      std::back_inserter(tmp_lst),
                      id_cmp_op);

  // apply toaddmodlist
  target_list.clear();
  std::set_union(add_diff_list.begin(),
                 add_diff_list.end(),
                 tmp_lst.begin(),
                 tmp_lst.end(),
                 std::back_inserter(target_list),
                 id_cmp_op);
}

/**
 * Apply toAddModList changes
 * @param src_list original list of rrc fields
 * @param add_diff_list added/modified elements
 * @param target_list resulting list. (Can be same as src_list)
 */
template <typename AddModList>
void apply_addmodlist_diff(AddModList& src_list, AddModList& add_diff_list, AddModList& target_list)
{
  // Sort Lists by ID
  auto id_cmp_op = rrc_obj_id_list_cmp<AddModList>{};
  std::sort(src_list.begin(), src_list.end(), id_cmp_op);
  std::sort(add_diff_list.begin(), add_diff_list.end(), id_cmp_op);

  if (&target_list != &src_list) {
    target_list.resize(0);
    std::set_union(add_diff_list.begin(),
                   add_diff_list.end(),
                   src_list.begin(),
                   src_list.end(),
                   std::back_inserter(target_list),
                   id_cmp_op);
  } else {
    AddModList l;
    std::set_union(
        add_diff_list.begin(), add_diff_list.end(), src_list.begin(), src_list.end(), std::back_inserter(l), id_cmp_op);
    target_list = l;
  }
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
  std::sort(src_list.begin(), src_list.end(), rrb_obj_id_cmp<decltype(*src_list.begin())>{});
  std::sort(target_list.begin(), target_list.end(), rrb_obj_id_cmp<decltype(*target_list.begin())>{});

  auto src_it    = src_list.begin();
  auto target_it = target_list.begin();

  bool src_left    = src_it != src_list.end();
  bool target_left = target_it != target_list.end();
  while (src_left or target_left) {
    if (not target_left or (src_left and asn1::rrc::get_rrc_obj_id(*src_it) < asn1::rrc::get_rrc_obj_id(*target_it))) {
      rem_func(src_it++);
    } else if (not src_left or
               (target_left and asn1::rrc::get_rrc_obj_id(*src_it) > asn1::rrc::get_rrc_obj_id(*target_it))) {
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
