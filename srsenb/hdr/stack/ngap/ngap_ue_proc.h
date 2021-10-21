/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#ifndef SRSENB_NGAP_UE_PROC_H
#define SRSENB_NGAP_UE_PROC_H

#include "ngap_interfaces.h"
#include "ngap_ue_utils.h"

#include "srsenb/hdr/stack/ngap/ngap_ue_bearer_manager.h"
#include "srsran/asn1/asn1_utils.h"
#include "srsran/asn1/ngap.h"
#include "srsran/common/buffer_pool.h"
#include "srsran/common/stack_procedure.h"
#include "srsran/interfaces/gnb_rrc_nr_interfaces.h"

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
                                           ngap_ue_bearer_manager*   bearer_manager,
                                           srslog::basic_logger&     logger_);
  srsran::proc_outcome_t init(const asn1::ngap_nr::ue_context_release_cmd_s& msg);
  srsran::proc_outcome_t step();
  static const char*     name() { return "UE Context Release"; }

private:
  ngap_ue_ctxt_t*           ue_ctxt;
  ngap_interface_ngap_proc* parent         = nullptr;
  rrc_interface_ngap_nr*    rrc            = nullptr;
  ngap_ue_bearer_manager*   bearer_manager = nullptr;
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