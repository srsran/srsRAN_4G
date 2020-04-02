/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srslte/asn1/rrc_asn1.h"
#include "srslte/common/buffer_pool.h"
#include "srslte/common/log_filter.h"
#include "srslte/common/test_common.h"
#include "srslte/test/ue_test_interfaces.h"
#include "srslte/upper/pdcp.h"
#include "srsue/hdr/stack/rrc/rrc.h"
#include "srsue/hdr/stack/rrc/rrc_meas.h"
#include "srsue/hdr/stack/upper/nas.h"
#include <iostream>

using namespace asn1::rrc;
using namespace srsue;

class phy_test final : public phy_interface_rrc_lte
{
public:
  void set_serving_cell(uint32_t pci, uint32_t earfcn)
  {
    serving_pci    = pci;
    serving_earfcn = earfcn;
  }

  // Not implemented methods
  void set_config(srslte::phy_cfg_t& config,
                  uint32_t           cc_idx    = 0,
                  uint32_t           earfcn    = 0,
                  srslte_cell_t*     cell_info = nullptr) override
  {
  }
  void              set_config_tdd(srslte_tdd_config_t& tdd_config) override {}
  void              set_config_mbsfn_sib2(srslte::mbsfn_sf_cfg_t* cfg_list, uint32_t nof_cfgs) override {}
  void              set_config_mbsfn_sib13(const srslte::sib13_t& sib13) override {}
  void              set_config_mbsfn_mcch(const srslte::mcch_msg_t& mcch) override {}
  cell_search_ret_t cell_search(phy_cell_t* cell) override { return {}; }
  bool              cell_is_camping() override { return false; }
  bool              cell_select(const phy_cell_t* cell = nullptr) override { return false; }
  void              reset() override {}
  void              enable_pregen_signals(bool enable) override {}

  void set_cells_to_meas(uint32_t earfcn, const std::set<uint32_t>& pci) override
  {
    freqs_started.insert(earfcn);
    cells_started[earfcn] = pci;
  }
  void meas_stop() override
  {
    freqs_started.clear();
    cells_started.clear();
  }

  void reset_test()
  {
    meas_reset_called = false;
    meas_stop();
  }

  uint32_t meas_nof_freqs() { return freqs_started.size(); }

  uint32_t meas_nof_cells(uint32_t earfcn)
  {
    if (cells_started.count(earfcn)) {
      return cells_started[earfcn].size();
    } else {
      return 0;
    }
  }

  bool meas_freq_started(uint32_t earfcn) { return freqs_started.count(earfcn) > 0; }
  bool meas_cell_started(uint32_t earfcn, uint32_t pci)
  {
    if (cells_started.count(earfcn)) {
      return cells_started[earfcn].count(pci) > 0;
    } else {
      return false;
    }
  }

private:
  bool                                    meas_reset_called = false;
  std::set<uint32_t>                      freqs_started;
  std::map<uint32_t, std::set<uint32_t> > cells_started;
  uint32_t                                serving_pci    = 0;
  uint32_t                                serving_earfcn = 0;
};

class nas_test : public srsue::nas
{
public:
  nas_test(srslte::task_handler_interface* t) : srsue::nas(t) {}
  bool is_attached() override { return false; }
};

class pdcp_test : public srslte::pdcp
{
public:
  pdcp_test(const char* logname, srslte::task_handler_interface* t) : srslte::pdcp(t, logname) {}
  void write_sdu(uint32_t lcid, srslte::unique_byte_buffer_t sdu, bool blocking = false) override
  {
    ul_dcch_msg_s  ul_dcch_msg;
    asn1::cbit_ref bref(sdu->msg, sdu->N_bytes);
    if (ul_dcch_msg.unpack(bref) != asn1::SRSASN_SUCCESS or
        ul_dcch_msg.msg.type().value != ul_dcch_msg_type_c::types_opts::c1) {
      return;
    }
    switch (ul_dcch_msg.msg.c1().type()) {
      case ul_dcch_msg_type_c::c1_c_::types::rrc_conn_recfg_complete:
        if (!error) {
          error = !expecting_reconf_complete;
        }
        received_reconf_complete = true;
        break;
      case ul_dcch_msg_type_c::c1_c_::types::meas_report:
        if (!error) {
          error = expecting_reconf_complete;
        }
        if (!expecting_reconf_complete) {
          meas_res          = ul_dcch_msg.msg.c1().meas_report().crit_exts.c1().meas_report_r8().meas_results;
          meas_res_received = true;
        }
        break;
      default:
        error = true;
        break;
    }
  }
  bool get_meas_res(meas_results_s& meas_res_)
  {
    if (meas_res_received) {
      meas_res_         = meas_res;
      meas_res_received = false;
      return true;
    }
    return false;
  }

  bool get_error() { return error; }
  bool expecting_reconf_complete = false;
  bool received_reconf_complete  = false;

private:
  bool           error             = false;
  meas_results_s meas_res          = {};
  bool           meas_res_received = false;
};

class rrc_test : public rrc
{
  srsue::stack_test_dummy* stack = nullptr;

public:
  rrc_test(srslte::log_ref log_, stack_test_dummy* stack_) : rrc(stack_), stack(stack_)
  {
    pool     = srslte::byte_buffer_pool::get_instance();
    nastest  = std::unique_ptr<nas_test>(new nas_test(stack));
    pdcptest = std::unique_ptr<pdcp_test>(new pdcp_test(log_->get_service_name().c_str(), stack));
  };
  void init() { rrc::init(&phytest, nullptr, nullptr, pdcptest.get(), nastest.get(), nullptr, nullptr, {}); }

  void run_tti(uint32_t tti_)
  {
    stack->timers.step_all();
    rrc::run_tti();
  }

