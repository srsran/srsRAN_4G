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

#include "srsgnb/hdr/stack/ric/e2sm_kpm.h"
#include "srsgnb/hdr/stack/ric/e2sm_kpm_metrics.h"
#include <numeric>

const std::string e2sm_kpm::short_name       = "ORAN-E2SM-KPM";
const std::string e2sm_kpm::oid              = "1.3.6.1.4.1.53148.1.2.2.2";
const std::string e2sm_kpm::func_description = "KPM Monitor";
const uint32_t    e2sm_kpm::revision         = 0;

e2sm_kpm::e2sm_kpm(srslog::basic_logger& logger_) : e2sm(short_name, oid, func_description, revision), logger(logger_)
{
  random_gen = srsran_random_init(1234);

  // add supported metrics
  for (auto& metric : get_e2sm_kpm_28_552_metrics()) {
    if (metric.supported) {
      supported_meas_types.push_back(metric);
    }
  }
  for (auto& metric : get_e2sm_kpm_34_425_metrics()) {
    if (metric.supported) {
      supported_meas_types.push_back(metric);
    }
  }
  for (auto& metric : e2sm_kpm_oran_metrics()) {
    if (metric.supported) {
      supported_meas_types.push_back(metric);
    }
  }
  for (auto& metric : e2sm_kpm_custom_metrics()) {
    if (metric.supported) {
      supported_meas_types.push_back(metric);
    }
  }
}

void e2sm_kpm::receive_e2_metrics_callback(const enb_metrics_t& m)
{
  last_enb_metrics = m;
  logger.debug("e2sm_kpm received new enb metrics, CPU0 Load: %.1f", last_enb_metrics.sys.cpu_load[0]);

  // TODO: for INSERT type of reporting check if a specified metrics exceeds predefined thresholds,
  // if so then send RIC indication throught proper subscription
}

bool e2sm_kpm::generate_ran_function_description(RANfunction_description& desc, ra_nfunction_item_s& ran_func)
{
  desc.function_shortname = short_name;
  desc.function_e2_sm_oid = oid;
  desc.function_desc      = func_description;

  e2_sm_kpm_ra_nfunction_description_s e2sm_kpm_ra_nfunction_description;
  e2sm_kpm_ra_nfunction_description.ran_function_name.ran_function_short_name.from_string(short_name.c_str());
  e2sm_kpm_ra_nfunction_description.ran_function_name.ran_function_e2_sm_oid.from_string(oid.c_str());
  e2sm_kpm_ra_nfunction_description.ran_function_name.ran_function_description.from_string(func_description.c_str());
  if (desc.function_instance) {
    e2sm_kpm_ra_nfunction_description.ran_function_name.ran_function_instance_present = true;
    e2sm_kpm_ra_nfunction_description.ran_function_name.ran_function_instance         = desc.function_instance;
  }

  // O-RAN.WG3.E2SM-KPM-R003-v03.00, 7.3.1 Event Trigger Style Types
  auto& event_trigger_style_list = e2sm_kpm_ra_nfunction_description.ric_event_trigger_style_list;
  event_trigger_style_list.resize(1);
  event_trigger_style_list[0].ric_event_trigger_style_type = 1;
  event_trigger_style_list[0].ric_event_trigger_style_name.from_string("Periodic report");
  event_trigger_style_list[0].ric_event_trigger_format_type = 1; // uses RIC Event Trigger Definition Format 1

  // O-RAN.WG3.E2SM-KPM-R003-v03.00, 7.4.1 REPORT Service Style Type
  /*
  auto& report_style_list = e2sm_kpm_ra_nfunction_description.ric_report_style_list;
  report_style_list.resize(5);

  report_style_list[0].ric_report_style_type = 1;
  report_style_list[0].ric_report_style_name.from_string("E2 Node Measurement");
  report_style_list[0].ric_action_format_type  = 1;
  report_style_list[0].ric_ind_hdr_format_type = 1;
  report_style_list[0].ric_ind_msg_format_type = 1;
  // A measurement ID can be used for subscription instead of a measurement type if an identifier of a certain
  // measurement type was exposed by an E2 Node via the RAN Function Definition IE.
  // measurement name to ID mapping (local to the E2 node), here only an example:
  report_style_list[0].meas_info_action_list.resize(1);
  report_style_list[0].meas_info_action_list[0].meas_name.from_string("RRU.PrbTotDl");
  report_style_list[0].meas_info_action_list[0].meas_id = 123;

  report_style_list[1].ric_report_style_type = 2;
  report_style_list[1].ric_report_style_name.from_string("E2 Node Measurement for a single UE");
  report_style_list[1].ric_action_format_type  = 2; // includes UE ID
  report_style_list[1].ric_ind_hdr_format_type = 1;
  report_style_list[1].ric_ind_msg_format_type = 1;

  report_style_list[2].ric_report_style_type = 3;
  report_style_list[2].ric_report_style_name.from_string("Condition-based, UE-level E2 Node Measurement");
  report_style_list[2].ric_action_format_type  = 3;
  report_style_list[2].ric_ind_hdr_format_type = 1;
  report_style_list[2].ric_ind_msg_format_type = 2;

  report_style_list[3].ric_report_style_type = 4;
  report_style_list[3].ric_report_style_name.from_string("Common Condition-based, UE-level Measurement");
  report_style_list[3].ric_action_format_type  = 4;
  report_style_list[3].ric_ind_hdr_format_type = 1;
  report_style_list[3].ric_ind_msg_format_type = 3;

  report_style_list[4].ric_report_style_type = 5;
  report_style_list[4].ric_report_style_name.from_string("E2 Node Measurement for multiple UEs");
  report_style_list[4].ric_action_format_type  = 5;
  report_style_list[4].ric_ind_hdr_format_type = 1;
  report_style_list[4].ric_ind_msg_format_type = 3;
  */
  logger.info("Generating RAN function description");
  srsran::unique_byte_buffer_t buf = srsran::make_byte_buffer();
  asn1::bit_ref                bref(buf->msg, buf->get_tailroom());
  if (e2sm_kpm_ra_nfunction_description.pack(bref) != asn1::SRSASN_SUCCESS) {
    printf("Failed to pack TX E2 PDU\n");
    return false;
  }
  buf->N_bytes = bref.distance_bytes();

  ran_func.ran_function_definition.resize(buf->N_bytes);
  buf->msg[1] = 0x30; // TODO: needed to keep wireshak happy, need better fix
  std::copy(buf->msg, buf->msg + buf->N_bytes, ran_func.ran_function_definition.data());

  return true;
}

