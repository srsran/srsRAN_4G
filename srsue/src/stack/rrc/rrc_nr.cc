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

using namespace asn1::rrc_nr;

namespace srsue {

const char* rrc_nr::rrc_nr_state_text[] = {"IDLE", "CONNECTED", "CONNECTED-INACTIVE"};

rrc_nr::rrc_nr(srslte::task_sched_handle task_sched_) : log_h("RRC"), task_sched(task_sched_) {}

rrc_nr::~rrc_nr() = default;

void rrc_nr::init(phy_interface_rrc_nr*       phy_,
                  mac_interface_rrc_nr*       mac_,
                  rlc_interface_rrc*          rlc_,
                  pdcp_interface_rrc*         pdcp_,
                  gw_interface_rrc*           gw_,
                  rrc_eutra_interface_rrc_nr* rrc_eutra_,
                  srslte::timer_handler*      timers_,
                  stack_interface_rrc*        stack_,
                  const rrc_nr_args_t&        args_)
{
  phy       = phy_;
  rlc       = rlc_;
  pdcp      = pdcp_;
  gw        = gw_;
  rrc_eutra = rrc_eutra_;
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
  log_h->info("Creating dummy DRB on LCID=%d\n", args.coreless.drb_lcid);
  srslte::rlc_config_t rlc_cnfg = srslte::rlc_config_t::default_rlc_um_nr_config(6);
  rlc->add_bearer(args.coreless.drb_lcid, rlc_cnfg);

  srslte::pdcp_config_t pdcp_cnfg{args.coreless.drb_lcid,
                                  srslte::PDCP_RB_IS_DRB,
                                  srslte::SECURITY_DIRECTION_DOWNLINK,
                                  srslte::SECURITY_DIRECTION_UPLINK,
                                  srslte::PDCP_SN_LEN_18,
                                  srslte::pdcp_t_reordering_t::ms500,
                                  srslte::pdcp_discard_timer_t ::ms100};

  pdcp->add_bearer(args.coreless.drb_lcid, pdcp_cnfg);
  return;
}
void rrc_nr::get_metrics(rrc_nr_metrics_t& m) {}

// Timeout callback interface
void rrc_nr::timer_expired(uint32_t timeout_id)
{
  log_h->debug("[NR] Handling Timer Expired\n");
  if (timeout_id == fake_measurement_timer.id()) {
    log_h->debug("[NR] Triggered Fake Measurement\n");

    phy_meas_nr_t              fake_meas = {};
    std::vector<phy_meas_nr_t> phy_meas_nr;
    fake_meas.rsrp     = -60.0;
    fake_meas.rsrq     = -60.0;
    fake_meas.cfo_hz   = 1.0;
    fake_meas.arfcn_nr = 632256;
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
    log_h->info("%s - %s %s (%d B)\n", source.c_str(), (dir == Rx) ? "Rx" : "Tx", msg_type.c_str(), pdu->N_bytes);
  } else if (log_h->get_level() >= srslte::LOG_LEVEL_DEBUG) {
    asn1::json_writer json_writer;
    msg.to_json(json_writer);
    log_h->debug_hex(pdu->msg,
                     pdu->N_bytes,
                     "%s - %s %s (%d B)\n",
                     source.c_str(),
                     (dir == Rx) ? "Rx" : "Tx",
                     msg_type.c_str(),
                     pdu->N_bytes);
    log_h->debug_long("Content:\n%s\n", json_writer.to_string().c_str());
  }
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
  band_param_eutra.eutra().band_eutra                   = 1;
  band_param_eutra.eutra().ca_bw_class_dl_eutra         = asn1::rrc_nr::ca_bw_class_eutra_opts::options::a;
  band_param_eutra.eutra().ca_bw_class_ul_eutra         = asn1::rrc_nr::ca_bw_class_eutra_opts::options::a;
  band_combination.band_list.push_back(band_param_eutra);

  struct band_params_c band_param_nr;
  band_param_nr.set_nr();
  band_param_nr.nr().ca_bw_class_dl_nr_present = true;
  band_param_nr.nr().ca_bw_class_ul_nr_present = true;
  band_param_nr.nr().band_nr                   = 78;
  band_param_nr.nr().ca_bw_class_dl_nr         = asn1::rrc_nr::ca_bw_class_nr_opts::options::a;
  band_param_nr.nr().ca_bw_class_ul_nr         = asn1::rrc_nr::ca_bw_class_nr_opts::options::a;
  band_combination.band_list.push_back(band_param_nr);

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

  freq_band_info_c band_info_nr;
  band_info_nr.set_band_info_nr();
  band_info_nr.band_info_nr().band_nr = 78;
  mrdc_cap.rf_params_mrdc.applied_freq_band_list_filt.push_back(band_info_nr);

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

  feature_set_c feature_set_nr;
  feature_set_nr.set_nr();
  feature_set_nr.nr().dl_set_nr = 1;
  feature_set_nr.nr().ul_set_nr = 1;
  feature_sets_per_band.push_back(feature_set_nr);

  feature_set_combination.push_back(feature_sets_per_band);

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
      eutra_nr_caps_pdu->msg, eutra_nr_caps_pdu->N_bytes, "EUTRA-NR capabilities (%u B)\n", eutra_nr_caps_pdu->N_bytes);

  return;
}

void rrc_nr::get_nr_capabilities(srslte::byte_buffer_t* nr_caps_pdu)
{

  struct ue_nr_cap_s nr_cap;

  nr_cap.access_stratum_release = access_stratum_release_opts::rel15;
  // PDCP
  nr_cap.pdcp_params.max_num_rohc_context_sessions = pdcp_params_s::max_num_rohc_context_sessions_opts::cs2;

  band_nr_s band_nr;
  band_nr.band_nr              = 78;
  band_nr.ue_pwr_class_present = true;
  band_nr.ue_pwr_class         = band_nr_s::ue_pwr_class_opts::pc3;
  nr_cap.rf_params.supported_band_list_nr.push_back(band_nr);

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

  log_h->debug_hex(nr_caps_pdu->msg, nr_caps_pdu->N_bytes, "NR capabilities (%u B)\n", nr_caps_pdu->N_bytes);
  return;
};

void rrc_nr::phy_set_cells_to_meas(uint32_t carrier_freq_r15)
{
  log_h->debug("[NR] Measuring phy cell %d \n", carrier_freq_r15);
  // Start timer for fake measurements
  auto timer_expire_func = [this](uint32_t tid) { timer_expired(tid); };
  fake_measurement_timer.set(10, timer_expire_func);
  fake_measurement_timer.run();
}

// RLC interface
void rrc_nr::max_retx_attempted() {}

// STACK interface
void rrc_nr::cell_search_completed(const rrc_interface_phy_lte::cell_search_ret_t& cs_ret, const phy_cell_t& found_cell)
{}

} // namespace srsue