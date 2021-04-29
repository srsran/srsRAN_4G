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

#include "srsue/hdr/stack/rrc/rrc_nr.h"
#include "srsran/common/security.h"
#include "srsran/common/standard_streams.h"
#include "srsran/interfaces/ue_pdcp_interfaces.h"
#include "srsran/interfaces/ue_rlc_interfaces.h"
#include "srsue/hdr/stack/upper/usim.h"

#define Error(fmt, ...) rrc_ptr->logger.error("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)
#define Warning(fmt, ...) rrc_ptr->logger.warning("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)
#define Info(fmt, ...) rrc_ptr->logger.info("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)
#define Debug(fmt, ...) rrc_ptr->logger.debug("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)

using namespace asn1::rrc_nr;
using namespace asn1;
using namespace srsran;
namespace srsue {

const char* rrc_nr::rrc_nr_state_text[] = {"IDLE", "CONNECTED", "CONNECTED-INACTIVE"};

rrc_nr::rrc_nr(srsran::task_sched_handle task_sched_) :
  logger(srslog::fetch_basic_logger("RRC-NR")), task_sched(task_sched_), conn_recfg_proc(this)
{}

rrc_nr::~rrc_nr() = default;

void rrc_nr::init(phy_interface_rrc_nr*       phy_,
                  mac_interface_rrc_nr*       mac_,
                  rlc_interface_rrc*          rlc_,
                  pdcp_interface_rrc*         pdcp_,
                  gw_interface_rrc*           gw_,
                  rrc_eutra_interface_rrc_nr* rrc_eutra_,
                  usim_interface_rrc_nr*      usim_,
                  srsran::timer_handler*      timers_,
                  stack_interface_rrc*        stack_,
                  const rrc_nr_args_t&        args_)
{
  phy       = phy_;
  rlc       = rlc_;
  pdcp      = pdcp_;
  gw        = gw_;
  mac       = mac_;
  rrc_eutra = rrc_eutra_;
  usim      = usim_;
  timers    = timers_;
  stack     = stack_;
  args      = args_;

  running               = true;
  sim_measurement_timer = task_sched.get_unique_timer();
}

void rrc_nr::stop()
{
  running = false;
}

void rrc_nr::init_core_less()
{
  logger.info("Creating dummy DRB on LCID=%d", args.coreless.drb_lcid);
  srsran::rlc_config_t rlc_cnfg = srsran::rlc_config_t::default_rlc_um_nr_config(6);
  rlc->add_bearer(args.coreless.drb_lcid, rlc_cnfg);

  srsran::pdcp_config_t pdcp_cnfg{args.coreless.drb_lcid,
                                  srsran::PDCP_RB_IS_DRB,
                                  srsran::SECURITY_DIRECTION_DOWNLINK,
                                  srsran::SECURITY_DIRECTION_UPLINK,
                                  srsran::PDCP_SN_LEN_18,
                                  srsran::pdcp_t_reordering_t::ms500,
                                  srsran::pdcp_discard_timer_t::ms100,
                                  false,
                                  srsran_rat_t::nr};

  pdcp->add_bearer(args.coreless.drb_lcid, pdcp_cnfg);
  return;
}
void rrc_nr::get_metrics(rrc_nr_metrics_t& m) {}

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
  if (timeout_id == sim_measurement_timer.id()) {
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
void rrc_nr::write_pdu(uint32_t lcid, srsran::unique_byte_buffer_t pdu) {}
void rrc_nr::write_pdu_bcch_bch(srsran::unique_byte_buffer_t pdu) {}
void rrc_nr::write_pdu_bcch_dlsch(srsran::unique_byte_buffer_t pdu) {}
void rrc_nr::write_pdu_pcch(srsran::unique_byte_buffer_t pdu) {}
void rrc_nr::write_pdu_mch(uint32_t lcid, srsran::unique_byte_buffer_t pdu) {}

void rrc_nr::get_eutra_nr_capabilities(srsran::byte_buffer_t* eutra_nr_caps_pdu)
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
  mrdc_cap.rf_params_mrdc.supported_band_combination_list_present = true;

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

  mrdc_cap.rf_params_mrdc.applied_freq_band_list_filt_present = true;

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

  mrdc_cap.feature_set_combinations_present = true;

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

  return;
}

bool rrc_nr::rrc_reconfiguration(bool                endc_release_and_add_r15,
                                 bool                nr_secondary_cell_group_cfg_r15_present,
                                 asn1::dyn_octstring nr_secondary_cell_group_cfg_r15,
                                 bool                sk_counter_r15_present,
                                 uint32_t            sk_counter_r15,
                                 bool                nr_radio_bearer_cfg1_r15_present,
                                 asn1::dyn_octstring nr_radio_bearer_cfg1_r15)
{
  if (not conn_recfg_proc.launch(reconf_initiator_t::mcg_srb1,
                                 endc_release_and_add_r15,
                                 nr_secondary_cell_group_cfg_r15_present,
                                 nr_secondary_cell_group_cfg_r15,
                                 sk_counter_r15_present,
                                 sk_counter_r15,
                                 nr_radio_bearer_cfg1_r15_present,
                                 nr_radio_bearer_cfg1_r15)) {
    logger.error("Unable to launch NR RRC reconfiguration procedure");
    return false;
  } else {
    callback_list.add_proc(conn_recfg_proc);
  }
  return true;
}

void rrc_nr::get_nr_capabilities(srsran::byte_buffer_t* nr_caps_pdu)
{
  struct ue_nr_cap_s nr_cap;

  nr_cap.access_stratum_release = access_stratum_release_opts::rel15;
  // PDCP
  nr_cap.pdcp_params.max_num_rohc_context_sessions = pdcp_params_s::max_num_rohc_context_sessions_opts::cs2;

  for (const auto& band : args.supported_bands_nr) {
    band_nr_s band_nr;
    band_nr.band_nr              = band;
    band_nr.ue_pwr_class_present = true;
    band_nr.ue_pwr_class         = band_nr_s::ue_pwr_class_opts::pc3;
    nr_cap.rf_params.supported_band_list_nr.push_back(band_nr);
  }

  nr_cap.rlc_params_present                  = true;
  nr_cap.rlc_params.um_with_short_sn_present = true;
  nr_cap.rlc_params.um_with_long_sn_present  = true;
  nr_cap.pdcp_params.short_sn_present        = args.pdcp_short_sn_support;

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
  return;
};

void rrc_nr::phy_meas_stop()
{
  // possbile race condition for sim_measurement timer, which might be set at the same moment as stopped => fix with
  // phy integration
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

  lc_ch_id = rlc_bearer_cfg.lc_ch_id;
  if (rlc_bearer_cfg.served_radio_bearer_present == true) {
    if (rlc_bearer_cfg.served_radio_bearer.type() == rlc_bearer_cfg_s::served_radio_bearer_c_::types::drb_id) {
      drb_id = rlc_bearer_cfg.served_radio_bearer.drb_id();
      add_lcid_drb(lc_ch_id, drb_id);
    }
  } else {
    logger.warning("In RLC bearer cfg does not contain served radio bearer");
    return false;
  }

  if (rlc_bearer_cfg.rlc_cfg_present == true) {
    rlc_cfg = srsran::make_rlc_config_t(rlc_bearer_cfg.rlc_cfg);
    if (rlc_bearer_cfg.rlc_cfg.type() == asn1::rrc_nr::rlc_cfg_c::types::um_bi_dir) {
      if (rlc_bearer_cfg.rlc_cfg.um_bi_dir().dl_um_rlc.sn_field_len_present &&
          rlc_bearer_cfg.rlc_cfg.um_bi_dir().ul_um_rlc.sn_field_len_present &&
          rlc_bearer_cfg.rlc_cfg.um_bi_dir().dl_um_rlc.sn_field_len !=
              rlc_bearer_cfg.rlc_cfg.um_bi_dir().ul_um_rlc.sn_field_len) {
        logger.warning("NR RLC sequence number length is not the same in uplink and downlink");
        return false;
      }
    } else {
      logger.warning("NR RLC type is not unacknowledged mode bidirectional");
      return false;
    }
  } else {
    logger.warning("In RLC bearer cfg does not contain rlc cfg");
    return false;
  }

  // Setup RLC
  rlc->add_bearer(lc_ch_id, rlc_cfg);

  if (rlc_bearer_cfg.mac_lc_ch_cfg_present == true && rlc_bearer_cfg.mac_lc_ch_cfg.ul_specific_params_present) {
    logical_channel_config_t logical_channel_cfg;
    logical_channel_cfg = srsran::make_mac_logical_channel_cfg_t(lc_ch_id, rlc_bearer_cfg.mac_lc_ch_cfg);
    mac->setup_lcid(logical_channel_cfg);
  }
  return true;
}
bool rrc_nr::apply_mac_cell_group(const mac_cell_group_cfg_s& mac_cell_group_cfg)
{
  if (mac_cell_group_cfg.sched_request_cfg_present) {
    if (mac_cell_group_cfg.sched_request_cfg.sched_request_to_add_mod_list_present) {
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

    if (mac_cell_group_cfg.sched_request_cfg.sched_request_to_release_list_present) {
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
    if (mac_cell_group_cfg.tag_cfg.tag_to_add_mod_list_present) {
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
    if (mac_cell_group_cfg.tag_cfg.tag_to_release_list_present) {
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
    if (mac_cell_group_cfg.phr_cfg.type() == setup_release_c<asn1::rrc_nr::phr_cfg_s>::types_opts::setup) {
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
  if (pdcch_cfg.search_spaces_to_add_mod_list_present) {
    for (uint32_t i = 0; i < pdcch_cfg.search_spaces_to_add_mod_list.size(); i++) {
      srsran_search_space_t search_space;
      if (make_phy_search_space_cfg(pdcch_cfg.search_spaces_to_add_mod_list[i], &search_space) == true) {
        phy_cfg.pdcch.search_space[search_space.id]         = search_space;
        phy_cfg.pdcch.search_space_present[search_space.id] = true;
      } else {
        logger.warning("Warning while building search_space structure");
        return false;
      }
    }
  } else {
    logger.warning("Option search_spaces_to_add_mod_list not present");
    return false;
  }
  if (pdcch_cfg.ctrl_res_set_to_add_mod_list_present) {
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
  if (pdsch_cfg.dmrs_dl_for_pdsch_map_type_a_present) {
    if (pdsch_cfg.dmrs_dl_for_pdsch_map_type_a.type() == setup_release_c<dmrs_dl_cfg_s>::types_opts::setup) {
      srsran_dmrs_sch_add_pos_t srsran_dmrs_sch_add_pos;
      if (make_phy_dmrs_dl_additional_pos(pdsch_cfg.dmrs_dl_for_pdsch_map_type_a.setup(), &srsran_dmrs_sch_add_pos) ==
          true) {
        phy_cfg.pdsch.dmrs_typeA.additional_pos = srsran_dmrs_sch_add_pos;
        phy_cfg.pdsch.dmrs_typeA.present        = true;
      } else {
        logger.warning("Warning while build srsran_dmrs_sch_add_pos structure");
        return false;
      }
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

  if (pdsch_cfg.zp_csi_rs_res_to_add_mod_list_present) {
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
  } else {
    logger.warning("Option zp_csi_rs_res_to_add_mod_list not present");
    return false;
  }

  if (pdsch_cfg.p_zp_csi_rs_res_set_present) {
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
  } else {
    logger.warning("Option p_zp_csi_rs_res_set not present");
    return false;
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
      if (res_list_present[res_id] == true) {
        phy_cfg.csi.reports[report_cfg_id].periodic.resource = res_list[res_id];
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
  if (csi_meas_cfg.csi_report_cfg_to_add_mod_list_present) {
    for (uint32_t i = 0; i < csi_meas_cfg.csi_report_cfg_to_add_mod_list.size(); i++) {
      if (apply_res_csi_report_cfg(csi_meas_cfg.csi_report_cfg_to_add_mod_list[i]) == false) {
        return false;
      }
    }
  } else {
    logger.warning("Option csi_report_cfg_to_add_mod_list not present");
    return false;
  }

  if (csi_meas_cfg.nzp_csi_rs_res_to_add_mod_list_present) {
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
  } else {
    logger.warning("Option nzp_csi_rs_res_to_add_mod_list not present");
    return false;
  }

  if (csi_meas_cfg.nzp_csi_rs_res_set_to_add_mod_list_present) {
    for (uint32_t i = 0; i < csi_meas_cfg.nzp_csi_rs_res_set_to_add_mod_list.size(); i++) {
      uint8_t set_id = csi_meas_cfg.nzp_csi_rs_res_set_to_add_mod_list[i].nzp_csi_res_set_id;
      for (uint32_t j = 0; j < csi_meas_cfg.nzp_csi_rs_res_set_to_add_mod_list[i].nzp_csi_rs_res.size(); j++) {
        uint8_t res = csi_meas_cfg.nzp_csi_rs_res_set_to_add_mod_list[i].nzp_csi_rs_res[j];
        // use temporally stored values to assign
        if (csi_rs_nzp_res.find(res) == csi_rs_nzp_res.end()) {
          logger.warning("Can not find p_zp_csi_rs_res in temporally stored csi_rs_zp_res");
          return false;
        }
        phy_cfg.pdsch.nzp_csi_rs_sets[set_id].data[j] = csi_rs_nzp_res[res];
        phy_cfg.pdsch.nzp_csi_rs_sets[set_id].count += 1;
      }
      if (csi_meas_cfg.nzp_csi_rs_res_set_to_add_mod_list[i].trs_info_present) {
        phy_cfg.pdsch.nzp_csi_rs_sets[set_id].trs_info = true;
      }
    }
  } else {
    logger.warning("Option p_zp_csi_rs_res_set not present");
    return false;
  }

  return true;
}

bool rrc_nr::apply_dl_common_cfg(const asn1::rrc_nr::dl_cfg_common_s& dl_cfg_common)
{
  if (dl_cfg_common.init_dl_bwp_present) {
    if (dl_cfg_common.freq_info_dl_present) {
      if (make_phy_carrier_cfg(dl_cfg_common.freq_info_dl, &phy_cfg.carrier) == false) {
        logger.warning("Warning while making carrier phy config");
        return false;
      }
    } else {
      logger.warning("Option freq_info_dl not present");
      return false;
    }
    if (dl_cfg_common.init_dl_bwp.pdsch_cfg_common_present) {
      if (dl_cfg_common.init_dl_bwp.pdsch_cfg_common.type() ==
          asn1::rrc_nr::setup_release_c<asn1::rrc_nr::pdsch_cfg_common_s>::types_opts::setup) {
        const pdcch_cfg_common_s& pdcch_cfg_common = dl_cfg_common.init_dl_bwp.pdcch_cfg_common.setup();
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
        if (pdcch_cfg_common.common_search_space_list_present) {
          for (uint32_t i = 0; i < pdcch_cfg_common.common_search_space_list.size(); i++) {
            srsran_search_space_t search_space;
            if (make_phy_search_space_cfg(pdcch_cfg_common.common_search_space_list[i], &search_space) == true) {
              phy_cfg.pdcch.search_space[search_space.id]         = search_space;
              phy_cfg.pdcch.search_space_present[search_space.id] = true;
            } else {
              logger.warning("Warning while building search_space structure");
              return false;
            }
          }
        } else {
          logger.warning("Option common_search_space_list not present");
          return false;
        }
        if (pdcch_cfg_common.ra_search_space_present) {
          if (phy_cfg.pdcch.search_space_present[pdcch_cfg_common.ra_search_space] == true) {
            // phy_cfg.pdcch.ra_rnti                 = 0x16; //< Supposed to be deduced from PRACH configuration
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
        if (pdsch_cfg_common.pdsch_time_domain_alloc_list_present) {
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
  if (ul_cfg_common.init_ul_bwp_present) {
    if (ul_cfg_common.init_ul_bwp.rach_cfg_common_present) {
      if (ul_cfg_common.init_ul_bwp.rach_cfg_common.type() == setup_release_c<rach_cfg_common_s>::types_opts::setup) {
        rach_nr_cfg_t rach_nr_cfg = make_mac_rach_cfg(ul_cfg_common.init_ul_bwp.rach_cfg_common.setup());
        mac->set_config(rach_nr_cfg);

        // Make the RACH configuration for PHY
        if (not make_phy_rach_cfg(ul_cfg_common.init_ul_bwp.rach_cfg_common.setup(), &phy_cfg.prach)) {
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
        if (ul_cfg_common.init_ul_bwp.pusch_cfg_common.setup().pusch_time_domain_alloc_list_present) {
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
        logger.info("PUCCH cfg commont setup not handled");
      } else {
        logger.warning("Option pucch_cfg_common not of type setup");
        return false;
      }
    } else {
      logger.warning("Option pucch_cfg_common not present");
      return false;
    }
  } else {
    logger.warning("Option init_ul_bwp not present");
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
  if (pucch_cfg.res_to_add_mod_list_present) {
    for (uint32_t i = 0; i < pucch_cfg.res_to_add_mod_list.size(); i++) {
      uint32_t res_id = pucch_cfg.res_to_add_mod_list[i].pucch_res_id;
      if (make_phy_res_config(pucch_cfg.res_to_add_mod_list[i], format_2_max_code_rate, &res_list[res_id]) == true) {
        res_list_present[res_id] = true;
      } else {
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
  if (pucch_cfg.res_set_to_add_mod_list_present) {
    for (uint32_t i = 0; i < pucch_cfg.res_set_to_add_mod_list.size(); i++) {
      uint32_t set_id                          = pucch_cfg.res_set_to_add_mod_list[i].pucch_res_set_id;
      phy_cfg.pucch.sets[set_id].nof_resources = pucch_cfg.res_set_to_add_mod_list[i].res_list.size();
      for (uint32_t j = 0; j < pucch_cfg.res_set_to_add_mod_list[i].res_list.size(); j++) {
        uint32_t res_id = pucch_cfg.res_set_to_add_mod_list[i].res_list[j];
        if (res_list_present[res_id] == true) {
          phy_cfg.pucch.sets[set_id].resources[j] = res_list[res_id];
        } else {
          logger.error(
              "Resources set not present for assign pucch sets (res_id %d, setid %d, j %d)", res_id, set_id, j);
        }
      }
    }
  }

  if (pucch_cfg.sched_request_res_to_add_mod_list_present) {
    for (uint32_t i = 0; i < pucch_cfg.sched_request_res_to_add_mod_list.size(); i++) {
      uint32_t                      res_id = pucch_cfg.sched_request_res_to_add_mod_list[i].sched_request_res_id;
      srsran_pucch_nr_sr_resource_t srsran_pucch_nr_sr_resource;
      if (make_phy_sr_resource(pucch_cfg.sched_request_res_to_add_mod_list[i], &srsran_pucch_nr_sr_resource) ==
          true) { // TODO: fix that if indexing is solved
        phy_cfg.pucch.sr_resources[res_id] = srsran_pucch_nr_sr_resource;

        // Set PUCCH resource
        if (pucch_cfg.sched_request_res_to_add_mod_list[i].res_present) {
          uint32_t pucch_res_id = pucch_cfg.sched_request_res_to_add_mod_list[i].res;
          if (res_list_present[res_id]) {
            phy_cfg.pucch.sr_resources[res_id].resource = res_list[pucch_res_id];
          } else {
            logger.warning("Warning SR's PUCCH resource is invalid (%d)", pucch_res_id);
            phy_cfg.pucch.sr_resources[res_id].configured = false;
          }
        } else {
          logger.warning("Warning SR resource is present but no PUCCH resource is assigned to it");
          phy_cfg.pucch.sr_resources[res_id].configured = false;
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

  if (pucch_cfg.dl_data_to_ul_ack_present) {
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
  srsran_resource_alloc_t resource_alloc;
  if (make_phy_pusch_alloc_type(pusch_cfg, &resource_alloc) == true) {
    phy_cfg.pusch.alloc = resource_alloc;
  }

  if (pusch_cfg.dmrs_ul_for_pusch_map_type_a_present) {
    if (pusch_cfg.dmrs_ul_for_pusch_map_type_a.type() == setup_release_c<dmrs_ul_cfg_s>::types_opts::setup) {
      srsran_dmrs_sch_add_pos_t srsran_dmrs_sch_add_pos;
      if (make_phy_dmrs_ul_additional_pos(pusch_cfg.dmrs_ul_for_pusch_map_type_a.setup(), &srsran_dmrs_sch_add_pos) ==
          true) {
        phy_cfg.pusch.dmrs_typeA.additional_pos = srsran_dmrs_sch_add_pos;
        phy_cfg.pusch.dmrs_typeA.present        = true;
      } else {
        logger.warning("Warning while build srsran_dmrs_sch_add_pos structure");
        return false;
      }
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
        if (make_phy_pusch_scaling(pusch_cfg.uci_on_pusch.setup(), &phy_cfg.pusch.scaling) == false) {
          logger.warning("Warning while building scaling structure");
          return false;
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
      if (recfg_with_sync.sp_cell_cfg_common.ul_cfg_common_present) {
        if (apply_ul_common_cfg(recfg_with_sync.sp_cell_cfg_common.ul_cfg_common) == false) {
          return false;
        }
      } else {
        logger.warning("Secondary primary cell ul cfg common not present");
        return false;
      }
      if (recfg_with_sync.sp_cell_cfg_common.dl_cfg_common_present) {
        if (apply_dl_common_cfg(recfg_with_sync.sp_cell_cfg_common.dl_cfg_common) == false) {
          return false;
        }
      } else {
        logger.warning("DL cfg common not present");
        return false;
      }
      if (recfg_with_sync.sp_cell_cfg_common.tdd_ul_dl_cfg_common_present) {
        srsran_tdd_config_nr_t tdd;
        if (make_phy_tdd_cfg(recfg_with_sync.sp_cell_cfg_common.tdd_ul_dl_cfg_common, &tdd) == true) {
          phy_cfg.tdd = tdd;
        } else {
          logger.warning("Warning while building tdd structure");
          return false;
        }
      } else {
        logger.warning("TDD UL DL config not present");
        return false;
      }
    }
  } else {
    logger.warning("Reconfig with with sync not present");
    return false;
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
          apply_sp_cell_init_dl_pdsch(sp_cell_cfg.sp_cell_cfg_ded.init_dl_bwp.pdsch_cfg.setup());
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
          logger.warning("Option pucch_cfg not present");
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
          logger.warning("Option pusch_cfg not present");
          return false;
        }
      } else {
        logger.warning("Option init_ul_bwp not present");
        return false;
      }
    } else {
      logger.warning("Option ul_cfg not present");
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
      logger.warning("Option pdsch_serving_cell_cfg not present");
      return false;
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
      logger.warning("Option csi_meas_cfg not present");
      return false;
    }

  } else {
    logger.warning("Option sp_cell_cfg_ded not present");
    return false;
  }
  phy->set_config(phy_cfg);
  mac->start_ra_procedure();
  return true;
}

bool rrc_nr::apply_phy_cell_group_cfg(const phys_cell_group_cfg_s& phys_cell_group_cfg)
{
  srsran_ue_dl_nr_harq_ack_cfg_t harq_ack;
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
  if (cell_group_cfg.rlc_bearer_to_add_mod_list_present) {
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
    if (apply_sp_cell_cfg(cell_group_cfg.sp_cell_cfg) == false) {
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

bool rrc_nr::apply_drb_add_mod(const drb_to_add_mod_s& drb_cfg)
{
  if (!drb_cfg.pdcp_cfg_present) {
    logger.error("Cannot add DRB - incomplete configuration");
    return false;
  }

  uint32_t lcid = get_lcid_for_drbid(drb_cfg.drb_id);

  // Setup PDCP
  if (!(drb_cfg.pdcp_cfg.drb_present == true)) {
    logger.error("PDCP config does not contain DRB config");
    return false;
  }

  if (!(drb_cfg.cn_assoc_present == true)) {
    logger.error("DRB config does not contain an associated cn");
    return false;
  }

  if (!(drb_cfg.cn_assoc.type() == drb_to_add_mod_s::cn_assoc_c_::types_opts::eps_bearer_id)) {
    logger.error("CN association type not supported %s ", drb_cfg.cn_assoc.type().to_string());
    return false;
  }
  uint32_t eps_bearer_id            = drb_cfg.cn_assoc.eps_bearer_id();
  drb_eps_bearer_id[drb_cfg.drb_id] = eps_bearer_id;

  if (drb_cfg.pdcp_cfg.drb.pdcp_sn_size_dl_present && drb_cfg.pdcp_cfg.drb.pdcp_sn_size_ul_present &&
      (drb_cfg.pdcp_cfg.drb.pdcp_sn_size_ul.to_number() != drb_cfg.pdcp_cfg.drb.pdcp_sn_size_dl.to_number())) {
    logger.warning("PDCP SN size in UL and DL are not the same. make_drb_pdcp_config_t will use the DL SN size %d ",
                   drb_cfg.pdcp_cfg.drb.pdcp_sn_size_dl.to_number());
  }

  srsran::pdcp_config_t pdcp_cfg = make_drb_pdcp_config_t(drb_cfg.drb_id, true, drb_cfg.pdcp_cfg);
  pdcp->add_bearer(lcid, pdcp_cfg);
  gw->update_lcid(eps_bearer_id, lcid);
  return true;
}

bool rrc_nr::apply_security_cfg(const security_cfg_s& security_cfg)
{
  // TODO derive correct keys
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
      case ciphering_algorithm_e::nea1:
        sec_cfg.cipher_algo = CIPHERING_ALGORITHM_ID_128_EEA1;
        break;
      case ciphering_algorithm_e::nea2:
        sec_cfg.cipher_algo = CIPHERING_ALGORITHM_ID_128_EEA2;
        break;
      case ciphering_algorithm_e::nea3:
        sec_cfg.cipher_algo = CIPHERING_ALGORITHM_ID_128_EEA3;
        break;
      default:
        logger.warning("Unsupported algorithm %s", security_cfg.security_algorithm_cfg.ciphering_algorithm.to_string());
        return false;
    }

    if (security_cfg.security_algorithm_cfg.integrity_prot_algorithm_present) {
      switch (security_cfg.security_algorithm_cfg.integrity_prot_algorithm) {
        case integrity_prot_algorithm_e::nia0:
          sec_cfg.integ_algo = INTEGRITY_ALGORITHM_ID_EIA0;
          break;
        case integrity_prot_algorithm_e::nia1:
          sec_cfg.integ_algo = INTEGRITY_ALGORITHM_ID_128_EIA1;
          break;
        case integrity_prot_algorithm_e::nia2:
          sec_cfg.integ_algo = INTEGRITY_ALGORITHM_ID_128_EIA2;
          break;
        case integrity_prot_algorithm_e::nia3:
          sec_cfg.integ_algo = INTEGRITY_ALGORITHM_ID_128_EIA3;
          break;
        default:
          logger.warning("Unsupported algorithm %s",
                         security_cfg.security_algorithm_cfg.integrity_prot_algorithm.to_string());
          return false;
      }
    }
    if (usim->update_nr_context(&sec_cfg) == false) {
      return false;
    }
  }

  // Apply security config for all known NR lcids
  for (auto& lcid : lcid_drb) {
    pdcp->config_security(lcid.first, sec_cfg);
    pdcp->enable_encryption(lcid.first);
  }
  return true;
}

bool rrc_nr::apply_radio_bearer_cfg(const radio_bearer_cfg_s& radio_bearer_cfg)
{
  if (radio_bearer_cfg.drb_to_add_mod_list_present) {
    for (uint32_t i = 0; i < radio_bearer_cfg.drb_to_add_mod_list.size(); i++) {
      if (apply_drb_add_mod(radio_bearer_cfg.drb_to_add_mod_list[i]) == false) {
        return false;
      }
    }
  }
  if (radio_bearer_cfg.drb_to_release_list_present) {
    for (uint32_t i = 0; i < radio_bearer_cfg.drb_to_release_list.size(); i++) {
      if (apply_drb_release(radio_bearer_cfg.drb_to_release_list[i]) == false) {
        return false;
      }
    }
  }
  if (radio_bearer_cfg.security_cfg_present) {
    if (apply_security_cfg(radio_bearer_cfg.security_cfg) == false) {
      return false;
    }
  }
  return true;
}
// RLC interface
void rrc_nr::max_retx_attempted() {}

// MAC interface
void rrc_nr::ra_completed() {}
void rrc_nr::ra_problem()
{
  rrc_eutra->nr_scg_failure_information(scg_failure_cause_t::random_access_problem);
}

void rrc_nr::release_pucch_srs() {}

// STACK interface
void rrc_nr::cell_search_completed(const rrc_interface_phy_lte::cell_search_ret_t& cs_ret, const phy_cell_t& found_cell)
{}

/* Procedures */
rrc_nr::connection_reconf_no_ho_proc::connection_reconf_no_ho_proc(rrc_nr* parent_) : rrc_ptr(parent_), initiator(nr) {}

proc_outcome_t rrc_nr::connection_reconf_no_ho_proc::init(const reconf_initiator_t initiator_,
                                                          const bool               endc_release_and_add_r15,
                                                          const bool nr_secondary_cell_group_cfg_r15_present,
                                                          const asn1::dyn_octstring nr_secondary_cell_group_cfg_r15,
                                                          const bool                sk_counter_r15_present,
                                                          const uint32_t            sk_counter_r15,
                                                          const bool                nr_radio_bearer_cfg1_r15_present,
                                                          const asn1::dyn_octstring nr_radio_bearer_cfg1_r15)
{
  Info("Starting...");
  initiator = initiator_;

  rrc_recfg_s        rrc_recfg;
  cell_group_cfg_s   cell_group_cfg;
  radio_bearer_cfg_s radio_bearer_cfg;
  asn1::SRSASN_CODE  err;

  if (nr_secondary_cell_group_cfg_r15_present) {
    cbit_ref bref(nr_secondary_cell_group_cfg_r15.data(), nr_secondary_cell_group_cfg_r15.size());
    err = rrc_recfg.unpack(bref);
    if (err != asn1::SRSASN_SUCCESS) {
      Error("Could not unpack NR reconfiguration message.");
      return proc_outcome_t::error;
    }

    rrc_ptr->log_rrc_message(
        "RRC NR Reconfiguration", Rx, nr_secondary_cell_group_cfg_r15, rrc_recfg, "NR Secondary Cell Group Cfg R15");

    if (rrc_recfg.crit_exts.type() != asn1::rrc_nr::rrc_recfg_s::crit_exts_c_::types::rrc_recfg) {
      Error("Reconfiguration does not contain Secondary Cell Group Config");
      return proc_outcome_t::error;
    }

    if (not rrc_recfg.crit_exts.rrc_recfg().secondary_cell_group_present) {
      Error("Reconfiguration does not contain Secondary Cell Group Config");
      return proc_outcome_t::error;
    }

    cbit_ref bref0(rrc_recfg.crit_exts.rrc_recfg().secondary_cell_group.data(),
                   rrc_recfg.crit_exts.rrc_recfg().secondary_cell_group.size());

    err = cell_group_cfg.unpack(bref0);
    if (err != asn1::SRSASN_SUCCESS) {
      Error("Could not unpack cell group message message.");
      return proc_outcome_t::error;
    }

    rrc_ptr->log_rrc_message("RRC NR Reconfiguration",
                             Rx,
                             rrc_recfg.crit_exts.rrc_recfg().secondary_cell_group,
                             cell_group_cfg,
                             "Secondary Cell Group Config");

    Info("Applying Cell Group Cfg");
    if (!rrc_ptr->apply_cell_group_cfg(cell_group_cfg)) {
      return proc_outcome_t::error;
    }
  }

  if (sk_counter_r15_present) {
    Info("Applying Cell Group Cfg");
    if (!rrc_ptr->configure_sk_counter((uint16_t)sk_counter_r15)) {
      return proc_outcome_t::error;
    }
  }

  if (nr_radio_bearer_cfg1_r15_present) {
    cbit_ref bref1(nr_radio_bearer_cfg1_r15.data(), nr_radio_bearer_cfg1_r15.size());

    err = radio_bearer_cfg.unpack(bref1);
    if (err != asn1::SRSASN_SUCCESS) {
      Error("Could not unpack radio bearer config.");
      return proc_outcome_t::error;
    }

    rrc_ptr->log_rrc_message(
        "RRC NR Reconfiguration", Rx, nr_radio_bearer_cfg1_r15, radio_bearer_cfg, "Radio Bearer Config R15");

    Info("Applying Radio Bearer Cfg");
    if (!rrc_ptr->apply_radio_bearer_cfg(radio_bearer_cfg)) {
      return proc_outcome_t::error;
    }
  }

  return proc_outcome_t::success;
}

proc_outcome_t rrc_nr::connection_reconf_no_ho_proc::react(const bool& config_complete)
{
  if (not config_complete) {
    Error("NR reconfiguration failed");
    return proc_outcome_t::error;
  }

  // TODO phy ctrl
  // in case there are scell to configure, wait for second phy configuration
  // if (not rrc_ptr->phy_ctrl->is_config_pending()) {
  //   return proc_outcome_t::yield;
  // }

  Info("Reconfig NR return successful");
  return proc_outcome_t::success;
}

void rrc_nr::connection_reconf_no_ho_proc::then(const srsran::proc_state_t& result)
{
  if (result.is_success()) {
    Info("Finished %s successfully", name());
    srsran::console("RRC NR reconfiguration successful.\n");
    rrc_ptr->rrc_eutra->nr_rrc_con_reconfig_complete(true);
  } else {
    // 5.3.5.8.2 Inability to comply with RRCReconfiguration
    switch (initiator) {
      case reconf_initiator_t::mcg_srb1:
        rrc_ptr->rrc_eutra->nr_notify_reconfiguration_failure();
        break;
      default:
        Warning("Reconfiguration failure not implemented for initiator %d", initiator);
        break;
    }
    srsran::console("RRC NR reconfiguration failed.\n");
    Warning("Finished %s with failure", name());
  }
  return;
}

} // namespace srsue