  // Set RRC in state RRC_CONNECTED
  void connect()
  {
    dl_ccch_msg_s dl_ccch_msg = {};
    dl_ccch_msg.msg.set_c1();
    dl_ccch_msg.msg.c1().set_rrc_conn_setup();
    dl_ccch_msg.msg.c1().rrc_conn_setup().crit_exts.set_c1().set_rrc_conn_setup_r8();
    send_ccch_msg(dl_ccch_msg);
    run_tti(tti++);
  }

  bool send_meas_cfg(rrc_conn_recfg_r8_ies_s& rrc_conn_recfg)
  {
    phytest.reset_test();
    pdcptest->received_reconf_complete = false;

    dl_dcch_msg_s dl_dcch_msg = {};
    dl_dcch_msg.msg.set_c1();
    dl_dcch_msg.msg.c1().set_rrc_conn_recfg();
    dl_dcch_msg.msg.c1().rrc_conn_recfg().crit_exts.set_c1().set_rrc_conn_recfg_r8();
    dl_dcch_msg.msg.c1().rrc_conn_recfg().crit_exts.c1().rrc_conn_recfg_r8() = rrc_conn_recfg;
    pdcptest->expecting_reconf_complete                                      = true;
    send_dcch_msg(dl_dcch_msg);
    pdcptest->expecting_reconf_complete = false;

    return !pdcptest->get_error() && pdcptest->received_reconf_complete;
  }

  void send_ccch_msg(dl_ccch_msg_s& dl_ccch_msg)
  {
    srslte::unique_byte_buffer_t pdu = srslte::allocate_unique_buffer(*pool, true);

    asn1::bit_ref bref(pdu->msg, pdu->get_tailroom());
    dl_ccch_msg.pack(bref);
    bref.align_bytes_zero();
    pdu->N_bytes = (uint32_t)bref.distance_bytes(pdu->msg);
    pdu->set_timestamp();
    write_pdu(0, std::move(pdu));
  }

  void send_dcch_msg(dl_dcch_msg_s& dl_dcch_msg)
  {
    srslte::unique_byte_buffer_t pdu = srslte::allocate_unique_buffer(*pool, true);
    ;
    asn1::bit_ref bref(pdu->msg, pdu->get_tailroom());
    dl_dcch_msg.pack(bref);
    bref.align_bytes_zero();
    pdu->N_bytes = (uint32_t)bref.distance_bytes(pdu->msg);
    pdu->set_timestamp();
    write_pdu(1, std::move(pdu));
  }

  void set_serving_cell(uint32_t pci, uint32_t earfcn)
  {
    std::vector<rrc_interface_phy_lte::phy_meas_t> phy_meas = {};
    rrc_interface_phy_lte::phy_meas_t              meas     = {};
    meas.pci                                                = pci;
    meas.earfcn                                             = earfcn;
    phy_meas.push_back(meas); // neighbour cell
    new_cell_meas(phy_meas);
    run_tti(1);
    phytest.set_serving_cell(pci, earfcn);
    rrc::set_serving_cell({pci, earfcn}, false);
  }

  bool has_neighbour_cell(const uint32_t earfcn, const uint32_t pci) { return rrc::has_neighbour_cell(earfcn, pci); }

  bool get_meas_res(meas_results_s& meas_res) { return pdcptest->get_meas_res(meas_res); }

  phy_test phytest;

private:
  std::unique_ptr<pdcp_test> pdcptest;
  std::unique_ptr<nas_test>  nastest;
  uint32_t                   tti  = 0;
  srslte::byte_buffer_pool*  pool = nullptr;
};

// Test Cell sear
int cell_select_test()
{
  srslte::log_ref log1("RRC_MEAS");
  log1->set_level(srslte::LOG_LEVEL_DEBUG);
  log1->set_hex_limit(-1);

  printf("==========================================================\n");
  printf("======            Cell Select Testing      ===============\n");
  printf("==========================================================\n");

  stack_test_dummy stack;
  rrc_test         rrctest(log1, &stack);
  rrctest.init();
  rrctest.connect();

  // Add a first serving cell
  rrctest.set_serving_cell(1, 1);

  // Add a second serving cell
  rrctest.set_serving_cell(2, 2);

  // Select the second serving cell
  rrctest.set_serving_cell(2, 2);

  TESTASSERT(!rrctest.has_neighbour_cell(2, 2));

  return SRSLTE_SUCCESS;
}

