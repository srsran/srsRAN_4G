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

#include "srsenb/hdr/stack/rrc/rrc_nr.h"
#include "srsenb/hdr/common/common_enb.h"
#include "srsenb/test/mac/nr/sched_nr_cfg_generators.h"
#include "srsran/asn1/rrc_nr_utils.h"
#include "srsran/common/band_helper.h"
#include "srsran/common/common_nr.h"
#include "srsran/common/phy_cfg_nr_default.h"
#include "srsran/common/standard_streams.h"

using namespace asn1::rrc_nr;

namespace srsenb {

rrc_nr::rrc_nr(srsran::task_sched_handle task_sched_) :
  logger(srslog::fetch_basic_logger("RRC-NR")), task_sched(task_sched_)
{}

int rrc_nr::init(const rrc_nr_cfg_t&         cfg_,
                 phy_interface_stack_nr*     phy_,
                 mac_interface_rrc_nr*       mac_,
                 rlc_interface_rrc*          rlc_,
                 pdcp_interface_rrc*         pdcp_,
                 ngap_interface_rrc_nr*      ngap_,
                 gtpu_interface_rrc_nr*      gtpu_,
                 rrc_eutra_interface_rrc_nr* rrc_eutra_)
{
  phy       = phy_;
  mac       = mac_;
  rlc       = rlc_;
  pdcp      = pdcp_;
  ngap      = ngap_;
  gtpu      = gtpu_;
  rrc_eutra = rrc_eutra_;

  // TODO: overwriting because we are not passing config right now
  cfg = update_default_cfg(cfg_);

  // derived
  slot_dur_ms = 1;

  if (generate_sibs() != SRSRAN_SUCCESS) {
    logger.error("Couldn't generate SIB messages.");
    return SRSRAN_ERROR;
  }

  // TODO: PHY isn't initialized at this stage yet
  // config_phy();
  config_mac();

  logger.info("Started");

  running = true;

  return SRSRAN_SUCCESS;
}

void rrc_nr::stop()
{
  if (running) {
    running = false;
  }
  users.clear();
}

template <class T>
void rrc_nr::log_rrc_message(const std::string&           source,
                             const direction_t            dir,
                             const srsran::byte_buffer_t* pdu,
                             const T&                     msg)
{
  if (logger.debug.enabled()) {
    asn1::json_writer json_writer;
    msg.to_json(json_writer);
    logger.debug(pdu->msg,
                 pdu->N_bytes,
                 "%s - %s %s (%d B)",
                 source.c_str(),
                 dir == Tx ? "Tx" : "Rx",
                 msg.msg.c1().type().to_string(),
                 pdu->N_bytes);
    logger.debug("Content:\n%s", json_writer.to_string().c_str());
  } else if (logger.info.enabled()) {
    logger.info(
        "%s - %s %s (%d B)", source.c_str(), dir == Tx ? "Tx" : "Rx", msg.msg.c1().type().to_string(), pdu->N_bytes);
  }
}

rrc_nr_cfg_t rrc_nr::update_default_cfg(const rrc_nr_cfg_t& current)
{
  // NOTE: This function is temporary.
  rrc_nr_cfg_t cfg_default = current;

  // Fill MIB
  cfg_default.mib.sub_carrier_spacing_common.value = mib_s::sub_carrier_spacing_common_opts::scs15or60;
  cfg_default.mib.ssb_subcarrier_offset            = 0;
  cfg_default.mib.intra_freq_resel.value           = mib_s::intra_freq_resel_opts::allowed;
  cfg_default.mib.cell_barred.value                = mib_s::cell_barred_opts::not_barred;
  cfg_default.mib.pdcch_cfg_sib1.search_space_zero = 0;
  cfg_default.mib.pdcch_cfg_sib1.ctrl_res_set_zero = 0;
  cfg_default.mib.dmrs_type_a_position.value       = mib_s::dmrs_type_a_position_opts::pos2;
  cfg_default.mib.sys_frame_num.from_number(0);

  // Fill SIB1
  cfg_default.sib1.cell_access_related_info.plmn_id_list.resize(1);
  cfg_default.sib1.cell_access_related_info.plmn_id_list[0].plmn_id_list.resize(1);
  srsran::plmn_id_t plmn;
  plmn.from_string("90170");
  srsran::to_asn1(&cfg_default.sib1.cell_access_related_info.plmn_id_list[0].plmn_id_list[0], plmn);
  cfg_default.sib1.cell_access_related_info.plmn_id_list[0].cell_id.from_number(1);
  cfg_default.sib1.cell_access_related_info.plmn_id_list[0].cell_reserved_for_oper.value =
      plmn_id_info_s::cell_reserved_for_oper_opts::not_reserved;
  cfg_default.sib1.si_sched_info_present                                  = true;
  cfg_default.sib1.si_sched_info.si_request_cfg.rach_occasions_si_present = true;
  cfg_default.sib1.si_sched_info.si_request_cfg.rach_occasions_si.rach_cfg_si.ra_resp_win.value =
      rach_cfg_generic_s::ra_resp_win_opts::sl8;
  cfg_default.sib1.si_sched_info.si_win_len.value = si_sched_info_s::si_win_len_opts::s20;
  cfg_default.sib1.si_sched_info.sched_info_list.resize(1);
  cfg_default.sib1.si_sched_info.sched_info_list[0].si_broadcast_status.value =
      sched_info_s::si_broadcast_status_opts::broadcasting;
  cfg_default.sib1.si_sched_info.sched_info_list[0].si_periodicity.value = sched_info_s::si_periodicity_opts::rf16;
  cfg_default.sib1.si_sched_info.sched_info_list[0].sib_map_info.resize(1);
  // scheduling of SI messages
  cfg_default.sib1.si_sched_info.sched_info_list[0].sib_map_info[0].type.value = sib_type_info_s::type_opts::sib_type2;
  cfg_default.sib1.si_sched_info.sched_info_list[0].sib_map_info[0].value_tag_present = true;
  cfg_default.sib1.si_sched_info.sched_info_list[0].sib_map_info[0].value_tag         = 0;

  // Fill SIB2+
  cfg_default.nof_sibs                     = 1;
  sib2_s& sib2                             = cfg_default.sibs[0].set_sib2();
  sib2.cell_resel_info_common.q_hyst.value = sib2_s::cell_resel_info_common_s_::q_hyst_opts::db5;
  // TODO: Fill SIB2 values

  return cfg_default;
}

// This function is called from PRACH worker (can wait)
int rrc_nr::add_user(uint16_t rnti)
{
  if (users.count(rnti) == 0) {
    users.insert(std::make_pair(rnti, std::unique_ptr<ue>(new ue(this, rnti))));
    rlc->add_user(rnti);
    pdcp->add_user(rnti);
    logger.info("Added new user rnti=0x%x", rnti);
    return SRSRAN_SUCCESS;
  } else {
    logger.error("Adding user rnti=0x%x (already exists)", rnti);
    return SRSRAN_ERROR;
  }
}

void rrc_nr::rem_user(uint16_t rnti)
{
  auto user_it = users.find(rnti);
  if (user_it != users.end()) {
    // First remove MAC and GTPU to stop processing DL/UL traffic for this user
    mac->remove_ue(rnti); // MAC handles PHY
    rlc->rem_user(rnti);
    pdcp->rem_user(rnti);
    users.erase(rnti);

    srsran::console("Disconnecting rnti=0x%x.\n", rnti);
    logger.info("Removed user rnti=0x%x", rnti);
  } else {
    logger.error("Removing user rnti=0x%x (does not exist)", rnti);
  }
}

/* Function called by MAC after the reception of a C-RNTI CE indicating that the UE still has a
 * valid RNTI.
 */
int rrc_nr::update_user(uint16_t new_rnti, uint16_t old_rnti)
{
  if (new_rnti == old_rnti) {
    logger.error("rnti=0x%x received MAC CRNTI CE with same rnti");
    return SRSRAN_ERROR;
  }

  // Remove new_rnti
  auto new_ue_it = users.find(new_rnti);
  if (new_ue_it != users.end()) {
    task_sched.defer_task([this, new_rnti]() { rem_user(new_rnti); });
  }

  // Send Reconfiguration to old_rnti if is RRC_CONNECT or RRC Release if already released here
  auto old_it = users.find(old_rnti);
  if (old_it == users.end()) {
    logger.info("rnti=0x%x received MAC CRNTI CE: 0x%x, but old context is unavailable", new_rnti, old_rnti);
    return SRSRAN_ERROR;
  }
  ue* ue_ptr = old_it->second.get();

  logger.info("Resuming rnti=0x%x RRC connection due to received C-RNTI CE from rnti=0x%x.", old_rnti, new_rnti);
  ue_ptr->crnti_ce_received();

  return SRSRAN_SUCCESS;
}

void rrc_nr::set_activity_user(uint16_t rnti)
{
  auto it = users.find(rnti);
  if (it == users.end()) {
    logger.info("rnti=0x%x not found. Can't set activity", rnti);
    return;
  }
  ue* ue_ptr = it->second.get();

  // inform EUTRA RRC about user activity
  if (ue_ptr->is_endc()) {
    // inform EUTRA RRC about user activity
    rrc_eutra->set_activity_user(ue_ptr->get_eutra_rnti());
  }
}

void rrc_nr::config_phy()
{
  static const srsran::phy_cfg_nr_t default_phy_cfg =
      srsran::phy_cfg_nr_default_t{srsran::phy_cfg_nr_default_t::reference_cfg_t{}};
  srsenb::phy_interface_rrc_nr::common_cfg_t common_cfg = {};
  common_cfg.carrier                                    = default_phy_cfg.carrier;
  common_cfg.pdcch                                      = default_phy_cfg.pdcch;
  common_cfg.prach                                      = default_phy_cfg.prach;
  if (phy->set_common_cfg(common_cfg) < SRSRAN_SUCCESS) {
    logger.error("Couldn't set common PHY config");
    return;
  }
}

void rrc_nr::config_mac()
{
  // Fill MAC scheduler configuration for SIBs
  // TODO: use parsed cell NR cfg configuration
  std::vector<srsenb::sched_nr_interface::cell_cfg_t> sched_cells_cfg = {srsenb::get_default_cells_cfg(1)};

  // FIXME: entire SI configuration, etc needs to be ported to NR
  sched_interface::cell_cfg_t cell_cfg;
  set_sched_cell_cfg_sib1(&cell_cfg, cfg.sib1);

  // set SIB length
  for (uint32_t i = 0; i < nof_si_messages + 1; i++) {
    cell_cfg.sibs[i].len = sib_buffer[i]->N_bytes;
  }

  // PUCCH width
  cell_cfg.nrb_pucch = SRSRAN_MAX(cfg.sr_cfg.nof_prb, /* TODO: where is n_rb2 in NR? */ 0);
  logger.info("Allocating %d PRBs for PUCCH", cell_cfg.nrb_pucch);

  // Copy Cell configuration
  // cell_cfg.cell = cfg.cell;

  // Configure MAC/scheduler
  mac->cell_cfg(sched_cells_cfg);
}

int32_t rrc_nr::generate_sibs()
{
  // MIB packing
  bcch_bch_msg_s mib_msg;
  mib_s&         mib = mib_msg.msg.set_mib();
  mib                = cfg.mib;
  {
    srsran::unique_byte_buffer_t mib_buf = srsran::make_byte_buffer();
    if (mib_buf == nullptr) {
      logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
      return SRSRAN_ERROR;
    }
    asn1::bit_ref bref(mib_buf->msg, mib_buf->get_tailroom());
    if (mib_msg.pack(bref) != asn1::SRSASN_SUCCESS) {
      logger.error("Couldn't pack mib msg");
      return SRSRAN_ERROR;
    }
    mib_buf->N_bytes = bref.distance_bytes();
    logger.debug(mib_buf->msg, mib_buf->N_bytes, "MIB payload (%d B)", mib_buf->N_bytes);
    mib_buffer = std::move(mib_buf);
  }

  si_sched_info_s::sched_info_list_l_& sched_info = cfg.sib1.si_sched_info.sched_info_list;
  uint32_t nof_messages = cfg.sib1.si_sched_info_present ? cfg.sib1.si_sched_info.sched_info_list.size() : 0;

  // msg is array of SI messages, each SI message msg[i] may contain multiple SIBs
  // all SIBs in a SI message msg[i] share the same periodicity
  sib_buffer.reserve(nof_messages + 1);
  asn1::dyn_array<bcch_dl_sch_msg_s> msg(nof_messages + 1);

  // Copy SIB1 to first SI message
  msg[0].msg.set_c1().set_sib_type1() = cfg.sib1;

  // Copy rest of SIBs
  for (uint32_t sched_info_elem = 0; sched_info_elem < nof_messages; sched_info_elem++) {
    uint32_t msg_index = sched_info_elem + 1; // first msg is SIB1, therefore start with second

    msg[msg_index].msg.set_c1().set_sys_info().crit_exts.set_sys_info();
    auto& sib_list = msg[msg_index].msg.c1().sys_info().crit_exts.sys_info().sib_type_and_info;

    for (uint32_t mapping = 0; mapping < sched_info[sched_info_elem].sib_map_info.size(); ++mapping) {
      uint32_t sibidx = sched_info[sched_info_elem].sib_map_info[mapping].type; // SIB2 == 0
      sib_list.push_back(cfg.sibs[sibidx]);
    }
  }

  // Pack payload for all messages
  for (uint32_t msg_index = 0; msg_index < nof_messages + 1; msg_index++) {
    srsran::unique_byte_buffer_t sib = srsran::make_byte_buffer();
    if (sib == nullptr) {
      logger.error("Couldn't allocate PDU in %s().", __FUNCTION__);
      return SRSRAN_ERROR;
    }
    asn1::bit_ref bref(sib->msg, sib->get_tailroom());
    if (msg[msg_index].pack(bref) != asn1::SRSASN_SUCCESS) {
      logger.error("Failed to pack SIB message %d", msg_index);
      return SRSRAN_ERROR;
    }
    sib->N_bytes = bref.distance_bytes();
    sib_buffer.push_back(std::move(sib));

    // Log SIBs in JSON format
    fmt::memory_buffer strbuf;
    fmt::format_to(strbuf, "SI message={} payload", msg_index);
    log_rrc_message(fmt::to_string(strbuf), Tx, sib_buffer.back().get(), msg[msg_index]);
  }

  nof_si_messages = sib_buffer.size() - 1;

  return SRSRAN_SUCCESS;
}

/*******************************************************************************
  MAC interface
*******************************************************************************/

int rrc_nr::read_pdu_bcch_bch(const uint32_t tti, srsran::unique_byte_buffer_t& buffer)
{
  if (mib_buffer == nullptr || buffer->get_tailroom() < mib_buffer->N_bytes) {
    return SRSRAN_ERROR;
  }
  memcpy(buffer->msg, mib_buffer->msg, mib_buffer->N_bytes);
  buffer->N_bytes = mib_buffer->N_bytes;
  return SRSRAN_SUCCESS;
}

int rrc_nr::read_pdu_bcch_dlsch(uint32_t sib_index, srsran::unique_byte_buffer_t& buffer)
{
  if (sib_index >= sib_buffer.size()) {
    logger.error("SIB %d is not a configured SIB.", sib_index);
    return SRSRAN_ERROR;
  }

  if (buffer->get_tailroom() < sib_buffer[sib_index]->N_bytes) {
    logger.error("Not enough space to fit SIB %d into buffer (%d < %d)",
                 sib_index,
                 buffer->get_tailroom(),
                 sib_buffer[sib_index]->N_bytes);
    return SRSRAN_ERROR;
  }

  memcpy(buffer->msg, sib_buffer[sib_index]->msg, sib_buffer[sib_index]->N_bytes);
  buffer->N_bytes = sib_buffer[sib_index]->N_bytes;

  return SRSRAN_SUCCESS;
}

void rrc_nr::get_metrics(srsenb::rrc_metrics_t& m)
{
  // return metrics
}

void rrc_nr::handle_pdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t pdu)
{
  if (pdu) {
    logger.info(pdu->msg, pdu->N_bytes, "Rx %s PDU", get_rb_name(lcid));
  }

  if (users.count(rnti) == 1) {
    switch (static_cast<srsran::nr_srb>(lcid)) {
      case srsran::nr_srb::srb0:
        //        parse_ul_ccch(rnti, std::move(pdu));
        break;
      case srsran::nr_srb::srb1:
      case srsran::nr_srb::srb2:
        //        parse_ul_dcch(p.rnti, p.lcid, std::move(p.pdu));
        break;
      default:
        logger.error("Rx PDU with invalid bearer id: %d", lcid);
        break;
    }
  } else {
    logger.warning("Discarding PDU for removed rnti=0x%x", rnti);
  }
}

