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

enum e2_metric_data_type_t { INTEGER, REAL };

typedef struct {
  std::string           name;
  bool                  supported;
  e2_metric_data_type_t data_type;
  std::string           units;
  bool                  min_val_present;
  double                min_val;
  bool                  max_val_present;
  double                max_val;
  uint32_t              supported_labels;
  uint32_t              supported_scopes;
} E2SM_KPM_metric_t;

typedef struct {
  std::string           name;
  std::string           label;
  e2_metric_data_type_t data_type;
  std::vector<int32_t>  integer_values;
  std::vector<float>    real_values;
} E2SM_KPM_meas_values_t;

class e2sm_kpm : public e2sm
{
public:
  static const std::string short_name;
  static const std::string oid;
  static const std::string func_description;
  static const uint32_t    revision;

  e2sm_kpm(srslog::basic_logger& logger_);
  ~e2sm_kpm() = default;

  virtual bool generate_ran_function_description(RANfunction_description& desc, ra_nfunction_item_s& ran_func);
  virtual bool process_ric_event_trigger_definition(ricsubscription_request_s       subscription_request,
                                                    RIC_event_trigger_definition_t& event_def);
  virtual bool process_ric_action_definition(ri_caction_to_be_setup_item_s ric_action, E2AP_RIC_action_t& action_entry);
  virtual bool remove_ric_action_definition(E2AP_RIC_action_t& action_entry);
  virtual bool execute_action_fill_ric_indication(E2AP_RIC_action_t& action_entry, ric_indication_t& ric_indication);

  virtual void receive_e2_metrics_callback(const enb_metrics_t& m);

private:
  bool _process_ric_action_definition_format1(e2_sm_kpm_action_definition_format1_s& action_definition_format1);
  bool _process_ric_action_definition_format2(e2_sm_kpm_action_definition_format2_s& action_definition_format2);
  bool _process_ric_action_definition_format3(e2_sm_kpm_action_definition_format3_s& action_definition_format3);
  bool _process_ric_action_definition_format4(e2_sm_kpm_action_definition_format4_s& action_definition_format4);
  bool _process_ric_action_definition_format5(e2_sm_kpm_action_definition_format5_s& action_definition_format5);

  bool _fill_ric_ind_msg_format1(e2_sm_kpm_action_definition_format1_s& action, E2SM_KPM_RIC_ind_message_t& r_ind_msg);
  bool _fill_ric_ind_msg_format1(e2_sm_kpm_action_definition_format2_s& action, E2SM_KPM_RIC_ind_message_t& r_ind_msg);
  bool _fill_ric_ind_msg_format2(e2_sm_kpm_action_definition_format3_s& action, E2SM_KPM_RIC_ind_message_t& r_ind_msg);
  bool _fill_ric_ind_msg_format3(e2_sm_kpm_action_definition_format4_s& action, E2SM_KPM_RIC_ind_message_t& r_ind_msg);
  bool _fill_ric_ind_msg_format3(e2_sm_kpm_action_definition_format5_s& action, E2SM_KPM_RIC_ind_message_t& r_ind_msg);

  void _fill_measurement_records(std::string meas_name, std::string label, meas_record_l& meas_record_list);
  bool _generate_indication_header(E2SM_KPM_RIC_ind_header_t hdr, srsran::unique_byte_buffer_t& buf);
  bool _generate_indication_message(E2SM_KPM_RIC_ind_message_t msg, srsran::unique_byte_buffer_t& buf);

  bool _get_meas_definition(std::string meas_name, E2SM_KPM_metric_t& def);
  bool _get_last_meas_value(E2SM_KPM_meas_values_t& meas_values);
  bool _get_last_N_meas_values(uint32_t N, E2SM_KPM_meas_values_t& meas_values);

  bool _get_last_integer_type_meas_value(std::string meas_name, std::string label, int32_t& value);
  bool _get_last_real_type_meas_value(std::string meas_name, std::string label, float& value);

  srslog::basic_logger&                             logger;
  std::vector<E2SM_KPM_metric_t>                    supported_meas_types;
  std::map<uint32_t, e2_sm_kpm_action_definition_s> registered_actions;

  srsran_random_t random_gen;

  enb_metrics_t last_enb_metrics;
};

#endif /*E2SM_KPM*/
