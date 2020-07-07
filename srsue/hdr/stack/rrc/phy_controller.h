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

#ifndef SRSLTE_PHY_CONTROLLER_H
#define SRSLTE_PHY_CONTROLLER_H

#include "srslte/common/fsm.h"
#include "srslte/common/logmap.h"
#include "srslte/interfaces/ue_interfaces.h"

namespace srsue {

class phy_controller : public srslte::fsm_t<phy_controller>
{
  using phy_cell_t        = phy_interface_rrc_lte::phy_cell_t;
  using cell_search_ret_t = phy_interface_rrc_lte::cell_search_ret_t;

public:
  static const uint32_t wait_sync_timeout_ms = 50;

  // events
  struct cell_srch_res {
    cell_search_ret_t cs_ret;
    phy_cell_t        found_cell;
  };
  using cell_sel_res = bool;
  struct cell_sel_cmd {
    phy_cell_t                           phy_cell;
    srslte::event_callback<cell_sel_res> callback;
  };
  using cell_search_cmd = srslte::event_callback<cell_srch_res>;
  struct in_sync_ev {};
  struct out_sync_ev {};
  struct timeout_ev {};

  explicit phy_controller(phy_interface_rrc_lte* phy_, stack_interface_rrc* stack_);

  // PHY procedures interfaces
  bool start_cell_select(const phy_cell_t& phy_cell, const srslte::event_callback<cell_sel_res>& on_complete);
  bool start_cell_search(const srslte::event_callback<cell_srch_res>& on_complete);
  bool cell_search_completed(cell_search_ret_t cs_ret, phy_cell_t found_cell);
  bool cell_selection_completed(bool outcome);
  void in_sync() { trigger(in_sync_ev{}); }
  void out_sync() { trigger(out_sync_ev{}); }

  // state getters
  bool cell_is_camping() { return phy->cell_is_camping(); }
  bool is_in_sync() const { return is_in_state<in_sync_st>(); }

private:
  phy_interface_rrc_lte* phy   = nullptr;
  stack_interface_rrc*   stack = nullptr;

protected:
  // states
  struct unknown_st {};
  struct in_sync_st {};
  struct out_sync_st {};
  struct selecting_cell : public subfsm_t<selecting_cell> {
    struct timeout_ev {};

    struct wait_result {};
    struct wait_in_sync {
      void enter(selecting_cell* f, const cell_sel_res& ev);
    };

    explicit selecting_cell(phy_controller* parent_);
    void enter(phy_controller* f, const cell_sel_cmd& ev);
    void exit(phy_controller* f);

    srslte::timer_handler::unique_timer  wait_in_sync_timer;
    phy_cell_t                           target_cell = {};
    cell_sel_res                         result      = {};
    srslte::event_callback<cell_sel_res> csel_callback;

  protected:
    // guard functions
    bool is_cell_selected(wait_result& s, const cell_sel_res& ev) { return ev; }

    // event handlers
    void set_success(wait_in_sync& s, const in_sync_ev& ev) { result = true; }

    state_list<wait_result, wait_in_sync> states{this};

    // clang-format off
    using c           = selecting_cell;
    using transitions = transition_table<
    //    Start            Target          Event          Action             Guard
    // +----------------+---------------+--------------+------------------+----------------------+
    row< wait_result,     wait_in_sync,   cell_sel_res,  nullptr,           &c::is_cell_selected >,
    row< wait_result,     unknown_st,     cell_sel_res                                           >,
    // +----------------+---------------+--------------+------------------+----------------------+
    row< wait_in_sync,    in_sync_st,     in_sync_ev,    &c::set_success                         >,
    row< wait_in_sync,    out_sync_st,    timeout_ev                                             >
    // +----------------+---------------+--------------+------------------+----------------------+
    >;
    // clang-format on
  };
  struct searching_cell {
    void enter(phy_controller* f, const cell_search_cmd& cmd);

    std::vector<srslte::event_callback<cell_srch_res> > csearch_callbacks;
  };
  state_list<unknown_st, in_sync_st, out_sync_st, searching_cell, selecting_cell> states{this,
                                                                                         unknown_st{},
                                                                                         in_sync_st{},
                                                                                         out_sync_st{},
                                                                                         searching_cell{},
                                                                                         selecting_cell{this}};

  // event handlers
  void handle_cell_search_res(searching_cell& s, const cell_srch_res& result);
  void share_cell_search_res(searching_cell& s, const cell_search_cmd& cmd);

  // clang-format off
  using c = phy_controller;
  using transitions = transition_table<
  //   Start            Target           Event                 Action                       Guard
  // +----------------+-----------------+------------------+------------------------------+---------------------+
  row< unknown_st,      selecting_cell,   cell_sel_cmd >,
  row< unknown_st,      searching_cell,   cell_search_cmd >,
  row< unknown_st,      in_sync_st,       in_sync_ev >,
  row< unknown_st,      out_sync_st,      out_sync_ev >,
  // +----------------+-----------------+------------------+------------------------------+---------------------+
  row< in_sync_st,      selecting_cell,   cell_sel_cmd >,
  row< in_sync_st,      searching_cell,   cell_search_cmd >,
  row< in_sync_st,      out_sync_st,      out_sync_ev >,
  // +----------------+-----------------+------------------+------------------------------+---------------------+
  row< out_sync_st,     selecting_cell,   cell_sel_cmd >,
  row< out_sync_st,     searching_cell,   cell_search_cmd >,
  row< out_sync_st,     in_sync_st,       in_sync_ev >,
  // +----------------+-----------------+------------------+------------------------------+---------------------+
  row< searching_cell,  unknown_st,       cell_srch_res,     &c::handle_cell_search_res >,
  upd< searching_cell,                    cell_search_cmd,   &c::share_cell_search_res >
  // +----------------+-----------------+------------------+------------------------------+---------------------+
  >;
  // clang-format on
};

} // namespace srsue

#endif // SRSLTE_PHY_CONTROLLER_H
