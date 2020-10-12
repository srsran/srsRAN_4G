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

#ifndef SRSLTE_FSM_H
#define SRSLTE_FSM_H

#include "srslte/adt/move_callback.h"
#include "srslte/common/logmap.h"
#include "type_utils.h"
#include <cstdio>
#include <limits>
#include <list>
#include <memory>
#include <tuple>

#define otherfsmDebug(f, fmt, ...) f->get_log()->debug("FSM \"%s\" - " fmt, get_type_name(*f).c_str(), ##__VA_ARGS__)
#define otherfsmInfo(f, fmt, ...) f->get_log()->info("FSM \"%s\" - " fmt, get_type_name(*f).c_str(), ##__VA_ARGS__)
#define otherfsmWarning(f, fmt, ...)                                                                                   \
  f->get_log()->warning("FSM \"%s\" - " fmt, get_type_name(*f).c_str(), ##__VA_ARGS__)
#define otherfsmError(f, fmt, ...) f->get_log()->error("FSM \"%s\" - " fmt, get_type_name(*f).c_str(), ##__VA_ARGS__)

#define fsmDebug(fmt, ...) otherfsmDebug(this, fmt, ##__VA_ARGS__)
#define fsmInfo(fmt, ...) otherfsmInfo(this, fmt, ##__VA_ARGS__)
#define fsmWarning(fmt, ...) otherfsmWarning(this, fmt, ##__VA_ARGS__)
#define fsmError(fmt, ...) otherfsmError(this, fmt, ##__VA_ARGS__)

namespace srslte {

//! Forward declarations
template <typename Derived>
class base_fsm_t;
template <typename Derived, typename ParentFSM>
class composite_fsm_t;

//! Check if type T is an FSM
template <typename T>
using is_fsm = std::is_base_of<base_fsm_t<T>, T>;

//! Check if type T is a composite FSM
template <typename T, typename TCheck = void>
struct is_composite_fsm : public std::false_type {};
template <typename T>
struct is_composite_fsm<T, typename std::enable_if<is_fsm<T>::value>::type> {
  const static bool value = T::is_nested;
};

namespace fsm_details {

/// check whether to log unhandled event
template <typename Event>
auto should_log_unhandled_event(const Event* ev) -> decltype(Event::log_verbose)
{
  return Event::log_verbose;
}
inline bool should_log_unhandled_event(...)
{
  return true;
}

//! Meta-function to filter transition_list<Rows...> by <Event, SrcState> types
template <class Event, class SrcState, class...>
struct filter_transition_type;
template <class Event, class SrcState, class... Rows>
struct filter_transition_type<Event, SrcState, type_list<Rows...> > {
  template <class Row>
  using predicate = typename Row::template is_match<SrcState, Event>;
  using type      = typename type_utils::filter<predicate, Rows...>::type;
};
template <class Event, class SrcState>
struct filter_transition_type<Event, SrcState, type_list<> > {
  using type = type_list<>;
};

//! Visitor to get current state's name string
struct state_name_visitor {
  template <typename State>
  void operator()(State&& s)
  {
    name = get_type_name(s);
  }

  std::string name = "invalid";
};

//! Enable/Disable SFINAE meta-function to check if <State> is part of <FSM> state list
template <typename FSM, typename State, typename T = void>
using enable_if_fsm_state = typename std::enable_if<FSM::template can_hold_state<State>(), T>::type;
template <typename FSM, typename State, typename T = void>
using disable_if_fsm_state = typename std::enable_if<not FSM::template can_hold_state<State>(), T>::type;

template <typename FSM>
using enable_if_subfsm = typename std::enable_if<is_composite_fsm<FSM>::value>::type;
template <typename FSM>
using disable_if_subfsm = typename std::enable_if<not is_composite_fsm<FSM>::value>::type;

//! Metafunction to determine if FSM can hold given State type
template <typename FSM>
using fsm_state_list_type = decltype(std::declval<typename FSM::derived_view>().states);
template <typename FSM>
using fsm_transitions = typename FSM::derived_view::transitions;

//! Detection of enter/exit methods of a state.
template <typename FSM, typename State>
auto call_enter2(FSM* f, State* s) -> decltype(s->enter(f))
{
  s->enter(f);
}
inline void call_enter2(...)
{
  // do nothing
}
template <typename FSM, typename State, typename Event>
auto call_enter(FSM* f, State* s, const Event& ev) -> decltype(s->enter(f, ev))
{
  // pass event to enter method
  s->enter(f, ev);
}
template <typename FSM, typename State, typename... Args>
inline void call_enter(FSM* f, State* s, Args&&...)
{
  call_enter2(f, s);
}
template <typename FSM, typename State>
auto call_exit(FSM* f, State* s) -> decltype(s->exit(f))
{
  s->exit(f);
}
inline void call_exit(...) {}

//! Find State in FSM recursively (e.g. find State in FSM,FSM::parentFSM,FSM::parentFSM::parentFSM,...)
template <typename State, typename FSM>
static auto get_state_recursive(FSM* f) -> enable_if_fsm_state<FSM, State, State*>
{
  return &f->states.template get_unchecked<State>();
}

template <typename FSM>
typename FSM::derived_view* get_derived(FSM* f)
{
  return static_cast<typename FSM::derived_view*>(f);
}

template <typename State, typename FSM>
static auto get_state_recursive(FSM* f) -> disable_if_fsm_state<FSM, State, State*>
{
  static_assert(FSM::is_nested, "State is not present in the FSM list of valid states");
  return get_state_recursive<State>(get_derived(f->parent_fsm()));
}

//! Helper type for FSM state-related operations
template <typename FSM, typename State>
struct state_traits {
  static_assert(FSM::template can_hold_state<State>(), "FSM type does not hold provided State\n");
  using state_t   = State;
  using is_subfsm = std::integral_constant<bool, ::srslte::is_composite_fsm<State>::value>;

  //! enter new state. enter is called recursively for subFSMs
  template <typename Event>
  static void enter_state(FSM* f, State* s, const Event& ev)
  {
    enter_(f, s, ev, is_subfsm{});
  }
  //! Change state. If DestState is not a state of FSM, call same function for parentFSM recursively
  template <typename DestState, typename Event>
  static enable_if_fsm_state<FSM, DestState> transit_state(FSM* f, const Event& ev)
  {
    call_exit(f, &f->states.template get_unchecked<State>());
    f->states.template transit<DestState>();
    state_traits<FSM, DestState>::enter_state(f, &f->states.template get_unchecked<DestState>(), ev);
  }
  template <typename DestState, typename Event>
  static disable_if_fsm_state<FSM, DestState> transit_state(FSM* f, const Event& ev)
  {
    using parent_state_traits = state_traits<typename FSM::parent_t::derived_view, typename FSM::derived_t>;
    call_exit(f, &f->states.template get_unchecked<State>());
    parent_state_traits::template transit_state<DestState>(get_derived(f->parent_fsm()), ev);
  }

private:
  //! In case of State is a subFSM
  template <typename Event>
  static void enter_(FSM* f, State* s, const Event& ev, std::true_type)
  {
    using init_type = typename fsm_state_list_type<State>::init_state_t;
    // set default FSM type
    get_derived(s)->states.template transit<init_type>();
    // call FSM enter function
    call_enter(f, s, ev);
    // call initial substate enter
    state_traits<typename State::derived_view, init_type>::enter_state(
        get_derived(s), &get_derived(s)->states.template get_unchecked<init_type>(), ev);
  }
  //! In case of State is basic state
  template <typename Event>
  static void enter_(FSM* f, State* s, const Event& ev, std::false_type)
  {
    call_enter(f, s, ev);
  }
};

//! Trigger Event reaction for the first Row for which the Guard passes
template <typename FSM, typename... Types>
struct apply_first_guard_pass;

template <typename FSM, typename First, typename... Rows>
struct apply_first_guard_pass<FSM, type_list<First, Rows...> > {
  template <typename SrcState>
  static bool trigger(FSM* f, SrcState& s, const typename First::event_t& ev)
  {
    using src_state  = SrcState;
    using dest_state = typename First::dest_state_t;
    using event_type = typename First::event_t;

    bool triggered = First::react(f, s, ev);
    if (triggered) {
      // Log Transition
      if (std::is_same<src_state, dest_state>::value) {
        otherfsmInfo(static_cast<typename FSM::derived_t*>(f),
                     "Event \"%s\" triggered state \"%s\" update\n",
                     get_type_name<event_type>().c_str(),
                     get_type_name<src_state>().c_str());
      } else {
        otherfsmInfo(static_cast<typename FSM::derived_t*>(f),
                     "transition detected - %s -> %s (cause: %s)",
                     get_type_name<src_state>().c_str(),
                     get_type_name<dest_state>().c_str(),
                     get_type_name<event_type>().c_str());
        // Apply state change operations
        state_traits<FSM, src_state>::template transit_state<dest_state>(f, ev);
      }
      return true;
    }
    return apply_first_guard_pass<FSM, type_list<Rows...> >::trigger(f, s, ev);
  }
};

template <typename FSM>
struct apply_first_guard_pass<FSM, type_list<> > {
  template <typename SrcState, typename Event>
  static bool trigger(FSM* f, SrcState& s, const Event& ev)
  {
    if (should_log_unhandled_event(&ev)) {
      otherfsmDebug(static_cast<typename FSM::derived_t*>(f),
                    "unhandled event caught in state \"%s\": \"%s\"\n",
                    get_type_name<SrcState>().c_str(),
                    get_type_name<Event>().c_str());
    }
    return false;
  }
};

//! Trigger Event, that will result in a state transition
template <typename FSM, typename Event>
struct trigger_visitor {
  using event_t = typename std::decay<Event>::type;

  trigger_visitor(FSM* f_, Event&& ev_) : f(f_), ev(std::forward<Event>(ev_)) {}

  /**
   * @brief Trigger visitor callback for the current state.
   * @description tries to find an fsm::trigger method in case the current state is a subfsm. If it does not
   * find it, searches for a react(current_state&, dest_state&, event) method at the current level
   * Stores True in "result" if state changed. False otherwise
   */
  template <typename CurrentState>
  disable_if_subfsm<CurrentState> operator()(CurrentState& s)
  {
    result = call_react(s);
  }

  template <typename CurrentState>
  enable_if_subfsm<CurrentState> operator()(CurrentState& s)
  {
    // Enter here for SubFSMs
    result = s.process_event(std::forward<Event>(ev));
    if (not result) {
      result = call_react(s);
    }
  }

  template <typename SrcState>
  bool call_react(SrcState& s)
  {
    using trigger_list =
        typename filter_transition_type<event_t, SrcState, typename FSM::derived_view::transitions>::type;
    return apply_first_guard_pass<FSM, trigger_list>::trigger(f, s, ev);
  }

  FSM*  f;
  Event ev;
  bool  result = false;
};

} // namespace fsm_details

template <typename Derived>
class base_fsm_t
{
public:
  using derived_t = Derived;

  //! get access to derived protected members from the base
  class derived_view : public Derived
  {
  public:
    // propagate user fsm methods
    using Derived::states;
    using typename Derived::transitions;
  };

  template <typename... Rows>
  using transition_table = type_list<Rows...>;

  //! Params of a state transition
  template <typename SrcState,
            typename DestState,
            typename Event,
            void (Derived::*ReactFn)(SrcState&, const Event&) = nullptr,
            bool (Derived::*GuardFn)(SrcState&, const Event&) = nullptr>
  struct row {
    using src_state_t                                                   = SrcState;
    using dest_state_t                                                  = DestState;
    using event_t                                                       = Event;
    constexpr static void (Derived::*react_fn)(SrcState&, const Event&) = ReactFn;
    constexpr static bool (Derived::*guard_fn)(SrcState&, const Event&) = GuardFn;

    static bool react(derived_view* f, src_state_t& s, const event_t& ev)
    {
      if (guard_fn == nullptr or (f->*guard_fn)(s, ev)) {
        if (react_fn != nullptr) {
          (f->*react_fn)(s, ev);
        }
        return true;
      }
      return false;
    }

    template <typename SrcState2, typename Event2>
    using is_match = std::is_same<type_list<SrcState2, Event2>, type_list<src_state_t, event_t> >;
  };

  template <typename SrcState,
            typename Event,
            void (Derived::*ReactFn)(SrcState&, const Event&) = nullptr,
            bool (Derived::*GuardFn)(SrcState&, const Event&) = nullptr>
  using upd = row<SrcState, SrcState, Event, ReactFn, GuardFn>;

  template <typename DestState,
            typename Event,
            void (Derived::*ReactFn)(const Event&) = nullptr,
            bool (Derived::*GuardFn)(const Event&) = nullptr>
  struct to_state {
    using dest_state_t                                       = DestState;
    using event_t                                            = Event;
    constexpr static void (Derived::*react_fn)(const Event&) = ReactFn;
    constexpr static bool (Derived::*guard_fn)(const Event&) = GuardFn;

    template <typename SrcState>
    static bool react(derived_view* f, SrcState& s, const event_t& ev)
    {
      if (guard_fn == nullptr or (f->*guard_fn)(ev)) {
        if (react_fn != nullptr) {
          (f->*react_fn)(ev);
        }
        return true;
      }
      return false;
    }

    template <typename SrcState2, typename Event2>
    using is_match = std::is_same<Event2, event_t>;
  };

  //! Struct used to store FSM states
  template <typename... States>
  struct state_list : public std::tuple<States...> {
    using tuple_base_t = std::tuple<States...>;
    using init_state_t = typename std::decay<decltype(std::get<0>(std::declval<tuple_base_t>()))>::type;
    static_assert(not type_list_contains<Derived, States...>(), "An FSM cannot contain itself as state\n");

    template <typename... Args>
    state_list(base_fsm_t<Derived>* f, Args&&... args) : tuple_base_t(std::forward<Args>(args)...)
    {
      if (not Derived::is_nested) {
        // If Root FSM, call initial state enter method
        fsm_details::state_traits<derived_view, init_state_t>::enter_state(
            f->derived(), &get_unchecked<init_state_t>(), std::false_type{});
      }
    }

    template <typename State>
    bool is() const
    {
      return type_idx<State>() == current_idx;
    }

    template <typename State>
    State& get_unchecked()
    {
      return std::get<type_idx<State>()>(*this);
    }

    template <typename State>
    const State& get_unchecked() const
    {
      return std::get<type_idx<State>()>(*this);
    }

    template <typename NextState>
    void transit()
    {
      current_idx = type_idx<NextState>();
    }

    template <typename State>
    constexpr static bool can_hold_type()
    {
      return srslte::type_list_contains<State, States...>();
    }

    template <typename State>
    constexpr static size_t type_idx()
    {
      return get_type_index<State, States...>();
    }

    size_t get_type_idx() const { return current_idx; }

  private:
    size_t current_idx = 0;
  };

  template <typename State>
  bool is_in_state() const
  {
    return derived()->states.template is<State>();
  }

  template <typename State>
  const State* get_if_current_state() const
  {
    return is_in_state<State>() ? get_state<State>() : nullptr;
  }

  template <typename State>
  State* get_state()
  {
    return &derived()->states.template get_unchecked<State>();
  }

  template <typename State>
  const State* get_state() const
  {
    return &derived()->states.template get_unchecked<State>();
  }

  std::string current_state_name() const
  {
    fsm_details::state_name_visitor visitor{};
    srslte::visit(visitor, derived()->states);
    return visitor.name;
  }

  //! Static method to check if State belongs to the list of possible states
  template <typename State>
  constexpr static bool can_hold_state()
  {
    return fsm_details::fsm_state_list_type<base_fsm_t<Derived> >::template can_hold_type<State>();
  }

protected:
  // Access to CRTP derived class
  derived_view* derived() { return static_cast<derived_view*>(this); }

  const derived_view* derived() const { return static_cast<const derived_view*>(this); }

  template <typename Ev>
  bool process_event(Ev&& e)
  {
    fsm_details::trigger_visitor<derived_view, Ev> visitor{derived(), std::forward<Ev>(e)};
    srslte::visit(visitor, derived()->states);
    return visitor.result;
  }
};

template <typename Derived, typename ParentFSM>
class composite_fsm_t;

//! CRTP Class for all non-nested FSMs
template <typename Derived>
class fsm_t : public base_fsm_t<Derived>
{
protected:
  using base_t = fsm_t<Derived>;
  template <typename SubFSM>
  using subfsm_t = composite_fsm_t<SubFSM, Derived>;

public:
  static const bool is_nested = false;

  explicit fsm_t(srslte::log_ref log_) : log_h(log_) {}

  // Push Events to FSM
  template <typename Ev>
  bool trigger(Ev&& e)
  {
    if (trigger_locked) {
      scheduled_event(std::forward<Ev>(e), typename std::is_lvalue_reference<Ev>::type{});
      return false;
    }
    trigger_locked = true;
    bool ret       = process_event(std::forward<Ev>(e));
    while (not pending_events.empty()) {
      pending_events.front()();
      pending_events.pop_front();
    }
    trigger_locked = false;
    return ret;
  }

  void set_fsm_event_log_level(srslte::LOG_LEVEL_ENUM e) { fsm_event_log_level = e; }

  srslte::log_ref get_log() const { return log_h; }

  bool is_trigger_locked() const { return trigger_locked; }

  //! Log FSM activity method, e.g. state transitions
  template <typename... Args>
  void log_fsm_activity(const char* format, Args&&... args)
  {
    switch (fsm_event_log_level) {
      case LOG_LEVEL_DEBUG:
        log_h->debug(format, std::forward<Args>(args)...);
        break;
      case LOG_LEVEL_INFO:
        log_h->info(format, std::forward<Args>(args)...);
        break;
      case LOG_LEVEL_WARNING:
        log_h->warning(format, std::forward<Args>(args)...);
        break;
      case LOG_LEVEL_ERROR:
        log_h->error(format, std::forward<Args>(args)...);
        break;
      default:
        break;
    }
  }

protected:
  using base_fsm_t<Derived>::derived;
  using base_fsm_t<Derived>::process_event;

  template <typename Ev>
  void scheduled_event(Ev&& e, std::true_type t)
  {
    pending_events.emplace_back([this, e]() { process_event(e); });
  }
  template <typename Ev>
  void scheduled_event(Ev&& e, std::false_type t)
  {
    pending_events.emplace_back(std::bind([this](Ev& e) { process_event(std::move(e)); }, std::move(e)));
  }

  srslte::log_ref                            log_h;
  srslte::LOG_LEVEL_ENUM                     fsm_event_log_level = LOG_LEVEL_INFO;
  bool                                       trigger_locked      = false;
  std::deque<srslte::move_callback<void()> > pending_events;
};

template <typename Derived, typename ParentFSM>
class composite_fsm_t : public base_fsm_t<Derived>
{
public:
  using base_t                = composite_fsm_t<Derived, ParentFSM>;
  using parent_t              = ParentFSM;
  static const bool is_nested = true;

  explicit composite_fsm_t(ParentFSM* parent_fsm_) : fsm_ptr(parent_fsm_) {}
  composite_fsm_t(composite_fsm_t&&) noexcept = default;
  composite_fsm_t& operator=(composite_fsm_t&&) noexcept = default;

  // Get pointer to outer FSM in case of HFSM
  const parent_t* parent_fsm() const { return fsm_ptr; }

  parent_t* parent_fsm() { return fsm_ptr; }

  srslte::log_ref get_log() const { return parent_fsm()->get_log(); }

  // Push Events to root FSM
  template <typename Ev>
  bool trigger(Ev&& e)
  {
    return parent_fsm()->trigger(std::forward<Ev>(e));
  }

  // Push events to this subFSM
  using base_fsm_t<Derived>::process_event;

protected:
  using parent_fsm_t = ParentFSM;

  ParentFSM* fsm_ptr = nullptr;
};

/**************************
 *    Procedure FSM
 *************************/

template <typename T>
struct proc_launch_ev {
  T args;
};

template <typename Result>
struct proc_complete_ev {
  Result result;
};

struct failure_ev {};

template <typename Derived, typename Result = bool>
class proc_fsm_t : public fsm_t<Derived>
{
  using fsm_type = Derived;
  using fsm_t<Derived>::derived;

protected:
  using fsm_t<Derived>::log_h;

public:
  using base_t = proc_fsm_t<Derived, Result>;
  using fsm_t<Derived>::trigger;

  // events
  template <typename Arg>
  using launch_ev   = srslte::proc_launch_ev<Arg>;
  using complete_ev = srslte::proc_complete_ev<Result>;

  // states
  struct idle_st {
    void enter(Derived* f)
    {
      if (f->launch_counter > 0) {
        f->log_h->warning(
            "FSM \"%s\": No result was set for run no. %d\n", get_type_name<Derived>().c_str(), f->launch_counter);
      }
    }

    void enter(Derived* f, const complete_ev& ev)
    {
      f->log_h->info("FSM \"%s\": Finished run no. %d\n", get_type_name<Derived>().c_str(), f->launch_counter);
      f->last_result = ev.result;
      for (auto& func : f->listening_fsms) {
        func(ev);
      }
      f->listening_fsms.clear();
    }

    void exit(Derived* f)
    {
      f->launch_counter++;
      f->log_h->info("FSM \"%s\": Starting run no. %d\n", get_type_name<Derived>().c_str(), f->launch_counter);
    }
  };

  explicit proc_fsm_t(srslte::log_ref log_) : fsm_t<Derived>(log_) {}

  bool is_running() const { return not base_t::template is_in_state<idle_st>(); }

  const Result& get_result() const
  {
    if (launch_counter > 0 and base_t::template is_in_state<idle_st>()) {
      return last_result;
    }
    THROW_BAD_ACCESS("in proc_fsm_t::get_result");
  }

  template <typename OtherFSM>
  void await(OtherFSM* f)
  {
    if (is_running()) {
      listening_fsms.push_back([f](const complete_ev& ev) { return f->trigger(ev); });
    } else {
      f->trigger(last_result);
    }
  }

private:
  int    launch_counter = 0;
  Result last_result    = {};

  std::vector<std::function<void(const complete_ev& ev)> > listening_fsms;
};

template <typename ProcFSM>
class proc_wait_st
{
public:
  explicit proc_wait_st(ProcFSM* proc_ptr_) : proc_ptr(proc_ptr_) {}

  template <typename FSM, typename Ev>
  void enter(FSM* f, const Ev& ev)
  {
    if (proc_ptr->is_running()) {
      f->get_log()->error("Unable to launch proc1\n");
      f->trigger(typename ProcFSM::complete_ev{false});
    }
    proc_ptr->trigger(srslte::proc_launch_ev<Ev>{ev});
    proc_ptr->await(f);
  }

private:
  ProcFSM* proc_ptr = nullptr;
};

/**************************************
 *      Event Trigger Scheduling
 *************************************/

template <typename Event>
struct event_callback {
  event_callback() = default;
  template <typename FSM>
  explicit event_callback(FSM* f)
  {
    callback = [f](const Event& ev) { f->trigger(ev); };
  }

  void operator()(const Event& ev) { callback(ev); }
  void operator()(const Event& ev) const { callback(ev); }

  srslte::move_task_t to_move_task(const Event& ev)
  {
    auto& copied_callback = callback;
    return [copied_callback, ev]() { copied_callback(ev); };
  }

  std::function<void(const Event&)> callback;
};

template <typename Event>
srslte::move_task_t make_move_task(const event_callback<Event>& callback, const Event& ev)
{
  auto& copied_callback = callback;
  return [copied_callback, ev]() { copied_callback(ev); };
}

template <typename Event>
srslte::move_task_t make_move_task(std::vector<event_callback<Event> >&& callbacks, const Event& ev)
{
  return std::bind(
      [ev](const std::vector<event_callback<Event> >& callbacks) {
        for (const auto& callback : callbacks) {
          callback(ev);
        }
      },
      std::move(callbacks));
}

} // namespace srslte

#endif // SRSLTE_FSM_H
