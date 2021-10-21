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

#include "srsran/asn1/rrc/meascfg.h"
#include "srsran/common/buffer_pool.h"
#include "srsran/common/test_common.h"
#include "srsran/test/ue_test_interfaces.h"
#include "srsran/upper/pdcp.h"
#include "srsue/hdr/stack/rrc/rrc.h"
#include "srsue/hdr/stack/rrc/rrc_meas.h"
#include "srsue/hdr/stack/rrc/rrc_nr.h"
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
  bool set_config(const srsran::phy_cfg_t& config, uint32_t cc_idx) override { return true; }
  bool set_scell(srsran_cell_t cell_info, uint32_t cc_idx, uint32_t earfcn) override { return true; }
  void set_config_tdd(srsran_tdd_config_t& tdd_config) override {}
  void set_config_mbsfn_sib2(srsran::mbsfn_sf_cfg_t* cfg_list, uint32_t nof_cfgs) override {}
  void set_config_mbsfn_sib13(const srsran::sib13_t& sib13) override {}
  void set_config_mbsfn_mcch(const srsran::mcch_msg_t& mcch) override {}
  bool cell_search() override { return true; }
  bool cell_is_camping() override { return true; }
  void deactivate_scells() override {}
  bool cell_select(phy_cell_t cell) override
  {
    last_selected_cell = cell;
    return true;
  }

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

  phy_cell_t last_selected_cell = {};

private:
  bool                                    meas_reset_called = false;
  std::set<uint32_t>                      freqs_started;
  std::map<uint32_t, std::set<uint32_t> > cells_started;
  uint32_t                                serving_pci    = 0;
  uint32_t                                serving_earfcn = 0;
};

class mac_test : public srsue::mac_interface_rrc
{
public:
  srsran::task_sched_handle task_sched;
  srsue::rrc*               rrc_ptr;

  mac_test(srsue::rrc* rrc_, srsran::task_sched_handle task_sched_) : rrc_ptr(rrc_), task_sched(task_sched_) {}