/*******************************************************************************
  PDCP interface
*******************************************************************************/
void rrc_nr::write_pdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t pdu)
{
  handle_pdu(rnti, lcid, std::move(pdu));
}

void rrc_nr::notify_pdcp_integrity_error(uint16_t rnti, uint32_t lcid) {}

/*******************************************************************************
  NGAP interface
*******************************************************************************/

int rrc_nr::ue_set_security_cfg_key(uint16_t rnti, const asn1::fixed_bitstring<256, false, true>& key)
{
  return SRSRAN_SUCCESS;
}
int rrc_nr::ue_set_bitrates(uint16_t rnti, const asn1::ngap_nr::ue_aggregate_maximum_bit_rate_s& rates)
{
  return SRSRAN_SUCCESS;
}
int rrc_nr::set_aggregate_max_bitrate(uint16_t rnti, const asn1::ngap_nr::ue_aggregate_maximum_bit_rate_s& rates)
{
  return SRSRAN_SUCCESS;
}
int rrc_nr::ue_set_security_cfg_capabilities(uint16_t rnti, const asn1::ngap_nr::ue_security_cap_s& caps)
{
  return SRSRAN_SUCCESS;
}
int rrc_nr::start_security_mode_procedure(uint16_t rnti)
{
  return SRSRAN_SUCCESS;
}
int rrc_nr::establish_rrc_bearer(uint16_t rnti, uint16_t pdu_session_id, srsran::const_byte_span nas_pdu, uint32_t lcid)
{
  return SRSRAN_SUCCESS;
}

int rrc_nr::allocate_lcid(uint16_t rnti)
{
  return SRSRAN_SUCCESS;
}

void rrc_nr::write_dl_info(uint16_t rnti, srsran::unique_byte_buffer_t sdu) {}

/*******************************************************************************
  Interface for EUTRA RRC
*******************************************************************************/

int rrc_nr::sgnb_addition_request(uint16_t eutra_rnti, const sgnb_addition_req_params_t& params)
{
  task_sched.defer_task([this, eutra_rnti, params]() {
    // try to allocate new user
    uint16_t nr_rnti = mac->reserve_rnti(0);
    if (nr_rnti == SRSRAN_INVALID_RNTI) {
      logger.error("Failed to allocate RNTI at MAC");
      rrc_eutra->sgnb_addition_reject(eutra_rnti);
      return;
    }

    if (add_user(nr_rnti) != SRSRAN_SUCCESS) {
      logger.error("Failed to allocate RNTI at RRC");
      rrc_eutra->sgnb_addition_reject(eutra_rnti);
      return;
    }

    // new RNTI is now registered at MAC and RRC
    auto user_it = users.find(nr_rnti);
    if (user_it == users.end()) {
      logger.warning("Unrecognised rnti: 0x%x", nr_rnti);
      return;
    }
    user_it->second->handle_sgnb_addition_request(eutra_rnti, params);
  });

  // return straight away
  return SRSRAN_SUCCESS;
}

int rrc_nr::sgnb_reconfiguration_complete(uint16_t eutra_rnti, asn1::dyn_octstring reconfig_response)
{
  // user has completeted the reconfiguration and has acked on 4G side, wait until RA on NR
  logger.info("Received Reconfiguration complete for RNTI=0x%x", eutra_rnti);
  return SRSRAN_SUCCESS;
}

/*******************************************************************************
  UE class

  Every function in UE class is called from a mutex environment thus does not
  need extra protection.
*******************************************************************************/
rrc_nr::ue::ue(rrc_nr* parent_, uint16_t rnti_) : parent(parent_), rnti(rnti_), uecfg(srsenb::get_default_ue_cfg(1)) {}

void rrc_nr::ue::send_connection_setup()
{
  dl_ccch_msg_s dl_ccch_msg;
  dl_ccch_msg.msg.set_c1().set_rrc_setup().rrc_transaction_id = ((transaction_id++) % 4u);
  rrc_setup_ies_s&    setup  = dl_ccch_msg.msg.c1().rrc_setup().crit_exts.set_rrc_setup();
  radio_bearer_cfg_s& rr_cfg = setup.radio_bearer_cfg;

  // Add DRB1 to cfg
  rr_cfg.drb_to_add_mod_list_present = true;
  rr_cfg.drb_to_add_mod_list.resize(1);
  auto& drb_item                               = rr_cfg.drb_to_add_mod_list[0];
  drb_item.drb_id                              = 1;
  drb_item.pdcp_cfg_present                    = true;
  drb_item.pdcp_cfg.ciphering_disabled_present = true;
  //  drb_item.cn_assoc_present = true;
  //  drb_item.cn_assoc.set_eps_bearer_id() = ;
  drb_item.recover_pdcp_present = false;

  // TODO: send config to RLC/PDCP

  send_dl_ccch(&dl_ccch_msg);
}

