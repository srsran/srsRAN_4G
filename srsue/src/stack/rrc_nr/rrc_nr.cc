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

#include "srsue/hdr/stack/rrc_nr/rrc_nr.h"
#include "srsran/common/band_helper.h"
#include "srsran/common/security.h"
#include "srsran/common/standard_streams.h"
#include "srsran/interfaces/ue_pdcp_interfaces.h"
#include "srsran/interfaces/ue_rlc_interfaces.h"
#include "srsue/hdr/stack/rrc_nr/rrc_nr_procedures.h"
#include "srsue/hdr/stack/upper/usim.h"

using namespace asn1::rrc_nr;
using namespace asn1;
using namespace srsran;

namespace srsue {

const static char* rrc_nr_state_text[] = {"IDLE", "CONNECTED", "CONNECTED-INACTIVE"};

rrc_nr::rrc_nr(srsran::task_sched_handle task_sched_) :
  logger(srslog::fetch_basic_logger("RRC-NR")),
  task_sched(task_sched_),
  conn_recfg_proc(*this),
  conn_setup_proc(*this),
  setup_req_proc(*this),
  cell_selector(*this),
  meas_cells(task_sched_)
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  random_gen = srsran_random_init(tv.tv_usec);
  set_phy_default_config();
}

rrc_nr::~rrc_nr()
{
  srsran_random_free(random_gen);
}

int rrc_nr::init(phy_interface_rrc_nr*       phy_,
                 mac_interface_rrc_nr*       mac_,
                 rlc_interface_rrc*          rlc_,
                 pdcp_interface_rrc*         pdcp_,
                 sdap_interface_rrc*         sdap_,
                 gw_interface_rrc*           gw_,
                 nas_5g_interface_rrc_nr*    nas_,
                 rrc_eutra_interface_rrc_nr* rrc_eutra_,
                 usim_interface_rrc_nr*      usim_,
                 srsran::timer_handler*      timers_,
                 stack_interface_rrc*        stack_,
                 const rrc_nr_args_t&        args_)
{
  phy       = phy_;
  rlc       = rlc_;
  pdcp      = pdcp_;
  sdap      = sdap_;
  gw        = gw_;
  nas       = nas_;
  mac       = mac_;
  rrc_eutra = rrc_eutra_;
  usim      = usim_;
  stack     = stack_;
  args      = args_;

  // allocate RRC timers
  t300 = task_sched.get_unique_timer();
  t301 = task_sched.get_unique_timer();
  t302 = task_sched.get_unique_timer();
  t304 = task_sched.get_unique_timer();
  t310 = task_sched.get_unique_timer();
  t311 = task_sched.get_unique_timer();

  if (rrc_eutra == nullptr) {
    // SA mode
    plmn_is_selected = true;

    // setup inital HARQ config
    srsran::dl_harq_cfg_nr_t harq_cfg = {};
    harq_cfg.nof_procs                = 8;
    mac->set_config(harq_cfg);

    // Setup SRB0
    logical_channel_config_t lch = {};
    mac->setup_lcid(lch);

    // Carrier config
    srsran::srsran_band_helper bands;
    phy_cfg.carrier.dl_center_frequency_hz = bands.nr_arfcn_to_freq(args.dl_nr_arfcn);
    phy_cfg.carrier.ul_center_frequency_hz = bands.nr_arfcn_to_freq(bands.get_ul_arfcn_from_dl_arfcn(args.dl_nr_arfcn));
    phy_cfg.carrier.ssb_center_freq_hz     = bands.nr_arfcn_to_freq(args.ssb_nr_arfcn);
    phy_cfg.carrier.nof_prb                = args.nof_prb;
    phy_cfg.carrier.max_mimo_layers        = 1;
    phy_cfg.carrier.scs                    = args.scs;
    phy_cfg.duplex.mode                    = bands.get_duplex_mode(bands.get_band_from_dl_arfcn(args.dl_nr_arfcn));

    // SSB configuration
    phy_cfg.ssb.periodicity_ms       = 10;
    phy_cfg.ssb.position_in_burst[0] = true;
    phy_cfg.ssb.scs                  = args.ssb_scs;
  }

  running               = true;
  sim_measurement_timer = task_sched.get_unique_timer();
  return SRSRAN_SUCCESS;
}

void rrc_nr::stop()
{
  running = false;
}

void rrc_nr::get_metrics(rrc_nr_metrics_t& m)
{
  m.state = state;
}

const char* rrc_nr::get_rb_name(uint32_t lcid)
{
  if (is_nr_srb(lcid)) {
    return get_srb_name(nr_lcid_to_srb(lcid));
  }
  if (lcid_drb.find(lcid) != lcid_drb.end()) {
    return get_drb_name(lcid_drb[lcid]);
  }
  logger.warning("Unable to find lcid: %d. Return invalid LCID");
  return "invalid LCID";
}

// Timeout callback interface
void rrc_nr::timer_expired(uint32_t timeout_id)
{
  logger.debug("Handling Timer Expired");
  if (timeout_id == sim_measurement_timer.id() && rrc_eutra != nullptr) {
    logger.debug("Triggered simulated measurement");

    phy_meas_nr_t              sim_meas = {};
    std::vector<phy_meas_nr_t> phy_meas_nr;
    sim_meas.rsrp     = -60.0;
    sim_meas.rsrq     = -60.0;
    sim_meas.cfo_hz   = 1.0;
    sim_meas.arfcn_nr = sim_measurement_carrier_freq_r15;
    sim_meas.pci_nr   = args.sim_nr_meas_pci;
    phy_meas_nr.push_back(sim_meas);
    rrc_eutra->new_cell_meas_nr(phy_meas_nr);

    auto timer_expire_func = [this](uint32_t tid) { timer_expired(tid); };
    sim_measurement_timer.set(sim_measurement_timer_duration_ms, timer_expire_func);
    sim_measurement_timer.run();
  }
}

void rrc_nr::srsran_rrc_log(const char* str) {}

template <class T>
void rrc_nr::log_rrc_message(const std::string&           source,
                             direction_t                  dir,
                             const srsran::byte_buffer_t* pdu,
                             const T&                     msg,
                             const std::string&           msg_type)
{
  if (logger.debug.enabled()) {
    asn1::json_writer json_writer;
    msg.to_json(json_writer);
    logger.debug(pdu->msg,
                 pdu->N_bytes,
                 "%s - %s %s (%d B)",
                 source.c_str(),
                 (dir == Rx) ? "Rx" : "Tx",
                 msg_type.c_str(),
                 pdu->N_bytes);
    logger.debug("Content:%s", json_writer.to_string().c_str());
  } else if (logger.info.enabled()) {
    logger.info("%s - %s %s (%d B)", source.c_str(), (dir == Rx) ? "Rx" : "Tx", msg_type.c_str(), pdu->N_bytes);
  }
}

template <class T>
void rrc_nr::log_rrc_message(const std::string& source,
                             direction_t        dir,
                             dyn_octstring      oct,
                             const T&           msg,
                             const std::string& msg_type)
{
  if (logger.debug.enabled()) {
    asn1::json_writer json_writer;
    msg.to_json(json_writer);
    logger.debug(oct.data(),
                 oct.size(),
                 "%s - %s %s (%d B)",
                 source.c_str(),
                 (dir == Rx) ? "Rx" : "Tx",
                 msg_type.c_str(),
                 oct.size());
    logger.debug("Content:%s", json_writer.to_string().c_str());
  } else if (logger.info.enabled()) {
    logger.info("%s - %s %s (%d B)", source.c_str(), (dir == Rx) ? "Rx" : "Tx", msg_type.c_str(), oct.size());
  }
}

bool rrc_nr::add_lcid_drb(uint32_t lcid, uint32_t drb_id)
{
  if (lcid_drb.find(lcid) != lcid_drb.end()) {
    logger.error("Couldn't add DRB to LCID (%d). DRB %d already exists.", lcid, drb_id);
    return false;
  } else {
    logger.info("Adding lcid %d and radio bearer ID %d", lcid, drb_id);
    lcid_drb[lcid] = nr_drb_id_to_drb(drb_id);
  }
  return true;
}

uint32_t rrc_nr::get_lcid_for_drbid(uint32_t drb_id)
{
  for (auto& rb : lcid_drb) {
    if (rb.second == nr_drb_id_to_drb(drb_id)) {
      return rb.first;
    }
  }
  logger.error("Couldn't find LCID for DRB. DRB %d does exist.", drb_id);
  return 0;
}

// PHY interface
void rrc_nr::in_sync() {}
void rrc_nr::out_of_sync() {}

// MAC interface
void rrc_nr::run_tti(uint32_t tti) {}

// PDCP interface
void rrc_nr::write_pdu(uint32_t lcid, srsran::unique_byte_buffer_t pdu)
{
  logger.debug("RX PDU, LCID: %d", lcid);
  switch (static_cast<nr_srb>(lcid)) {
    case nr_srb::srb0:
      decode_dl_ccch(std::move(pdu));
      break;
    case nr_srb::srb1:
    case nr_srb::srb2:
      decode_dl_dcch(lcid, std::move(pdu));
      break;
    default:
      logger.error("RX PDU with invalid bearer id: %d", lcid);
      break;
  }
}

void rrc_nr::decode_dl_ccch(unique_byte_buffer_t pdu)
{
  asn1::cbit_ref              bref(pdu->msg, pdu->N_bytes);
  asn1::rrc_nr::dl_ccch_msg_s dl_ccch_msg;
  if (dl_ccch_msg.unpack(bref) != asn1::SRSASN_SUCCESS or
      dl_ccch_msg.msg.type().value != dl_ccch_msg_type_c::types_opts::c1) {
    logger.error(pdu->msg, pdu->N_bytes, "Failed to unpack DL-CCCH message (%d B)", pdu->N_bytes);
    return;
  }
  log_rrc_message(
      get_rb_name(srb_to_lcid(nr_srb::srb0)), Rx, pdu.get(), dl_ccch_msg, dl_ccch_msg.msg.c1().type().to_string());

  dl_ccch_msg_type_c::c1_c_* c1 = &dl_ccch_msg.msg.c1();
  switch (dl_ccch_msg.msg.c1().type().value) {
    case dl_ccch_msg_type_c::c1_c_::types::rrc_reject: {
      // 5.3.15
      const auto& reject = c1->rrc_reject();
      srsran::console("Received RRC Reject\n");

      t300.stop();

      if (reject.crit_exts.rrc_reject().wait_time_present) {
        // nas->set_barring(srsran::barring_t::all);
        t302.set(reject.crit_exts.rrc_reject().wait_time * 1000, [this](uint32_t tid) { timer_expired(tid); });
        t302.run();
      } else {
        // Perform the actions upon expiry of T302 if wait time is zero
        // nas->set_barring(srsran::barring_t::none);
        // start_go_idle();
      }
    } break;
    case dl_ccch_msg_type_c::c1_c_::types::rrc_setup: {
      transaction_id             = c1->rrc_setup().rrc_transaction_id;
      rrc_setup_s rrc_setup_copy = c1->rrc_setup();
      task_sched.defer_task([this, rrc_setup_copy]() { handle_rrc_setup(rrc_setup_copy); });
      break;
    }
    default:
      logger.error("The provided DL-CCCH message type is not recognized");
      break;
  }
}

void rrc_nr::decode_dl_dcch(uint32_t lcid, unique_byte_buffer_t pdu)
{
  asn1::cbit_ref              bref(pdu->msg, pdu->N_bytes);
  asn1::rrc_nr::dl_dcch_msg_s dl_dcch_msg;
  if (dl_dcch_msg.unpack(bref) != asn1::SRSASN_SUCCESS or
      dl_dcch_msg.msg.type().value != dl_dcch_msg_type_c::types_opts::c1) {
    logger.error(pdu->msg, pdu->N_bytes, "Failed to unpack DL-DCCH message (%d B)", pdu->N_bytes);
    return;
  }
  log_rrc_message(get_rb_name(lcid), Rx, pdu.get(), dl_dcch_msg, dl_dcch_msg.msg.c1().type().to_string());

  dl_dcch_msg_type_c::c1_c_* c1 = &dl_dcch_msg.msg.c1();
  switch (dl_dcch_msg.msg.c1().type().value) {
    // TODO: ADD missing cases
    case dl_dcch_msg_type_c::c1_c_::types::rrc_recfg: {
      rrc_recfg_s recfg = c1->rrc_recfg();
      task_sched.defer_task([this, recfg]() { handle_rrc_reconfig(recfg); });
      break;
    }
    case dl_dcch_msg_type_c::c1_c_::types::dl_info_transfer: {
      dl_info_transfer_s dl_info_transfer = c1->dl_info_transfer();
      task_sched.defer_task([this, dl_info_transfer]() { handle_dl_info_transfer(dl_info_transfer); });
      break;
    }
    case dl_dcch_msg_type_c::c1_c_::types::security_mode_cmd: {
      security_mode_cmd_s smc = c1->security_mode_cmd();
      task_sched.defer_task([this, smc]() { handle_security_mode_command(smc); });
      break;
    }
    case dl_dcch_msg_type_c::c1_c_::types::rrc_release: {
      rrc_release_s rrc_release = c1->rrc_release();
      task_sched.defer_task([this, rrc_release]() { handle_rrc_release(rrc_release); });
      break;
    }
    case dl_dcch_msg_type_c::c1_c_::types::ue_cap_enquiry: {
      ue_cap_enquiry_s ue_cap_enquiry = c1->ue_cap_enquiry();
      task_sched.defer_task([this, ue_cap_enquiry]() { handle_ue_capability_enquiry(ue_cap_enquiry); });
      break;
    }
    default:
      logger.error("The provided DL-DCCH message type is not recognized or supported.");
      break;
  }
}

