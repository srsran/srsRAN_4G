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

#include "srslte/common/stack_procedure.h"
#include <iostream>

#define TESTASSERT(cond)                                                                                               \
  {                                                                                                                    \
    if (!(cond)) {                                                                                                     \
      std::cout << "[" << __FUNCTION__ << "][Line " << __LINE__ << "]: FAIL at " << (#cond) << std::endl;              \
      return -1;                                                                                                       \
    }                                                                                                                  \
  }

using srslte::proc_outcome_t;

enum class obj_state_t { default_ctor, move_ctor, copy_ctor, from_move_ctor, from_move_assign };

const char* to_string(obj_state_t o)
{
  switch (o) {
    case obj_state_t::default_ctor:
      return "default_ctor";
    case obj_state_t::move_ctor:
      return "move_ctor";
    case obj_state_t::copy_ctor:
      return "copy_ctor";
    case obj_state_t::from_move_ctor:
      return "from_move_ctor";
    case obj_state_t::from_move_assign:
      return "from_move_assign";
  }
  return "";
}

class TestObj
{
public:
  TestObj() = default;
  TestObj(const TestObj& other)
  {
    printf("TestObj copy ctor called: {%s,%d} <- {%s,%d}!!!\n", to_string(state), id, to_string(other.state), other.id);
    id       = other.id;
    state    = obj_state_t::copy_ctor;
    counters = other.counters;
    copy_counter++;
  }
  TestObj(TestObj&& other) noexcept
  {
    printf("TestObj move ctor called: {%s,%d} <- {%s,%d}!!!\n", to_string(state), id, to_string(other.state), other.id);
    id          = other.id;
    state       = obj_state_t::move_ctor;
    counters    = other.counters;
    other.state = obj_state_t::from_move_ctor;
    move_counter++;
  }
  ~TestObj()
  {
    dtor_counter++;
    printf("TestObj {%s,%d} dtor called!!!\n", to_string(state), id);
  }
  TestObj& operator=(const TestObj& other)
  {
    printf("TestObj copy operator called: {%s,%d} <- {%s,%d}!!!\n",
           to_string(state),
           id,
           to_string(other.state),
           other.id);
    id       = other.id;
    state    = other.state;
    counters = other.counters;
    copy_counter++;
    return *this;
  }
  TestObj& operator=(TestObj&& other) noexcept
  {
    printf("TestObj move operator called: {%s,%d} <- {%s,%d}!!!\n",
           to_string(state),
           id,
           to_string(other.state),
           other.id);
    if (&other != this) {
      id          = other.id;
      state       = other.state;
      counters    = other.counters;
      other.state = obj_state_t::from_move_assign;
      move_counter++;
    }
    return *this;
  }

  obj_state_t state = obj_state_t::default_ctor;
  int         id    = 0;
  struct stats_t {
    int then_counter  = 0;
    int reset_counter = 0;
  };
  mutable stats_t counters;
  static int      copy_counter;
  static int      move_counter;
  static int      dtor_counter;
};

int TestObj::copy_counter = 0;
int TestObj::move_counter = 0;
int TestObj::dtor_counter = 0;

void new_test()
{
  TestObj::copy_counter = 0;
  TestObj::move_counter = 0;
  TestObj::dtor_counter = 0;
}

class custom_proc
{
public:
  custom_proc() : ctor_value(5) {}

  proc_outcome_t init(int a_)
  {
    if (a_ < 0) {
      printf("Failed to initiate custom_proc\n");
      return proc_outcome_t::error;
    }
    obj.id = a_;
    return proc_outcome_t::yield;
  }
  proc_outcome_t step()
  {
    if (counter++ > 5) {
      return proc_outcome_t::success;
    }
    return proc_outcome_t::yield;
  }
  void then(const srslte::proc_result_t<int>& result) const
  {
    printf("TestObj %d then() was called\n", obj.id);
    obj.counters.then_counter++;
  }

  const char* name() const { return "custom proc"; }
  void        clear()
  {
    reset_called = true;
    printf("TestObj was reset\n");
  }
  int get_result() const { return obj.id; }

  TestObj   obj;
  const int ctor_value   = 5;
  bool      reset_called = false;

private:
  int counter = 0;
};

class custom_proc2_t
{
public:
  proc_outcome_t init()
  {
    exit_val  = "init";
    event_val = "";
    counter   = 0;
    return proc_outcome_t::yield;
  }
  proc_outcome_t step()
  {
    if (counter++ > 5) {
      exit_val = "success";
      return proc_outcome_t::success;
    }
    return proc_outcome_t::yield;
  }
  // trigger itf
  struct event_t {
    std::string event_val;
  };
  proc_outcome_t react(const event_t& event)
  {
    event_val = event.event_val;
    return proc_outcome_t::yield;
  }

  std::string get_result() const { return exit_val; }

  std::string exit_val  = "";
  std::string event_val = "";
  int         counter   = 0;
};

static_assert(std::is_same<typename srslte::proc_t<custom_proc, int>::result_type, int>::value,
              "Failed derivation of result type");
static_assert(std::is_same<typename srslte::proc_t<custom_proc, std::string>::result_type, std::string>::value,
              "Failed derivation of result type");

int test_local_1()
{
  /*
   * Description: Test if a procedure is cleaned automatically after its lifetime has ended
   */
  new_test();
  printf("\n--- Test %s ---\n", __func__);
  srslte::proc_t<custom_proc, int> proc;
  TESTASSERT(proc.is_idle() and not proc.is_busy())

  proc.launch(1);
  TESTASSERT(not proc.is_idle() and proc.is_busy())
  TESTASSERT(not proc.get()->reset_called)

  while (proc.run()) {
  }
  TESTASSERT(proc.is_idle() and not proc.is_busy())
  TESTASSERT(proc.get()->obj.counters.then_counter == 1)
  TESTASSERT(proc.get()->reset_called) // Proc is ready to be reused

  const custom_proc& procobj = *proc.get();
  TESTASSERT(procobj.obj.id == 1)
  TESTASSERT(not proc.is_busy() and proc.is_idle())
  TESTASSERT(proc.get()->ctor_value == 5)

  printf("EXIT\n");
  TESTASSERT(TestObj::copy_counter == 0);
  TESTASSERT(TestObj::move_counter == 0);
  TESTASSERT(TestObj::dtor_counter == 0); // destructor not called yet
  return 0;
}

int test_callback_1()
{
  /*
   * Description: Test a procedure inserted in a manager list via "proc_manager_list_t::add_proc(...)"
   *              - check if the proc is cleared automatically after it finished
   *              - check if the proc_future value is correctly updated
   *              - check if creating a new future does not affect previous one
   */
  new_test();
  printf("\n--- Test %s ---\n", __func__);
  srslte::proc_manager_list_t      callbacks;
  srslte::proc_t<custom_proc, int> proc;
  TESTASSERT(not proc.is_busy() and proc.is_idle())

  TESTASSERT(proc.launch(2))
  callbacks.add_proc(proc); // We have to call pop() explicitly to take the result
  TESTASSERT(callbacks.size() == 1)
  srslte::proc_future_t<int> proc_fut = proc.get_future();

  while (callbacks.size() > 0) {
    TESTASSERT(not proc_fut.is_complete())
    TESTASSERT(proc.is_busy())
    callbacks.run();
  }
  TESTASSERT(proc.is_idle());
  TESTASSERT(proc_fut.is_success() and *proc_fut.value() == 2)
  TESTASSERT(proc.get()->obj.id == 2)
  TESTASSERT(proc.get()->obj.counters.then_counter == 1)
  TESTASSERT(proc.get()->reset_called) // Proc is ready to be reused

  srslte::proc_future_t<int> proc_fut2 = proc.get_future();
  TESTASSERT(not proc_fut2.is_complete() and proc_fut.is_complete())

  printf("EXIT\n");
  TESTASSERT(TestObj::copy_counter == 0);
  TESTASSERT(TestObj::move_counter == 0);
  TESTASSERT(TestObj::dtor_counter == 0); // handler not yet destructed
  return 0;
}

int test_callback_2()
{
  /*
   * Description: Test a procedure inserted in a manager list as an r-value
   *              - check if the proc disappears automatically after it finished
   */
  new_test();
  printf("\n--- Test %s ---\n", __func__);
  srslte::proc_manager_list_t      callbacks;
  srslte::proc_t<custom_proc, int> proc;
  TESTASSERT(not proc.is_busy());
  srslte::proc_future_t<int> fut = proc.get_future();
  TESTASSERT(fut.is_empty());

  TESTASSERT(proc.launch(&fut, 3));
  TESTASSERT(proc.is_busy());
  callbacks.add_proc(std::move(proc));
  TESTASSERT(callbacks.size() == 1);

  while (callbacks.size() > 0) {
    callbacks.run();
  }
  TESTASSERT(fut.is_success() and *fut.value() == 3)

  printf("EXIT\n");
  TESTASSERT(TestObj::copy_counter == 0);
  TESTASSERT(TestObj::move_counter == 0); // it does not move proc itself, but its pointer
  TESTASSERT(TestObj::dtor_counter == 1); // handler not yet destructed, but we called proc move
  return 0;
}

int test_callback_3()
{
  /*
   * Description: Test for Lambda procedure types
   *              - test if a lambda that we passed decrements a counter correctly
   *              - test if when the lambda goes out of scope, procedure still works fine
   */
  new_test();
  printf("\n--- Test %s ---\n", __func__);
  srslte::proc_manager_list_t callbacks;
  int*                        counter = new int(5);

  {
    callbacks.add_task([counter]() {
      printf("current counter=%d\n", *counter);
      if (--(*counter) == 0) {
        return proc_outcome_t::success;
      }
      return proc_outcome_t::yield;
    });
  }

  while (callbacks.size() > 0) {
    callbacks.run();
  }

  TESTASSERT(*counter == 0);
  delete counter;

  return 0;
}

int test_callback_4()
{
  /*
   * Description: Test if finished procedure does not get added to the dispatch list
   */
  new_test();
  printf("\n--- Test %s ---\n", __func__);
  srslte::proc_manager_list_t      callbacks;
  srslte::proc_t<custom_proc, int> proc;
  TESTASSERT(proc.launch(5));
  while (proc.run()) {
    TESTASSERT(proc.is_busy());
  }
  TESTASSERT(proc.is_idle());
  TESTASSERT(proc.get()->obj.counters.then_counter == 1)
  TESTASSERT(proc.get()->reset_called);
  callbacks.add_proc(proc);
  TESTASSERT(callbacks.size() == 0); // do not add finished callbacks

  return 0;
}

int test_complete_callback_1()
{
  /*
   * Description: Test if then() callbacks are correctly called
   */
  printf("\n--- Test %s ---\n", __func__);
  srslte::proc_manager_list_t                 callbacks;
  srslte::proc_t<custom_proc2_t, std::string> proc;

  std::string run_result;
  auto        continuation_task = [&run_result](const srslte::proc_result_t<std::string>& e) {
    run_result = e.is_success() ? "SUCCESS" : "FAILURE";
  };
  const std::string results[] = {"", "SUCCESS", "", "SUCCESS", "SUCCESS", "SUCCESS"};
  for (uint32_t i = 0; i < 6; ++i) {
    run_result = "";
    if (i == 1) {
      TESTASSERT(proc.then(continuation_task) == 0)
    } else if (i == 3) {
      TESTASSERT(proc.then_always(continuation_task) == 0)
    }

    srslte::proc_future_t<std::string> fut;
    TESTASSERT(proc.launch(&fut));
    TESTASSERT(proc.get()->exit_val == "init")
    while (proc.run()) {
      TESTASSERT(proc.get()->exit_val == "init")
      TESTASSERT(proc.is_busy())
    }
    TESTASSERT(proc.is_idle() and proc.get()->exit_val == "success")
    TESTASSERT(fut.is_success() and *fut.value() == "success")

    TESTASSERT(run_result == results[i])
  }
  return 0;
}

int test_event_handler_1()
{
  /*
   * Description: Test if event handler calls trigger for multiple procedures
   */
  printf("\n--- Test %s ---\n", __func__);
  srslte::proc_t<custom_proc2_t, std::string>      proc, proc2;
  srslte::event_handler_t<custom_proc2_t::event_t> ev_handler;

  TESTASSERT(proc.launch())
  TESTASSERT(proc2.launch())
  TESTASSERT(proc.is_busy() and proc2.is_busy())
  TESTASSERT(proc.get()->exit_val == "init" and proc2.get()->exit_val == "init")
  TESTASSERT(proc.get()->event_val.empty() and proc2.get()->event_val.empty())

  ev_handler.on_next_trigger(proc);
  ev_handler.on_every_trigger(proc2);

  ev_handler.trigger(custom_proc2_t::event_t{"event1"});
  TESTASSERT(proc.get()->event_val == "event1" and proc.get()->event_val == "event1")
  ev_handler.trigger(custom_proc2_t::event_t{"event2"});
  TESTASSERT(proc.get()->event_val == "event1" and proc2.get()->event_val == "event2")

  printf("Procedures correctly triggered by event handler\n");

  return 0;
}

int main()
{
  TESTASSERT(test_local_1() == 0)
  TESTASSERT(test_callback_1() == 0)
  TESTASSERT(test_callback_2() == 0)
  TESTASSERT(test_callback_3() == 0)
  TESTASSERT(test_callback_4() == 0)
  TESTASSERT(test_complete_callback_1() == 0)
  TESTASSERT(test_event_handler_1() == 0)

  std::cout << "\n---------------\nResult: Success\n";

  return 0;
}
