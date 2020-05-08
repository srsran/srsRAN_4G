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

#include "srslte/common/fsm.h"
#include "srslte/common/test_common.h"

/////////////////////////////

// Events
struct ev1 {};
struct ev2 {};

class fsm1 : public srslte::fsm_t<fsm1>
{
public:
  uint32_t idle_enter_counter = 0, state1_enter_counter = 0, inner_enter_counter = 0;
  uint32_t foo_counter = 0;

  // states
  struct idle_st {
    void enter(fsm1* f);
  };
  struct state1 {
    void enter(fsm1* f);
    void exit(fsm1* f);
  };

  explicit fsm1(srslte::log_ref log_) : srslte::fsm_t<fsm1>(log_) {}

  // this state is another FSM
  class fsm2 : public subfsm_t<fsm2>
  {
  public:
    // states
    struct state_inner {
      void enter(fsm2* f)
      {
        f->log_h->info("fsm1::%s::enter called\n", srslte::get_type_name(*this).c_str());
        f->parent_fsm()->inner_enter_counter++;
      }
    };
    struct state_inner2 {
      void enter(fsm2* f) { f->log_h->info("fsm1::%s::enter called\n", srslte::get_type_name(*this).c_str()); }
      void exit(fsm2* f) { f->log_h->info("fsm1::%s::exit called\n", srslte::get_type_name(*this).c_str()); }
    };

    explicit fsm2(fsm1* f_) : nested_fsm_t(f_) {}
    ~fsm2() { log_h->info("%s being destroyed!", get_type_name(*this).c_str()); }

  private:
    void inner_action1(state_inner& s, state_inner& d, const ev1& e);
    void inner_action2(state_inner& s, state_inner2& d, const ev2& e);
    void inner_action3(state_inner2& s, state1& d, const ev2& e);

  protected:
    // list of states
    state_list<state_inner, state_inner2> states{this};
    // clang-format off
    using transitions = transition_table<
      //      Start         Target    Event        Action
      //  +------------+-------------+----+----------------------+
      row<state_inner,  state_inner,  ev1, &fsm2::inner_action1  >,
      row<state_inner,  state_inner2, ev2, &fsm2::inner_action2  >,
      row<state_inner2, state1,       ev2, &fsm2::inner_action3  >
      //  +------------+-------------+----+----------------------+
    >;
    // clang-format on
  };

private:
  void action1(idle_st& s, state1& d, const ev1& e);
  void action2(state1& s, fsm2& d, const ev1& e);
  void action3(state1& s, idle_st& d, const ev2& e);

protected:
  void foo(ev1 e) { foo_counter++; }

  // list of states + transitions
  state_list<idle_st, state1, fsm2> states = {this, idle_st{}, state1{}, fsm2{this}};

  // clang-format off
  using transitions = transition_table<
    //      Start         Target    Event        Action
    //  +------------+-------------+----+------------------+
    row< idle_st,     state1,       ev1,  &fsm1::action1>,
    row< state1,      fsm2,         ev1,  &fsm1::action2>,
    row< state1,      idle_st,      ev2,  &fsm1::action3>
  //  +------------+-------------+----+--------------------+
  >;
  // clang-format on
};

void fsm1::idle_st::enter(fsm1* f)
{
  f->log_h->info("%s::enter custom called\n", srslte::get_type_name(*this).c_str());
  f->idle_enter_counter++;
}
void fsm1::state1::enter(fsm1* f)
{
  f->log_h->info("%s::enter custom called\n", srslte::get_type_name(*this).c_str());
  f->state1_enter_counter++;
}
void fsm1::state1::exit(fsm1* f)
{
  f->log_h->info("%s::exit custom called\n", srslte::get_type_name(*this).c_str());
}

// FSM event handlers
void fsm1::fsm2::inner_action1(state_inner& s, state_inner& d, const ev1& e)
{
  log_h->info("fsm2::state_inner::react called\n");
}

