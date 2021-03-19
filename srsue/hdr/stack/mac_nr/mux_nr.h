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

#ifndef SRSUE_MUX_NR_H
#define SRSUE_MUX_NR_H

#include "srsran/common/byte_buffer.h"
#include "srsran/common/common.h"
#include "srsran/srslog/srslog.h"
#include "srsran/srsran.h"

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
  srsran::unique_byte_buffer_t msg3_buff = nullptr;
  typedef enum { none, pending, transmitted } msg3_state_t;
  msg3_state_t msg3_state = none;
};
} // namespace srsue

#endif // SRSUE_MUX_NR_H