void rrc_nr::ue::send_dl_ccch(dl_ccch_msg_s* dl_ccch_msg)
{
  // Allocate a new PDU buffer, pack the message and send to PDCP
  srsran::unique_byte_buffer_t pdu = srsran::make_byte_buffer();
  if (pdu == nullptr) {
    parent->logger.error("Allocating pdu");
  }
  asn1::bit_ref bref(pdu->msg, pdu->get_tailroom());
  if (dl_ccch_msg->pack(bref) == asn1::SRSASN_ERROR_ENCODE_FAIL) {
    parent->logger.error("Failed to pack DL-CCCH message. Discarding msg.");
  }
  pdu->N_bytes = bref.distance_bytes();

  char buf[32] = {};
  sprintf(buf, "SRB0 - rnti=0x%x", rnti);
  parent->log_rrc_message(buf, Tx, pdu.get(), *dl_ccch_msg);
  parent->rlc->write_sdu(rnti, (uint32_t)srsran::nr_srb::srb0, std::move(pdu));
}

// Helper for the RRC Reconfiguration sender to pack hard-coded config
int rrc_nr::ue::pack_secondary_cell_group_config_common(asn1::rrc_nr::cell_group_cfg_s& cell_group_cfg_pack)
{
  auto& pscell_cfg = parent->cfg.cell_list.at(UE_PSCELL_CC_IDX);

  srsran::srsran_band_helper band_helper;

  // RLC for DRB1 (with fixed LCID)
  cell_group_cfg_pack.rlc_bearer_to_add_mod_list_present = true;
  cell_group_cfg_pack.rlc_bearer_to_add_mod_list.resize(1);
  auto& rlc                       = cell_group_cfg_pack.rlc_bearer_to_add_mod_list[0];
  rlc.lc_ch_id                    = drb1_lcid;
  rlc.served_radio_bearer_present = true;
  rlc.served_radio_bearer.set_drb_id();
  rlc.served_radio_bearer.drb_id() = 1;
  rlc.rlc_cfg_present              = true;
  rlc.rlc_cfg.set_um_bi_dir();
  rlc.rlc_cfg.um_bi_dir().ul_um_rlc.sn_field_len_present = true;
  rlc.rlc_cfg.um_bi_dir().ul_um_rlc.sn_field_len         = sn_field_len_um_opts::size12;
  rlc.rlc_cfg.um_bi_dir().dl_um_rlc.sn_field_len_present = true;
  rlc.rlc_cfg.um_bi_dir().dl_um_rlc.sn_field_len         = sn_field_len_um_opts::size12;
  rlc.rlc_cfg.um_bi_dir().dl_um_rlc.t_reassembly         = t_reassembly_opts::ms50;

  // MAC logical channel config
  rlc.mac_lc_ch_cfg_present                    = true;
  rlc.mac_lc_ch_cfg.ul_specific_params_present = true;
  rlc.mac_lc_ch_cfg.ul_specific_params.prio    = 11;
  rlc.mac_lc_ch_cfg.ul_specific_params.prioritised_bit_rate =
      asn1::rrc_nr::lc_ch_cfg_s::ul_specific_params_s_::prioritised_bit_rate_opts::kbps0;
  rlc.mac_lc_ch_cfg.ul_specific_params.bucket_size_dur =
      asn1::rrc_nr::lc_ch_cfg_s::ul_specific_params_s_::bucket_size_dur_opts::ms100;
  rlc.mac_lc_ch_cfg.ul_specific_params.lc_ch_group_present      = true;
  rlc.mac_lc_ch_cfg.ul_specific_params.lc_ch_group              = 6;
  rlc.mac_lc_ch_cfg.ul_specific_params.sched_request_id_present = true;
  rlc.mac_lc_ch_cfg.ul_specific_params.sched_request_id         = 0;

  // mac-CellGroup-Config for BSR and SR
  cell_group_cfg_pack.mac_cell_group_cfg_present                         = true;
  auto& mac_cell_group                                                   = cell_group_cfg_pack.mac_cell_group_cfg;
  mac_cell_group.sched_request_cfg_present                               = true;
  mac_cell_group.sched_request_cfg.sched_request_to_add_mod_list_present = true;
  mac_cell_group.sched_request_cfg.sched_request_to_add_mod_list.resize(1);
  mac_cell_group.sched_request_cfg.sched_request_to_add_mod_list[0].sched_request_id = 0;
  mac_cell_group.sched_request_cfg.sched_request_to_add_mod_list[0].sr_trans_max =
      asn1::rrc_nr::sched_request_to_add_mod_s::sr_trans_max_opts::n64;
  mac_cell_group.bsr_cfg_present            = true;
  mac_cell_group.bsr_cfg.periodic_bsr_timer = asn1::rrc_nr::bsr_cfg_s::periodic_bsr_timer_opts::sf20;
  mac_cell_group.bsr_cfg.retx_bsr_timer     = asn1::rrc_nr::bsr_cfg_s::retx_bsr_timer_opts::sf320;
  // Skip TAG and PHR config

  cell_group_cfg_pack.sp_cell_cfg_present               = true;
  cell_group_cfg_pack.sp_cell_cfg.serv_cell_idx_present = true;

  // SP Cell Dedicated config
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded_present             = true;
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.init_dl_bwp_present = true;

  // PDCCH config
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.init_dl_bwp.pdcch_cfg_present = true;
  auto& pdcch_cfg_dedicated = cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.init_dl_bwp.pdcch_cfg;
  auto& pdcch_ded_setup     = pdcch_cfg_dedicated.set_setup();
  pdcch_ded_setup.ctrl_res_set_to_add_mod_list_present = true;
  pdcch_ded_setup.ctrl_res_set_to_add_mod_list.resize(1);
  pdcch_ded_setup.ctrl_res_set_to_add_mod_list[0].ctrl_res_set_id = 2;
  pdcch_ded_setup.ctrl_res_set_to_add_mod_list[0].freq_domain_res.from_number(
      0b111111110000000000000000000000000000000000000);
  pdcch_ded_setup.ctrl_res_set_to_add_mod_list[0].dur = 1;
  pdcch_ded_setup.ctrl_res_set_to_add_mod_list[0].cce_reg_map_type.set_non_interleaved();
  pdcch_ded_setup.ctrl_res_set_to_add_mod_list[0].precoder_granularity =
      asn1::rrc_nr::ctrl_res_set_s::precoder_granularity_opts::same_as_reg_bundle;

  // search spaces
  pdcch_ded_setup.search_spaces_to_add_mod_list_present = true;
  pdcch_ded_setup.search_spaces_to_add_mod_list.resize(1);
  pdcch_ded_setup.search_spaces_to_add_mod_list[0].search_space_id                                = 2;
  pdcch_ded_setup.search_spaces_to_add_mod_list[0].ctrl_res_set_id_present                        = true;
  pdcch_ded_setup.search_spaces_to_add_mod_list[0].ctrl_res_set_id                                = 2;
  pdcch_ded_setup.search_spaces_to_add_mod_list[0].monitoring_slot_periodicity_and_offset_present = true;
  pdcch_ded_setup.search_spaces_to_add_mod_list[0].monitoring_slot_periodicity_and_offset.set_sl1();
  pdcch_ded_setup.search_spaces_to_add_mod_list[0].monitoring_symbols_within_slot_present = true;
  pdcch_ded_setup.search_spaces_to_add_mod_list[0].monitoring_symbols_within_slot.from_number(0b10000000000000);
  pdcch_ded_setup.search_spaces_to_add_mod_list[0].nrof_candidates_present = true;
  pdcch_ded_setup.search_spaces_to_add_mod_list[0].nrof_candidates.aggregation_level1 =
      asn1::rrc_nr::search_space_s::nrof_candidates_s_::aggregation_level1_opts::n0;
  pdcch_ded_setup.search_spaces_to_add_mod_list[0].nrof_candidates.aggregation_level2 =
      asn1::rrc_nr::search_space_s::nrof_candidates_s_::aggregation_level2_opts::n2;
  pdcch_ded_setup.search_spaces_to_add_mod_list[0].nrof_candidates.aggregation_level4 =
      asn1::rrc_nr::search_space_s::nrof_candidates_s_::aggregation_level4_opts::n1;
  pdcch_ded_setup.search_spaces_to_add_mod_list[0].nrof_candidates.aggregation_level8 =
      asn1::rrc_nr::search_space_s::nrof_candidates_s_::aggregation_level8_opts::n0;
  pdcch_ded_setup.search_spaces_to_add_mod_list[0].nrof_candidates.aggregation_level16 =
      asn1::rrc_nr::search_space_s::nrof_candidates_s_::aggregation_level16_opts::n0;
  pdcch_ded_setup.search_spaces_to_add_mod_list[0].search_space_type_present = true;
  pdcch_ded_setup.search_spaces_to_add_mod_list[0].search_space_type.set_ue_specific();
  pdcch_ded_setup.search_spaces_to_add_mod_list[0].search_space_type.ue_specific().dci_formats = asn1::rrc_nr::
      search_space_s::search_space_type_c_::ue_specific_s_::dci_formats_opts::formats0_minus0_and_minus1_minus0;

  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.init_dl_bwp.pdsch_cfg_present = true;
  auto& pdsch_cfg_dedicated = cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.init_dl_bwp.pdsch_cfg;

  pdsch_cfg_dedicated.set_setup();
  pdsch_cfg_dedicated.setup().dmrs_dl_for_pdsch_map_type_a_present = true;
  pdsch_cfg_dedicated.setup().dmrs_dl_for_pdsch_map_type_a.set_setup();
  pdsch_cfg_dedicated.setup().dmrs_dl_for_pdsch_map_type_a.setup().dmrs_add_position_present = true;
  pdsch_cfg_dedicated.setup().dmrs_dl_for_pdsch_map_type_a.setup().dmrs_add_position =
      asn1::rrc_nr::dmrs_dl_cfg_s::dmrs_add_position_opts::pos1;
  pdsch_cfg_dedicated.setup().tci_states_to_add_mod_list_present = true;
  pdsch_cfg_dedicated.setup().tci_states_to_add_mod_list.resize(1);
  pdsch_cfg_dedicated.setup().tci_states_to_add_mod_list[0].tci_state_id = 0;
  pdsch_cfg_dedicated.setup().tci_states_to_add_mod_list[0].qcl_type1.ref_sig.set_ssb();
  pdsch_cfg_dedicated.setup().tci_states_to_add_mod_list[0].qcl_type1.ref_sig.ssb() = 0;
  pdsch_cfg_dedicated.setup().tci_states_to_add_mod_list[0].qcl_type1.qcl_type =
      asn1::rrc_nr::qcl_info_s::qcl_type_opts::type_d;
  pdsch_cfg_dedicated.setup().res_alloc = pdsch_cfg_s::res_alloc_opts::res_alloc_type1;
  pdsch_cfg_dedicated.setup().rbg_size  = asn1::rrc_nr::pdsch_cfg_s::rbg_size_opts::cfg1;
  pdsch_cfg_dedicated.setup().prb_bundling_type.set_static_bundling();
  pdsch_cfg_dedicated.setup().prb_bundling_type.static_bundling().bundle_size_present = true;
  pdsch_cfg_dedicated.setup().prb_bundling_type.static_bundling().bundle_size =
      asn1::rrc_nr::pdsch_cfg_s::prb_bundling_type_c_::static_bundling_s_::bundle_size_opts::wideband;

  // ZP-CSI
  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list_present = true;
  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list.resize(1);
  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list[0].zp_csi_rs_res_id = 0;
  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list[0].res_map.freq_domain_alloc.set_row4();
  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list[0].res_map.freq_domain_alloc.row4().from_number(0b100);
  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list[0].res_map.nrof_ports =
      asn1::rrc_nr::csi_rs_res_map_s::nrof_ports_opts::p4;

  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list[0].res_map.first_ofdm_symbol_in_time_domain = 8;
  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list[0].res_map.cdm_type =
      asn1::rrc_nr::csi_rs_res_map_s::cdm_type_opts::fd_cdm2;
  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list[0].res_map.density.set_one();

  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list[0].res_map.freq_band.start_rb     = 0;
  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list[0].res_map.freq_band.nrof_rbs     = 52;
  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list[0].periodicity_and_offset_present = true;
  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list[0].periodicity_and_offset.set_slots80();
  pdsch_cfg_dedicated.setup().zp_csi_rs_res_to_add_mod_list[0].periodicity_and_offset.slots80() = 1;
  pdsch_cfg_dedicated.setup().p_zp_csi_rs_res_set_present                                       = true;
  pdsch_cfg_dedicated.setup().p_zp_csi_rs_res_set.set_setup();
  pdsch_cfg_dedicated.setup().p_zp_csi_rs_res_set.setup().zp_csi_rs_res_set_id = 0;
  pdsch_cfg_dedicated.setup().p_zp_csi_rs_res_set.setup().zp_csi_rs_res_id_list.resize(1);

  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.first_active_dl_bwp_id_present = true;
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.ul_cfg_present                 = true;

  // UL config dedicated
  // PUCCH
  auto& ul_config                         = cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.ul_cfg;
  ul_config.init_ul_bwp_present           = true;
  ul_config.init_ul_bwp.pucch_cfg_present = true;
  ul_config.init_ul_bwp.pucch_cfg.set_setup();
  ul_config.init_ul_bwp.pucch_cfg.setup().format2_present = true;
  ul_config.init_ul_bwp.pucch_cfg.setup().format2.set_setup();
  ul_config.init_ul_bwp.pucch_cfg.setup().format2.setup().max_code_rate_present = true;
  ul_config.init_ul_bwp.pucch_cfg.setup().format2.setup().max_code_rate         = pucch_max_code_rate_opts::zero_dot25;

  // SR resources
  ul_config.init_ul_bwp.pucch_cfg.setup().sched_request_res_to_add_mod_list_present = true;
  ul_config.init_ul_bwp.pucch_cfg.setup().sched_request_res_to_add_mod_list.resize(1);
  auto& sr_res1                          = ul_config.init_ul_bwp.pucch_cfg.setup().sched_request_res_to_add_mod_list[0];
  sr_res1.sched_request_res_id           = 1;
  sr_res1.sched_request_id               = 0;
  sr_res1.periodicity_and_offset_present = true;
  sr_res1.res_present                    = true;
  sr_res1.res                            = 2; // PUCCH resource for SR

  // DL data
  ul_config.init_ul_bwp.pucch_cfg.setup().dl_data_to_ul_ack_present = true;

  // PUCCH Resource for format 1
  srsran_pucch_nr_resource_t resource_small = {};
  resource_small.starting_prb               = 0;
  resource_small.format                     = SRSRAN_PUCCH_NR_FORMAT_1;
  resource_small.initial_cyclic_shift       = 0;
  resource_small.nof_symbols                = 14;
  resource_small.start_symbol_idx           = 0;
  resource_small.time_domain_occ            = 0;

  // PUCCH Resource for format 2
  srsran_pucch_nr_resource_t resource_big = {};
  resource_big.starting_prb               = 51;
  resource_big.format                     = SRSRAN_PUCCH_NR_FORMAT_2;
  resource_big.nof_prb                    = 1;
  resource_big.nof_symbols                = 2;
  resource_big.start_symbol_idx           = 12;

  // Resource for SR
  srsran_pucch_nr_resource_t resource_sr = {};
  resource_sr.starting_prb               = 51;
  resource_sr.format                     = SRSRAN_PUCCH_NR_FORMAT_1;
  resource_sr.initial_cyclic_shift       = 0;
  resource_sr.nof_symbols                = 14;
  resource_sr.start_symbol_idx           = 0;
  resource_sr.time_domain_occ            = 0;

  // Make 3 possible resources
  ul_config.init_ul_bwp.pucch_cfg.setup().res_to_add_mod_list_present = true;
  ul_config.init_ul_bwp.pucch_cfg.setup().res_to_add_mod_list.resize(3);
  if (not srsran::make_phy_res_config(
          resource_small, ul_config.init_ul_bwp.pucch_cfg.setup().res_to_add_mod_list[0], 0)) {
    parent->logger.warning("Failed to create 1-2 bit NR PUCCH resource");
  }
  if (not srsran::make_phy_res_config(
          resource_big, ul_config.init_ul_bwp.pucch_cfg.setup().res_to_add_mod_list[1], 1)) {
    parent->logger.warning("Failed to create >2 bit NR PUCCH resource");
  }
  if (not srsran::make_phy_res_config(resource_sr, ul_config.init_ul_bwp.pucch_cfg.setup().res_to_add_mod_list[2], 2)) {
    parent->logger.warning("Failed to create SR NR PUCCH resource");
  }

  // Make 2 PUCCH resource sets
  ul_config.init_ul_bwp.pucch_cfg.setup().res_set_to_add_mod_list_present = true;
  ul_config.init_ul_bwp.pucch_cfg.setup().res_set_to_add_mod_list.resize(2);

  // Make PUCCH resource set for 1-2 bit
  ul_config.init_ul_bwp.pucch_cfg.setup().res_set_to_add_mod_list[0].pucch_res_set_id = 0;
  ul_config.init_ul_bwp.pucch_cfg.setup().res_set_to_add_mod_list[0].res_list.resize(8);
  for (auto& e : ul_config.init_ul_bwp.pucch_cfg.setup().res_set_to_add_mod_list[0].res_list) {
    e = 0;
  }

  // Make PUCCH resource set for >2 bit
  ul_config.init_ul_bwp.pucch_cfg.setup().res_set_to_add_mod_list[1].pucch_res_set_id = 1;
  ul_config.init_ul_bwp.pucch_cfg.setup().res_set_to_add_mod_list[1].res_list.resize(8);
  for (auto& e : ul_config.init_ul_bwp.pucch_cfg.setup().res_set_to_add_mod_list[1].res_list) {
    e = 1;
  }

  // PUSCH config
  ul_config.init_ul_bwp.pusch_cfg_present = true;
  ul_config.init_ul_bwp.pusch_cfg.set_setup();
  auto& pusch_cfg_ded                                = ul_config.init_ul_bwp.pusch_cfg.setup();
  pusch_cfg_ded.dmrs_ul_for_pusch_map_type_a_present = true;
  pusch_cfg_ded.dmrs_ul_for_pusch_map_type_a.set_setup();
  pusch_cfg_ded.dmrs_ul_for_pusch_map_type_a.setup().dmrs_add_position_present = true;
  pusch_cfg_ded.dmrs_ul_for_pusch_map_type_a.setup().dmrs_add_position = dmrs_ul_cfg_s::dmrs_add_position_opts::pos1;
  // PUSH power control skipped
  pusch_cfg_ded.res_alloc = pusch_cfg_s::res_alloc_opts::res_alloc_type1;

  // UCI
  pusch_cfg_ded.uci_on_pusch_present = true;
  pusch_cfg_ded.uci_on_pusch.set_setup();
  pusch_cfg_ded.uci_on_pusch.setup().beta_offsets_present = true;
  pusch_cfg_ded.uci_on_pusch.setup().beta_offsets.set_semi_static();
  auto& beta_offset_semi_static                        = pusch_cfg_ded.uci_on_pusch.setup().beta_offsets.semi_static();
  beta_offset_semi_static.beta_offset_ack_idx1_present = true;
  beta_offset_semi_static.beta_offset_ack_idx1         = 9;
  beta_offset_semi_static.beta_offset_ack_idx2_present = true;
  beta_offset_semi_static.beta_offset_ack_idx2         = 9;
  beta_offset_semi_static.beta_offset_ack_idx3_present = true;
  beta_offset_semi_static.beta_offset_ack_idx3         = 9;
  beta_offset_semi_static.beta_offset_csi_part1_idx1_present = true;
  beta_offset_semi_static.beta_offset_csi_part1_idx1         = 6;
  beta_offset_semi_static.beta_offset_csi_part1_idx2_present = true;
  beta_offset_semi_static.beta_offset_csi_part1_idx2         = 6;
  beta_offset_semi_static.beta_offset_csi_part2_idx1_present = true;
  beta_offset_semi_static.beta_offset_csi_part2_idx1         = 6;
  beta_offset_semi_static.beta_offset_csi_part2_idx2_present = true;
  beta_offset_semi_static.beta_offset_csi_part2_idx2         = 6;
  pusch_cfg_ded.uci_on_pusch.setup().scaling                 = uci_on_pusch_s::scaling_opts::f1;

  ul_config.first_active_ul_bwp_id_present = true;
  ul_config.first_active_ul_bwp_id         = 0;
  // Serving cell config (only to setup)
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.pdcch_serving_cell_cfg_present = true;
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.pdcch_serving_cell_cfg.set_setup();

  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.pdsch_serving_cell_cfg_present = true;
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.pdsch_serving_cell_cfg.set_setup();
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.pdsch_serving_cell_cfg.setup().nrof_harq_processes_for_pdsch_present =
      true;
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.pdsch_serving_cell_cfg.setup().nrof_harq_processes_for_pdsch =
      pdsch_serving_cell_cfg_s::nrof_harq_processes_for_pdsch_opts::n16;

  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.csi_meas_cfg_present = true;
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.csi_meas_cfg.set_setup();

  // CSI report config
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.csi_meas_cfg.setup().csi_report_cfg_to_add_mod_list_present = true;
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.csi_meas_cfg.setup().csi_report_cfg_to_add_mod_list.resize(1);
  auto& csi_report =
      cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.csi_meas_cfg.setup().csi_report_cfg_to_add_mod_list[0];
  csi_report.report_cfg_id                       = 0;
  csi_report.res_for_ch_meas                     = 0;
  csi_report.csi_im_res_for_interference_present = true;
  csi_report.csi_im_res_for_interference         = 1;
  csi_report.report_cfg_type.set_periodic();
  csi_report.report_cfg_type.periodic().report_slot_cfg.set_slots80();
  csi_report.report_cfg_type.periodic().pucch_csi_res_list.resize(1);
  csi_report.report_cfg_type.periodic().pucch_csi_res_list[0].ul_bw_part_id = 0;
  csi_report.report_cfg_type.periodic().pucch_csi_res_list[0].pucch_res     = 1; // was 17 in orig PCAP
  csi_report.report_quant.set_cri_ri_pmi_cqi();
  // Report freq config (optional)
  csi_report.report_freq_cfg_present                = true;
  csi_report.report_freq_cfg.cqi_format_ind_present = true;
  csi_report.report_freq_cfg.cqi_format_ind =
      asn1::rrc_nr::csi_report_cfg_s::report_freq_cfg_s_::cqi_format_ind_opts::wideband_cqi;
  csi_report.time_restrict_for_ch_meass = asn1::rrc_nr::csi_report_cfg_s::time_restrict_for_ch_meass_opts::not_cfgured;
  csi_report.time_restrict_for_interference_meass =
      asn1::rrc_nr::csi_report_cfg_s::time_restrict_for_interference_meass_opts::not_cfgured;
  csi_report.group_based_beam_report.set_disabled();
  // Skip CQI table (optional)
  csi_report.cqi_table_present = true;
  csi_report.cqi_table         = asn1::rrc_nr::csi_report_cfg_s::cqi_table_opts::table2;
  csi_report.subband_size      = asn1::rrc_nr::csi_report_cfg_s::subband_size_opts::value1;

  // Reconfig with Sync
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync_present   = true;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.new_ue_id = rnti;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.t304      = recfg_with_sync_s::t304_opts::ms1000;

  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common_present           = true;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ss_pbch_block_pwr = 0;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.n_timing_advance_offset =
      asn1::rrc_nr::serving_cell_cfg_common_s::n_timing_advance_offset_opts::n0;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dmrs_type_a_position =
      asn1::rrc_nr::serving_cell_cfg_common_s::dmrs_type_a_position_opts::pos2;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.pci_present = true;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.pci         = pscell_cfg.phy_cell.carrier.pci;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ssb_subcarrier_spacing_present = true;

  // DL config
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common_present              = true;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.freq_info_dl_present = true;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.freq_info_dl.freq_band_list
      .push_back(parent->cfg.cell_list[0].band);
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.freq_info_dl.absolute_freq_point_a =
      band_helper.get_abs_freq_point_a_arfcn(parent->cfg.cell_list[0].phy_cell.carrier.nof_prb,
                                             parent->cfg.cell_list[0].dl_arfcn);
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.freq_info_dl
      .absolute_freq_ssb_present = true;

  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.freq_info_dl
      .scs_specific_carrier_list.resize(1);

  cell_group_cfg_pack.phys_cell_group_cfg_present = true;
  cell_group_cfg_pack.phys_cell_group_cfg.pdsch_harq_ack_codebook =
      phys_cell_group_cfg_s::pdsch_harq_ack_codebook_opts::dynamic_value;

  auto& dl_carrier = cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.freq_info_dl
                         .scs_specific_carrier_list[0];
  dl_carrier.offset_to_carrier  = 0;
  dl_carrier.subcarrier_spacing = subcarrier_spacing_opts::khz15;
  dl_carrier.carrier_bw         = 52;

  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.init_dl_bwp_present = true;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.init_dl_bwp.generic_params
      .location_and_bw = 14025;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.init_dl_bwp.generic_params
      .subcarrier_spacing = subcarrier_spacing_opts::khz15;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.init_dl_bwp
      .pdcch_cfg_common_present = true;
  auto& pdcch_cfg_common =
      cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.init_dl_bwp.pdcch_cfg_common;
  pdcch_cfg_common.set_setup();
  pdcch_cfg_common.setup().common_ctrl_res_set_present         = true;
  pdcch_cfg_common.setup().common_ctrl_res_set.ctrl_res_set_id = 1;
  pdcch_cfg_common.setup().common_ctrl_res_set.freq_domain_res.from_number(
      0b111111110000000000000000000000000000000000000);
  pdcch_cfg_common.setup().common_ctrl_res_set.dur = 1;
  pdcch_cfg_common.setup().common_ctrl_res_set.cce_reg_map_type.set_non_interleaved();
  pdcch_cfg_common.setup().common_ctrl_res_set.precoder_granularity =
      asn1::rrc_nr::ctrl_res_set_s::precoder_granularity_opts::same_as_reg_bundle;

  // common search space list
  pdcch_cfg_common.setup().common_search_space_list_present = true;
  pdcch_cfg_common.setup().common_search_space_list.resize(1);
  pdcch_cfg_common.setup().common_search_space_list[0].search_space_id           = 1;
  pdcch_cfg_common.setup().common_search_space_list[0].ctrl_res_set_id_present   = true;
  pdcch_cfg_common.setup().common_search_space_list[0].ctrl_res_set_id           = 1;
  pdcch_cfg_common.setup().common_search_space_list[0].search_space_type_present = true;
  pdcch_cfg_common.setup().common_search_space_list[0].search_space_type.set_common();
  pdcch_cfg_common.setup()
      .common_search_space_list[0]
      .search_space_type.common()
      .dci_format0_minus0_and_format1_minus0_present                           = true;
  pdcch_cfg_common.setup().common_search_space_list[0].nrof_candidates_present = true;
  pdcch_cfg_common.setup().common_search_space_list[0].nrof_candidates.aggregation_level1 =
      asn1::rrc_nr::search_space_s::nrof_candidates_s_::aggregation_level1_opts::n1;
  pdcch_cfg_common.setup().common_search_space_list[0].nrof_candidates.aggregation_level2 =
      asn1::rrc_nr::search_space_s::nrof_candidates_s_::aggregation_level2_opts::n1;
  pdcch_cfg_common.setup().common_search_space_list[0].nrof_candidates.aggregation_level4 =
      asn1::rrc_nr::search_space_s::nrof_candidates_s_::aggregation_level4_opts::n1;
  pdcch_cfg_common.setup().common_search_space_list[0].nrof_candidates.aggregation_level8 =
      asn1::rrc_nr::search_space_s::nrof_candidates_s_::aggregation_level8_opts::n0;
  pdcch_cfg_common.setup().common_search_space_list[0].nrof_candidates.aggregation_level16 =
      asn1::rrc_nr::search_space_s::nrof_candidates_s_::aggregation_level16_opts::n0;
  pdcch_cfg_common.setup().common_search_space_list[0].monitoring_slot_periodicity_and_offset_present = true;
  pdcch_cfg_common.setup().common_search_space_list[0].monitoring_slot_periodicity_and_offset.set_sl1();
  pdcch_cfg_common.setup().common_search_space_list[0].monitoring_symbols_within_slot_present = true;
  pdcch_cfg_common.setup().common_search_space_list[0].monitoring_symbols_within_slot.from_number(0b10000000000000);
  pdcch_cfg_common.setup().ra_search_space_present = true;
  pdcch_cfg_common.setup().ra_search_space         = 1;

  // PDSCH config common
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.init_dl_bwp
      .pdsch_cfg_common_present = true;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.init_dl_bwp.pdsch_cfg_common
      .set_setup();
  auto& pdsch_cfg_common = cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.init_dl_bwp
                               .pdsch_cfg_common.setup();
  pdsch_cfg_common.pdsch_time_domain_alloc_list_present = true;
  pdsch_cfg_common.pdsch_time_domain_alloc_list.resize(1);
  pdsch_cfg_common.pdsch_time_domain_alloc_list[0].map_type = pdsch_time_domain_res_alloc_s::map_type_opts::type_a;
  pdsch_cfg_common.pdsch_time_domain_alloc_list[0].start_symbol_and_len = 40;

  // UL config
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common_present = true;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.dummy = time_align_timer_opts::ms500;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.freq_info_ul_present = true;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.freq_info_ul.freq_band_list
      .push_back(parent->cfg.cell_list[0].band);
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.freq_info_ul.absolute_freq_point_a =
      band_helper.get_abs_freq_point_a_arfcn(parent->cfg.cell_list[0].phy_cell.carrier.nof_prb,
                                             parent->cfg.cell_list[0].ul_arfcn);
  ;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.freq_info_ul
      .scs_specific_carrier_list.resize(1);
  auto& ul_carrier = cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.freq_info_ul
                         .scs_specific_carrier_list[0];
  ul_carrier.offset_to_carrier  = 0;
  ul_carrier.subcarrier_spacing = subcarrier_spacing_opts::khz15;
  ul_carrier.carrier_bw         = 52;

  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.init_ul_bwp_present = true;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.init_ul_bwp.generic_params
      .location_and_bw = 14025;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.init_ul_bwp.generic_params
      .subcarrier_spacing = subcarrier_spacing_opts::khz15;

  // RACH config
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.init_ul_bwp.rach_cfg_common_present =
      true;
  auto& rach_cfg_common_pack =
      cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.init_ul_bwp.rach_cfg_common;

  rach_cfg_common_pack.set_setup();
  rach_cfg_common_pack.setup().rach_cfg_generic.msg1_fdm                  = rach_cfg_generic_s::msg1_fdm_opts::one;
  rach_cfg_common_pack.setup().rach_cfg_generic.msg1_freq_start           = 1;
  rach_cfg_common_pack.setup().rach_cfg_generic.zero_correlation_zone_cfg = 0;
  rach_cfg_common_pack.setup().rach_cfg_generic.preamb_rx_target_pwr      = -110;
  rach_cfg_common_pack.setup().rach_cfg_generic.preamb_trans_max =
      asn1::rrc_nr::rach_cfg_generic_s::preamb_trans_max_opts::n7;
  rach_cfg_common_pack.setup().rach_cfg_generic.pwr_ramp_step =
      asn1::rrc_nr::rach_cfg_generic_s::pwr_ramp_step_opts::db4;
  rach_cfg_common_pack.setup().rach_cfg_generic.ra_resp_win = asn1::rrc_nr::rach_cfg_generic_s::ra_resp_win_opts::sl10;
  rach_cfg_common_pack.setup().ra_contention_resolution_timer =
      asn1::rrc_nr::rach_cfg_common_s::ra_contention_resolution_timer_opts::sf64;
  rach_cfg_common_pack.setup().prach_root_seq_idx.set(
      asn1::rrc_nr::rach_cfg_common_s::prach_root_seq_idx_c_::types_opts::l839);
  rach_cfg_common_pack.setup().prach_root_seq_idx.set_l839() = 0; // matches value in phy_cfg_nr_default_t()
  rach_cfg_common_pack.setup().restricted_set_cfg =
      asn1::rrc_nr::rach_cfg_common_s::restricted_set_cfg_opts::unrestricted_set;

  // PUSCH config common
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.init_ul_bwp
      .pusch_cfg_common_present = true;
  auto& pusch_cfg_common_pack =
      cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.init_ul_bwp.pusch_cfg_common;
  pusch_cfg_common_pack.set_setup();
  pusch_cfg_common_pack.setup().pusch_time_domain_alloc_list_present = true;
  pusch_cfg_common_pack.setup().pusch_time_domain_alloc_list.resize(2);
  pusch_cfg_common_pack.setup().pusch_time_domain_alloc_list[0].k2_present = true;
  pusch_cfg_common_pack.setup().pusch_time_domain_alloc_list[0].k2         = 4;
  pusch_cfg_common_pack.setup().pusch_time_domain_alloc_list[0].map_type =
      asn1::rrc_nr::pusch_time_domain_res_alloc_s::map_type_opts::type_a;
  pusch_cfg_common_pack.setup().pusch_time_domain_alloc_list[0].start_symbol_and_len = 27;
  pusch_cfg_common_pack.setup().pusch_time_domain_alloc_list[1].k2_present           = true;
  pusch_cfg_common_pack.setup().pusch_time_domain_alloc_list[1].k2                   = 3;
  pusch_cfg_common_pack.setup().pusch_time_domain_alloc_list[1].map_type =
      asn1::rrc_nr::pusch_time_domain_res_alloc_s::map_type_opts::type_a;
  pusch_cfg_common_pack.setup().pusch_time_domain_alloc_list[1].start_symbol_and_len = 27;
  pusch_cfg_common_pack.setup().p0_nominal_with_grant                                = -90;

  // PUCCH config common
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.init_ul_bwp
      .pucch_cfg_common_present = true;
  auto& pucch_cfg_common_pack =
      cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.init_ul_bwp.pucch_cfg_common;
  pucch_cfg_common_pack.set_setup();
  pucch_cfg_common_pack.setup().pucch_group_hop    = asn1::rrc_nr::pucch_cfg_common_s::pucch_group_hop_opts::neither;
  pucch_cfg_common_pack.setup().p0_nominal_present = true;
  pucch_cfg_common_pack.setup().p0_nominal         = -90;

  // SSB config (optional)
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ssb_positions_in_burst_present = true;
  auto& ssb_pos_in_burst = cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ssb_positions_in_burst;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ssb_periodicity_serving_cell_present = true;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ssb_periodicity_serving_cell =
      serving_cell_cfg_common_s::ssb_periodicity_serving_cell_opts::ms20;

  return SRSRAN_SUCCESS;
}