  int get_dlsch_with_sib1(bcch_dl_sch_msg_s& dlsch_msg)
  {
    sib_type1_s    sib1;
    uint8_t        asn1_msg[] = {0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    asn1::cbit_ref bref{asn1_msg, sizeof(asn1_msg)};
    return dlsch_msg.unpack(bref);
  }
  int get_dlsch_with_sys_info(bcch_dl_sch_msg_s& dlsch_msg)
  {
    sib_type1_s sib1;
    uint8_t     asn1_msg[] = {0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00};
    asn1::cbit_ref bref{asn1_msg, sizeof(asn1_msg)};
    return dlsch_msg.unpack(bref);
  }

  void bcch_start_rx(int si_window_start, int si_window_length) override
  {
    task_sched.defer_task([this]() {
      srsran::unique_byte_buffer_t pdu;
      for (uint32_t i = 0; i < 2; ++i) {
        bcch_dl_sch_msg_s dlsch_msg;
        if (i == 0) {
          get_dlsch_with_sib1(dlsch_msg);
        } else {
          get_dlsch_with_sys_info(dlsch_msg);
        }

        pdu = srsran::make_byte_buffer();
        if (pdu == nullptr) {
          return;
        }
        asn1::bit_ref bref(pdu->msg, pdu->get_tailroom());
        dlsch_msg.pack(bref);
        pdu->N_bytes = bref.distance_bytes();
        rrc_ptr->write_pdu_bcch_dlsch(std::move(pdu));
      }
    });
  }
  void bcch_stop_rx() override {}
  void pcch_start_rx() override {}

  void setup_lcid(uint32_t lcid, uint32_t lcg, uint32_t priority, int PBR_x_tti, uint32_t BSD) override {}

  void mch_start_rx(uint32_t lcid) override {}

  void set_config(srsran::mac_cfg_t& mac_cfg) override {}
  void set_config(srsran::sr_cfg_t& sr_cfg) override {}
  void set_rach_ded_cfg(uint32_t preamble_index, uint32_t prach_mask) override {}

  uint16_t get_crnti() override { return 0; }
  void     set_contention_id(uint64_t uecri) override {}
  void     set_ho_rnti(uint16_t crnti, uint16_t target_pci) override {}

  void reconfiguration(const uint32_t& cc_idx, const bool& enable) override {}
  void reset() override {}
};

class rrc_nr_test final : public srsue::rrc_nr_interface_rrc
{
public:
  ~rrc_nr_test() = default;
  int  get_eutra_nr_capabilities(srsran::byte_buffer_t* eutra_nr_caps) override { return SRSRAN_SUCCESS; };
  int  get_nr_capabilities(srsran::byte_buffer_t* nr_cap) override { return SRSRAN_SUCCESS; };
  void phy_set_cells_to_meas(uint32_t carrier_freq_r15) override{};
  void phy_meas_stop() override{};
  bool rrc_reconfiguration(bool                endc_release_and_add_r15,
                           bool                nr_secondary_cell_group_cfg_r15_present,
                           asn1::dyn_octstring nr_secondary_cell_group_cfg_r15,
                           bool                sk_counter_r15_present,
                           uint32_t            sk_counter_r15,
                           bool                nr_radio_bearer_cfg1_r15_present,
                           asn1::dyn_octstring nr_radio_bearer_cfg1_r15) override
  {
    return false;
  }
  void rrc_release() override {}
  bool is_config_pending() override { return false; };
};

class nas_test : public srsue::nas
{
public:
  nas_test(srsran::task_sched_handle t) : srsue::nas(srslog::fetch_basic_logger("NAS"), t) {}
  bool is_registered() override { return false; }
};

class pdcp_test : public srsran::pdcp
{
public:
  pdcp_test(const char* logname, srsran::task_sched_handle t) : srsran::pdcp(t, logname) {}
  void write_sdu(uint32_t lcid, srsran::unique_byte_buffer_t sdu, int sn = -1) override
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

class rrc_test : public srsue::rrc
{
  stack_test_dummy* stack = nullptr;

public:
  rrc_test(const std::string& log_name, stack_test_dummy* stack_) :
    rrc(stack_, &stack_->task_sched), stack(stack_), mactest(this, &stack_->task_sched)
  {
    nastest   = std::unique_ptr<nas_test>(new nas_test(&stack->task_sched));
    pdcptest  = std::unique_ptr<pdcp_test>(new pdcp_test(log_name.c_str(), &stack->task_sched));
    rrcnrtest = std::unique_ptr<rrc_nr_test>(new rrc_nr_test());
  }
  void init()
  {
    rrc::init(&phytest, &mactest, nullptr, pdcptest.get(), nastest.get(), nullptr, nullptr, rrcnrtest.get(), {});
  }

  void run_tti(uint32_t tti_)
  {
    stack->task_sched.tic();
    stack->task_sched.run_pending_tasks();
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
    stack->task_sched.run_pending_tasks();
    set_config_complete(true);
    pdcptest->expecting_reconf_complete = false;

    return !pdcptest->get_error() && pdcptest->received_reconf_complete;
  }

  void send_ccch_msg(dl_ccch_msg_s& dl_ccch_msg)
  {
    srsran::unique_byte_buffer_t pdu = srsran::make_byte_buffer();
    if (pdu == nullptr) {
      return;
    }

    asn1::bit_ref bref(pdu->msg, pdu->get_tailroom());
    dl_ccch_msg.pack(bref);
    bref.align_bytes_zero();
    pdu->N_bytes = (uint32_t)bref.distance_bytes(pdu->msg);
    pdu->set_timestamp();
    write_pdu(0, std::move(pdu));
  }

  void send_dcch_msg(dl_dcch_msg_s& dl_dcch_msg)
  {
    srsran::unique_byte_buffer_t pdu = srsran::make_byte_buffer();
    if (pdu == nullptr) {
      return;
    }
    asn1::bit_ref bref(pdu->msg, pdu->get_tailroom());
    dl_dcch_msg.pack(bref);
    bref.align_bytes_zero();
    pdu->N_bytes = (uint32_t)bref.distance_bytes(pdu->msg);
    pdu->set_timestamp();
    write_pdu(1, std::move(pdu));
  }

