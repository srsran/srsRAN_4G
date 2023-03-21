#include "srsgnb/hdr/stack/ric/e2sm_kpm.h"

const std::string e2sm_kpm::short_name       = "ORAN-E2SM-KPM";
const std::string e2sm_kpm::oid              = "1.3.6.1.4.1.53148.1.2.2.2";
const std::string e2sm_kpm::func_description = "KPM Monitor";
const uint32_t    e2sm_kpm::revision         = 0;

e2sm_kpm::e2sm_kpm(srslog::basic_logger& logger_) : e2sm(short_name, oid, func_description, revision), logger(logger_)
{
  random_gen = srsran_random_init(1234);

  supported_meas_types.push_back("RRU.PrbTotDl");
  supported_meas_types.push_back("RRU.PrbTotUl");
  supported_meas_types.push_back("test");
}

bool e2sm_kpm::generate_ran_function_description(RANfunction_description& desc, srsran::unique_byte_buffer_t& buf)
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
  asn1::bit_ref bref(buf->msg, buf->get_tailroom());
  if (e2sm_kpm_ra_nfunction_description.pack(bref) != asn1::SRSASN_SUCCESS) {
    printf("Failed to pack TX E2 PDU\n");
    return false;
  }
  buf->N_bytes = bref.distance_bytes();
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
  e2_sm_kpm_action_definition_s e2sm_kpm_action_def;
  asn1::cbit_ref                bref(ric_action.ric_action_definition.data(), ric_action.ric_action_definition.size());

  uint64_t         granul_period;
  uint64_t         eutra_cell_id;
  uint64_t         plmn_id;
  ueid_c           ue_id;
  meas_info_list_l meas_info_list;

  if (e2sm_kpm_action_def.unpack(bref) != asn1::SRSASN_SUCCESS) {
    return false;
  }

  switch (e2sm_kpm_action_def.ric_style_type) {
    case 1:
      granul_period = e2sm_kpm_action_def.action_definition_formats.action_definition_format1().granul_period;

      if (granul_period == 0) {
        logger.debug("Action granularity period of %i is not supported -> do not admitted action %i",
                     granul_period,
                     ric_action.ric_action_id);
        return false;
      }

      if (e2sm_kpm_action_def.action_definition_formats.action_definition_format1().cell_global_id_present) {
        if (e2sm_kpm_action_def.action_definition_formats.action_definition_format1().cell_global_id.type() ==
            cgi_c::types_opts::eutra_cgi) {
          eutra_cell_id = e2sm_kpm_action_def.action_definition_formats.action_definition_format1()
                              .cell_global_id.eutra_cgi()
                              .eutra_cell_id.to_number();
          plmn_id = e2sm_kpm_action_def.action_definition_formats.action_definition_format1()
                        .cell_global_id.eutra_cgi()
                        .plmn_id.to_number();
          logger.debug("plmn_id 0x%x, eutra_cell_id %i", plmn_id, eutra_cell_id);
          // TODO: check if E2 node has cell_id and plmn_id
        }
      }

      meas_info_list = e2sm_kpm_action_def.action_definition_formats.action_definition_format1().meas_info_list;
      for (uint32_t i = 0; i < meas_info_list.size(); i++) {
        std::string meas_name = meas_info_list[i].meas_type.meas_name().to_string();
        if (std::find(supported_meas_types.begin(), supported_meas_types.end(), meas_name.c_str()) ==
            supported_meas_types.end()) {
          printf("Unsupported measurement name: %s --> do not admit action %i \n",
                 meas_name.c_str(),
                 ric_action.ric_action_id);
          return false;
        }

        printf("Admitted action: measurement name: %s with the following labels: \n", meas_name.c_str());
        for (uint32_t l = 0; l < meas_info_list[i].label_info_list.size(); l++) {
          if (meas_info_list[i].label_info_list[l].meas_label.no_label_present) {
            printf("--- Label %i: NO LABEL\n", i);
          }
          if (meas_info_list[i].label_info_list[l].meas_label.min_present) {
            printf("--- Label %i: MIN\n", i);
          }
          if (meas_info_list[i].label_info_list[l].meas_label.max_present) {
            printf("--- Label %i: MAX\n", i);
          }
          if (meas_info_list[i].label_info_list[l].meas_label.avg_present) {
            printf("--- Label %i: AVG\n", i);
          }
        }
      }
      action_entry.sm_local_ric_action_id = _generate_local_action_id();
      registered_actions.insert(
          std::pair<uint32_t, e2_sm_kpm_action_definition_s>(action_entry.sm_local_ric_action_id, e2sm_kpm_action_def));
      break;
    default:
      logger.info("Unknown RIC style type %i -> do not admit action %i (type %i)",
                  e2sm_kpm_action_def.ric_style_type,
                  ric_action.ric_action_id,
                  ric_action.ric_action_type);
      return false;
  }

  return true;
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
  uint64_t                 granul_period;
  meas_info_list_l         action_meas_info_list;

  ric_indication.indication_type       = ri_cind_type_opts::report;
  e2_sm_kpm_action_definition_s action = registered_actions.at(action_entry.sm_local_ric_action_id);

  if (action.ric_style_type == 1) {
    granul_period         = action.action_definition_formats.action_definition_format1().granul_period;
    action_meas_info_list = action.action_definition_formats.action_definition_format1().meas_info_list;

    ric_ind_header.collet_start_time = std::time(0);
    ric_indication.ri_cind_hdr       = srsran::make_byte_buffer();
    this->generate_indication_header(ric_ind_header, ric_indication.ri_cind_hdr);

    ric_ind_message.ind_msg_format = e2_sm_kpm_ind_msg_s::ind_msg_formats_c_::types_opts::ind_msg_format1;
    // ric_ind_message.granul_period = granul_period; // not implemented by flexric and crashes it
    ric_ind_message.granul_period = 0;

    ric_ind_message.meas_info_list.resize(action_meas_info_list.size());
    ric_ind_message.meas_data.resize(action_meas_info_list.size());
    for (uint32_t i = 0; i < action_meas_info_list.size(); i++) {
      // structs to fill
      meas_info_item_s& meas_info_item = ric_ind_message.meas_info_list[i];
      meas_data_item_s& meas_data      = ric_ind_message.meas_data[i];

      // measure definition
      meas_info_item_s meas_def_item = action_meas_info_list[i];
      std::string      meas_name     = meas_def_item.meas_type.meas_name().to_string();

      meas_info_item.meas_type.set_meas_name().from_string(meas_name.c_str());
      meas_info_item.label_info_list.resize(meas_def_item.label_info_list.size());

      for (uint32_t l = 0; l < meas_def_item.label_info_list.size(); l++) {
        if (meas_def_item.label_info_list[l].meas_label.no_label_present) {
          meas_info_item.label_info_list[l].meas_label.no_label_present = true;
          meas_info_item.label_info_list[l].meas_label.no_label         = meas_label_s::no_label_opts::true_value;
          this->_fill_measurement_records(meas_name, "no_label", meas_data.meas_record);
        }
        if (meas_def_item.label_info_list[l].meas_label.min_present) {
          meas_info_item.label_info_list[l].meas_label.min_present = true;
          meas_info_item.label_info_list[l].meas_label.min         = meas_label_s::min_opts::true_value;
          this->_fill_measurement_records(meas_name, "min", meas_data.meas_record);
        }
        if (meas_def_item.label_info_list[l].meas_label.max_present) {
          meas_info_item.label_info_list[l].meas_label.max_present = true;
          meas_info_item.label_info_list[l].meas_label.max         = meas_label_s::max_opts::true_value;
          this->_fill_measurement_records(meas_name, "max", meas_data.meas_record);
        }
        if (meas_def_item.label_info_list[l].meas_label.avg_present) {
          meas_info_item.label_info_list[l].meas_label.avg_present = true;
          meas_info_item.label_info_list[l].meas_label.avg         = meas_label_s::avg_opts::true_value;
          this->_fill_measurement_records(meas_name, "avg", meas_data.meas_record);
        }
        if (meas_def_item.label_info_list[l].meas_label.sum_present) {
          meas_info_item.label_info_list[l].meas_label.sum_present = true;
          meas_info_item.label_info_list[l].meas_label.sum         = meas_label_s::sum_opts::true_value;
          this->_fill_measurement_records(meas_name, "sum", meas_data.meas_record);
        }
      }
    }
  }

  ric_indication.ri_cind_msg = srsran::make_byte_buffer();
  this->generate_indication_message(ric_ind_message, ric_indication.ri_cind_msg);
  return true;
}

void e2sm_kpm::_fill_measurement_records(std::string meas_name, std::string label, meas_record_l& meas_record_list)
{
  uint32_t nof_records = srsran_random_uniform_int_dist(random_gen, 3, 10);
  printf("Fill last N=%i measurements of %s value for label: %s\n", nof_records, meas_name.c_str(), label.c_str());

  meas_record_list.resize(nof_records);
  for (uint32_t i = 0; i < nof_records; i++) {
    meas_record_list[i].set_integer() = srsran_random_uniform_int_dist(random_gen, 0, 100);
  }
}

bool e2sm_kpm::generate_indication_header(E2SM_KPM_RIC_ind_header_t hdr, srsran::unique_byte_buffer_t& buf)
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

bool e2sm_kpm::generate_indication_message(E2SM_KPM_RIC_ind_message_t msg, srsran::unique_byte_buffer_t& buf)
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
