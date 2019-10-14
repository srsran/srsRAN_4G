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
 * class: proc_impl_t
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
class proc_impl_t
{
public:
  proc_impl_t() : proc_state(proc_state_t::inactive) {}
  virtual ~proc_impl_t()        = default;
  virtual proc_outcome_t step() = 0;
  virtual void           stop() {} // may be overloaded

  bool run()
  {
    proc_outcome_t outcome = proc_outcome_t::repeat;
    while (is_running() and outcome == proc_outcome_t::repeat) {
      outcome = step();
      if (outcome == proc_outcome_t::error) {
        set_proc_state(proc_state_t::error);
      } else if (outcome == proc_outcome_t::success) {
        set_proc_state(proc_state_t::success);
      }
    }
    return is_running();
  }

  void set_proc_state(proc_state_t new_state)
  {
    proc_state = new_state;
    if (proc_state == proc_state_t::error or proc_state == proc_state_t::success) {
      stop();
    }
  }
  bool is_error() const { return proc_state == proc_state_t::error; }
  bool is_success() const { return proc_state == proc_state_t::success; }
  bool is_running() const { return proc_state == proc_state_t::on_going; }
  bool is_complete() const { return is_success() or is_error(); }
  bool is_active() const { return proc_state != proc_state_t::inactive; }

private:
  proc_state_t proc_state;
};

/**************************************************************************************
 * class: proc_t<T>
 * Handles the lifetime, of a procedure T that derives from proc_impl_t, including
 * its alloc, initialization, and reset back to initial state once the procedure has been
 * completed and the user has extracted its results.
 * Can only be re-launched when a procedure T becomes inactive.
 * It uses a unique_ptr<T> to allow the use of procedures that are forward declared.
 * It provides the following methods:
 * - run() - calls proc_impl_t::run(). See above.
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
class proc_t
{
public:
  explicit proc_t() : proc_impl_ptr(new T()) {}
  T*   get() { return proc_impl_ptr.get(); }
  bool is_active() const { return proc_impl_ptr->is_active(); }
  bool is_complete() const { return proc_impl_ptr->is_complete(); }
  T*   release() { return proc_impl_ptr.release(); }
  bool run() { return proc_impl_ptr->run(); }
  void clear()
  {
    // Destructs the current object, and calls default ctor (which sets proc back to inactive and ready for another run)
    proc_impl_ptr->~T();
    new (proc_impl_ptr.get()) T();
  }

  template <class Event>
  void trigger_event(Event&& e)
  {
    if (proc_impl_ptr->is_running()) {
      proc_outcome_t outcome = proc_impl_ptr->trigger_event(std::forward<Event>(e));
      if (outcome == proc_outcome_t::error) {
        proc_impl_ptr->set_proc_state(proc_state_t::error);
      } else if (outcome == proc_outcome_t::success) {
        proc_impl_ptr->set_proc_state(proc_state_t::success);
      }
    }
  }

  T pop()
  {
    if (not proc_impl_ptr->is_complete()) {
      return T();
    }
    T ret(std::move(*proc_impl_ptr));
    clear();
    return ret;
  }

  template <class... Args>
  bool launch(Args&&... args)
  {
    if (is_active()) {
      // if already active
      return false;
    }
    proc_impl_ptr->set_proc_state(proc_state_t::on_going);
    proc_outcome_t init_ret = proc_impl_ptr->init(std::forward<Args>(args)...);
    switch (init_ret) {
      case proc_outcome_t::error:
        proc_impl_ptr->set_proc_state(proc_state_t::error); // call stop as an error
        clear();
        return false;
      case proc_outcome_t::success:
        proc_impl_ptr->set_proc_state(proc_state_t::success);
        break;
      case proc_outcome_t::repeat:
        run(); // call run right away
        break;
      case proc_outcome_t::yield:
        break;
    }
    return true;
  }

private:
  std::unique_ptr<T> proc_impl_ptr;
};

/**************************************************************************************
 * class: func_proc_t
 * A proc_impl_t used to store lambda functions and other function pointers as a step()
 * method, avoiding this way, always having to create a new class per procedure.
 ************************************************************************************/
