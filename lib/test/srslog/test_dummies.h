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

#ifndef TEST_DUMMIES
#define TEST_DUMMIES

#include "srslte/srslog/detail/log_backend.h"
#include "srslte/srslog/sink.h"

namespace test_dummies {

/// A Dummy implementation of a formatter.
class log_formatter_dummy : public srslog::log_formatter
{
public:
  void format(srslog::detail::log_entry_metadata&& metadata,
              fmt::memory_buffer& buffer) override
  {}

  std::unique_ptr<log_formatter> clone() const override { return nullptr; }

private:
  void format_context_begin(const srslog::detail::log_entry_metadata& md,
                            const std::string& ctx_name,
                            unsigned size,
                            fmt::memory_buffer& buffer) override
  {}
  void format_context_end(const srslog::detail::log_entry_metadata& md,
                          const std::string& ctx_name,
                          fmt::memory_buffer& buffer) override
  {}
  void format_metric_set_begin(const std::string& set_name,
                               unsigned size,
                               unsigned level,
                               fmt::memory_buffer& buffer) override
  {}
  void format_metric_set_end(const std::string& set_name,
                             unsigned level,
                             fmt::memory_buffer& buffer) override
  {}
  void format_list_begin(const std::string& list_name,
                         unsigned size,
                         unsigned level,
                         fmt::memory_buffer& buffer) override
  {}
  void format_list_end(const std::string& list_name,
                       unsigned level,
                       fmt::memory_buffer& buffer) override
  {}
  void format_metric(const std::string& metric_name,
                     const std::string& metric_value,
                     const std::string& metric_units,
                     srslog::metric_kind kind,
                     unsigned level,
                     fmt::memory_buffer& buffer) override
  {}
};

/// A Dummy implementation of a sink.
class sink_dummy : public srslog::sink
{
public:
  sink_dummy() :
    sink(std::unique_ptr<srslog::log_formatter>(new log_formatter_dummy))
  {}

  srslog::detail::error_string
  write(srslog::detail::memory_buffer buffer) override
  {
    return {};
  }

  srslog::detail::error_string flush() override { return {}; }
};

/// A Dummy implementation of the log backend.
class backend_dummy : public srslog::detail::log_backend
{
public:
  void start() override {}

  void push(srslog::detail::log_entry&& entry) override {}

  bool is_running() const override { return true; }
};

} // namespace test_dummies

#endif // TEST_DUMMIES