  void set_serving_cell(uint32_t pci, uint32_t earfcn)
  {
    if (not has_neighbour_cell(earfcn, pci)) {
      add_neighbour_cell(pci, earfcn);
    }
    phytest.set_serving_cell(pci, earfcn);
    rrc::set_serving_cell({pci, earfcn}, false);
  }

  void add_neighbour_cell(uint32_t pci, uint32_t earfcn, float rsrp = 0)
  {
    std::vector<phy_meas_t> phy_meas = {};
    phy_meas_t              meas     = {};
    meas.pci                         = pci;
    meas.earfcn                      = earfcn;
    meas.rsrp                        = rsrp;
    phy_meas.push_back(meas); // neighbour cell
    new_cell_meas(phy_meas);
    run_tti(1);
  }

  void add_neighbour_cell_nr(uint32_t pci, uint32_t earfcn, float rsrp = 0)
  {
    std::vector<phy_meas_nr_t> phy_meas = {};
    phy_meas_nr_t              meas     = {};
    meas.pci_nr                         = pci;
    meas.arfcn_nr                       = earfcn;
    meas.rsrp                           = rsrp;
    phy_meas.push_back(meas); // neighbour cell
    new_cell_meas_nr(phy_meas);
    run_tti(1);
  }

  using rrc::has_neighbour_cell;
  using rrc::is_serving_cell;
  using rrc::start_cell_select;

  bool get_meas_res(meas_results_s& meas_res) { return pdcptest->get_meas_res(meas_res); }

