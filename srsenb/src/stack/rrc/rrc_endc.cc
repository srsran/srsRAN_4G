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
    meas_cfg.meas_obj_to_add_mod_list.resize(2);

    auto& meas_obj       = meas_cfg.meas_obj_to_add_mod_list[0];
    meas_obj.meas_obj_id = 1;
    meas_obj.meas_obj.set_meas_obj_eutra();
    meas_obj.meas_obj.meas_obj_eutra().carrier_freq       = 300;
    meas_obj.meas_obj.meas_obj_eutra().allowed_meas_bw    = allowed_meas_bw_opts::mbw50;
    meas_obj.meas_obj.meas_obj_eutra().presence_ant_port1 = false;
    meas_obj.meas_obj.meas_obj_eutra().neigh_cell_cfg.from_number(0b01);

    auto& meas_obj2       = meas_cfg.meas_obj_to_add_mod_list[1];
    meas_obj2.meas_obj_id = 2;
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

    // report config
    meas_cfg.report_cfg_to_add_mod_list_present = true;
    meas_cfg.report_cfg_to_add_mod_list.resize(1);
    auto& report_cfg = meas_cfg.report_cfg_to_add_mod_list[0];

    report_cfg.report_cfg_id = 1;
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

    // measIdToAddModList
    meas_cfg.meas_id_to_add_mod_list_present = true;
    meas_cfg.meas_id_to_add_mod_list.resize(1);
    auto& meas_id         = meas_cfg.meas_id_to_add_mod_list[0];
    meas_id.meas_id       = 1;
    meas_id.meas_obj_id   = 2;
    meas_id.report_cfg_id = 1;

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
  } else {
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
    reconf_v1510.sk_counter_r15_present = true;
    reconf_v1510.sk_counter_r15         = 0;
  }

  return true;
}

//! Called when UE capabilities are received
void rrc::ue::rrc_endc::handle_ue_capabilities(const asn1::rrc::ue_eutra_cap_s& eutra_caps)
{
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

  if (not is_in_state<idle_st>()) {
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

  // Start EN-DC activation
  logger.info("Triggering SgNB addition");
  rrc_enb->rrc_nr->sgnb_addition_request(rrc_ue->rnti);

  sgnb_add_req_sent_ev sgnb_add_req{};
  trigger(sgnb_add_req);
}

void rrc::ue::rrc_endc::handle_sgnb_addition_ack(const asn1::dyn_octstring& nr_secondary_cell_group_cfg_r15,
                                                 const asn1::dyn_octstring& nr_radio_bearer_cfg1_r15)
{
  logger.info("Received SgNB addition acknowledgement for rnti=%d", rrc_ue->rnti);

  // prepare reconfiguration message with NR fields
  srsran::unique_byte_buffer_t pdu = srsran::make_byte_buffer();
  if (pdu == nullptr) {
    logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
    return;
  }
  // rrc_enb->send_connection_reconf(std::move(pdu));
}

void rrc::ue::rrc_endc::handle_sgnb_addition_reject()
{
  logger.error("Received SgNB addition reject for rnti=%d", rrc_ue->rnti);
}

void rrc::ue::rrc_endc::handle_recfg_complete(wait_recfg_comp& s, const recfg_complete_ev& ev)
{
  logger.info("User rnti=0x%x successfully enabled EN-DC", rrc_ue->rnti);
}

} // namespace srsenb
