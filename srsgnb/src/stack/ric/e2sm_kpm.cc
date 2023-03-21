#include "srsgnb/hdr/stack/ric/e2sm_kpm.h"

const std::string e2sm_kpm::short_name       = "ORAN-E2SM-KPM";
const std::string e2sm_kpm::oid              = "1.3.6.1.4.1.53148.1.2.2.2";
const std::string e2sm_kpm::func_description = "KPM Monitor";
const uint32_t    e2sm_kpm::revision         = 0;

e2sm_kpm::e2sm_kpm(srslog::basic_logger& logger_) : e2sm(short_name, oid, func_description, revision), logger(logger_)
{
  supported_meas_types.push_back("RRU.PrbTotDl");
  supported_meas_types.push_back("RRU.PrbTotUl");
  supported_meas_types.push_back("test");
}

bool e2sm_kpm::generate_ran_function_description(RANfunction_description& desc, srsran::unique_byte_buffer_t& buf)
{
  using namespace asn1::e2sm_kpm;
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

bool e2sm_kpm::process_subscription_request(asn1::e2ap::ricsubscription_request_s subscription_request,
                                            E2SM_KPM_RIC_event_definition&        event_def)
{
  using namespace asn1::e2sm_kpm;
  e2_sm_kpm_event_trigger_definition_s trigger_def;
  asn1::cbit_ref bref(subscription_request->ricsubscription_details->ric_event_trigger_definition.data(),
                      subscription_request->ricsubscription_details->ric_event_trigger_definition.size());

  if (trigger_def.unpack(bref) != asn1::SRSASN_SUCCESS) {
    return false;
  }

  event_def.report_period = trigger_def.event_definition_formats.event_definition_format1().report_period;
  return true;
}

bool e2sm_kpm::process_ric_action_definition(asn1::e2ap::ri_caction_to_be_setup_item_s ric_action)
{
  using namespace asn1::e2sm_kpm;
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

bool e2sm_kpm::generate_indication_header(E2SM_KPM_RIC_ind_header hdr, srsran::unique_byte_buffer_t& buf)
{
  using namespace asn1::e2sm_kpm;
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

bool e2sm_kpm::generate_indication_message(E2SM_KPM_RIC_ind_message msg, srsran::unique_byte_buffer_t& buf)
{
  using namespace asn1::e2sm_kpm;
  e2_sm_kpm_ind_msg_s e2_sm_kpm_ind_msg;

  switch (msg.ind_msg_format) {
    case asn1::e2sm_kpm::e2_sm_kpm_ind_msg_s::ind_msg_formats_c_::types::ind_msg_format1:
      e2_sm_kpm_ind_msg.ind_msg_formats.set_ind_msg_format1();
      e2_sm_kpm_ind_msg.ind_msg_formats.ind_msg_format1().meas_data      = msg.meas_data;
      e2_sm_kpm_ind_msg.ind_msg_formats.ind_msg_format1().meas_info_list = msg.meas_info_list;

      if (msg.granul_period) {
        e2_sm_kpm_ind_msg.ind_msg_formats.ind_msg_format1().granul_period_present = true;
        e2_sm_kpm_ind_msg.ind_msg_formats.ind_msg_format1().granul_period         = msg.granul_period;
      }

      break;
    case asn1::e2sm_kpm::e2_sm_kpm_ind_msg_s::ind_msg_formats_c_::types::ind_msg_format2:
      e2_sm_kpm_ind_msg.ind_msg_formats.set_ind_msg_format2();
      // TODO: support format2
      break;
    case asn1::e2sm_kpm::e2_sm_kpm_ind_msg_s::ind_msg_formats_c_::types::ind_msg_format3:
      e2_sm_kpm_ind_msg.ind_msg_formats.set_ind_msg_format3();
      // TODO: support format3
      break;
    case asn1::e2sm_kpm::e2_sm_kpm_ind_msg_s::ind_msg_formats_c_::types::nulltype:
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