// Tests the measObject configuration and the successful activation of PHY cells to search for
int meas_obj_test()
{
  srslte::log_ref log1("RRC_MEAS");
  log1->set_level(srslte::LOG_LEVEL_DEBUG);
  log1->set_hex_limit(-1);

  printf("==========================================================\n");
  printf("======    Object Configuration Testing    ===============\n");
  printf("==========================================================\n");

  stack_test_dummy stack;
  rrc_test         rrctest(log1, &stack);
  rrctest.init();
  rrctest.connect();

  // Configure serving cell. First add neighbour, then set it as serving cell
  rrctest.set_serving_cell(1, 1);

  rrc_conn_recfg_r8_ies_s rrc_conn_recfg = {};
  rrc_conn_recfg.meas_cfg_present        = true;
  meas_cfg_s& meas_cfg                   = rrc_conn_recfg.meas_cfg;

  log1->info("Test1: Remove non-existing measObject, reportConfig and measId\n");
  meas_cfg = {};
  meas_cfg.meas_id_to_rem_list.push_back(3);
  meas_cfg.meas_obj_to_rem_list.push_back(3);
  meas_cfg.report_cfg_to_rem_list.push_back(3);
  meas_cfg.meas_id_to_rem_list_present  = true;
  meas_cfg.meas_obj_to_rem_list_present = true;

  // Just test it doesn't crash
  TESTASSERT(rrctest.send_meas_cfg(rrc_conn_recfg));
  TESTASSERT(rrctest.phytest.meas_nof_freqs() == 0);

  log1->info("Test2: Add measId pointing to non-existing measObject or reportConfig\n");
  meas_cfg               = {};
  meas_id_to_add_mod_s m = {};
  m.meas_obj_id          = 1;
  m.report_cfg_id        = 1;
  m.meas_id              = 1;
  meas_cfg.meas_id_to_add_mod_list.push_back(m);
  meas_cfg.meas_id_to_add_mod_list_present = true;

  // Just test it doesn't crash
  TESTASSERT(rrctest.send_meas_cfg(rrc_conn_recfg));
  TESTASSERT(rrctest.phytest.meas_nof_freqs() == 0);

  log1->info("Test3: Add meaObject and report of unsupported type. Setup a supported report for later use\n");
  meas_cfg                  = {};
  meas_obj_to_add_mod_s obj = {};
  obj.meas_obj.set_meas_obj_utra();
  meas_cfg.meas_obj_to_add_mod_list.push_back(obj);
  meas_cfg.meas_obj_to_add_mod_list_present = true;
  report_cfg_to_add_mod_s rep               = {};
  rep.report_cfg_id                         = 2;
  rep.report_cfg.set_report_cfg_inter_rat().trigger_type.set_periodical().purpose =
      report_cfg_inter_rat_s::trigger_type_c_::periodical_s_::purpose_opts::report_strongest_cells;
  rep.report_cfg.report_cfg_inter_rat().report_interv.value = report_interv_opts::ms640;
  rep.report_cfg.report_cfg_inter_rat().report_amount       = report_cfg_inter_rat_s::report_amount_opts::r1;
  meas_cfg.report_cfg_to_add_mod_list.push_back(rep);
  rep               = {};
  rep.report_cfg_id = 1;
  rep.report_cfg.set_report_cfg_eutra();
  rep.report_cfg.report_cfg_eutra().report_interv       = report_interv_opts::ms120;
  rep.report_cfg.report_cfg_eutra().report_amount       = report_cfg_eutra_s::report_amount_opts::r1;
  rep.report_cfg.report_cfg_eutra().report_quant.value  = report_cfg_eutra_s::report_quant_opts::both;
  rep.report_cfg.report_cfg_eutra().trigger_quant.value = report_cfg_eutra_s::trigger_quant_opts::rsrp;
  rep.report_cfg.report_cfg_eutra().trigger_type.set_event().event_id.set_event_a1().a1_thres.set_thres_rsrp();
  rep.report_cfg.report_cfg_eutra().trigger_type.event().time_to_trigger.value = time_to_trigger_opts::ms640;
  meas_cfg.report_cfg_to_add_mod_list.push_back(rep);
  meas_cfg.report_cfg_to_add_mod_list_present = true;

  // Just test it doesn't crash
  TESTASSERT(rrctest.send_meas_cfg(rrc_conn_recfg));
  TESTASSERT(rrctest.phytest.meas_nof_freqs() == 0);

  log1->info("Test4: Add 2 measObjects and 2 measId both pointing to the same measObject \n");
  meas_cfg = {};
  for (int i = 0; i < 2; i++) {
    m               = {};
    m.meas_obj_id   = 1; // same object
    m.report_cfg_id = 1;
    m.meas_id       = 1 + i; // add 2 different measIds
    meas_cfg.meas_id_to_add_mod_list.push_back(m);
  }
  meas_cfg.meas_id_to_add_mod_list_present = true;
  for (int i = 0; i < 2; i++) {
    obj             = {};
    obj.meas_obj_id = 1 + i;
    obj.meas_obj.set_meas_obj_eutra();
    obj.meas_obj.meas_obj_eutra().carrier_freq          = 100 + i;
    obj.meas_obj.meas_obj_eutra().allowed_meas_bw.value = allowed_meas_bw_opts::mbw6;
    if (i == 1) { // 2nd object has cells, 1st one doesn't
      for (int j = 1; j <= 4; j++) {
        cells_to_add_mod_s cell           = {};
        cell.pci                          = 10 + j;
        cell.cell_idx                     = j;
        cell.cell_individual_offset.value = q_offset_range_opts::db0;
        obj.meas_obj.meas_obj_eutra().cells_to_add_mod_list.push_back(cell);
      }
      obj.meas_obj.meas_obj_eutra().cells_to_add_mod_list_present = true;
    }
    meas_cfg.meas_obj_to_add_mod_list.push_back(obj);
  }
  meas_cfg.meas_obj_to_add_mod_list_present = true;

  TESTASSERT(rrctest.send_meas_cfg(rrc_conn_recfg));
  // Test we configure 1 frequency with no cells
  TESTASSERT(rrctest.phytest.meas_nof_freqs() == 1);
  TESTASSERT(rrctest.phytest.meas_freq_started(100));
  TESTASSERT(rrctest.phytest.meas_nof_cells(100) == 0);

  log1->info("Test5: Add existing objects and measId. Now add measId for 2nd cell\n");
  meas_cfg        = {};
  m               = {};
  m.meas_obj_id   = 2; // same object
  m.report_cfg_id = 1;
  m.meas_id       = 3;
  meas_cfg.meas_id_to_add_mod_list.push_back(m);
  meas_cfg.meas_id_to_add_mod_list_present = true;
  for (int i = 0; i < 2; i++) {
    obj             = {};
    obj.meas_obj_id = 1 + i;
    obj.meas_obj.set_meas_obj_eutra();
    obj.meas_obj.meas_obj_eutra().carrier_freq          = 1 + i;
    obj.meas_obj.meas_obj_eutra().allowed_meas_bw.value = allowed_meas_bw_opts::mbw15;
    meas_cfg.meas_obj_to_add_mod_list.push_back(obj);
  }
  meas_cfg.meas_obj_to_add_mod_list_present = true;
  rrctest.phytest.reset_test();
  TESTASSERT(rrctest.send_meas_cfg(rrc_conn_recfg));

  // Test we configure 2 frequency. 2nd has 4 cells
  TESTASSERT(rrctest.phytest.meas_nof_freqs() == 2);
  TESTASSERT(rrctest.phytest.meas_freq_started(1));
  TESTASSERT(rrctest.phytest.meas_freq_started(2));
  TESTASSERT(rrctest.phytest.meas_nof_cells(1) == 0);
  TESTASSERT(rrctest.phytest.meas_nof_cells(2) == 4);
  for (int j = 1; j <= 4; j++) {
    TESTASSERT(rrctest.phytest.meas_cell_started(2, 10 + j));
  }

  // Reconfigure 2nd object only, we should see 8 cells now
  log1->info("Test6: Add 1 cell to 1st object. Mixed add/mod and removal command.\n");
  meas_cfg                                  = {};
  meas_cfg.meas_obj_to_add_mod_list_present = true;

  // 1st object add 1 cell, none existed
  obj             = {};
  obj.meas_obj_id = 1;
  obj.meas_obj.set(meas_obj_to_add_mod_s::meas_obj_c_::types_opts::meas_obj_eutra);
  obj.meas_obj.meas_obj_eutra().carrier_freq                  = 1;
  obj.meas_obj.meas_obj_eutra().cells_to_add_mod_list_present = true;
  obj.meas_obj.meas_obj_eutra().allowed_meas_bw.value         = allowed_meas_bw_opts::mbw6;
  cells_to_add_mod_s cell                                     = {};
  cell.cell_idx                                               = 1;
  cell.pci                                                    = 1;
  cell.cell_individual_offset.value                           = q_offset_range_opts::db0;
  obj.meas_obj.meas_obj_eutra().cells_to_add_mod_list.push_back(cell);
  meas_cfg.meas_obj_to_add_mod_list.push_back(obj);

  // 2nd object remove 3 cells (1 non-existing)
  obj             = {};
  obj.meas_obj_id = 2;
  obj.meas_obj.set(meas_obj_to_add_mod_s::meas_obj_c_::types_opts::meas_obj_eutra);
  obj.meas_obj.meas_obj_eutra().carrier_freq = 2;
  obj.meas_obj.meas_obj_eutra().cells_to_rem_list.push_back(2);
  obj.meas_obj.meas_obj_eutra().cells_to_rem_list.push_back(4);
  obj.meas_obj.meas_obj_eutra().cells_to_rem_list.push_back(6);
  obj.meas_obj.meas_obj_eutra().cells_to_rem_list_present = true;

  // 2nd object add 5 cells, 1 existing, 1 just removed, 3 new
  uint32_t new_idx[5] = {2, 3, 5, 6};
  for (int j = 0; j < 4; j++) {
    cell                              = {};
    cell.pci                          = 20 + j + 1;
    cell.cell_idx                     = new_idx[j];
    cell.cell_individual_offset.value = q_offset_range_opts::db0;
    obj.meas_obj.meas_obj_eutra().cells_to_add_mod_list.push_back(cell);
  }
  obj.meas_obj.meas_obj_eutra().allowed_meas_bw.value         = allowed_meas_bw_opts::mbw6;
  obj.meas_obj.meas_obj_eutra().cells_to_add_mod_list_present = true;
  meas_cfg.meas_obj_to_add_mod_list.push_back(obj);

  rrctest.phytest.reset_test();
  TESTASSERT(rrctest.send_meas_cfg(rrc_conn_recfg));
  TESTASSERT(rrctest.phytest.meas_nof_cells(1) == 1);
  TESTASSERT(rrctest.phytest.meas_cell_started(1, 1));
  TESTASSERT(rrctest.phytest.meas_nof_cells(2) == 5);
  TESTASSERT(rrctest.phytest.meas_cell_started(2, 11));  // wasn't changed
  TESTASSERT(!rrctest.phytest.meas_cell_started(2, 12)); // was removed
  TESTASSERT(!rrctest.phytest.meas_cell_started(2, 14)); // was removed
  TESTASSERT(rrctest.phytest.meas_cell_started(2, 21));  // was added
  TESTASSERT(rrctest.phytest.meas_cell_started(2, 22));  // was updated
  TESTASSERT(rrctest.phytest.meas_cell_started(2, 23));  // was added
  TESTASSERT(rrctest.phytest.meas_cell_started(2, 24));  // was added

  log1->info("Test7: PHY finds new neighbours in frequency 1 and 2, check RRC instructs to search them\n");
  std::vector<rrc_interface_phy_lte::phy_meas_t> phy_meas = {};
  phy_meas.push_back({0, 0, 0.0f, 1, 31});
  phy_meas.push_back({-1, 0, 0.0f, 1, 32});
  phy_meas.push_back({-2, 0, 0.0f, 1, 33});
  phy_meas.push_back({-3, 0, 0.0f, 1, 34});
  rrctest.new_cell_meas(phy_meas);
  rrctest.run_tti(1);
  phy_meas = {};
  phy_meas.push_back({-4, 0, 0.0f, 1, 35});
  phy_meas.push_back({-5, 0, 0.0f, 1, 36});
  phy_meas.push_back({-6, 0, 0.0f, 1, 37});
  phy_meas.push_back({1, 0, 0.0f, 1, 30});
  phy_meas.push_back({0, 0, 0.0f, 2, 31});
  rrctest.new_cell_meas(phy_meas);
  rrctest.run_tti(1);

  TESTASSERT(rrctest.phytest.meas_nof_cells(1) == 8);
  TESTASSERT(rrctest.phytest.meas_cell_started(1, 1));
  TESTASSERT(rrctest.phytest.meas_cell_started(1, 30));
  TESTASSERT(rrctest.phytest.meas_cell_started(1, 31));
  TESTASSERT(rrctest.phytest.meas_cell_started(1, 32));
  TESTASSERT(rrctest.phytest.meas_cell_started(1, 33));
  TESTASSERT(rrctest.phytest.meas_cell_started(1, 34));
  TESTASSERT(rrctest.phytest.meas_cell_started(1, 35));
  TESTASSERT(rrctest.phytest.meas_cell_started(1, 36));
  TESTASSERT(rrctest.phytest.meas_nof_cells(2) == 6);
  TESTASSERT(rrctest.phytest.meas_cell_started(2, 11));  // wasn't changed
  TESTASSERT(!rrctest.phytest.meas_cell_started(2, 12)); // was removed
  TESTASSERT(!rrctest.phytest.meas_cell_started(2, 14)); // was removed
  TESTASSERT(rrctest.phytest.meas_cell_started(2, 21));  // was added
  TESTASSERT(rrctest.phytest.meas_cell_started(2, 22));  // was updated
  TESTASSERT(rrctest.phytest.meas_cell_started(2, 23));  // was added
  TESTASSERT(rrctest.phytest.meas_cell_started(2, 24));  // was added
  TESTASSERT(rrctest.phytest.meas_cell_started(2, 31));

  log1->info("Test8: Simulate a Release (reset() call) make sure resets correctly\n");
  rrctest.init();
  rrctest.run_tti(1);
  rrctest.connect();
  rrctest.run_tti(1);

  log1->info("Test9: Config removal\n");
  meas_cfg = {};
  meas_cfg.meas_obj_to_rem_list.push_back(1);
  meas_cfg.meas_obj_to_rem_list.push_back(2);
  meas_cfg.meas_obj_to_rem_list_present = true;
  meas_cfg.report_cfg_to_rem_list.push_back(1);
  meas_cfg.report_cfg_to_rem_list.push_back(2);
  meas_cfg.report_cfg_to_rem_list_present = true;
  meas_cfg.meas_id_to_rem_list.push_back(1);
  meas_cfg.meas_id_to_rem_list.push_back(2);
  meas_cfg.meas_id_to_rem_list_present = true;
  printf("==========================================================\n");
  return 0;
}

