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

#include "srsran/asn1/e2ap.h"
#include "srsran/common/byte_buffer.h"
#include "srsran/srsran.h"

#ifndef SRSRAN_E2SM_H
#define SRSRAN_E2SM_H

enum e2sm_type_t { E2SM_KPM, UNKNOWN };

struct RANfunction_description;

struct RIC_event_trigger_definition {
  enum e2sm_event_trigger_type_t { E2SM_REPORT, E2SM_INSERT, E2SM_POLICY, UNKNOWN_TRIGGER };
  e2sm_event_trigger_type_t type;
  uint64_t                  report_period;
};

class e2sm
{
public:
  e2sm();
  e2sm(std::string short_name, std::string oid, std::string func_description, uint32_t revision) :
    _short_name(short_name), _oid(oid), _func_description(func_description), _revision(revision){};
  virtual ~e2sm() = default;

  std::string get_short_name() { return _short_name; };
  std::string get_oid() { return _oid; };
  std::string get_func_description() { return _func_description; };
  uint32_t    get_revision() { return _revision; };

  virtual bool generate_ran_function_description(RANfunction_description& desc, srsran::unique_byte_buffer_t& buf) = 0;
  virtual bool process_ric_event_trigger_definition(asn1::e2ap::ricsubscription_request_s subscription_request,
                                                    RIC_event_trigger_definition&         event_def)                       = 0;

private:
  const std::string _short_name;
  const std::string _oid;
  const std::string _func_description;
  const uint32_t    _revision;
};

struct RANfunction_description {
  bool        accepted          = false;
  int         function_instance = 0;
  e2sm_type_t sm_type;
  e2sm*       sm_ptr;
  std::string function_shortname;
  std::string function_e2_sm_oid;
  std::string function_desc;
};

#endif // SRSRAN_E2SM_H