void rrc_nr::write_pdu_bcch_bch(srsran::unique_byte_buffer_t pdu) {}
void rrc_nr::write_pdu_bcch_dlsch(srsran::unique_byte_buffer_t pdu)
{
  decode_pdu_bcch_dlsch(std::move(pdu));
}

void rrc_nr::decode_pdu_bcch_dlsch(srsran::unique_byte_buffer_t pdu)
{
  // Stop BCCH search after successful reception of 1 BCCH block
  // mac->bcch_stop_rx();

  bcch_dl_sch_msg_s dlsch_msg;
  asn1::cbit_ref    dlsch_bref(pdu->msg, pdu->N_bytes);
  asn1::SRSASN_CODE err = dlsch_msg.unpack(dlsch_bref);

  if (err != asn1::SRSASN_SUCCESS or dlsch_msg.msg.type().value != bcch_dl_sch_msg_type_c::types_opts::c1) {
    logger.error(pdu->msg, pdu->N_bytes, "Could not unpack BCCH DL-SCH message (%d B).", pdu->N_bytes);
    return;
  }

  log_rrc_message("BCCH-DLSCH", Rx, pdu.get(), dlsch_msg, dlsch_msg.msg.c1().type().to_string());

  if (dlsch_msg.msg.c1().type() == bcch_dl_sch_msg_type_c::c1_c_::types::sib_type1) {
    logger.info("Processing SIB1 (1/1)");
    handle_sib1(dlsch_msg.msg.c1().sib_type1());
  }
}

void rrc_nr::set_phy_default_config()
{
  phy_cfg = {};

  // uses default values provided in 38.311 TS 138 331 V16.6 page 361
  if (make_phy_beta_offsets({}, &phy_cfg.pusch.beta_offsets) == false) {
    logger.warning("Couldn't set default beta_offsets config");
  }

  // no default value provided, asume factor 1.0
  uci_on_pusch_s uci_on_pusch = {};
  uci_on_pusch.scaling        = uci_on_pusch_s::scaling_opts::f1;
  if (make_phy_pusch_scaling(uci_on_pusch, &phy_cfg.pusch.scaling) == false) {
    logger.warning("Couldn't set default scaling config");
  }

  // no default value specified, use dynamic
  phys_cell_group_cfg_s phys_cell_group_cfg   = {};
  phys_cell_group_cfg.pdsch_harq_ack_codebook = phys_cell_group_cfg_s::pdsch_harq_ack_codebook_opts::dynamic_value;
  if (make_phy_harq_ack_cfg(phys_cell_group_cfg, &phy_cfg.harq_ack) == false) {
    logger.warning("Couldn't set default HARQ ack config");
  }
}

void rrc_nr::handle_sib1(const sib1_s& sib1)
{
  if (meas_cells.serving_cell().has_sib1()) {
    logger.info("SIB1 already processed");
    return;
  }

  meas_cells.serving_cell().set_sib1(sib1);

  logger.info("SIB1 received, CellID=%d", meas_cells.serving_cell().get_cell_id() & 0xfff);

  // clang-format off
  // unhandled fields:
  // - cellSelectionInfo
  // - cellAccessRelatedInfo
  // - connEstFailureControl
  // - servingCellConfigCommon:
  //    - downlinkConfigCommon.frequencyInfoDL.frequencyBandList
  //    - downlinkConfigCommon.frequencyInfoDL.offsetToPointA
  //    - downlinkConfigCommon.initialDownlinkBWP.genericParameters
  //    - downlinkConfigCommon.initialDownlinkBWP.pdcch-ConfigCommon.commonSearchSpaceList.searchSpaceSIB1
  //    - downlinkConfigCommon.initialDownlinkBWP.pdcch-ConfigCommon.commonSearchSpaceList.search_space_other_sys_info
  //    - downlinkConfigCommon.initialDownlinkBWP.pdcch-ConfigCommon.commonSearchSpaceList.paging_search_space
  //    - downlinkConfigCommon.bcch-Config
  //    - downlinkConfigCommon.pcch-Config
  //    - uplinkConfigCommon.frequencyInfoUL.frequencyBandList
  //    - uplinkConfigCommon.frequencyInfoUL.p_max
  //    - uplinkConfigCommon.initialUplinkBWP.genericParameters
  //    - uplinkConfigCommon.initialUplinkBWP.rach-ConfigCommon.rach-ConfigGeneric.msg1-FDM
  //    - uplinkConfigCommon.initialUplinkBWP.rach-ConfigCommon.ssb_per_rach_occasion_and_cb_preambs_per_ssb
  //    - uplinkConfigCommon.initialUplinkBWP.rach-ConfigCommon.restricted_set_cfg
  //    - uplinkConfigCommon.initialUplinkBWP.pusch-ConfigCommon.pusch-TimeDomainResourceAllocationList.p0-NominalWithGrant
  //    - ss-PBCH-BlockPower
  // clang-format on

  // ue-TimersAndConstants
  auto timer_expire_func = [this](uint32_t tid) { timer_expired(tid); };
  t300.set(sib1.ue_timers_and_consts.t300.to_number(), timer_expire_func);
  t301.set(sib1.ue_timers_and_consts.t301.to_number(), timer_expire_func);
  t310.set(sib1.ue_timers_and_consts.t310.to_number(), timer_expire_func);
  t311.set(sib1.ue_timers_and_consts.t311.to_number(), timer_expire_func);
  N310 = sib1.ue_timers_and_consts.n310.to_number();
  N311 = sib1.ue_timers_and_consts.n311.to_number();

  logger.info("Set Constants and Timers: N310=%d, N311=%d, t300=%d, t301=%d, t310=%d, t311=%d",
              N310,
              N311,
              t300.duration(),
              t301.duration(),
              t310.duration(),
              t311.duration());

  // Apply RACH and timeAlginmentTimer configuration
  mac_cfg_nr_t mac_cfg = {};
  make_mac_rach_cfg(sib1.serving_cell_cfg_common.ul_cfg_common.init_ul_bwp.rach_cfg_common.setup(), &mac_cfg.rach_cfg);
  mac_cfg.time_alignment_timer = sib1.serving_cell_cfg_common.ul_cfg_common.time_align_timer_common.to_number();

  mac->set_config(mac_cfg.rach_cfg);

  // Apply PDSCH Config Common
  if (sib1.serving_cell_cfg_common.dl_cfg_common.init_dl_bwp.pdsch_cfg_common.setup()
          .pdsch_time_domain_alloc_list.size() > 0) {
    if (not fill_phy_pdsch_cfg_common(sib1.serving_cell_cfg_common.dl_cfg_common.init_dl_bwp.pdsch_cfg_common.setup(),
                                      &phy_cfg.pdsch)) {
      logger.warning("Could not set PDSCH config.");
    }
  }

  // Apply PUSCH Config Common
  if (not fill_phy_pusch_cfg_common(sib1.serving_cell_cfg_common.ul_cfg_common.init_ul_bwp.pusch_cfg_common.setup(),
                                    &phy_cfg.pusch)) {
    logger.warning("Could not set PUSCH config.");
  }

  // Apply PUCCH Config Common
  fill_phy_pucch_cfg_common(sib1.serving_cell_cfg_common.ul_cfg_common.init_ul_bwp.pucch_cfg_common.setup(),
                            &phy_cfg.pucch.common);

  // Apply RACH Config Common
  if (not make_phy_rach_cfg(sib1.serving_cell_cfg_common.ul_cfg_common.init_ul_bwp.rach_cfg_common.setup(),
                            sib1.serving_cell_cfg_common.tdd_ul_dl_cfg_common_present ? SRSRAN_DUPLEX_MODE_TDD
                                                                                      : SRSRAN_DUPLEX_MODE_FDD,
                            &phy_cfg.prach)) {
    logger.warning("Could not set phy rach config.");
    return;
  }

  // Apply PDCCH Config Common
  fill_phy_pdcch_cfg_common(sib1.serving_cell_cfg_common.dl_cfg_common.init_dl_bwp.pdcch_cfg_common.setup(),
                            &phy_cfg.pdcch);

  // Apply Carrier Config
  fill_phy_carrier_cfg(sib1.serving_cell_cfg_common, &phy_cfg.carrier);

  // Apply SSB Config
  fill_phy_ssb_cfg(sib1.serving_cell_cfg_common, &phy_cfg.ssb);

  // Apply n-TimingAdvanceOffset
  if (sib1.serving_cell_cfg_common.n_timing_advance_offset_present) {
    switch (sib1.serving_cell_cfg_common.n_timing_advance_offset.value) {
      case serving_cell_cfg_common_sib_s::n_timing_advance_offset_opts::n0:
        phy_cfg.t_offset = 0;
        break;
      case serving_cell_cfg_common_sib_s::n_timing_advance_offset_opts::n25600:
        phy_cfg.t_offset = 25600;
        break;
      case serving_cell_cfg_common_sib_s::n_timing_advance_offset_opts::n39936:
        phy_cfg.t_offset = 39936;
        break;
      default:
        logger.error("Invalid n_ta_offset option");
        break;
    }
  } else {
    phy_cfg.t_offset = 25600;
  }

  phy_cfg_state = PHY_CFG_STATE_SA_SIB_CFG;
  if (not phy->set_config(phy_cfg)) {
    logger.warning("Could not set phy config.");
    return;
  }

  // Notify cell selector of successful SIB1 reception
  cell_selector.trigger(true);
}

void rrc_nr::write_pdu_pcch(srsran::unique_byte_buffer_t pdu) {}
void rrc_nr::write_pdu_mch(uint32_t lcid, srsran::unique_byte_buffer_t pdu) {}
void rrc_nr::notify_pdcp_integrity_error(uint32_t lcid) {}

// NAS interface
int rrc_nr::write_sdu(srsran::unique_byte_buffer_t sdu)
{
  if (state == RRC_NR_STATE_IDLE) {
    logger.warning("Received ULInformationTransfer SDU when in IDLE");
    return SRSRAN_ERROR;
  }
  send_ul_info_transfer(std::move(sdu));
  return SRSRAN_SUCCESS;
}

bool rrc_nr::is_connected()
{
  return state == RRC_NR_STATE_CONNECTED;
}

int rrc_nr::connection_request(srsran::nr_establishment_cause_t cause, srsran::unique_byte_buffer_t dedicated_info_nas_)
{
  if (not setup_req_proc.launch(cause, std::move(dedicated_info_nas_))) {
    logger.error("Failed to initiate setup request procedure");
    return SRSRAN_ERROR;
  }
  callback_list.add_proc(setup_req_proc);
  return SRSRAN_SUCCESS;
}

uint16_t rrc_nr::get_mcc()
{
  return meas_cells.serving_cell().get_mcc();
}

uint16_t rrc_nr::get_mnc()
{
  return meas_cells.serving_cell().get_mnc();
}

// Senders
void rrc_nr::send_ul_info_transfer(unique_byte_buffer_t nas_msg)
{
  logger.debug("Preparing UL Info Transfer");

  ul_dcch_msg_s           ul_dcch_msg;
  ul_info_transfer_ies_s* ul_info_transfer =
      &ul_dcch_msg.msg.set_c1().set_ul_info_transfer().crit_exts.set_ul_info_transfer();

  // Try to resize target buffer first
  ul_info_transfer->ded_nas_msg.resize(nas_msg->N_bytes);

  // check we have enough space in target buffer
  if (nas_msg->N_bytes > ul_info_transfer->ded_nas_msg.size()) {
    logger.error("NAS message too big to send in UL Info transfer (%d > %d).",
                 nas_msg->N_bytes,
                 ul_info_transfer->ded_nas_msg.size());
    return;
  }

  // copy message content
  memcpy(ul_info_transfer->ded_nas_msg.data(), nas_msg->msg, nas_msg->N_bytes);

  // send message
  send_ul_dcch_msg(srb_to_lcid(nr_srb::srb1), ul_dcch_msg);
}

void rrc_nr::send_security_mode_complete()
{
  ul_dcch_msg_s ul_dcch_msg;
  auto&         smc = ul_dcch_msg.msg.set_c1().set_security_mode_complete().crit_exts.set_security_mode_complete();
  ul_dcch_msg.msg.c1().security_mode_complete().rrc_transaction_id = transaction_id;
  send_ul_dcch_msg(srb_to_lcid(nr_srb::srb1), ul_dcch_msg);
}

void rrc_nr::send_setup_request(srsran::nr_establishment_cause_t cause)
{
  logger.debug("Preparing RRC Setup Request");

  // Prepare SetupRequest packet
  ul_ccch_msg_s            ul_ccch_msg;
  rrc_setup_request_ies_s* rrc_setup_req = &ul_ccch_msg.msg.set_c1().set_rrc_setup_request().rrc_setup_request;

  // TODO: implement ng_minus5_g_s_tmsi_part1
  rrc_setup_req->ue_id.set_random_value();
  uint64_t random_id = srsran_random_uniform_int_dist(random_gen, 0, 12345);
  for (uint i = 0; i < 5; i++) { // fill random ID bytewise, 40 bits = 5 bytes
    random_id |= ((uint64_t)rand() & 0xFF) << i * 8;
  }
  rrc_setup_req->ue_id.random_value().from_number(random_id, rrc_setup_req->ue_id.random_value().length());
  rrc_setup_req->establishment_cause = (establishment_cause_opts::options)cause;

  send_ul_ccch_msg(ul_ccch_msg);
}

