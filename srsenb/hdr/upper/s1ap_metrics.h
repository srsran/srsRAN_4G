/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2017 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of srsLTE.
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

#ifndef SRSENB_S1AP_METRICS_H
#define SRSENB_S1AP_METRICS_H


namespace srsenb {

typedef enum{
  S1AP_ATTACHING = 0, // Attempting to create S1 connection
  S1AP_READY,         // S1 connected
  S1AP_ERROR          // Failure
}S1AP_STATUS_ENUM;

struct s1ap_metrics_t
{
  S1AP_STATUS_ENUM status;
};

} // namespace srsenb

#endif // SRSENB_S1AP_METRICS_H
