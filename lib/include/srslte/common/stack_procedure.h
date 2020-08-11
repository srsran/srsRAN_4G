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

#include "srslte/adt/move_callback.h"
#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <vector>

#ifndef SRSLTE_RESUMABLE_PROCEDURES_H
#define SRSLTE_RESUMABLE_PROCEDURES_H

namespace srslte {

enum class proc_outcome_t { yield, success, error };

/**************************************************************************************
 * helper functions for method optional overloading
 ************************************************************************************/
namespace proc_detail {
// used by proc_t<T> to call T::then() method only if it exists
template <typename T, typename ProcResult>
auto optional_then(T* obj, const ProcResult* result) -> decltype(obj->then(*result))
{
  obj->then(*result);
}
inline auto optional_then(...) -> void
{
  // do nothing
}
// used by proc_t<T> to call proc T::clear() method only if it exists
template <class T>
auto optional_clear(T* obj) -> decltype(obj->clear())
{
  obj->clear();
}
inline auto optional_clear(...) -> void
{
  // do nothing
}
template <class T>
auto        get_result_type(const T& obj) -> decltype(obj.get_result());
inline auto get_result_type(...) -> void;
} // namespace proc_detail

/**************************************************************************************
 * class: callback_group_t<Args...>
 *        Bundles several callbacks with callable interface "void(Args...)".
 *        Calls to operator(Args&&...) call all the registered callbacks.
 *        Two methods to register a callback - call it once, or always call it.
 ************************************************************************************/

template <typename... Args>
class callback_group_t
{
public:
  using callback_id_t = uint32_t;
  using callback_t    = srslte::move_callback<void(Args...)>;

  //! register callback, that gets called once
  callback_id_t on_next_call(callback_t f_)
  {
    uint32_t idx               = get_new_callback();
    func_list[idx].func        = std::move(f_);
    func_list[idx].call_always = false;
    return idx;
  }
  callback_id_t on_every_call(callback_t f_)
  {
    uint32_t idx               = get_new_callback();
    func_list[idx].func        = std::move(f_);
    func_list[idx].call_always = true;
    return idx;
  }

  // call all callbacks
  template <typename... ArgsRef>
  void operator()(ArgsRef&&... args)
  {
    for (auto& f : func_list) {
      if (f.active) {
        f.func(std::forward<ArgsRef>(args)...);
        if (not f.call_always) {
          f.active = false;
        }
      }
    }
  }

private:
  uint32_t get_new_callback()
  {
    uint32_t i = 0;
    for (; i < func_list.size() and func_list[i].active; ++i) {
    }
    if (i == func_list.size()) {
      func_list.emplace_back();
    }
    func_list[i].active = true;
    return i;
  }

  struct call_item_t {
    bool       active;
    callback_t func;
    bool       call_always;
  };
  std::vector<call_item_t> func_list;
};

/**************************************************************************************
 * class: proc_result_t
 * Stores the result of a procedure run. Can optionally contain a value T, in case of a
 * successful run.
 **************************************************************************************/

namespace proc_detail {
struct proc_result_base_t {
  bool is_success() const { return state == result_state_t::value; }
  bool is_error() const { return state == result_state_t::error; }
  bool is_complete() const { return state != result_state_t::none; }
  void set_val() { state = result_state_t::value; }
  void set_error() { state = result_state_t::error; }
  void clear() { state = result_state_t::none; }

protected:
  enum class result_state_t { none, value, error } state = result_state_t::none;
};
} // namespace proc_detail
template <typename T>
struct proc_result_t : public proc_detail::proc_result_base_t {
  const T* value() const { return state == result_state_t::value ? &t : nullptr; }
  void     set_val(const T& t_)
  {
    proc_result_base_t::set_val();
    t = t_;
  }
  template <typename Proc>
  void extract_val(Proc& p)
  {
    set_val(p.get_result());
  }

protected:
  T t;
};
template <>
struct proc_result_t<void> : public proc_detail::proc_result_base_t {
  template <typename Proc>
  void extract_val(Proc& p)
  {
    set_val();
  }
};
// specialization for ResultType=void
using proc_state_t = proc_result_t<void>;

/**************************************************************************************
 * class: proc_future_t
 * Contains a pointer to the result of a procedure run. This pointer gets updated with
 * the actual result once the procedure completes.
 **************************************************************************************/
template <typename ResultType>
class proc_future_t
{
public:
  proc_future_t() = default;
  explicit proc_future_t(const std::shared_ptr<proc_result_t<ResultType> >& p_) : ptr(p_) {}
  bool              is_error() const { return not is_empty() and ptr->is_error(); }
  bool              is_success() const { return not is_empty() and ptr->is_success(); }
  bool              is_complete() const { return not is_empty() and ptr->is_complete(); }
  const ResultType* value() const { return is_success() ? ptr->value() : nullptr; }
  bool              is_empty() const { return ptr == nullptr; }
  void              clear() { ptr.reset(); }

private:
  std::shared_ptr<proc_result_t<ResultType> > ptr;
};
using proc_future_state_t = proc_future_t<void>;

/**************************************************************************************
 * class: proc_base_t
 * Provides a polymorphic interface for resumable procedures. This base can then be used
 * by a "proc_manager_list_t" via the virtual method "proc_base_t::run()".
 * With public methods:
 * - run() - executes proc_t<T>::step(), and updates procedure state.
 * - is_busy()/is_idle() - tells if procedure is currently running. Busy procedures
 *                         cannot be re-launched
 * - then() - called automatically when a procedure has finished. Useful for actions
 *            upon procedure completion, like sending back a response or logging.
 * With protected methods:
 * - step() - method overriden by child class that will be called by run(). step()
 *            executes a procedure "action" based on its current internal state,
 *            and return a proc_outcome_t variable with possible values:
 *            - yield - the procedure performed the action but hasn't completed yet.
 *            - error - the procedure has finished unsuccessfully
 *            - success - the procedure has completed successfully
 ************************************************************************************/
class proc_base_t
{
public:
  virtual ~proc_base_t() = default;

