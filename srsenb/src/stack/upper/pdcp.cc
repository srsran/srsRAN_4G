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

#include "srsenb/hdr/stack/upper/pdcp.h"
#include "srsenb/hdr/common/common_enb.h"
#include "srsran/interfaces/enb_gtpu_interfaces.h"
#include "srsran/interfaces/enb_rlc_interfaces.h"
#include "srsran/interfaces/enb_rrc_interface_pdcp.h"

namespace srsenb {

pdcp::pdcp(srsran::task_sched_handle task_sched_, srslog::basic_logger& logger_) :
  task_sched(task_sched_), logger(logger_)
{}

void pdcp::init(rlc_interface_pdcp* rlc_, rrc_interface_pdcp* rrc_, gtpu_interface_pdcp* gtpu_)
{
  rlc  = rlc_;
  rrc  = rrc_;
  gtpu = gtpu_;
}

void pdcp::stop()
{
  for (std::map<uint32_t, user_interface>::iterator iter = users.begin(); iter != users.end(); ++iter) {
    clear_user(&iter->second);
  }
  users.clear();
}

void pdcp::add_user(uint16_t rnti)
{
  if (users.count(rnti) == 0) {
    unique_rnti_ptr<srsran::pdcp> obj = make_rnti_obj<srsran::pdcp>(rnti, task_sched, logger.id().c_str());
    obj->init(&users[rnti].rlc_itf, &users[rnti].rrc_itf, &users[rnti].gtpu_itf);
    users[rnti].rlc_itf.rnti  = rnti;
    users[rnti].gtpu_itf.rnti = rnti;
    users[rnti].rrc_itf.rnti  = rnti;

    users[rnti].rrc_itf.rrc   = rrc;
    users[rnti].rlc_itf.rlc   = rlc;
    users[rnti].gtpu_itf.gtpu = gtpu;
    users[rnti].pdcp          = std::move(obj);
  }
}

// Private unlocked deallocation of user
void pdcp::clear_user(user_interface* ue)
{
  ue->pdcp->stop();
  ue->pdcp.reset();
}

void pdcp::rem_user(uint16_t rnti)
{
  if (users.count(rnti)) {
    clear_user(&users[rnti]);
    users.erase(rnti);
  }
}

void pdcp::add_bearer(uint16_t rnti, uint32_t lcid, const srsran::pdcp_config_t& cfg)
{
  if (users.count(rnti)) {
    if (rnti != SRSRAN_MRNTI) {
      users[rnti].pdcp->add_bearer(lcid, cfg);
    } else {
      users[rnti].pdcp->add_bearer_mrb(lcid, cfg);
    }
  }
}

void pdcp::del_bearer(uint16_t rnti, uint32_t lcid)
{
  if (users.count(rnti)) {
    users[rnti].pdcp->del_bearer(lcid);
  }
}

void pdcp::set_enabled(uint16_t rnti, uint32_t lcid, bool enabled)
{
  if (users.count(rnti)) {
    users[rnti].pdcp->set_enabled(lcid, enabled);
  }
}

void pdcp::reset(uint16_t rnti)
{
  if (users.count(rnti)) {
    users[rnti].pdcp->reset();
  }
}

void pdcp::config_security(uint16_t rnti, uint32_t lcid, const srsran::as_security_config_t& sec_cfg)
{
  if (users.count(rnti)) {
    users[rnti].pdcp->config_security(lcid, sec_cfg);
  }
}

void pdcp::enable_integrity(uint16_t rnti, uint32_t lcid)
{
  users[rnti].pdcp->enable_integrity(lcid, srsran::DIRECTION_TXRX);
}

void pdcp::enable_encryption(uint16_t rnti, uint32_t lcid)
{
  users[rnti].pdcp->enable_encryption(lcid, srsran::DIRECTION_TXRX);
}

bool pdcp::get_bearer_state(uint16_t rnti, uint32_t lcid, srsran::pdcp_lte_state_t* state)
{
  if (users.count(rnti) == 0) {
    return false;
  }
  return users[rnti].pdcp->get_bearer_state(lcid, state);
}

bool pdcp::set_bearer_state(uint16_t rnti, uint32_t lcid, const srsran::pdcp_lte_state_t& state)
{
  if (users.count(rnti) == 0) {
    return false;
  }
  return users[rnti].pdcp->set_bearer_state(lcid, state);
}

void pdcp::reestablish(uint16_t rnti)
{
  if (users.count(rnti) == 0) {
    return;
  }
  users[rnti].pdcp->reestablish();
}

void pdcp::send_status_report(uint16_t rnti)
{
  if (users.count(rnti) == 0) {
    return;
  }
  users[rnti].pdcp->send_status_report();
}

void pdcp::notify_delivery(uint16_t rnti, uint32_t lcid, const srsran::pdcp_sn_vector_t& pdcp_sns)
{
  if (users.count(rnti)) {
    users[rnti].pdcp->notify_delivery(lcid, pdcp_sns);
  }
}

void pdcp::notify_failure(uint16_t rnti, uint32_t lcid, const srsran::pdcp_sn_vector_t& pdcp_sns)
{
  if (users.count(rnti)) {
    users[rnti].pdcp->notify_failure(lcid, pdcp_sns);
  }
}

void pdcp::write_sdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t sdu, int pdcp_sn)
{
  if (users.count(rnti)) {
    if (rnti != SRSRAN_MRNTI) {
      // TODO: Handle PDCP SN coming from GTPU
      users[rnti].pdcp->write_sdu(lcid, std::move(sdu), pdcp_sn);
    } else {
      users[rnti].pdcp->write_sdu_mch(lcid, std::move(sdu));
    }
  }
}

