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
  // TODO: add fields needef in format2 and format3
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

// TODO: define all labels and scopes

/* Labels supported for a metric */
enum e2sm_kpm_label_enum {
  NO_LABEL      = 0x0001,
  MIN_LABEL     = 0x0002,
  MAX_LABEL     = 0x0004,
  AVG_LABEL     = 0x0008,
  SUM_LABEL     = 0x0010,
  UNKNOWN_LABEL = 0x8000
};

e2sm_kpm_label_enum str2kpm_label(const std::string& label_str);

/* Scopes supported for a metric */
enum e2sm_kpm_metric_scope_enum {
  ENB_LEVEL     = 0x0001,
  CELL_LEVEL    = 0x0002,
  UE_LEVEL      = 0x0004,
  BEARER_LEVEL  = 0x0008,
  UNKNOWN_LEVEL = 0xffff
};

typedef struct {
  uint32_t                   action_id;
  std::string                name;
  e2sm_kpm_label_enum        label;
  e2sm_kpm_metric_scope_enum scope;
  e2_metric_data_type_t      data_type;
  std::vector<int32_t>       integer_values;
  std::vector<float>         real_values;
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

  bool _fill_ric_ind_msg_format1(uint32_t                               action_id,
                                 e2_sm_kpm_action_definition_format1_s& action,
                                 E2SM_KPM_RIC_ind_message_t&            r_ind_msg);
  bool _fill_ric_ind_msg_format1(uint32_t                               action_id,
                                 e2_sm_kpm_action_definition_format2_s& action,
                                 E2SM_KPM_RIC_ind_message_t&            r_ind_msg);
  bool _fill_ric_ind_msg_format2(uint32_t                               action_id,
                                 e2_sm_kpm_action_definition_format3_s& action,
                                 E2SM_KPM_RIC_ind_message_t&            r_ind_msg);
  bool _fill_ric_ind_msg_format3(uint32_t                               action_id,
                                 e2_sm_kpm_action_definition_format4_s& action,
                                 E2SM_KPM_RIC_ind_message_t&            r_ind_msg);
  bool _fill_ric_ind_msg_format3(uint32_t                               action_id,
                                 e2_sm_kpm_action_definition_format5_s& action,
                                 E2SM_KPM_RIC_ind_message_t&            r_ind_msg);

  void _add_measurement_records(uint32_t            action_id,
                                std::string         meas_name,
                                e2sm_kpm_label_enum label,
                                meas_record_l&      meas_record_list);
  bool _generate_indication_header(E2SM_KPM_RIC_ind_header_t hdr, srsran::unique_byte_buffer_t& buf);
  bool _generate_indication_message(E2SM_KPM_RIC_ind_message_t msg, srsran::unique_byte_buffer_t& buf);

  bool                    _get_meas_definition(std::string meas_name, E2SM_KPM_metric_t& def);
  uint64_t                _get_meas_collection_start_time(uint32_t action_id);
  void                    _save_meas_collection_start_time(uint32_t action_id, uint64_t timestamp);
  E2SM_KPM_meas_values_t& _get_collected_meas_values(uint32_t act_id, std::string meas_name, e2sm_kpm_label_enum label);
  bool                    _save_last_meas_value(E2SM_KPM_meas_values_t& meas_values);

  std::vector<e2sm_kpm_label_enum> _get_present_labels(const meas_info_item_s& action_meas_info_item);

  bool _extract_last_integer_type_meas_value(E2SM_KPM_meas_values_t& meas_values,
                                             const enb_metrics_t&    enb_metrics,
                                             int32_t&                value);
  bool _extract_last_real_type_meas_value(E2SM_KPM_meas_values_t& meas_values,
                                          const enb_metrics_t&    enb_metrics,
                                          float&                  value);

  srslog::basic_logger&                             logger;
  std::vector<E2SM_KPM_metric_t>                    supported_meas_types;
  std::map<uint32_t, e2_sm_kpm_action_definition_s> registered_actions;
  std::map<uint32_t, uint64_t>                      action_meas_collection_start_timestamp;
  std::vector<E2SM_KPM_meas_values_t>               collected_meas_data;

  srsran_random_t random_gen;

  enb_metrics_t last_enb_metrics;
};

#endif /*E2SM_KPM*/