void config_default_report_test(rrc_conn_recfg_r8_ies_s&             rrc_conn_recfg,
                                eutra_event_s::event_id_c_           event_id,
                                time_to_trigger_e                    time_trigger,
                                uint32_t                             hyst,
                                report_cfg_eutra_s::report_amount_e_ report_amount,
                                report_interv_e                      report_interv)
{
  rrc_conn_recfg.meas_cfg_present = true;
  meas_cfg_s& meas_cfg            = rrc_conn_recfg.meas_cfg;

  meas_cfg               = {};
  meas_id_to_add_mod_s m = {};
  m.meas_obj_id          = 4;
  m.report_cfg_id        = 1;
  m.meas_id              = 1;
  meas_cfg.meas_id_to_add_mod_list.push_back(m);
  if (event_id.type() == eutra_event_s::event_id_c_::types::event_a3) {
    m               = {};
    m.meas_obj_id   = 6;
    m.report_cfg_id = 1;
    m.meas_id       = 2;
    meas_cfg.meas_id_to_add_mod_list.push_back(m);
  }
  meas_cfg.meas_id_to_add_mod_list_present = true;

  meas_obj_to_add_mod_s obj = {};
  obj.meas_obj.set_meas_obj_eutra();
  obj.meas_obj.meas_obj_eutra().carrier_freq          = 1;
  obj.meas_obj.meas_obj_eutra().allowed_meas_bw.value = allowed_meas_bw_opts::mbw6;
  obj.meas_obj_id                                     = 4;
  meas_cfg.meas_obj_to_add_mod_list.push_back(obj);
  obj = {};
  obj.meas_obj.set_meas_obj_eutra();
  obj.meas_obj.meas_obj_eutra().carrier_freq          = 2;
  obj.meas_obj.meas_obj_eutra().allowed_meas_bw.value = allowed_meas_bw_opts::mbw6;
  obj.meas_obj_id                                     = 6;
  meas_cfg.meas_obj_to_add_mod_list.push_back(obj);
  meas_cfg.meas_obj_to_add_mod_list_present = true;

  // Disable avearging
  meas_cfg.quant_cfg_present                                = true;
  meas_cfg.quant_cfg.quant_cfg_eutra_present                = true;
  meas_cfg.quant_cfg.quant_cfg_eutra.filt_coef_rsrp_present = true;
  meas_cfg.quant_cfg.quant_cfg_eutra.filt_coef_rsrp.value   = filt_coef_opts::fc0;

  // Report event
  report_cfg_to_add_mod_s rep = {};
  rep.report_cfg_id           = 1;
  rep.report_cfg.set_report_cfg_eutra();
  rep.report_cfg.report_cfg_eutra().trigger_type.set_event();
  rep.report_cfg.report_cfg_eutra().trigger_type.event().event_id        = event_id;
  rep.report_cfg.report_cfg_eutra().trigger_type.event().time_to_trigger = time_trigger;
  rep.report_cfg.report_cfg_eutra().trigger_type.event().hysteresis      = hyst;
  rep.report_cfg.report_cfg_eutra().trigger_quant.value                  = report_cfg_eutra_s::trigger_quant_opts::rsrp;
  rep.report_cfg.report_cfg_eutra().report_quant.value  = report_cfg_eutra_s::report_quant_opts::same_as_trigger_quant;
  rep.report_cfg.report_cfg_eutra().max_report_cells    = 8;
  rep.report_cfg.report_cfg_eutra().report_interv.value = report_interv;
  rep.report_cfg.report_cfg_eutra().report_amount.value = report_amount;
  meas_cfg.report_cfg_to_add_mod_list.push_back(rep);
  meas_cfg.report_cfg_to_add_mod_list_present = true;
}

