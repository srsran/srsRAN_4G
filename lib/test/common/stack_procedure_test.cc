/*
 * Copyright 2013-2019 Software Radio Systems Limited
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
    id    = other.id;
    state = obj_state_t::copy_ctor;
    copy_counter++;
  }
  TestObj(TestObj&& other) noexcept
  {
    printf("TestObj move ctor called: {%s,%d} <- {%s,%d}!!!\n", to_string(state), id, to_string(other.state), other.id);
    id          = other.id;
    state       = obj_state_t::move_ctor;
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
    id    = other.id;
    state = other.state;
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
      other.state = obj_state_t::from_move_assign;
      move_counter++;
    }
    return *this;
  }

  obj_state_t state = obj_state_t::default_ctor;
  int         id    = 0;
  static int  copy_counter;
  static int  move_counter;
  static int  dtor_counter;
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
    obj.id       = a_;
    reset_called = false;
    return proc_outcome_t::yield;
  }
  proc_outcome_t step()
  {
    if (counter++ > 5) {
      return proc_outcome_t::success;
    }
    return proc_outcome_t::yield;
  }
  void        stop(bool is_success) { printf("TestObj %d stop() was called\n", obj.id); }
  const char* name() const { return "custom proc"; }
  void        clear()
  {
    reset_called = true;
    printf("TestObj was reset\n");
  }

  TestObj   obj;
  const int ctor_value   = 5;
  bool      reset_called = false;

private:
  int counter = 0;
};

int test_local_1()
{
  new_test();
  printf("\n--- Test %s ---\n", __func__);
  srslte::proc_t<custom_proc> proc;
  TESTASSERT(not proc.is_active());

  proc.launch(1);
  TESTASSERT(proc.is_active());
  TESTASSERT(not proc.is_complete());
  TESTASSERT(not proc.get()->reset_called);

  while (proc.run()) {
  }

  TESTASSERT(proc.is_active());
  TESTASSERT(proc.is_complete());

  const custom_proc& procobj = *proc.get();
  TESTASSERT(procobj.obj.id == 1);
  TESTASSERT(proc.is_active());
  printf("clear() being called\n");
  proc.clear();
  TESTASSERT(not proc.is_active());
  TESTASSERT(proc.get()->reset_called); // Proc is ready to be reused
  TESTASSERT(proc.get()->ctor_value == 5);

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
   *              - check if the proc is not cleared automatically after it finished (need to check the result)
   *              - check if pop() works as expected, and resets proc after proc_result_t<T> goes out of scope
   */
  new_test();
  printf("\n--- Test %s ---\n", __func__);
  srslte::proc_manager_list_t         callbacks;
  srslte::proc_t<custom_proc> proc;
  TESTASSERT(not proc.is_active());

  TESTASSERT(proc.launch(2));
  callbacks.add_proc(proc); // We have to call pop() explicitly to take the result
  TESTASSERT(callbacks.size() == 1);

  while (callbacks.size() > 0) {
    TESTASSERT(proc.is_active());
    TESTASSERT(not proc.is_complete());
    callbacks.run();
  }
  TESTASSERT(proc.is_active());
  TESTASSERT(proc.is_complete());

  TESTASSERT(proc.get()->obj.id == 2);
  TESTASSERT(proc.is_active());
  {
    printf("pop being called\n");
    srslte::proc_result_t<custom_proc> ret = proc.pop();
    TESTASSERT(proc.is_active());
    TESTASSERT(ret.is_success());
    // proc::reset() is finally called
  }
  TESTASSERT(not proc.is_active());
  TESTASSERT(proc.get()->reset_called); // Proc is ready to be reused

  printf("EXIT\n");
  TESTASSERT(TestObj::copy_counter == 0);
  TESTASSERT(TestObj::move_counter == 0);
  TESTASSERT(TestObj::dtor_counter == 0); // handler not yet destructed
  return 0;
}

int test_callback_2()
{
  /*
   * Description: Test a procedure inserted in a manager list via "proc_manager_list_t::consume_proc(...)"
   *              - check if the proc disappears automatically after it finished
   */
  new_test();
  printf("\n--- Test %s ---\n", __func__);
  srslte::proc_manager_list_t         callbacks;
  srslte::proc_t<custom_proc> proc;
  TESTASSERT(not proc.is_active());

  TESTASSERT(proc.launch(3));
  TESTASSERT(proc.is_active());
  TESTASSERT(not proc.is_complete());
  callbacks.consume_proc(std::move(proc));
  TESTASSERT(callbacks.size() == 1);

  while (callbacks.size() > 0) {
    callbacks.run();
  }
  // since the proc was consumed, it is erased without the need for pop()

  printf("EXIT\n");
  TESTASSERT(TestObj::copy_counter == 0);
  TESTASSERT(TestObj::move_counter == 0); // it does not move proc itself, but its pointer
  TESTASSERT(TestObj::dtor_counter == 1); // handler not yet destructed, but we called proc move
  return 0;
}

int test_callback_3()
{
  /*
   * Description: Test a procedure inserted in a manager list via "proc_manager_list_t::defer_proc(...)"
   *              - check if the proc is cleared automatically after it finished
   */
  new_test();
  printf("\n--- Test %s ---\n", __func__);
  srslte::proc_manager_list_t         callbacks;
  srslte::proc_t<custom_proc> proc;
  TESTASSERT(not proc.is_active());

  proc.launch(4);
  TESTASSERT(proc.is_active());
  TESTASSERT(not proc.is_complete());
  callbacks.defer_proc(proc); // we still have access to proc, but we do not need to call pop()
  TESTASSERT(callbacks.size() == 1);
  TESTASSERT(proc.is_active());
  TESTASSERT(not proc.is_complete());

  while (callbacks.size() > 0) {
    TESTASSERT(proc.is_active());
    TESTASSERT(not proc.is_complete());
    callbacks.run();
  }
  TESTASSERT(not proc.is_active());
  TESTASSERT(not proc.is_complete());
  TESTASSERT(proc.get()->reset_called); // Proc is ready to be reused

  printf("EXIT\n");
  TESTASSERT(TestObj::copy_counter == 0);
  TESTASSERT(TestObj::move_counter == 0);
  TESTASSERT(TestObj::dtor_counter == 0); // handler not yet destructed
  return 0;
}

int test_callback_4()
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
    callbacks.defer_task([counter]() {
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

  //  printf("counter=%d\n", counter);
  TESTASSERT(*counter == 0);
  delete counter;

  return 0;
}

int test_callback_5()
{
  /*
   * Description: Test if finished procedure does not get added to the dispatch list
   */
  new_test();
  printf("\n--- Test %s ---\n", __func__);
  srslte::proc_manager_list_t         callbacks;
  srslte::proc_t<custom_proc> proc;
  TESTASSERT(proc.launch(5));
  while (proc.run()) {
    TESTASSERT(proc.is_active());
  }
  TESTASSERT(proc.is_active());
  TESTASSERT(not proc.get()->reset_called);
  TESTASSERT(proc.is_complete());
  callbacks.defer_proc(proc);
  TESTASSERT(callbacks.size() == 0); // do not add finished callbacks

  return 0;
}

int main()
{
  TESTASSERT(test_local_1() == 0);
  TESTASSERT(test_callback_1() == 0);
  TESTASSERT(test_callback_2() == 0);
  TESTASSERT(test_callback_3() == 0);
  TESTASSERT(test_callback_4() == 0);
  TESTASSERT(test_callback_5() == 0);

  return 0;
}