  //! common proc::run() interface. Returns true if procedure is still running
  bool run()
  {
    if (is_busy()) {
      proc_outcome_t outcome = step();
      handle_outcome(outcome);
    }
    return is_busy();
  }

  //! interface to check if proc is still running
  bool is_busy() const { return proc_state == proc_status_t::on_going; }
  bool is_idle() const { return proc_state == proc_status_t::idle; }

protected:
  enum class proc_status_t { idle, on_going };
  virtual proc_outcome_t step()                    = 0;
  virtual void           run_then(bool is_success) = 0;

  void handle_outcome(proc_outcome_t outcome)
  {
    if (outcome == proc_outcome_t::error or outcome == proc_outcome_t::success) {
      bool success = outcome == proc_outcome_t::success;
      run_then(success);
    }
  }

  proc_status_t proc_state = proc_status_t::idle;
};

/**************************************************************************************
 * class: proc_t<T, ResultType>
 * Manages the lifetime of a procedure of type T, including its alloc, launching,
 * and reset back to "inactive" state once the procedure has been completed.
 * The result of a procedure run is of type "proc_result_t<ResultType>". ResultType has
 * to coincide with the type returned by the method "T::get_result()".
 * There are three main ways to use the result of a procedure run:
 * - "T::then(const proc_result_t<T>&)" - method in T that runs on completion, and
 *                                        gets as argument the result of the run
 * - "proc_t<T>::get_future()" - returns a proc_future_t<T> which the user can use
 *                               directly to check the result of a run
 * - "proc_t<T>::then/then_always()" - provide dynamically a continuation task, for
 *                                     instance, by providing a lambda
 * It uses a unique_ptr<T> to allow the use of procedures that are forward declared.
 ************************************************************************************/

// Implementation of the Procedure Manager functionality, including launching, trigger events, clearing
template <class T, typename ResultType = void>
class proc_t : public proc_base_t
{
public:
  //  cannot derive automatically this type
  using result_type          = ResultType;
  using proc_result_type     = proc_result_t<result_type>;
  using proc_future_type     = proc_future_t<result_type>;
  using then_callback_list_t = callback_group_t<const proc_result_type&>;
  using callback_t           = typename then_callback_list_t::callback_t;
  using callback_id_t        = typename then_callback_list_t::callback_id_t;

  template <typename... Args>
  explicit proc_t(Args&&... args) : proc_ptr(new T(std::forward<Args>(args)...))
  {
    static_assert(std::is_same<result_type, decltype(proc_detail::get_result_type(std::declval<T>()))>::value,
                  "The types \"proc_t::result_type\" and the return of T::get_result() have to match");
  }

  const T* get() const { return proc_ptr.get(); }
  T*       release() { return proc_ptr.release(); }

  //! method to handle external events. "T" must have the method "T::react(const Event&)" for the trigger to take effect
  template <class Event>
  bool trigger(Event&& e)
  {
    if (is_busy()) {
      proc_outcome_t outcome = proc_ptr->react(std::forward<Event>(e));
      handle_outcome(outcome);
    }
    return is_busy();
  }

  //! returns an object which the user can use to check if the procedure has ended.
  proc_future_type get_future()
  {
    if (is_idle()) {
      return proc_future_type{};
    }
    if (future_result == nullptr) {
      future_result = std::make_shared<proc_result_type>();
    }
    return proc_future_type{future_result};
  }

  //! methods to schedule continuation tasks
  callback_id_t then(callback_t c) { return complete_callbacks.on_next_call(std::move(c)); }
  callback_id_t then_always(callback_t c) { return complete_callbacks.on_every_call(std::move(c)); }