void send_report(rrc_test&                   rrctest,
                 const std::vector<float>    rsrp,
                 const std::vector<uint32_t> earfcn,
                 const std::vector<uint32_t> pci)
{
  std::vector<rrc_interface_phy_lte::phy_meas_t> phy_meas = {};
  for (uint32_t i = 0; i < pci.size(); i++) {
    float r = rsrp[0];
    if (rsrp.size() == pci.size()) {
      r = rsrp[i];
    }
    uint32_t e = earfcn[0];
    if (earfcn.size() == pci.size()) {
      e = earfcn[i];
    }
    phy_meas.push_back({r, -5, 0.0f, e, pci[i]});
  }
  rrctest.new_cell_meas(phy_meas);
  rrctest.run_tti(1);
}

void middle_condition(rrc_test&                        rrctest,
                      const eutra_event_s::event_id_c_ event_id,
                      const uint32_t                   hyst,
                      const uint32_t                   earfcn,
                      const std::vector<uint32_t>      pci)
{
  if (event_id.type() == eutra_event_s::event_id_c_::types_opts::event_a1) {
    float rsrp_th = -140 + event_id.event_a1().a1_thres.thres_rsrp() + 0.5 * hyst;
    send_report(rrctest, {rsrp_th - (float)1e-2}, {earfcn}, pci);
  } else {
    float              offset = 0.5 * event_id.event_a3().a3_offset;
    std::vector<float> rsrp   = {};
    rsrp.reserve(pci.size());
    for (uint32_t i = 0; i < pci.size(); i++) {
      if (i == 0) {
        rsrp.push_back(-60);
      } else {
        rsrp.push_back(-60 + offset + 0.5 * hyst - (float)1e-2);
      }
    }
    send_report(rrctest, rsrp, {0, earfcn}, pci);
  }
}

