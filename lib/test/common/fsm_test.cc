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
  struct idle_st {};
  struct state1 {};

  explicit fsm1(srslte::log_ref log_) : srslte::fsm_t<fsm1>(log_) {}

  // this state is another FSM
  class fsm2 : public subfsm_t<fsm2>
  {
  public:
    // states
    struct state_inner {};
    struct state_inner2 {};

    explicit fsm2(fsm1* f_) : nested_fsm_t(f_) {}
    ~fsm2() { log_h->info("%s being destroyed!", get_type_name(*this).c_str()); }

  protected:
    void enter(state_inner& s)
    {
      log_h->info("fsm1::%s::enter called\n", srslte::get_type_name(s).c_str());
      parent_fsm()->inner_enter_counter++;
    }
    void enter(state_inner2& s) { log_h->info("fsm1::%s::enter called\n", srslte::get_type_name(s).c_str()); }
    void exit(state_inner2& s) { log_h->info("fsm1::%s::exit called\n", srslte::get_type_name(s).c_str()); }

    // FSM2 transitions
    auto react(state_inner& s, ev1 e) -> to_state<state_inner>;
    auto react(state_inner& s, ev2 e) -> to_state<state_inner2>;
    auto react(state_inner2& s, ev2 e) -> to_state<state1>;

    // list of states
    state_list<state_inner, state_inner2> states{this};
  };

protected:
  // enter/exit
  void enter(idle_st& s);
  void enter(state1& s);

  // transitions
  auto react(idle_st& s, ev1 e) -> srslte::to_state<state1>;
  auto react(state1& s, ev1 e) -> srslte::to_state<fsm2>;
  auto react(state1& s, ev2 e) -> srslte::to_states<idle_st, fsm2>;

  void foo(ev1 e) { foo_counter++; }

  // list of states
  state_list<idle_st, state1, fsm2> states = {this, idle_st{}, state1{}, fsm2{this}};
};

void fsm1::enter(idle_st& s)
{
  log_h->info("%s::enter custom called\n", srslte::get_type_name(s).c_str());
  idle_enter_counter++;
}
void fsm1::enter(state1& s)
{
  log_h->info("%s::enter custom called\n", srslte::get_type_name(s).c_str());
  state1_enter_counter++;
}

// FSM event handlers
auto fsm1::fsm2::react(state_inner& s, ev1) -> to_state<state_inner>
{
  log_h->info("fsm2::state_inner::react called\n");
  return {};
}

auto fsm1::fsm2::react(state_inner& s, ev2) -> to_state<state_inner2>
{
  log_h->info("fsm2::state_inner::react called\n");
  return {};
}

auto fsm1::fsm2::react(state_inner2& s, ev2) -> to_state<state1>
{
  log_h->info("fsm2::state_inner::react called\n");
  return {};
}

auto fsm1::react(idle_st& s, ev1 e) -> to_state<state1>
{
  log_h->info("%s::react called\n", srslte::get_type_name(s).c_str());
  foo(e);
  return {};
}
auto fsm1::react(state1& s, ev1) -> to_state<fsm2>
{
  log_h->info("%s::react called\n", srslte::get_type_name(s).c_str());
  return {};
}
auto fsm1::react(state1& s, ev2) -> srslte::to_states<idle_st, fsm2>
{
  log_h->info("%s::react called\n", srslte::get_type_name(s).c_str());
  return srslte::to_state<idle_st>{};
}

// Static Checks

