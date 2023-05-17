/**
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSRAN_E2AP_RIC_SUBSCRIPTION_H
#define SRSRAN_E2AP_RIC_SUBSCRIPTION_H

#include "e2ap.h"
#include "srsran/common/task_scheduler.h"
#include "srsran/common/threads.h"
#include "srsran/srsran.h"

namespace srsenb {

class e2ap::ric_subscription
{
public:
  ric_subscription(e2ap* e2ap, ricsubscription_request_s ric_subscription_request);
  virtual ~ric_subscription() { parent = nullptr; };

  uint32_t get_ric_requestor_id() { return ric_requestor_id; };
  uint32_t get_ric_instance_id() { return ric_instance_id; };
  bool     is_initialized() { return initialized; };

  void start_subscription();
  void delete_subscription();

  bool process_subscription_modification_request(uint32_t ric_subscription_modification_request);
  bool process_subscription_modification_confirm(uint32_t ric_subscription_modification_confirm);
  bool process_subscription_modification_refuse(uint32_t ric_subscription_modification_refuse);

private:
  void     _send_subscription_response();
  void     _send_subscription_failure();
  void     _send_ric_indication();
  uint32_t _generate_ric_indication_sn();

  e2ap* parent      = nullptr;
  bool  initialized = false;

  uint32_t ric_requestor_id;
  uint32_t ric_instance_id;
  uint16_t ra_nfunction_id;

  e2sm*                sm_ptr           = nullptr;
  uint32_t             reporting_period = 0; // ms
  srsran::unique_timer reporting_timer;      // for RIC indication reporting

  std::vector<E2AP_RIC_action_t> admitted_actions;
  std::vector<uint32_t>          not_admitted_actions;

  uint32_t _ric_indication_sn_gen = 0;
};

} // namespace srsenb
#endif // SRSRAN_E2AP_RIC_SUBSCRIPTION_H
