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

#include "srslte/common/logmap.h"
#include "type_utils.h"
#include <cstdio>
#include <limits>
#include <memory>
#include <tuple>

namespace srslte {

//! Forward declarations
template <typename Derived>
class fsm_t;

namespace fsm_details {

//! Meta-function to filter transition list <Rows...> by <Event, SrcState> types
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

//! Enable/Disable meta-function if <State> is part of <FSM> state list
template <typename FSM, typename State, typename T = void>
using enable_if_fsm_state = typename std::enable_if<FSM::template can_hold_state<State>(), T>::type;
template <typename FSM, typename State, typename T = void>
using disable_if_fsm_state = typename std::enable_if<not FSM::template can_hold_state<State>(), T>::type;

template <typename FSM>
constexpr bool is_fsm()
{
  return std::is_base_of<fsm_t<FSM>, FSM>::value;
}

template <typename FSM>
constexpr typename std::enable_if<is_fsm<FSM>(), bool>::type is_subfsm()
{
  return FSM::is_nested;
}

template <typename FSM>
constexpr typename std::enable_if<not is_fsm<FSM>(), bool>::type is_subfsm()
{
  return false;
}

template <typename FSM>
using enable_if_subfsm = typename std::enable_if<is_subfsm<FSM>()>::type;
template <typename FSM>
using disable_if_subfsm = typename std::enable_if<not is_subfsm<FSM>()>::type;

//! Metafunction to determine if FSM can hold given State type
template <typename FSM>
using fsm_state_list_type = decltype(std::declval<typename FSM::derived_view>().states);
template <typename FSM>
using fsm_transitions = typename FSM::derived_view::transitions;

//! Detection of enter/exit methods of a state.
template <typename FSM, typename State>
auto call_enter(FSM* f, State* s) -> decltype(s->enter(f))
{
  s->enter(f);
}
void call_enter(...) {}
template <typename FSM, typename State>
auto call_exit(FSM* f, State* s) -> decltype(s->exit(f))
{
  s->exit(f);
}
void call_exit(...) {}

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
  using is_subfsm = std::integral_constant<bool, ::srslte::fsm_details::is_subfsm<State>()>;

