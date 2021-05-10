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

#ifndef SRSLOG_DETAIL_SUPPORT_DYN_ARG_STORE_POOL_H
#define SRSLOG_DETAIL_SUPPORT_DYN_ARG_STORE_POOL_H

#include "srsran/srslog/bundled/fmt/printf.h"
#include "srsran/srslog/detail/support/backend_capacity.h"

namespace srslog {

namespace detail {

/// Keeps a pool of dynamic_format_arg_store objects. The main reason for this class is that the arg store objects are
/// implemented with std::vectors, so we want to avoid allocating memory each time we create a new object. Instead,
/// reserve memory for each vector during initialization and recycle the objects.
class dyn_arg_store_pool
{
public:
  dyn_arg_store_pool()
  {
    pool.resize(SRSLOG_QUEUE_CAPACITY);
    for (auto& elem : pool) {
      // Reserve for 10 normal and 2 named arguments.
      elem.reserve(10, 2);
    }
    free_list.reserve(SRSLOG_QUEUE_CAPACITY);
    for (auto& elem : pool) {
      free_list.push_back(&elem);
    }
  }

  /// Returns a pointer to a free dyn arg store object, otherwise returns nullptr.
  fmt::dynamic_format_arg_store<fmt::printf_context>* alloc()
  {
    scoped_lock lock(m);
    if (free_list.empty()) {
      return nullptr;
    }

    auto* p = free_list.back();
    free_list.pop_back();

    return p;
  }

  /// Deallocate the given dyn arg store object returning it to the pool.
  void dealloc(fmt::dynamic_format_arg_store<fmt::printf_context>* p)
  {
    if (!p) {
      return;
    }

    p->clear();
    scoped_lock lock(m);
    free_list.push_back(p);
  }

private:
  std::vector<fmt::dynamic_format_arg_store<fmt::printf_context> > pool;
  std::vector<fmt::dynamic_format_arg_store<fmt::printf_context>*> free_list;
  mutable mutex                                                    m;
};

} // namespace detail

} // namespace srslog

#endif // SRSLOG_DETAIL_SUPPORT_DYN_ARG_STORE_POOL_H
