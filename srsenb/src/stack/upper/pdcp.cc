/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

#include "srsenb/hdr/stack/upper/pdcp.h"
#include "srsenb/hdr/stack/upper/common_enb.h"

namespace srsenb {

pdcp::pdcp(srslte::log* log_) : log_h(log_), pool(srslte::byte_buffer_pool::get_instance()) {}

void pdcp::init(rlc_interface_pdcp* rlc_, rrc_interface_pdcp* rrc_, gtpu_interface_pdcp* gtpu_)
{
  rlc   = rlc_; 
  rrc   = rrc_; 
  gtpu  = gtpu_;

  pthread_rwlock_init(&rwlock, NULL);
}

void pdcp::stop()
{
  pthread_rwlock_wrlock(&rwlock);
  for(std::map<uint32_t, user_interface>::iterator iter=users.begin(); iter!=users.end(); ++iter) {
    clear_user(&iter->second);
  }
  users.clear();
  pthread_rwlock_unlock(&rwlock);
  pthread_rwlock_destroy(&rwlock);
}

void pdcp::add_user(uint16_t rnti)
{
  pthread_rwlock_rdlock(&rwlock);
  if (users.count(rnti) == 0) {
    srslte::pdcp* obj = new srslte::pdcp(log_h);
    obj->init(&users[rnti].rlc_itf, &users[rnti].rrc_itf, &users[rnti].gtpu_itf);
    users[rnti].rlc_itf.rnti  = rnti;
    users[rnti].gtpu_itf.rnti = rnti;
    users[rnti].rrc_itf.rnti  = rnti;
    
    users[rnti].rrc_itf.rrc   = rrc;
    users[rnti].rlc_itf.rlc   = rlc;
    users[rnti].gtpu_itf.gtpu = gtpu;
    users[rnti].pdcp = obj;
  }
  pthread_rwlock_unlock(&rwlock);
}

// Private unlocked deallocation of user
void pdcp::clear_user(user_interface *ue)
{
  ue->pdcp->stop();
  delete ue->pdcp;
  ue->pdcp = NULL;
}

void pdcp::rem_user(uint16_t rnti)
{
  pthread_rwlock_wrlock(&rwlock);
  if (users.count(rnti)) {
    clear_user(&users[rnti]);
    users.erase(rnti);
  }
  pthread_rwlock_unlock(&rwlock);
}

void pdcp::add_bearer(uint16_t rnti, uint32_t lcid, srslte::pdcp_config_t cfg)
{
  pthread_rwlock_rdlock(&rwlock);
  if (users.count(rnti)) {
    if(rnti != SRSLTE_MRNTI){
      users[rnti].pdcp->add_bearer(lcid, cfg);
    } else {
      users[rnti].pdcp->add_bearer_mrb(lcid, cfg);
    }
  }
  pthread_rwlock_unlock(&rwlock);
}

void pdcp::reset(uint16_t rnti)
{
  pthread_rwlock_rdlock(&rwlock);
  if (users.count(rnti)) {
    users[rnti].pdcp->reset();
  }
  pthread_rwlock_unlock(&rwlock);
}

void pdcp::config_security(uint16_t rnti, uint32_t lcid, uint8_t* k_rrc_enc_, uint8_t* k_rrc_int_, uint8_t* k_up_enc_,
                           srslte::CIPHERING_ALGORITHM_ID_ENUM cipher_algo_,
                           srslte::INTEGRITY_ALGORITHM_ID_ENUM integ_algo_)
{
  pthread_rwlock_rdlock(&rwlock);
  if (users.count(rnti)) {
    users[rnti].pdcp->config_security(lcid, k_rrc_enc_, k_rrc_int_, k_up_enc_, cipher_algo_, integ_algo_);
  }
  pthread_rwlock_unlock(&rwlock);
}

void pdcp::enable_integrity(uint16_t rnti, uint32_t lcid)
{
  pthread_rwlock_rdlock(&rwlock);
  users[rnti].pdcp->enable_integrity(lcid);
  pthread_rwlock_unlock(&rwlock);
}

void pdcp::enable_encryption(uint16_t rnti, uint32_t lcid)
{
  pthread_rwlock_rdlock(&rwlock);
  users[rnti].pdcp->enable_encryption(lcid);
  pthread_rwlock_unlock(&rwlock);
}

void pdcp::write_pdu(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t sdu)
{
  pthread_rwlock_rdlock(&rwlock);
  if (users.count(rnti)) {
    users[rnti].pdcp->write_pdu(lcid, std::move(sdu));
  }
  pthread_rwlock_unlock(&rwlock);
}

void pdcp::write_sdu(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t sdu)
{
  pthread_rwlock_rdlock(&rwlock);
  if (users.count(rnti)) {
    if(rnti != SRSLTE_MRNTI){
      users[rnti].pdcp->write_sdu(lcid, std::move(sdu));
    }else {
      users[rnti].pdcp->write_sdu_mch(lcid, std::move(sdu));
    }
  }
  pthread_rwlock_unlock(&rwlock);
}

void pdcp::user_interface_gtpu::write_pdu(uint32_t lcid, srslte::unique_byte_buffer_t pdu)
{
  gtpu->write_pdu(rnti, lcid, std::move(pdu));
}

void pdcp::user_interface_rlc::write_sdu(uint32_t lcid, srslte::unique_byte_buffer_t sdu, bool blocking)
{
  rlc->write_sdu(rnti, lcid, std::move(sdu));
}

bool pdcp::user_interface_rlc::rb_is_um(uint32_t lcid) {
  return rlc->rb_is_um(rnti, lcid);
}

void pdcp::user_interface_rrc::write_pdu(uint32_t lcid, srslte::unique_byte_buffer_t pdu)
{
  rrc->write_pdu(rnti, lcid, std::move(pdu));
}

void pdcp::user_interface_rrc::write_pdu_bcch_bch(srslte::unique_byte_buffer_t pdu)
{
  ERROR("Error: Received BCCH from ue=%d\n", rnti);
}

void pdcp::user_interface_rrc::write_pdu_bcch_dlsch(srslte::unique_byte_buffer_t pdu)
{
  ERROR("Error: Received BCCH from ue=%d\n", rnti);
}

void pdcp::user_interface_rrc::write_pdu_pcch(srslte::unique_byte_buffer_t pdu)
{
  ERROR("Error: Received PCCH from ue=%d\n", rnti);
}

std::string pdcp::user_interface_rrc::get_rb_name(uint32_t lcid)
{
  return std::string(rb_id_text[lcid]);
}

}
