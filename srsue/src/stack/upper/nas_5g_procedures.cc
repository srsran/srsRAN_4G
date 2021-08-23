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

#include "srsue/hdr/stack/upper/nas_5g_procedures.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <unistd.h>

using namespace srsran;

namespace srsue {

nas_5g::registration_procedure::registration_procedure(nas_5g_interface_procedures* parent_nas_) :
  parent_nas(parent_nas_)
{}

srsran::proc_outcome_t nas_5g::registration_procedure::init()
{
  parent_nas->send_registration_request();
  return srsran::proc_outcome_t::yield;
}
srsran::proc_outcome_t nas_5g::registration_procedure::step()
{
  return srsran::proc_outcome_t::success;
}

// PDU Sessions Establishment Procedure
nas_5g::pdu_session_establishment_procedure::pdu_session_establishment_procedure(
    nas_5g_interface_procedures* parent_nas_,
    srslog::basic_logger&        logger_) :
  logger(logger_), parent_nas(parent_nas_)
{}

srsran::proc_outcome_t nas_5g::pdu_session_establishment_procedure::init(const uint16_t          pdu_session_id_,
                                                                         const pdu_session_cfg_t pdu_session_cfg)
{
  // Get PDU transaction identity
  transaction_identity = parent_nas->allocate_next_proc_trans_id();
  pdu_session_id       = pdu_session_id_;
  parent_nas->send_pdu_session_establishment_request(transaction_identity, pdu_session_id, pdu_session_cfg);

  return srsran::proc_outcome_t::yield;
}

srsran::proc_outcome_t nas_5g::pdu_session_establishment_procedure::react(
    const srsran::nas_5g::pdu_session_establishment_accept_t& pdu_session_est_accept)
{
  // TODO check the pdu session values
  if (pdu_session_est_accept.dnn_present == false) {
    logger.warning("Expected DNN in PDU session establishment accept");
    return proc_outcome_t::error;
  }
  if (pdu_session_est_accept.pdu_address_present == false) {
    logger.warning("Expected PDU Address in PDU session establishment accept");
    return proc_outcome_t::error;
  }
  if (parent_nas->add_pdu_session(pdu_session_id,
                                  pdu_session_est_accept.selected_pdu_session_type.pdu_session_type_value,
                                  pdu_session_est_accept.pdu_address) != SRSRAN_SUCCESS) {
    logger.warning("Adding PDU session failed\n");
    return srsran::proc_outcome_t::error;
  }
  return srsran::proc_outcome_t::success;
}

srsran::proc_outcome_t nas_5g::pdu_session_establishment_procedure::react(
    const srsran::nas_5g::pdu_session_establishment_reject_t& session_est_reject)
{
  return srsran::proc_outcome_t::success;
}

srsran::proc_outcome_t nas_5g::pdu_session_establishment_procedure::step()
{
  return srsran::proc_outcome_t::success;
}

} // namespace srsue