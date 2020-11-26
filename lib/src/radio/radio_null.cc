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

#include "srslte/radio/radio_null.h"
#include <mutex>

namespace srslte {

radio_null::radio_null(srslte::logger* logger_) : log("RF"), radio_base(logger_) {}

} // namespace srslte
