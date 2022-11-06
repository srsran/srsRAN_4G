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

#include "srsenb/hdr/stack/rrc/rrc_endc.h"

namespace srsenb {

#define Info(fmt, ...) logger.info("ENDC: " fmt, ##__VA_ARGS__)
#define Error(fmt, ...) logger.error("ENDC: " fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) logger.warning("ENDC: " fmt, ##__VA_ARGS__)
#define Debug(fmt, ...) logger.debug("ENDC: " fmt, ##__VA_ARGS__)

#define procInfo(fmt, ...) parent->logger.info("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)
#define procWarning(fmt, ...) parent->logger.warning("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)
#define procError(fmt, ...) parent->logger.error("Proc \"%s\" - " fmt, name(), ##__VA_ARGS__)

using namespace asn1::rrc;

/*************************************************************************************************
 *                                  rrc_endc class
 ************************************************************************************************/

rrc::ue::rrc_endc::rrc_endc(rrc::ue* outer_ue, const rrc_endc_cfg_t& endc_cfg_) :
  base_t(outer_ue->parent->logger),
  rrc_ue(outer_ue),
  rrc_enb(outer_ue->parent),
  logger(outer_ue->parent->logger),
  endc_cfg(endc_cfg_)
{
  // start SgNB activation if B1 events are disabled
  if (endc_cfg.act_from_b1_event == false) {
    start_sgnb_addition();
  }
}

rrc::ue::rrc_endc::~rrc_endc()
{
  start_sgnb_release();
}

//! Method to add NR fields to a RRC Connection Reconfiguration Message
bool rrc::ue::rrc_endc::fill_conn_recfg(asn1::rrc::rrc_conn_recfg_r8_ies_s* conn_recfg)
{
  if (not is_endc_supported()) {
    // skipping ENDC-related field
    return false;
  }

  if (not is_endc_activation_running() && endc_cfg.act_from_b1_event) {
    // add measConfig
    conn_recfg->meas_cfg_present = true;
    meas_cfg_s& meas_cfg         = conn_recfg->meas_cfg;

    meas_cfg.meas_obj_to_add_mod_list_present = true;

    // store id of nr meas object to remove it in second reconfiguration message
    nr_meas_obj_id = meas_cfg.meas_obj_to_add_mod_list.size() + 1;

    meas_obj_to_add_mod_s meas_obj = {};
    meas_obj.meas_obj_id           = nr_meas_obj_id;
    meas_obj.meas_obj.set_meas_obj_nr_r15();
    auto& meas_obj_nr                                                         = meas_obj.meas_obj.meas_obj_nr_r15();
    meas_obj_nr.carrier_freq_r15                                              = endc_cfg.abs_frequency_ssb;
    meas_obj_nr.rs_cfg_ssb_r15.meas_timing_cfg_r15.periodicity_and_offset_r15 = endc_cfg.ssb_period_offset;
    meas_obj_nr.rs_cfg_ssb_r15.meas_timing_cfg_r15.ssb_dur_r15                = endc_cfg.ssb_duration;
    meas_obj_nr.rs_cfg_ssb_r15.subcarrier_spacing_ssb_r15                     = endc_cfg.ssb_ssc;
    meas_obj_nr.ext                                                           = true;
    meas_obj_nr.band_nr_r15.set_present(true);
    meas_obj_nr.band_nr_r15.get()->set_setup() = endc_cfg.nr_band;
    meas_cfg.meas_obj_to_add_mod_list.push_back(meas_obj);

    // report config
    meas_cfg.report_cfg_to_add_mod_list_present = true;

    // store id of nr report config to remove it in second reconfiguration message
    nr_report_cfg_id = meas_cfg.report_cfg_to_add_mod_list.size() + 1;

    report_cfg_to_add_mod_s report_cfg = {};
    report_cfg.report_cfg_id           = nr_report_cfg_id;
    report_cfg.report_cfg.set_report_cfg_inter_rat();
    report_cfg.report_cfg.report_cfg_inter_rat().trigger_type.set_event();
    report_cfg.report_cfg.report_cfg_inter_rat().trigger_type.event().event_id.set_event_b1_nr_r15();
    report_cfg.report_cfg.report_cfg_inter_rat()
        .trigger_type.event()
        .event_id.event_b1_nr_r15()
        .b1_thres_nr_r15.set_nr_rsrp_r15();
    report_cfg.report_cfg.report_cfg_inter_rat()
        .trigger_type.event()
        .event_id.event_b1_nr_r15()
        .b1_thres_nr_r15.nr_rsrp_r15() = 56;
    report_cfg.report_cfg.report_cfg_inter_rat().trigger_type.event().event_id.event_b1_nr_r15().report_on_leave_r15 =
        false;
    report_cfg.report_cfg.report_cfg_inter_rat().trigger_type.event().hysteresis      = 0;
    report_cfg.report_cfg.report_cfg_inter_rat().trigger_type.event().time_to_trigger = time_to_trigger_opts::ms100;

    report_cfg.report_cfg.report_cfg_inter_rat().max_report_cells = 8;
    report_cfg.report_cfg.report_cfg_inter_rat().report_interv    = report_interv_opts::ms120;
    report_cfg.report_cfg.report_cfg_inter_rat().report_amount    = report_cfg_inter_rat_s::report_amount_opts::r1;
    report_cfg.report_cfg.report_cfg_inter_rat().report_quant_cell_nr_r15.set_present(true);
    report_cfg.report_cfg.report_cfg_inter_rat().ext                                     = true;
    report_cfg.report_cfg.report_cfg_inter_rat().report_quant_cell_nr_r15.get()->ss_rsrp = true;
    report_cfg.report_cfg.report_cfg_inter_rat().report_quant_cell_nr_r15.get()->ss_rsrq = true;
    report_cfg.report_cfg.report_cfg_inter_rat().report_quant_cell_nr_r15.get()->ss_sinr = true;
    meas_cfg.report_cfg_to_add_mod_list.push_back(report_cfg);

    // measIdToAddModList
    meas_cfg.meas_id_to_add_mod_list_present = true;

    // store id of nr meas to remove it in second reconfiguration message
    nr_meas_id = meas_cfg.meas_id_to_add_mod_list.size() + 1;

    meas_id_to_add_mod_s meas_id = {};
    meas_id.meas_id              = nr_meas_id;
    meas_id.meas_obj_id          = meas_obj.meas_obj_id;
    meas_id.report_cfg_id        = report_cfg.report_cfg_id;
    meas_cfg.meas_id_to_add_mod_list.push_back(meas_id);

    // quantityConfig
    meas_cfg.quant_cfg_present                 = true;
    meas_cfg.quant_cfg.quant_cfg_eutra_present = true;
    meas_cfg.quant_cfg.quant_cfg_nr_list_r15.set_present(true);
    meas_cfg.quant_cfg.quant_cfg_nr_list_r15.get()->resize(1);
    meas_cfg.quant_cfg.ext = true;
    auto& meas_quant       = meas_cfg.quant_cfg.quant_cfg_nr_list_r15.get()[0];
    meas_quant[0].meas_quant_cell_nr_r15.filt_coeff_rsrp_r15_present = true;
    meas_quant[0].meas_quant_cell_nr_r15.filt_coeff_rsrp_r15         = filt_coef_opts::fc3;

    // measGapConfig
    meas_cfg.meas_gap_cfg_present = false; // No LTE measGaps allowed while in NSA mode
    meas_cfg.meas_gap_cfg.set_setup();
    meas_cfg.meas_gap_cfg.setup().gap_offset.set_gp0() = 16;
  } else if (is_in_state<prepare_recfg_st>()) {
    // Deactivate measurement reports, as we do not support measurements after NR activation
    conn_recfg->meas_cfg_present = true;
    meas_cfg_s& meas_cfg         = conn_recfg->meas_cfg;
    // Remove meas config
    meas_cfg.meas_obj_to_rem_list_present = true;
    meas_cfg.meas_obj_to_rem_list.resize(1);
    meas_cfg.meas_obj_to_rem_list[0] = nr_meas_obj_id;
    // remove report config
    meas_cfg.report_cfg_to_rem_list_present = true;
    meas_cfg.report_cfg_to_rem_list.resize(1);
    meas_cfg.report_cfg_to_rem_list[0] = nr_report_cfg_id;
    // remove meas id
    meas_cfg.meas_id_to_rem_list_present = true;
    meas_cfg.meas_id_to_rem_list.resize(1);
    meas_cfg.meas_id_to_rem_list[0] = nr_meas_id;

    // TODO: use bearer manager to remove EUTRA DRB
    conn_recfg->rr_cfg_ded.drb_to_release_list_present = true;
    conn_recfg->rr_cfg_ded.drb_to_release_list.resize(1);
    conn_recfg->rr_cfg_ded.drb_to_release_list[0] = 1;

    // don't send EUTRA dedicated config again
    conn_recfg->rr_cfg_ded.phys_cfg_ded_present = false;

    // set MAC main config dedicated
    conn_recfg->rr_cfg_ded.mac_main_cfg_present = true;
    conn_recfg->rr_cfg_ded.mac_main_cfg.set_explicit_value();

    auto& mac_main_cfg = conn_recfg->rr_cfg_ded.mac_main_cfg.explicit_value();

    mac_main_cfg.time_align_timer_ded = time_align_timer_opts::infinity;
    mac_main_cfg.phr_cfg_present      = true;
    mac_main_cfg.phr_cfg.set_setup();
    mac_main_cfg.phr_cfg.setup().dl_pathloss_change =
        asn1::rrc::mac_main_cfg_s::phr_cfg_c_::setup_s_::dl_pathloss_change_opts::db3;
    mac_main_cfg.phr_cfg.setup().periodic_phr_timer =
        asn1::rrc::mac_main_cfg_s::phr_cfg_c_::setup_s_::periodic_phr_timer_opts::sf500;
    mac_main_cfg.phr_cfg.setup().prohibit_phr_timer =
        asn1::rrc::mac_main_cfg_s::phr_cfg_c_::setup_s_::prohibit_phr_timer_opts::sf200;

    // Disable DC-PHR reporting
    mac_main_cfg.ext = false;
    mac_main_cfg.mac_main_cfg_v1020.set_present();
    mac_main_cfg.dual_connect_phr.set_present();
    mac_main_cfg.dual_connect_phr.get()->set_setup();
    mac_main_cfg.dual_connect_phr.get()->setup().phr_mode_other_cg_r12 =
        asn1::rrc::mac_main_cfg_s::dual_connect_phr_c_::setup_s_::phr_mode_other_cg_r12_opts::real;

    // only add reconfigure EN-DC extension/release 15.10 field if ENDC activation is active
    conn_recfg->non_crit_ext_present                                                                  = true;
    conn_recfg->non_crit_ext.non_crit_ext_present                                                     = true;
    conn_recfg->non_crit_ext.non_crit_ext.non_crit_ext_present                                        = true;
    conn_recfg->non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext_present                           = true;
    conn_recfg->non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext_present              = true;
    conn_recfg->non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext_present = true;
    conn_recfg->non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext_present =
        true;
    conn_recfg->non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext
        .non_crit_ext_present                = true;
    rrc_conn_recfg_v1510_ies_s& reconf_v1510 = conn_recfg->non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext
                                                   .non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext;
    reconf_v1510.nr_cfg_r15_present = true;
    reconf_v1510.nr_cfg_r15.set_setup();

    reconf_v1510.nr_cfg_r15.setup().endc_release_and_add_r15                = false;
    reconf_v1510.nr_cfg_r15.setup().nr_secondary_cell_group_cfg_r15_present = true;
    reconf_v1510.nr_cfg_r15.setup().nr_secondary_cell_group_cfg_r15 =
        get_state<prepare_recfg_st>()->sgnb_config.nr_secondary_cell_group_cfg_r15;

    reconf_v1510.sk_counter_r15_present = true;
    reconf_v1510.sk_counter_r15         = 0;

    reconf_v1510.nr_radio_bearer_cfg1_r15_present = true;
    reconf_v1510.nr_radio_bearer_cfg1_r15         = get_state<prepare_recfg_st>()->sgnb_config.nr_radio_bearer_cfg1_r15;

    // inform FSM
    rrc_recfg_sent_ev recfg_sent{};
    trigger(recfg_sent);
  }

  return true;
}

//! Called when UE capabilities are received
void rrc::ue::rrc_endc::handle_eutra_capabilities(const asn1::rrc::ue_eutra_cap_s& eutra_caps)
{
  // skip any further checks if eNB runs without NR cells
  if (rrc_enb->cfg.num_nr_cells == 0) {
    Debug("Skipping UE capabilities. No NR cell configured.");
    trigger(disable_endc_ev{});
    return;
  }

  // Only enabled ENDC support if UE caps have been exchanged and UE signals support
  if (eutra_caps.non_crit_ext_present) {
    if (eutra_caps.non_crit_ext.non_crit_ext_present) {
      if (eutra_caps.non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext_present) {
        if (eutra_caps.non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext_present) {
          auto& ue_cap_v1170 =
              eutra_caps.non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext;
          if (ue_cap_v1170.non_crit_ext_present) {
            if (ue_cap_v1170.non_crit_ext.non_crit_ext.non_crit_ext_present) {
              if (ue_cap_v1170.non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext_present) {
                if (ue_cap_v1170.non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext
                        .non_crit_ext_present) {
                  if (ue_cap_v1170.non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext
                          .non_crit_ext.non_crit_ext_present) {
                    auto& ue_cap_v1330 = ue_cap_v1170.non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext
                                             .non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext;
                    if (ue_cap_v1330.non_crit_ext_present) {
                      if (ue_cap_v1330.non_crit_ext.non_crit_ext.non_crit_ext_present) {
                        if (ue_cap_v1330.non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext_present) {
                          if (ue_cap_v1330.non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext
                                  .non_crit_ext_present) {
                            auto& ue_cap_v1510 = ue_cap_v1330.non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext
                                                     .non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext;
                            if (ue_cap_v1510.irat_params_nr_r15_present) {
                              if (ue_cap_v1510.irat_params_nr_r15.en_dc_r15_present) {
                                logger.info("Enabling ENDC support for rnti=%d", rrc_ue->rnti);
                                return;
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  trigger(disable_endc_ev{});
}

//! Method called whenever the eNB receives a MeasReport from the UE
void rrc::ue::rrc_endc::handle_ue_meas_report(const meas_report_s& msg)
{
  // Ignore event if ENDC isn't supported
  if (not is_endc_supported()) {
    return;
  }

  if (not is_in_state<endc_deactivated_st>()) {
    Info("Received a MeasReport while already enabling ENDC support. Ignoring...");
    return;
  }
  // Check if meas_id is valid
  const meas_results_s& meas_res = msg.crit_exts.c1().meas_report_r8().meas_results;
  if (not meas_res.meas_result_neigh_cells_present) {
    Info("Received a MeasReport, but the UE did not detect any cell.");
    return;
  }

  // only handle NR cells here, EUTRA is handled in mobility class
  if (meas_res.meas_result_neigh_cells.type().value !=
      meas_results_s::meas_result_neigh_cells_c_::types::meas_result_neigh_cell_list_nr_r15) {
    return;
  }

  start_sgnb_addition();
}

void rrc::ue::rrc_endc::start_sgnb_addition()
{
  // Start EN-DC activation using EPS bearer of EUTRA DRB1
  rrc_nr_interface_rrc::sgnb_addition_req_params_t params = {};

  const auto& drb_list = rrc_ue->bearer_list.get_established_drbs();
  if (drb_list.size() > 0) {
    // move first establised DRB to NR cell
    const auto& drb1      = drb_list[0];
    const auto& erab_list = rrc_ue->bearer_list.get_erabs();
    auto        erab_it   = erab_list.find(drb1.eps_bearer_id);
    if (erab_it != erab_list.end()) {
      params.eps_bearer_id = drb1.eps_bearer_id;
      params.five_qi       = erab_it->second.qos_params.qci; // use QCI as 5QI
    } else {
      logger.error("Couldn't find ERAB config for DRB%d. Aborting SgNB addition for E-UTRA rnti=0x%x",
                   drb1.drb_id,
                   rrc_ue->rnti);
      return;
    }
  } else {
    logger.error("No LTE DRB established. Aborting SgNB addition for E-UTRA rnti=0x%x", rrc_ue->rnti);
    return;
  }

  logger.info("Triggering SgNB addition for E-UTRA rnti=0x%x", rrc_ue->rnti);
  rrc_enb->rrc_nr->sgnb_addition_request(rrc_ue->rnti, params);

  sgnb_add_req_sent_ev sgnb_add_req{};
  trigger(sgnb_add_req);
}

void rrc::ue::rrc_endc::start_sgnb_release()
{
  sgnb_rel_req_ev sgnb_rel_req{nr_rnti};
  trigger(sgnb_rel_req);
}

rrc::ue::rrc_endc::prepare_recfg_st::prepare_recfg_st(rrc_endc* parent_) : logger(parent_->logger) {}

void rrc::ue::rrc_endc::prepare_recfg_st::enter(rrc_endc* f, const sgnb_add_req_ack_ev& ev)
{
  // store SgNB provided config
  sgnb_config = ev.params;

  logger.debug(sgnb_config.nr_secondary_cell_group_cfg_r15.data(),
               sgnb_config.nr_secondary_cell_group_cfg_r15.size(),
               "nr-SecondaryCellGroupConfig-r15:");
  logger.debug(sgnb_config.nr_radio_bearer_cfg1_r15.data(),
               sgnb_config.nr_radio_bearer_cfg1_r15.size(),
               "nr-RadioBearerConfig1-r15:");
}

// The gNB has accepted the SgNB addition and has already allocated the user and established all bearers
void rrc::ue::rrc_endc::handle_sgnb_add_req_ack(wait_sgnb_add_req_resp_st& s, const sgnb_add_req_ack_ev& ev)
{
  // TODO: copy buffered PDCP data to SeNB

  // TODO: path update procedure with GTPU modify bearer request (for mode 3A and 3X)

  // re-register EPS bearer over NR PDCP
  rrc_enb->bearer_manager.add_eps_bearer(
      ev.params.nr_rnti, ev.params.eps_bearer_id, srsran::srsran_rat_t::nr, lcid_drb_nr);

  // change GTPU tunnel RNTI to match NR RNTI
  rrc_enb->gtpu->mod_bearer_rnti(rrc_ue->rnti, ev.params.nr_rnti);

  // store RNTI for later
  nr_rnti = ev.params.nr_rnti;
}

void rrc::ue::rrc_endc::handle_sgnb_rel_req(const sgnb_rel_req_ev& ev)
{
  logger.info("Triggering SgNB release for E-UTRA rnti=0x%x", rrc_ue->rnti);
  rrc_enb->bearer_manager.rem_user(nr_rnti);
  rrc_enb->rrc_nr->sgnb_release_request(nr_rnti);
}

bool rrc::ue::rrc_endc::is_endc_supported()
{
  return not is_in_state<endc_disabled_st>();
}

void rrc::ue::rrc_endc::handle_rrc_reest(endc_activated_st& s, const rrc_reest_rx_ev& ev)
{
  // Transition GTPU tunnel rnti back from NR RNTI to LTE RNTI, given that the reconfiguration failed
  rrc_enb->gtpu->mod_bearer_rnti(nr_rnti, rrc_ue->rnti);
}

void rrc::ue::rrc_endc::handle_endc_disabled(const disable_endc_ev& ev)
{
  logger.info("Disabling NR EN-DC support for rnti=0x%x", nr_rnti);
}

bool rrc::ue::rrc_endc::requires_rel_req(const sgnb_rel_req_ev& ev)
{
  return not is_in_state<endc_disabled_st>() and not is_in_state<endc_deactivated_st>() and
         not is_in_state<wait_sgnb_rel_req_resp_st>();
}

} // namespace srsenb