void fsm1::fsm2::inner_action2(state_inner& s, state_inner2& d, const ev2& e)
{
  log_h->info("fsm2::state_inner::react called\n");
}

void fsm1::fsm2::inner_action3(state_inner2& s, state1& d, const ev2& e)
{
  log_h->info("fsm2::state_inner2::react called\n");
}

void fsm1::action1(idle_st& s, state1& d, const ev1& e)
{
  log_h->info("%s::react called\n", srslte::get_type_name(s).c_str());
  foo(e);
}

void fsm1::action2(state1& s, fsm2& d, const ev1& ev)
{
  log_h->info("%s::react called\n", srslte::get_type_name(s).c_str());
}

void fsm1::action3(state1& s, idle_st& d, const ev2& ev)
{
  log_h->info("%s::react called\n", srslte::get_type_name(s).c_str());
}

// Static Checks

namespace srslte {
namespace fsm_details {

static_assert(is_fsm<fsm1>(), "invalid metafunction\n");
static_assert(is_subfsm<fsm1::fsm2>(), "invalid metafunction\n");
static_assert(type_list_size(typename filter_transition_type<ev1, fsm1::idle_st, fsm_transitions<fsm1> >::type{}) > 0,
              "invalid filter metafunction\n");
static_assert(
    std::is_same<fsm_state_list_type<fsm1>, fsm1::state_list<fsm1::idle_st, fsm1::state1, fsm1::fsm2> >::value,
    "get state list failed\n");
static_assert(fsm1::can_hold_state<fsm1::state1>(), "failed can_hold_state check\n");
static_assert(std::is_same<enable_if_fsm_state<fsm1, fsm1::idle_st>, void>::value, "get state list failed\n");
static_assert(std::is_same<disable_if_fsm_state<fsm1, fsm1::fsm2::state_inner>, void>::value,
              "get state list failed\n");

} // namespace fsm_details
} // namespace srslte

// Runtime checks

int test_hsm()
{
  srslte::log_ref log_h{"HSM"};
  log_h->set_level(srslte::LOG_LEVEL_INFO);

  fsm1 f{log_h};
  TESTASSERT(f.idle_enter_counter == 1);
  TESTASSERT(get_type_name(f) == "fsm1");
  TESTASSERT(f.current_state_name() == "idle_st");
  TESTASSERT(f.is_in_state<fsm1::idle_st>());
  TESTASSERT(f.foo_counter == 0);

  // Moving Idle -> State1
  ev1 e;
  f.trigger(e);
  TESTASSERT(f.current_state_name() == "state1");
  TESTASSERT(f.is_in_state<fsm1::state1>());

  // Moving State1 -> fsm2
  f.trigger(e);
  TESTASSERT(f.current_state_name() == "fsm2");
  TESTASSERT(f.is_in_state<fsm1::fsm2>());
  TESTASSERT(f.get_if_current_state<fsm1::fsm2>()->current_state_name() == "state_inner");
  TESTASSERT(f.inner_enter_counter == 1);

  // Fsm2 does not listen to ev1
  f.trigger(e);
  TESTASSERT(f.current_state_name() == "fsm2");
  TESTASSERT(f.is_in_state<fsm1::fsm2>());
  TESTASSERT(f.get_if_current_state<fsm1::fsm2>()->current_state_name() == "state_inner");

  // Fsm2 state_inner -> state_inner2
  f.trigger(ev2{});
  TESTASSERT(f.current_state_name() == "fsm2");
  TESTASSERT(f.is_in_state<fsm1::fsm2>());
  TESTASSERT(f.get_if_current_state<fsm1::fsm2>()->current_state_name() == "state_inner2");

  // Moving fsm2 -> state1
  f.trigger(ev2{});
  TESTASSERT(f.current_state_name() == "state1");
  TESTASSERT(f.is_in_state<fsm1::state1>());
  TESTASSERT(f.state1_enter_counter == 2);

  // Moving state1 -> idle
  f.trigger(ev2{});
  TESTASSERT(std::string{f.current_state_name()} == "idle_st");
  TESTASSERT(f.is_in_state<fsm1::idle_st>());
  TESTASSERT(f.foo_counter == 1);
  TESTASSERT(f.idle_enter_counter == 2);

  // Call unhandled event
  f.trigger(ev2{});
  TESTASSERT(f.current_state_name() == "idle_st");

  // Enter fsm2 again
  f.trigger(ev1{});
  f.trigger(ev1{});
  TESTASSERT(f.current_state_name() == "fsm2");
  TESTASSERT(f.is_in_state<fsm1::fsm2>());
  TESTASSERT(f.get_if_current_state<fsm1::fsm2>()->current_state_name() == "state_inner");

  return SRSLTE_SUCCESS;
}

