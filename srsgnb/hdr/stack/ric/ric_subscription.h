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

#ifndef SRSRAN_RIC_SUBSCRIPTION_H
#define SRSRAN_RIC_SUBSCRIPTION_H

#include "srsgnb/hdr/stack/ric/e2ap.h"
#include "srsgnb/hdr/stack/ric/ric_client.h"
#include "srsran/common/task_scheduler.h"
#include "srsran/common/threads.h"
#include "srsran/srsran.h"

namespace srsenb {

class ric_client::ric_subscription
{
public:
  ric_subscription(ric_client* ric_client, ricsubscription_request_s ric_subscription_request);
  virtual ~ric_subscription() = default;

  uint32_t get_ric_requestor_id() { return ric_requestor_id; };
  uint32_t get_ric_instance_id() { return ric_instance_id; };
  bool     is_initialized() { return initialized; };

  void start_subscription();
  void send_subscription_failure();
  void delete_subscription();

  bool handle_subscription_modification_request(uint32_t ric_subscription_modification_request);
  bool handle_subscription_modification_confirm(uint32_t ric_subscription_modification_confirm);
  bool handle_subscription_modification_refuse(uint32_t ric_subscription_modification_refuse);

private:
  void send_ric_indication();
  uint32_t _generate_ric_indication_sn();

  ric_client* parent      = nullptr;
  bool        initialized = false;

  uint32_t ric_requestor_id;
  uint32_t ric_instance_id;
  uint16_t ra_nfunction_id;

  e2sm*                sm_ptr           = nullptr;
  uint32_t             reporting_period = 0; // ms
  srsran::unique_timer reporting_timer;      // for RIC indication reporting

  std::vector<E2AP_RIC_action_t> admitted_actions;
  std::vector<uint32_t>        not_admitted_actions;

  uint32_t _ric_indication_sn_gen = 0;
};

} // namespace srsenb
#endif // SRSRAN_RIC_SUBSCRIPTION_H
