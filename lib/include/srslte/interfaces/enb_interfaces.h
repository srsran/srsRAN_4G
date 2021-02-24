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

#include "srslte/common/common.h"
#include "srslte/common/interfaces_common.h"
#include "srslte/srslte.h"

#ifndef SRSLTE_ENB_INTERFACES_H
#define SRSLTE_ENB_INTERFACES_H

namespace srsenb {

class stack_interface_phy_lte;
class stack_interface_s1ap_lte
{
public:
  virtual void add_mme_socket(int fd)    = 0;
  virtual void remove_mme_socket(int fd) = 0;
};

class stack_interface_gtpu_lte
{
public:
  virtual void add_gtpu_s1u_socket_handler(int fd) = 0;
  virtual void add_gtpu_m1u_socket_handler(int fd) = 0;
};

} // namespace srsenb

#endif // SRSLTE_ENB_INTERFACES_H
