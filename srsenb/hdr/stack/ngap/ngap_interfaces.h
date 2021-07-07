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

#ifndef SRSENB_NGAP_INTERFACES_H
#define SRSENB_NGAP_INTERFACES_H

namespace srsenb {
class ngap_interface_ngap_proc
{
public:
  virtual bool send_initial_ctxt_setup_response() = 0;
};
} // namespace srsenb

#endif