/**
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srsran/asn1/e2ap.h"
#include "srsran/common/byte_buffer.h"
#include "srsran/common/task_scheduler.h"
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
  e2sm(std::string             short_name,
       std::string             oid,
       std::string             func_description,
       uint32_t                revision,
       srsran::task_scheduler* _task_sched_ptr) :
    _short_name(short_name),
    _oid(oid),
    _func_description(func_description),
    _revision(revision),
    task_sched_ptr(_task_sched_ptr){};
  virtual ~e2sm() = default;

  std::string get_short_name() { return _short_name; };
  std::string get_oid() { return _oid; };
  std::string get_func_description() { return _func_description; };
  uint32_t    get_revision() { return _revision; };

  virtual bool generate_ran_function_description(RANfunction_description& desc, ra_nfunction_item_s& ran_func)       = 0;
  virtual bool process_ric_event_trigger_definition(ricsubscription_request_s       subscription_request,
                                                    RIC_event_trigger_definition_t& event_def)                       = 0;
  virtual bool process_ric_action_definition(ri_caction_to_be_setup_item_s ric_action,
                                             E2AP_RIC_action_t&            action_entry)                             = 0;
  virtual bool remove_ric_action_definition(E2AP_RIC_action_t& action_entry)                                         = 0;
  virtual bool generate_ric_indication_content(E2AP_RIC_action_t& action_entry, ric_indication_t& ric_indication)    = 0;

  virtual void receive_e2_metrics_callback(const enb_metrics_t& m) = 0;

protected:
  uint32_t _get_local_action_id() { return _registered_action_id_gen; };
  uint32_t _generate_new_local_action_id() { return _registered_action_id_gen++; };

  srsran::task_scheduler* task_sched_ptr = nullptr;

private:
  const std::string _short_name;
  const std::string _oid;
  const std::string _func_description;
  const uint32_t    _revision = 0;

  uint32_t _registered_action_id_gen = 1000;
};

struct RANfunction_description {
  bool        accepted          = false;
  int         function_instance = 0;
  e2sm*       sm_ptr            = nullptr;
  std::string function_shortname;
  std::string function_e2_sm_oid;
  std::string function_desc;
};

#endif // SRSRAN_E2SM_H
