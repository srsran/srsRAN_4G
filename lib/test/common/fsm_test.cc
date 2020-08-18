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

std::vector<std::string> calls;
template <typename State>
void call_log_helper(State* state, srslte::log_ref log_h, const char* type)
{
  std::string callname = srslte::get_type_name<State>() + "::" + type;
  log_h->info("%s custom called\n", callname.c_str());
  calls.push_back(callname);
}

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
    void enter(fsm1* f) {}
    void enter(fsm1* f, const ev1& ev);
    void enter(fsm1* f, const ev2& ev);
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
        call_log_helper(this, f->get_log(), "enter");
        f->parent_fsm()->inner_enter_counter++;
      }
    };
    struct state_inner2 {
      void enter(fsm2* f) { call_log_helper(this, f->get_log(), "enter"); }
      void exit(fsm2* f) { call_log_helper(this, f->get_log(), "exit"); }
    };

    explicit fsm2(fsm1* f_) : composite_fsm_t(f_) {}
    fsm2(fsm2&&)  = default;
    fsm2& operator=(fsm2&&) = default;
    ~fsm2() { get_log()->info("%s being destroyed!", get_type_name(*this).c_str()); }

    void enter(fsm1* f) { call_log_helper(this, get_log(), "enter"); }
    void exit(fsm1* f) { call_log_helper(this, get_log(), "exit"); }

  private:
    void inner_action1(state_inner& s, const ev1& e);
    void inner_action2(state_inner& s, const ev2& e);
    void inner_action3(state_inner2& s, const ev2& e);

  protected:
    // list of states
    state_list<state_inner, state_inner2> states{this};
    // clang-format off
    using transitions = transition_table<
      //     Start       Target       Event    Action
      //  +------------+-------------+----+----------------------+
      upd<state_inner,                ev1, &fsm2::inner_action1  >,
      row<state_inner,  state_inner2, ev2, &fsm2::inner_action2  >,
      row<state_inner2, state1,       ev2, &fsm2::inner_action3  >
      //  +------------+-------------+----+----------------------+
    >;
    // clang-format on
  };

private:
  void action1(idle_st& s, const ev1& e);
  void action2(state1& s, const ev1& e);
  void action3(state1& s, const ev2& e);

protected:
  void foo(ev1 e) { foo_counter++; }

  // list of states + transitions
  state_list<idle_st, state1, fsm2> states = {this, idle_st{}, state1{}, fsm2{this}};

  // clang-format off
  using transitions = transition_table<
    //      Start         Target    Event        Action
    //  +------------+-------------+----+------------------+
    row< idle_st,     state1,       ev1,  &fsm1::action1   >,
    row< state1,      fsm2,         ev1,  &fsm1::action2   >,
    row< state1,      idle_st,      ev2,  &fsm1::action3   >
    //  +------------+-------------+----+------------------+
  >;
  // clang-format on
};

void fsm1::idle_st::enter(fsm1* f)
{
  call_log_helper(this, f->log_h, "enter");
  f->idle_enter_counter++;
}
void fsm1::state1::enter(fsm1* f, const ev1& ev)
{
  call_log_helper(this, f->log_h, "enter");
  f->state1_enter_counter++;
}
void fsm1::state1::enter(fsm1* f, const ev2& ev)
{
  call_log_helper(this, f->log_h, "enter2");
  f->state1_enter_counter++;
}
void fsm1::state1::exit(fsm1* f)
{
  call_log_helper(this, f->log_h, "exit");
}

// FSM event handlers
void fsm1::fsm2::inner_action1(state_inner& s, const ev1& e)
{
  call_log_helper(this, get_log(), "inner_action1");
}

void fsm1::fsm2::inner_action2(state_inner& s, const ev2& e)
{
  call_log_helper(this, get_log(), "inner_action2");
}

void fsm1::fsm2::inner_action3(state_inner2& s, const ev2& e)
{
  get_log()->info("fsm2::state_inner2::react called\n");
}

void fsm1::action1(idle_st& s, const ev1& e)
{
  call_log_helper(this, log_h, "action1");
  foo(e);
}

void fsm1::action2(state1& s, const ev1& ev)
{
  call_log_helper(this, log_h, "action2");
}

void fsm1::action3(state1& s, const ev2& ev)
{
  call_log_helper(this, log_h, "action3");
}

// Static Checks

namespace srslte {
namespace fsm_details {

static_assert(is_fsm<fsm1>::value, "invalid metafunction\n");
static_assert(is_composite_fsm<fsm1::fsm2>::value, "invalid metafunction\n");
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

