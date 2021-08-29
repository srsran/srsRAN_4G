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
#ifndef SRSRAN_RRC_RLF_REPORT_H_
#define SRSRAN_RRC_RLF_REPORT_H_

#include "rrc_cell.h"
#include "srsran/asn1/rrc.h"
#include "srsran/common/common.h"

namespace srsue {

using namespace asn1::rrc;

// RRC RLF-Report class
class rrc_rlf_report
{
public:
  enum failure_type_t { rlf, hof };

  void init(srsran::task_sched_handle task_sched);

  // Returns true if VarRLF-Report structure has info available
  bool has_info();

  // Called upon T304 expiry (type == hof) or Detection of radio link failure (type == rlf)
  void set_failure(meas_cell_list<meas_cell_eutra>& meas_cells, failure_type_t type);

  // Called upon transmission of ReestablishmentRequest message
  void set_reest_gci(const asn1::fixed_bitstring<28>& gci, const asn1::rrc::plmn_id_s& plmn_id);

  // Called upon initiation of RadioReconfiguration message including MobilityInfo IE
  void received_ho_command(const asn1::fixed_bitstring<28>& current_gci);

  // Returns a copy of the rlf_report_r9 ASN1 struct
  rlf_report_r9_s get_report();

  // Clears VarRLF-Report contents
  void clear();

private:
  asn1::fixed_bitstring<28> ho_gci;

  bool                                has_event          = false;
  rlf_report_r9_s                     rlf_report         = {};
  srsran::timer_handler::unique_timer timer_conn_failure = {};
};
} // namespace srsue

#endif // SRSRAN_RRC_RLF_REPORT_H_