// Helper for the RRC Reconfiguration sender to pack hard-coded config
int rrc_nr::ue::pack_secondary_cell_group_config_fdd(asn1::dyn_octstring& packed_secondary_cell_config)
{
  auto& cell_group_cfg_pack = cell_group_cfg;
  pack_secondary_cell_group_config_common(cell_group_cfg);

  uint32_t absolute_freq_ssb;

  if (parent->cfg.cell_list[0].band == 3) { // band n3
    absolute_freq_ssb = 367930;
  } else if (parent->cfg.cell_list[0].band == 5) { // band n5
    absolute_freq_ssb = 176210;
  } else if (parent->cfg.cell_list[0].band == 7) { // band n7
    absolute_freq_ssb = 529470;
  } else {
    parent->logger.error("Unsupported dl_arfcn.");
    return SRSRAN_ERROR;
  }

  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.first_active_dl_bwp_id = 0;
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.ul_cfg_present         = true;

  // UL config dedicated
  auto& ul_config = cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.ul_cfg;
  // SR resources
  auto& sr_res1 = ul_config.init_ul_bwp.pucch_cfg.setup().sched_request_res_to_add_mod_list[0];
  sr_res1.periodicity_and_offset.set_sl40() = 8;
  sr_res1.res                               = 2; // PUCCH resource for SR

  // DL data
  ul_config.init_ul_bwp.pucch_cfg.setup().dl_data_to_ul_ack.resize(1);
  ul_config.init_ul_bwp.pucch_cfg.setup().dl_data_to_ul_ack[0] = 4;

  // nzp-CSI-RS Resource
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.csi_meas_cfg.setup().nzp_csi_rs_res_to_add_mod_list_present = true;
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.csi_meas_cfg.setup().nzp_csi_rs_res_to_add_mod_list.resize(5);
  auto& nzp_csi_res = cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.csi_meas_cfg.setup();
  // item 0
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[0].nzp_csi_rs_res_id = 0;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[0].res_map.freq_domain_alloc.set_row2();
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[0].res_map.freq_domain_alloc.row2().from_number(0b100000000000);
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[0].res_map.nrof_ports =
      asn1::rrc_nr::csi_rs_res_map_s::nrof_ports_opts::p1;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[0].res_map.first_ofdm_symbol_in_time_domain = 4;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[0].res_map.cdm_type =
      asn1::rrc_nr::csi_rs_res_map_s::cdm_type_opts::no_cdm;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[0].res_map.density.set_one();
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[0].res_map.freq_band.start_rb = 0;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[0].res_map.freq_band.nrof_rbs = 52;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[0].pwr_ctrl_offset            = 0;
  // Skip pwr_ctrl_offset_ss_present
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[0].scrambling_id = parent->cfg.cell_list[0].phy_cell.cell_id;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[0].periodicity_and_offset_present = true;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[0].periodicity_and_offset.set_slots80();
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[0].periodicity_and_offset.slots80() = 1;
  // optional
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[0].qcl_info_periodic_csi_rs_present = true;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[0].qcl_info_periodic_csi_rs         = 0;
  // item 1
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[1].nzp_csi_rs_res_id = 1;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[1].res_map.freq_domain_alloc.set_row1();
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[1].res_map.freq_domain_alloc.row1().from_number(0b0001);
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[1].res_map.nrof_ports =
      asn1::rrc_nr::csi_rs_res_map_s::nrof_ports_opts::p1;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[1].res_map.first_ofdm_symbol_in_time_domain = 4;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[1].res_map.cdm_type =
      asn1::rrc_nr::csi_rs_res_map_s::cdm_type_opts::no_cdm;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[1].res_map.density.set_three();
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[1].res_map.freq_band.start_rb = 0;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[1].res_map.freq_band.nrof_rbs = 52;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[1].pwr_ctrl_offset            = 0;
  // Skip pwr_ctrl_offset_ss_present
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[1].scrambling_id = parent->cfg.cell_list[0].phy_cell.cell_id;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[1].periodicity_and_offset_present = true;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[1].periodicity_and_offset.set_slots40();
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[1].periodicity_and_offset.slots40() = 11;
  // optional
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[1].qcl_info_periodic_csi_rs_present = true;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[1].qcl_info_periodic_csi_rs         = 0;
  // item 2
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[2].nzp_csi_rs_res_id = 2;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[2].res_map.freq_domain_alloc.set_row1();
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[2].res_map.freq_domain_alloc.row1().from_number(0b0001);
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[2].res_map.nrof_ports =
      asn1::rrc_nr::csi_rs_res_map_s::nrof_ports_opts::p1;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[2].res_map.first_ofdm_symbol_in_time_domain = 8;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[2].res_map.cdm_type =
      asn1::rrc_nr::csi_rs_res_map_s::cdm_type_opts::no_cdm;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[2].res_map.density.set_three();
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[2].res_map.freq_band.start_rb = 0;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[2].res_map.freq_band.nrof_rbs = 52;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[2].pwr_ctrl_offset            = 0;
  // Skip pwr_ctrl_offset_ss_present
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[2].scrambling_id = parent->cfg.cell_list[0].phy_cell.cell_id;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[2].periodicity_and_offset_present = true;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[2].periodicity_and_offset.set_slots40();
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[2].periodicity_and_offset.slots40() = 11;
  // optional
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[2].qcl_info_periodic_csi_rs_present = true;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[2].qcl_info_periodic_csi_rs         = 0;
  // item 3
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[3].nzp_csi_rs_res_id = 3;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[3].res_map.freq_domain_alloc.set_row1();
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[3].res_map.freq_domain_alloc.row1().from_number(0b0001);
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[3].res_map.nrof_ports =
      asn1::rrc_nr::csi_rs_res_map_s::nrof_ports_opts::p1;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[3].res_map.first_ofdm_symbol_in_time_domain = 4;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[3].res_map.cdm_type =
      asn1::rrc_nr::csi_rs_res_map_s::cdm_type_opts::no_cdm;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[3].res_map.density.set_three();
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[3].res_map.freq_band.start_rb = 0;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[3].res_map.freq_band.nrof_rbs = 52;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[3].pwr_ctrl_offset            = 0;
  // Skip pwr_ctrl_offset_ss_present
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[3].scrambling_id = parent->cfg.cell_list[0].phy_cell.cell_id;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[3].periodicity_and_offset_present = true;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[3].periodicity_and_offset.set_slots40();
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[3].periodicity_and_offset.slots40() = 12;
  // optional
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[3].qcl_info_periodic_csi_rs_present = true;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[3].qcl_info_periodic_csi_rs         = 0;
  // item 4
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[4].nzp_csi_rs_res_id = 4;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[4].res_map.freq_domain_alloc.set_row1();
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[4].res_map.freq_domain_alloc.row1().from_number(0b0001);
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[4].res_map.nrof_ports =
      asn1::rrc_nr::csi_rs_res_map_s::nrof_ports_opts::p1;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[4].res_map.first_ofdm_symbol_in_time_domain = 8;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[4].res_map.cdm_type =
      asn1::rrc_nr::csi_rs_res_map_s::cdm_type_opts::no_cdm;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[4].res_map.density.set_three();
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[4].res_map.freq_band.start_rb = 0;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[4].res_map.freq_band.nrof_rbs = 52;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[4].pwr_ctrl_offset            = 0;
  // Skip pwr_ctrl_offset_ss_present
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[4].scrambling_id = parent->cfg.cell_list[0].phy_cell.cell_id;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[4].periodicity_and_offset_present = true;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[4].periodicity_and_offset.set_slots40();
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[4].periodicity_and_offset.slots40() = 12;
  // optional
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[4].qcl_info_periodic_csi_rs_present = true;
  nzp_csi_res.nzp_csi_rs_res_to_add_mod_list[4].qcl_info_periodic_csi_rs         = 0;

  // nzp-CSI-RS ResourceSet
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.csi_meas_cfg.setup().nzp_csi_rs_res_set_to_add_mod_list_present =
      true;
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.csi_meas_cfg.setup().nzp_csi_rs_res_set_to_add_mod_list.resize(2);
  auto& nzp_csi_res_set = cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.csi_meas_cfg.setup();
  // item 0
  nzp_csi_res_set.nzp_csi_rs_res_set_to_add_mod_list[0].nzp_csi_res_set_id = 0;
  nzp_csi_res_set.nzp_csi_rs_res_set_to_add_mod_list[0].nzp_csi_rs_res.resize(1);
  nzp_csi_res_set.nzp_csi_rs_res_set_to_add_mod_list[0].nzp_csi_rs_res[0] = 0;
  // item 1
  nzp_csi_res_set.nzp_csi_rs_res_set_to_add_mod_list[1].nzp_csi_res_set_id = 1;
  nzp_csi_res_set.nzp_csi_rs_res_set_to_add_mod_list[1].nzp_csi_rs_res.resize(4);
  nzp_csi_res_set.nzp_csi_rs_res_set_to_add_mod_list[1].nzp_csi_rs_res[0] = 1;
  nzp_csi_res_set.nzp_csi_rs_res_set_to_add_mod_list[1].nzp_csi_rs_res[1] = 2;
  nzp_csi_res_set.nzp_csi_rs_res_set_to_add_mod_list[1].nzp_csi_rs_res[2] = 3;
  nzp_csi_res_set.nzp_csi_rs_res_set_to_add_mod_list[1].nzp_csi_rs_res[3] = 4;
  // Skip TRS info

  // CSI IM config
  // TODO: add csi im config

  // CSI resource config
  // TODO: add csi resource config

  // CSI report config
  auto& csi_report =
      cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.csi_meas_cfg.setup().csi_report_cfg_to_add_mod_list[0];
  csi_report.report_cfg_type.periodic().report_slot_cfg.slots80() = 5;

  // Reconfig with Sync
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ssb_subcarrier_spacing =
      subcarrier_spacing_opts::khz15;

  // DL config
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common_present = true;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.freq_info_dl.absolute_freq_ssb =
      absolute_freq_ssb; // TODO: calculate from actual DL ARFCN

  // RACH config
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.init_ul_bwp.rach_cfg_common_present =
      true;
  auto& rach_cfg_common_pack =
      cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.init_ul_bwp.rach_cfg_common;

  rach_cfg_common_pack.set_setup();
  rach_cfg_common_pack.setup().rach_cfg_generic.prach_cfg_idx = 16;

  // SSB config (optional)
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ssb_positions_in_burst_present = true;
  auto& ssb_pos_in_burst = cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ssb_positions_in_burst;
  ssb_pos_in_burst.set_short_bitmap().from_number(0b1000);

  // make sufficiant space
  packed_secondary_cell_config.resize(256);
  asn1::bit_ref bref_pack(packed_secondary_cell_config.data(), packed_secondary_cell_config.size());
  if (cell_group_cfg_pack.pack(bref_pack) != asn1::SRSASN_SUCCESS) {
    parent->logger.error("Failed to pack NR secondary cell config");
    return SRSRAN_ERROR;
  }
  packed_secondary_cell_config.resize(bref_pack.distance_bytes());

  return SRSRAN_SUCCESS;
}