  phy_test phytest;
  mac_test mactest;

private:
  std::unique_ptr<pdcp_test>   pdcptest;
  std::unique_ptr<nas_test>    nastest;
  std::unique_ptr<rrc_nr_test> rrcnrtest;
  uint32_t                     tti = 0;
};

// Test Cell select
int cell_select_test()
{
  printf("==========================================================\n");
  printf("======            Cell Select Testing      ===============\n");
  printf("==========================================================\n");

  {
    // CHECK: The starting serving cell pci=2 is the weakest, and cell selection procedure chooses pci=1
    // CHECK: phy cell selection is successful, and rrc remains in pci=1
    stack_test_dummy stack;
    rrc_test         rrctest(srslog::fetch_basic_logger("RRC_MEAS").id(), &stack);
    rrctest.init();
    rrctest.connect();

    // Add a first serving cell
    rrctest.add_neighbour_cell(1, 1, 2.0);
    rrctest.set_serving_cell(1, 1);
    TESTASSERT(!rrctest.has_neighbour_cell(1, 1));
    TESTASSERT(!rrctest.has_neighbour_cell(2, 2));

    // Add a second serving cell
    rrctest.add_neighbour_cell(2, 2, 1.0);
    rrctest.set_serving_cell(2, 2);
    TESTASSERT(rrctest.has_neighbour_cell(1, 1));
    TESTASSERT(!rrctest.has_neighbour_cell(2, 2));

    // Start cell selection procedure. The RRC will start with strongest cell
    TESTASSERT(rrctest.start_cell_select() == SRSRAN_SUCCESS);
    stack.run_pending_tasks();
    TESTASSERT(rrctest.phytest.last_selected_cell.earfcn == 2);
    TESTASSERT(rrctest.phytest.last_selected_cell.pci == 2);
    TESTASSERT(!rrctest.has_neighbour_cell(2, 2));
    TESTASSERT(rrctest.has_neighbour_cell(1, 1));
    // Note: cell selection procedure is not done yet at this point.
  }

  {
    // CHECK: The starting serving cell pci=1 is the strongest, and the cell selection procedure calls phy_cell_select
    // for pci=1.
    // CHECK: Cell selection fails in the phy, and rrc moves to pci=2
    stack_test_dummy stack;
    rrc_test         rrctest(srslog::fetch_basic_logger("RRC_MEAS").id(), &stack);
    rrctest.init();
    rrctest.connect();

    rrctest.add_neighbour_cell(1, 1, 2.0);
    rrctest.add_neighbour_cell(2, 2, 1.1);
    rrctest.add_neighbour_cell(3, 2, 1.0);
    rrctest.set_serving_cell(1, 1);
    TESTASSERT(not rrctest.has_neighbour_cell(1, 1));
    TESTASSERT(rrctest.has_neighbour_cell(2, 2));
    TESTASSERT(rrctest.has_neighbour_cell(2, 3));

    // Start cell selection procedure. The RRC will start with strongest cell
    TESTASSERT(rrctest.start_cell_select() == SRSRAN_SUCCESS);
    TESTASSERT(rrctest.phytest.last_selected_cell.earfcn == 1);
    TESTASSERT(rrctest.phytest.last_selected_cell.pci == 1);
    stack.run_pending_tasks();
    rrctest.cell_select_complete(false); // it will fail to select pci=1
    stack.run_pending_tasks();
    rrctest.cell_select_complete(true); // it will select pci=2
    rrctest.in_sync();
    stack.run_pending_tasks(); // it will select pci=2
    rrctest.run_tti(0);        // Needed to advance si acquisition procedure
    TESTASSERT(rrctest.phytest.last_selected_cell.earfcn == 2);
    TESTASSERT(rrctest.phytest.last_selected_cell.pci == 2);
    TESTASSERT(rrctest.has_neighbour_cell(1, 1));
    TESTASSERT(rrctest.has_neighbour_cell(2, 3));
    TESTASSERT(not rrctest.has_neighbour_cell(2, 2));

    // CHECK: UE moves to stronger intra-freq neighbor
    // CHECK: Cell Selection fails, make sure it goes to Cell Search
    rrctest.add_neighbour_cell(4, 2, 100);

    phy_cell_t                               cell_search_cell = {};
    rrc_interface_phy_lte::cell_search_ret_t cell_search_ret  = {};
    cell_search_cell.pci                                      = 5;
    cell_search_cell.earfcn                                   = 5;
    cell_search_ret.found = srsue::rrc_interface_phy_lte::cell_search_ret_t::CELL_FOUND;
    TESTASSERT(rrctest.start_cell_select() == SRSRAN_SUCCESS);
    rrctest.cell_select_complete(false); // it will fail to select pci=2
    stack.run_pending_tasks();
    rrctest.cell_select_complete(false); // it will fail to select pci=3
    stack.run_pending_tasks();
    rrctest.cell_search_complete(cell_search_ret, cell_search_cell);
    stack.run_pending_tasks();
    TESTASSERT(rrctest.phytest.last_selected_cell.earfcn == 5);
    TESTASSERT(rrctest.phytest.last_selected_cell.pci == 5);
    rrctest.cell_select_complete(true);
    rrctest.in_sync();
    stack.run_pending_tasks();
    TESTASSERT(not rrctest.has_neighbour_cell(5, 5));
    TESTASSERT(rrctest.is_serving_cell(5, 5));
  }

  return SRSRAN_SUCCESS;
}

// Tests the measObject configuration and the successful activation of PHY cells to search for
int meas_obj_test()
{
  auto& rrc_meas_logger = srslog::fetch_basic_logger("RRC_MEAS");

  printf("==========================================================\n");
  printf("======    Object Configuration Testing    ===============\n");
  printf("==========================================================\n");

  stack_test_dummy stack;
  rrc_test         rrctest(rrc_meas_logger.id(), &stack);
  rrctest.init();
  rrctest.connect();

  // Configure serving cell. First add neighbour, then set it as serving cell
  rrctest.set_serving_cell(1, 1);

  rrc_conn_recfg_r8_ies_s rrc_conn_recfg = {};
  rrc_conn_recfg.meas_cfg_present        = true;
  meas_cfg_s& meas_cfg                   = rrc_conn_recfg.meas_cfg;

  rrc_meas_logger.info("Test1: Remove non-existing measObject, reportConfig and measId");
  meas_cfg = {};
  meas_cfg.meas_id_to_rem_list.push_back(3);
  meas_cfg.meas_obj_to_rem_list.push_back(3);
  meas_cfg.report_cfg_to_rem_list.push_back(3);
  meas_cfg.meas_id_to_rem_list_present  = true;
  meas_cfg.meas_obj_to_rem_list_present = true;

  // Just test it doesn't crash
  TESTASSERT(rrctest.send_meas_cfg(rrc_conn_recfg));
  TESTASSERT(rrctest.phytest.meas_nof_freqs() == 0);

  rrc_meas_logger.info("Test2: Add measId pointing to non-existing measObject or reportConfig");
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

  rrc_meas_logger.info("Test3: Add meaObject and report of unsupported type. Setup a supported report for later use");
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

  rrc_meas_logger.info("Test4: Add 2 measObjects and 2 measId both pointing to the same measObject ");
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

  rrc_meas_logger.info("Test5: Add existing objects and measId. Now add measId for 2nd cell");
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
  rrc_meas_logger.info("Test6: Add 1 cell to 1st object. Mixed add/mod and removal command.");
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

  rrc_meas_logger.info("Test7: PHY finds new neighbours in frequency 1 and 2, check RRC instructs to search them");
  std::vector<phy_meas_t> phy_meas = {};
  phy_meas.push_back({srsran::srsran_rat_t::lte, 0, 0, 0.0f, 1, 31});
  phy_meas.push_back({srsran::srsran_rat_t::lte, -1, 0, 0.0f, 1, 32});
  phy_meas.push_back({srsran::srsran_rat_t::lte, -2, 0, 0.0f, 1, 33});
  phy_meas.push_back({srsran::srsran_rat_t::lte, -3, 0, 0.0f, 1, 34});
  rrctest.new_cell_meas(phy_meas);
  rrctest.run_tti(1);
  phy_meas = {};
  phy_meas.push_back({srsran::srsran_rat_t::lte, -4, 0, 0.0f, 1, 35});
  phy_meas.push_back({srsran::srsran_rat_t::lte, -5, 0, 0.0f, 1, 36});
  phy_meas.push_back({srsran::srsran_rat_t::lte, -6, 0, 0.0f, 1, 37});
  phy_meas.push_back({srsran::srsran_rat_t::lte, 1, 0, 0.0f, 1, 30});
  phy_meas.push_back({srsran::srsran_rat_t::lte, 0, 0, 0.0f, 2, 31});
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

  rrc_meas_logger.info("Test8: Simulate a Release (reset() call) make sure resets correctly");
  rrctest.init();
  rrctest.run_tti(1);
  rrctest.connect();
  rrctest.run_tti(1);

  rrc_meas_logger.info("Test9: Config removal");
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
  std::vector<phy_meas_t> phy_meas = {};
  for (uint32_t i = 0; i < pci.size(); i++) {
    float r = rsrp[0];
    if (rsrp.size() == pci.size()) {
      r = rsrp[i];
    }
    uint32_t e = earfcn[0];
    if (earfcn.size() == pci.size()) {
      e = earfcn[i];
    }
    phy_meas.push_back({srsran::srsran_rat_t::lte, r, -5, 0.0f, e, pci[i]});
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
  auto& rrc_meas_logger = srslog::fetch_basic_logger("RRC_MEAS");

  printf("==========================================================\n");
  printf("============       Report Testing  A1      ===============\n");
  printf("==========================================================\n");

  stack_test_dummy stack;
  rrc_test         rrctest(rrc_meas_logger.id(), &stack);
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
    rrc_meas_logger.info("Report %d/%d enter condition is true", i, ttt_iters / 2);
    enter_condition(rrctest, event_id, hyst, 0, {1, 2});
    // Check doesn't generate measurement report
    TESTASSERT(!rrctest.get_meas_res(meas_res));
  }

  rrc_meas_logger.info("Report leaving enter condition");
  // Not satisfy entering condition for 1 TTI
  middle_condition(rrctest, event_id, hyst, 0, {1});
  TESTASSERT(!rrctest.get_meas_res(meas_res));

  // Should go again all timeToTrigger, should not trigger measurement until end
  for (int i = 0; i < ttt_iters; i++) {
    rrc_meas_logger.info("Report %d/%d enter condition is true", i, ttt_iters);
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
      rrc_meas_logger.info("Report %d/%d enter condition is true", i, ttt_iters);
      enter_condition(rrctest, event_id, hyst, 0, {1});
    }
    // Do not expect report if timer not expired
    TESTASSERT(!rrctest.get_meas_res(meas_res));
    // Wait to generate all reports
    for (int i = 0; i < report_amount.to_number() - 1; i++) {
      rrc_meas_logger.info("Testing report %d/%d", i, report_amount.to_number());
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
          rrc_meas_logger.info("Stepping timer %d/%d", j, interval);
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
      rrc_meas_logger.info("Report %d/%d enter condition is true", i, ttt_iters);
      enter_condition(rrctest, event_id, hyst, 0, {1});
    }
    // Do not expect report
    TESTASSERT(!rrctest.get_meas_res(meas_res));

    // Leaving condition for timeToTrigger
    for (int i = 0; i < ttt_iters; i++) {
      rrc_meas_logger.info("Report %d/%d leaving condition is true", i, ttt_iters);
      exit_condition(rrctest, event_id, hyst, 0, {1});
      // Check doesn't generate measurement report
      TESTASSERT(!rrctest.get_meas_res(meas_res));
    }
    // Trigger again condition
    for (int i = 0; i < ttt_iters; i++) {
      rrc_meas_logger.info("Report %d/%d enter condition is true", i, ttt_iters);
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
  auto& rrc_meas_logger = srslog::fetch_basic_logger("RRC_MEAS");

  printf("==========================================================\n");
  printf("============       Report Testing A3       ===============\n");
  printf("==========================================================\n");

  stack_test_dummy stack;
  rrc_test         rrctest(rrc_meas_logger.id(), &stack);
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

  rrc_meas_logger.info("Test no-enter condition and no trigger report ");
  no_condition(rrctest, {0}, {1});
  TESTASSERT(!rrctest.get_meas_res(meas_res));

  no_condition(rrctest, {0, 1}, {1, 0});
  TESTASSERT(!rrctest.get_meas_res(meas_res));

  rrc_meas_logger.info("Test enter condition triggers report. 1 neighbour cell in enter + 1 in exit ");
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
  rrc_meas_logger.info("Test enter condition for the same cell does not trigger report");
  rrctest.run_tti(1);
  TESTASSERT(!rrctest.get_meas_res(meas_res));

  rrc_meas_logger.info("Test enter condition for different earfcn triggers report");
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
  rrc_meas_logger.info("Test a new cell enter condition triggers report");
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
  rrc_meas_logger.info("Test exit condition");
  exit_condition(rrctest, event_id, hyst, 1, {1, 0});
  if (report_on_leave) {
    TESTASSERT(rrctest.get_meas_res(meas_res));
  }

  // 2 enters again, now expect report again
  rrc_meas_logger.info("Test trigger again the cell that exited");
  enter_condition(rrctest, event_id, hyst, 1, {1, 0});
  TESTASSERT(rrctest.get_meas_res(meas_res));
  TESTASSERT(meas_res.meas_id == 1);
  TESTASSERT(meas_res.meas_result_neigh_cells_present);
  TESTASSERT(meas_res.meas_result_neigh_cells.meas_result_list_eutra().size() == 2);
  TESTASSERT(meas_res.meas_result_neigh_cells.meas_result_list_eutra()[0].pci == 3);
  TESTASSERT(meas_res.meas_result_neigh_cells.meas_result_list_eutra()[0].meas_result.rsrp_result ==
             81 + (hyst + a3_offset) / 2);

  return SRSRAN_SUCCESS;
}

// Minimal testcase for testing inter rat reporting with nr
int meas_obj_inter_rat_nr_test()
{
  printf("==========================================================\n");
  printf("======    NR Inter Rat Configuration Testing    ==========\n");
  printf("==========================================================\n");

  stack_test_dummy stack;
  rrc_test         rrctest(srslog::fetch_basic_logger("RRC_MEAS").id(), &stack);
  rrctest.init();
  rrctest.connect();

  rrc_conn_recfg_r8_ies_s rrc_conn_recfg = {};
  rrc_conn_recfg.meas_cfg_present        = true;
  meas_cfg_s& meas_cfg                   = rrc_conn_recfg.meas_cfg;

  meas_obj_to_add_mod_s obj = {};

  obj.meas_obj_id = 2;
  obj.meas_obj.set_meas_obj_nr_r15();
  obj.meas_obj.meas_obj_nr_r15().ext              = true;
  obj.meas_obj.meas_obj_nr_r15().carrier_freq_r15 = 631680;
  obj.meas_obj.meas_obj_nr_r15().rs_cfg_ssb_r15.meas_timing_cfg_r15.periodicity_and_offset_r15.set_sf20_r15();
  obj.meas_obj.meas_obj_nr_r15().rs_cfg_ssb_r15.meas_timing_cfg_r15.ssb_dur_r15 =
      asn1::rrc::mtc_ssb_nr_r15_s::ssb_dur_r15_opts::options::sf1;
  obj.meas_obj.meas_obj_nr_r15().rs_cfg_ssb_r15.subcarrier_spacing_ssb_r15 =
      asn1::rrc::rs_cfg_ssb_nr_r15_s::subcarrier_spacing_ssb_r15_opts::options::khz30;
  obj.meas_obj.meas_obj_nr_r15().quant_cfg_set_r15 = 1;
  obj.meas_obj.meas_obj_nr_r15().band_nr_r15.reset(new asn1::rrc::meas_obj_nr_r15_s::band_nr_r15_c_{});
  obj.meas_obj.meas_obj_nr_r15().band_nr_r15->set_setup() = 78;

  meas_cfg.meas_obj_to_add_mod_list.push_back(obj);

  obj             = {};
  obj.meas_obj_id = 1;
  obj.meas_obj.set_meas_obj_eutra();
  obj.meas_obj.meas_obj_eutra().carrier_freq       = 300;
  obj.meas_obj.meas_obj_eutra().allowed_meas_bw    = asn1::rrc::allowed_meas_bw_opts::options::mbw100;
  obj.meas_obj.meas_obj_eutra().presence_ant_port1 = true;
  obj.meas_obj.meas_obj_eutra().neigh_cell_cfg.from_number(01);

  meas_cfg.meas_obj_to_add_mod_list.push_back(obj);
  meas_cfg.meas_obj_to_add_mod_list_present = true;

  report_cfg_to_add_mod_s rep = {};
  rep.report_cfg_id           = 1;
  rep.report_cfg.set_report_cfg_inter_rat();
  rep.report_cfg.report_cfg_inter_rat().ext = true;
  rep.report_cfg.report_cfg_inter_rat().trigger_type.set_event();
  rep.report_cfg.report_cfg_inter_rat().trigger_type.event().event_id.set_event_b1_nr_r15();
  rep.report_cfg.report_cfg_inter_rat()
      .trigger_type.event()
      .event_id.event_b1_nr_r15()
      .b1_thres_nr_r15.set_nr_rsrp_r15();
  rep.report_cfg.report_cfg_inter_rat().trigger_type.event().event_id.event_b1_nr_r15().b1_thres_nr_r15.nr_rsrp_r15() =
      56;
  rep.report_cfg.report_cfg_inter_rat().trigger_type.event().event_id.event_b1_nr_r15().report_on_leave_r15 = true;
  rep.report_cfg.report_cfg_inter_rat().trigger_type.event().hysteresis                                     = 0;
  rep.report_cfg.report_cfg_inter_rat().trigger_type.event().time_to_trigger =
      asn1::rrc::time_to_trigger_opts::options::ms100;
  rep.report_cfg.report_cfg_inter_rat().max_report_cells = 8;
  rep.report_cfg.report_cfg_inter_rat().report_interv    = asn1::rrc::report_interv_opts::options::ms120;
  rep.report_cfg.report_cfg_inter_rat().report_amount    = asn1::rrc::report_cfg_inter_rat_s::report_amount_opts::r1;
  rep.report_cfg.report_cfg_inter_rat().report_quant_cell_nr_r15.reset(new asn1::rrc::report_quant_nr_r15_s{});
  rep.report_cfg.report_cfg_inter_rat().report_quant_cell_nr_r15->ss_rsrp = true;
  rep.report_cfg.report_cfg_inter_rat().report_quant_cell_nr_r15->ss_rsrq = true;
  rep.report_cfg.report_cfg_inter_rat().report_quant_cell_nr_r15->ss_sinr = true;

  meas_cfg.report_cfg_to_add_mod_list.push_back(rep);
  meas_cfg.report_cfg_to_add_mod_list_present = true;

  meas_cfg.meas_id_to_add_mod_list_present = true;
  meas_id_to_add_mod_s meas                = {};
  meas.meas_id                             = 1;
  meas.meas_obj_id                         = 2;
  meas.report_cfg_id                       = 1;
  meas_cfg.meas_id_to_add_mod_list.push_back(meas);

  // Just test it doesn't crash
  TESTASSERT(rrctest.send_meas_cfg(rrc_conn_recfg));
  TESTASSERT(rrctest.phytest.meas_nof_freqs() == 0);

  rrctest.add_neighbour_cell(2, 300, 2.0);
  rrctest.set_serving_cell(2, 300);
  rrctest.add_neighbour_cell_nr(500, 631680, -60.0);
  int ttt_iters = 100 + 1; // 100 ms

  for (int i = 0; i < ttt_iters; i++) {
    srslog::fetch_basic_logger("RRC_MEAS").info("Report %d/%d enter condition is true", i, ttt_iters);
    rrctest.add_neighbour_cell_nr(500, 631680, -60.0);
  }

  return SRSRAN_SUCCESS;
}

int main(int argc, char** argv)
{
  auto& rrc_meas_logger = srslog::fetch_basic_logger("RRC_MEAS", false);
  rrc_meas_logger.set_level(srslog::basic_levels::debug);
  rrc_meas_logger.set_hex_dump_max_size(-1);
  auto& rrc_logger = srslog::fetch_basic_logger("RRC", false);
  rrc_logger.set_level(srslog::basic_levels::debug);
  rrc_logger.set_hex_dump_max_size(-1);

  srslog::init();

  TESTASSERT(cell_select_test() == SRSRAN_SUCCESS);
  TESTASSERT(meas_obj_test() == SRSRAN_SUCCESS);
  TESTASSERT(meas_obj_inter_rat_nr_test() == SRSRAN_SUCCESS);
  TESTASSERT(
      a1event_report_test(
          30, time_to_trigger_opts::ms40, 3, report_cfg_eutra_s::report_amount_opts::r1, report_interv_opts::ms120) ==
      SRSRAN_SUCCESS);
  TESTASSERT(
      a1event_report_test(
          30, time_to_trigger_opts::ms0, 3, report_cfg_eutra_s::report_amount_opts::r1, report_interv_opts::ms120) ==
      SRSRAN_SUCCESS);
  TESTASSERT(
      a1event_report_test(
          30, time_to_trigger_opts::ms40, 3, report_cfg_eutra_s::report_amount_opts::r8, report_interv_opts::ms120) ==
      SRSRAN_SUCCESS);
  TESTASSERT(a3event_report_test(6, 3, true) == SRSRAN_SUCCESS);
  return SRSRAN_SUCCESS;
}
