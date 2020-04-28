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

//! Transition Type
template <typename NextState>
struct to_state {
  using next_state = NextState;
};

template <typename... NextStates>
struct to_states {
  template <typename ChosenState>
  to_states(to_state<ChosenState>) : state_idx(get_type_index<ChosenState, NextStates...>())
  {}

  template <typename State>
  bool is() const
  {
    return get_type_index<State, NextStates...>() == state_idx;
  }

  size_t get_type_idx() const { return state_idx; }

  size_t state_idx;
};

//! Forward declaration
template <typename Derived>
class fsm_t;

namespace fsm_details {

//! Visitor to get a state's name string
struct state_name_visitor {
  template <typename State>
  void operator()(State&& s)
  {
    name = get_type_name(s);
  }
  std::string name = "invalid";
};

//! Visitor to convert a to_state<States...> back to a single state
template <typename FSM, typename PrevState>
struct to_state_visitor {
  to_state_visitor(FSM* f_, PrevState* p_) : f(f_), p(p_) {}
  template <typename State>
  void       operator()();
  FSM*       f;
  PrevState* p;
};

//! Helper metafunctions
template <typename FSM, typename State>
using enable_if_fsm_state = typename std::enable_if<FSM::template can_hold_state<State>()>::type;
template <typename FSM, typename State>
using disable_if_fsm_state = typename std::enable_if<not FSM::template can_hold_state<State>()>::type;
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

struct fsm_helper {
  //! Metafunction to determine if FSM can hold given State type
  template <typename FSM>
  using fsm_state_list_type = decltype(std::declval<typename FSM::derived_view>().states);

  //! Call FSM/State enter method
  template <typename FSM, typename State>
  static enable_if_subfsm<State> call_enter(FSM* f, State* s)
  {
    using init_type = typename fsm_state_list_type<State>::init_state_t;
    // set default FSM type
    s->derived()->states.template transit<init_type>();
    // call FSM enter function
    f->enter(*s);
    // call initial substate enter
    call_enter(s->derived(), &s->derived()->states.template get_unchecked<init_type>());
  }
  template <typename FSM, typename State, typename... Args>
  static disable_if_subfsm<State> call_enter(FSM* f, State* s)
  {
    f->enter(*s);
  }

  //! TargetState is type-erased (a choice). Apply its stored type to the fsm current state
  template <typename FSM, typename... Args, typename PrevState>
  static void handle_state_change(FSM* f, to_states<Args...>* s, PrevState* p)
  {
    to_state_visitor<FSM, PrevState> visitor{f, p};
    srslte::static_visit(visitor, *s);
  }
  //! Simple state transition in FSM (no same_state of entry in nested FSM)
  template <typename FSM, typename State, typename PrevState>
  static auto handle_state_change(FSM* f, to_state<State>* s, PrevState* p) -> enable_if_fsm_state<FSM, State>
  {
    if (std::is_same<State, PrevState>::value) {
      f->log_h->info("FSM \"%s\": No transition occurred while in state \"%s\"\n",
                     get_type_name<typename FSM::derived_t>().c_str(),
                     get_type_name<State>().c_str());
      return;
    }
    f->exit(f->states.template get_unchecked<PrevState>());
    f->states.template transit<State>();
    f->log_h->info("FSM \"%s\": Detected transition \"%s\" -> \"%s\"",
                   get_type_name<typename FSM::derived_t>().c_str(),
                   get_type_name<PrevState>().c_str(),
                   get_type_name<State>().c_str());
    call_enter(f, &f->states.template get_unchecked<State>());
  }
  //! State not present in current FSM. Attempt state transition in parent FSM in the case of NestedFSM
  template <typename FSM, typename State, typename PrevState>
  static auto handle_state_change(FSM* f, to_state<State>* s, PrevState* p) -> disable_if_fsm_state<FSM, State>
  {
    static_assert(FSM::is_nested, "State is not present in the FSM list of valid states");
    f->exit(f->states.template get_unchecked<PrevState>());
    handle_state_change(f->parent_fsm()->derived(), s, static_cast<typename FSM::derived_t*>(f));
  }

  //! Trigger Event, that will result in a state transition
  template <typename FSM, typename Event>
  struct trigger_visitor {
    trigger_visitor(FSM* f_, Event&& ev_) : f(f_), ev(std::forward<Event>(ev_)) {}

