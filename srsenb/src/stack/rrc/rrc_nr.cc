/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srsenb/hdr/stack/rrc/rrc_nr.h"
#include "srsenb/hdr/stack/upper/common_enb.h"
#include "srslte/asn1/rrc_nr_asn1_utils.h"
#include "srslte/interfaces/nr_common_interface_types.h"

using namespace asn1::rrc_nr;

namespace srsenb {

rrc_nr::rrc_nr(srslte::timer_handler* timers_) :
  m_log("RRC"),
  pool(srslte::byte_buffer_pool::get_instance()),
  timers(timers_)
{}

void rrc_nr::init(const rrc_nr_cfg_t&     cfg_,
                  phy_interface_stack_nr* phy_,
                  mac_interface_rrc_nr*   mac_,
                  rlc_interface_rrc_nr*   rlc_,
                  pdcp_interface_rrc_nr*  pdcp_,
                  ngap_interface_rrc_nr*  ngap_,
                  gtpu_interface_rrc_nr*  gtpu_)
{
  phy  = phy_;
  mac  = mac_;
  rlc  = rlc_;
  pdcp = pdcp_;
  gtpu = gtpu_;
  ngap = ngap_;

  // FIXME: overwriting because we are not passing config right now
  cfg = update_default_cfg(cfg_);

  // config logging
  m_log->set_level(cfg.log_level);
  m_log->set_hex_limit(cfg.log_hex_limit);

  // derived
  slot_dur_ms = 1;

  nof_si_messages = generate_sibs();
  config_mac();

  // add dummy user
  m_log->info("Creating dummy DRB for RNTI=%d on LCID=%d\n", cfg.coreless.rnti, cfg.coreless.drb_lcid);
  add_user(cfg.coreless.rnti);
  srslte::rlc_config_t rlc_cnfg = srslte::rlc_config_t::default_rlc_um_nr_config(6);
  rlc->add_bearer(cfg.coreless.rnti, cfg.coreless.drb_lcid, rlc_cnfg);
  srslte::pdcp_config_t pdcp_cnfg{cfg.coreless.drb_lcid,
                                  srslte::PDCP_RB_IS_DRB,
                                  srslte::SECURITY_DIRECTION_DOWNLINK,
                                  srslte::SECURITY_DIRECTION_UPLINK,
                                  srslte::PDCP_SN_LEN_18,
                                  srslte::pdcp_t_reordering_t::ms500,
                                  srslte::pdcp_discard_timer_t::infinity};
  pdcp->add_bearer(cfg.coreless.rnti, cfg.coreless.drb_lcid, pdcp_cnfg);

  m_log->info("Started\n");

  running = true;
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
                             const srslte::byte_buffer_t* pdu,
                             const T&                     msg)
{
  if (m_log->get_level() == srslte::LOG_LEVEL_INFO) {
    m_log->info("%s - %s %s (%d B)\n",
                source.c_str(),
                dir == Tx ? "Tx" : "Rx",
                msg.msg.c1().type().to_string().c_str(),
                pdu->N_bytes);
  } else if (m_log->get_level() >= srslte::LOG_LEVEL_DEBUG) {
    asn1::json_writer json_writer;
    msg.to_json(json_writer);
    m_log->debug_hex(pdu->msg,
                     pdu->N_bytes,
                     "%s - %s %s (%d B)\n",
                     source.c_str(),
                     dir == Tx ? "Tx" : "Rx",
                     msg.msg.c1().type().to_string().c_str(),
                     pdu->N_bytes);
    m_log->debug("Content:\n%s\n", json_writer.to_string().c_str());
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

  cfg_default.cell.nof_prb         = 25;
  cfg_default.cell.nof_ports       = 1;
  cfg_default.cell.id              = 0;
  cfg_default.cell.cp              = SRSLTE_CP_NORM;
  cfg_default.cell.frame_type      = SRSLTE_FDD;
  cfg_default.cell.phich_length    = SRSLTE_PHICH_NORM;
  cfg_default.cell.phich_resources = SRSLTE_PHICH_R_1;

  // Fill SIB1
  cfg_default.sib1.cell_access_related_info.plmn_id_list.resize(1);
  cfg_default.sib1.cell_access_related_info.plmn_id_list[0].plmn_id_list.resize(1);
  srslte::plmn_id_t plmn;
  plmn.from_string("90170");
  srslte::to_asn1(&cfg_default.sib1.cell_access_related_info.plmn_id_list[0].plmn_id_list[0], plmn);
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
  // FIXME: Fill SIB2 values

  // set loglevel
  cfg_default.log_level     = "debug";
  cfg_default.log_hex_limit = 10000;

  return cfg_default;
}

// This function is called from PRACH worker (can wait)
void rrc_nr::add_user(uint16_t rnti)
{
  if (users.count(rnti) == 0) {
    users.insert(std::make_pair(rnti, std::unique_ptr<ue>(new ue(this, rnti))));
    rlc->add_user(rnti);
    pdcp->add_user(rnti);
    m_log->info("Added new user rnti=0x%x\n", rnti);
  } else {
    m_log->error("Adding user rnti=0x%x (already exists)\n", rnti);
  }
}

void rrc_nr::config_mac()
{
  // Fill MAC scheduler configuration for SIBs
  srsenb::sched_interface::cell_cfg_t sched_cfg;
  set_sched_cell_cfg_sib1(&sched_cfg, cfg.sib1);

  // set SIB length
  for (uint32_t i = 0; i < nof_si_messages + 1; i++) {
    sched_cfg.sibs[i].len = sib_buffer[i]->N_bytes;
  }

  // PUCCH width
  sched_cfg.nrb_pucch = SRSLTE_MAX(cfg.sr_cfg.nof_prb, cfg.cqi_cfg.nof_prb);
  m_log->info("Allocating %d PRBs for PUCCH\n", sched_cfg.nrb_pucch);

  // Copy Cell configuration
  sched_cfg.cell = cfg.cell;

  // Configure MAC scheduler
  mac->cell_cfg(&sched_cfg);
}

uint32_t rrc_nr::generate_sibs()
{
  // MIB packing
  bcch_bch_msg_s mib_msg;
  mib_s&         mib = mib_msg.msg.set_mib();
  mib                = cfg.mib;
  {
    srslte::unique_byte_buffer_t mib_buf = srslte::allocate_unique_buffer(*pool);
    asn1::bit_ref                bref(mib_buf->msg, mib_buf->get_tailroom());
    mib_msg.pack(bref);
    mib_buf->N_bytes = bref.distance_bytes();
    m_log->debug_hex(mib_buf->msg, mib_buf->N_bytes, "MIB payload (%d B)\n", mib_buf->N_bytes);
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

    msg[msg_index].msg.set_c1().set_sys_info().crit_exts.set_sys_info_r15();
    auto& sib_list = msg[msg_index].msg.c1().sys_info().crit_exts.sys_info_r15().sib_type_and_info;

    for (uint32_t mapping = 0; mapping < sched_info[sched_info_elem].sib_map_info.size(); ++mapping) {
      uint32_t sibidx = sched_info[sched_info_elem].sib_map_info[mapping].type; // SIB2 == 0
      sib_list.push_back(cfg.sibs[sibidx]);
    }
  }

  // Pack payload for all messages
  for (uint32_t msg_index = 0; msg_index < nof_messages + 1; msg_index++) {
    srslte::unique_byte_buffer_t sib = srslte::allocate_unique_buffer(*pool);
    asn1::bit_ref                bref(sib->msg, sib->get_tailroom());
    msg[msg_index].pack(bref);
    sib->N_bytes = bref.distance_bytes();
    sib_buffer.push_back(std::move(sib));

    // Log SIBs in JSON format
    log_rrc_message("SIB payload", Tx, sib_buffer.back().get(), msg[msg_index]);
  }

  return sib_buffer.size() - 1;
}

/*******************************************************************************
  MAC interface
*******************************************************************************/

int rrc_nr::read_pdu_bcch_bch(const uint32_t tti, srslte::unique_byte_buffer_t& buffer)
{
  if (mib_buffer == nullptr || buffer->get_tailroom() < mib_buffer->N_bytes) {
    return SRSLTE_ERROR;
  }
  memcpy(buffer->msg, mib_buffer->msg, mib_buffer->N_bytes);
  buffer->N_bytes = mib_buffer->N_bytes;
  return SRSLTE_SUCCESS;
}

int rrc_nr::read_pdu_bcch_dlsch(uint32_t sib_index, srslte::unique_byte_buffer_t& buffer)
{
  if (sib_index >= sib_buffer.size()) {
    m_log->error("SIB %d is not a configured SIB.\n", sib_index);
    return SRSLTE_ERROR;
  }

  if (buffer->get_tailroom() < sib_buffer[sib_index]->N_bytes) {
    m_log->error("Not enough space to fit SIB %d into buffer (%d < %d)\n",
                 sib_index,
                 buffer->get_tailroom(),
                 sib_buffer[sib_index]->N_bytes);
    return SRSLTE_ERROR;
  }

  memcpy(buffer->msg, sib_buffer[sib_index]->msg, sib_buffer[sib_index]->N_bytes);
  buffer->N_bytes = sib_buffer[sib_index]->N_bytes;

  return SRSLTE_SUCCESS;
}

void rrc_nr::get_metrics(srsenb::rrc_metrics_t& m)
{
  // return metrics
}

void rrc_nr::handle_pdu(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t pdu)
{
  if (pdu) {
    m_log->info_hex(pdu->msg, pdu->N_bytes, "Rx %s PDU", srslte::to_string(static_cast<srslte::rb_id_nr_t>(lcid)));
  }

  if (users.count(rnti) == 1) {
    switch (lcid) {
      case srsenb::RB_ID_SRB0:
        //        parse_ul_ccch(rnti, std::move(pdu));
        break;
      case srsenb::RB_ID_SRB1:
      case srsenb::RB_ID_SRB2:
        //        parse_ul_dcch(p.rnti, p.lcid, std::move(p.pdu));
        break;
      default:
        m_log->error("Rx PDU with invalid bearer id: %d", lcid);
        break;
    }
  } else {
    m_log->warning("Discarding PDU for removed rnti=0x%x\n", rnti);
  }
}

/*******************************************************************************
  PDCP interface
*******************************************************************************/
void rrc_nr::write_pdu(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t pdu)
{
  handle_pdu(rnti, lcid, std::move(pdu));
}

/*******************************************************************************
  UE class

  Every function in UE class is called from a mutex environment thus does not
  need extra protection.
*******************************************************************************/
rrc_nr::ue::ue(rrc_nr* parent_, uint16_t rnti_) : parent(parent_), rnti(rnti_)
{
  pool = srslte::byte_buffer_pool::get_instance();

  // setup periodic RRCSetup send
  rrc_setup_periodic_timer = parent->timers->get_unique_timer();
  rrc_setup_periodic_timer.set(5000, [this](uint32_t tid) {
    send_connection_setup();
    rrc_setup_periodic_timer.run();
  });
  rrc_setup_periodic_timer.run();
}

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
  srslte::unique_byte_buffer_t pdu = srslte::allocate_unique_buffer(*pool);
  if (pdu == nullptr) {
    parent->m_log->error("Allocating pdu\n");
  }
  asn1::bit_ref bref(pdu->msg, pdu->get_tailroom());
  if (dl_ccch_msg->pack(bref) == asn1::SRSASN_ERROR_ENCODE_FAIL) {
    parent->m_log->error("Failed to pack DL-CCCH message. Discarding msg.\n");
  }
  pdu->N_bytes = 1u + (uint32_t)bref.distance_bytes(pdu->msg);

  char buf[32] = {};
  sprintf(buf, "SRB0 - rnti=0x%x", rnti);
  parent->log_rrc_message(buf, Tx, pdu.get(), *dl_ccch_msg);
  parent->rlc->write_sdu(rnti, RB_ID_SRB0, std::move(pdu));
}

} // namespace srsenb