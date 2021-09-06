/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

rrc::ue::rrc_endc::rrc_endc(rrc::ue* outer_ue) :
  base_t(outer_ue->parent->logger), rrc_ue(outer_ue), rrc_enb(outer_ue->parent), logger(outer_ue->parent->logger)
{}

//! Method to add NR fields to a RRC Connection Reconfiguration Message
bool rrc::ue::rrc_endc::fill_conn_recfg(asn1::rrc::rrc_conn_recfg_r8_ies_s* conn_recfg)
{
  if (not endc_supported) {
    // skipping ENDC-related field
    return false;
  }

  if (not is_endc_activation_running()) {
    // add hard-coded measConfig
    conn_recfg->meas_cfg_present = true;
    meas_cfg_s& meas_cfg         = conn_recfg->meas_cfg;

    meas_cfg.meas_obj_to_add_mod_list_present = true;

    meas_obj_to_add_mod_s meas_obj = {};
    meas_obj.meas_obj_id           = meas_cfg.meas_obj_to_add_mod_list.size() + 1;
    meas_obj.meas_obj.set_meas_obj_eutra();
    meas_obj.meas_obj.meas_obj_eutra().carrier_freq       = 300;
    meas_obj.meas_obj.meas_obj_eutra().allowed_meas_bw    = allowed_meas_bw_opts::mbw50;
    meas_obj.meas_obj.meas_obj_eutra().presence_ant_port1 = false;
    meas_obj.meas_obj.meas_obj_eutra().neigh_cell_cfg.from_number(0b01);
    meas_cfg.meas_obj_to_add_mod_list.push_back(meas_obj);

    meas_obj_to_add_mod_s meas_obj2 = {};
    meas_obj2.meas_obj_id           = meas_cfg.meas_obj_to_add_mod_list.size() + 1;
    meas_obj2.meas_obj.set_meas_obj_nr_r15();
    meas_obj2.meas_obj.meas_obj_nr_r15().carrier_freq_r15 = 634176;
    meas_obj2.meas_obj.meas_obj_nr_r15().rs_cfg_ssb_r15.meas_timing_cfg_r15.periodicity_and_offset_r15.set_sf20_r15();
    meas_obj2.meas_obj.meas_obj_nr_r15().rs_cfg_ssb_r15.meas_timing_cfg_r15.ssb_dur_r15 =
        asn1::rrc::mtc_ssb_nr_r15_s::ssb_dur_r15_opts::sf1;
    meas_obj2.meas_obj.meas_obj_nr_r15().rs_cfg_ssb_r15.subcarrier_spacing_ssb_r15 =
        asn1::rrc::rs_cfg_ssb_nr_r15_s::subcarrier_spacing_ssb_r15_opts::khz30;
    meas_obj2.meas_obj.meas_obj_nr_r15().ext = true;
    meas_obj2.meas_obj.meas_obj_nr_r15().band_nr_r15.set_present(true);
    meas_obj2.meas_obj.meas_obj_nr_r15().band_nr_r15.get()->set_setup() = 78;
    meas_cfg.meas_obj_to_add_mod_list.push_back(meas_obj2);

    // report config
    meas_cfg.report_cfg_to_add_mod_list_present = true;
    report_cfg_to_add_mod_s report_cfg          = {};

    report_cfg.report_cfg_id = meas_cfg.report_cfg_to_add_mod_list.size() + 1;
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

    report_cfg.report_cfg.report_cfg_inter_rat().max_report_cells = 1;
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
    meas_id_to_add_mod_s meas_id             = {};
    meas_id.meas_id                          = meas_obj.meas_obj_id;
    meas_id.meas_obj_id                      = meas_obj2.meas_obj_id;
    meas_id.report_cfg_id                    = report_cfg.report_cfg_id;
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
    meas_cfg.meas_gap_cfg_present = true;
    meas_cfg.meas_gap_cfg.set_setup();
    meas_cfg.meas_gap_cfg.setup().gap_offset.set_gp0() = 16;
  } else if (is_in_state<prepare_recfg_st>()) {
    // FIXME: use bearer manager to remove EUTRA DRB
    conn_recfg->rr_cfg_ded.drb_to_release_list_present = true;
    conn_recfg->rr_cfg_ded.drb_to_release_list.resize(1);
    conn_recfg->rr_cfg_ded.drb_to_release_list[0] = 1;

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
    reconf_v1510.nr_cfg_r15_present     = true;
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
  if (rrc_enb->cfg.cell_list_nr.empty()) {
    Debug("Skipping UE capabilities. No NR cell configured.");
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
                                endc_supported = true;
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
}

//! Method called whenever the eNB receives a MeasReport from the UE
void rrc::ue::rrc_endc::handle_ue_meas_report(const meas_report_s& msg)
{
  // Ignore event if ENDC isn't supported
  if (not endc_supported) {
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

  // Start EN-DC activation using EPS bearer of EUTRA DRB1
  rrc_nr_interface_rrc::sgnb_addition_req_params_t params = {};
  params.eps_bearer_id =
      rrc_enb->bearer_manager.get_lcid_bearer(rrc_ue->rnti, drb_to_lcid((lte_drb)eutra_drb_id)).eps_bearer_id;
  logger.info("Triggering SgNB addition");
  rrc_enb->rrc_nr->sgnb_addition_request(rrc_ue->rnti, params);

  sgnb_add_req_sent_ev sgnb_add_req{};
  trigger(sgnb_add_req);
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

  // delete EPS bearer mapping over EUTRA PDCP
  rrc_enb->bearer_manager.remove_eps_bearer(rrc_ue->rnti, ev.params.eps_bearer_id);

  // re-register EPS bearer over NR PDCP
  rrc_enb->bearer_manager.add_eps_bearer(
      ev.params.nr_rnti, ev.params.eps_bearer_id, srsran::srsran_rat_t::nr, lcid_drb_nr);

  // change GTPU tunnel RNTI to match NR RNTI
  rrc_enb->gtpu->mod_bearer_rnti(rrc_ue->rnti, ev.params.nr_rnti);
}

bool rrc::ue::rrc_endc::is_endc_supported()
{
  return endc_supported;
}

} // namespace srsenb
