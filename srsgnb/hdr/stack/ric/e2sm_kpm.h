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

#include "e2sm.h"
#include "e2sm_kpm_common.h"
#include "srsran/asn1/e2ap.h"
#include "srsran/asn1/e2sm.h"
#include "srsran/asn1/e2sm_kpm_v2.h"
#include "srsran/srsran.h"

#ifndef RIC_E2SM_KPM_H
#define RIC_E2SM_KPM_H

using namespace asn1::e2ap;
using namespace asn1::e2sm_kpm;

class e2sm_kpm_report_service;

class e2sm_kpm : public e2sm
{
public:
  static const std::string short_name;
  static const std::string oid;
  static const std::string func_description;
  static const uint32_t    revision;

  e2sm_kpm(srslog::basic_logger& logger_, srsran::task_scheduler* _task_sched_ptr);
  ~e2sm_kpm();

  virtual bool generate_ran_function_description(RANfunction_description& desc, ra_nfunction_item_s& ran_func);
  virtual bool process_ric_event_trigger_definition(ricsubscription_request_s       subscription_request,
                                                    RIC_event_trigger_definition_t& event_def);
  virtual bool process_ric_action_definition(ri_caction_to_be_setup_item_s ric_action, E2AP_RIC_action_t& action_entry);
  virtual bool remove_ric_action_definition(E2AP_RIC_action_t& action_entry);
  virtual bool generate_ric_indication_content(E2AP_RIC_action_t& action_entry, ric_indication_t& ric_indication);

  virtual void receive_e2_metrics_callback(const enb_metrics_t& m);

  friend class e2sm_kpm_report_service;
  friend class e2sm_kpm_report_service_style1;
  friend class e2sm_kpm_report_service_style2;
  friend class e2sm_kpm_report_service_style3;
  friend class e2sm_kpm_report_service_style4;
  friend class e2sm_kpm_report_service_style5;

private:
  bool _generate_indication_header(e2_sm_kpm_ind_hdr_s& hdr, srsran::unique_byte_buffer_t& buf);
  bool _generate_indication_message(e2_sm_kpm_ind_msg_s& msg, srsran::unique_byte_buffer_t& buf);
  bool                     _get_meas_definition(std::string meas_name, e2sm_kpm_metric_t& def);
  std::vector<std::string> _get_supported_meas(uint32_t level_mask);

  bool _collect_meas_value(e2sm_kpm_meas_def_t& meas_value, meas_record_item_c& item);
  bool
  _extract_integer_type_meas_value(e2sm_kpm_meas_def_t& meas_value, const enb_metrics_t& enb_metrics, uint32_t& value);
  bool _extract_real_type_meas_value(e2sm_kpm_meas_def_t& meas_value, const enb_metrics_t& enb_metrics, float& value);

  srslog::basic_logger&                        logger;
  std::vector<e2sm_kpm_metric_t>               supported_meas_types;
  std::map<uint32_t, e2sm_kpm_report_service*> registered_actions_data;

  srsran_random_t random_gen;

  enb_metrics_t last_enb_metrics;
};

#endif /*E2SM_KPM*/
