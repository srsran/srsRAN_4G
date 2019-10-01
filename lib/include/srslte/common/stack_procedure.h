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

#include <functional>
#include <list>
#include <memory>
#include <mutex>

#ifndef SRSLTE_RESUMABLE_PROCEDURES_H
#define SRSLTE_RESUMABLE_PROCEDURES_H

namespace srslte {

enum class proc_state_t { on_going, success, error, inactive };
enum class proc_outcome_t { repeat, yield, success, error };

/**************************************************************************************
 * helper functions for overloading
 ************************************************************************************/
namespace detail {
template <class T>
auto optional_stop(T* obj, int is_success) -> decltype(obj->stop())
{
  obj->stop(is_success);
}

inline auto optional_stop(...) -> void
{
  // do nothing
}

template <class T>
auto optional_clear(T* obj) -> decltype(obj->clear())
{
  obj->clear();
}

inline auto optional_clear(...) -> void
{
  // do nothing
}
} // namespace detail

/**************************************************************************************
 * class: proc_itf_t
 * Provides an polymorphic interface for resumable procedures. This base can then be used
 * by a task dispatch queue via the method "run()".
 * Every procedure starts in inactive state, and finishes with success or error.
 * With methods:
 * - run() - executes a procedure, returning true if the procedure is still running
 *           or false, if it has completed
 * - step() - method overriden by child class that will be called by run(). step()
 *            executes a procedure "action" based on its current internal state,
 *            and return a proc_outcome_t variable with possible values:
 *            - yield - the procedure performed the action but hasn't completed yet.
 *            - repeat - the same as yield, but explicitly asking that run() should
 *            recall step() again (probably the procedure state has changed)
 *            - error - the procedure has finished unsuccessfully
 *            - success - the procedure has completed successfully
 * - stop() - called automatically when a procedure has finished. Useful for actions
 *            upon procedure completion, like sending back a response.
 * - set_proc_state() / is_#() - setter and getters for current procedure state
 ************************************************************************************/
class proc_manager_itf_t
{
public:
  virtual bool run()            = 0;
  virtual ~proc_manager_itf_t() = default;
};

template <typename T>
struct proc_result_t;

/**************************************************************************************
 * class: proc_manager_t<T>
 * Manages the lifetime, of a procedure T, including its alloc, launching,
 * and reset back to initial, uninit state once the procedure has been
 * completed and the user has extracted its results.
 * Can only be re-launched when a procedure T becomes inactive.
 * It uses a unique_ptr<T> to allow the use of procedures that are forward declared.
 * It provides the following methods:
 * - run() - calls T::step() and update the procedure state
 * - launch() - initializes the procedure T by calling T::init(...). Handles the case
 *              of failed initialization, and forbids the initialization of procedures
 *              that are already active.
 * - pop() - extracts the result of the procedure if it has finished, and sets
 *           proc_t<T> back to inactive
 * - trigger_event(Event) - used for handling external events. The procedure T will
 *                          have to define a method "trigger_event(Event)" as well,
 *                          specifying how each event type should be handled.
 ************************************************************************************/
template <class T>
class proc_t final : public proc_manager_itf_t
{
public:
  template <typename... Args>
  explicit proc_t(Args&&... args) : proc_impl_ptr(new T(std::forward<Args>(args)...))
  {
  }

  bool run() override
  {
    proc_outcome_t outcome = proc_outcome_t::repeat;
    while (is_running() and outcome == proc_outcome_t::repeat) {
      outcome = proc_impl_ptr->step();
      handle_outcome(outcome);
    }
    return is_running();
  }

  void clear()
  {
    // resets procedure and sets proc_t back to inactive
    detail::optional_clear(proc_impl_ptr.get());
    proc_state = proc_state_t::inactive;
  }

  const T* get() const { return proc_impl_ptr.get(); }
  bool     is_active() const { return proc_state != proc_state_t::inactive; }
  bool     is_complete() const { return proc_state == proc_state_t::success or proc_state == proc_state_t::error; }
  T*       release() { return proc_impl_ptr.release(); }

  template <class Event>
  void trigger_event(Event&& e)
  {
    if (is_running()) {
      proc_outcome_t outcome = proc_impl_ptr->trigger_event(std::forward<Event>(e));
      handle_outcome(outcome);
    }
  }

  template <class... Args>
  bool launch(Args&&... args)
  {
    if (is_active()) {
      // if already active
      return false;
    }
    proc_state              = proc_state_t::on_going;
    proc_outcome_t init_ret = proc_impl_ptr->init(std::forward<Args>(args)...);
    switch (init_ret) {
      case proc_outcome_t::error:
        handle_outcome(init_ret);
        clear();
        return false;
      case proc_outcome_t::success:
        handle_outcome(init_ret);
        // does not reset, and returns true
        break;
      case proc_outcome_t::repeat:
        run(); // call run right away
        break;
      case proc_outcome_t::yield:
        break;
    }
    return true;
  }

  proc_result_t<T> pop();

protected:
  friend proc_result_t<T>;
  bool is_running() const { return proc_state == proc_state_t::on_going; }
  void handle_outcome(proc_outcome_t outcome)
  {
    switch (outcome) {
      case proc_outcome_t::error:
        proc_state = proc_state_t::error;
        detail::optional_stop(proc_impl_ptr.get(), false);
        break;
      case proc_outcome_t::success:
        proc_state = proc_state_t::success;
        detail::optional_stop(proc_impl_ptr.get(), true);
        break;
      case proc_outcome_t::repeat:
        run();
        break;
      default:
        break;
    }
  }