/////////////////////////////

struct procevent1 {
  bool is_success;
};

struct proc1 : public srslte::proc_fsm_t<proc1, int> {
public:
  struct procstate1 {};

  proc1(srslte::log_ref log_) : base_t(log_) {}

protected:
  // Transitions
  void init(idle_st& s, procstate1& d, const srslte::proc_launch_ev<int*>& ev);

  void handle_success(procstate1& s, idle_st& d, const procevent1& ev);

  void handle_failure(procstate1& s, idle_st& d, const procevent1& ev);

  bool is_success(procstate1& s, const procevent1& ev) const { return ev.is_success; }

  bool is_failure(procstate1& s, const procevent1& ev) const { return not ev.is_success; }

  state_list<idle_st, procstate1> states{this, idle_st{}, procstate1{}};
  // clang-format off
  using transitions = transition_table<
    //      Start         Target             Event                        Action            Guard (optional)
    //  +------------+-------------+----------------------------+------------------------+--------------------+
    row< idle_st,      procstate1,  srslte::proc_launch_ev<int*>, &proc1::init                                >,
    row< procstate1,   idle_st,     procevent1,                   &proc1::handle_success, &proc1::is_success  >,
    row< procstate1,   idle_st,     procevent1,                   &proc1::handle_failure, &proc1::is_failure  >
    //  +------------+-------------+----------------------------+------------------------+--------------------+
  >;
  // clang-format on
};

void proc1::init(idle_st& s, procstate1& d, const srslte::proc_launch_ev<int*>& ev)
{
  log_h->info("started!\n");
}

void proc1::handle_success(procstate1& s, idle_st& d, const procevent1& ev)
{
  log_h->info("success!\n");
  d = {true, 5};
}

void proc1::handle_failure(procstate1& s, idle_st& d, const procevent1& ev)
{
  log_h->info("failure!\n");
  d = {false, 3};
}

int test_fsm_proc()
{
  proc1 proc{srslte::logmap::get("PROC")};
  proc.get_log()->set_level(srslte::LOG_LEVEL_INFO);
  proc.set_fsm_event_log_level(srslte::LOG_LEVEL_INFO);

  int v = 2;
  TESTASSERT(proc.current_state_name() == "idle_st");
  proc.launch(&v);
  TESTASSERT(proc.current_state_name() == "procstate1");
  proc.launch(&v);
  TESTASSERT(proc.current_state_name() == "procstate1");
  proc.trigger(5);
  TESTASSERT(proc.current_state_name() == "procstate1");
  proc.trigger(procevent1{true});
  TESTASSERT(proc.current_state_name() == "idle_st");
  TESTASSERT(proc.get_state<proc1::idle_st>()->is_success());
  proc.launch(&v);
  TESTASSERT(proc.current_state_name() == "procstate1");
  proc.trigger(procevent1{false});
  TESTASSERT(proc.current_state_name() == "idle_st");
  TESTASSERT(not proc.get_state<proc1::idle_st>()->is_success());

  return SRSLTE_SUCCESS;
}

///////////////////////////

