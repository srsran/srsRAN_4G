/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSENB_NGAP_UE_PROC_H
#define SRSENB_NGAP_UE_PROC_H

#include "ngap_interfaces.h"
#include "ngap_ue_utils.h"

#include "srsran/asn1/asn1_utils.h"
#include "srsran/asn1/ngap.h"
#include "srsran/common/buffer_pool.h"
#include "srsran/common/stack_procedure.h"
#include "srsran/interfaces/gnb_rrc_nr_interfaces.h"
#include "srsenb/hdr/stack/ngap/ngap_ue_bearer_manager.h"

#include <map>
#include <string>

namespace srsenb {

// TS 38.413 - Section 8.3 - UE Context Management Procedures

// TS 38.413 - Section 8.3.1 - Initial Context Setup
class ngap_ue_initial_context_setup_proc
{
public:
  explicit ngap_ue_initial_context_setup_proc(ngap_interface_ngap_proc* parent_,
                                              rrc_interface_ngap_nr*    rrc_,
                                              ngap_ue_ctxt_t*           ue_ctxt,
                                              srslog::basic_logger&     logger_);
  srsran::proc_outcome_t init(const asn1::ngap_nr::init_context_setup_request_s& msg);
  srsran::proc_outcome_t react(const bool rrc_reconf_outcome);
  srsran::proc_outcome_t step();
  static const char*     name() { return "Initial Context Setup"; }

private:
  ngap_ue_ctxt_t*           ue_ctxt;
  ngap_interface_ngap_proc* parent = nullptr;
  rrc_interface_ngap_nr*    rrc    = nullptr;
  srslog::basic_logger&     logger;
};

// TS 38.413 - Section 8.3.2 - UE Context Release Request (NG-RAN node initiated)
class ngap_ue_ue_context_release_proc
{
public:
  explicit ngap_ue_ue_context_release_proc(ngap_interface_ngap_proc* parent_,
                                           rrc_interface_ngap_nr*    rrc_,
                                           ngap_ue_ctxt_t*           ue_ctxt,
                                           srslog::basic_logger&     logger_);
  srsran::proc_outcome_t init(const asn1::ngap_nr::ue_context_release_cmd_s& msg);
  srsran::proc_outcome_t step();
  static const char*     name() { return "UE Context Release"; }

private:
  ngap_ue_ctxt_t*           ue_ctxt;
  ngap_interface_ngap_proc* parent = nullptr;
  rrc_interface_ngap_nr*    rrc    = nullptr;
  srslog::basic_logger&     logger;
};

// TS 38.413 - Section 8.3.4 - UE Context Modification
class ngap_ue_ue_context_modification_proc
{
public:
  explicit ngap_ue_ue_context_modification_proc(ngap_interface_ngap_proc* parent_, srslog::basic_logger& logger_);
  srsran::proc_outcome_t init();
  srsran::proc_outcome_t step();
  static const char*     name() { return "UE Context Modification"; }

private:
  ngap_interface_ngap_proc* parent;
};

class ngap_ue_pdu_session_res_setup_proc
{
public:
  explicit ngap_ue_pdu_session_res_setup_proc(ngap_interface_ngap_proc* parent_,
                                              rrc_interface_ngap_nr*    rrc_,
                                              ngap_ue_ctxt_t*           ue_ctxt,
                                              ngap_ue_bearer_manager*   bearer_manager,
                                              srslog::basic_logger&     logger_);
  srsran::proc_outcome_t init(const asn1::ngap_nr::pdu_session_res_setup_request_s& msg);
  srsran::proc_outcome_t step();
  static const char*     name() { return "UE PDU Session Resource Setup"; }

private:
  ngap_ue_ctxt_t*           ue_ctxt;
  ngap_interface_ngap_proc* parent;
  ngap_ue_bearer_manager*   bearer_manager;
  rrc_interface_ngap_nr*    rrc = nullptr;
  srslog::basic_logger&     logger;
};

} // namespace srsenb

#endif