void enter_condition(rrc_test&                        rrctest,
                     const eutra_event_s::event_id_c_ event_id,
                     const uint32_t                   hyst,
                     const uint32_t                   earfcn,
                     const std::vector<uint32_t>      pci)
{
  if (event_id.type() == eutra_event_s::event_id_c_::types_opts::event_a1) {
    float rsrp_th = -140 + event_id.event_a1().a1_thres.thres_rsrp() + 0.5 * hyst;
    send_report(rrctest, {rsrp_th + (float)1e-2}, {earfcn}, pci);
  } else {
    float              offset = 0.5 * event_id.event_a3().a3_offset;
    std::vector<float> rsrp   = {};
    rsrp.reserve(pci.size());
    for (uint32_t i = 0; i < pci.size(); i++) {
      if (i == 0) {
        rsrp.push_back(-60);
      } else {
        rsrp.push_back(-60 + offset + 0.01 * pci[i] + 0.5 * hyst + (float)1e-2);
      }
    }
    send_report(rrctest, rsrp, {0, earfcn}, pci);
  }
}

void no_condition(rrc_test& rrctest, const std::vector<uint32_t>& earfcn, const std::vector<uint32_t>& pci)
{
  std::vector<float> rsrp = {};
  rsrp.reserve(pci.size());
  for (uint32_t i = 0; i < pci.size(); i++) {
    rsrp.push_back(-60.0f);
  }
  send_report(rrctest, rsrp, earfcn, pci);
}

void exit_condition(rrc_test&                        rrctest,
                    const eutra_event_s::event_id_c_ event_id,
                    const uint32_t                   hyst,
                    const uint32_t                   earfcn,
                    const std::vector<uint32_t>      pci)
{
  if (event_id.type() == eutra_event_s::event_id_c_::types_opts::event_a1) {
    float rsrp_th_leave = -140 + event_id.event_a1().a1_thres.thres_rsrp() - 0.5 * hyst;
    send_report(rrctest, {rsrp_th_leave - (float)1e-2}, {earfcn}, pci);
  } else {
    float              offset = 0.5 * event_id.event_a3().a3_offset;
    std::vector<float> rsrp   = {};
    rsrp.reserve(pci.size());
    for (uint32_t i = 0; i < pci.size(); i++) {
      if (i == 0) {
        rsrp.push_back(-60);
      } else {
        rsrp.push_back(-60 + offset - 0.5 * hyst - (float)1e-2);
      }
    }
    send_report(rrctest, rsrp, {0, earfcn}, pci);
  }
}