namespace srslte {
namespace fsm_details {

static_assert(is_fsm<fsm1>(), "invalid metafunction\n");
static_assert(is_subfsm<fsm1::fsm2>(), "invalid metafunction\n");
static_assert(std::is_same<fsm_helper::fsm_state_list_type<fsm1>,
                           fsm1::state_list<fsm1::idle_st, fsm1::state1, fsm1::fsm2> >::value,
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

struct procevent1 {};
struct procevent2 {};

struct proc1 : public srslte::proc_fsm_t<proc1, int> {
public:
  struct procstate1 {};

  proc1(srslte::log_ref log_) : base_t(log_) {}

protected:
  // Transitions
  auto react(idle_st& s, srslte::proc_launch_ev<int*> ev) -> to_state<procstate1>;
  auto react(procstate1& s, procevent1 ev) -> to_state<complete_st>;
  auto react(procstate1& s, procevent2 ev) -> to_state<complete_st>;
  auto react(complete_st& s, reset_ev ev) -> to_state<idle_st>;

  // example of uncaught event handling
  template <typename State>
  to_state<State> react(State& s, int e)
  {
    log_h->info("I dont know how to handle an \"int\" event\n");
    return {};
  }

  state_list<idle_st, procstate1, complete_st> states{this, idle_st{}, procstate1{}, complete_st{}};
};

auto proc1::react(idle_st& s, srslte::proc_launch_ev<int*> ev) -> to_state<procstate1>
{
  log_h->info("started!\n");
  return {};
}
auto proc1::react(procstate1& s, procevent1 ev) -> to_state<complete_st>
{
  log_h->info("success!\n");
  return set_success(5);
}
auto proc1::react(procstate1& s, procevent2 ev) -> to_state<complete_st>
{
  log_h->info("failure!\n");
  return set_failure();
}
auto proc1::react(complete_st& s, reset_ev ev) -> to_state<idle_st>
{
  log_h->info("propagate results %s\n", is_success() ? "success" : "failure");
  if (is_success()) {
    log_h->info("result was %d\n", get_result());
  }
  return {};
}

int test_fsm_proc()
{
  proc1 proc{srslte::logmap::get("PROC")};
  proc.get_log()->set_level(srslte::LOG_LEVEL_INFO);
  proc.set_fsm_event_log_level(srslte::LOG_LEVEL_INFO);
  int v = 2;
  proc.launch(&v);
  proc.launch(&v);
  proc.trigger(5);
  proc.trigger(procevent1{});
  proc.launch(&v);
  proc.trigger(procevent2{});

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
  auto react(emm_null_st& s, const enable_s1_ev& ev) -> to_state<emm_deregistered>;
  auto react(emm_deregistered& s, disable_s1_ev ev) -> to_state<emm_null_st>;
  auto react(emm_deregistered& s, attach_request_ev ev) -> to_state<emm_registered_initiated>;
  auto react(emm_registered_initiated& s, emm_registr_fail_ev ev) -> to_state<emm_deregistered>;
  auto react(emm_registered_initiated& s, attach_accept_ev ev) -> to_state<emm_registered>;
  auto react(emm_registered& s, sr_initiated_ev ev) -> to_state<emm_service_req_initiated>;
  auto react(emm_service_req_initiated& s, sr_outcome_ev) -> to_state<emm_registered>;
  auto react(emm_registered& s, tau_request_ev ev) -> to_state<emm_ta_updating_initiated>;
  auto react(emm_registered& s, detach_request_ev ev) -> to_state<emm_deregistered_initiated>;
  auto react(emm_ta_updating_initiated& s, tau_outcome_ev ev) -> to_state<emm_registered>;
  auto react(emm_ta_updating_initiated& s, tau_reject_other_cause_ev ev) -> to_state<emm_deregistered>;
  auto react(emm_deregistered_initiated& s, detach_accept_ev ev) -> to_state<emm_deregistered>;
  // on power-off go to deregistered state. Disable react if we are already in deregistered
  template <typename AnyState>
  auto react(AnyState& s, power_off_ev ev) -> to_state<emm_deregistered>;

  state_list<emm_null_st,
             emm_deregistered,
             emm_registered_initiated,
             emm_registered,
             emm_service_req_initiated,
             emm_ta_updating_initiated,
             emm_deregistered_initiated>
      states{nullptr};
};

#define LOGEVENT() log_h->info("Received an \"%s\" event\n", srslte::get_type_name(ev).c_str())

auto nas_fsm::react(emm_null_st& s, const enable_s1_ev& ev) -> to_state<emm_deregistered>
{
  LOGEVENT();
  return {};
}

auto nas_fsm::react(emm_deregistered& s, disable_s1_ev ev) -> to_state<emm_null_st>
{
  LOGEVENT();
  return {};
}

auto nas_fsm::react(emm_deregistered& s, attach_request_ev ev) -> to_state<emm_registered_initiated>
{
  LOGEVENT();
  return {};
}

auto nas_fsm::react(emm_registered_initiated& s, emm_registr_fail_ev ev) -> to_state<emm_deregistered>
{
  LOGEVENT();
  return {};
}

auto nas_fsm::react(emm_registered_initiated& s, attach_accept_ev ev) -> to_state<emm_registered>
{
  LOGEVENT();
  return {};
}

auto nas_fsm::react(emm_registered& s, sr_initiated_ev ev) -> to_state<emm_service_req_initiated>
{
  LOGEVENT();
  return {};
}
auto nas_fsm::react(emm_service_req_initiated& s, sr_outcome_ev ev) -> to_state<emm_registered>
{
  LOGEVENT();
  return {};
}
auto nas_fsm::react(emm_registered& s, tau_request_ev ev) -> to_state<emm_ta_updating_initiated>
{
  LOGEVENT();
  return {};
}
auto nas_fsm::react(emm_registered& s, detach_request_ev ev) -> to_state<emm_deregistered_initiated>
{
  LOGEVENT();
  return {};
}
auto nas_fsm::react(emm_ta_updating_initiated& s, tau_outcome_ev ev) -> to_state<emm_registered>
{
  LOGEVENT();
  return {};
}
auto nas_fsm::react(emm_ta_updating_initiated& s, tau_reject_other_cause_ev ev) -> to_state<emm_deregistered>
{
  LOGEVENT();
  return {};
}
auto nas_fsm::react(emm_deregistered_initiated& s, detach_accept_ev ev) -> to_state<emm_deregistered>
{
  LOGEVENT();
  return {};
}
template <typename AnyState>
auto nas_fsm::react(AnyState& s, power_off_ev ev) -> to_state<emm_deregistered>
{
  LOGEVENT();
  return {};
}

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
