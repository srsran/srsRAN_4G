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

#include "srsenb/hdr/stack/upper/pdcp_nr.h"
#include "lib/include/srslte/interfaces/nr_common_interface_types.h"

namespace srsenb {

pdcp_nr::pdcp_nr(srslte::task_sched_handle task_sched_, const char* logname) :
  task_sched(task_sched_),
  m_log(logname),
  pool(srslte::byte_buffer_pool::get_instance())
{}

void pdcp_nr::init(const pdcp_nr_args_t&   args_,
                   rlc_interface_pdcp_nr*  rlc_,
                   rrc_interface_pdcp_nr*  rrc_,
                   sdap_interface_pdcp_nr* sdap_)
{
  m_args = args_;
  m_rlc  = rlc_;
  m_rrc  = rrc_;
  m_sdap = sdap_;

  m_log->set_level(m_args.log_level);
  m_log->set_hex_limit(m_args.log_hex_limit);
}

void pdcp_nr::stop()
{
  for (auto& user : users) {
    user.second.pdcp->stop();
  }
  users.clear();
}

void pdcp_nr::add_user(uint16_t rnti)
{
  if (users.count(rnti) == 0) {
    users[rnti].pdcp.reset(new srslte::pdcp(task_sched, "PDCP"));
    users[rnti].rlc_itf.rnti  = rnti;
    users[rnti].sdap_itf.rnti = rnti;
    users[rnti].rrc_itf.rnti  = rnti;
    users[rnti].rlc_itf.rlc   = m_rlc;
    users[rnti].rrc_itf.rrc   = m_rrc;
    users[rnti].sdap_itf.sdap = m_sdap;
    users[rnti].pdcp->init(&users[rnti].rlc_itf, &users[rnti].rrc_itf, &users[rnti].sdap_itf);
  }
}

void pdcp_nr::rem_user(uint16_t rnti)
{
  users.erase(rnti);
}

void pdcp_nr::add_bearer(uint16_t rnti, uint32_t lcid, srslte::pdcp_config_t cfg)
{
  if (users.count(rnti)) {
    users[rnti].pdcp->add_bearer(lcid, cfg);
  }
}

void pdcp_nr::reset(uint16_t rnti)
{
  if (users.count(rnti)) {
    users[rnti].pdcp->reset();
  }
}

void pdcp_nr::config_security(uint16_t rnti, uint32_t lcid, srslte::as_security_config_t sec_cfg)
{
  if (users.count(rnti)) {
    users[rnti].pdcp->config_security(lcid, sec_cfg);
  }
}

void pdcp_nr::enable_integrity(uint16_t rnti, uint32_t lcid)
{
  users[rnti].pdcp->enable_integrity(lcid, srslte::DIRECTION_TXRX);
}

void pdcp_nr::enable_encryption(uint16_t rnti, uint32_t lcid)
{
  users[rnti].pdcp->enable_encryption(lcid, srslte::DIRECTION_TXRX);
}

void pdcp_nr::write_pdu(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t sdu)
{
  if (users.count(rnti)) {
    users[rnti].pdcp->write_pdu(lcid, std::move(sdu));
  } else {
    m_log->error("Can't write PDU. RNTI=0x%X doesn't exist.\n", rnti);
  }
}

void pdcp_nr::write_sdu(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t sdu)
{
  if (users.count(rnti)) {
    users[rnti].pdcp->write_sdu(lcid, std::move(sdu));
  } else {
    m_log->error("Can't write SDU. RNTI=0x%X doesn't exist.\n", rnti);
  }
}

void pdcp_nr::user_interface_sdap::write_pdu(uint32_t lcid, srslte::unique_byte_buffer_t pdu)
{
  sdap->write_pdu(rnti, lcid, std::move(pdu));
}

void pdcp_nr::user_interface_rlc::write_sdu(uint32_t lcid, srslte::unique_byte_buffer_t sdu)
{
  rlc->write_sdu(rnti, lcid, std::move(sdu));
}

void pdcp_nr::user_interface_rlc::discard_sdu(uint32_t lcid, uint32_t discard_sn)
{
  fprintf(stderr, "discard_sdu method not implemented.\n");
}

bool pdcp_nr::user_interface_rlc::rb_is_um(uint32_t lcid)
{
  return rlc->rb_is_um(rnti, lcid);
}

bool pdcp_nr::user_interface_rlc::sdu_queue_is_full(uint32_t lcid)
{
  return rlc->sdu_queue_is_full(rnti, lcid);
}

void pdcp_nr::user_interface_rrc::write_pdu(uint32_t lcid, srslte::unique_byte_buffer_t pdu)
{
  rrc->write_pdu(rnti, lcid, std::move(pdu));
}

void pdcp_nr::user_interface_rrc::write_pdu_bcch_bch(srslte::unique_byte_buffer_t pdu)
{
  ERROR("Error: Received BCCH from ue=%d\n", rnti);
}

void pdcp_nr::user_interface_rrc::write_pdu_bcch_dlsch(srslte::unique_byte_buffer_t pdu)
{
  ERROR("Error: Received BCCH from ue=%d\n", rnti);
}

void pdcp_nr::user_interface_rrc::write_pdu_pcch(srslte::unique_byte_buffer_t pdu)
{
  ERROR("Error: Received PCCH from ue=%d\n", rnti);
}

std::string pdcp_nr::user_interface_rrc::get_rb_name(uint32_t lcid)
{
  return srslte::to_string(static_cast<srslte::rb_id_nr_t>(lcid));
}

} // namespace srsenb