bool e2sm_kpm::process_ric_event_trigger_definition(ricsubscription_request_s       subscription_request,
                                                    RIC_event_trigger_definition_t& event_def)
{
  e2_sm_kpm_event_trigger_definition_s trigger_def;
  asn1::cbit_ref bref(subscription_request->ricsubscription_details->ric_event_trigger_definition.data(),
                      subscription_request->ricsubscription_details->ric_event_trigger_definition.size());

  if (trigger_def.unpack(bref) != asn1::SRSASN_SUCCESS) {
    return false;
  }

  event_def.type          = RIC_event_trigger_definition_t::e2sm_event_trigger_type_t::E2SM_REPORT;
  event_def.report_period = trigger_def.event_definition_formats.event_definition_format1().report_period;
  return true;
}

bool e2sm_kpm::process_ric_action_definition(ri_caction_to_be_setup_item_s ric_action, E2AP_RIC_action_t& action_entry)
{
  bool                          admit_action = false;
  e2_sm_kpm_action_definition_s e2sm_kpm_action_def;
  asn1::cbit_ref                bref(ric_action.ric_action_definition.data(), ric_action.ric_action_definition.size());

  if (e2sm_kpm_action_def.unpack(bref) != asn1::SRSASN_SUCCESS) {
    return false;
  }

  switch (e2sm_kpm_action_def.ric_style_type) {
    case 1:
      admit_action = _process_ric_action_definition_format1(
          e2sm_kpm_action_def.action_definition_formats.action_definition_format1());
      break;
    case 2:
      admit_action = _process_ric_action_definition_format2(
          e2sm_kpm_action_def.action_definition_formats.action_definition_format2());
      break;
    case 3:
      admit_action = _process_ric_action_definition_format3(
          e2sm_kpm_action_def.action_definition_formats.action_definition_format3());
      break;
    case 4:
      admit_action = _process_ric_action_definition_format4(
          e2sm_kpm_action_def.action_definition_formats.action_definition_format4());
      break;
    case 5:
      admit_action = _process_ric_action_definition_format5(
          e2sm_kpm_action_def.action_definition_formats.action_definition_format5());
      break;
    default:
      logger.info("Unknown RIC style type %i -> do not admit action %i (type %i)",
                  e2sm_kpm_action_def.ric_style_type,
                  ric_action.ric_action_id,
                  ric_action.ric_action_type);
      return false;
  }

  if (admit_action) {
    action_entry.sm_local_ric_action_id = _generate_local_action_id();
    registered_actions.insert(
        std::pair<uint32_t, e2_sm_kpm_action_definition_s>(action_entry.sm_local_ric_action_id, e2sm_kpm_action_def));
  }

  return admit_action;
}