class nas_fsm : public srslte::fsm_t<nas_fsm>
{
public:
  // states
  struct emm_null_st {};
  struct emm_deregistered {};
  struct emm_deregistered_initiated {};
  struct emm_ta_updating_initiated {};
  struct emm_registered {};
  struct emm_service_req_initiated {};
  struct emm_registered_initiated {};

  // events
  struct enable_s1_ev {};
  struct disable_s1_ev {};
  struct attach_request_ev {};
  struct emm_registr_fail_ev {}; ///< attach rejected, network init DETACH request, lower layer failure
  struct attach_accept_ev {};    ///< attach accept and default EPS bearer context activated
  struct sr_initiated_ev {};
  struct sr_outcome_ev {};
  struct tau_request_ev {};
  struct tau_outcome_ev {};
  struct tau_reject_other_cause_ev {};
  struct power_off_ev {};
  struct detach_request_ev {};
  struct detach_accept_ev {};

  nas_fsm(srslte::log_ref log_) : fsm_t<nas_fsm>(log_) {}

protected:
  state_list<emm_null_st,
             emm_deregistered,
             emm_registered_initiated,
             emm_registered,
             emm_service_req_initiated,
             emm_ta_updating_initiated,
             emm_deregistered_initiated>
      states{nullptr};
  // clang-format off
  using transitions = transition_table<
  //              Start                       Target                     Event
  //  +-----------------------------+-------------------------+-----------------------------+
    row< emm_null_st,                emm_deregistered,           enable_s1_ev              >,
    row< emm_deregistered,           emm_null_st,                disable_s1_ev             >,
    row< emm_deregistered,           emm_registered_initiated,   attach_request_ev         >,
    row< emm_registered_initiated,   emm_deregistered,           emm_registr_fail_ev       >,
    row< emm_registered_initiated,   emm_registered,             attach_accept_ev          >,
    row< emm_registered,             emm_service_req_initiated,  sr_initiated_ev           >,
    row< emm_service_req_initiated,  emm_registered,             sr_outcome_ev             >,
    row< emm_registered,             emm_ta_updating_initiated,  tau_request_ev            >,
    row< emm_registered,             emm_deregistered_initiated, detach_request_ev         >,
    row< emm_ta_updating_initiated,  emm_registered,             tau_outcome_ev            >,
    row< emm_ta_updating_initiated,  emm_deregistered,           tau_reject_other_cause_ev >,
    row< emm_deregistered_initiated, emm_deregistered,           detach_accept_ev          >,
    from_any_state<emm_deregistered, power_off_ev>
  >;
  // clang-format on
};

