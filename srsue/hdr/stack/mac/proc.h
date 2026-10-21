// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

#ifndef SRSUE_PROC_H
#define SRSUE_PROC_H

#include <stdint.h>

/* Interface for a MAC procedure */

namespace srsue {

class proc
{
public:
  proc() { running = false; }
  void         run() { running = true; }
  void         stop() { running = false; }
  bool         is_running() { return running; }
  virtual void step(uint32_t tti) = 0;

private:
  bool running;
};

} // namespace srsue

#endif // SRSUE_PROC_H