// Helper for the RRC Reconfiguration sender to pack hard-coded config
int rrc_nr::ue::pack_secondary_cell_group_config_tdd(asn1::dyn_octstring& packed_secondary_cell_config)
{
  auto& cell_group_cfg_pack = cell_group_cfg;
  pack_secondary_cell_group_config_common(cell_group_cfg);

  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.first_active_dl_bwp_id = 1;
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.ul_cfg_present         = true;

  // UL config dedicated
  auto& ul_config = cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.ul_cfg;
  // SR resources
  auto& sr_res1 = ul_config.init_ul_bwp.pucch_cfg.setup().sched_request_res_to_add_mod_list[0];
  // SR resources
  sr_res1.periodicity_and_offset.set_sl40() = 8;
  sr_res1.res                               = 2; // PUCCH resource for SR

  // DL data
  ul_config.init_ul_bwp.pucch_cfg.setup().dl_data_to_ul_ack.resize(6);
  ul_config.init_ul_bwp.pucch_cfg.setup().dl_data_to_ul_ack[0] = 6;
  ul_config.init_ul_bwp.pucch_cfg.setup().dl_data_to_ul_ack[1] = 5;
  ul_config.init_ul_bwp.pucch_cfg.setup().dl_data_to_ul_ack[2] = 4;
  ul_config.init_ul_bwp.pucch_cfg.setup().dl_data_to_ul_ack[3] = 4;
  ul_config.init_ul_bwp.pucch_cfg.setup().dl_data_to_ul_ack[4] = 4;
  ul_config.init_ul_bwp.pucch_cfg.setup().dl_data_to_ul_ack[5] = 4;

  // nzp-CSI-RS Resource
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.csi_meas_cfg.setup().nzp_csi_rs_res_to_add_mod_list_present = true;
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.csi_meas_cfg.setup().nzp_csi_rs_res_to_add_mod_list.resize(1);
  auto& nzp_csi_res =
      cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.csi_meas_cfg.setup().nzp_csi_rs_res_to_add_mod_list[0];
  nzp_csi_res.nzp_csi_rs_res_id = 0;
  nzp_csi_res.res_map.freq_domain_alloc.set_row2();
  nzp_csi_res.res_map.freq_domain_alloc.row2().from_number(0b100000000000);
  nzp_csi_res.res_map.nrof_ports                       = asn1::rrc_nr::csi_rs_res_map_s::nrof_ports_opts::p1;
  nzp_csi_res.res_map.first_ofdm_symbol_in_time_domain = 4;
  nzp_csi_res.res_map.cdm_type                         = asn1::rrc_nr::csi_rs_res_map_s::cdm_type_opts::no_cdm;
  nzp_csi_res.res_map.density.set_one();
  nzp_csi_res.res_map.freq_band.start_rb = 0;
  nzp_csi_res.res_map.freq_band.nrof_rbs = 52;
  nzp_csi_res.pwr_ctrl_offset            = 0;
  // Skip pwr_ctrl_offset_ss_present
  nzp_csi_res.scrambling_id                        = 0;
  nzp_csi_res.periodicity_and_offset_present       = true;
  nzp_csi_res.periodicity_and_offset.set_slots80() = 0;
  // optional
  nzp_csi_res.qcl_info_periodic_csi_rs_present = true;
  nzp_csi_res.qcl_info_periodic_csi_rs         = 0;

  // nzp-CSI-RS ResourceSet
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.csi_meas_cfg.setup().nzp_csi_rs_res_set_to_add_mod_list_present =
      true;
  cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.csi_meas_cfg.setup().nzp_csi_rs_res_set_to_add_mod_list.resize(1);
  auto& nzp_csi_res_set =
      cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.csi_meas_cfg.setup().nzp_csi_rs_res_set_to_add_mod_list[0];
  nzp_csi_res_set.nzp_csi_res_set_id = 0;
  nzp_csi_res_set.nzp_csi_rs_res.resize(1);
  nzp_csi_res_set.nzp_csi_rs_res[0] = 0;
  // Skip TRS info

  // CSI report config
  auto& csi_report =
      cell_group_cfg_pack.sp_cell_cfg.sp_cell_cfg_ded.csi_meas_cfg.setup().csi_report_cfg_to_add_mod_list[0];
  csi_report.report_cfg_type.periodic().report_slot_cfg.slots80() = 7;

  // Reconfig with Sync
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.smtc.release();
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ssb_subcarrier_spacing =
      subcarrier_spacing_opts::khz30;

  // DL config
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common_present = true;
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.freq_info_dl.absolute_freq_ssb =
      634176; // TODO: calculate from actual DL ARFCN

  auto& pdcch_cfg_common =
      cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.dl_cfg_common.init_dl_bwp.pdcch_cfg_common;
  pdcch_cfg_common.set_setup();
  pdcch_cfg_common.setup().ext = false;

  // RACH config
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.init_ul_bwp.rach_cfg_common_present =
      true;
  auto& rach_cfg_common_pack =
      cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ul_cfg_common.init_ul_bwp.rach_cfg_common;

  rach_cfg_common_pack.set_setup();
  rach_cfg_common_pack.setup().rach_cfg_generic.prach_cfg_idx = 0;

  // SSB config (optional)
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ssb_positions_in_burst_present = true;
  auto& ssb_pos_in_burst = cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.ssb_positions_in_burst;
  ssb_pos_in_burst.set_medium_bitmap().from_number(0b10000000);

  // // TDD UL-DL config
  cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.tdd_ul_dl_cfg_common_present = true;
  auto& tdd_config = cell_group_cfg_pack.sp_cell_cfg.recfg_with_sync.sp_cell_cfg_common.tdd_ul_dl_cfg_common;
  tdd_config.ref_subcarrier_spacing        = subcarrier_spacing_e::khz15;
  tdd_config.pattern1.dl_ul_tx_periodicity = asn1::rrc_nr::tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_opts::ms10;
  tdd_config.pattern1.nrof_dl_slots        = 6;
  tdd_config.pattern1.nrof_dl_symbols      = 0;
  tdd_config.pattern1.nrof_ul_slots        = 4;
  tdd_config.pattern1.nrof_ul_symbols      = 0;

  // make sufficiant space
  packed_secondary_cell_config.resize(256);
  asn1::bit_ref bref_pack(packed_secondary_cell_config.data(), packed_secondary_cell_config.size());
  if (cell_group_cfg_pack.pack(bref_pack) != asn1::SRSASN_SUCCESS) {
    parent->logger.error("Failed to pack NR secondary cell config");
    return SRSRAN_ERROR;
  }
  packed_secondary_cell_config.resize(bref_pack.distance_bytes());

  return SRSRAN_SUCCESS;
}

