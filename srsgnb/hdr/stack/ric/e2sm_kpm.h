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
#include "srsran/asn1/e2sm_kpm.h"
#include "srsran/srsran.h"

#ifndef RIC_E2SM_KPM_H
#define RIC_E2SM_KPM_H

class e2sm_kpm
{
public:
  e2sm_kpm(srslog::basic_logger& logger_);
  bool generate_ran_function_description(srsran::unique_byte_buffer_t& buf);
  bool process_ric_action_definition();
  bool generate_indication_header();
  bool generate_indication_message();

private:
  srslog::basic_logger& logger;
};

#endif /*E2SM_KPM*/
