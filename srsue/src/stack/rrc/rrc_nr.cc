/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsue/hdr/stack/rrc/rrc_nr.h"
#include "srslte/common/security.h"
#include "srsue/hdr/stack/upper/usim.h"

#define Error(fmt, ...) rrc_ptr->log_h->error("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)
#define Warning(fmt, ...) rrc_ptr->log_h->warning("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)
#define Info(fmt, ...) rrc_ptr->log_h->info("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)
#define Debug(fmt, ...) rrc_ptr->log_h->debug("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)

using namespace asn1::rrc_nr;
using namespace asn1;
using namespace srslte;
namespace srsue {

const char* rrc_nr::rrc_nr_state_text[] = {"IDLE", "CONNECTED", "CONNECTED-INACTIVE"};

rrc_nr::rrc_nr(srslte::task_sched_handle task_sched_) : log_h("RRC"), task_sched(task_sched_), conn_recfg_proc(this) {}

rrc_nr::~rrc_nr() = default;

void rrc_nr::init(phy_interface_rrc_nr*       phy_,
                  mac_interface_rrc_nr*       mac_,
                  rlc_interface_rrc*          rlc_,
                  pdcp_interface_rrc*         pdcp_,
                  gw_interface_rrc*           gw_,
                  rrc_eutra_interface_rrc_nr* rrc_eutra_,
                  usim_interface_rrc_nr*      usim_,
                  srslte::timer_handler*      timers_,
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

  running                = true;
  fake_measurement_timer = task_sched.get_unique_timer();
}

void rrc_nr::stop()
{
  running = false;
}

void rrc_nr::init_core_less()
{
  log_h->info("Creating dummy DRB on LCID=%d", args.coreless.drb_lcid);
  srslte::rlc_config_t rlc_cnfg = srslte::rlc_config_t::default_rlc_um_nr_config(6);
  rlc->add_bearer(args.coreless.drb_lcid, rlc_cnfg);

  srslte::pdcp_config_t pdcp_cnfg{args.coreless.drb_lcid,
                                  srslte::PDCP_RB_IS_DRB,
                                  srslte::SECURITY_DIRECTION_DOWNLINK,
                                  srslte::SECURITY_DIRECTION_UPLINK,
                                  srslte::PDCP_SN_LEN_18,
                                  srslte::pdcp_t_reordering_t::ms500,
                                  srslte::pdcp_discard_timer_t::ms100,
                                  false};

  pdcp->add_bearer(args.coreless.drb_lcid, pdcp_cnfg);
  return;
}
void rrc_nr::get_metrics(rrc_nr_metrics_t& m) {}

// Timeout callback interface
void rrc_nr::timer_expired(uint32_t timeout_id)
{
  log_h->debug("[NR] Handling Timer Expired");
  if (timeout_id == fake_measurement_timer.id()) {
    log_h->debug("[NR] Triggered Fake Measurement");

    phy_meas_nr_t              fake_meas = {};
    std::vector<phy_meas_nr_t> phy_meas_nr;
    fake_meas.rsrp     = -60.0;
    fake_meas.rsrq     = -60.0;
    fake_meas.cfo_hz   = 1.0;
    fake_meas.arfcn_nr = fake_measurement_carrier_freq_r15;
    fake_meas.pci_nr   = 500;
    phy_meas_nr.push_back(fake_meas);
    rrc_eutra->new_cell_meas_nr(phy_meas_nr);

    auto timer_expire_func = [this](uint32_t tid) { timer_expired(tid); };
    fake_measurement_timer.set(10, timer_expire_func);
    fake_measurement_timer.run();
  }
}

void rrc_nr::srslte_rrc_log(const char* str) {}

template <class T>
void rrc_nr::log_rrc_message(const std::string&           source,
                             direction_t                  dir,
                             const srslte::byte_buffer_t* pdu,
                             const T&                     msg,
                             const std::string&           msg_type)
{
  if (log_h->get_level() == srslte::LOG_LEVEL_INFO) {
    log_h->info("%s - %s %s (%d B)", source.c_str(), (dir == Rx) ? "Rx" : "Tx", msg_type.c_str(), pdu->N_bytes);
  } else if (log_h->get_level() >= srslte::LOG_LEVEL_DEBUG) {
    asn1::json_writer json_writer;
    msg.to_json(json_writer);
    log_h->debug_hex(pdu->msg,
                     pdu->N_bytes,
                     "%s - %s %s (%d B)",
                     source.c_str(),
                     (dir == Rx) ? "Rx" : "Tx",
                     msg_type.c_str(),
                     pdu->N_bytes);
    log_h->debug_long("Content:\n%s", json_writer.to_string().c_str());
  }
}

template <class T>
void rrc_nr::log_rrc_message(const std::string& source,
                             direction_t        dir,
                             dyn_octstring      oct,
                             const T&           msg,
                             const std::string& msg_type)
{
  if (log_h->get_level() == srslte::LOG_LEVEL_INFO) {
    log_h->info("%s - %s %s (%d B)", source.c_str(), (dir == Rx) ? "Rx" : "Tx", msg_type.c_str(), oct.size());
  } else if (log_h->get_level() >= srslte::LOG_LEVEL_DEBUG) {
    asn1::json_writer json_writer;
    msg.to_json(json_writer);
    log_h->debug_hex(oct.data(),
                     oct.size(),
                     "%s - %s %s (%d B)",
                     source.c_str(),
                     (dir == Rx) ? "Rx" : "Tx",
                     msg_type.c_str(),
                     oct.size());
    log_h->debug_long("Content:\n%s", json_writer.to_string().c_str());
  }
}

bool rrc_nr::add_lcid_rb(uint32_t lcid, rb_type_t rb_type, uint32_t rbid)
{
  if (lcid_rb.find(lcid) != lcid_rb.end()) {
    log_h->error("Couldn't add RB to LCID. RB %d does exist.", rbid);
    return false;
  } else {
    log_h->info("Adding lcid %d and radio bearer ID %d with type %s ", lcid, rbid, (rb_type == Srb) ? "SRB" : "DRB");
    lcid_rb[lcid].rb_id   = rbid;
    lcid_rb[lcid].rb_type = rb_type;
  }
  return true;
}

uint32_t rrc_nr::get_lcid_for_rbid(uint32_t rb_id)
{
  for (auto& rb : lcid_rb) {
    if (rb.second.rb_id == rb_id) {
      return rb.first;
    }
  }
  log_h->error("Couldn't find LCID for rb LCID. RB %d does exist.", rb_id);
  return 0;
}

// PHY interface
void rrc_nr::in_sync() {}
void rrc_nr::out_of_sync() {}

// MAC interface
void rrc_nr::run_tti(uint32_t tti) {}

// PDCP interface
void rrc_nr::write_pdu(uint32_t lcid, srslte::unique_byte_buffer_t pdu) {}
void rrc_nr::write_pdu_bcch_bch(srslte::unique_byte_buffer_t pdu) {}
void rrc_nr::write_pdu_bcch_dlsch(srslte::unique_byte_buffer_t pdu) {}
void rrc_nr::write_pdu_pcch(srslte::unique_byte_buffer_t pdu) {}
void rrc_nr::write_pdu_mch(uint32_t lcid, srslte::unique_byte_buffer_t pdu) {}

void rrc_nr::get_eutra_nr_capabilities(srslte::byte_buffer_t* eutra_nr_caps_pdu)
{
  struct ue_mrdc_cap_s mrdc_cap;

  band_combination_s band_combination;

  struct band_params_c band_param_eutra;
  band_param_eutra.set_eutra();
  band_param_eutra.eutra().ca_bw_class_dl_eutra_present = true;
  band_param_eutra.eutra().ca_bw_class_ul_eutra_present = true;
  band_param_eutra.eutra().band_eutra                   = 1; // TODO: this also needs to be set here?
  band_param_eutra.eutra().ca_bw_class_dl_eutra         = asn1::rrc_nr::ca_bw_class_eutra_opts::options::a;
  band_param_eutra.eutra().ca_bw_class_ul_eutra         = asn1::rrc_nr::ca_bw_class_eutra_opts::options::a;
  band_combination.band_list.push_back(band_param_eutra);

  // TODO check if band is requested
  for (const auto& band : args.supported_bands) {
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
  freq_band_info_c band_info_eutra;
  band_info_eutra.set_band_info_eutra();
  band_info_eutra.band_info_eutra().ca_bw_class_dl_eutra_present = false;
  band_info_eutra.band_info_eutra().ca_bw_class_ul_eutra_present = false;
  band_info_eutra.band_info_eutra().band_eutra                   = 1;
  mrdc_cap.rf_params_mrdc.applied_freq_band_list_filt.push_back(band_info_eutra);

  for (const auto& band : args.supported_bands) {
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

  for (const auto& band : args.supported_bands) {
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

  log_h->debug_hex(
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
  // sanity check only for now
  if (nr_secondary_cell_group_cfg_r15_present == false || sk_counter_r15_present == false ||
      nr_radio_bearer_cfg1_r15_present == false) {
    log_h->error("RRC NR Reconfiguration failed sanity check failed");
    return false;
  }

  rrc_recfg_s        rrc_recfg;
  cell_group_cfg_s   cell_group_cfg;
  radio_bearer_cfg_s radio_bearer_cfg;
  asn1::SRSASN_CODE  err;

  cbit_ref bref(nr_secondary_cell_group_cfg_r15.data(), nr_secondary_cell_group_cfg_r15.size());

  err = rrc_recfg.unpack(bref);
  if (err != asn1::SRSASN_SUCCESS) {
    log_h->error("Could not unpack NR reconfiguration message.");
    return false;
  }

  log_rrc_message(
      "RRC NR Reconfiguration", Rx, nr_secondary_cell_group_cfg_r15, rrc_recfg, "NR Secondary Cell Group Cfg R15");

  if (rrc_recfg.crit_exts.type() == asn1::rrc_nr::rrc_recfg_s::crit_exts_c_::types::rrc_recfg) {
    if (rrc_recfg.crit_exts.rrc_recfg().secondary_cell_group_present == true) {
      cbit_ref bref0(rrc_recfg.crit_exts.rrc_recfg().secondary_cell_group.data(),
                     rrc_recfg.crit_exts.rrc_recfg().secondary_cell_group.size());

      err = cell_group_cfg.unpack(bref0);
      if (err != asn1::SRSASN_SUCCESS) {
        log_h->error("Could not unpack cell group message message.");
        return false;
      }

      log_rrc_message("RRC NR Reconfiguration",
                      Rx,
                      rrc_recfg.crit_exts.rrc_recfg().secondary_cell_group,
                      cell_group_cfg,
                      "Secondary Cell Group Config");
    } else {
      log_h->error("Reconfiguration does not contain Secondary Cell Group Config");
      return false;
    }
  }

  cbit_ref bref1(nr_radio_bearer_cfg1_r15.data(), nr_radio_bearer_cfg1_r15.size());

  err = radio_bearer_cfg.unpack(bref1);
  if (err != asn1::SRSASN_SUCCESS) {
    log_h->error("Could not unpack radio bearer config.");
    return false;
  }

  log_rrc_message("RRC NR Reconfiguration", Rx, nr_radio_bearer_cfg1_r15, radio_bearer_cfg, "Radio Bearer Config R15");
  if (not conn_recfg_proc.launch(endc_release_and_add_r15,
                                 rrc_recfg,
                                 cell_group_cfg,
                                 sk_counter_r15_present,
                                 sk_counter_r15,
                                 radio_bearer_cfg)) {
    log_h->error("Unable to launch NR RRC configuration procedure");
    return false;
  } else {
    callback_list.add_proc(conn_recfg_proc);
  }
  return true;
}

void rrc_nr::get_nr_capabilities(srslte::byte_buffer_t* nr_caps_pdu)
{
  struct ue_nr_cap_s nr_cap;

  nr_cap.access_stratum_release = access_stratum_release_opts::rel15;
  // PDCP
  nr_cap.pdcp_params.max_num_rohc_context_sessions = pdcp_params_s::max_num_rohc_context_sessions_opts::cs2;

  for (const auto& band : args.supported_bands) {
    band_nr_s band_nr;
    band_nr.band_nr              = band;
    band_nr.ue_pwr_class_present = true;
    band_nr.ue_pwr_class         = band_nr_s::ue_pwr_class_opts::pc3;
    nr_cap.rf_params.supported_band_list_nr.push_back(band_nr);
  }

  nr_cap.rlc_params_present                  = true;
  nr_cap.rlc_params.um_with_short_sn_present = true;
  nr_cap.rlc_params.um_with_long_sn_present  = true;

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

  log_h->debug_hex(nr_caps_pdu->msg, nr_caps_pdu->N_bytes, "NR capabilities (%u B)", nr_caps_pdu->N_bytes);
  return;
};

void rrc_nr::phy_meas_stop()
{
  // possbile race condition for fake_measurement timer, which might be set at the same moment as stopped => fix with
  // phy integration
  log_h->debug("[NR] Stopping fake measurements");
  fake_measurement_timer.stop();
}

void rrc_nr::phy_set_cells_to_meas(uint32_t carrier_freq_r15)
{
  log_h->debug("[NR] Measuring phy cell %d ", carrier_freq_r15);
  // Start timer for fake measurements
  auto timer_expire_func            = [this](uint32_t tid) { timer_expired(tid); };
  fake_measurement_carrier_freq_r15 = carrier_freq_r15;
  fake_measurement_timer.set(10, timer_expire_func);
  fake_measurement_timer.run();
}

void rrc_nr::configure_sk_counter(uint16_t sk_counter)
{
  log_h->info("[NR] Configure new SK counter %d. Update Key for secondary gnb", sk_counter);
  usim->generate_nr_context(sk_counter, &sec_cfg);
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
      add_lcid_rb(lc_ch_id, Drb, drb_id);
    } else {
      srb_id = rlc_bearer_cfg.served_radio_bearer.srb_id();
      add_lcid_rb(lc_ch_id, Srb, srb_id);
    }
  } else {
    log_h->warning("In RLC bearer cfg does not contain served radio bearer");
    return false;
  }

  if (rlc_bearer_cfg.rlc_cfg_present == true) {
    rlc_cfg = srslte::make_rlc_config_t(rlc_bearer_cfg.rlc_cfg);
    if (rlc_bearer_cfg.rlc_cfg.type() == asn1::rrc_nr::rlc_cfg_c::types::um_bi_dir) {
      if (rlc_bearer_cfg.rlc_cfg.um_bi_dir().dl_um_rlc.sn_field_len_present &&
          rlc_bearer_cfg.rlc_cfg.um_bi_dir().ul_um_rlc.sn_field_len_present &&
          rlc_bearer_cfg.rlc_cfg.um_bi_dir().dl_um_rlc.sn_field_len !=
              rlc_bearer_cfg.rlc_cfg.um_bi_dir().ul_um_rlc.sn_field_len) {
        log_h->warning("NR RLC sequence number length is not the same in uplink and downlink");
      }
    } else {
      log_h->warning("NR RLC type is not unacknowledged mode bidirectional");
    }
  } else {
    log_h->warning("In RLC bearer cfg does not contain rlc cfg");
    return false;
  }

  // Setup RLC
  rlc->add_bearer(lc_ch_id, rlc_cfg);

  if (rlc_bearer_cfg.mac_lc_ch_cfg_present == true && rlc_bearer_cfg.mac_lc_ch_cfg.ul_specific_params_present) {
    logical_channel_config_t logical_channel_cfg;
    logical_channel_cfg = srslte::make_mac_logical_channel_cfg_t(lc_ch_id, rlc_bearer_cfg.mac_lc_ch_cfg);
    mac->setup_lcid(logical_channel_cfg);
  }
  return true;
}
bool rrc_nr::apply_mac_cell_group(const mac_cell_group_cfg_s& mac_cell_group_cfg)
{
  if (mac_cell_group_cfg.sched_request_cfg_present) {
    sr_cfg_t sr_cfg;
    if (mac_cell_group_cfg.sched_request_cfg.sched_request_to_add_mod_list_present) {
      if (mac_cell_group_cfg.sched_request_cfg.sched_request_to_add_mod_list.size() > 1) {
        log_h->warning("Only handling 1 scheduling request index to add");
        sr_cfg.dsr_transmax = mac_cell_group_cfg.sched_request_cfg.sched_request_to_add_mod_list[1].sr_trans_max;
        mac->set_config(sr_cfg);
      }
    }

    if (mac_cell_group_cfg.sched_request_cfg.sched_request_to_release_list_present) {
      log_h->warning("Not handling sched request to release list");
    }
  }
  if (mac_cell_group_cfg.sched_request_cfg_present)

    if (mac_cell_group_cfg.bsr_cfg_present) {
      log_h->debug("Handling MAC BSR config");
      srslte::bsr_cfg_t bsr_cfg;
      bsr_cfg.periodic_timer = mac_cell_group_cfg.bsr_cfg.periodic_bsr_timer.to_number();
      bsr_cfg.retx_timer     = mac_cell_group_cfg.bsr_cfg.retx_bsr_timer.to_number();
      mac->set_config(bsr_cfg);
    }

  if (mac_cell_group_cfg.tag_cfg_present) {
    log_h->warning("Not handling tag cfg in MAC cell group config");
  }

  if (mac_cell_group_cfg.phr_cfg_present) {
    log_h->warning("Not handling phr cfg in MAC cell group config");
  }

  if (mac_cell_group_cfg.skip_ul_tx_dynamic) {
    log_h->warning("Not handling phr cfg in skip_ul_tx_dynamic cell group config");
  }
  return true;
}

bool rrc_nr::apply_sp_cell_cfg(const sp_cell_cfg_s& sp_cell_cfg)
{
  // TODO Setup PHY @andre and @phy interface?
  if (sp_cell_cfg.recfg_with_sync_present) {
    const recfg_with_sync_s& recfg_with_sync = sp_cell_cfg.recfg_with_sync;
    mac->set_crnti(recfg_with_sync.new_ue_id);
  }
  return true;
}

bool rrc_nr::apply_cell_group_cfg(const cell_group_cfg_s& cell_group_cfg)
{
  if (cell_group_cfg.rlc_bearer_to_add_mod_list_present == true) {
    for (uint32_t i = 0; i < cell_group_cfg.rlc_bearer_to_add_mod_list.size(); i++) {
      apply_rlc_add_mod(cell_group_cfg.rlc_bearer_to_add_mod_list[i]);
    }
  }
  if (cell_group_cfg.mac_cell_group_cfg_present == true) {
    apply_mac_cell_group(cell_group_cfg.mac_cell_group_cfg);
  }
  if (cell_group_cfg.phys_cell_group_cfg_present == true) {
    log_h->warning("Not handling physical cell group config");
  }
  if (cell_group_cfg.sp_cell_cfg_present) {
    apply_sp_cell_cfg(cell_group_cfg.sp_cell_cfg);
  }
  return true;
}

bool rrc_nr::apply_drb_add_mod(const drb_to_add_mod_s& drb_cfg)
{
  if (!drb_cfg.pdcp_cfg_present) {
    log_h->error("Cannot add DRB - incomplete configuration");
    return false;
  }

  uint32_t lcid = get_lcid_for_rbid(drb_cfg.drb_id);

  // Setup PDCP
  if (!(drb_cfg.pdcp_cfg.drb_present == true)) {
    log_h->error("PDCP config does not contain DRB config");
    return false;
  }

  if (!(drb_cfg.cn_assoc_present == true)) {
    log_h->error("DRB config does not contain an associated cn");
    return false;
  }

  if (!(drb_cfg.cn_assoc.type() == drb_to_add_mod_s::cn_assoc_c_::types_opts::eps_bearer_id)) {
    log_h->error("CN associtaion type not supported %s ", drb_cfg.cn_assoc.type().to_string().c_str());
    return false;
  }
  uint32_t eps_bearer_id            = drb_cfg.cn_assoc.eps_bearer_id();
  drb_eps_bearer_id[drb_cfg.drb_id] = eps_bearer_id;

  if (drb_cfg.pdcp_cfg.drb.pdcp_sn_size_dl_present && drb_cfg.pdcp_cfg.drb.pdcp_sn_size_ul_present &&
      (drb_cfg.pdcp_cfg.drb.pdcp_sn_size_ul.to_number() != drb_cfg.pdcp_cfg.drb.pdcp_sn_size_dl.to_number())) {
    log_h->warning("PDCP SN size in UL and DL are not the same. make_drb_pdcp_config_t will use the DL SN size %d ",
                   drb_cfg.pdcp_cfg.drb.pdcp_sn_size_dl.to_number());
  }

  srslte::pdcp_config_t pdcp_cfg = make_drb_pdcp_config_t(drb_cfg.drb_id, true, drb_cfg.pdcp_cfg);
  pdcp->add_bearer(lcid, pdcp_cfg);
  gw->update_lcid(eps_bearer_id, lcid);
  return true;
}

bool rrc_nr::apply_security_cfg(const security_cfg_s& security_cfg)
{
  // TODO derive correct keys
  if (security_cfg.key_to_use_present) {
    if (security_cfg.key_to_use.value != security_cfg_s::key_to_use_opts::options::secondary) {
      log_h->warning("Only secondary key supported yet");
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
        log_h->warning("Unsupported algorithm");
        break;
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
          log_h->warning("Unsupported algorithm");
          break;
      }
    }
    usim->update_nr_context(&sec_cfg);
  }

  // Apply security config for all known NR lcids
  for (auto& lcid : lcid_rb) {
    pdcp->config_security(lcid.first, sec_cfg);
    pdcp->enable_encryption(lcid.first);
  }
  return true;
}

bool rrc_nr::apply_radio_bearer_cfg(const radio_bearer_cfg_s& radio_bearer_cfg)
{
  if (radio_bearer_cfg.drb_to_add_mod_list_present) {
    for (uint32_t i = 0; i < radio_bearer_cfg.drb_to_add_mod_list.size(); i++) {
      apply_drb_add_mod(radio_bearer_cfg.drb_to_add_mod_list[i]);
    }
  }
  if (radio_bearer_cfg.security_cfg_present) {
    apply_security_cfg(radio_bearer_cfg.security_cfg);
  }
  return true;
}
// RLC interface
void rrc_nr::max_retx_attempted() {}

// STACK interface
void rrc_nr::cell_search_completed(const rrc_interface_phy_lte::cell_search_ret_t& cs_ret, const phy_cell_t& found_cell)
{}

/* Procedures */
rrc_nr::connection_reconf_no_ho_proc::connection_reconf_no_ho_proc(rrc_nr* parent_) : rrc_ptr(parent_) {}

proc_outcome_t rrc_nr::connection_reconf_no_ho_proc::init(const bool                       endc_release_and_add_r15,
                                                          const asn1::rrc_nr::rrc_recfg_s& rrc_recfg,
                                                          const asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg,
                                                          bool                                  sk_counter_r15_present,
                                                          const uint32_t                        sk_counter_r15,
                                                          const asn1::rrc_nr::radio_bearer_cfg_s& radio_bearer_cfg)
{
  Info("Starting...");

  Info("Applying Cell Group Cfg");
  if (!rrc_ptr->apply_cell_group_cfg(cell_group_cfg)) {
    return proc_outcome_t::error;
  }

  if (sk_counter_r15_present) {
    Info("Applying Cell Group Cfg");
    rrc_ptr->configure_sk_counter((uint16_t)sk_counter_r15);
  }

  Info("Applying Radio Bearer Cfg");
  if (!rrc_ptr->apply_radio_bearer_cfg(radio_bearer_cfg)) {
    return proc_outcome_t::error;
  }
  return proc_outcome_t::success;
}

proc_outcome_t rrc_nr::connection_reconf_no_ho_proc::react(const bool& config_complete)
{
  if (not config_complete) {
    Error("Failed to config PHY");
    return proc_outcome_t::error;
  }

  rrc_ptr->rrc_eutra->nr_rrc_con_reconfig_complete(true);

  Info("Reconfig NR return successful");
  return proc_outcome_t::success;
}

void rrc_nr::connection_reconf_no_ho_proc::then(const srslte::proc_state_t& result)
{
  if (result.is_success()) {
    Info("Finished %s successfully", name());
    srslte::console("RRC NR reconfiguration successful.");
    return;
  }

  // Section 5.3.5.5 - Reconfiguration failure
  // rrc_ptr->con_reconfig_failed();
}

} // namespace srsue