// Packs a hard-coded RRC Reconfiguration with fixed params for all layers (for now)
int rrc_nr::ue::pack_rrc_reconfiguraiton(asn1::dyn_octstring& packed_rrc_reconfig)
{
  rrc_recfg_s reconfig;
  reconfig.rrc_transaction_id = ((transaction_id++) % 4u);
  rrc_recfg_ies_s& recfg_ies  = reconfig.crit_exts.set_rrc_recfg();

  // add secondary cell group config
  recfg_ies.secondary_cell_group_present = true;

  if (parent->cfg.cell_list[0].duplex_mode == SRSRAN_DUPLEX_MODE_FDD) {
    if (pack_secondary_cell_group_config_fdd(recfg_ies.secondary_cell_group) == SRSRAN_ERROR) {
      parent->logger.error("Failed to pack TDD RRC Reconfiguration");
      return SRSRAN_ERROR;
    }
  } else {
    if (pack_secondary_cell_group_config_tdd(recfg_ies.secondary_cell_group) == SRSRAN_ERROR) {
      parent->logger.error("Failed to pack TDD RRC Reconfiguration");
      return SRSRAN_ERROR;
    }
  }

  // now pack ..
  packed_rrc_reconfig.resize(512);
  asn1::bit_ref bref_pack(packed_rrc_reconfig.data(), packed_rrc_reconfig.size());
  if (reconfig.pack(bref_pack) != asn1::SRSASN_SUCCESS) {
    parent->logger.error("Failed to pack RRC Reconfiguration");
    return SRSRAN_ERROR;
  }
  packed_rrc_reconfig.resize(bref_pack.distance_bytes());

  return SRSRAN_SUCCESS;
}

