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
  if (not is_endc_activation_running()) {
    // TODO: add measConfig related field to enable measurements on NR carrier
    return false;
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
    return true;
  }
}

//! Method called whenever the eNB receives a MeasReport from the UE
void rrc::ue::rrc_endc::handle_ue_meas_report(const meas_report_s& msg, srsran::unique_byte_buffer_t pdu)
{
  // Start EN-DC activation
  logger.info("Triggering SgNB addition");
  rrc_enb->rrc_nr->sgnb_addition_request(rrc_ue->rnti);
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