// Test A1-event reporting and management of report amount and interval
int a1event_report_test(uint32_t                             a1_rsrp_th,
                        time_to_trigger_e                    time_trigger,
                        uint32_t                             hyst,
                        report_cfg_eutra_s::report_amount_e_ report_amount,
                        report_interv_e                      report_interv)
{

  srslte::log_ref log1("RRC_MEAS");
  log1->set_level(srslte::LOG_LEVEL_DEBUG);
  log1->set_hex_limit(-1);

  printf("==========================================================\n");
  printf("============       Report Testing  A1      ===============\n");
  printf("==========================================================\n");

  stack_test_dummy stack;
  rrc_test         rrctest(log1, &stack);
  rrctest.init();
  rrctest.connect();

  // Configure serving cell. First add neighbour, then set it as serving cell
  rrctest.set_serving_cell(1, 1);

  // default report configuration
  rrc_conn_recfg_r8_ies_s    rrc_conn_recfg = {};
  eutra_event_s::event_id_c_ event_id       = {};

  event_id.set_event_a1();
  event_id.event_a1().a1_thres.set_thres_rsrp();
  event_id.event_a1().a1_thres.thres_rsrp() = a1_rsrp_th;

  config_default_report_test(rrc_conn_recfg, event_id, time_trigger, hyst, report_amount, report_interv);

  TESTASSERT(rrctest.send_meas_cfg(rrc_conn_recfg));

  meas_results_s meas_res = {};

  int ttt_iters = time_trigger.to_number() + 1;

  // Entering condition during half timeToTrigger, should not trigger measurement
  for (int i = 0; i < ttt_iters / 2; i++) {
    log1->info("Report %d/%d enter condition is true\n", i, ttt_iters / 2);
    enter_condition(rrctest, event_id, hyst, 0, {1, 2});
    // Check doesn't generate measurement report
    TESTASSERT(!rrctest.get_meas_res(meas_res));
  }

  log1->info("Report leaving enter condition\n");
  // Not satisfy entering condition for 1 TTI
  middle_condition(rrctest, event_id, hyst, 0, {1});
  TESTASSERT(!rrctest.get_meas_res(meas_res));

  // Should go again all timeToTrigger, should not trigger measurement until end
  for (int i = 0; i < ttt_iters; i++) {
    log1->info("Report %d/%d enter condition is true\n", i, ttt_iters);
    enter_condition(rrctest, event_id, hyst, 0, {1, 2});
    if (i < ttt_iters - 1) {
      // Check doesn't generate measurement report
      TESTASSERT(!rrctest.get_meas_res(meas_res));
    }
  }

  // Check report is correct: RSRP=32, RSRQ=30 and measId=1
  TESTASSERT(rrctest.get_meas_res(meas_res));
  TESTASSERT(meas_res.meas_id == 1);
  TESTASSERT(!meas_res.meas_result_neigh_cells_present);
  TESTASSERT(meas_res.meas_result_pcell.rsrp_result == 32);
  TESTASSERT(meas_res.meas_result_pcell.rsrq_result == 30);

  // Test multiple reports are sent if report_amount > 1
  if (report_amount.to_number() > 1) {
    // Trigger again entering condition for the same cell it shouldn't trigger a new report, just keep sending the
    // periodic reports without restarting counter
    for (int i = 0; i < ttt_iters; i++) {
      log1->info("Report %d/%d enter condition is true\n", i, ttt_iters);
      enter_condition(rrctest, event_id, hyst, 0, {1});
    }
    // Do not expect report if timer not expired
    TESTASSERT(!rrctest.get_meas_res(meas_res));
    // Wait to generate all reports
    for (int i = 0; i < report_amount.to_number() - 1; i++) {
      log1->info("Testing report %d/%d\n", i, report_amount.to_number());
      int interval = report_interv.to_number();
      if (i == 0) {
        // already stepped these iterations
        interval -= ttt_iters;
      }
      for (int j = 0; j < interval; j++) {
        if (j == 0 && i > report_amount.to_number() - 3) {
          // Exit the enter condition in the last one, should still send the last report
          middle_condition(rrctest, event_id, hyst, 0, {1});
        } else {
          log1->info("Stepping timer %d/%d\n", j, interval);
          rrctest.run_tti(1);
        }
        if (j < interval - 1) {
          // Do not expect report if timer not expired
          TESTASSERT(!rrctest.get_meas_res(meas_res));
        } else {
          // expect 1 report every interval ms
          TESTASSERT(rrctest.get_meas_res(meas_res));
        }
      }
    }
    // Do not expect more reports
    for (int j = 0; j < report_interv.to_number(); j++) {
      rrctest.run_tti(1);
      TESTASSERT(!rrctest.get_meas_res(meas_res));
    }
    // Trigger again condition
    for (int i = 0; i < ttt_iters; i++) {
      log1->info("Report %d/%d enter condition is true\n", i, ttt_iters);
      enter_condition(rrctest, event_id, hyst, 0, {1});
    }
    // Do not expect report
    TESTASSERT(!rrctest.get_meas_res(meas_res));

    // Leaving condition for timeToTrigger
    for (int i = 0; i < ttt_iters; i++) {
      log1->info("Report %d/%d leaving condition is true\n", i, ttt_iters);
      exit_condition(rrctest, event_id, hyst, 0, {1});
      // Check doesn't generate measurement report
      TESTASSERT(!rrctest.get_meas_res(meas_res));
    }
    // Trigger again condition
    for (int i = 0; i < ttt_iters; i++) {
      log1->info("Report %d/%d enter condition is true\n", i, ttt_iters);
      enter_condition(rrctest, event_id, hyst, 0, {1});
    }
    // Expect report
    TESTASSERT(rrctest.get_meas_res(meas_res));
  }

  printf("==========================================================\n");
  return 0;
}

