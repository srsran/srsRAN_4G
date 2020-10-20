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

#include "srslte/adt/observer.h"
#include "srslte/common/fsm.h"
#include "srslte/common/logmap.h"
#include "srslte/interfaces/ue_interfaces.h"
#include <bitset>

namespace srsue {

class phy_controller : public srslte::fsm_t<phy_controller>
{
  using cell_search_ret_t = rrc_interface_phy_lte::cell_search_ret_t;

public:
  static const uint32_t wait_sync_timeout_ms = 50;

  // events
  struct cell_srch_res {
    cell_search_ret_t cs_ret;
    phy_cell_t        found_cell;
  };
  struct cell_sel_res {
    bool result;
  };
  struct cell_sel_cmd {
    phy_cell_t phy_cell;
  };
  struct cell_search_cmd {};
  struct in_sync_ev {
    static const bool log_verbose = false;
  };
  struct out_sync_ev {};

  explicit phy_controller(phy_interface_rrc_lte*                        phy_,
                          srslte::task_sched_handle                     task_sched_,
                          std::function<void(uint32_t, uint32_t, bool)> on_cell_selection = {});

  // PHY procedures interfaces
  bool start_cell_select(const phy_cell_t& phy_cell, srslte::event_observer<bool> observer = {});
  bool start_cell_search(srslte::event_observer<cell_srch_res> observer);
  void cell_search_completed(cell_search_ret_t cs_ret, phy_cell_t found_cell);
  void cell_selection_completed(bool outcome);
  void in_sync();
  void out_sync() { trigger(out_sync_ev{}); }
  bool set_cell_config(const srslte::phy_cfg_t& config, uint32_t cc_idx = 0);
  void set_phy_to_default();
  void set_phy_to_default_dedicated();
  void set_phy_to_default_pucch_srs();
  void set_config_complete();

  // state getters
  bool cell_is_camping() { return phy->cell_is_camping(); }
  bool is_in_sync() const { return is_in_state<in_sync_st>(); }
  bool is_config_pending() const { return nof_pending_configs == 0; }

  srslte::span<const srslte::phy_cfg_t>   current_cell_config() const { return current_cells_cfg; }
  srslte::span<srslte::phy_cfg_t>         current_cell_config() { return current_cells_cfg; }
  const std::bitset<SRSLTE_MAX_CARRIERS>& current_config_scells() const { return configured_scell_mask; }

  // FSM states
  struct unknown_st {};
  struct in_sync_st {};
  struct out_sync_st {};
  struct selecting_cell : public subfsm_t<selecting_cell> {
    struct timeout_ev {};

    struct wait_csel_res {};
    struct wait_in_sync {
      void enter(selecting_cell* f);
    };

    explicit selecting_cell(phy_controller* parent_);
    void enter(phy_controller* f, const cell_sel_cmd& ev);
    void exit(phy_controller* f);

    srslte::timer_handler::unique_timer wait_in_sync_timer;
    phy_cell_t                          target_cell = {};
    cell_sel_res                        csel_res    = {};

  protected:
    // guard functions
    bool is_cell_selected(wait_csel_res& s, const cell_sel_res& ev) { return ev.result; }

    // event handlers
    void set_success(wait_in_sync& s, const in_sync_ev& ev) { csel_res.result = true; }

    state_list<wait_csel_res, wait_in_sync> states{this};

    // clang-format off
    using c           = selecting_cell;
    using transitions = transition_table<
    //    Start            Target          Event          Action             Guard
    // +----------------+---------------+--------------+------------------+----------------------+
    row< wait_csel_res,   wait_in_sync,   cell_sel_res,  nullptr,           &c::is_cell_selected >,
    row< wait_csel_res,   unknown_st,     cell_sel_res                                           >,
    // +----------------+---------------+--------------+------------------+----------------------+
    row< wait_in_sync,    in_sync_st,     in_sync_ev,    &c::set_success                         >,
    row< wait_in_sync,    unknown_st,     timeout_ev                                             >,
    to_state<             unknown_st,     srslte::failure_ev                                     >
    // +----------------+---------------+--------------+------------------+----------------------+
    >;
    // clang-format on
  };
  struct searching_cell {
    void enter(phy_controller* f);
  };

private:
  phy_interface_rrc_lte*                             phy = nullptr;
  srslte::task_sched_handle                          task_sched;
  srslte::event_observer<bool>                       cell_selection_notifier;
  std::function<void(uint32_t, uint32_t, bool)>      cell_selection_always_observer;
  srslte::event_dispatcher<cell_srch_res>            cell_search_observers;
  uint32_t                                           nof_pending_configs   = 0;
  std::array<srslte::phy_cfg_t, SRSLTE_MAX_CARRIERS> current_cells_cfg     = {};
  std::bitset<SRSLTE_MAX_CARRIERS>                   configured_scell_mask = {};

  bool set_cell_config(const srslte::phy_cfg_t& cfg, uint32_t cc_idx, bool is_set);

protected:
  state_list<unknown_st, in_sync_st, out_sync_st, searching_cell, selecting_cell> states{this,
                                                                                         unknown_st{},
                                                                                         in_sync_st{},
                                                                                         out_sync_st{},
                                                                                         searching_cell{},
                                                                                         selecting_cell{this}};

  // event handlers
  void handle_cell_search_res(searching_cell& s, const cell_srch_res& result);

  // clang-format off
  using c = phy_controller;
  using transitions = transition_table<
  //   Start            Target           Event                   Action
  // +----------------+-----------------+------------------+------------------------------+
  row< unknown_st,      selecting_cell,   cell_sel_cmd                                    >,
  row< unknown_st,      searching_cell,   cell_search_cmd                                 >,
  row< unknown_st,      in_sync_st,       in_sync_ev                                      >,
  row< unknown_st,      out_sync_st,      out_sync_ev                                     >,
  // +----------------+-----------------+------------------+------------------------------+
  row< in_sync_st,      selecting_cell,   cell_sel_cmd                                    >,
  row< in_sync_st,      searching_cell,   cell_search_cmd                                 >,
  row< in_sync_st,      out_sync_st,      out_sync_ev                                     >,
  // +----------------+-----------------+------------------+------------------------------+
  row< out_sync_st,     selecting_cell,   cell_sel_cmd                                    >,
  row< out_sync_st,     searching_cell,   cell_search_cmd                                 >,
  row< out_sync_st,     in_sync_st,       in_sync_ev                                      >,
  // +----------------+-----------------+------------------+------------------------------+
  row< searching_cell,  unknown_st,       cell_srch_res,     &c::handle_cell_search_res   >
  // +----------------+-----------------+------------------+------------------------------+
  >;
  // clang-format on
};

} // namespace srsue

#endif // SRSLTE_PHY_CONTROLLER_H
