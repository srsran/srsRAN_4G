/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#include "srsue/hdr/stack/rrc/rrc_rlf_report.h"
#include "srsran/rrc/rrc_common.h"

namespace srsue {

void rrc_rlf_report::init(srsran::task_sched_handle task_sched)
{
  timer_conn_failure = task_sched.get_unique_timer();
}

// Returns true if VarRLF-Report structure has info available
bool rrc_rlf_report::has_info()
{
  return has_event;
}

// Called upon T304 expiry (type == hof) or Detection of radio link failure (type == rlf)
void rrc_rlf_report::set_failure(meas_cell_list<meas_cell_eutra>& meas_cells, failure_type_t type)
{
  has_event = true;

  // clear the information included in VarRLF-Report, if any
  rlf_report = {};

  // set the plmn-Identity to the RPLMN

  // set the measResultLastServCell to include the RSRP and RSRQ, if available, of the PCell based on
  // measurements collected up to the moment the UE detected radio link failure
  rlf_report.meas_result_last_serv_cell_r9.rsrp_result_r9 =
      rrc_value_to_range(srsran::quant_rsrp, meas_cells.serving_cell().get_rsrp());
  rlf_report.meas_result_last_serv_cell_r9.rsrq_result_r9 =
      rrc_value_to_range(srsran::quant_rsrq, meas_cells.serving_cell().get_rsrq());
  rlf_report.meas_result_last_serv_cell_r9.rsrq_result_r9_present = true;

  // set the measResultNeighCells to include the best measured cells, other than the PCell, ordered such that
  // the best cell is listed first, and based on measurements collected up to the moment the UE detected radio
  // link failure
  if (meas_cells.nof_neighbours() > 0) {
    rlf_report.meas_result_neigh_cells_r9_present                           = true;
    rlf_report.meas_result_neigh_cells_r9.meas_result_list_eutra_r9_present = true;
    rlf_report.meas_result_neigh_cells_r9.meas_result_list_eutra_r9.clear();
    meas_cells.sort_neighbour_cells();
    // It is not clear how the sorting and grouping of cells per frequency must be done.
    // We use a separate MeasResultList2EUTRA-r9 struct for each pci/frequency pair
    for (const auto& f : meas_cells) {
      meas_result2_eutra_r9_s meas2        = {};
      meas2.carrier_freq_r9                = f->get_earfcn();
      meas_result_eutra_s meas             = {};
      meas.pci                             = f->get_pci();
      meas.meas_result.rsrp_result_present = true;
      meas.meas_result.rsrq_result_present = true;
      meas.meas_result.rsrp_result         = rrc_value_to_range(srsran::quant_rsrp, f->get_rsrp());
      meas.meas_result.rsrq_result         = rrc_value_to_range(srsran::quant_rsrq, f->get_rsrq());
      meas2.meas_result_list_r9.push_back(meas);
      rlf_report.meas_result_neigh_cells_r9.meas_result_list_eutra_r9.push_back(meas2);
    }
  }

  // set the failedPCellId to the global cell identity, if available, and otherwise to the physical cell identity and
  // carrier frequency of the PCell where radio link failure is detected;
  rlf_report.failed_pcell_id_r10.set_present(true);
  if (meas_cells.serving_cell().has_sib1()) {
    rlf_report.failed_pcell_id_r10->set_cell_global_id_r10().cell_id = meas_cells.serving_cell().get_cell_id_bit();
    rlf_report.failed_pcell_id_r10->cell_global_id_r10().plmn_id     = meas_cells.serving_cell().get_plmn_asn1(0);
  } else {
    rlf_report.failed_pcell_id_r10->set_pci_arfcn_r10();
    rlf_report.failed_pcell_id_r10->pci_arfcn_r10().pci_r10          = meas_cells.serving_cell().get_pci();
    rlf_report.failed_pcell_id_r10->pci_arfcn_r10().carrier_freq_r10 = meas_cells.serving_cell().get_earfcn();
  }

  // if an RRCConnectionReconfiguration message including the mobilityControlInfo was received before the
  // connection failure
  if (timer_conn_failure.is_running()) {
    timer_conn_failure.stop();

    // include previousPCellId and set it to the global cell identity of the PCell where the last
    // RRCConnectionReconfiguration including the mobilityControlInfo message was received;
    rlf_report.prev_pcell_id_r10.set_present(true);
    rlf_report.prev_pcell_id_r10->cell_id = ho_gci;
    rlf_report.prev_pcell_id_r10->plmn_id = meas_cells.serving_cell().get_plmn_asn1(0);

    // set the timeConnFailure to the elapsed time since reception of the last
    // RRCConnectionReconfiguration message including the mobilityControlInfo;
    rlf_report.time_conn_fail_r10_present = true;
    rlf_report.time_conn_fail_r10         = timer_conn_failure.time_elapsed() / 100; // 1 unit = 100 ms
  }

  // set the connectionFailureType
  rlf_report.conn_fail_type_r10_present = true;
  rlf_report.conn_fail_type_r10 =
      type == rlf ? rlf_report_r9_s::conn_fail_type_r10_opts::rlf : rlf_report_r9_s::conn_fail_type_r10_opts::hof;

  rlf_report.ext = true;
}

void rrc_rlf_report::set_reest_gci(const asn1::fixed_bitstring<28>& gci, const asn1::rrc::plmn_id_s& plmn_id)
{
  rlf_report.reest_cell_id_r10.set_present(true);
  rlf_report.reest_cell_id_r10->cell_id = gci;
  rlf_report.reest_cell_id_r10->plmn_id = plmn_id;
}

void rrc_rlf_report::received_ho_command(const asn1::fixed_bitstring<28>& current_gci)
{
  if (timer_conn_failure.is_valid()) {
    timer_conn_failure.stop();
    timer_conn_failure.run();
    ho_gci = current_gci;
  }
}

rlf_report_r9_s rrc_rlf_report::get_report()
{
  return rlf_report;
}

// Clears VarRLF-Report contents
void rrc_rlf_report::clear()
{
  has_event  = false;
  rlf_report = {};
}

} // namespace srsue