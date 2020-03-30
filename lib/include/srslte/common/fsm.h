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

#include "choice_type.h"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <limits>
#include <memory>
#include <tuple>

namespace srslte {

// using same_state = mpark::monostate;
struct same_state {
};
template <typename... Args>
using state_list = choice_t<Args...>;

namespace fsm_details {

//! Visitor to get a state's name string
struct state_name_visitor {
  template <typename State>
  void operator()(State&& s)
  {
    name = s.name();
  }
  const char* name = "invalid state";
};

template <typename TargetVariant, typename PrevState>
struct variant_convert {
  template <typename State>
  void operator()(State& s)
  {
    static_assert(not std::is_same<typename std::decay<State>::type, typename std::decay<PrevState>::type>::value,
                  "State cannot transition to itself.\n");
    *v = std::move(s);
  }
  TargetVariant* v;
  PrevState*     p;
};

struct fsm_helper {
  //! Stayed in same state
  template <typename FSM, typename PrevState>
  static void handle_state_change(FSM* f, same_state* s, PrevState* p)
  {
    // do nothing
  }
  //! TargetState is type-erased. Apply its stored type to the fsm current state
  template <typename FSM, typename... Args, typename PrevState>
  static void handle_state_change(FSM* f, choice_t<Args...>* s, PrevState* p)
  {
    fsm_details::variant_convert<decltype(f->states), PrevState> visitor{.v = &f->states, .p = p};
    s->visit(visitor);
  }
  //! Simple state transition in FSM
  template <typename FSM, typename State, typename PrevState>
  static auto handle_state_change(FSM* f, State* s, PrevState* p) -> decltype(f->states = std::move(*s), void())
  {
    static_assert(not std::is_same<State, PrevState>::value, "State cannot transition to itself.\n");
    f->states = std::move(*s);
  }
  //! State not present in current FSM. Attempt state transition in parent FSM in the case of NestedFSM
  template <typename FSM, typename... Args>
  static void handle_state_change(FSM* f, Args&&... args)
  {
    static_assert(FSM::is_nested, "State is not present in the FSM list of valid states");
    handle_state_change(f->parent_fsm()->derived(), args...);
  }

  //! Trigger Event, that will result in a state transition
  template <typename FSM, typename Event>
  struct trigger_visitor {
    trigger_visitor(FSM* f_, Event&& ev_) : f(f_), ev(std::forward<Event>(ev_)) {}

    //! Trigger visitor callback for the current state
    template <typename CurrentState>
    void operator()(CurrentState& s)
    {
      call_trigger(&s);
    }

    //! Compute next state type
    template <typename State>
    using NextState = decltype(std::declval<FSM>().react(std::declval<State&>(), std::declval<Event>()));

    //! In case a react(CurrentState&, Event) method is found
    template <typename State>
    auto call_trigger(State* current_state) -> NextState<State>
    {
      using next_state = NextState<State>;
      static_assert(not std::is_same<next_state, State>::value, "State cannot transition to itself.\n");
      auto target_state = f->react(*current_state, std::move(ev));
      fsm_helper::handle_state_change(f, &target_state, current_state);
      return target_state;
    }
    //! In case a react(CurrentState&, Event) method is not found, but we are in a NestedFSM with a trigger method
    template <typename State>
    auto call_trigger(State* s) -> decltype(std::declval<State>().trigger(std::declval<Event>()))
    {
      s->trigger(std::move(ev));
    }
    //! No trigger or react method found. Do nothing
    void call_trigger(...) {}

    FSM*  f;
    Event ev;
  };
};

} // namespace fsm_details

//! Base class for states and FSMs
class state_t
{
public:
  state_t()                        = default;
  virtual const char* name() const = 0;
};

template <typename Derived>
class fsm_t
{
public:
  // get access to derived protected members from the base
  class derived_view : public Derived
  {
  public:
    using Derived::react;
    using Derived::states;
  };
  static const bool is_nested = false;

  virtual const char* name() const = 0;

  // Push Events to FSM
  template <typename Ev>
  void trigger(Ev&& e)
  {
    fsm_details::fsm_helper::trigger_visitor<derived_view, Ev> visitor{derived(), std::forward<Ev>(e)};
    derived()->states.visit(visitor);
  }

  template <typename State>
  bool is_in_state() const
  {
    return derived()->states.template is<State>();
  }

  template <typename State>
  const State* get_state() const
  {
    return srslte::get_if<State>(derived()->states);
  }

  const char* get_state_name() const
  {
    fsm_details::state_name_visitor visitor{};
    derived()->states.visit(visitor);
    return visitor.name;
  }

protected:
  friend struct fsm_details::fsm_helper;

  // Access to CRTP derived class
  derived_view*       derived() { return static_cast<derived_view*>(this); }
  const derived_view* derived() const { return static_cast<const derived_view*>(this); }
};

template <typename Derived, typename ParentFSM>
class nested_fsm_t : public fsm_t<Derived>
{
  using base_t      = fsm_t<Derived>;
  using parent_t    = ParentFSM;
  using parent_view = typename parent_t::derived_view;

public:
  static const bool is_nested = true;

  explicit nested_fsm_t(ParentFSM* parent_fsm_) : fsm_ptr(parent_fsm_) {}

  // Get pointer to outer FSM in case of HSM
  const parent_t* parent_fsm() const { return fsm_ptr; }
  parent_t*       parent_fsm() { return fsm_ptr; }

protected:
  using parent_fsm_t = ParentFSM;

  ParentFSM* fsm_ptr = nullptr;
};

} // namespace srslte

#endif // SRSLTE_FSM_H