void rrc_nr::send_ul_ccch_msg(const asn1::rrc_nr::ul_ccch_msg_s& msg)
{
  unique_byte_buffer_t pdu = srsran::make_byte_buffer();
  if (pdu == nullptr) {
    logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return;
  }

  asn1::bit_ref bref(pdu->msg, pdu->get_tailroom());
  if (msg.pack(bref) != SRSASN_SUCCESS) {
    logger.error("Coulnd't pack UL-CCCH message.");
    return;
  }
  bref.align_bytes_zero();
  pdu->N_bytes = (uint32_t)bref.distance_bytes(pdu->msg);
  pdu->set_timestamp();

  // Set UE contention resolution ID in MAC
  uint64_t uecri      = 0;
  uint8_t* ue_cri_ptr = (uint8_t*)&uecri;
  uint32_t nbytes     = 6;
  for (uint32_t i = 0; i < nbytes; i++) {
    ue_cri_ptr[nbytes - i - 1] = pdu->msg[i];
  }

  logger.debug("Setting UE contention resolution ID: %" PRIu64 "", uecri);
  mac->set_contention_id(uecri);

  uint32_t lcid = 0;
  log_rrc_message(get_rb_name(lcid), Tx, pdu.get(), msg, msg.msg.c1().type().to_string());

  rlc->write_sdu(lcid, std::move(pdu));
}

void rrc_nr::send_ul_dcch_msg(uint32_t lcid, const ul_dcch_msg_s& msg)
{
  // Reset and reuse sdu buffer if provided
  unique_byte_buffer_t pdu = srsran::make_byte_buffer();
  if (pdu == nullptr) {
    logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return;
  }

  asn1::bit_ref bref(pdu->msg, pdu->get_tailroom());
  msg.pack(bref);
  bref.align_bytes_zero();
  pdu->N_bytes = (uint32_t)bref.distance_bytes(pdu->msg);
  pdu->set_timestamp();

  if (msg.msg.type() == ul_dcch_msg_type_c::types_opts::options::c1) {
    log_rrc_message(get_rb_name(lcid), Tx, pdu.get(), msg, msg.msg.c1().type().to_string());
  }

  pdcp->write_sdu(lcid, std::move(pdu));
}

void rrc_nr::send_con_setup_complete(srsran::unique_byte_buffer_t nas_msg)
{
  logger.debug("Preparing RRC Connection Setup Complete");

  // Prepare ConnectionSetupComplete packet
  asn1::rrc_nr::ul_dcch_msg_s ul_dcch_msg;
  rrc_setup_complete_ies_s*   rrc_setup_complete =
      &ul_dcch_msg.msg.set_c1().set_rrc_setup_complete().crit_exts.set_rrc_setup_complete();

  ul_dcch_msg.msg.c1().rrc_setup_complete().rrc_transaction_id = transaction_id;

  rrc_setup_complete->sel_plmn_id                      = 1;
  rrc_setup_complete->registered_amf_present           = false;
  rrc_setup_complete->guami_type_present               = false;
  rrc_setup_complete->ng_minus5_g_s_tmsi_value_present = false;

  rrc_setup_complete->ded_nas_msg.resize(nas_msg->N_bytes);
  memcpy(rrc_setup_complete->ded_nas_msg.data(), nas_msg->msg, nas_msg->N_bytes);

  send_ul_dcch_msg(srb_to_lcid(nr_srb::srb1), ul_dcch_msg);
}

void rrc_nr::send_rrc_reconfig_complete()
{
  logger.debug("Preparing RRC Connection Reconfig Complete");

  asn1::rrc_nr::ul_dcch_msg_s ul_dcch_msg;
  auto& rrc_reconfig_complete = ul_dcch_msg.msg.set_c1().set_rrc_recfg_complete().crit_exts.set_rrc_recfg_complete();
  ul_dcch_msg.msg.c1().rrc_recfg_complete().rrc_transaction_id = transaction_id;

  send_ul_dcch_msg(srb_to_lcid(nr_srb::srb1), ul_dcch_msg);
}

int rrc_nr::send_ue_capability_info(const asn1::rrc_nr::ue_cap_enquiry_s& msg)
{
  transaction_id = msg.rrc_transaction_id;

  ue_cap_enquiry_ies_s ue_cap_enquiry_ies = msg.crit_exts.ue_cap_enquiry();

  asn1::rrc_nr::ul_dcch_msg_s ul_dcch_msg;

  auto& ue_cap_info = ul_dcch_msg.msg.set_c1().set_ue_cap_info().crit_exts.set_ue_cap_info();
  ul_dcch_msg.msg.c1().ue_cap_info().rrc_transaction_id = msg.rrc_transaction_id;

  for (auto ue_cap_rat_request : ue_cap_enquiry_ies.ue_cap_rat_request_list) {
    if (ue_cap_rat_request.rat_type.value == rat_type_opts::nr) {
      ue_cap_info.ue_cap_rat_container_list_present = true;
      ue_cap_rat_container_s ue_cap_rat_container;
      ue_cap_rat_container.rat_type.value = rat_type_opts::nr;

      ue_nr_cap_s ue_cap;
      ue_cap.access_stratum_release = access_stratum_release_opts::rel15;

      // RLC params
      ue_cap.rlc_params_present                  = true;
      ue_cap.rlc_params.am_with_short_sn_present = true;
      ue_cap.rlc_params.um_with_short_sn_present = true;
      ue_cap.rlc_params.um_with_long_sn_present  = true;

      // PDCP parameters
      ue_cap.pdcp_params.supported_rohc_profiles.profile0x0000 = false;
      ue_cap.pdcp_params.supported_rohc_profiles.profile0x0001 = false;
      ue_cap.pdcp_params.supported_rohc_profiles.profile0x0002 = false;
      ue_cap.pdcp_params.supported_rohc_profiles.profile0x0003 = false;
      ue_cap.pdcp_params.supported_rohc_profiles.profile0x0004 = false;
      ue_cap.pdcp_params.supported_rohc_profiles.profile0x0006 = false;
      ue_cap.pdcp_params.supported_rohc_profiles.profile0x0101 = false;
      ue_cap.pdcp_params.supported_rohc_profiles.profile0x0102 = false;
      ue_cap.pdcp_params.supported_rohc_profiles.profile0x0103 = false;
      ue_cap.pdcp_params.supported_rohc_profiles.profile0x0104 = false;
      ue_cap.pdcp_params.max_num_rohc_context_sessions.value   = pdcp_params_s::max_num_rohc_context_sessions_opts::cs2;

      if (args.pdcp_short_sn_support) {
        ue_cap.pdcp_params.short_sn_present = true;
      }
      // PHY Parameters
      ue_cap.phy_params.phy_params_common_present = true;

      // RF Parameters
      for (const auto band : args.supported_bands_nr) {
        band_nr_s band_nr;
        band_nr.band_nr = band;
        ue_cap.rf_params.supported_band_list_nr.push_back(band_nr);

        // supportedBandCombinationList
        band_combination_s band_combination;
        band_params_c      band_params;
        band_params.set_nr().band_nr = band;
        band_combination.band_list.push_back(band_params);
        ue_cap.rf_params.supported_band_combination_list.push_back(band_combination);
      }
      // featureSets
      ue_cap.feature_sets_present = true;
      feature_set_dl_per_cc_s feature_set_dl_per_cc;
      feature_set_ul_per_cc_s feature_set_ul_per_cc;

      feature_set_dl_per_cc.supported_bw_dl.set_fr1().value = supported_bw_c::fr1_opts::mhz10;
      feature_set_ul_per_cc.supported_bw_ul.set_fr1().value = supported_bw_c::fr1_opts::mhz10;

      switch (args.scs) {
        case srsran_subcarrier_spacing_15kHz:
          feature_set_dl_per_cc.supported_subcarrier_spacing_dl = subcarrier_spacing_opts::khz15;
          feature_set_ul_per_cc.supported_subcarrier_spacing_ul = subcarrier_spacing_opts::khz15;
          break;
        case srsran_subcarrier_spacing_30kHz:
          feature_set_dl_per_cc.supported_subcarrier_spacing_dl = subcarrier_spacing_opts::khz30;
          feature_set_ul_per_cc.supported_subcarrier_spacing_ul = subcarrier_spacing_opts::khz30;
          break;
        default:
          logger.warning("Unsupported subcarrier spacing value");
      }

      ue_cap.feature_sets.feature_sets_dl_per_cc.push_back(feature_set_dl_per_cc);
      ue_cap.feature_sets.feature_sets_ul_per_cc.push_back(feature_set_ul_per_cc);

#if 1
      ue_cap_rat_container.ue_cap_rat_container.resize(512);
      asn1::bit_ref bref_pack(ue_cap_rat_container.ue_cap_rat_container.data(),
                              ue_cap_rat_container.ue_cap_rat_container.size());

      if (ue_cap.pack(bref_pack) != asn1::SRSASN_SUCCESS) {
        logger.error("Failed to pack UE NR Capabilities in UE Capability Info");
        return SRSRAN_ERROR;
      }
      ue_cap_rat_container.ue_cap_rat_container.resize(bref_pack.distance_bytes());
#else
      // hard-coded capabilities from third-party
      ue_cap_rat_container.ue_cap_rat_container.from_string("E1A01000074F5A03020000C0A0241262C001206A0609B00C39F30C7942"
                                                            "C0E098040623809506C4DD608D21A08107CA01165B262A87813E43"
                                                            "9F40CF88E3C639F30C7942C0E070F09C0013C0070004F0001601C00140"
                                                            "A836036B04690D04083E500892D931541439F11C78C73E618F2858"
                                                            "1C0E1E04FE0000003F80000000A00E05");
#endif

      ue_cap_info.ue_cap_rat_container_list.push_back(ue_cap_rat_container);
    }
  }
  send_ul_dcch_msg(srb_to_lcid(nr_srb::srb1), ul_dcch_msg);
  return SRSASN_SUCCESS;
}

// EUTRA-RRC interface
int rrc_nr::get_eutra_nr_capabilities(srsran::byte_buffer_t* eutra_nr_caps_pdu)
{
  struct ue_mrdc_cap_s mrdc_cap;

  band_combination_s band_combination;

  for (const auto& band : args.supported_bands_eutra) {
    struct band_params_c band_param_eutra;
    band_param_eutra.set_eutra();
    band_param_eutra.eutra().ca_bw_class_dl_eutra_present = true;
    band_param_eutra.eutra().ca_bw_class_ul_eutra_present = true;
    band_param_eutra.eutra().band_eutra                   = band;
    band_param_eutra.eutra().ca_bw_class_dl_eutra         = asn1::rrc_nr::ca_bw_class_eutra_opts::options::a;
    band_param_eutra.eutra().ca_bw_class_ul_eutra         = asn1::rrc_nr::ca_bw_class_eutra_opts::options::a;
    band_combination.band_list.push_back(band_param_eutra);
  }

  // TODO check if band is requested
  for (const auto& band : args.supported_bands_nr) {
    struct band_params_c band_param_nr;
    band_param_nr.set_nr();
    band_param_nr.nr().ca_bw_class_dl_nr_present = true;
    band_param_nr.nr().ca_bw_class_ul_nr_present = true;
    band_param_nr.nr().band_nr                   = band;
    band_param_nr.nr().ca_bw_class_dl_nr         = asn1::rrc_nr::ca_bw_class_nr_opts::options::a;
    band_param_nr.nr().ca_bw_class_ul_nr         = asn1::rrc_nr::ca_bw_class_nr_opts::options::a;
    band_combination.band_list.push_back(band_param_nr);
  }

  mrdc_cap.rf_params_mrdc.supported_band_combination_list.push_back(band_combination);

  mrdc_cap.rf_params_mrdc.ext = true;

  // RF Params MRDC applied_freq_band_list_filt
  for (const auto& band : args.supported_bands_eutra) {
    freq_band_info_c band_info_eutra;
    band_info_eutra.set_band_info_eutra();
    band_info_eutra.band_info_eutra().ca_bw_class_dl_eutra_present = false;
    band_info_eutra.band_info_eutra().ca_bw_class_ul_eutra_present = false;
    band_info_eutra.band_info_eutra().band_eutra                   = band;
    mrdc_cap.rf_params_mrdc.applied_freq_band_list_filt.push_back(band_info_eutra);
  }

  for (const auto& band : args.supported_bands_nr) {
    freq_band_info_c band_info_nr;
    band_info_nr.set_band_info_nr();
    band_info_nr.band_info_nr().band_nr = band;
    mrdc_cap.rf_params_mrdc.applied_freq_band_list_filt.push_back(band_info_nr);
  }

  // rf_params_mrdc supported band combination list v1540

  band_combination_list_v1540_l* band_combination_list_v1450 = new band_combination_list_v1540_l();
  band_combination_v1540_s       band_combination_v1540;

  band_params_v1540_s band_params_a;
  band_params_a.srs_tx_switch_present      = true;
  band_params_a.srs_carrier_switch_present = false;
  band_params_a.srs_tx_switch.supported_srs_tx_port_switch =
      band_params_v1540_s::srs_tx_switch_s_::supported_srs_tx_port_switch_opts::not_supported;
  band_combination_v1540.band_list_v1540.push_back(band_params_a);

  band_params_v1540_s band_params_b;
  band_params_b.srs_tx_switch_present = true;
  band_params_b.srs_tx_switch.supported_srs_tx_port_switch =
      band_params_v1540_s::srs_tx_switch_s_::supported_srs_tx_port_switch_opts::t1r2;
  band_params_b.srs_carrier_switch_present = false;
  band_combination_v1540.band_list_v1540.push_back(band_params_b);

  // clang-format off
  band_combination_v1540.ca_params_nr_v1540_present = false;
  band_combination_v1540.ca_params_nr_v1540.simul_csi_reports_all_cc_present = true;
  band_combination_v1540.ca_params_nr_v1540.csi_rs_im_reception_for_feedback_per_band_comb.max_num_simul_nzp_csi_rs_act_bwp_all_cc_present = true;
  band_combination_v1540.ca_params_nr_v1540.csi_rs_im_reception_for_feedback_per_band_comb.max_num_simul_nzp_csi_rs_act_bwp_all_cc = 5;
  band_combination_v1540.ca_params_nr_v1540.csi_rs_im_reception_for_feedback_per_band_comb.total_num_ports_simul_nzp_csi_rs_act_bwp_all_cc_present = true;
  band_combination_v1540.ca_params_nr_v1540.csi_rs_im_reception_for_feedback_per_band_comb.total_num_ports_simul_nzp_csi_rs_act_bwp_all_cc = 32;
  // clang-format on
  band_combination_list_v1450->push_back(band_combination_v1540);
  mrdc_cap.rf_params_mrdc.supported_band_combination_list_v1540.reset(band_combination_list_v1450);

  feature_set_combination_l feature_set_combination;

  feature_sets_per_band_l feature_sets_per_band;

  feature_set_c feature_set_eutra;
  feature_set_eutra.set_eutra();
  feature_set_eutra.eutra().dl_set_eutra = 1;
  feature_set_eutra.eutra().ul_set_eutra = 1;
  feature_sets_per_band.push_back(feature_set_eutra);
  feature_set_combination.push_back(feature_sets_per_band);

  for (const auto& band : args.supported_bands_nr) {
    feature_sets_per_band.resize(0);
    feature_set_c feature_set_nr;
    feature_set_nr.set_nr();
    feature_set_nr.nr().dl_set_nr = 1;
    feature_set_nr.nr().ul_set_nr = 1;
    feature_sets_per_band.push_back(feature_set_nr);
    feature_set_combination.push_back(feature_sets_per_band);
  }

  mrdc_cap.feature_set_combinations.push_back(feature_set_combination);

  // Pack mrdc_cap
  asn1::bit_ref bref(eutra_nr_caps_pdu->msg, eutra_nr_caps_pdu->get_tailroom());
  mrdc_cap.pack(bref);
  eutra_nr_caps_pdu->N_bytes = bref.distance_bytes();

#if 0
  uint8_t eutra_nr_cap_raw[] = {0x01, 0x1c, 0x04, 0x81, 0x60, 0x00, 0x1c, 0x4d, 0x00, 0x00, 0x00, 0x04,
                                0x00, 0x40, 0x04, 0x04, 0xd0, 0x10, 0x74, 0x06, 0x14, 0xe8, 0x1b, 0x10,
                                0x78, 0x00, 0x00, 0x20, 0x00, 0x10, 0x08, 0x08, 0x01, 0x00, 0x20};
  if (sizeof(eutra_nr_cap_raw) <= 2048) {
    memcpy(eutra_nr_caps_pdu->msg, eutra_nr_cap_raw, sizeof(eutra_nr_cap_raw));
    eutra_nr_caps_pdu->N_bytes = sizeof(eutra_nr_cap_raw);
  }
#endif

  logger.debug(
      eutra_nr_caps_pdu->msg, eutra_nr_caps_pdu->N_bytes, "EUTRA-NR capabilities (%u B)", eutra_nr_caps_pdu->N_bytes);

  return SRSRAN_SUCCESS;
}

