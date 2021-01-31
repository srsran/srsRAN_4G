/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#ifndef SRSUE_TTCN3_SWAPPABLE_SINK_H
#define SRSUE_TTCN3_SWAPPABLE_SINK_H

#include "srslte/srslog/sink.h"

/// A custom sink implementation that allows hot swapping file sinks so that loggers can write to different files
/// dynamically.
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

private:
  srslog::sink* s;
};

#endif // SRSUE_TTCN3_SWAPPABLE_SINK_H
