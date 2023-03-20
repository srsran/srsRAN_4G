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

  void start_ric_indication_reporting();
  void stop_ric_indication_reporting();

private:
  void send_ric_indication();

  ric_client* parent = nullptr;

  uint32_t ric_requestor_id;
  uint32_t ric_instance_id;
  uint16_t ra_nfunction_id;
  uint16_t ri_caction_id;

  uint32_t             reporting_period; // ms
  srsran::unique_timer reporting_timer;  // for RIC indication reporting
};

} // namespace srsenb
#endif // SRSRAN_RIC_SUBSCRIPTION_H