bool e2sm_kpm::_process_ric_action_definition_format1(e2_sm_kpm_action_definition_format1_s& action_definition_format1)
{
  uint64_t         granul_period;
  uint64_t         eutra_cell_id;
  uint64_t         plmn_id;
  ueid_c           ue_id;
  meas_info_list_l meas_info_list;

  granul_period = action_definition_format1.granul_period;

  if (granul_period == 0) {
    logger.debug("Action granularity period of %i is not supported -> do not admitted action\n", granul_period);
    return false;
  }

  if (action_definition_format1.cell_global_id_present) {
    if (action_definition_format1.cell_global_id.type() == cgi_c::types_opts::eutra_cgi) {
      eutra_cell_id = action_definition_format1.cell_global_id.eutra_cgi().eutra_cell_id.to_number();
      plmn_id       = action_definition_format1.cell_global_id.eutra_cgi().plmn_id.to_number();
      logger.debug("plmn_id 0x%x, eutra_cell_id %i", plmn_id, eutra_cell_id);
      // TODO: check if E2 node has cell_id and plmn_id
    }
  }

  meas_info_list = action_definition_format1.meas_info_list;
  for (uint32_t i = 0; i < meas_info_list.size(); i++) {
    std::string       meas_name = meas_info_list[i].meas_type.meas_name().to_string();
    E2SM_KPM_metric_t metric_definition;
    if (not _get_meas_definition(meas_name, metric_definition)) {
      printf("Unsupported measurement name: \"%s\" --> do not admit action\n", meas_name.c_str());
      return false;
    }

    printf("Admitted action: measurement name: \"%s\" with the following labels: \n", meas_name.c_str());
    // TODO: add all labels defined in e2sm_kpm doc, if at least one label not supported do not admit action?
    for (uint32_t l = 0; l < meas_info_list[i].label_info_list.size(); l++) {
      if (meas_info_list[i].label_info_list[l].meas_label.no_label_present) {
        if (metric_definition.supported_labels & NO_LABEL) {
          printf("--- Label %i: NO LABEL\n", i);
        }
      }
      if (meas_info_list[i].label_info_list[l].meas_label.min_present) {
        if (metric_definition.supported_labels & MIN_LABEL) {
          printf("--- Label %i: MIN\n", i);
        }
      }
      if (meas_info_list[i].label_info_list[l].meas_label.max_present) {
        if (metric_definition.supported_labels & MAX_LABEL) {
          printf("--- Label %i: MAX\n", i);
        }
      }
      if (meas_info_list[i].label_info_list[l].meas_label.avg_present) {
        if (metric_definition.supported_labels & AVG_LABEL) {
          printf("--- Label %i: AVG\n", i);
        }
      }
    }
  }

  return true;
}

bool e2sm_kpm::_process_ric_action_definition_format2(e2_sm_kpm_action_definition_format2_s& action_definition_format2)
{
  return false;
}

bool e2sm_kpm::_process_ric_action_definition_format3(e2_sm_kpm_action_definition_format3_s& action_definition_format3)
{
  return false;
}

bool e2sm_kpm::_process_ric_action_definition_format4(e2_sm_kpm_action_definition_format4_s& action_definition_format4)
{
  return false;
}

bool e2sm_kpm::_process_ric_action_definition_format5(e2_sm_kpm_action_definition_format5_s& action_definition_format5)
{
  return false;
}

bool e2sm_kpm::remove_ric_action_definition(E2AP_RIC_action_t& action_entry)
{
  if (registered_actions.count(action_entry.sm_local_ric_action_id)) {
    registered_actions.erase(action_entry.sm_local_ric_action_id);
    return true;
  }
  return false;
}

