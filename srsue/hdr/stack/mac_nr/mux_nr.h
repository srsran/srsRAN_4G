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

#ifndef SRSUE_MUX_NR_H
#define SRSUE_MUX_NR_H

#include "srslte/common/byte_buffer.h"
#include "srslte/common/common.h"
#include "srslte/srslog/srslog.h"
#include "srslte/srslte.h"

namespace srsue {
class mux_nr
{
public:
  explicit mux_nr(srslog::basic_logger& logger);
  ~mux_nr(){};
  void reset();
  void init();

  void step();

  void msg3_flush();
  void msg3_prepare();
  void msg3_transmitted();
  bool msg3_is_transmitted();
  bool msg3_is_pending();
  bool msg3_is_empty();

private:
  srslog::basic_logger&        logger;
  srslte::unique_byte_buffer_t msg3_buff = nullptr;
  typedef enum { none, pending, transmitted } msg3_state_t;
  msg3_state_t msg3_state = none;
};
} // namespace srsue

#endif // SRSUE_MUX_NR_H
