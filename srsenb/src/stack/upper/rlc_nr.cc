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

#include "srsenb/hdr/stack/upper/rlc_nr.h"
#include "srslte/interfaces/nr_common_interface_types.h"

namespace srsenb {

rlc_nr::rlc_nr(const char* logname) : m_log(logname), pool(srslte::byte_buffer_pool::get_instance()) {}

void rlc_nr::init(pdcp_interface_rlc_nr* pdcp_,
                  rrc_interface_rlc_nr*  rrc_,
                  mac_interface_rlc_nr*  mac_,
                  srslte::timer_handler* timers_)
{
  m_pdcp = pdcp_;
  m_rrc  = rrc_;
  m_mac  = mac_;
  timers = timers_;
}

void rlc_nr::stop()
{
  for (auto& user : users) {
    user.second.m_rlc->stop();
  }
  users.clear();
}

void rlc_nr::add_user(uint16_t rnti)
{
  if (users.count(rnti) == 0) {
    user_interface user_itf;
    user_itf.rnti   = rnti;
    user_itf.m_pdcp = m_pdcp;
    user_itf.m_rrc  = m_rrc;
    user_itf.parent = this;
    user_itf.m_rlc.reset(new srslte::rlc(m_log->get_service_name().c_str()));
    users[rnti] = std::move(user_itf);
    users[rnti].m_rlc->init(&users[rnti], &users[rnti], timers, (int)srslte::rb_id_nr_t::NR_SRB0);
  }
}

void rlc_nr::rem_user(uint16_t rnti)
{
  if (users.count(rnti)) {
    users[rnti].m_rlc->stop();
    users.erase(rnti);
  } else {
    m_log->error("Removing rnti=0x%x. Already removed\n", rnti);
  }
}

void rlc_nr::clear_buffer(uint16_t rnti)
{
  if (users.count(rnti)) {
    users[rnti].m_rlc->empty_queue();
    for (int i = 0; i < SRSLTE_N_RADIO_BEARERS; i++) {
      m_mac->rlc_buffer_state(rnti, i, 0, 0);
    }
    m_log->info("Cleared buffer rnti=0x%x\n", rnti);
  }
}

void rlc_nr::add_bearer(uint16_t rnti, uint32_t lcid, srslte::rlc_config_t cnfg)
{
  if (users.count(rnti)) {
    users[rnti].m_rlc->add_bearer(lcid, cnfg);
  }
}

void rlc_nr::add_bearer_mrb(uint16_t rnti, uint32_t lcid)
{
  if (users.count(rnti)) {
    users[rnti].m_rlc->add_bearer_mrb(lcid);
  }
}

void rlc_nr::read_pdu_pcch(uint8_t* payload, uint32_t buffer_size)
{
  m_rrc->read_pdu_pcch(payload, buffer_size);
}

int rlc_nr::read_pdu(uint16_t rnti, uint32_t lcid, uint8_t* payload, uint32_t nof_bytes)
{
  int      ret;
  uint32_t tx_queue;

  if (users.count(rnti)) {
    if (rnti != SRSLTE_MRNTI) {
      ret      = users[rnti].m_rlc->read_pdu(lcid, payload, nof_bytes);
      tx_queue = users[rnti].m_rlc->get_buffer_state(lcid);
    } else {
      ret      = users[rnti].m_rlc->read_pdu_mch(lcid, payload, nof_bytes);
      tx_queue = users[rnti].m_rlc->get_total_mch_buffer_state(lcid);
    }
    // In the eNodeB, there is no polling for buffer state from the scheduler, thus
    // communicate buffer state every time a PDU is read

    uint32_t retx_queue = 0;
    m_log->debug("Buffer state PDCP: rnti=0x%x, lcid=%d, tx_queue=%d\n", rnti, lcid, tx_queue);
    m_mac->rlc_buffer_state(rnti, lcid, tx_queue, retx_queue);
  } else {
    ret = SRSLTE_ERROR;
  }
  return ret;
}

void rlc_nr::write_pdu(uint16_t rnti, uint32_t lcid, uint8_t* payload, uint32_t nof_bytes)
{
  if (users.count(rnti)) {
    users[rnti].m_rlc->write_pdu(lcid, payload, nof_bytes);

    // In the eNodeB, there is no polling for buffer state from the scheduler, thus
    // communicate buffer state every time a new PDU is written
    uint32_t tx_queue   = users[rnti].m_rlc->get_buffer_state(lcid);
    uint32_t retx_queue = 0;
    m_log->debug("Buffer state PDCP: rnti=0x%x, lcid=%d, tx_queue=%d\n", rnti, lcid, tx_queue);
    m_mac->rlc_buffer_state(rnti, lcid, tx_queue, retx_queue);
  }
}

// void rlc::read_pdu_bcch_dlsch(uint32_t sib_index, uint8_t* payload)
//{
//  // RLC is transparent for BCCH
//  m_rrc->read_pdu_bcch_dlsch(sib_index, payload);
//}

void rlc_nr::write_sdu(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t sdu)
{
  uint32_t tx_queue;

  if (users.count(rnti)) {
    if (rnti != SRSLTE_MRNTI) {
      users[rnti].m_rlc->write_sdu(lcid, std::move(sdu));
      tx_queue = users[rnti].m_rlc->get_buffer_state(lcid);
    } else {
      users[rnti].m_rlc->write_sdu_mch(lcid, std::move(sdu));
      tx_queue = users[rnti].m_rlc->get_total_mch_buffer_state(lcid);
    }
    // In the eNodeB, there is no polling for buffer state from the scheduler, thus
    // communicate buffer state every time a new SDU is written

    uint32_t retx_queue = 0;
    m_mac->rlc_buffer_state(rnti, lcid, tx_queue, retx_queue);
    m_log->info("Buffer state: rnti=0x%x, lcid=%d, tx_queue=%d\n", rnti, lcid, tx_queue);
  }
}

bool rlc_nr::rb_is_um(uint16_t rnti, uint32_t lcid)
{
  bool ret = false;
  if (users.count(rnti)) {
    ret = users[rnti].m_rlc->rb_is_um(lcid);
  }
  return ret;
}

bool rlc_nr::sdu_queue_is_full(uint16_t rnti, uint32_t lcid)
{
  bool ret = false;
  if (users.count(rnti)) {
    ret = users[rnti].m_rlc->sdu_queue_is_full(lcid);
  }
  return ret;
}
void rlc_nr::user_interface::max_retx_attempted()
{
  m_rrc->max_retx_attempted(rnti);
}

void rlc_nr::user_interface::write_pdu(uint32_t lcid, srslte::unique_byte_buffer_t sdu)
{
  if (lcid == (int)srslte::rb_id_nr_t::NR_SRB0) {
    m_rrc->write_pdu(rnti, lcid, std::move(sdu));
  } else {
    m_pdcp->write_pdu(rnti, lcid, std::move(sdu));
  }
}

void rlc_nr::user_interface::write_pdu_bcch_bch(srslte::unique_byte_buffer_t sdu)
{
  ERROR("Error: Received BCCH from ue=%d\n", rnti);
}

void rlc_nr::user_interface::write_pdu_bcch_dlsch(srslte::unique_byte_buffer_t sdu)
{
  ERROR("Error: Received BCCH from ue=%d\n", rnti);
}

void rlc_nr::user_interface::write_pdu_pcch(srslte::unique_byte_buffer_t sdu)
{
  ERROR("Error: Received PCCH from ue=%d\n", rnti);
}

std::string rlc_nr::user_interface::get_rb_name(uint32_t lcid)
{
  return srslte::to_string(static_cast<srslte::rb_id_nr_t>(lcid));
}

} // namespace srsenb