bool rrc_nr::rrc_reconfiguration(bool endc_release_and_add_r15, const asn1::rrc_nr::rrc_recfg_s& rrc_nr_reconf)
{
  if (not conn_recfg_proc.launch(reconf_initiator_t::mcg_srb1, endc_release_and_add_r15, rrc_nr_reconf)) {
    logger.error("Unable to launch NR RRC reconfiguration procedure");
    return false;
  } else {
    callback_list.add_proc(conn_recfg_proc);
  }
  return true;
}

void rrc_nr::rrc_release()
{
  rlc->reset();
  pdcp->reset();
  mac->reset();
  lcid_drb.clear();

  // Apply actions only applicable in SA mode
  if (rrc_eutra == nullptr) {
    stack->reset_eps_bearers();
  }
}

int rrc_nr::get_nr_capabilities(srsran::byte_buffer_t* nr_caps_pdu)
{
  struct ue_nr_cap_s nr_cap;

  nr_cap.access_stratum_release = access_stratum_release_opts::rel15;
  // PDCP
  nr_cap.pdcp_params.max_num_rohc_context_sessions = pdcp_params_s::max_num_rohc_context_sessions_opts::cs2;

  for (const auto& band : args.supported_bands_nr) {
    band_nr_s band_nr;
    band_nr.band_nr                    = band;
    band_nr.ue_pwr_class_present       = true;
    band_nr.ue_pwr_class               = band_nr_s::ue_pwr_class_opts::pc3;
    band_nr.pusch_minus256_qam_present = true;
    nr_cap.rf_params.supported_band_list_nr.push_back(band_nr);
  }

  nr_cap.rlc_params_present                                       = true;
  nr_cap.rlc_params.um_with_short_sn_present                      = true;
  nr_cap.rlc_params.um_with_long_sn_present                       = true;
  nr_cap.pdcp_params.short_sn_present                             = args.pdcp_short_sn_support;
  nr_cap.phy_params.phy_params_fr1_present                        = true;
  nr_cap.phy_params.phy_params_fr1.pdsch_minus256_qam_fr1_present = true;

  // Pack nr_caps
  asn1::bit_ref bref(nr_caps_pdu->msg, nr_caps_pdu->get_tailroom());
  nr_cap.pack(bref);
  nr_caps_pdu->N_bytes = bref.distance_bytes();

#if 0
  uint8_t nr_cap_raw[] = {
      0xe1, 0x00, 0x00, 0x00, 0x01, 0x47, 0x7a, 0x03, 0x02, 0x00, 0x00, 0x01, 0x40, 0x48, 0x07, 0x06, 0x0e, 0x02, 0x0c,
      0x00, 0x02, 0x13, 0x60, 0x10, 0x73, 0xe4, 0x20, 0xf0, 0x00, 0x80, 0xc1, 0x30, 0x08, 0x0c, 0x00, 0x00, 0x0a, 0x05,
      0x89, 0xba, 0xc2, 0x19, 0x43, 0x40, 0x88, 0x10, 0x74, 0x18, 0x60, 0x4c, 0x04, 0x41, 0x6c, 0x90, 0x14, 0x06, 0x0c,
      0x78, 0xc7, 0x3e, 0x42, 0x0f, 0x00, 0x58, 0x0c, 0x0e, 0x0e, 0x02, 0x21, 0x3c, 0x84, 0xfc, 0x4d, 0xe0, 0x00, 0x12,
      0x00, 0x00, 0x00, 0x00, 0xe5, 0x4d, 0x00, 0x01, 0x00, 0x00, 0x04, 0x18, 0x60, 0x00, 0x34, 0xaa, 0x60};
  if (sizeof(nr_cap_raw) <= 2048) {
    memcpy(nr_caps_pdu->msg, nr_cap_raw, sizeof(nr_cap_raw));
    nr_caps_pdu->N_bytes = sizeof(nr_cap_raw);
  }
#endif

  logger.debug(nr_caps_pdu->msg, nr_caps_pdu->N_bytes, "NR capabilities (%u B)", nr_caps_pdu->N_bytes);
  return SRSRAN_SUCCESS;
};

void rrc_nr::phy_meas_stop()
{
  // possbile race condition for sim_measurement timer, which might be set at the same moment as stopped => fix
  // with phy integration
  logger.debug("Stopping simulated measurements");
  sim_measurement_timer.stop();
}

void rrc_nr::phy_set_cells_to_meas(uint32_t carrier_freq_r15)
{
  logger.debug("Measuring phy cell %d ", carrier_freq_r15);
  // Start timer for fake measurements
  auto timer_expire_func           = [this](uint32_t tid) { timer_expired(tid); };
  sim_measurement_carrier_freq_r15 = carrier_freq_r15;
  sim_measurement_timer.set(sim_measurement_timer_duration_ms, timer_expire_func);
  sim_measurement_timer.run();
}

bool rrc_nr::configure_sk_counter(uint16_t sk_counter)
{
  logger.info("Configure new SK counter %d. Update Key for secondary gnb", sk_counter);
  if (usim->generate_nr_context(sk_counter, &sec_cfg) == false) {
    return false;
  }
  security_is_activated = true;
  return true;
}

bool rrc_nr::is_config_pending()
{
  if (conn_recfg_proc.is_busy()) {
    return true;
  }
  return false;
}

bool rrc_nr::apply_rlc_add_mod(const rlc_bearer_cfg_s& rlc_bearer_cfg)
{
  uint32_t     lc_ch_id = 0;
  uint32_t     drb_id   = 0;
  uint32_t     srb_id   = 0;
  rlc_config_t rlc_cfg;
  // We set this to true if below we detect it's a DRB
  bool is_drb = false;

  lc_ch_id = rlc_bearer_cfg.lc_ch_id;
  if (rlc_bearer_cfg.served_radio_bearer_present == true) {
    if (rlc_bearer_cfg.served_radio_bearer.type() == rlc_bearer_cfg_s::served_radio_bearer_c_::types::drb_id) {
      drb_id = rlc_bearer_cfg.served_radio_bearer.drb_id();
      add_lcid_drb(lc_ch_id, drb_id);
      is_drb = true;
    } else if (rlc_bearer_cfg.served_radio_bearer.type() == rlc_bearer_cfg_s::served_radio_bearer_c_::types::srb_id) {
      srb_id = rlc_bearer_cfg.served_radio_bearer.srb_id();
    }
  } else {
    logger.error("In RLC bearer cfg does not contain served radio bearer");
    return false;
  }

  if (rlc_bearer_cfg.rlc_cfg_present == true) {
    uint8_t bearer_id = static_cast<uint8_t>(is_drb ? drb_id : srb_id);
    if (srsran::make_rlc_config_t(rlc_bearer_cfg.rlc_cfg, bearer_id, &rlc_cfg) != SRSRAN_SUCCESS) {
      logger.error("Failed to build RLC config");
      return false;
    }
  } else if (not is_drb) {
    logger.debug("Using default RLC configs for SRB%d", srb_id);
    rlc_cfg = rlc_config_t::default_rlc_am_nr_config();
  } else {
    logger.error("In RLC bearer cfg does not contain rlc cfg");
    return false;
  }

  // Setup RLC
  rlc->add_bearer(lc_ch_id, rlc_cfg);

  if (rlc_bearer_cfg.mac_lc_ch_cfg_present == true && rlc_bearer_cfg.mac_lc_ch_cfg.ul_specific_params_present) {
    logical_channel_config_t logical_channel_cfg;
    logical_channel_cfg = srsran::make_mac_logical_channel_cfg_t(lc_ch_id, rlc_bearer_cfg.mac_lc_ch_cfg);
    mac->setup_lcid(logical_channel_cfg);
  } else {
    logger.error("Bearer config for LCID %d does not contain mac-LogicalChannelConfig.", lc_ch_id);
    return false;
  }
  return true;
}

bool rrc_nr::apply_mac_cell_group(const mac_cell_group_cfg_s& mac_cell_group_cfg)
{
  if (mac_cell_group_cfg.sched_request_cfg_present) {
    if (mac_cell_group_cfg.sched_request_cfg.sched_request_to_add_mod_list.size() > 0) {
      if (mac_cell_group_cfg.sched_request_cfg.sched_request_to_add_mod_list.size() == 1) {
        const sched_request_to_add_mod_s& asn1_cfg =
            mac_cell_group_cfg.sched_request_cfg.sched_request_to_add_mod_list[0];
        sr_cfg_nr_t sr_cfg              = {};
        sr_cfg.enabled                  = true;
        sr_cfg.num_items                = 1;
        sr_cfg.item[0].sched_request_id = asn1_cfg.sched_request_id;
        sr_cfg.item[0].trans_max        = asn1_cfg.sr_trans_max.to_number();
        if (asn1_cfg.sr_prohibit_timer_present) {
          sr_cfg.item[0].prohibit_timer = asn1_cfg.sr_trans_max;
        }
        if (mac->set_config(sr_cfg) != SRSRAN_SUCCESS) {
          logger.error("Couldn't configure SR procedure.");
          return false;
        }
      } else {
        logger.warning("Only handling 1 scheduling request index to add");
        return false;
      }
    }

    if (mac_cell_group_cfg.sched_request_cfg.sched_request_to_release_list.size() > 0) {
      logger.warning("Not handling sched request to release list");
      return false;
    }
  }
  if (mac_cell_group_cfg.sched_request_cfg_present)

    if (mac_cell_group_cfg.bsr_cfg_present) {
      logger.debug("Handling MAC BSR config");
      srsran::bsr_cfg_nr_t bsr_cfg = {};
      bsr_cfg.periodic_timer       = mac_cell_group_cfg.bsr_cfg.periodic_bsr_timer.to_number();
      bsr_cfg.retx_timer           = mac_cell_group_cfg.bsr_cfg.retx_bsr_timer.to_number();
      if (mac->set_config(bsr_cfg) != SRSRAN_SUCCESS) {
        return false;
      }
    }

  if (mac_cell_group_cfg.tag_cfg_present) {
    if (mac_cell_group_cfg.tag_cfg.tag_to_add_mod_list.size() > 0) {
      for (uint32_t i = 0; i < mac_cell_group_cfg.tag_cfg.tag_to_add_mod_list.size(); i++) {
        tag_cfg_nr_t tag_cfg_nr     = {};
        tag_cfg_nr.tag_id           = mac_cell_group_cfg.tag_cfg.tag_to_add_mod_list[i].tag_id;
        tag_cfg_nr.time_align_timer = mac_cell_group_cfg.tag_cfg.tag_to_add_mod_list[i].time_align_timer.to_number();
        if (mac->add_tag_config(tag_cfg_nr) != SRSRAN_SUCCESS) {
          logger.warning("Unable to add TAG config with tag_id %d", tag_cfg_nr.tag_id);
          return false;
        }
      }
    }
    if (mac_cell_group_cfg.tag_cfg.tag_to_release_list.size() > 0) {
      for (uint32_t i = 0; i < mac_cell_group_cfg.tag_cfg.tag_to_release_list.size(); i++) {
        uint32_t tag_id = mac_cell_group_cfg.tag_cfg.tag_to_release_list[i];
        if (mac->remove_tag_config(tag_id) != SRSRAN_SUCCESS) {
          logger.warning("Unable to release TAG config with tag_id %d", tag_id);
          return false;
        }
      }
    }
  }

  if (mac_cell_group_cfg.phr_cfg_present) {
    if (mac_cell_group_cfg.phr_cfg.is_setup()) {
      phr_cfg_nr_t phr_cfg_nr;
      if (make_mac_phr_cfg_t(mac_cell_group_cfg.phr_cfg.setup(), &phr_cfg_nr) != true) {
        logger.warning("Unable to build PHR config");
        return false;
      }
      if (mac->set_config(phr_cfg_nr) != SRSRAN_SUCCESS) {
        logger.warning("Unable to set PHR config");
        return false;
      }
    }
  }

  if (mac_cell_group_cfg.skip_ul_tx_dynamic) {
    logger.warning("Not handling phr cfg in skip_ul_tx_dynamic cell group config");
  }
  return true;
}