  //! launch a procedure, returning true if successful or running and false if it error or it failed to launch
  template <class... Args>
  bool launch(Args&&... args)
  {
    if (is_busy()) {
      return false;
    }
    proc_state              = proc_base_t::proc_status_t::on_going;
    proc_outcome_t init_ret = proc_ptr->init(std::forward<Args>(args)...);
    handle_outcome(init_ret);
    return init_ret != proc_outcome_t::error;
  }

  //! launch a procedure, returning a future where the result is going to be saved
  template <class... Args>
  bool launch(proc_future_type* fut, Args&&... args)
  {
    if (is_busy()) {
      fut->clear();
      return false;
    }
    proc_state              = proc_base_t::proc_status_t::on_going;
    *fut                    = get_future();
    proc_outcome_t init_ret = proc_ptr->init(std::forward<Args>(args)...);
    handle_outcome(init_ret);
    return init_ret != proc_outcome_t::error;
  }

protected:
  proc_outcome_t step() final { return proc_ptr->step(); }

  void run_then(bool is_success) final
  {
    proc_state = proc_status_t::idle;
    proc_result_type result;
    // update result state
    if (is_success) {
      result.extract_val(*proc_ptr);
    } else {
      result.set_error();
    }
    // propagate proc_result to future if it exists, and release future
    if (future_result != nullptr) {
      *future_result = result;
      future_result.reset();
    }
    // call T::then() if it exists
    proc_detail::optional_then(proc_ptr.get(), &result);
    // signal continuations
    complete_callbacks(std::move(result));
    // back to inactive
    proc_detail::optional_clear(proc_ptr.get());
  }

  std::unique_ptr<T>                proc_ptr;
  std::shared_ptr<proc_result_type> future_result; //! used if get_future() itf is used.
  then_callback_list_t              complete_callbacks;
};

/**************************************************************************************
 * class: event_handler_t<Args...>
 *        Bundles several proc_managers together with same trigger(Args...) itf.
 *        Once trigger(...) is called, all registered proc_managers get triggered
 *        as well.
 ************************************************************************************/
// NOTE: Potential improvements: a method "trigger_during_this_run" that unregisters the handler
//       once the procedure run is finished.
template <typename EventType>
class event_handler_t
{
public:
  using callback_id_t = typename callback_group_t<EventType>::callback_id_t;

  template <typename Proc, typename ResultType>
  callback_id_t on_next_trigger(proc_t<Proc, ResultType>& p)
  {
    return callbacks.on_next_call([&p](EventType&& ev) { p.trigger(std::forward<EventType>(ev)); });
  }

  template <typename Proc, typename ResultType>
  callback_id_t on_every_trigger(proc_t<Proc, ResultType>& p)
  {
    return callbacks.on_every_call([&p](EventType&& ev) { p.trigger(std::forward<EventType>(ev)); });
  }

  void trigger(EventType&& ev) { callbacks(std::forward<EventType>(ev)); }

private:
  callback_group_t<EventType> callbacks;
};

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
 * class: proc_manager_list_t
 * Stores procedure managers and, when run() is called, calls sequentially all
 * the stored procedures run() method, and removes the procedures if they have
 * already completed.
 * There are two ways to add a procedure to the list:
 * - add_proc(...) - adds a proc_t<T>, and once the procedure has completed, takes it
 *                   out of the container. In case a r-value ref is passed, this class
 *                   calls its destructor.
 * - add_task(...) - same as add_proc(...) but takes a function pointer that
 *                   specifies a proc_impl_t step() function
 ************************************************************************************/
class proc_manager_list_t
{
  using proc_deleter_t = std::function<void(proc_base_t*)>;
  using proc_obj_t     = std::unique_ptr<proc_base_t, proc_deleter_t>;

public:
  template <typename T, typename ResultType>
  void add_proc(proc_t<T, ResultType>& proc)
  {
    if (proc.is_idle()) {
      return;
    }
    proc_obj_t ptr(&proc, [](proc_base_t* p) { /* do nothing */ });
    proc_list.push_back(std::move(ptr));
  }

  // since it receives a r-value, it calls the default destructor
  template <class T, typename ResultType>
  void add_proc(proc_t<T, ResultType>&& proc)
  {
    if (proc.is_idle()) {
      return;
    }
    proc_obj_t ptr(new proc_t<T, ResultType>(std::move(proc)), std::default_delete<proc_base_t>());
    proc_list.push_back(std::move(ptr));
  }

  bool add_task(std::function<proc_outcome_t()> step_func)
  {
    proc_t<func_proc_t> proc(std::move(step_func));
    if (not proc.launch()) {
      return false;
    }
    add_proc(std::move(proc));
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
