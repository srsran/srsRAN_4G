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