bool rrc_nr::apply_sp_cell_init_dl_pdcch(const asn1::rrc_nr::pdcch_cfg_s& pdcch_cfg)
{
  if (pdcch_cfg.search_spaces_to_add_mod_list.size() > 0) {
    for (uint32_t i = 0; i < pdcch_cfg.search_spaces_to_add_mod_list.size(); i++) {
      srsran_search_space_t search_space;
      if (make_phy_search_space_cfg(pdcch_cfg.search_spaces_to_add_mod_list[i], &search_space) == true) {
        phy_cfg.pdcch.search_space[search_space.id]         = search_space;
        phy_cfg.pdcch.search_space_present[search_space.id] = true;
      } else {
        logger.warning("Warning while building search_space structure id=%d", i);
        return false;
      }
    }
  } else {
    logger.warning("Option search_spaces_to_add_mod_list not present");
    return false;
  }
  if (pdcch_cfg.ctrl_res_set_to_add_mod_list.size() > 0) {
    for (uint32_t i = 0; i < pdcch_cfg.ctrl_res_set_to_add_mod_list.size(); i++) {
      srsran_coreset_t coreset;
      if (make_phy_coreset_cfg(pdcch_cfg.ctrl_res_set_to_add_mod_list[i], &coreset) == true) {
        phy_cfg.pdcch.coreset[coreset.id]         = coreset;
        phy_cfg.pdcch.coreset_present[coreset.id] = true;
      } else {
        logger.warning("Warning while building coreset structure");
        return false;
      }
    }
  } else {
    logger.warning("Option ctrl_res_set_to_add_mod_list not present");
  }
  return true;
}

bool rrc_nr::apply_sp_cell_init_dl_pdsch(const asn1::rrc_nr::pdsch_cfg_s& pdsch_cfg)
{
  if (pdsch_cfg.mcs_table_present) {
    switch (pdsch_cfg.mcs_table) {
      case pdsch_cfg_s::mcs_table_opts::qam256:
        phy_cfg.pdsch.mcs_table = srsran_mcs_table_256qam;
        break;
      case pdsch_cfg_s::mcs_table_opts::qam64_low_se:
        phy_cfg.pdsch.mcs_table = srsran_mcs_table_qam64LowSE;
        break;
      case pdsch_cfg_s::mcs_table_opts::nulltype:
        logger.warning("Warning while selecting pdsch mcs_table");
        return false;
    }
  } else {
    // If the field is absent the UE applies the value 64QAM.
    phy_cfg.pdsch.mcs_table = srsran_mcs_table_64qam;
  }

  if (pdsch_cfg.dmrs_dl_for_pdsch_map_type_a_present) {
    if (pdsch_cfg.dmrs_dl_for_pdsch_map_type_a.type() == setup_release_c<dmrs_dl_cfg_s>::types_opts::setup) {
      // See TS 38.331, DMRS-DownlinkConfig. Also, see TS 38.214, 5.1.6.2 - DM-RS reception procedure.
      phy_cfg.pdsch.dmrs_typeA.additional_pos = srsran_dmrs_sch_add_pos_2;
      phy_cfg.pdsch.dmrs_typeA.present        = true;
    } else {
      logger.warning("Option dmrs_dl_for_pdsch_map_type_a not of type setup");
      return false;
    }
  } else {
    logger.warning("Option dmrs_dl_for_pdsch_map_type_a not present");
    return false;
  }

  srsran_resource_alloc_t resource_alloc;
  if (make_phy_pdsch_alloc_type(pdsch_cfg, &resource_alloc) == true) {
    phy_cfg.pdsch.alloc = resource_alloc;
  }

  if (pdsch_cfg.zp_csi_rs_res_to_add_mod_list.size() > 0) {
    for (uint32_t i = 0; i < pdsch_cfg.zp_csi_rs_res_to_add_mod_list.size(); i++) {
      srsran_csi_rs_zp_resource_t zp_csi_rs_resource;
      if (make_phy_zp_csi_rs_resource(pdsch_cfg.zp_csi_rs_res_to_add_mod_list[i], &zp_csi_rs_resource) == true) {
        // temporally store csi_rs_zp_res
        csi_rs_zp_res[zp_csi_rs_resource.id] = zp_csi_rs_resource;
      } else {
        logger.warning("Warning while building zp_csi_rs resource");
        return false;
      }
    }
  }

  if (pdsch_cfg.p_zp_csi_rs_res_set_present) {
    // check if resources have been processed
    if (pdsch_cfg.zp_csi_rs_res_to_add_mod_list.size() == 0) {
      logger.warning("Can't build ZP-CSI config, option zp_csi_rs_res_to_add_mod_list not present");
      return false;
    }
    if (pdsch_cfg.p_zp_csi_rs_res_set.type() == setup_release_c<zp_csi_rs_res_set_s>::types_opts::setup) {
      for (uint32_t i = 0; i < pdsch_cfg.p_zp_csi_rs_res_set.setup().zp_csi_rs_res_id_list.size(); i++) {
        uint8_t res = pdsch_cfg.p_zp_csi_rs_res_set.setup().zp_csi_rs_res_id_list[i];
        // use temporally stored values to assign
        if (csi_rs_zp_res.find(res) == csi_rs_zp_res.end()) {
          logger.warning("Can not find p_zp_csi_rs_res in temporally stored csi_rs_zp_res");
          return false;
        }
        phy_cfg.pdsch.p_zp_csi_rs_set.data[i] = csi_rs_zp_res[res];
        phy_cfg.pdsch.p_zp_csi_rs_set.count += 1;
      }
    } else {
      logger.warning("Option p_zp_csi_rs_res_set not of type setup");
      return false;
    }
  }
  return true;
}

bool rrc_nr::apply_res_csi_report_cfg(const asn1::rrc_nr::csi_report_cfg_s& csi_report_cfg)
{
  uint32_t                   report_cfg_id = csi_report_cfg.report_cfg_id;
  srsran_csi_hl_report_cfg_t srsran_csi_hl_report_cfg;
  if (make_phy_csi_report(csi_report_cfg, &srsran_csi_hl_report_cfg) == true) {
    phy_cfg.csi.reports[report_cfg_id] = srsran_csi_hl_report_cfg;
  } else {
    logger.warning("Warning while building report structure");
    return false;
  }
  if (csi_report_cfg.report_cfg_type.type() == csi_report_cfg_s::report_cfg_type_c_::types_opts::options::periodic) {
    if (csi_report_cfg.report_cfg_type.periodic().pucch_csi_res_list.size() > 0) {
      uint32_t res_id = csi_report_cfg.report_cfg_type.periodic()
                            .pucch_csi_res_list[0]
                            .pucch_res; // TODO: support and check more items
      if (pucch_res_list.contains(res_id)) {
        phy_cfg.csi.reports[report_cfg_id].periodic.resource = pucch_res_list[res_id];
      } else {
        logger.error("Resources set not present for assigning pucch sets (res_id %d)", res_id);
        return false;
      }
    } else {
      logger.warning("List size to small: pucch_csi_res_list.size() < 0");
      return false;
    }
  }
  return true;
}

bool rrc_nr::apply_csi_meas_cfg(const asn1::rrc_nr::csi_meas_cfg_s& csi_meas_cfg)
{
  for (uint32_t i = 0; i < csi_meas_cfg.csi_report_cfg_to_add_mod_list.size(); i++) {
    if (apply_res_csi_report_cfg(csi_meas_cfg.csi_report_cfg_to_add_mod_list[i]) == false) {
      return false;
    }
  }

  for (uint32_t i = 0; i < csi_meas_cfg.nzp_csi_rs_res_to_add_mod_list.size(); i++) {
    srsran_csi_rs_nzp_resource_t csi_rs_nzp_resource;
    if (make_phy_nzp_csi_rs_resource(csi_meas_cfg.nzp_csi_rs_res_to_add_mod_list[i], &csi_rs_nzp_resource) == true) {
      // temporally store csi_rs_zp_res
      csi_rs_nzp_res[csi_rs_nzp_resource.id] = csi_rs_nzp_resource;
    } else {
      logger.warning("Warning while building phy_nzp_csi_rs resource");
      return false;
    }
  }

  for (uint32_t i = 0; i < csi_meas_cfg.nzp_csi_rs_res_set_to_add_mod_list.size(); i++) {
    uint8_t set_id = csi_meas_cfg.nzp_csi_rs_res_set_to_add_mod_list[i].nzp_csi_res_set_id;
    for (uint32_t j = 0; j < csi_meas_cfg.nzp_csi_rs_res_set_to_add_mod_list[i].nzp_csi_rs_res.size(); j++) {
      uint8_t res = csi_meas_cfg.nzp_csi_rs_res_set_to_add_mod_list[i].nzp_csi_rs_res[j];
      // use temporally stored values to assign
      if (csi_rs_nzp_res.find(res) == csi_rs_nzp_res.end()) {
        logger.warning("Can not find nzp_csi_rs_res in temporally stored csi_rs_nzp_res");
        return false;
      }
      phy_cfg.pdsch.nzp_csi_rs_sets[set_id].data[j] = csi_rs_nzp_res[res];
      phy_cfg.pdsch.nzp_csi_rs_sets[set_id].count += 1;
    }
    if (csi_meas_cfg.nzp_csi_rs_res_set_to_add_mod_list[i].trs_info_present) {
      phy_cfg.pdsch.nzp_csi_rs_sets[set_id].trs_info = true;
    }
  }

  return true;
}

bool rrc_nr::apply_dl_common_cfg(const asn1::rrc_nr::dl_cfg_common_s& dl_cfg_common)
{
  if (dl_cfg_common.freq_info_dl_present) {
    if (make_phy_carrier_cfg(dl_cfg_common.freq_info_dl, &phy_cfg.carrier) == false) {
      logger.warning("Warning while making carrier phy config");
      return false;
    }
  } else {
    logger.warning("Option freq_info_dl not present, S-UL not supported.");
    return false;
  }
  if (dl_cfg_common.init_dl_bwp_present) {
    if (dl_cfg_common.init_dl_bwp.pdsch_cfg_common_present) {
      if (dl_cfg_common.init_dl_bwp.pdsch_cfg_common.is_setup()) {
        const pdcch_cfg_common_s& pdcch_cfg_common = dl_cfg_common.init_dl_bwp.pdcch_cfg_common.setup();

        // Load CORESET Zero
        if (pdcch_cfg_common.ctrl_res_set_zero_present) {
          // Get pointA and SSB absolute frequencies
          double pointA_abs_freq_Hz =
              phy_cfg.carrier.dl_center_frequency_hz -
              phy_cfg.carrier.nof_prb * SRSRAN_NRE * SRSRAN_SUBC_SPACING_NR(phy_cfg.carrier.scs) / 2;
          double ssb_abs_freq_Hz = phy_cfg.carrier.ssb_center_freq_hz;

          // Calculate integer SSB to pointA frequency offset in Hz
          uint32_t ssb_pointA_freq_offset_Hz =
              (ssb_abs_freq_Hz > pointA_abs_freq_Hz) ? (uint32_t)(ssb_abs_freq_Hz - pointA_abs_freq_Hz) : 0;
          srsran_subcarrier_spacing_t ssb_scs = phy_cfg.ssb.scs;

          // Select PDCCH subcarrrier spacing from PDCCH BWP
          srsran_subcarrier_spacing_t pdcch_scs = phy_cfg.carrier.scs;

          // Make CORESET Zero from provided field and given subcarrier spacing
          srsran_coreset_t coreset0 = {};
          if (srsran_coreset_zero(phy_cfg.carrier.pci,
                                  ssb_pointA_freq_offset_Hz,
                                  ssb_scs,
                                  pdcch_scs,
                                  pdcch_cfg_common.ctrl_res_set_zero,
                                  &coreset0) < SRSASN_SUCCESS) {
            logger.warning("Not possible to create CORESET Zero (ssb_scs=%s, pdcch_scs=%s, idx=%d)",
                           srsran_subcarrier_spacing_to_str(ssb_scs),
                           srsran_subcarrier_spacing_to_str(pdcch_scs),
                           pdcch_cfg_common.ctrl_res_set_zero);
            return false;
          }

          // Write CORESET Zero in index 0
          phy_cfg.pdcch.coreset[0]         = coreset0;
          phy_cfg.pdcch.coreset_present[0] = true;
        }

        if (pdcch_cfg_common.common_ctrl_res_set_present) {
          srsran_coreset_t coreset;
          if (make_phy_coreset_cfg(pdcch_cfg_common.common_ctrl_res_set, &coreset) == true) {
            phy_cfg.pdcch.coreset[coreset.id]         = coreset;
            phy_cfg.pdcch.coreset_present[coreset.id] = true;
          } else {
            logger.warning("Warning while building coreset structure");
            return false;
          }
        } else {
          logger.warning("Option common_ctrl_res_set not present");
          return false;
        }
        if (pdcch_cfg_common.common_search_space_list.size() > 0) {
          for (uint32_t i = 0; i < pdcch_cfg_common.common_search_space_list.size(); i++) {
            srsran_search_space_t search_space;
            if (make_phy_search_space_cfg(pdcch_cfg_common.common_search_space_list[i], &search_space) == true) {
              phy_cfg.pdcch.search_space[search_space.id]         = search_space;
              phy_cfg.pdcch.search_space_present[search_space.id] = true;
            } else {
              logger.warning("Warning while building search_space structure for common search space");
              return false;
            }
          }
        } else {
          logger.warning("Option common_search_space_list not present");
          return false;
        }
        if (pdcch_cfg_common.ra_search_space_present) {
          if (phy_cfg.pdcch.search_space_present[pdcch_cfg_common.ra_search_space] == true) {
            phy_cfg.pdcch.ra_search_space         = phy_cfg.pdcch.search_space[pdcch_cfg_common.ra_search_space];
            phy_cfg.pdcch.ra_search_space_present = true;
            phy_cfg.pdcch.ra_search_space.type    = srsran_search_space_type_common_1;
          } else {
            logger.warning("Search space %d not presenet for random access search space",
                           pdcch_cfg_common.ra_search_space);
          }
        } else {
          logger.warning("Option ra_search_space not present");
          return false;
        }
      } else {
        logger.warning("Option pdsch_cfg_common not of type setup");
        return false;
      }
    } else {
      logger.warning("Option pdsch_cfg_common not present");
      return false;
    }
    if (dl_cfg_common.init_dl_bwp.pdsch_cfg_common_present) {
      if (dl_cfg_common.init_dl_bwp.pdsch_cfg_common.type() == setup_release_c<pdsch_cfg_common_s>::types::setup) {
        pdsch_cfg_common_s pdsch_cfg_common = dl_cfg_common.init_dl_bwp.pdsch_cfg_common.setup();
        if (pdsch_cfg_common.pdsch_time_domain_alloc_list.size() > 0) {
          for (uint32_t i = 0; i < pdsch_cfg_common.pdsch_time_domain_alloc_list.size(); i++) {
            srsran_sch_time_ra_t common_time_ra;
            if (make_phy_common_time_ra(pdsch_cfg_common.pdsch_time_domain_alloc_list[i], &common_time_ra) == true) {
              phy_cfg.pdsch.common_time_ra[i]  = common_time_ra;
              phy_cfg.pdsch.nof_common_time_ra = i + 1;
            } else {
              logger.warning("Warning while building common_time_ra structure");
              return false;
            }
          }
        } else {
          logger.warning("Option pdsch_time_domain_alloc_list not present");
          return false;
        }
      } else {
        logger.warning("Option pdsch_cfg_common not of type setup");
        return false;
      }
    } else {
      logger.warning("Option pdsch_cfg_common not present");
      return false;
    }
  } else {
    logger.warning("Option init_dl_bwp not present");
    return false;
  }
  return true;
}

