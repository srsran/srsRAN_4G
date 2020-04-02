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

srslte::log_ref test_log{"TEST"};

/////////////////////////////

// Events
struct ev1 {
};
struct ev2 {
};

class fsm1 : public srslte::fsm_t<fsm1>
{
public:
  uint32_t idle_enter_counter = 0, state1_enter_counter = 0;
  uint32_t foo_counter = 0;

  const char* name() const override { return "fsm1"; }

  // idle state
  struct idle_st : public srslte::state_t {
    idle_st(fsm1* f) { f->idle_enter_counter++; }
    void        enter() final { test_log->info("fsm1::%s::enter called\n", name()); }
    void        exit() final { test_log->info("fsm1::%s::exit called\n", name()); }
    const char* name() const final { return "idle"; }
  };

  // simple state
  class state1 : public srslte::state_t
  {
  public:
    state1(fsm1* f) { f->state1_enter_counter++; }
    void        enter() final { test_log->info("fsm1::%s::enter called\n", name()); }
    void        exit() final { test_log->info("fsm1::%s::exit called\n", name()); }
    const char* name() const final { return "state1"; }
  };

  // this state is another FSM
  class fsm2 : public srslte::nested_fsm_t<fsm2, fsm1>
  {
  public:
    struct state_inner : public srslte::state_t {
      const char* name() const final { return "state_inner"; }
      void        enter() { test_log->info("fsm2::%s::enter called\n", name()); }
      void        exit() final { test_log->info("fsm2::%s::exit called\n", name()); }
    };

    fsm2(fsm1* f_) : nested_fsm_t(f_) {}
    void        enter() final { test_log->info("%s::enter called\n", name()); }
    void        exit() { test_log->info("%s::exit called\n", name()); }
    const char* name() const final { return "fsm2"; }

  protected:
    // FSM2 transitions
    auto react(state_inner& s, ev1 e) -> srslte::same_state;
    auto react(state_inner& s, ev2 e) -> state1;

    // list of states
    state_list<state_inner> states;
  };

protected:
  // transitions
  auto react(idle_st& s, ev1 e) -> state1;
  auto react(state1& s, ev1 e) -> fsm2;
  auto react(state1& s, ev2 e) -> srslte::choice_t<idle_st, fsm2>;

  void foo(ev1 e) { foo_counter++; }

  // list of states
  state_list<idle_st, state1, fsm2> states{idle_st{this}};
};

// FSM event handlers
auto fsm1::fsm2::react(state_inner& s, ev1 e) -> srslte::same_state
{
  test_log->info("fsm2::state_inner::react called\n");
  return {};
}

auto fsm1::fsm2::react(state_inner& s, ev2 e) -> state1
{
  test_log->info("fsm2::state_inner::react called\n");
  return state1{parent_fsm()};
}

auto fsm1::react(idle_st& s, ev1 e) -> state1
{
  test_log->info("fsm1::%s::react called\n", s.name());
  foo(e);
  return state1{this};
}
auto fsm1::react(state1& s, ev1 e) -> fsm2
{
  test_log->info("fsm1::%s::react called\n", s.name());
  return fsm2{this};
}
auto fsm1::react(state1& s, ev2 e) -> srslte::choice_t<idle_st, fsm2>
{
  test_log->info("fsm1::%s::react called\n", s.name());
  return idle_st{this};
}

// Static Checks

namespace srslte {
namespace fsm_details {

static_assert(std::is_same<fsm_helper::get_fsm_state_list<fsm1>,
                           fsm1::state_list<fsm1::idle_st, fsm1::state1, fsm1::fsm2> >::value,
              "get state list failed\n");
static_assert(std::is_same<fsm_helper::enable_if_fsm_state<fsm1, fsm1::idle_st>, void>::value,
              "get state list failed\n");
static_assert(std::is_same<fsm_helper::disable_if_fsm_state<fsm1, fsm1::fsm2::state_inner>, void>::value,
              "get state list failed\n");
static_assert(fsm1::can_hold_state<fsm1::state1>(), "can hold state method failed\n");

} // namespace fsm_details
} // namespace srslte

// Runtime checks

int test_hsm()
{
  test_log->set_level(srslte::LOG_LEVEL_INFO);

  fsm1 f;
  TESTASSERT(std::string{f.name()} == "fsm1");
  TESTASSERT(std::string{f.get_state_name()} == "idle");
  TESTASSERT(f.is_in_state<fsm1::idle_st>());
  TESTASSERT(f.foo_counter == 0);
  TESTASSERT(f.idle_enter_counter == 1);

  // Moving Idle -> State1
  ev1 e;
  f.trigger(e);
  TESTASSERT(std::string{f.get_state_name()} == "state1");
  TESTASSERT(f.is_in_state<fsm1::state1>());

  // Moving State1 -> fsm2
  f.trigger(e);
  TESTASSERT(std::string{f.get_state_name()} == "fsm2");
  TESTASSERT(f.is_in_state<fsm1::fsm2>());
  TESTASSERT(std::string{f.get_state<fsm1::fsm2>()->get_state_name()} == "state_inner");

  // Fsm2 does not listen to ev1
  f.trigger(e);
  TESTASSERT(std::string{f.get_state_name()} == "fsm2");
  TESTASSERT(f.is_in_state<fsm1::fsm2>());
  TESTASSERT(std::string{f.get_state<fsm1::fsm2>()->get_state_name()} == "state_inner");

  // Moving fsm2 -> state1
  f.trigger(ev2{});
  TESTASSERT(std::string{f.get_state_name()} == "state1");
  TESTASSERT(f.is_in_state<fsm1::state1>());
  TESTASSERT(f.state1_enter_counter == 2);

  // Moving state1 -> idle
  f.trigger(ev2{});
  TESTASSERT(std::string{f.get_state_name()} == "idle");
  TESTASSERT(f.is_in_state<fsm1::idle_st>());
  TESTASSERT(f.foo_counter == 1);
  TESTASSERT(f.idle_enter_counter == 2);

  return SRSLTE_SUCCESS;
}

int main()
{
  TESTASSERT(test_hsm() == SRSLTE_SUCCESS);

  return SRSLTE_SUCCESS;
}
