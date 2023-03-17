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
#include "srsran/asn1/e2sm.h"
#include "srsran/asn1/e2sm_kpm_v2.h"
#include "srsran/srsran.h"

#ifndef RIC_E2SM_KPM_H
#define RIC_E2SM_KPM_H

struct RANfunction_description {
  bool        accepted = false;
  std::string function_desc;
  std::string function_shortname;
  std::string function_e2_sm_oid;
  int         function_instance;
};

class e2sm_kpm
{
public:
  e2sm_kpm(srslog::basic_logger& logger_);
  bool
  generate_ran_function_description(int function_id, RANfunction_description desc, srsran::unique_byte_buffer_t& buf);
  int  process_ric_action_definition();
  bool generate_indication_header(srsran::unique_byte_buffer_t& buf);
  bool generate_indication_message(srsran::unique_byte_buffer_t& buf);

private:
  srslog::basic_logger& logger;
};

#endif /*E2SM_KPM*/
