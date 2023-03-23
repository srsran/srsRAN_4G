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
#include "srsran/interfaces/enb_metrics_interface.h"
#include "srsran/srsran.h"

#ifndef SRSRAN_E2SM_H
#define SRSRAN_E2SM_H

using namespace asn1::e2ap;
using namespace srsenb;

struct RANfunction_description;

typedef struct {
  enum e2sm_event_trigger_type_t { E2SM_REPORT, E2SM_INSERT, E2SM_POLICY, UNKNOWN_TRIGGER };
  e2sm_event_trigger_type_t type;
  uint64_t                  report_period;
} RIC_event_trigger_definition_t;

typedef struct {
  uint16_t          ric_action_id;
  ri_caction_type_e ric_action_type;
  uint32_t          sm_local_ric_action_id;
} E2AP_RIC_action_t;

typedef struct {
  uint32_t                     ric_requestor_id;
  uint32_t                     ric_instance_id;
  uint32_t                     ra_nfunction_id;
  uint32_t                     ri_caction_id;
  bool                         ri_indication_sn_present;
  uint32_t                     ri_indication_sn;
  ri_cind_type_e               indication_type;
  srsran::unique_byte_buffer_t ri_cind_hdr;
  srsran::unique_byte_buffer_t ri_cind_msg;
} ric_indication_t;

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

  virtual bool generate_ran_function_description(RANfunction_description& desc, ra_nfunction_item_s& ran_func)       = 0;
  virtual bool process_ric_event_trigger_definition(ricsubscription_request_s     subscription_request,
                                                    RIC_event_trigger_definition_t& event_def)                       = 0;
  virtual bool process_ric_action_definition(ri_caction_to_be_setup_item_s ric_action,
                                             E2AP_RIC_action_t&              action_entry)                           = 0;
  virtual bool remove_ric_action_definition(E2AP_RIC_action_t& action_entry)                                         = 0;
  virtual bool execute_action_fill_ric_indication(E2AP_RIC_action_t& action_entry, ric_indication_t& ric_indication) = 0;

  virtual void receive_e2_metrics_callback(const enb_metrics_t& m) = 0;

protected:
  uint32_t _generate_local_action_id() { return _registered_action_id_gen++; };

private:
  const std::string _short_name;
  const std::string _oid;
  const std::string _func_description;
  const uint32_t    _revision;

  uint32_t _registered_action_id_gen = 1000;
};

struct RANfunction_description {
  bool        accepted          = false;
  int         function_instance = 0;
  e2sm*       sm_ptr;
  std::string function_shortname;
  std::string function_e2_sm_oid;
  std::string function_desc;
};

#endif // SRSRAN_E2SM_H