bool rrc_nr::apply_ul_common_cfg(const asn1::rrc_nr::ul_cfg_common_s& ul_cfg_common)
{
  srsran::srsran_band_helper bands;

  if (ul_cfg_common.freq_info_ul_present && ul_cfg_common.freq_info_ul.absolute_freq_point_a_present) {
    // Update UL frequency point if provided
    phy_cfg.carrier.ul_center_frequency_hz = bands.get_center_freq_from_abs_freq_point_a(
        phy_cfg.carrier.nof_prb, ul_cfg_common.freq_info_ul.absolute_freq_point_a);
  }
  if (ul_cfg_common.init_ul_bwp_present) {
    if (ul_cfg_common.init_ul_bwp.rach_cfg_common_present) {
      if (ul_cfg_common.init_ul_bwp.rach_cfg_common.type() == setup_release_c<rach_cfg_common_s>::types_opts::setup) {
        rach_cfg_nr_t rach_cfg_nr = {};
        make_mac_rach_cfg(ul_cfg_common.init_ul_bwp.rach_cfg_common.setup(), &rach_cfg_nr);
        mac->set_config(rach_cfg_nr);

        // Make the RACH configuration for PHY
        if (not make_phy_rach_cfg(
                ul_cfg_common.init_ul_bwp.rach_cfg_common.setup(), SRSRAN_DUPLEX_MODE_FDD, &phy_cfg.prach)) {
          logger.warning("Error parsing rach_cfg_common");
          return false;
        }

      } else {
        logger.warning("Option rach_cfg_common not of type setup");
        return false;
      }
    } else {
      logger.warning("Option rach_cfg_common not present");
      return false;
    }
    if (ul_cfg_common.init_ul_bwp.pusch_cfg_common_present) {
      if (ul_cfg_common.init_ul_bwp.pusch_cfg_common.type() == setup_release_c<pusch_cfg_common_s>::types_opts::setup) {
        if (ul_cfg_common.init_ul_bwp.pusch_cfg_common.setup().pusch_time_domain_alloc_list.size() > 0) {
          for (uint32_t i = 0;
               i < ul_cfg_common.init_ul_bwp.pusch_cfg_common.setup().pusch_time_domain_alloc_list.size();
               i++) {
            srsran_sch_time_ra_t common_time_ra;
            if (make_phy_common_time_ra(
                    ul_cfg_common.init_ul_bwp.pusch_cfg_common.setup().pusch_time_domain_alloc_list[i],
                    &common_time_ra) == true) {
              phy_cfg.pusch.common_time_ra[i]  = common_time_ra;
              phy_cfg.pusch.nof_common_time_ra = i + 1;
            } else {
              logger.warning("Warning while building common_time_ra structure");
            }
          }
        } else {
          logger.warning("Option pusch_time_domain_alloc_list not present");
          return false;
        }
      } else {
        logger.warning("Option pusch_cfg_common not of type setup");
        return false;
      }
    } else {
      logger.warning("Option pusch_cfg_common not present");
      return false;
    }
    if (ul_cfg_common.init_ul_bwp.pucch_cfg_common_present) {
      if (ul_cfg_common.init_ul_bwp.pucch_cfg_common.type() == setup_release_c<pucch_cfg_common_s>::types_opts::setup) {
        logger.info("PUCCH cfg common setup not handled");
      } else {
        logger.warning("Option pucch_cfg_common not of type setup");
        return false;
      }
    } else {
      logger.warning("Option pucch_cfg_common not present");
      return false;
    }
  } else {
    logger.warning("Option init_ul_bwp in spCellConfigCommon not present");
    return false;
  }
  return true;
}

bool rrc_nr::apply_sp_cell_ded_ul_pucch(const asn1::rrc_nr::pucch_cfg_s& pucch_cfg)
{
  // determine format 2 max code rate
  uint32_t format_2_max_code_rate = 0;
  if (pucch_cfg.format2_present && pucch_cfg.format2.type() == setup_release_c<pucch_format_cfg_s>::types::setup) {
    if (pucch_cfg.format2.setup().max_code_rate_present) {
      if (make_phy_max_code_rate(pucch_cfg.format2.setup(), &format_2_max_code_rate) == false) {
        logger.warning("Warning while building format_2_max_code_rate");
      }
    }
  } else {
    logger.warning("Option format2 not present or not of type setup");
    return false;
  }

  // now look up resource and assign into internal struct
  if (pucch_cfg.res_to_add_mod_list.size() > 0) {
    for (uint32_t i = 0; i < pucch_cfg.res_to_add_mod_list.size(); i++) {
      uint32_t res_id = pucch_cfg.res_to_add_mod_list[i].pucch_res_id;
      pucch_res_list.insert(res_id, {});
      if (!make_phy_res_config(pucch_cfg.res_to_add_mod_list[i], format_2_max_code_rate, &pucch_res_list[res_id])) {
        logger.warning("Warning while building pucch_nr_resource structure");
        return false;
      }
    }
  } else {
    logger.warning("Option res_to_add_mod_list not present");
    return false;
  }

  // Check first all resource lists and
  phy_cfg.pucch.enabled = true;
  if (pucch_cfg.res_set_to_add_mod_list.size() > 0) {
    for (uint32_t i = 0; i < pucch_cfg.res_set_to_add_mod_list.size(); i++) {
      uint32_t set_id                          = pucch_cfg.res_set_to_add_mod_list[i].pucch_res_set_id;
      phy_cfg.pucch.sets[set_id].nof_resources = pucch_cfg.res_set_to_add_mod_list[i].res_list.size();
      for (uint32_t j = 0; j < pucch_cfg.res_set_to_add_mod_list[i].res_list.size(); j++) {
        uint32_t res_id = pucch_cfg.res_set_to_add_mod_list[i].res_list[j];
        if (pucch_res_list.contains(res_id)) {
          phy_cfg.pucch.sets[set_id].resources[j] = pucch_res_list[res_id];
        } else {
          logger.error(
              "Resources set not present for assign pucch sets (res_id %d, setid %d, j %d)", res_id, set_id, j);
        }
      }
    }
  }

  if (pucch_cfg.sched_request_res_to_add_mod_list.size() > 0) {
    for (uint32_t i = 0; i < pucch_cfg.sched_request_res_to_add_mod_list.size(); i++) {
      uint32_t                      sr_res_id = pucch_cfg.sched_request_res_to_add_mod_list[i].sched_request_res_id;
      srsran_pucch_nr_sr_resource_t srsran_pucch_nr_sr_resource;
      if (make_phy_sr_resource(pucch_cfg.sched_request_res_to_add_mod_list[i], &srsran_pucch_nr_sr_resource) ==
          true) { // TODO: fix that if indexing is solved
        phy_cfg.pucch.sr_resources[sr_res_id] = srsran_pucch_nr_sr_resource;

        // Set PUCCH resource
        if (pucch_cfg.sched_request_res_to_add_mod_list[i].res_present) {
          uint32_t pucch_res_id = pucch_cfg.sched_request_res_to_add_mod_list[i].res;
          if (pucch_res_list.contains(pucch_res_id)) {
            phy_cfg.pucch.sr_resources[sr_res_id].resource = pucch_res_list[pucch_res_id];
          } else {
            logger.warning("Warning SR (%d) PUCCH resource is invalid (%d)", sr_res_id, pucch_res_id);
            phy_cfg.pucch.sr_resources[sr_res_id].configured = false;
            return false;
          }
        } else {
          logger.warning("Warning SR resource is present but no PUCCH resource is assigned to it");
          phy_cfg.pucch.sr_resources[sr_res_id].configured = false;
          return false;
        }

      } else {
        logger.warning("Warning while building srsran_pucch_nr_sr_resource structure");
        return false;
      }
    }
  } else {
    logger.warning("Option sched_request_res_to_add_mod_list not present");
    return false;
  }

  if (pucch_cfg.dl_data_to_ul_ack.size() > 0) {
    for (uint32_t i = 0; i < pucch_cfg.dl_data_to_ul_ack.size(); i++) {
      phy_cfg.harq_ack.dl_data_to_ul_ack[i] = pucch_cfg.dl_data_to_ul_ack[i];
    }
    phy_cfg.harq_ack.nof_dl_data_to_ul_ack = pucch_cfg.dl_data_to_ul_ack.size();
  } else {
    logger.warning("Option dl_data_to_ul_ack not present");
    return false;
  }

  return true;
};

bool rrc_nr::apply_sp_cell_ded_ul_pusch(const asn1::rrc_nr::pusch_cfg_s& pusch_cfg)
{
  if (pusch_cfg.mcs_table_present) {
    switch (pusch_cfg.mcs_table) {
      case pusch_cfg_s::mcs_table_opts::qam256:
        phy_cfg.pusch.mcs_table = srsran_mcs_table_256qam;
        break;
      case pusch_cfg_s::mcs_table_opts::qam64_low_se:
        phy_cfg.pusch.mcs_table = srsran_mcs_table_qam64LowSE;
        break;
      case pusch_cfg_s::mcs_table_opts::nulltype:
        logger.warning("Warning while selecting pusch mcs_table");
        return false;
    }
  } else {
    // If the field is absent the UE applies the value 64QAM.
    phy_cfg.pusch.mcs_table = srsran_mcs_table_64qam;
  }

  srsran_resource_alloc_t resource_alloc;
  if (make_phy_pusch_alloc_type(pusch_cfg, &resource_alloc) == true) {
    phy_cfg.pusch.alloc = resource_alloc;
  }

  if (pusch_cfg.dmrs_ul_for_pusch_map_type_a_present) {
    if (pusch_cfg.dmrs_ul_for_pusch_map_type_a.type() == setup_release_c<dmrs_ul_cfg_s>::types_opts::setup) {
      // // See TS 38.331, DMRS-UplinkConfig. Also, see TS 38.214, 6.2.2 - UE DM-RS transmission procedure.
      phy_cfg.pusch.dmrs_typeA.additional_pos = srsran_dmrs_sch_add_pos_2;
      phy_cfg.pusch.dmrs_typeA.present        = true;
    } else {
      logger.warning("Option dmrs_ul_for_pusch_map_type_a not of type setup");
      return false;
    }
  } else {
    logger.warning("Option dmrs_ul_for_pusch_map_type_a not present");
    return false;
  }
  if (pusch_cfg.uci_on_pusch_present) {
    if (pusch_cfg.uci_on_pusch.type() == setup_release_c<uci_on_pusch_s>::types_opts::setup) {
      if (pusch_cfg.uci_on_pusch.setup().beta_offsets_present) {
        if (pusch_cfg.uci_on_pusch.setup().beta_offsets.type() ==
            uci_on_pusch_s::beta_offsets_c_::types_opts::semi_static) {
          srsran_beta_offsets_t beta_offsets;
          if (make_phy_beta_offsets(pusch_cfg.uci_on_pusch.setup().beta_offsets.semi_static(), &beta_offsets) == true) {
            phy_cfg.pusch.beta_offsets = beta_offsets;
          } else {
            logger.warning("Warning while building beta_offsets structure");
            return false;
          }
        } else {
          logger.warning("Option beta_offsets not of type semi_static");
          return false;
        }
        if (pusch_cfg.uci_on_pusch_present) {
          if (make_phy_pusch_scaling(pusch_cfg.uci_on_pusch.setup(), &phy_cfg.pusch.scaling) == false) {
            logger.warning("Warning while building scaling structure");
            return false;
          }
        }
      } else {
        logger.warning("Option beta_offsets not present");
        return false;
      }
    } else {
      logger.warning("Option uci_on_pusch of type setup");
      return false;
    }
  } else {
    logger.warning("Option uci_on_pusch not present");
    return false;
  }
  return true;
};

