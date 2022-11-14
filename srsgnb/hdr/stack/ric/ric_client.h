/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 *
 */

#ifndef RIC_CLIENT_H
#define RIC_CLIENT_H

#include "srsgnb/hdr/stack/ric/e2ap.h"
#include "srsran/common/network_utils.h"
#include "srsran/common/threads.h"
#include "srsran/srsran.h"
static const int e2ap_ppid = 70;
static const int e2ap_port = 36422;
namespace srsenb {
class ric_client : public srsran::thread
{
public:
  ric_client();
  bool init();
  void stop();
  void run_thread();
  bool send_sctp(srsran::unique_byte_buffer_t& buf);
  bool send_e2_setup_request();

private:
  e2ap                  e2ap_;
  srsran::unique_socket ric_socket;
  struct sockaddr_in    ric_addr = {}; // RIC address
  bool                  running  = false;
};
} // namespace srsenb

#endif /* RIC_CLIENT_H */