  // Ensure correct call order
  TESTASSERT(calls[0] == srslte::get_type_name<fsm1::idle_st>() + "::enter"); // enter for init state called
  TESTASSERT(calls[1] == srslte::get_type_name<fsm1>() + "::action1");
  TESTASSERT(calls[2] == srslte::get_type_name<fsm1::state1>() + "::enter");
  TESTASSERT(calls[3] == srslte::get_type_name<fsm1>() + "::action2");
  TESTASSERT(calls[4] == srslte::get_type_name<fsm1::state1>() + "::exit");
  TESTASSERT(calls[5] == srslte::get_type_name<fsm1::fsm2>() + "::enter"); // entry is recursive
  TESTASSERT(calls[6] == srslte::get_type_name<fsm1::fsm2::state_inner>() + "::enter");
  TESTASSERT(calls[7] == srslte::get_type_name<fsm1::fsm2>() + "::inner_action1");
  TESTASSERT(calls[8] == srslte::get_type_name<fsm1::fsm2>() + "::inner_action2");
  TESTASSERT(calls[9] == srslte::get_type_name<fsm1::fsm2::state_inner2>() + "::enter");
  TESTASSERT(calls[10] == srslte::get_type_name<fsm1::fsm2::state_inner2>() + "::exit");
  TESTASSERT(calls[11] == srslte::get_type_name<fsm1::fsm2>() + "::exit");     // exit is recursive
  TESTASSERT(calls[12] == srslte::get_type_name<fsm1::state1>() + "::enter2"); // differentiates different entry funcs

  return SRSLTE_SUCCESS;
}

/////////////////////////////

struct procevent1 {
  bool is_success;
};

struct proc1 : public srslte::proc_fsm_t<proc1, int> {
public:
  struct procstate1 {
    void enter(proc1* f, const srslte::proc_launch_ev<int>& ev);
  };

  explicit proc1(srslte::log_ref log_) : base_t(log_) {}

protected:
  // Transitions
  void handle_success(procstate1& s, const procevent1& ev);
  void handle_failure(procstate1& s, const procevent1& ev);

  bool is_success(procstate1& s, const procevent1& ev) { return ev.is_success; }

  bool is_failure(procstate1& s, const procevent1& ev) { return not ev.is_success; }

  state_list<idle_st, procstate1> states{this, idle_st{}, procstate1{}};
  // clang-format off
  using transitions = transition_table<
    //      Start         Target      Event              Action                Guard (optional)
    //  +------------+-------------+----------------+------------------------+--------------------+
    row< idle_st,      procstate1,  launch_ev<int>                                                >,
    upd< procstate1,                procevent1,       &proc1::handle_success, &proc1::is_success  >,
    upd< procstate1,                procevent1,       &proc1::handle_failure, &proc1::is_failure  >,
    to_state<          idle_st,     complete_ev                                                   >
    //  +------------+-------------+----------------+------------------------+--------------------+
  >;
  // clang-format on
};

void proc1::procstate1::enter(proc1* f, const launch_ev<int>& ev)
{
  f->log_h->info("started!\n");
}

void proc1::handle_success(procstate1& s, const procevent1& ev)
{
  log_h->info("success!\n");
  trigger(complete_ev{5});
}

void proc1::handle_failure(procstate1& s, const procevent1& ev)
{
  log_h->info("failure!\n");
  trigger(complete_ev{3});
}

struct proc_listener_fsm : public srslte::fsm_t<proc_listener_fsm> {
public:
  struct st1 {};
  struct st2 {};
  using proc1_st = srslte::proc_wait_st<proc1>;

  explicit proc_listener_fsm(srslte::log_ref log_, proc1* proc_ptr_) :
    base_t(log_),
    states(this, st1{}, st2{}, proc1_st{proc_ptr_})
  {}

protected:
  bool is_success(proc1_st& s, const proc1::complete_ev& ev) { return ev.result; }

