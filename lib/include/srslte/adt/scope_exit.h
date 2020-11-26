/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSLTE_SCOPE_EXIT_H
#define SRSLTE_SCOPE_EXIT_H

#include <type_traits>
#include <utility>

namespace srslte {

namespace detail {

template <typename Callable>
struct scope_exit {
  template <typename C>
  explicit scope_exit(C&& f_) : exit_function(std::forward<C>(f_))
  {}
  scope_exit(scope_exit&& rhs) noexcept : exit_function(std::move(rhs.exit_function)), active(rhs.active)
  {
    rhs.release();
  }
  scope_exit(const scope_exit&) = delete;
  scope_exit& operator=(const scope_exit&) = delete;
  scope_exit& operator=(scope_exit&&) noexcept = delete;
  ~scope_exit()
  {
    if (active) {
      exit_function();
    }
  }

  void release() { active = false; }

private:
  Callable exit_function;
  bool     active = true;
};

} // namespace detail

/**
 * @brief Defers callable call to scope exit
 * @tparam Callable Any type with a call operator
 * @param callable function that is called at scope exit
 * @return object that has to be stored in a local variable
 */
template <typename Callable>
detail::scope_exit<typename std::decay<Callable>::type> make_scope_exit(Callable&& callable)
{
  return detail::scope_exit<typename std::decay<Callable>::type>{std::forward<Callable>(callable)};
}

#define DEFER(FUNC) auto on_exit_call = make_scope_exit([&]() { FUNC })

} // namespace srslte

#endif // SRSLTE_SCOPE_EXIT_H