    /**
     * @brief Trigger visitor callback for the current state.
     * @description tries to find an fsm::trigger method in case the current state is a nested fsm. If it does not
     * find it, searches for a react(current_state&, event) method at the current level
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

    template <typename State>
    using enable_if_react = decltype(std::declval<FSM>().react(std::declval<State&>(), std::declval<Event&&>()),
                                     bool());
    //! In case there is a react method
    template <typename State>
    auto call_react(State& s) -> decltype(std::declval<FSM>().react(s, std::declval<Event&&>()), bool())
    {
      auto target_state = f->react(s, std::forward<Event>(ev));
      fsm_helper::handle_state_change(f, &target_state, &s);
      return true;
    }
    template <typename... Args>
    bool call_react(Args...)
    {
      f->log_fsm_activity(
          "FSM \"%s\": Unhandled event caught: \"%s\"\n", get_type_name<FSM>().c_str(), get_type_name<Event>().c_str());
      return false;
    }

    FSM*  f;
    Event ev;
    bool  result = false;
  };
};

template <typename FSM, typename PrevState>
template <typename State>
void to_state_visitor<FSM, PrevState>::operator()()
{
  to_state<State> t;
  fsm_helper::handle_state_change(f, &t, p);
}

} // namespace fsm_details

//! Gets the typename currently stored in the choice_t
template <typename... Args>
std::string get_type_name(const srslte::to_states<Args...>& t)
{
  fsm_details::state_name_visitor v{};
  srslte::visit(v, t);
  return v.name;
}

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
    using derived_t::base_t::enter;
    using derived_t::base_t::exit;
    // propagate user fsm methods
    using Derived::enter;
    using Derived::exit;
    using Derived::react;
    using Derived::states;
  };

  static const bool is_nested = false;
  template <typename NextState>
  using to_state = srslte::to_state<NextState>;
  template <typename... NextStates>
  using to_states = srslte::to_states<NextStates...>;

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
        fsm_details::fsm_helper::call_enter(f->derived(), &get_unchecked<init_state_t>());
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
    fsm_details::fsm_helper::trigger_visitor<derived_view, Ev> visitor{derived(), std::forward<Ev>(e)};
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
    return fsm_details::fsm_helper::fsm_state_list_type<fsm_t<Derived> >::template can_hold_type<State>();
  }

  void            set_fsm_event_log_level(srslte::LOG_LEVEL_ENUM e) { fsm_event_log_level = e; }
  srslte::log_ref get_log() const { return log_h; }

protected:
  friend struct fsm_details::fsm_helper;

  // Access to CRTP derived class
  derived_view*       derived() { return static_cast<derived_view*>(this); }
  const derived_view* derived() const { return static_cast<const derived_view*>(this); }

  template <typename State>
  void enter(State& s)
  {
    // do nothing by default
  }
  template <typename State>
  void exit(State& s)
  {
    // do nothing by default
  }

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
  parent_t*       parent_fsm() { return fsm_ptr; }

protected:
  using parent_fsm_t = ParentFSM;
  using fsm_t<Derived>::enter;
  using fsm_t<Derived>::exit;

  ParentFSM* fsm_ptr = nullptr;
};

template <typename Proc>
struct proc_complete_ev {
  proc_complete_ev(bool success_) : success(success_) {}
  bool success;
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
  friend struct fsm_details::fsm_helper;

protected:
  using fsm_t<Derived>::log_h;
  using fsm_t<Derived>::enter;
  using fsm_t<Derived>::exit;

  template <typename State>
  auto react(State&, srslte::proc_launch_ev<int*> e) -> to_state<State>
  {
    log_h->warning("Unhandled event \"launch\" caught when procedure is already running\n");
    return {};
  }

public:
  using base_t = proc_fsm_t<Derived, Result>;
  using fsm_t<Derived>::trigger;

  // events
  struct reset_ev {};

  // states
  struct idle_st {};
  struct complete_st {};

  explicit proc_fsm_t(srslte::log_ref log_) : fsm_t<Derived>(log_) {}

  bool is_running() const { return base_t::template is_in_state<idle_st>(); }

  template <typename... Args>
  void launch(Args&&... args)
  {
    trigger(proc_launch_ev<Args...>(std::forward<Args>(args)...));
  }

protected:
  void exit(idle_st& s)
  {
    launch_counter++;
    log_h->info("Starting run no. %d\n", launch_counter);
  }
  void enter(complete_st& s) { trigger(reset_ev{}); }

  auto react(complete_st& s, reset_ev ev) -> to_state<idle_st> { return {}; }

  srslte::to_state<complete_st> set_success(Result&& r = {})
  {
    result  = std::forward<Result>(r);
    success = true;
    return {};
  }
  srslte::to_state<complete_st> set_failure()
  {
    success = false;
    return {};
  }
  bool          is_success() const { return success; }
  const Result& get_result() const
  {
    if (is_success()) {
      return result;
    }
    THROW_BAD_ACCESS("in proc_fsm_t::get_result");
  }

private:
  int    launch_counter = 0;
  bool   success        = false;
  Result result         = {};
};

} // namespace srslte

#endif // SRSLTE_FSM_H