bool rrc_nr::apply_sp_cell_cfg(const sp_cell_cfg_s& sp_cell_cfg)
{
  update_sp_cell_cfg(sp_cell_cfg);

  return true;
}

bool rrc_nr::update_sp_cell_cfg(const sp_cell_cfg_s& sp_cell_cfg)
{
  // NSA specific handling to defer CSI, SR, SRS config until after RA (see TS 38.331, Section 5.3.5.3)
  srsran_csi_hl_cfg_t prev_csi = phy_cfg.csi;

  if (sp_cell_cfg.recfg_with_sync_present) {
    const recfg_with_sync_s& recfg_with_sync = sp_cell_cfg.recfg_with_sync;
    mac->set_crnti(recfg_with_sync.new_ue_id);
    if (recfg_with_sync.sp_cell_cfg_common_present) {
      if (recfg_with_sync.sp_cell_cfg_common.pci_present) {
        phy_cfg.carrier.pci             = recfg_with_sync.sp_cell_cfg_common.pci;
        phy_cfg.carrier.max_mimo_layers = 1; // TODO: flatten
      } else {
        logger.warning("Option PCI not present");
        return false;
      }
      // Read essential DL carrier settings
      if (recfg_with_sync.sp_cell_cfg_common.dl_cfg_common_present) {
        if (apply_dl_common_cfg(recfg_with_sync.sp_cell_cfg_common.dl_cfg_common) == false) {
          return false;
        }
      } else {
        logger.warning("Secondary primary cell dl cfg common not present");
        return false;
      }
      if (recfg_with_sync.sp_cell_cfg_common.ul_cfg_common_present) {
        if (apply_ul_common_cfg(recfg_with_sync.sp_cell_cfg_common.ul_cfg_common) == false) {
          return false;
        }
      } else {
        logger.warning("Secondary primary cell ul cfg common not present");
        return false;
      }
      // Build SSB config
      phy_cfg_nr_t::ssb_cfg_t ssb_cfg = {};
      if (make_phy_ssb_cfg(phy_cfg.carrier, recfg_with_sync.sp_cell_cfg_common, &ssb_cfg) == true) {
        phy_cfg.ssb = ssb_cfg;
      } else {
        logger.warning("Warning while building SSB config structure");
        return false;
      }
      if (recfg_with_sync.sp_cell_cfg_common.tdd_ul_dl_cfg_common_present) {
        logger.debug("TDD UL DL config present, using TDD");
        srsran_duplex_config_nr_t duplex;
        if (make_phy_tdd_cfg(recfg_with_sync.sp_cell_cfg_common.tdd_ul_dl_cfg_common, &duplex)) {
          phy_cfg.duplex                      = duplex;
          phy_cfg.prach.tdd_config.configured = true;
        } else {
          logger.warning("Warning while building duplex structure");
          return false;
        }
      } else {
        logger.debug("TDD UL DL config not present, using FDD");
      }
    }
  } else {
    // for SA this is not sent
    logger.debug("Reconfig with sync not present");
  }

  // Dedicated config
  if (sp_cell_cfg.sp_cell_cfg_ded_present) {
    // Dedicated Downlink
    if (sp_cell_cfg.sp_cell_cfg_ded.init_dl_bwp_present) {
      if (sp_cell_cfg.sp_cell_cfg_ded.init_dl_bwp.pdcch_cfg_present) {
        if (sp_cell_cfg.sp_cell_cfg_ded.init_dl_bwp.pdcch_cfg.type() ==
            setup_release_c<pdcch_cfg_s>::types_opts::setup) {
          if (apply_sp_cell_init_dl_pdcch(sp_cell_cfg.sp_cell_cfg_ded.init_dl_bwp.pdcch_cfg.setup()) == false) {
            return false;
          }
        } else {
          logger.warning("Option pdcch_cfg not of type setup");
          return false;
        }
      } else {
        logger.warning("Option pdcch_cfg not present");
        return false;
      }
      if (sp_cell_cfg.sp_cell_cfg_ded.init_dl_bwp.pdsch_cfg_present) {
        if (sp_cell_cfg.sp_cell_cfg_ded.init_dl_bwp.pdsch_cfg.type() ==
            setup_release_c<pdsch_cfg_s>::types_opts::setup) {
          if (apply_sp_cell_init_dl_pdsch(sp_cell_cfg.sp_cell_cfg_ded.init_dl_bwp.pdsch_cfg.setup()) == false) {
            logger.error("Couldn't apply PDSCH config for initial DL BWP in SpCell Cfg dedicated");
            return false;
          };
        } else {
          logger.warning("Option pdsch_cfg_cfg not of type setup");
          return false;
        }
      } else {
        logger.warning("Option pdsch_cfg not present");
        return false;
      }
    } else {
      logger.warning("Option init_dl_bwp not present");
      return false;
    }
    // Dedicated Uplink
    if (sp_cell_cfg.sp_cell_cfg_ded.ul_cfg_present) {
      if (sp_cell_cfg.sp_cell_cfg_ded.ul_cfg.init_ul_bwp_present) {
        if (sp_cell_cfg.sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg_present) {
          if (sp_cell_cfg.sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.type() ==
              setup_release_c<pucch_cfg_s>::types_opts::setup) {
            if (apply_sp_cell_ded_ul_pucch(sp_cell_cfg.sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pucch_cfg.setup()) == false) {
              return false;
            }
          } else {
            logger.warning("Option pucch_cfg not of type setup");
            return false;
          }
        } else {
          logger.warning("Option pucch_cfg for initial UL BWP in spCellConfigDedicated not present");
          return false;
        }
        if (sp_cell_cfg.sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pusch_cfg_present) {
          if (sp_cell_cfg.sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pusch_cfg.type() ==
              setup_release_c<pusch_cfg_s>::types_opts::setup) {
            if (apply_sp_cell_ded_ul_pusch(sp_cell_cfg.sp_cell_cfg_ded.ul_cfg.init_ul_bwp.pusch_cfg.setup()) == false) {
              return false;
            }
          } else {
            logger.warning("Option pusch_cfg not of type setup");
            return false;
          }
        } else {
          logger.warning("Option pusch_cfg in spCellConfigDedicated not present");
          return false;
        }
      } else {
        logger.warning("Option init_ul_bwp in spCellConfigDedicated not present");
        return false;
      }
    } else {
      logger.warning("Option ul_cfg in spCellConfigDedicated not present");
      return false;
    }

    if (sp_cell_cfg.sp_cell_cfg_ded.pdsch_serving_cell_cfg_present) {
      if (sp_cell_cfg.sp_cell_cfg_ded.pdsch_serving_cell_cfg.type() ==
          setup_release_c<asn1::rrc_nr::pdsch_serving_cell_cfg_s>::types_opts::setup) {
        dl_harq_cfg_nr_t dl_harq_cfg_nr;
        if (make_mac_dl_harq_cfg_nr_t(sp_cell_cfg.sp_cell_cfg_ded.pdsch_serving_cell_cfg.setup(), &dl_harq_cfg_nr) ==
            false) {
          logger.warning("Failed to make dl_harq_cfg_nr config");
          return false;
        }
        mac->set_config(dl_harq_cfg_nr);
      }
    } else {
      logger.debug("Option pdsch_serving_cell_cfg not present");
    }

    if (sp_cell_cfg.sp_cell_cfg_ded.csi_meas_cfg_present) {
      if (sp_cell_cfg.sp_cell_cfg_ded.csi_meas_cfg.type() == setup_release_c<csi_meas_cfg_s>::types_opts::setup) {
        if (apply_csi_meas_cfg(sp_cell_cfg.sp_cell_cfg_ded.csi_meas_cfg.setup()) == false) {
          return false;
        }
      } else {
        logger.warning("Option csi_meas_cfg not of type setup");
        return false;
      }
    } else {
      logger.warning("Option csi_meas_cfg in spCellConfigDedicated not present");
    }

  } else {
    logger.warning("Option sp_cell_cfg_ded not present");
    return false;
  }

  // Configure PHY
  if (sp_cell_cfg.recfg_with_sync_present) {
    // defer CSI config until after RA complete
    srsran::phy_cfg_nr_t current_phycfg = phy_cfg;
    current_phycfg.csi                  = prev_csi;
    phy_cfg_state                       = PHY_CFG_STATE_NSA_APPLY_SP_CELL;
    phy->set_config(current_phycfg);
  } else {
    // apply full config immediately
    phy_cfg_state = PHY_CFG_STATE_SA_FULL_CFG;
    phy->set_config(phy_cfg);
  }

  return true;
}

bool rrc_nr::apply_phy_cell_group_cfg(const phys_cell_group_cfg_s& phys_cell_group_cfg)
{
  srsran_harq_ack_cfg_hl_t harq_ack;
  if (make_phy_harq_ack_cfg(phys_cell_group_cfg, &harq_ack) == true) {
    phy_cfg.harq_ack = harq_ack;
  } else {
    logger.warning("Warning while building harq_ack structure");
    return false;
  }
  return true;
}

bool rrc_nr::apply_cell_group_cfg(const cell_group_cfg_s& cell_group_cfg)
{
  update_cell_group_cfg(cell_group_cfg);

  return true;
}

bool rrc_nr::update_cell_group_cfg(const cell_group_cfg_s& cell_group_cfg)
{
  if (cell_group_cfg.rlc_bearer_to_add_mod_list.size() > 0) {
    for (uint32_t i = 0; i < cell_group_cfg.rlc_bearer_to_add_mod_list.size(); i++) {
      if (apply_rlc_add_mod(cell_group_cfg.rlc_bearer_to_add_mod_list[i]) == false) {
        return false;
      }
    }
  }
  if (cell_group_cfg.mac_cell_group_cfg_present) {
    if (apply_mac_cell_group(cell_group_cfg.mac_cell_group_cfg) == false) {
      return false;
    }
  }
  if (cell_group_cfg.phys_cell_group_cfg_present) {
    if (apply_phy_cell_group_cfg(cell_group_cfg.phys_cell_group_cfg) == false) {
      return false;
    }
  }
  if (cell_group_cfg.sp_cell_cfg_present) {
    if (update_sp_cell_cfg(cell_group_cfg.sp_cell_cfg) == false) {
      return false;
    }
  }
  return true;
}

bool rrc_nr::apply_drb_release(const uint8_t drb)
{
  uint32_t lcid = get_lcid_for_drbid(drb);
  if (lcid == 0) {
    logger.warning("Can not release bearer with lcid %d and drb %d", lcid, drb);
    return false;
  }
  logger.info("Releasing bearer DRB: %d LCID: %d", drb, lcid);
  pdcp->del_bearer(lcid);
  // TODO
  //  2>  if the UE is operating in EN-DC
  // 3>  if a new bearer is not added either with NR or E-UTRA with same eps-BearerIdentity:
  // 4>  indicate the release of the DRB and the eps-BearerIdentity of the released DRB to upper layers.
  return true;
}

bool rrc_nr::apply_srb_add_mod(const srb_to_add_mod_s& srb_cfg)
{
  logger.debug("Applying SRB Add/Mod to SRB%d", srb_cfg.srb_id);
  if (srb_cfg.pdcp_cfg_present) {
    logger.error("Cannot add SRB - only default configuration supported.");
    return false;
  }

  srsran::pdcp_config_t pdcp_cfg = srsran::make_nr_srb_pdcp_config_t(srb_cfg.srb_id, true);
  pdcp->add_bearer(srb_cfg.srb_id, pdcp_cfg);

  return true;
}

