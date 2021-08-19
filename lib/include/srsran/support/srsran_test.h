/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSRAN_SRSRAN_TEST_H
#define SRSRAN_SRSRAN_TEST_H

#ifdef __cplusplus

#include "srsran_assert.h"

namespace srsran {
namespace detail {

template <typename T, typename U>
[[gnu::noinline, noreturn]] void assert_eq_failure(const T& expected_val, const U& actual_val)
{
  std::string s = fmt::format("Actual value '{}' differs from expected '{}'", actual_val, expected_val);
  srsran_assertion_failure("%s", s.c_str());
}

template <typename T>
[[gnu::noinline, noreturn]] void assert_neq_failure(const T& actual_val)
{
  std::string s = fmt::format("Value should not be equal to '{}'", actual_val);
  srsran_assertion_failure("%s", s.c_str());
}

} // namespace detail
} // namespace srsran

#define TESTASSERT_EQ(EXPECTED, ACTUAL)                                                                                \
  (void)((EXPECTED == ACTUAL) || (::srsran::detail::assert_eq_failure(EXPECTED, ACTUAL), 0))

#define TESTASSERT_NEQ(EXPECTED, ACTUAL)                                                                               \
  (void)((EXPECTED != ACTUAL) || (::srsran::detail::assert_neq_failure(ACTUAL), 0))

#define TESTASSERT(cond) srsran_assert((cond), "Fail at \"%s\"", (#cond))

#define TESTASSERT_SUCCESS(cond) srsran_assert((cond == SRSRAN_SUCCESS), "Operation \"%s\" was not successful", (#cond))

#else // __cplusplus

#include <stdio.h>

#define TESTASSERT(cond)                                                                                               \
  do {                                                                                                                 \
    if (!(cond)) {                                                                                                     \
      printf("[%s][Line %d] Fail at \"%s\"\n", __FUNCTION__, __LINE__, (#cond));                                       \
      return -1;                                                                                                       \
    }                                                                                                                  \
  } while (0)

#endif // __cplusplus

#endif // SRSRAN_SRSRAN_TEST_H
