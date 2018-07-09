/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

/******************************************************************************
 * File:        logger_stdout.h
 * Description: Interface for logging output
 *****************************************************************************/

#ifndef SRSLTE_LOGGER_STDOUT_H
#define SRSLTE_LOGGER_STDOUT_H

#include <stdio.h>
#include <string>
#include "srslte/common/logger.h"

namespace srslte {

  class logger_stdout : public logger
  {
  public:
    void log(std::string *msg) {
      if (msg) {
        fprintf(stdout, "%s", msg->c_str());
        delete msg;
      }
    }
  };

} // namespace srslte

#endif // SRSLTE_LOGGER_STDOUT_H