void pdcp::send_status_report(uint16_t rnti, uint32_t lcid)
{
  if (users.count(rnti)) {
    users[rnti].pdcp->send_status_report(lcid);
  }
}

std::map<uint32_t, srsran::unique_byte_buffer_t> pdcp::get_buffered_pdus(uint16_t rnti, uint32_t lcid)
{
  if (users.count(rnti)) {
    return users[rnti].pdcp->get_buffered_pdus(lcid);
  }
  return {};
}

void pdcp::write_pdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t sdu)
{
  if (users.count(rnti)) {
    users[rnti].pdcp->write_pdu(lcid, std::move(sdu));
  }
}

void pdcp::user_interface_gtpu::write_pdu(uint32_t lcid, srsran::unique_byte_buffer_t pdu)
{
  gtpu->write_pdu(rnti, lcid, std::move(pdu));
}

void pdcp::user_interface_rlc::write_sdu(uint32_t lcid, srsran::unique_byte_buffer_t sdu)
{
  rlc->write_sdu(rnti, lcid, std::move(sdu));
}

void pdcp::user_interface_rlc::discard_sdu(uint32_t lcid, uint32_t discard_sn)
{
  rlc->discard_sdu(rnti, lcid, discard_sn);
}

bool pdcp::user_interface_rlc::rb_is_um(uint32_t lcid)
{
  return rlc->rb_is_um(rnti, lcid);
}

bool pdcp::user_interface_rlc::is_suspended(uint32_t lcid)
{
  return rlc->is_suspended(rnti, lcid);
}

bool pdcp::user_interface_rlc::sdu_queue_is_full(uint32_t lcid)
{
  return rlc->sdu_queue_is_full(rnti, lcid);
}

void pdcp::user_interface_rrc::write_pdu(uint32_t lcid, srsran::unique_byte_buffer_t pdu)
{
  rrc->write_pdu(rnti, lcid, std::move(pdu));
}

void pdcp::user_interface_rrc::notify_pdcp_integrity_error(uint32_t lcid)
{
  rrc->notify_pdcp_integrity_error(rnti, lcid);
}

void pdcp::user_interface_rrc::write_pdu_bcch_bch(srsran::unique_byte_buffer_t pdu)
{
  ERROR("Error: Received BCCH from ue=%d", rnti);
}

void pdcp::user_interface_rrc::write_pdu_bcch_dlsch(srsran::unique_byte_buffer_t pdu)
{
  ERROR("Error: Received BCCH from ue=%d", rnti);
}

void pdcp::user_interface_rrc::write_pdu_pcch(srsran::unique_byte_buffer_t pdu)
{
  ERROR("Error: Received PCCH from ue=%d", rnti);
}

const char* pdcp::user_interface_rrc::get_rb_name(uint32_t lcid)
{
  return srsenb::get_rb_name(lcid);
}

void pdcp::get_metrics(pdcp_metrics_t& m, const uint32_t nof_tti)
{
  m.ues.resize(users.size());
  size_t count = 0;
  for (auto& user : users) {
    user.second.pdcp->get_metrics(m.ues[count], nof_tti);
    count++;
  }
}

} // namespace srsenb