bool e2sm_kpm::execute_action_fill_ric_indication(E2AP_RIC_action_t& action_entry, ric_indication_t& ric_indication)
{
  if (!registered_actions.count(action_entry.sm_local_ric_action_id)) {
    logger.info("Unknown RIC action ID: %i (type %i)  (SM local RIC action ID: %i)",
                action_entry.ric_action_id,
                action_entry.ric_action_type,
                action_entry.sm_local_ric_action_id);
    return false;
  }

  E2SM_KPM_RIC_ind_header_t  ric_ind_header;
  E2SM_KPM_RIC_ind_message_t ric_ind_message;

  ric_indication.indication_type = ri_cind_type_opts::report;

  // header is the same for all RIC service styles, i.e., type 1
  ric_ind_header.collet_start_time = std::time(0);
  ric_indication.ri_cind_hdr       = srsran::make_byte_buffer();
  this->_generate_indication_header(ric_ind_header, ric_indication.ri_cind_hdr);

  e2_sm_kpm_action_definition_s action = registered_actions.at(action_entry.sm_local_ric_action_id);

  switch (action.ric_style_type) {
    case 1:
      ric_ind_message.ind_msg_format = e2_sm_kpm_ind_msg_s::ind_msg_formats_c_::types_opts::ind_msg_format1;
      _fill_ric_ind_msg_format1(action.action_definition_formats.action_definition_format1(), ric_ind_message);
      break;
    case 2:
      ric_ind_message.ind_msg_format = e2_sm_kpm_ind_msg_s::ind_msg_formats_c_::types_opts::ind_msg_format1;
      _fill_ric_ind_msg_format1(action.action_definition_formats.action_definition_format2(), ric_ind_message);
      break;
    case 3:
      ric_ind_message.ind_msg_format = e2_sm_kpm_ind_msg_s::ind_msg_formats_c_::types_opts::ind_msg_format2;
      _fill_ric_ind_msg_format2(action.action_definition_formats.action_definition_format3(), ric_ind_message);
      break;
    case 4:
      ric_ind_message.ind_msg_format = e2_sm_kpm_ind_msg_s::ind_msg_formats_c_::types_opts::ind_msg_format3;
      _fill_ric_ind_msg_format3(action.action_definition_formats.action_definition_format4(), ric_ind_message);
      break;
    case 5:
      ric_ind_message.ind_msg_format = e2_sm_kpm_ind_msg_s::ind_msg_formats_c_::types_opts::ind_msg_format3;
      _fill_ric_ind_msg_format3(action.action_definition_formats.action_definition_format5(), ric_ind_message);
      break;
    default:
      logger.info("Unknown RIC style type %i -> do not admit action %i (type %i)", action.ric_style_type);
      return false;
  }

  ric_indication.ri_cind_msg = srsran::make_byte_buffer();
  this->_generate_indication_message(ric_ind_message, ric_indication.ri_cind_msg);
  return true;
}

