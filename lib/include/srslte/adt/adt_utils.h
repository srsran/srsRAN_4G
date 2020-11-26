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

#ifndef SRSLTE_ADT_UTILS_H
#define SRSLTE_ADT_UTILS_H

#ifdef __EXCEPTIONS

#include <stdexcept>

#define EXCEPTIONS_ENABLED 1

namespace srslte {

class bad_type_access : public std::runtime_error
{
public:
  explicit bad_type_access(const std::string& what_arg) : runtime_error(what_arg) {}
  explicit bad_type_access(const char* what_arg) : runtime_error(what_arg) {}
};

#define THROW_BAD_ACCESS(msg) throw bad_type_access(msg)

} // namespace srslte

#else

#define EXCEPTIONS_ENABLED 0

#include <cstdio>
#include <cstdlib>

namespace srslte {

#define THROW_BAD_ACCESS(msg)                                                                                          \
  std::fprintf(stderr, "ERROR: exception thrown with %s", msg);                                                        \
  std::abort()

} // namespace srslte

#endif

#endif // SRSLTE_ADT_UTILS_H