int test_nas_fsm()
{
  srslte::log_ref log_h{"NAS"};
  log_h->set_level(srslte::LOG_LEVEL_INFO);
  nas_fsm fsm{log_h};
  TESTASSERT(fsm.is_in_state<nas_fsm::emm_null_st>());

  // NULL -> EMM-DEREGISTERED
  fsm.trigger(nas_fsm::enable_s1_ev{});
  TESTASSERT(fsm.is_in_state<nas_fsm::emm_deregistered>());

  // EMM-DEREGISTERED -> EMM-NULL -> EMM-DEREGISTERED
  fsm.trigger(nas_fsm::disable_s1_ev{});
  TESTASSERT(fsm.is_in_state<nas_fsm::emm_null_st>());
  fsm.trigger(nas_fsm::enable_s1_ev{});
  TESTASSERT(fsm.is_in_state<nas_fsm::emm_deregistered>());

  // EMM-DEREGISTERED -> EMM-REGISTERED-INITIATED -> EMM-DEREGISTERED
  fsm.trigger(nas_fsm::attach_request_ev{});
  TESTASSERT(fsm.is_in_state<nas_fsm::emm_registered_initiated>());
  fsm.trigger(nas_fsm::emm_registr_fail_ev{});
  TESTASSERT(fsm.is_in_state<nas_fsm::emm_deregistered>());

  // EMM-DEREGISTERED -> EMM-REGISTERED-INITIATED -> EMM-REGISTERED
  fsm.trigger(nas_fsm::attach_request_ev{});
  TESTASSERT(fsm.is_in_state<nas_fsm::emm_registered_initiated>());
  fsm.trigger(nas_fsm::attach_accept_ev{});
  TESTASSERT(fsm.is_in_state<nas_fsm::emm_registered>());

  // EMM-REGISTERED -> EMM-SERVICE-REQUEST-INITIATED -> EMM-REGISTERED
  fsm.trigger(nas_fsm::sr_initiated_ev{});
  TESTASSERT(fsm.is_in_state<nas_fsm::emm_service_req_initiated>());
  fsm.trigger(nas_fsm::sr_outcome_ev{});
  TESTASSERT(fsm.is_in_state<nas_fsm::emm_registered>());

  // EMM-REGISTERED -> EMM-TRACKING-AREA-UPDATING-INITIATED -> EMM-REGISTERED
  fsm.trigger(nas_fsm::tau_request_ev{});
  TESTASSERT(fsm.is_in_state<nas_fsm::emm_ta_updating_initiated>());
  fsm.trigger(nas_fsm::tau_outcome_ev{});
  TESTASSERT(fsm.is_in_state<nas_fsm::emm_registered>());

  // EMM-REGISTERED -> EMM-DEREGISTED-INITIATED -> EMM-DEREGISTERED
  fsm.trigger(nas_fsm::detach_request_ev{});
  TESTASSERT(fsm.is_in_state<nas_fsm::emm_deregistered_initiated>());
  fsm.trigger(nas_fsm::detach_accept_ev{});
  TESTASSERT(fsm.is_in_state<nas_fsm::emm_deregistered>());

  // EMM-DEREGISTERED -> EMM-REGISTERED-INITIATED -> EMM-REGISTERED -> EMM-TRACKING-AREA-UPDATING-INITIATED ->
  // EMM-DEREGISTERED
  fsm.trigger(nas_fsm::attach_request_ev{});
  TESTASSERT(fsm.is_in_state<nas_fsm::emm_registered_initiated>());
  fsm.trigger(nas_fsm::attach_accept_ev{});
  TESTASSERT(fsm.is_in_state<nas_fsm::emm_registered>());
  fsm.trigger(nas_fsm::tau_request_ev{});
  fsm.trigger(nas_fsm::tau_reject_other_cause_ev{});
  TESTASSERT(fsm.is_in_state<nas_fsm::emm_deregistered>());

  // EMM-DEREGISTERED -> EMM-REGISTERED-INITIATED -> EMM-REGISTERED -> EMM-SERVICE-REQUEST-INITIATED -> EMM-DEREGISTERED
  // (power-off)
  fsm.trigger(nas_fsm::attach_request_ev{});
  fsm.trigger(nas_fsm::attach_accept_ev{});
  fsm.trigger(nas_fsm::sr_initiated_ev{});
  TESTASSERT(fsm.is_in_state<nas_fsm::emm_service_req_initiated>());
  fsm.trigger(nas_fsm::power_off_ev{});
  TESTASSERT(fsm.is_in_state<nas_fsm::emm_deregistered>());

  return SRSLTE_SUCCESS;
}

int main()
{
  srslte::log_ref testlog{"TEST"};
  testlog->set_level(srslte::LOG_LEVEL_INFO);
  TESTASSERT(test_hsm() == SRSLTE_SUCCESS);
  testlog->info("TEST \"hsm\" finished successfully\n\n");
  TESTASSERT(test_fsm_proc() == SRSLTE_SUCCESS);
  testlog->info("TEST \"proc\" finished successfully\n\n");
  TESTASSERT(test_nas_fsm() == SRSLTE_SUCCESS);
  testlog->info("TEST \"nas fsm\" finished successfully\n\n");

  return SRSLTE_SUCCESS;
}