bool e2sm_kpm::_fill_ric_ind_msg_format1(e2_sm_kpm_action_definition_format1_s& action,
                                         E2SM_KPM_RIC_ind_message_t&            ric_ind_msg)
{
  cgi_c            cell_global_id;
  meas_info_list_l action_meas_info_list = action.meas_info_list;

  if (action.cell_global_id_present) {
    cell_global_id = action.cell_global_id;
  }

  // ric_ind_message.granul_period = action.granul_period; // not implemented by flexric and crashes it
  ric_ind_msg.granul_period = 0;

  ric_ind_msg.meas_info_list.resize(action_meas_info_list.size());
  ric_ind_msg.meas_data.resize(action_meas_info_list.size());

  for (uint32_t i = 0; i < action_meas_info_list.size(); i++) {
    // structs to fill
    meas_info_item_s& meas_info_item = ric_ind_msg.meas_info_list[i];
    meas_data_item_s& meas_data      = ric_ind_msg.meas_data[i];

    // measurements definition
    meas_info_item_s meas_def_item = action_meas_info_list[i];
    std::string      meas_name     = meas_def_item.meas_type.meas_name().to_string();

    meas_info_item.meas_type.set_meas_name().from_string(meas_name.c_str());
    meas_info_item.label_info_list.resize(meas_def_item.label_info_list.size());

    // TODO: add all labels defined in e2sm_kpm doc
    for (uint32_t l = 0; l < meas_def_item.label_info_list.size(); l++) {
      if (meas_def_item.label_info_list[l].meas_label.no_label_present) {
        meas_info_item.label_info_list[l].meas_label.no_label_present = true;
        meas_info_item.label_info_list[l].meas_label.no_label         = meas_label_s::no_label_opts::true_value;
        this->_fill_measurement_records(meas_name, NO_LABEL, meas_data.meas_record);
      }
      if (meas_def_item.label_info_list[l].meas_label.min_present) {
        meas_info_item.label_info_list[l].meas_label.min_present = true;
        meas_info_item.label_info_list[l].meas_label.min         = meas_label_s::min_opts::true_value;
        this->_fill_measurement_records(meas_name, MIN_LABEL, meas_data.meas_record);
      }
      if (meas_def_item.label_info_list[l].meas_label.max_present) {
        meas_info_item.label_info_list[l].meas_label.max_present = true;
        meas_info_item.label_info_list[l].meas_label.max         = meas_label_s::max_opts::true_value;
        this->_fill_measurement_records(meas_name, MAX_LABEL, meas_data.meas_record);
      }
      if (meas_def_item.label_info_list[l].meas_label.avg_present) {
        meas_info_item.label_info_list[l].meas_label.avg_present = true;
        meas_info_item.label_info_list[l].meas_label.avg         = meas_label_s::avg_opts::true_value;
        this->_fill_measurement_records(meas_name, AVG_LABEL, meas_data.meas_record);
      }
      if (meas_def_item.label_info_list[l].meas_label.sum_present) {
        meas_info_item.label_info_list[l].meas_label.sum_present = true;
        meas_info_item.label_info_list[l].meas_label.sum         = meas_label_s::sum_opts::true_value;
        this->_fill_measurement_records(meas_name, SUM_LABEL, meas_data.meas_record);
      }
    }
  }

  return true;
}

bool e2sm_kpm::_fill_ric_ind_msg_format1(e2_sm_kpm_action_definition_format2_s& action,
                                         E2SM_KPM_RIC_ind_message_t&            ric_ind_msg)
{
  return false;
}

bool e2sm_kpm::_fill_ric_ind_msg_format2(e2_sm_kpm_action_definition_format3_s& action,
                                         E2SM_KPM_RIC_ind_message_t&            ric_ind_msg)
{
  return false;
}

bool e2sm_kpm::_fill_ric_ind_msg_format3(e2_sm_kpm_action_definition_format4_s& action,
                                         E2SM_KPM_RIC_ind_message_t&            ric_ind_msg)
{
  return false;
}

bool e2sm_kpm::_fill_ric_ind_msg_format3(e2_sm_kpm_action_definition_format5_s& action,
                                         E2SM_KPM_RIC_ind_message_t&            ric_ind_msg)
{
  return false;
}

bool e2sm_kpm::_get_meas_definition(std::string meas_name, E2SM_KPM_metric_t& def)
{
  // TODO: we need a generic string comparison, why do we need c_str() here?
  auto name_matches = [&meas_name](const E2SM_KPM_metric_t& x) { return x.name == meas_name.c_str(); };
  auto it           = std::find_if(supported_meas_types.begin(), supported_meas_types.end(), name_matches);
  if (it == supported_meas_types.end()) {
    return false;
  }
  def = *it;
  return true;
}

bool e2sm_kpm::_get_last_N_meas_values(uint32_t N, E2SM_KPM_meas_values_t& meas_values)
{
  // TODO: currently only N=1 supported
  return _get_last_meas_value(meas_values);
}

