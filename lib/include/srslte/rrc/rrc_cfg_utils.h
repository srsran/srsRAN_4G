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
#include <cassert>

namespace srslte {

//! Functor to compare RRC config elements (e.g. SRB/measObj/Rep) based on ID
template <typename T>
struct rrc_obj_id_cmp {
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
using rrc_obj_id_list_cmp = rrc_obj_id_cmp<typename Container::value_type>;

template <typename Container, typename IdType>
typename Container::iterator find_rrc_obj_id(Container& c, IdType id)
{
  return std::find_if(
      c.begin(), c.end(), [id](const typename Container::value_type& e) { return asn1::rrc::get_rrc_obj_id(e) == id; });
}
template <typename Container, typename IdType>
typename Container::const_iterator find_rrc_obj_id(const Container& c, IdType id)
{
  return std::find_if(
      c.begin(), c.end(), [id](const typename Container::value_type& e) { return asn1::rrc::get_rrc_obj_id(e) == id; });
}

//! Search of rrc cfg element based on ID. Assumes sorted list.
template <typename Container, typename IdType>
typename Container::iterator sorted_find_rrc_obj_id(Container& c, IdType id)
{
  auto it = std::lower_bound(c.begin(), c.end(), id, rrc_obj_id_list_cmp<Container>{});
  return (it == c.end() or asn1::rrc::get_rrc_obj_id(*it) != id) ? c.end() : it;
}
template <typename Container, typename IdType>
typename Container::const_iterator sorted_find_rrc_obj_id(const Container& c, IdType id)
{
  auto it = std::lower_bound(c.begin(), c.end(), id, rrc_obj_id_list_cmp<Container>{});
  return (it == c.end() or asn1::rrc::get_rrc_obj_id(*it) != id) ? c.end() : it;
}

//! Add Id to List in a sorted manner
template <typename Container, typename IdType>
typename Container::iterator add_rrc_obj_id(Container& c, IdType id)
{
  auto it = sorted_find_rrc_obj_id(c, id);
  if (it == c.end()) {
    c.push_back({});
    it = c.end() - 1;
    asn1::rrc::set_rrc_obj_id(*it, id);
    std::sort(c.begin(), c.end(), rrc_obj_id_list_cmp<Container>{});
    it = sorted_find_rrc_obj_id(c, id);
  }
  return it;
}

template <typename Container>
typename Container::iterator add_rrc_obj(Container& c, const typename Container::value_type& v)
{
  auto it = sorted_find_rrc_obj_id(c, asn1::rrc::get_rrc_obj_id(v));
  if (it == c.end()) {
    c.push_back(v);
    std::sort(c.begin(), c.end(), rrc_obj_id_list_cmp<Container>{});
    it = sorted_find_rrc_obj_id(c, asn1::rrc::get_rrc_obj_id(v));
  } else {
    *it = v;
  }
  return it;
}

/**
 * Find rrc obj id gap in list of rrc objs (e.g. {1, 2, 4} -> 3)
 * Expects list to be sorted
 * @return id value
 */
template <typename Container>
auto find_rrc_obj_id_gap(const Container& c) -> decltype(asn1::rrc::get_rrc_obj_id(c[0]))
{
  auto id_cmp_op = rrc_obj_id_list_cmp<Container>{};
  assert(std::is_sorted(c.begin(), c.end(), id_cmp_op));

  auto prev_it = c.begin();
  if (prev_it != c.end() and asn1::rrc::get_rrc_obj_id(*prev_it) == 1) {
    auto it = prev_it;
    for (++it; it != c.end(); prev_it = it, ++it) {
      if (asn1::rrc::get_rrc_obj_id(*it) > asn1::rrc::get_rrc_obj_id(*prev_it) + 1) {
        break;
      }
    }
  }
  return (prev_it == c.end()) ? 1 : asn1::rrc::get_rrc_obj_id(*prev_it) + 1; // starts at 1.
}

/**
 * Apply toAddModList changes
 * @param src_list original list of rrc fields
 * @param add_diff_list added/modified elements
 * @param target_list resulting list. (Can be same as src_list)
 */
template <typename AddModList>
void apply_addmodlist_diff(const AddModList& src_list, const AddModList& add_diff_list, AddModList& target_list)
{
  // Shortcut for empty case
  if (add_diff_list.size() == 0) {
    if (&target_list != &src_list) {
      target_list = src_list;
    }
    return;
  }
  auto id_cmp_op = rrc_obj_id_list_cmp<AddModList>{};
  assert(std::is_sorted(src_list.begin(), src_list.end(), id_cmp_op));
  assert(std::is_sorted(add_diff_list.begin(), add_diff_list.end(), id_cmp_op));

  AddModList l;
  std::set_union(
      add_diff_list.begin(), add_diff_list.end(), src_list.begin(), src_list.end(), std::back_inserter(l), id_cmp_op);
  target_list = l;
}

/**
 * Apply toAddModList/toRemoveList changes
 * @param src_list original list of rrc fields
 * @param add_diff_list added/modified elements
 * @param rm_diff_list removed elements
 * @param target_list resulting list. (Can be same as src_list)
 */
template <typename AddModList, typename RemoveList>
void apply_addmodremlist_diff(const AddModList& src_list,
                              const AddModList& add_diff_list,
                              RemoveList&       rm_diff_list,
                              AddModList&       target_list)
{
  if (add_diff_list.size() == 0 and rm_diff_list.size() == 0) {
    if (&target_list != &src_list) {
      target_list = src_list;
    }
    return;
  }
  auto id_cmp_op = rrc_obj_id_list_cmp<AddModList>{};
  assert(std::is_sorted(src_list.begin(), src_list.end(), id_cmp_op));
  assert(std::is_sorted(add_diff_list.begin(), add_diff_list.end(), id_cmp_op));
  assert(std::is_sorted(rm_diff_list.begin(), rm_diff_list.end()));

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

//! Update RRC field toAddModList
template <typename List, typename RemFunctor, typename AddFunctor, typename ModFunctor>
void compute_cfg_diff(const List& src_list,
                      const List& target_list,
                      RemFunctor  rem_func,
                      AddFunctor  add_func,
                      ModFunctor  mod_func)
{
  auto id_cmp_op = rrc_obj_id_list_cmp<List>{};
  assert(std::is_sorted(src_list.begin(), src_list.end(), id_cmp_op));
  assert(std::is_sorted(target_list.begin(), target_list.end(), id_cmp_op));

  auto src_it    = src_list.begin();
  auto target_it = target_list.begin();

  bool src_left    = src_it != src_list.end();
  bool target_left = target_it != target_list.end();
  while (src_left or target_left) {
    if (not target_left or (src_left and id_cmp_op(*src_it, *target_it))) {
      rem_func(src_it++);
    } else if (not src_left or (target_left and id_cmp_op(*target_it, *src_it))) {
      // a new object has been added to target
      add_func(target_it++);
    } else {
      mod_func(src_it++, target_it++);
    }
    src_left    = src_it != src_list.end();
    target_left = target_it != target_list.end();
  }
}

template <typename toAddModList, typename RemoveList>
void compute_cfg_diff(const toAddModList& src_list,
                      const toAddModList& target_list,
                      toAddModList&       add_diff_list,
                      RemoveList&         rem_diff_list)
{
  using it_t    = typename toAddModList::const_iterator;
  auto rem_func = [&rem_diff_list](it_t rem_it) { rem_diff_list.push_back(asn1::rrc::get_rrc_obj_id(*rem_it)); };
  auto add_func = [&add_diff_list](it_t add_it) { add_diff_list.push_back(*add_it); };
  auto mod_func = [&add_diff_list](it_t src_it, it_t target_it) {
    if (not(*src_it == *target_it)) {
      add_diff_list.push_back(*target_it);
    }
  };
  compute_cfg_diff(src_list, target_list, rem_func, add_func, mod_func);
}

} // namespace srslte

#endif // SRSLTE_RRC_CFG_UTILS_H
