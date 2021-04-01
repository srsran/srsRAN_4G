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

#include "srsran/common/common.h"
#include "srsran/common/interfaces_common.h"
#include "srsran/srsran.h"

#ifndef SRSRAN_ENB_INTERFACES_H
#define SRSRAN_ENB_INTERFACES_H

namespace srsenb {

class stack_interface_phy_lte;

class stack_interface_gtpu_lte
{
public:
  virtual void add_gtpu_s1u_socket_handler(int fd) = 0;
  virtual void add_gtpu_m1u_socket_handler(int fd) = 0;
};

} // namespace srsenb

#endif // SRSRAN_ENB_INTERFACES_H
