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

#include "srsran/common/test_common.h"
#include "srsue/hdr/stack/rrc/rrc_cell.h"
#include "srsue/hdr/stack/rrc/rrc_rlf_report.h"

using namespace srsue;

int test_single()
{
  srsran::task_scheduler          task_sched;
  rrc_rlf_report                  rlf_report;
  meas_cell_list<meas_cell_eutra> list{&task_sched};

  phy_meas_t pmeas{};
  pmeas.rsrp   = -20;
  pmeas.pci    = 1;
  pmeas.earfcn = 3400;
  pmeas.rsrq   = -10;

  list.add_meas_cell(pmeas);
  list.set_serving_cell(phy_cell_t{1, 3400}, false);

  rlf_report.set_failure(list, rrc_rlf_report::rlf);

  asn1::json_writer          jw;
  asn1::rrc::rlf_report_r9_s out = rlf_report.get_report();
  out.to_json(jw);
  printf("test_single: %s\n", jw.to_string().c_str());

  TESTASSERT(!out.meas_result_neigh_cells_r9_present);
  TESTASSERT(out.meas_result_neigh_cells_r9.meas_result_list_eutra_r9.size() == 0);
  TESTASSERT(out.failed_pcell_id_r10.is_present());
  TESTASSERT(out.failed_pcell_id_r10->pci_arfcn_r10().pci_r10 == 1);
  TESTASSERT(out.failed_pcell_id_r10->pci_arfcn_r10().carrier_freq_r10 = 3400);
  TESTASSERT(out.conn_fail_type_r10_present);
  TESTASSERT(out.conn_fail_type_r10.value == asn1::rrc::rlf_report_r9_s::conn_fail_type_r10_e_::rlf);

  return SRSRAN_SUCCESS;
}

int test_neighbours()
{
  srsran::task_scheduler          task_sched;
  rrc_rlf_report                  rlf_report;
  meas_cell_list<meas_cell_eutra> list{&task_sched};

  phy_meas_t pmeas{};
  pmeas.rsrp   = -20;
  pmeas.pci    = 1;
  pmeas.earfcn = 3400;
  pmeas.rsrq   = -10;

  list.add_meas_cell(pmeas);
  pmeas.pci = 4;
  list.add_meas_cell(pmeas);
  pmeas.pci = 6;
  list.add_meas_cell(pmeas);
  list.set_serving_cell(phy_cell_t{4, 3400}, false);

  TESTASSERT(!rlf_report.has_info());

  rlf_report.set_failure(list, rrc_rlf_report::hof);

  asn1::json_writer          jw;
  asn1::rrc::rlf_report_r9_s out = rlf_report.get_report();
  out.to_json(jw);
  printf("test_neighbours: %s\n", jw.to_string().c_str());

  TESTASSERT(out.meas_result_neigh_cells_r9_present);
  TESTASSERT(out.meas_result_neigh_cells_r9.meas_result_list_eutra_r9_present);
  TESTASSERT(out.meas_result_neigh_cells_r9.meas_result_list_eutra_r9.size() == 2);
  TESTASSERT(out.meas_result_neigh_cells_r9.meas_result_list_eutra_r9[0].carrier_freq_r9 = 3400);
  TESTASSERT(out.meas_result_neigh_cells_r9.meas_result_list_eutra_r9[0].meas_result_list_r9[0].pci == 1);
  TESTASSERT(out.meas_result_neigh_cells_r9.meas_result_list_eutra_r9[1].carrier_freq_r9 = 3400);
  TESTASSERT(out.meas_result_neigh_cells_r9.meas_result_list_eutra_r9[1].meas_result_list_r9[0].pci == 6);
  TESTASSERT(out.failed_pcell_id_r10.is_present());
  TESTASSERT(out.failed_pcell_id_r10->pci_arfcn_r10().pci_r10 == 4);
  TESTASSERT(out.failed_pcell_id_r10->pci_arfcn_r10().carrier_freq_r10 = 3400);
  TESTASSERT(out.conn_fail_type_r10_present);
  TESTASSERT(out.conn_fail_type_r10.value == asn1::rrc::rlf_report_r9_s::conn_fail_type_r10_e_::hof);

  TESTASSERT(rlf_report.has_info());

  return SRSRAN_SUCCESS;
}