void e2sm_kpm::_fill_measurement_records(std::string meas_name, e2sm_kpm_label_enum label, meas_record_l& meas_record_list)
{
  E2SM_KPM_metric_t metric_definition;
  if (not _get_meas_definition(meas_name, metric_definition)) {
    logger.debug("No definition for measurement type \"%s\"", metric_definition.name);
    return;
  }

  E2SM_KPM_meas_values_t meas_values;
  meas_values.name = metric_definition.name;
  // TODO: check if label supported
  meas_values.label     = label;
  meas_values.data_type = metric_definition.data_type;
  if (not _get_last_N_meas_values(1, meas_values)) {
    logger.debug("No measurement values for type \"%s\" \n", metric_definition.name.c_str());
    return;
  }

  if ((meas_values.integer_values.size() + meas_values.real_values.size()) == 0) {
    logger.debug("No measurement values for type \"%s\" \n", metric_definition.name.c_str());
    meas_record_list.resize(1);
    meas_record_list[0].set_no_value();
    return;
  }

  if (meas_values.data_type == e2_metric_data_type_t::INTEGER) {
    meas_record_list.resize(meas_values.integer_values.size());
    for (uint32_t i = 0; i < meas_values.integer_values.size(); i++) {
      meas_record_list[i].set_integer() = meas_values.integer_values[i];
    }
  } else {
    // values.data_type == e2_metric_data_type_t::REAL
    meas_record_list.resize(meas_values.real_values.size());
    for (uint32_t i = 0; i < meas_values.real_values.size(); i++) {
      real_s real_value;
      // TODO: real value seems to be not supported in asn1???
      // real_value.value = values.real_values[i];
      meas_record_list[i].set_real() = real_value;
    }
  }
}

bool e2sm_kpm::_generate_indication_header(E2SM_KPM_RIC_ind_header_t hdr, srsran::unique_byte_buffer_t& buf)
{
  e2_sm_kpm_ind_hdr_s e2_sm_kpm_ind_hdr;
  e2_sm_kpm_ind_hdr.ind_hdr_formats.ind_hdr_format1().collet_start_time.from_number(hdr.collet_start_time);
  e2_sm_kpm_ind_hdr.ind_hdr_formats.ind_hdr_format1().file_formatversion.from_string(hdr.file_formatversion);
  e2_sm_kpm_ind_hdr.ind_hdr_formats.ind_hdr_format1().sender_name.from_string(hdr.sender_name);
  e2_sm_kpm_ind_hdr.ind_hdr_formats.ind_hdr_format1().sender_type.from_string(hdr.sender_type);
  e2_sm_kpm_ind_hdr.ind_hdr_formats.ind_hdr_format1().vendor_name.from_string(hdr.vendor_name);

  asn1::bit_ref bref(buf->msg, buf->get_tailroom());
  if (e2_sm_kpm_ind_hdr.pack(bref) != asn1::SRSASN_SUCCESS) {
    printf("IND HEADER: Failed to pack TX E2 PDU\n");
    return false;
  }
  buf->N_bytes = bref.distance_bytes();

  return true;
}

bool e2sm_kpm::_generate_indication_message(E2SM_KPM_RIC_ind_message_t msg, srsran::unique_byte_buffer_t& buf)
{
  e2_sm_kpm_ind_msg_s e2_sm_kpm_ind_msg;

  switch (msg.ind_msg_format) {
    case e2_sm_kpm_ind_msg_s::ind_msg_formats_c_::types::ind_msg_format1:
      e2_sm_kpm_ind_msg.ind_msg_formats.set_ind_msg_format1();
      e2_sm_kpm_ind_msg.ind_msg_formats.ind_msg_format1().meas_data      = msg.meas_data;
      e2_sm_kpm_ind_msg.ind_msg_formats.ind_msg_format1().meas_info_list = msg.meas_info_list;

      if (msg.granul_period) {
        e2_sm_kpm_ind_msg.ind_msg_formats.ind_msg_format1().granul_period_present = true;
        e2_sm_kpm_ind_msg.ind_msg_formats.ind_msg_format1().granul_period         = msg.granul_period;
      }

      break;
    case e2_sm_kpm_ind_msg_s::ind_msg_formats_c_::types::ind_msg_format2:
      e2_sm_kpm_ind_msg.ind_msg_formats.set_ind_msg_format2();
      // TODO: support format2
      break;
    case e2_sm_kpm_ind_msg_s::ind_msg_formats_c_::types::ind_msg_format3:
      e2_sm_kpm_ind_msg.ind_msg_formats.set_ind_msg_format3();
      // TODO: support format3
      break;
    case e2_sm_kpm_ind_msg_s::ind_msg_formats_c_::types::nulltype:
      break;
    default:
      log_invalid_choice_id(msg.ind_msg_format, "e2_sm_kpm_ind_msg_s::ind_msg_formats_c_");
  }

  logger.info("Generating E2-SM-KPM Indication Message");
  asn1::bit_ref bref(buf->msg, buf->get_tailroom());
  if (e2_sm_kpm_ind_msg.pack(bref) != asn1::SRSASN_SUCCESS) {
    printf("IND MSG: Failed to pack TX E2 PDU\n");
    return false;
  }
  buf->N_bytes = bref.distance_bytes();

  return true;
}

