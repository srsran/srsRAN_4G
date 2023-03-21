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
#include "e2sm.h"
#include "srsran/asn1/e2ap.h"
#include "srsran/asn1/e2sm.h"
#include "srsran/asn1/e2sm_kpm_v2.h"
#include "srsran/srsran.h"

#ifndef RIC_E2SM_KPM_H
#define RIC_E2SM_KPM_H

using namespace asn1::e2ap;
using namespace asn1::e2sm_kpm;

typedef struct {
  uint32_t    collet_start_time;
  std::string file_formatversion;
  std::string sender_name;
  std::string sender_type;
  std::string vendor_name;
} E2SM_KPM_RIC_ind_header_t;

typedef struct {
  e2_sm_kpm_ind_msg_s::ind_msg_formats_c_::types ind_msg_format;
  meas_data_l                                    meas_data;
  meas_info_list_l                               meas_info_list;
  uint64_t                                       granul_period;
} E2SM_KPM_RIC_ind_message_t;

class e2sm_kpm : public e2sm
{
public:
  static const std::string short_name;
  static const std::string oid;
  static const std::string func_description;
  static const uint32_t    revision;

  e2sm_kpm(srslog::basic_logger& logger_);
  ~e2sm_kpm() = default;

  virtual bool generate_ran_function_description(RANfunction_description& desc, srsran::unique_byte_buffer_t& buf);
  virtual bool process_ric_event_trigger_definition(ricsubscription_request_s       subscription_request,
                                                    RIC_event_trigger_definition_t& event_def);
  virtual bool process_ric_action_definition(ri_caction_to_be_setup_item_s ric_action, E2AP_RIC_action_t& action_entry);
  virtual bool remove_ric_action_definition(E2AP_RIC_action_t& action_entry);
  virtual bool execute_action_fill_ric_indication(E2AP_RIC_action_t& action_entry, ric_indication_t& ric_indication);

  bool generate_indication_header(E2SM_KPM_RIC_ind_header_t hdr, srsran::unique_byte_buffer_t& buf);
  bool generate_indication_message(E2SM_KPM_RIC_ind_message_t msg, srsran::unique_byte_buffer_t& buf);

private:
  void _fill_measurement_records(std::string meas_name, std::string label, meas_record_l& meas_record_list);

  srslog::basic_logger&                             logger;
  std::vector<std::string>                          supported_meas_types;
  std::map<uint32_t, e2_sm_kpm_action_definition_s> registered_actions;

  srsran_random_t random_gen;
};

#endif /*E2SM_KPM*/