bool rrc_nr::apply_drb_add_mod(const drb_to_add_mod_s& drb_cfg)
{
  logger.debug("Applying DRB Add/Mod to DRB%d", drb_cfg.drb_id);
  if (!drb_cfg.pdcp_cfg_present) {
    logger.error("Cannot add DRB - incomplete configuration");
    return false;
  }

  uint32_t lcid = get_lcid_for_drbid(drb_cfg.drb_id);
  if (lcid == 0) {
    logger.error("Cannot find valid LCID for DRB %d", drb_cfg.drb_id);
    return false;
  }

  // Setup PDCP
  if (!(drb_cfg.pdcp_cfg.drb_present == true)) {
    logger.error("PDCP config does not contain DRB config");
    return false;
  }

  if (!(drb_cfg.cn_assoc_present == true)) {
    logger.error("DRB config does not contain an associated cn");
    return false;
  }

  if (drb_cfg.cn_assoc.type() == drb_to_add_mod_s::cn_assoc_c_::types_opts::eps_bearer_id) {
    // register EPS bearer over NR PDCP
    uint32_t eps_bearer_id            = drb_cfg.cn_assoc.eps_bearer_id();
    drb_eps_bearer_id[drb_cfg.drb_id] = eps_bearer_id;
    stack->add_eps_bearer(eps_bearer_id, srsran::srsran_rat_t::nr, lcid);
  } else if (drb_cfg.cn_assoc.type() == drb_to_add_mod_s::cn_assoc_c_::types_opts::sdap_cfg) {
    const auto& sdap_cfg = drb_cfg.cn_assoc.sdap_cfg();

    // Check supported configuration
    if (sdap_cfg.sdap_hdr_dl.value == sdap_cfg_s::sdap_hdr_dl_opts::present || !sdap_cfg.default_drb ||
        sdap_cfg.mapped_qos_flows_to_add.size() != 1) {
      logger.error(
          "Configuring SDAP: only UL headder is supported. Default DRB must be set and number of QoS flows must be 1");
      return false;
    }

    sdap_interface_rrc::bearer_cfg_t sdap_bearer_cfg = {};
    sdap_bearer_cfg.add_downlink_header = sdap_cfg.sdap_hdr_dl.value == sdap_cfg_s::sdap_hdr_dl_opts::present;
    sdap_bearer_cfg.add_uplink_header   = sdap_cfg.sdap_hdr_ul.value == sdap_cfg_s::sdap_hdr_ul_opts::present;
    sdap_bearer_cfg.is_data             = true;
    sdap_bearer_cfg.qfi                 = sdap_cfg.mapped_qos_flows_to_add[0];

    if (not sdap->set_bearer_cfg(lcid, sdap_bearer_cfg)) {
      logger.error("Configuring SDAP");
      return false;
    }

    uint32_t pdu_session_id = drb_cfg.cn_assoc.sdap_cfg().pdu_session;
    // Register PDU session as "EPS bearer" in bearer manager
    stack->add_eps_bearer(pdu_session_id, srsran::srsran_rat_t::nr, lcid);
  } else {
    logger.error("CN association type not supported %s", drb_cfg.cn_assoc.type().to_string());
    return false;
  }

  if (drb_cfg.pdcp_cfg.drb.pdcp_sn_size_dl_present && drb_cfg.pdcp_cfg.drb.pdcp_sn_size_ul_present &&
      (drb_cfg.pdcp_cfg.drb.pdcp_sn_size_ul.to_number() != drb_cfg.pdcp_cfg.drb.pdcp_sn_size_dl.to_number())) {
    logger.warning("PDCP SN size in UL and DL are not the same. make_drb_pdcp_config_t will use the DL SN size %d ",
                   drb_cfg.pdcp_cfg.drb.pdcp_sn_size_dl.to_number());
  }

  if (not security_is_activated) {
    logger.error("Trying to setup DRB%d, but security is not activated", drb_cfg.drb_id);
    return false;
  }
  srsran::pdcp_config_t pdcp_cfg = make_drb_pdcp_config_t(drb_cfg.drb_id, true, drb_cfg.pdcp_cfg);
  pdcp->add_bearer(lcid, pdcp_cfg);

  // Use already configured sec config, if no other sec config present in the RadioBearerConfig
  pdcp->config_security(lcid, sec_cfg);
  pdcp->enable_encryption(lcid, DIRECTION_TXRX);
  if (drb_cfg.pdcp_cfg.drb.integrity_protection_present) {
    pdcp->enable_integrity(lcid, DIRECTION_TXRX);
  }
  return true;
}

bool rrc_nr::apply_security_cfg(const security_cfg_s& security_cfg)
{
  logger.debug("Applying Security config");
  if (security_cfg.key_to_use_present) {
    if (security_cfg.key_to_use.value != security_cfg_s::key_to_use_opts::options::secondary) {
      logger.warning("Only secondary key supported yet");
      return false;
    }
  }

  if (security_cfg.security_algorithm_cfg_present) {
    switch (security_cfg.security_algorithm_cfg.ciphering_algorithm) {
      case ciphering_algorithm_e::nea0:
        sec_cfg.cipher_algo = CIPHERING_ALGORITHM_ID_EEA0;
        break;
      default:
        logger.error("Ciphering not supported by PDCP-NR at the moment %s. Requested algorithm=%s",
                     security_cfg.security_algorithm_cfg.ciphering_algorithm.to_string());
        srsran::console("Ciphering not supported by PDCP-NR at the moment. Requested algorithm=%s\n",
                        security_cfg.security_algorithm_cfg.ciphering_algorithm.to_string());
        return false;
    }

    if (security_cfg.security_algorithm_cfg.integrity_prot_algorithm_present) {
      switch (security_cfg.security_algorithm_cfg.integrity_prot_algorithm) {
        case integrity_prot_algorithm_e::nia0:
          sec_cfg.integ_algo = INTEGRITY_ALGORITHM_ID_EIA0;
          break;
        default:
          logger.error("Integrity protection not supported by PDCP-NR at the moment. Requested algorithm=%s.",
                       security_cfg.security_algorithm_cfg.ciphering_algorithm.to_string());
          srsran::console("Integrity protection not supported by PDCP-NR at the moment. Requested algorithm %s.\n",
                          security_cfg.security_algorithm_cfg.ciphering_algorithm.to_string());
          return false;
      }
    }
    if (usim->update_nr_context(&sec_cfg) == false) {
      return false;
    }
  }

  // Apply security config for all known NR lcids
  for (auto& lcid : lcid_drb) {
    logger.debug("Applying PDCP security config. LCID=%d", lcid.first);
    pdcp->config_security(lcid.first, sec_cfg);
    pdcp->enable_encryption(lcid.first);
  }
  return true;
}

bool rrc_nr::apply_radio_bearer_cfg(const radio_bearer_cfg_s& radio_bearer_cfg)
{
  for (const auto& srb : radio_bearer_cfg.srb_to_add_mod_list) {
    if (apply_srb_add_mod(srb) == false) {
      logger.error("Couldn't apply config for SRB%d.", srb.srb_id);
      return false;
    }
  }
  for (const auto& drb : radio_bearer_cfg.drb_to_add_mod_list) {
    if (apply_drb_add_mod(drb) == false) {
      return false;
    }
  }
  for (const auto& drb : radio_bearer_cfg.drb_to_release_list) {
    if (apply_drb_release(drb) == false) {
      return false;
    }
  }

  if (radio_bearer_cfg.security_cfg_present) {
    if (apply_security_cfg(radio_bearer_cfg.security_cfg) == false) {
      return false;
    }
  } else {
    logger.debug("No Security Config Present");
  }
  return true;
}

bool rrc_nr::handle_rrc_setup(const rrc_setup_s& setup)
{
  // Unpack masterCellGroup into container
  asn1::cbit_ref bref_cg(setup.crit_exts.rrc_setup().master_cell_group.data(),
                         setup.crit_exts.rrc_setup().master_cell_group.size());

  asn1::rrc_nr::cell_group_cfg_s cell_group;
  if (cell_group.unpack(bref_cg) != asn1::SRSASN_SUCCESS) {
    logger.error("Could not unpack master cell group config.");
    return false;
  }
  asn1::json_writer js;
  cell_group.to_json(js);
  logger.debug("Containerized MasterCellGroup: %s", js.to_string().c_str());

  state = RRC_NR_STATE_CONNECTED;
  srsran::console("RRC Connected\n");

  // defer transmission of Setup Complete until PHY reconfiguration has been completed
  if (not conn_setup_proc.launch(
          setup.crit_exts.rrc_setup().radio_bearer_cfg, cell_group, std::move(dedicated_info_nas))) {
    logger.error("Failed to initiate connection setup procedure");
    return false;
  }
  callback_list.add_proc(conn_setup_proc);
  return true;
}

void rrc_nr::handle_rrc_reconfig(const rrc_recfg_s& reconfig)
{
  transaction_id = reconfig.rrc_transaction_id;

  if (not conn_recfg_proc.launch(nr, false, reconfig)) {
    logger.error("Unable to launch connection reconfiguration procedure");
    return;
  }
  callback_list.add_proc(conn_recfg_proc);
}
void rrc_nr::handle_ue_capability_enquiry(const ue_cap_enquiry_s& ue_cap_enquiry)
{
  logger.info("Received UE Capability Enquiry");
  send_ue_capability_info(ue_cap_enquiry);
}

void rrc_nr::handle_dl_info_transfer(const dl_info_transfer_s& dl_info_transfer)
{
  transaction_id = dl_info_transfer.rrc_transaction_id;

  unique_byte_buffer_t pdu = srsran::make_byte_buffer();
  if (pdu == nullptr) {
    logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return;
  }
  if (pdu->get_tailroom() < dl_info_transfer.crit_exts.dl_info_transfer().ded_nas_msg.size()) {
    logger.error("DL Info Transfer too big (%d > %d)",
                 dl_info_transfer.crit_exts.dl_info_transfer().ded_nas_msg.size(),
                 pdu->get_tailroom());
    return;
  }
  pdu->N_bytes = dl_info_transfer.crit_exts.dl_info_transfer().ded_nas_msg.size();
  memcpy(pdu->msg, dl_info_transfer.crit_exts.dl_info_transfer().ded_nas_msg.data(), pdu->N_bytes);
  nas->write_pdu(std::move(pdu));
}

void rrc_nr::handle_security_mode_command(const asn1::rrc_nr::security_mode_cmd_s& smc)
{
  transaction_id = smc.rrc_transaction_id;

  const auto& sec_algo_cfg = smc.crit_exts.security_mode_cmd().security_cfg_smc.security_algorithm_cfg;
  sec_cfg.cipher_algo      = (CIPHERING_ALGORITHM_ID_ENUM)sec_algo_cfg.ciphering_algorithm.value;
  if (sec_algo_cfg.integrity_prot_algorithm_present) {
    sec_cfg.integ_algo = (INTEGRITY_ALGORITHM_ID_ENUM)sec_algo_cfg.integrity_prot_algorithm.value;
  } else {
    logger.error("Missing Integrity Algorithm Config");
  }

  logger.info("Received Security Mode Command nea: %s, nia: %s",
              ciphering_algorithm_id_nr_text[sec_cfg.cipher_algo],
              integrity_algorithm_id_nr_text[sec_cfg.integ_algo]);

  // Generate AS security keys
  generate_as_keys();
  security_is_activated = true;

  // Configure PDCP for security
  uint32_t lcid = srb_to_lcid(nr_srb::srb1);
  pdcp->config_security(lcid, sec_cfg);
  pdcp->enable_integrity(lcid, DIRECTION_TXRX);
  send_security_mode_complete();
  pdcp->enable_encryption(lcid, DIRECTION_TXRX);
}

void rrc_nr::handle_rrc_release(const asn1::rrc_nr::rrc_release_s& msg)
{
  logger.info("Received RRC Release");
  srsran::console("Received RRC Release\n");

  rrc_release();
}

// Security helper used by Security Mode Command and Mobility handling routines
void rrc_nr::generate_as_keys()
{
  as_key_t k_amf = {};
  nas->get_k_amf(k_amf);
  logger.debug(k_amf.data(), 32, "UE K_amf");
  logger.debug("Generating K_gnb with UL NAS COUNT: %d", nas->get_ul_nas_count());
  usim->generate_nr_as_keys(k_amf, nas->get_ul_nas_count(), &sec_cfg);
  logger.info(sec_cfg.k_rrc_enc.data(), 32, "RRC encryption key - k_rrc_enc");
  logger.info(sec_cfg.k_rrc_int.data(), 32, "RRC integrity key  - k_rrc_int");
  logger.info(sec_cfg.k_up_enc.data(), 32, "UP encryption key  - k_up_enc");
}

// RLC interface
void rrc_nr::max_retx_attempted() {}
void rrc_nr::protocol_failure() {}

// MAC interface
void rrc_nr::ra_completed()
{
  logger.info("RA completed.");
  if (rrc_eutra) {
    logger.debug("Applying remaining CSI configuration.");
    phy_cfg_state = PHY_CFG_STATE_NSA_RA_COMPLETED;
    phy->set_config(phy_cfg);
  } else {
    phy_cfg_state = PHY_CFG_STATE_NONE;
  }
}

void rrc_nr::ra_problem()
{
  if (rrc_eutra) {
    rrc_eutra->nr_scg_failure_information(scg_failure_cause_t::random_access_problem);
  } else {
    // TODO: handle RA problem
  }
}

void rrc_nr::release_pucch_srs() {}

// STACK interface
void rrc_nr::cell_search_found_cell(const rrc_interface_phy_nr::cell_search_result_t& result)
{
  cell_selector.trigger(result);
}

void rrc_nr::cell_select_completed(const rrc_interface_phy_nr::cell_select_result_t& result)
{
  cell_selector.trigger(result);
}

void rrc_nr::set_phy_config_complete(bool status)
{
  // inform procedures if they are running
  if (conn_setup_proc.is_busy()) {
    conn_setup_proc.trigger(status);
  }

  if (conn_recfg_proc.is_busy()) {
    conn_recfg_proc.trigger(status);
  }

  switch (phy_cfg_state) {
    case PHY_CFG_STATE_NONE:
      logger.warning("PHY configuration completed without a clear state.");
      break;
    case PHY_CFG_STATE_SA_MIB_CFG:
      logger.info("PHY configuration with MIB parameters completed.");
      break;
    case PHY_CFG_STATE_SA_SIB_CFG:
      logger.info("PHY configuration with SIB parameters completed.");
      break;
    case PHY_CFG_STATE_SA_FULL_CFG:
      logger.info("PHY configuration completed.");
      break;
    case PHY_CFG_STATE_NSA_APPLY_SP_CELL:
      // Start RA procedure
      logger.info("PHY configuration completed. Starting RA procedure.");
      mac->start_ra_procedure();
      break;
    case PHY_CFG_STATE_NSA_RA_COMPLETED:
      logger.info("Remaining CSI configuration completed.");
      break;
  }
  phy_cfg_state = PHY_CFG_STATE_NONE;
}

} // namespace srsue