  //! enter new state. enter is called recursively for subFSMs
  static void enter_state(FSM* f, State* s) { enter_(f, s, is_subfsm{}); }
  //! Change state. If DestState is not a state of FSM, call same function for parentFSM recursively
  template <typename DestState>
  static enable_if_fsm_state<FSM, DestState> transit_state(FSM* f)
  {
    call_exit(f, &f->states.template get_unchecked<State>());
    f->states.template transit<DestState>();
    state_traits<FSM, DestState>::enter_state(f, &f->states.template get_unchecked<DestState>());
  }
  template <typename DestState>
  static disable_if_fsm_state<FSM, DestState> transit_state(FSM* f)
  {
    using parent_state_traits = state_traits<typename FSM::parent_t::derived_view, typename FSM::derived_t>;
    call_exit(f, &f->states.template get_unchecked<State>());
    parent_state_traits::template transit_state<DestState>(get_derived(f->parent_fsm()));
  }

private:
  //! In case of State is a subFSM
  static void enter_(FSM* f, State* s, std::true_type)
  {
    using init_type = typename fsm_state_list_type<State>::init_state_t;
    // set default FSM type
    get_derived(s)->states.template transit<init_type>();
    // call FSM enter function
    call_enter(f, s);
    // call initial substate enter
    state_traits<typename State::derived_view, init_type>::enter_state(
        get_derived(s), &get_derived(s)->states.template get_unchecked<init_type>());
  }
  //! In case of State is basic state
  static void enter_(FSM* f, State* s, std::false_type) { call_enter(f, s); }
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
        f->log_fsm_activity("FSM \"%s\": Event \"%s\" updated state \"%s\"\n",
                            get_type_name<typename FSM::derived_t>().c_str(),
                            get_type_name<event_type>().c_str(),
                            get_type_name<src_state>().c_str());
      } else {
        f->log_fsm_activity("FSM \"%s\": Transition detected - %s -> %s (cause: %s)",
                            get_type_name<typename FSM::derived_t>().c_str(),
                            get_type_name<src_state>().c_str(),
                            get_type_name<dest_state>().c_str(),
                            get_type_name<event_type>().c_str());
        // Apply state change operations
        state_traits<FSM, src_state>::template transit_state<dest_state>(f);
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
    f->log_fsm_activity("FSM \"%s\": Unhandled event caught: \"%s\"\n",
                        get_type_name<typename FSM::derived_t>().c_str(),
                        get_type_name<Event>().c_str());
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
    result = s.trigger(std::forward<Event>(ev));
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

template <typename Derived, typename ParentFSM>
class nested_fsm_t;

//! CRTP Class for all non-nested FSMs
template <typename Derived>
class fsm_t
{
protected:
  using base_t = fsm_t<Derived>;
  template <typename SubFSM>
  using subfsm_t = nested_fsm_t<SubFSM, Derived>;

public:
  //! get access to derived protected members from the base
  class derived_view : public Derived
  {
  public:
    using derived_t = Derived;
    // propagate user fsm methods
    using Derived::states;
    using typename Derived::transitions;
  };

  //! Params of a state transition
  template <typename SrcState,
            typename DestState,
            typename Event,
            void (Derived::*ReactFn)(SrcState&, DestState&, const Event&) = nullptr,
            bool (Derived::*GuardFn)(SrcState&, const Event&) const       = nullptr>
  struct row {
    using src_state_t  = SrcState;
    using dest_state_t = DestState;
    using event_t      = Event;

    constexpr static void (Derived::*react_fn)(SrcState&, DestState&, const Event&) = ReactFn;

    constexpr static bool (Derived::*guard_fn)(SrcState&, const Event&) const = GuardFn;

    static bool react(derived_view* f, src_state_t& s, const event_t& ev)
    {
      if (guard_fn == nullptr or (f->*guard_fn)(s, ev)) {
        dest_state_t* d = fsm_details::get_state_recursive<dest_state_t, derived_view>(f);
        if (react_fn != nullptr) {
          (f->*react_fn)(s, *d, ev);
        }
        return true;
      }
      return false;
    }

    template <typename SrcState2, typename Event2>
    using is_match = std::is_same<type_list<SrcState2, Event2>, type_list<src_state_t, event_t> >;
  };

  template <typename DestState,
            typename Event,
            void (Derived::*ReactFn)(DestState&, const Event&) = nullptr,
            bool (Derived::*GuardFn)(const Event&) const       = nullptr>
  struct from_any_state {
    using dest_state_t = DestState;
    using event_t      = Event;

    constexpr static void (Derived::*react_fn)(DestState&, const Event&) = ReactFn;

    constexpr static bool (Derived::*guard_fn)(const Event&) const = GuardFn;

    template <typename SrcState>
    static bool react(derived_view* f, SrcState& s, const event_t& ev)
    {
      if (guard_fn == nullptr or (f->*guard_fn)(ev)) {
        dest_state_t* d = fsm_details::get_state_recursive<dest_state_t, derived_view>(f);
        if (react_fn != nullptr) {
          (f->*react_fn)(*d, ev);
        }
        return true;
      }
      return false;
    }

    template <typename SrcState2, typename Event2>
    using is_match = std::is_same<Event2, event_t>;
  };

  template <typename... Rows>
  using transition_table = type_list<Rows...>;

  static const bool is_nested = false;

  //! Struct used to store FSM states
  template <typename... States>
  struct state_list : public std::tuple<States...> {
    using tuple_base_t = std::tuple<States...>;
    using init_state_t = typename std::decay<decltype(std::get<0>(std::declval<tuple_base_t>()))>::type;
    static_assert(not type_list_contains<Derived, States...>(), "An FSM cannot contain itself as state\n");

    template <typename... Args>
    state_list(fsm_t<Derived>* f, Args&&... args) : tuple_base_t(std::forward<Args>(args)...)
    {
      if (not Derived::is_nested) {
        // If Root FSM, call initial state enter method
        fsm_details::state_traits<derived_view, init_state_t>::enter_state(f->derived(),
                                                                           &get_unchecked<init_state_t>());
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

  explicit fsm_t(srslte::log_ref log_) : log_h(log_) {}

  // Push Events to FSM
  template <typename Ev>
  bool trigger(Ev&& e)
  {
    fsm_details::trigger_visitor<derived_view, Ev> visitor{derived(), std::forward<Ev>(e)};
    srslte::visit(visitor, derived()->states);
    return visitor.result;
  }

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
    return fsm_details::fsm_state_list_type<fsm_t<Derived> >::template can_hold_type<State>();
  }

  void set_fsm_event_log_level(srslte::LOG_LEVEL_ENUM e) { fsm_event_log_level = e; }

  srslte::log_ref get_log() const { return log_h; }

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
  // Access to CRTP derived class
  derived_view* derived() { return static_cast<derived_view*>(this); }

  const derived_view* derived() const { return static_cast<const derived_view*>(this); }

  srslte::log_ref        log_h;
  srslte::LOG_LEVEL_ENUM fsm_event_log_level = LOG_LEVEL_INFO;
};

template <typename Derived, typename ParentFSM>
class nested_fsm_t : public fsm_t<Derived>
{
public:
  using base_t                = nested_fsm_t<Derived, ParentFSM>;
  using parent_t              = ParentFSM;
  static const bool is_nested = true;

  explicit nested_fsm_t(ParentFSM* parent_fsm_) : fsm_t<Derived>(parent_fsm_->get_log()), fsm_ptr(parent_fsm_) {}

  // Get pointer to outer FSM in case of HSM
  const parent_t* parent_fsm() const { return fsm_ptr; }

  parent_t* parent_fsm() { return fsm_ptr; }

protected:
  using parent_fsm_t = ParentFSM;

  ParentFSM* fsm_ptr = nullptr;
};

// event
template <typename... Args>
struct proc_launch_ev {
  std::tuple<Args...> args;

  explicit proc_launch_ev(Args&&... args_) : args(std::forward<Args>(args_)...) {}
};

template <typename Derived, typename Result = std::true_type>
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
  struct reset_ev {};

  // states
  struct idle_st {
    idle_st() = default;
    template <typename T>
    idle_st(bool success_, T&& r) : success(success_), result(std::forward<T>(r)), value_set(true)
    {}

    void enter(Derived* f)
    {
      if (f->launch_counter > 0) {
        f->log_h->info("FSM \"%s\": Finished run no. %d %s\n",
                       get_type_name<Derived>().c_str(),
                       f->launch_counter,
                       is_success() ? "successfully" : "with an error");
        if (not is_result_set()) {
          f->log_h->error(
              "FSM \"%s\": No result was set for run no. %d\n", get_type_name<Derived>().c_str(), f->launch_counter);
        }
      }
    }

    void exit(Derived* f)
    {
      value_set = false;
      success   = false;
      f->launch_counter++;
      f->log_h->info("FSM \"%s\": Starting run no. %d\n", get_type_name<Derived>().c_str(), f->launch_counter);
    }

    bool          is_result_set() const { return value_set; }
    bool          is_success() const { return value_set and success; }
    const Result& get_result() const { return result; }

  private:
    bool   success = false, value_set = false;
    Result result = {};
  };

  explicit proc_fsm_t(srslte::log_ref log_) : fsm_t<Derived>(log_) {}

  bool is_running() const { return base_t::template is_in_state<idle_st>(); }

  bool is_success() const { return base_t::template get_state<idle_st>()->is_success(); }

  const Result& get_result() const
  {
    if (is_success()) {
      return base_t::template get_state<idle_st>->get_result();
    }
    THROW_BAD_ACCESS("in proc_fsm_t::get_result");
  }

  template <typename... Args>
  void launch(Args&&... args)
  {
    trigger(proc_launch_ev<Args...>(std::forward<Args>(args)...));
  }

private:
  int launch_counter = 0;
};

} // namespace srslte

#endif // SRSLTE_FSM_H
