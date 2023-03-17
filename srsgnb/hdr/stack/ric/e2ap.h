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

#include "e2sm_kpm.h"
#include "srsran/asn1/e2ap.h"
#include "srsran/asn1/e2sm_kpm_v2.h"
#include "srsran/interfaces/e2_metrics_interface.h"
#include "srsran/interfaces/enb_metrics_interface.h"
#include "srsran/srsran.h"

#ifndef RIC_E2AP_H
#define RIC_E2AP_H

using namespace asn1::e2ap;
using namespace asn1::e2sm_kpm;

typedef struct {
  uint16_t ric_id;
  uint16_t plmn_id;
} global_ric_id_t;

typedef struct {
  e2node_component_interface_type_e interface_type;
  std::string                       amf_name;
  std::string                       request_part;
  std::string                       response_part;
} e2_node_component_t;

class e2ap
{
public:
  e2ap(srslog::basic_logger& logger, srsenb::e2_interface_metrics* _gnb_metrics);
  e2_ap_pdu_c generate_setup_request();
  int         process_setup_response(e2setup_resp_s setup_response);
  int         process_setup_failure();
  int         process_subscription_request(ricsubscription_request_s subscription_request);
  e2_ap_pdu_c generate_subscription_response();
  int         generate_subscription_failure();
  int         generate_indication();
  e2_ap_pdu_c generate_reset_request();
  e2_ap_pdu_c generate_reset_response();
  int         process_reset_request(reset_request_s reset_request);
  int         process_reset_response(reset_resp_s reset_response);
  int         get_reset_id();
  bool        has_setup_response() { return setup_response_received; }

private:
  srslog::basic_logger& logger;
  e2sm_kpm              e2sm_;
  bool                  setup_response_received        = false;
  bool                                        pending_subscription_request   = false;
  int                                         setup_procedure_transaction_id = 0;
  uint64_t                                    plmn_id                        = 0x05f510;
  uint64_t                                    gnb_id                         = 1;
  global_ric_id_t                             global_ric_id = {};
  std::map<uint32_t, RANfunction_description> ran_functions;
  srsenb::e2_interface_metrics*               gnb_metrics = nullptr;
  bool    reset_response_received             = false;
  int     reset_transaction_id                = 1;
  cause_c reset_cause                         = cause_c();
  int     reset_id                            = 1;
};

#endif /* RIC_E2AP_H */
