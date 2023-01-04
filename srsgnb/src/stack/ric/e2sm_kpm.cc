#include "srsgnb/hdr/stack/ric/e2sm_kpm.h"

e2sm_kpm::e2sm_kpm(srslog::basic_logger& logger_) : logger(logger_) {}

bool e2sm_kpm::generate_ran_function_description(srsran::unique_byte_buffer_t& buf)
{
  using namespace asn1::e2sm_kpm;
  e2_sm_kpm_ra_nfunction_description_s e2sm_kpm_ra_nfunction_description;
  e2sm_kpm_ra_nfunction_description.ran_function_name.ran_function_instance_present = true;
  e2sm_kpm_ra_nfunction_description.ran_function_name.ran_function_instance         = 0;
  e2sm_kpm_ra_nfunction_description.ran_function_name.ran_function_short_name.from_string("ORAN-E2SM-KPM");
  e2sm_kpm_ra_nfunction_description.ran_function_name.ran_function_e2_sm_oid.from_string("OID123");
  e2sm_kpm_ra_nfunction_description.ran_function_name.ran_function_description.from_string("KPM monitor");

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

  logger.info("Generating RAN function description");
  asn1::bit_ref bref(buf->msg, buf->get_tailroom());
  if (e2sm_kpm_ra_nfunction_description.pack(bref) != asn1::SRSASN_SUCCESS) {
    printf("Failed to pack TX E2 PDU\n");
    return false;
  }
  buf->N_bytes = bref.distance_bytes();
  return true;
}