  proc_state_t       proc_state = proc_state_t::inactive;
  std::unique_ptr<T> proc_impl_ptr;
};

template <typename T>
struct proc_result_t {
  explicit proc_result_t(proc_t<T>* parent_) : parent(parent_) {}
  ~proc_result_t()
  {
    if (parent->is_complete()) {
      parent->clear();
    }
  }
  const T* proc() const { return parent->is_complete() ? parent->proc_impl_ptr.get() : nullptr; }
  bool     is_success() const { return parent->proc_state == proc_state_t::success; }
  bool     is_error() const { return parent->proc_state == proc_state_t::error; }
  bool     is_complete() const { return parent->is_complete(); }

private:
  proc_t<T>* parent;
};

template <typename T>
proc_result_t<T> proc_t<T>::pop()
{
  return proc_result_t<T>{this};
}

/**************************************************************************************
 * class: func_proc_t
 * A proc used to store lambda functions and other function pointers as a step()
 * method, avoiding this way, always having to create a new class per procedure.
 ************************************************************************************/
class func_proc_t
{
public:
  explicit func_proc_t(std::function<proc_outcome_t()> step_func_) : step_func(std::move(step_func_)) {}
  proc_outcome_t init() { return proc_outcome_t::yield; }
  proc_outcome_t step() { return step_func(); }

private:
  std::function<proc_outcome_t()> step_func;
};

/**************************************************************************************
 * class: query_proc_t
 * A helper proc_impl_t whose step()/stop() are no op, but has a trigger_event() that
 * signals that the method has finished and store a result of type OutcomeType.
 ************************************************************************************/
template <class OutcomeType>
class query_proc_t
{
public:
  proc_outcome_t init() { return proc_outcome_t::yield; }
  proc_outcome_t step() { return proc_outcome_t::yield; }

  proc_outcome_t trigger_event(const OutcomeType& outcome_)
  {
    outcome = outcome_;
    return proc_outcome_t::success;
  }

  const OutcomeType& result() const { return outcome; }

private:
  OutcomeType outcome;
};

/**************************************************************************************
 * class: proc_manager_list_t
 * Stores procedure managers and, when run() is called, calls sequentially all
 * the stored procedures run() method, and removes the procedures if they have
 * completed.
 * There are different ways to add a procedure to the list:
 * - add_proc(...) - adds a proc_t<T>, and once the procedure has completed, takes it
 *                   out of the container without resetting it back to its initial state
 *                   or deleting. This is useful, if the user wants to extract the
 *                   procedure result after it has been taken off the manager list.
 *                   "proc" variable has to outlive its completion
 * - consume_proc(...) - receives a proc_t<T> as a rvalue, and calls the proc_t<T>
 *                       destructor once the procedure has ended. Useful, for procedures
 *                       for which the user is not interested in the result, or reusing
 * - defer_proc(...) - same as add_proc(...), but once the procedure has finished, it
 *                     automatically sets the procedure back to its initial state.
 *                     Useful if the user is not interested in handling the result
 * - defer_task(...) - same as consume_proc(...) but takes a function pointer that
 *                     specifies a proc_impl_t step() function
 ************************************************************************************/
class proc_manager_list_t
{
  using proc_deleter_t = std::function<void(proc_manager_itf_t*)>;
  using proc_obj_t     = std::unique_ptr<proc_manager_itf_t, proc_deleter_t>;

  template <typename T>
  struct recycle_deleter_t {
    void operator()(proc_manager_itf_t* p)
    {
      if (p != nullptr) {
        T* Tp = static_cast<T*>(p);
        Tp->clear();
        // just resets back to inactive, and does not dealloc
      }
    }
  };

public:
  template <typename T>
  void add_proc(proc_t<T>& proc)
  {
    if (proc.is_complete()) {
      return;
    }
    proc_obj_t ptr(&proc, [](proc_manager_itf_t* p) { /* do nothing */ });
    proc_list.push_back(std::move(ptr));
  }

  template <class T>
  void consume_proc(proc_t<T>&& proc)
  {
    if (proc.is_complete()) {
      return;
    }
    proc_obj_t ptr(new proc_t<T>(std::move(proc)), std::default_delete<proc_manager_itf_t>());
    proc_list.push_back(std::move(ptr));
  }

  template <typename T>
  void defer_proc(proc_t<T>& proc)
  {
    if (proc.is_complete()) {
      proc.clear();
      return;
    }
    proc_obj_t ptr(&proc, recycle_deleter_t<proc_t<T> >());
    proc_list.push_back(std::move(ptr));
  }

  bool defer_task(std::function<proc_outcome_t()> step_func)
  {
    proc_t<func_proc_t> proc(std::move(step_func));
    if (not proc.launch()) {
      return false;
    }
    consume_proc(std::move(proc));
    return true;
  }

  void run()
  {
    // Calls run for all callbacks. Remove the ones that have finished. The proc dtor is called.
    proc_list.remove_if([](proc_obj_t& elem) { return not elem->run(); });
  }

  size_t size() const { return proc_list.size(); }

private:
  std::list<proc_obj_t> proc_list;
};

} // namespace srslte

#endif // SRSLTE_RESUMABLE_PROCEDURES_H