// Packs a hard-coded NR radio bearer config with fixed params for RLC/PDCP (for now)
int rrc_nr::ue::pack_nr_radio_bearer_config(asn1::dyn_octstring& packed_nr_bearer_config)
{
  // set security config
  auto& radio_bearer_cfg_pack                        = radio_bearer_cfg;
  radio_bearer_cfg_pack.security_cfg_present         = true;
  auto& sec_cfg                                      = radio_bearer_cfg_pack.security_cfg;
  sec_cfg.key_to_use_present                         = true;
  sec_cfg.key_to_use                                 = asn1::rrc_nr::security_cfg_s::key_to_use_opts::secondary;
  sec_cfg.security_algorithm_cfg_present             = true;
  sec_cfg.security_algorithm_cfg.ciphering_algorithm = ciphering_algorithm_opts::nea0;

  // pack it
  packed_nr_bearer_config.resize(128);
  asn1::bit_ref bref_pack(packed_nr_bearer_config.data(), packed_nr_bearer_config.size());
  if (radio_bearer_cfg_pack.pack(bref_pack) != asn1::SRSASN_SUCCESS) {
    parent->logger.error("Failed to pack NR radio bearer config");
    return SRSRAN_ERROR;
  }

  // resize to packed length
  packed_nr_bearer_config.resize(bref_pack.distance_bytes());

  return SRSRAN_SUCCESS;
}

