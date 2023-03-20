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
#include "srsran/asn1/e2sm.h"
#include "srsran/asn1/e2sm_kpm_v2.h"
#include "srsran/srsran.h"

#ifndef RIC_E2SM_KPM_H
#define RIC_E2SM_KPM_H

struct RIC_indication_header {
  uint32_t    collet_start_time;
  std::string file_formatversion;
  std::string sender_name;
  std::string sender_type;
  std::string vendor_name;
};

struct RIC_indication_message {
  asn1::e2sm_kpm::e2_sm_kpm_ind_msg_s::ind_msg_formats_c_::types ind_msg_format;
  asn1::e2sm_kpm::meas_data_l                                    meas_data;
  asn1::e2sm_kpm::meas_info_list_l                               meas_info_list;
  uint64_t                                                       granul_period = 0;
};

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
  int          process_ric_action_definition();
  bool         generate_indication_header(RIC_indication_header hdr, srsran::unique_byte_buffer_t& buf);
  bool         generate_indication_message(RIC_indication_message msg, srsran::unique_byte_buffer_t& buf);

private:
  srslog::basic_logger& logger;
};

#endif /*E2SM_KPM*/
