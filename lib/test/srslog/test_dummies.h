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

#ifndef TEST_DUMMIES
#define TEST_DUMMIES

#include "srsran/srslog/detail/log_backend.h"
#include "srsran/srslog/shared_types.h"
#include "srsran/srslog/sink.h"

namespace test_dummies {

/// A Dummy implementation of a formatter.
class log_formatter_dummy : public srslog::log_formatter
{
public:
  void format(srslog::detail::log_entry_metadata&& metadata, fmt::memory_buffer& buffer) override {}

  std::unique_ptr<log_formatter> clone() const override { return nullptr; }

private:
  void format_context_begin(const srslog::detail::log_entry_metadata& md,
                            fmt::string_view                          ctx_name,
                            unsigned                                  size,
                            fmt::memory_buffer&                       buffer) override
  {}
  void format_context_end(const srslog::detail::log_entry_metadata& md,
                          fmt::string_view                          ctx_name,
                          fmt::memory_buffer&                       buffer) override
  {}
  void
  format_metric_set_begin(fmt::string_view set_name, unsigned size, unsigned level, fmt::memory_buffer& buffer) override
  {}
  void format_metric_set_end(fmt::string_view set_name, unsigned level, fmt::memory_buffer& buffer) override {}
  void format_list_begin(fmt::string_view list_name, unsigned size, unsigned level, fmt::memory_buffer& buffer) override
  {}
  void format_list_end(fmt::string_view list_name, unsigned level, fmt::memory_buffer& buffer) override {}
  void format_metric(fmt::string_view    metric_name,
                     fmt::string_view    metric_value,
                     fmt::string_view    metric_units,
                     srslog::metric_kind kind,
                     unsigned            level,
                     fmt::memory_buffer& buffer) override
  {}
};

/// A Dummy implementation of a sink.
class sink_dummy : public srslog::sink
{
public:
  sink_dummy() : sink(std::unique_ptr<srslog::log_formatter>(new log_formatter_dummy)) {}

  srslog::detail::error_string write(srslog::detail::memory_buffer buffer) override { return {}; }

  srslog::detail::error_string flush() override { return {}; }
};

/// A Dummy implementation of the log backend.
class backend_dummy : public srslog::detail::log_backend
{
public:
  void start(srslog::backend_priority priority) override {}

  bool push(srslog::detail::log_entry&& entry) override { return true; }

  bool is_running() const override { return true; }

  fmt::dynamic_format_arg_store<fmt::printf_context>* alloc_arg_store() override { return nullptr; }
};

} // namespace test_dummies

#endif // TEST_DUMMIES