  // clang-format off
  state_list<st1, st2, proc1_st > states;
  using f = proc_listener_fsm;
  using transitions = transition_table<
    //      Start         Target             Event             Action                 Guard (optional)
    //  +--------------+--------------+-----------------------+------------------------+-------------------+
    row<  st1,           proc1_st,      int                                                                >,
    row<  proc1_st,      st2,           proc1::complete_ev,    nullptr,                 &f::is_success     >,
    row<  proc1_st,      st1,           proc1::complete_ev                                                 >
    //  +--------------+--------------+-----------------------+------------------------+-------------------+
  >;
  // clang-format on
};

int test_fsm_proc()
{
  proc1 proc{srslte::logmap::get("PROC")};
  proc.get_log()->set_level(srslte::LOG_LEVEL_INFO);
  proc.set_fsm_event_log_level(srslte::LOG_LEVEL_INFO);

  int v = 2;
  TESTASSERT(proc.current_state_name() == "idle_st");
  proc.trigger(srslte::proc_launch_ev<int>{v});
  TESTASSERT(proc.current_state_name() == "procstate1");
  proc.trigger(srslte::proc_launch_ev<int>{v});
  TESTASSERT(proc.current_state_name() == "procstate1");
  proc.trigger(srslte::proc_launch_ev<int>{5});
  TESTASSERT(proc.current_state_name() == "procstate1");
  proc.trigger(procevent1{true});
  TESTASSERT(proc.current_state_name() == "idle_st");
  TESTASSERT(proc.get_result() == 5);
  proc.trigger(srslte::proc_launch_ev<int>{v});
  TESTASSERT(proc.current_state_name() == "procstate1");
  proc.trigger(procevent1{false});
  TESTASSERT(proc.current_state_name() == "idle_st");
  TESTASSERT(proc.get_result() == 3);

  {
    proc_listener_fsm outer_fsm{srslte::logmap::get("TEST"), &proc};
    TESTASSERT(outer_fsm.is_in_state<proc_listener_fsm::st1>());
    outer_fsm.trigger(6);
    TESTASSERT(outer_fsm.is_in_state<proc_listener_fsm::proc1_st>());
    TESTASSERT(proc.is_running());
    proc.trigger(procevent1{true});
    TESTASSERT(not proc.is_running());
    TESTASSERT(outer_fsm.is_in_state<proc_listener_fsm::st2>());
  }

  {
    proc_listener_fsm outer_fsm{srslte::logmap::get("TEST"), &proc};
    TESTASSERT(outer_fsm.is_in_state<proc_listener_fsm::st1>());
    proc.trigger(srslte::proc_launch_ev<int>{v});
    TESTASSERT(proc.is_running());
    outer_fsm.trigger(7);
    TESTASSERT(outer_fsm.is_in_state<proc_listener_fsm::st1>());
    TESTASSERT(proc.is_running());
  }

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
    to_state<                        emm_deregistered,           power_off_ev              >
  //  +-----------------------------+-------------------------+-----------------------------+
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

struct fsm3 : public srslte::fsm_t<fsm3> {
  struct st1 {};
  struct st2 {
    int  counter = 0;
    void enter(fsm3* fsm)
    {
      counter++;
      fsm->events.push_back(fsm->current_state_name());
    }
  };

  fsm3() : base_t(srslte::log_ref{"TEST"}) {}

  std::vector<std::string> events;

protected:
  void handle_ev1(st1& s, const ev1& ev)
  {
    trigger(ev2{});
    events.push_back(current_state_name() + "::action"); // still in st1
  }
  void handle_ev2(st2& s, const ev2& ev)
  {
    if (s.counter < 2) {
      trigger(ev1{});
    }
    events.push_back(current_state_name() + "::action"); // still in st2
  }

  state_list<st1, st2> states{this};
  // clang-format off
  using transitions = transition_table<
  //     Start                    Target                   Event               Action
  //  +------------------------+-------------------------+-------------------+--------------------+
  row<  st1,                      st2,                     ev1,                &fsm3::handle_ev1  >,
  row<  st2,                      st1,                     ev2,                &fsm3::handle_ev2  >
  //  +------------------------+-------------------------+-------------------+--------------------+
  >;
  // clang-format on
};

int test_fsm_self_trigger()
{
  fsm3 fsm;
  TESTASSERT(fsm.events.empty());

  TESTASSERT(fsm.is_in_state<fsm3::st1>());
  fsm.trigger(ev1{});
  TESTASSERT(fsm.is_in_state<fsm3::st1>());

  TESTASSERT(fsm.events.size() == 6);
  TESTASSERT(fsm.events[0] == "st1::action");
  TESTASSERT(fsm.events[1] == "st2");
  TESTASSERT(fsm.events[2] == "st2::action");
  TESTASSERT(fsm.events[3] == "st1::action");
  TESTASSERT(fsm.events[4] == "st2");
  TESTASSERT(fsm.events[5] == "st2::action");

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
  TESTASSERT(test_fsm_self_trigger() == SRSLTE_SUCCESS);
  testlog->info("TEST \"fsm self trigger\" finished successfully\n\n");

  return SRSLTE_SUCCESS;
}