// Test A3-event reporting and management of report amount and interval
int a3event_report_test(uint32_t a3_offset, uint32_t hyst, bool report_on_leave)
{

  srslte::log_ref log1("RRC_MEAS");
  log1->set_level(srslte::LOG_LEVEL_DEBUG);
  log1->set_hex_limit(-1);

  printf("==========================================================\n");
  printf("============       Report Testing A3       ===============\n");
  printf("==========================================================\n");

  stack_test_dummy stack;
  rrc_test         rrctest(log1, &stack);
  rrctest.init();
  rrctest.connect();

  // Configure serving cell. First add neighbour, then set it as serving cell
  rrctest.set_serving_cell(1, 1);

  // default report configuration
  rrc_conn_recfg_r8_ies_s    rrc_conn_recfg = {};
  eutra_event_s::event_id_c_ event_id       = {};

  event_id.set_event_a3();
  event_id.event_a3().a3_offset       = a3_offset;
  event_id.event_a3().report_on_leave = report_on_leave;

  config_default_report_test(rrc_conn_recfg,
                             event_id,
                             time_to_trigger_opts::ms0,
                             hyst,
                             report_cfg_eutra_s::report_amount_opts::r1,
                             report_interv_opts::ms120);

  TESTASSERT(rrctest.send_meas_cfg(rrc_conn_recfg));

  meas_results_s meas_res = {};

  log1->info("Test no-enter condition and no trigger report \n");
  no_condition(rrctest, {0}, {1});
  TESTASSERT(!rrctest.get_meas_res(meas_res));

  no_condition(rrctest, {0, 1}, {1, 0});
  TESTASSERT(!rrctest.get_meas_res(meas_res));

  log1->info("Test enter condition triggers report. 1 neighbour cell in enter + 1 in exit \n");
  float              offset = 0.5 * event_id.event_a3().a3_offset;
  std::vector<float> rsrp   = {};
  rsrp.push_back(-60 + offset + 0.5 * hyst + (float)1e-2);
  rsrp.push_back(-60 + offset - 0.5 * hyst - (float)1e-2);
  send_report(rrctest, rsrp, {1, 1}, {0, 3});

  // Check report is correct: RSRP=34, RSRQ=0 and measId=1
  TESTASSERT(rrctest.get_meas_res(meas_res));
  TESTASSERT(meas_res.meas_id == 1);
  TESTASSERT(meas_res.meas_result_pcell.rsrp_result == 81);
  TESTASSERT(meas_res.meas_result_pcell.rsrq_result == 30);
  TESTASSERT(meas_res.meas_result_neigh_cells_present);
  TESTASSERT(meas_res.meas_result_neigh_cells.meas_result_list_eutra().size() == 1);
  TESTASSERT(meas_res.meas_result_neigh_cells.meas_result_list_eutra()[0].pci == 0);
  TESTASSERT(meas_res.meas_result_neigh_cells.meas_result_list_eutra()[0].meas_result.rsrp_result ==
             81 + (hyst + a3_offset) / 2);

  // Next iteration in entering state does not trigger another report
  log1->info("Test enter condition for the same cell does not trigger report\n");
  rrctest.run_tti(1);
  TESTASSERT(!rrctest.get_meas_res(meas_res));

  log1->info("Test enter condition for different earfcn triggers report\n");
  enter_condition(rrctest, event_id, hyst, 2, {1, 3});
  TESTASSERT(rrctest.get_meas_res(meas_res));
  TESTASSERT(meas_res.meas_id == 2);
  TESTASSERT(meas_res.meas_result_pcell.rsrp_result == 81);
  TESTASSERT(meas_res.meas_result_pcell.rsrq_result == 30);
  TESTASSERT(meas_res.meas_result_neigh_cells_present);
  TESTASSERT(meas_res.meas_result_neigh_cells.meas_result_list_eutra().size() == 1);
  TESTASSERT(meas_res.meas_result_neigh_cells.meas_result_list_eutra()[0].pci == 3);
  TESTASSERT(meas_res.meas_result_neigh_cells.meas_result_list_eutra()[0].meas_result.rsrp_result ==
             81 + (hyst + a3_offset) / 2);

  // if a new cell enters conditions then expect another report
  log1->info("Test a new cell enter condition triggers report\n");
  enter_condition(rrctest, event_id, hyst, 1, {1, 3});
  TESTASSERT(rrctest.get_meas_res(meas_res));
  TESTASSERT(meas_res.meas_id == 1);
  TESTASSERT(meas_res.meas_result_pcell.rsrp_result == 81);
  TESTASSERT(meas_res.meas_result_pcell.rsrq_result == 30);
  TESTASSERT(meas_res.meas_result_neigh_cells_present);
  TESTASSERT(meas_res.meas_result_neigh_cells.meas_result_list_eutra().size() == 2);
  TESTASSERT(meas_res.meas_result_neigh_cells.meas_result_list_eutra()[0].pci ==
             3); // should be ordered by rsrp, which is proportional to pci in enter_condition()
  TESTASSERT(meas_res.meas_result_neigh_cells.meas_result_list_eutra()[0].meas_result.rsrp_result ==
             81 + (hyst + a3_offset) / 2);

  // cell pci=0 exists condition
  log1->info("Test exit condition\n");
  exit_condition(rrctest, event_id, hyst, 1, {1, 0});
  if (report_on_leave) {
    TESTASSERT(rrctest.get_meas_res(meas_res));
  }

  // 2 enters again, now expect report again
  log1->info("Test trigger again the cell that exited\n");
  enter_condition(rrctest, event_id, hyst, 1, {1, 0});
  TESTASSERT(rrctest.get_meas_res(meas_res));
  TESTASSERT(meas_res.meas_id == 1);
  TESTASSERT(meas_res.meas_result_neigh_cells_present);
  TESTASSERT(meas_res.meas_result_neigh_cells.meas_result_list_eutra().size() == 2);
  TESTASSERT(meas_res.meas_result_neigh_cells.meas_result_list_eutra()[0].pci == 3);
  TESTASSERT(meas_res.meas_result_neigh_cells.meas_result_list_eutra()[0].meas_result.rsrp_result ==
             81 + (hyst + a3_offset) / 2);

  return SRSLTE_SUCCESS;
}

int main(int argc, char** argv)
{
  TESTASSERT(cell_select_test() == SRSLTE_SUCCESS);
  TESTASSERT(meas_obj_test() == SRSLTE_SUCCESS);
  TESTASSERT(
      a1event_report_test(
          30, time_to_trigger_opts::ms40, 3, report_cfg_eutra_s::report_amount_opts::r1, report_interv_opts::ms120) ==
      SRSLTE_SUCCESS);
  TESTASSERT(
      a1event_report_test(
          30, time_to_trigger_opts::ms0, 3, report_cfg_eutra_s::report_amount_opts::r1, report_interv_opts::ms120) ==
      SRSLTE_SUCCESS);
  TESTASSERT(
      a1event_report_test(
          30, time_to_trigger_opts::ms40, 3, report_cfg_eutra_s::report_amount_opts::r8, report_interv_opts::ms120) ==
      SRSLTE_SUCCESS);
  TESTASSERT(a3event_report_test(6, 3, true) == SRSLTE_SUCCESS);
  return SRSLTE_SUCCESS;
}