class func_proc_t : public proc_impl_t
{
public:
  proc_outcome_t init(std::function<proc_outcome_t()> step_func_)
  {
    step_func = std::move(step_func_);
    return proc_outcome_t::yield;
  }

  proc_outcome_t step() final { return step_func(); }

private:
  std::function<proc_outcome_t()> step_func;
};

/**************************************************************************************
 * class: func_proc_t
 * A helper proc_impl_t whose step()/stop() are no op, but has a trigger_event() that
 * signals that the method has finished and store a result of type OutcomeType.
 ************************************************************************************/
template <class OutcomeType>
class query_proc_t : public proc_impl_t
{
public:
  proc_outcome_t init() { return proc_outcome_t::yield; }
  proc_outcome_t step() final { return proc_outcome_t::yield; }

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
 * class: callback_list_t
 * Stores procedures that derive from proc_impl_t. Its run() method calls sequentially
 * all the stored procedures run() method, and removes the procedures if they have
 * completed.
 * There are different ways to add a procedure to the list:
 * - add_proc(...) - adds a proc_t<T>, and once the procedure has completed, takes it
 *                   out of the container without resetting it back to its initial state
 *                   or deleting. This is useful, if the user wants to extract the
 *                   procedure result via proc_t<T>::pop()
 * - consume_proc(...) - receives a proc_t<T> as a rvalue, and calls the proc_t<T>
 *                       destructor once the procedure has ended. Useful, for procedures
 *                       for which the user is not interested in the result, or reusing
 * - defer_proc(...) - same as add_proc(...), but once the procedure has finished, it
 *                     automatically sets the procedure back to its initial state.
 *                     Useful if the user is not interested in handling the result
 * - defer_task(...) - same as consume_proc(...) but takes a function pointer that
 *                     specifies a proc_impl_t step() function
 ************************************************************************************/
class callback_list_t
{
public:
  typedef std::function<void(proc_impl_t*)>            proc_deleter_t;
  typedef std::unique_ptr<proc_impl_t, proc_deleter_t> callback_obj_t;
  template <class T>
  struct recycle_deleter_t {
    void operator()(proc_impl_t* p)
    {
      if (p != nullptr) {
        T* Tp = static_cast<T*>(p);
        Tp->~T();
        new (Tp) T();
      }
    }
  };

  template <class T>
  void add_proc(proc_t<T>& proc)
  {
    if (proc.is_complete()) {
      return;
    }
    callback_obj_t ptr(proc.get(), [](proc_impl_t* p) { /* do nothing */ });
    callbacks.push_back(std::move(ptr));
  }

  template <class T>
  void consume_proc(proc_t<T>&& proc)
  {
    if (proc.is_complete()) {
      return;
    }
    callback_obj_t ptr(proc.release(), std::default_delete<proc_impl_t>());
    callbacks.push_back(std::move(ptr));
  }

  template <class T>
  void defer_proc(proc_t<T>& proc)
  {
    if (proc.is_complete()) {
      proc.pop();
      return;
    }
    callback_obj_t ptr(proc.get(), recycle_deleter_t<T>());
    callbacks.push_back(std::move(ptr));
  }

  bool defer_task(std::function<proc_outcome_t()> step_func)
  {
    proc_t<func_proc_t> proc;
    if (not proc.launch(std::move(step_func))) {
      return false;
    }
    consume_proc(std::move(proc));
    return true;
  }

  void run()
  {
    // Calls run for all callbacks. Remove the ones that have finished. The proc dtor is called.
    callbacks.remove_if([](callback_obj_t& elem) { return not elem->run(); });
  }

  size_t size() const { return callbacks.size(); }

private:
  std::list<callback_obj_t> callbacks;
};

} // namespace srslte

#endif // SRSLTE_RESUMABLE_PROCEDURES_H
