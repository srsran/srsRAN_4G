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

#ifndef SRSUE_TTCN3_SWAPPABLE_SINK_H
#define SRSUE_TTCN3_SWAPPABLE_SINK_H

#include "srslte/srslog/sink.h"

/// A custom sink implementation that allows hot swapping file sinks so that loggers can write to different files
/// dynamically or other stream-like sinks like stdout.
class swappable_sink : public srslog::sink
{
public:
  swappable_sink(const std::string& filename, std::unique_ptr<srslog::log_formatter> f) :
    srslog::sink(std::move(f)), s(&srslog::fetch_file_sink(filename))
  {}

  /// Identifier of this custom sink.
  static const char* name() { return "swappable_sink"; }

  srslog::detail::error_string write(srslog::detail::memory_buffer buffer) override { return s->write(buffer); }

  srslog::detail::error_string flush() override { return s->flush(); }

  /// Swaps the current file sink with a new sink that will write to the specified file name.
  void swap_sink(const std::string& filename)
  {
    srslog::flush();
    s = &srslog::fetch_file_sink(filename);
  }

  /// Swaps the current sink to use the stdout sink.
  void swap_to_stdout()
  {
    srslog::flush();
    s = &srslog::fetch_stdout_sink();
  }

private:
  srslog::sink* s;
};

#endif // SRSUE_TTCN3_SWAPPABLE_SINK_H