bool e2sm_kpm::_get_last_meas_value(E2SM_KPM_meas_values_t& meas_values)
{
  meas_values.data_type =
      e2_metric_data_type_t::INTEGER; // TODO: overwrite as REAL seems to be not supported by asn1 now

  if (meas_values.data_type == e2_metric_data_type_t::INTEGER) {
    int32_t value;
    if (_extract_last_integer_type_meas_value(meas_values, last_enb_metrics, value)) {
      meas_values.integer_values.push_back(value);
    }
  } else {
    // values.data_type == e2_metric_data_type_t::REAL
    float value;
    if (_extract_last_real_type_meas_value(meas_values, last_enb_metrics, value)) {
      meas_values.real_values.push_back(value);
    }
  }

  return true;
}

bool e2sm_kpm::_extract_last_integer_type_meas_value(E2SM_KPM_meas_values_t& meas_values,
                                                     const enb_metrics_t&    enb_metrics,
                                                     int32_t&                value)
{
  // TODO: maybe add ID to metric types in e2sm_kpm_metrics definitions, so we do not have to compare strings?
  // TODO: make string comparison case insensitive
  // all integer type measurements
  // test: no_label
  if (meas_values.name.c_str() == std::string("test")) {
    switch (meas_values.label) {
      case NO_LABEL:
        value = (int32_t)enb_metrics.sys.cpu_load[0];
        printf("report last \"test\" value as int, (filled with CPU0_load) value %i \n", value);
        return true;
      default:
        return false;
    }
  }

  // random_int: no_label
  if (meas_values.name.c_str() == std::string("random_int")) {
    switch (meas_values.label) {
      case NO_LABEL:
        value = srsran_random_uniform_int_dist(random_gen, 0, 100);
        printf("report last \"random_int\" value as int, random value %i \n", value);
        return true;
      default:
        return false;
    }
  }

  return false;
}

bool e2sm_kpm::_extract_last_real_type_meas_value(E2SM_KPM_meas_values_t& meas_values,
                                                  const enb_metrics_t&    enb_metrics,
                                                  float&                  value)
{
  // all real type measurements
  // cpu0_load: no_label
  if (meas_values.name.c_str() == std::string("cpu0_load")) {
    switch (meas_values.label) {
      case NO_LABEL:
        value = enb_metrics.sys.cpu_load[0];
        return true;
      default:
        return false;
    }
  }

  // cpu_load: min,max,avg
  if (meas_values.name.c_str() == std::string("cpu_load")) {
    uint32_t size;
    switch (meas_values.label) {
      case MIN_LABEL:
        value = *std::min_element(enb_metrics.sys.cpu_load.begin(), enb_metrics.sys.cpu_load.end());
        return true;
      case MAX_LABEL:
        value = *std::max_element(enb_metrics.sys.cpu_load.begin(), enb_metrics.sys.cpu_load.end());
        return true;
      case AVG_LABEL:
        size  = enb_metrics.sys.cpu_load.size();
        value = std::accumulate(enb_metrics.sys.cpu_load.begin(), enb_metrics.sys.cpu_load.end(), 0.0 / size);
        return true;
      default:
        return false;
    }
  }

  return false;
}

e2sm_kpm_label_enum str2kpm_label(const std::string& label_str)
{
  std::string label_str_uc{label_str};
  std::transform(label_str_uc.cbegin(), label_str_uc.cend(), label_str_uc.begin(), [](unsigned char c) {
    return std::toupper(c);
  });

  if(label_str_uc == "NO_LABEL") return NO_LABEL;
  else if(label_str_uc == "MIN_LABEL") return MIN_LABEL;
  else if(label_str_uc == "MAX_LABEL") return MAX_LABEL;
  else if(label_str_uc == "AVG_LABEL") return AVG_LABEL;
  return UNKNOWN_LABEL;
}