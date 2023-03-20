#include "srsgnb/hdr/stack/ric/e2sm_kpm.h"

e2sm_kpm::e2sm_kpm(srslog::basic_logger& logger_) : logger(logger_) {}

bool e2sm_kpm::generate_ran_function_description(int                           function_id,
                                                 RANfunction_description       desc,
                                                 srsran::unique_byte_buffer_t& buf)
{
  using namespace asn1::e2sm_kpm;
  e2_sm_kpm_ra_nfunction_description_s e2sm_kpm_ra_nfunction_description;
  e2sm_kpm_ra_nfunction_description.ran_function_name.ran_function_instance_present = false;
  // e2sm_kpm_ra_nfunction_description.ran_function_name.ran_function_instance         = desc.function_instance;
  e2sm_kpm_ra_nfunction_description.ran_function_name.ran_function_short_name.from_string(desc.function_shortname);
  e2sm_kpm_ra_nfunction_description.ran_function_name.ran_function_e2_sm_oid.from_string(desc.function_e2_sm_oid);
  e2sm_kpm_ra_nfunction_description.ran_function_name.ran_function_description.from_string(desc.function_desc);

  /*
  e2sm_kpm_ra_nfunction_description.e2_sm_kpm_ra_nfunction_item.ric_event_trigger_style_list.resize(1);
  auto& ric_event_trigger_style_list_item =
      e2sm_kpm_ra_nfunction_description.e2_sm_kpm_ra_nfunction_item.ric_event_trigger_style_list[0];
  ric_event_trigger_style_list_item.ric_event_trigger_format_type = 5;
  ric_event_trigger_style_list_item.ric_event_trigger_style_type  = 1;
  ric_event_trigger_style_list_item.ric_event_trigger_style_name.from_string("Periodic report");
  auto& list = e2sm_kpm_ra_nfunction_description.e2_sm_kpm_ra_nfunction_item.ric_report_style_list;
  list.resize(6);
  for (int i = 0; i < (int)list.size(); i++) {
    auto& ric_report_style_list_item =
        e2sm_kpm_ra_nfunction_description.e2_sm_kpm_ra_nfunction_item.ric_report_style_list[i];
    ric_report_style_list_item.ric_ind_hdr_format_type = 1;
    ric_report_style_list_item.ric_ind_msg_format_type = 1;
    ric_report_style_list_item.ric_report_style_type   = i + 1;
    switch (i) {
      case 0:
        ric_report_style_list_item.ric_report_style_name.from_string(
            "O-DU Measurement Container for the 5GC connected deployment");
        break;
      case 1:
        ric_report_style_list_item.ric_report_style_name.from_string(
            "O-DU Measurement Container for the EPC connected deployment");
        break;
      case 2:
        ric_report_style_list_item.ric_report_style_name.from_string(
            "O-CU-CP Measurement Container for the 5GC connected deployment");
        break;
      case 3:
        ric_report_style_list_item.ric_report_style_name.from_string(
            "O-CU-CP Measurement Container for the EPC connected deployment");
        break;
      case 4:
        ric_report_style_list_item.ric_report_style_name.from_string(
            "O-CU-UP Measurement Container for the 5GC connected deployment");
        break;
      case 5:
        ric_report_style_list_item.ric_report_style_name.from_string(
            "O-CU-UP Measurement Container for the EPC connected deployment");
        break;
    };
    ric_report_style_list_item.ric_report_style_name.from_string("Periodic report");
  }
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

int e2sm_kpm::process_ric_action_definition()
{
  return 0;
}

bool e2sm_kpm::generate_indication_header(RIC_indication_header hdr, srsran::unique_byte_buffer_t& buf)
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

bool e2sm_kpm::generate_indication_message(RIC_indication_message msg, srsran::unique_byte_buffer_t& buf)
{
  using namespace asn1::e2sm_kpm;
  e2_sm_kpm_ind_msg_s e2_sm_kpm_ind_msg;

  switch (msg.ind_msg_format) {
    case asn1::e2sm_kpm::e2_sm_kpm_ind_msg_s::ind_msg_formats_c_::types::ind_msg_format1:
      e2_sm_kpm_ind_msg.ind_msg_formats.set_ind_msg_format1();
      e2_sm_kpm_ind_msg.ind_msg_formats.ind_msg_format1().meas_data      = msg.meas_data;
      e2_sm_kpm_ind_msg.ind_msg_formats.ind_msg_format1().meas_info_list = msg.meas_info_list;
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