int rrc_nr::ue::handle_sgnb_addition_request(uint16_t eutra_rnti_, const sgnb_addition_req_params_t& req_params)
{
  // Add DRB1 to RLC and PDCP
  if (add_drb() != SRSRAN_SUCCESS) {
    parent->logger.error("Failed to configure DRB");
    parent->rrc_eutra->sgnb_addition_reject(eutra_rnti_);
    return SRSRAN_ERROR;
  }

  // provide hard-coded NR configs
  rrc_eutra_interface_rrc_nr::sgnb_addition_ack_params_t ack_params = {};
  if (pack_rrc_reconfiguraiton(ack_params.nr_secondary_cell_group_cfg_r15) == SRSRAN_ERROR) {
    parent->logger.error("Failed to pack RRC Reconfiguration. Sending SgNB addition reject.");
    parent->rrc_eutra->sgnb_addition_reject(eutra_rnti_);
    return SRSRAN_ERROR;
  }

  if (pack_nr_radio_bearer_config(ack_params.nr_radio_bearer_cfg1_r15) == SRSRAN_ERROR) {
    parent->logger.error("Failed to pack NR radio bearer config. Sending SgNB addition reject.");
    parent->rrc_eutra->sgnb_addition_reject(eutra_rnti_);
    return SRSRAN_ERROR;
  }

  // send response to EUTRA
  ack_params.nr_rnti       = rnti;
  ack_params.eps_bearer_id = req_params.eps_bearer_id;
  parent->rrc_eutra->sgnb_addition_ack(eutra_rnti_, ack_params);

  // recognize RNTI as ENDC user
  endc       = true;
  eutra_rnti = eutra_rnti_;

  return SRSRAN_SUCCESS;
}

void rrc_nr::ue::crnti_ce_received()
{
  // Assume NSA mode active
  if (endc) {
    // send SgNB addition complete for ENDC users
    parent->rrc_eutra->sgnb_addition_complete(eutra_rnti, rnti);

    // Add DRB1 to MAC
    for (auto& drb : cell_group_cfg.rlc_bearer_to_add_mod_list) {
      uecfg.ue_bearers[drb.lc_ch_id].direction = mac_lc_ch_cfg_t::BOTH;
    }
    parent->mac->ue_cfg(rnti, uecfg);
  }
}

/**
 * @brief Set DRB configuration
 *
 * The function sets and configures all relavant fields for the DRB configuration (MAC, RLC, PDCP) in the
 * cellGroupConfig and also adds the bearer to the local RLC and PDCP entities.
 *
 * @return int SRSRAN_SUCCESS on success
 */
int rrc_nr::ue::add_drb()
{
  // RLC for DRB1 (with fixed LCID) inside cell_group_cfg
  auto& cell_group_cfg_pack = cell_group_cfg;

  cell_group_cfg_pack.rlc_bearer_to_add_mod_list_present = true;
  cell_group_cfg_pack.rlc_bearer_to_add_mod_list.resize(1);
  auto& rlc                       = cell_group_cfg_pack.rlc_bearer_to_add_mod_list[0];
  rlc.lc_ch_id                    = drb1_lcid;
  rlc.served_radio_bearer_present = true;
  rlc.served_radio_bearer.set_drb_id();
  rlc.served_radio_bearer.drb_id() = 1;
  rlc.rlc_cfg_present              = true;
  rlc.rlc_cfg.set_um_bi_dir();
  rlc.rlc_cfg.um_bi_dir().ul_um_rlc.sn_field_len_present = true;
  rlc.rlc_cfg.um_bi_dir().ul_um_rlc.sn_field_len         = sn_field_len_um_opts::size12;
  rlc.rlc_cfg.um_bi_dir().dl_um_rlc.sn_field_len_present = true;
  rlc.rlc_cfg.um_bi_dir().dl_um_rlc.sn_field_len         = sn_field_len_um_opts::size12;
  rlc.rlc_cfg.um_bi_dir().dl_um_rlc.t_reassembly         = t_reassembly_opts::ms50;

  // add RLC bearer
  srsran::rlc_config_t rlc_cfg;
  if (srsran::make_rlc_config_t(cell_group_cfg.rlc_bearer_to_add_mod_list[0].rlc_cfg, &rlc_cfg) != SRSRAN_SUCCESS) {
    parent->logger.error("Failed to build RLC config");
    return SRSRAN_ERROR;
  }
  parent->rlc->add_bearer(rnti, drb1_lcid, rlc_cfg);

  // MAC logical channel config
  rlc.mac_lc_ch_cfg_present                    = true;
  rlc.mac_lc_ch_cfg.ul_specific_params_present = true;
  rlc.mac_lc_ch_cfg.ul_specific_params.prio    = 11;
  rlc.mac_lc_ch_cfg.ul_specific_params.prioritised_bit_rate =
      asn1::rrc_nr::lc_ch_cfg_s::ul_specific_params_s_::prioritised_bit_rate_opts::kbps0;
  rlc.mac_lc_ch_cfg.ul_specific_params.bucket_size_dur =
      asn1::rrc_nr::lc_ch_cfg_s::ul_specific_params_s_::bucket_size_dur_opts::ms100;
  rlc.mac_lc_ch_cfg.ul_specific_params.lc_ch_group_present      = true;
  rlc.mac_lc_ch_cfg.ul_specific_params.lc_ch_group              = 6;
  rlc.mac_lc_ch_cfg.ul_specific_params.sched_request_id_present = true;
  rlc.mac_lc_ch_cfg.ul_specific_params.sched_request_id         = 0;
  // TODO: add LC config to MAC

  // PDCP config goes into radio_bearer_cfg
  auto& radio_bearer_cfg_pack                       = radio_bearer_cfg;
  radio_bearer_cfg_pack.drb_to_add_mod_list_present = true;
  radio_bearer_cfg_pack.drb_to_add_mod_list.resize(1);

  // configure fixed DRB1
  auto& drb_item                                = radio_bearer_cfg_pack.drb_to_add_mod_list[0];
  drb_item.drb_id                               = 1;
  drb_item.cn_assoc_present                     = true;
  drb_item.cn_assoc.set_eps_bearer_id()         = 5;
  drb_item.pdcp_cfg_present                     = true;
  drb_item.pdcp_cfg.ciphering_disabled_present  = true;
  drb_item.pdcp_cfg.drb_present                 = true;
  drb_item.pdcp_cfg.drb.pdcp_sn_size_dl_present = true;
  drb_item.pdcp_cfg.drb.pdcp_sn_size_dl         = asn1::rrc_nr::pdcp_cfg_s::drb_s_::pdcp_sn_size_dl_opts::len18bits;
  drb_item.pdcp_cfg.drb.pdcp_sn_size_ul_present = true;
  drb_item.pdcp_cfg.drb.pdcp_sn_size_ul         = asn1::rrc_nr::pdcp_cfg_s::drb_s_::pdcp_sn_size_ul_opts::len18bits;
  drb_item.pdcp_cfg.drb.discard_timer_present   = true;
  drb_item.pdcp_cfg.drb.discard_timer           = asn1::rrc_nr::pdcp_cfg_s::drb_s_::discard_timer_opts::ms100;
  drb_item.pdcp_cfg.drb.hdr_compress.set_not_used();
  drb_item.pdcp_cfg.t_reordering_present = true;
  drb_item.pdcp_cfg.t_reordering         = asn1::rrc_nr::pdcp_cfg_s::t_reordering_opts::ms0;

  // Add DRB1 to PDCP
  srsran::pdcp_config_t pdcp_cnfg = srsran::make_drb_pdcp_config_t(drb_item.drb_id, false, drb_item.pdcp_cfg);
  parent->pdcp->add_bearer(rnti, rlc.lc_ch_id, pdcp_cnfg);

  // Note: DRB1 is only activated in the MAC when the C-RNTI CE is received

  return SRSRAN_SUCCESS;
}

} // namespace srsenb