int test_reest()
{
  srsran::task_scheduler          task_sched;
  rrc_rlf_report                  rlf_report;
  meas_cell_list<meas_cell_eutra> list{&task_sched};

  phy_meas_t pmeas{};
  pmeas.rsrp   = -20;
  pmeas.pci    = 1;
  pmeas.earfcn = 3400;
  pmeas.rsrq   = -10;

  list.add_meas_cell(pmeas);
  pmeas.pci = 4;
  list.add_meas_cell(pmeas);
  pmeas.pci = 6;
  list.add_meas_cell(pmeas);
  list.set_serving_cell(phy_cell_t{4, 3400}, false);

  TESTASSERT(!rlf_report.has_info());

  rlf_report.set_failure(list, rrc_rlf_report::hof);
  rlf_report.set_reest_gci(list.serving_cell().get_cell_id_bit(), list.serving_cell().get_plmn_asn1(0));

  asn1::json_writer          jw;
  asn1::rrc::rlf_report_r9_s out = rlf_report.get_report();
  out.to_json(jw);
  printf("test_reest: %s\n", jw.to_string().c_str());

  TESTASSERT(out.meas_result_neigh_cells_r9_present);
  TESTASSERT(out.meas_result_neigh_cells_r9.meas_result_list_eutra_r9_present);
  TESTASSERT(out.failed_pcell_id_r10.is_present());
  TESTASSERT(out.conn_fail_type_r10_present);
  TESTASSERT(out.conn_fail_type_r10.value == asn1::rrc::rlf_report_r9_s::conn_fail_type_r10_e_::hof);
  TESTASSERT(out.reest_cell_id_r10.is_present());

  TESTASSERT(rlf_report.has_info());

  return SRSRAN_SUCCESS;
}

int test_ho()
{
  srsran::task_scheduler          task_sched;
  rrc_rlf_report                  rlf_report;
  meas_cell_list<meas_cell_eutra> list{&task_sched};

  rlf_report.init(&task_sched);

  phy_meas_t pmeas{};
  pmeas.rsrp   = -20;
  pmeas.pci    = 1;
  pmeas.earfcn = 3400;
  pmeas.rsrq   = -10;

  list.add_meas_cell(pmeas);
  pmeas.pci = 4;
  list.add_meas_cell(pmeas);
  pmeas.pci = 6;
  list.add_meas_cell(pmeas);
  list.set_serving_cell(phy_cell_t{4, 3400}, false);

  TESTASSERT(!rlf_report.has_info());

  rlf_report.received_ho_command(list.serving_cell().get_cell_id_bit());
  for (int i = 0; i < 1000; i++) {
    task_sched.tic();
    task_sched.run_pending_tasks();
  }
  rlf_report.set_failure(list, rrc_rlf_report::hof);
  rlf_report.set_reest_gci(list.serving_cell().get_cell_id_bit(), list.serving_cell().get_plmn_asn1(0));

  asn1::json_writer          jw;
  asn1::rrc::rlf_report_r9_s out = rlf_report.get_report();
  out.to_json(jw);
  printf("test_ho: %s\n", jw.to_string().c_str());

  TESTASSERT(out.meas_result_neigh_cells_r9_present);
  TESTASSERT(out.meas_result_neigh_cells_r9.meas_result_list_eutra_r9_present);
  TESTASSERT(out.failed_pcell_id_r10.is_present());
  TESTASSERT(out.conn_fail_type_r10_present);
  TESTASSERT(out.conn_fail_type_r10.value == asn1::rrc::rlf_report_r9_s::conn_fail_type_r10_e_::hof);
  TESTASSERT(out.reest_cell_id_r10.is_present());

  TESTASSERT(rlf_report.has_info());

  return SRSRAN_SUCCESS;
}

int main()
{
  TESTASSERT(test_single() == SRSRAN_SUCCESS);
  TESTASSERT(test_neighbours() == SRSRAN_SUCCESS);
  TESTASSERT(test_reest() == SRSRAN_SUCCESS);
  TESTASSERT(test_ho() == SRSRAN_SUCCESS);
  printf("Success\n");

  return SRSRAN_SUCCESS;
}
