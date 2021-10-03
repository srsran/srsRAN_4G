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

#ifndef SRSUE_NAS_5G_PROCEDURES_H_
#define SRSUE_NAS_5G_PROCEDURES_H_

#include "srsue/hdr/stack/upper/nas_5g.h"

namespace srsue {

/**
 * @brief 5G NAS registration procedure
 *
 * Specified in 24 501 V16.7.0
 * 5GMM specific procedures
 * 5.5.1 Registration procedure
 */
class nas_5g::registration_procedure
{
public:
  explicit registration_procedure(nas_5g_interface_procedures* parent_nas_);
  srsran::proc_outcome_t init();
  srsran::proc_outcome_t step();
  srsran::proc_outcome_t then();
  static const char*     name() { return "Registration Procedure"; }

private:
  nas_5g_interface_procedures* parent_nas;
};

/**
 * @brief 5G NAS (5GSM) UE-requested PDU session establishment procedure
 *
 * Specified in 24 501 V16.7.0
 * UE-requested 5GSM procedures
 * 6.4.1 UE-requested PDU session establishment procedure
 */
class nas_5g::pdu_session_establishment_procedure
{
public:
  explicit pdu_session_establishment_procedure(nas_5g_interface_procedures* parent_nas_, srslog::basic_logger& logger_);
  srsran::proc_outcome_t init(const uint16_t pdu_session_id, const pdu_session_cfg_t pdu_session);
  srsran::proc_outcome_t react(const srsran::nas_5g::pdu_session_establishment_accept_t& pdu_session_est_accept);
  srsran::proc_outcome_t react(const srsran::nas_5g::pdu_session_establishment_reject_t& pdu_session_est_reject);
  srsran::proc_outcome_t step();
  srsran::proc_outcome_t then();
  static const char*     name() { return "PDU Session Establishment Procedure"; }

private:
  srslog::basic_logger&        logger;
  nas_5g_interface_procedures* parent_nas;
  uint32_t                     transaction_identity = 0;
  uint16_t                     pdu_session_id       = 0;
};

} // namespace srsue

#endif // SRSUE_NAS_5G_PROCEDURES_H_