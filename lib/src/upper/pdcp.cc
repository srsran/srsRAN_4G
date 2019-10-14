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

#include "srslte/upper/pdcp.h"

namespace srslte {

pdcp::pdcp(srslte::log* log_) : pdcp_log(log_)
{
  pthread_rwlock_init(&rwlock, NULL);
}

pdcp::~pdcp()
{
  // destroy all remaining entities
  pthread_rwlock_wrlock(&rwlock);
  for (pdcp_map_t::iterator it = pdcp_array.begin(); it != pdcp_array.end(); ++it) {
    delete(it->second);
  }
  pdcp_array.clear();

  for (pdcp_map_t::iterator it = pdcp_array_mrb.begin(); it != pdcp_array_mrb.end(); ++it) {
    delete (it->second);
  }
  pdcp_array_mrb.clear();

  pthread_rwlock_unlock(&rwlock);
  pthread_rwlock_destroy(&rwlock);
}

void pdcp::init(srsue::rlc_interface_pdcp* rlc_, srsue::rrc_interface_pdcp* rrc_, srsue::gw_interface_pdcp* gw_)
{
  rlc          = rlc_;
  rrc          = rrc_;
  gw           = gw_;
}

void pdcp::stop()
{
}

void pdcp::reestablish() {
  pthread_rwlock_rdlock(&rwlock);
  for (pdcp_map_t::iterator it = pdcp_array.begin(); it != pdcp_array.end(); ++it) {
    it->second->reestablish();
  }
  pthread_rwlock_unlock(&rwlock);
}

void pdcp::reestablish(uint32_t lcid)
{
  pthread_rwlock_rdlock(&rwlock);
  if (valid_lcid(lcid)) {
    pdcp_array.at(lcid)->reestablish();
  }
  pthread_rwlock_unlock(&rwlock);
}

void pdcp::reset()
{
  // destroy all bearers
  pthread_rwlock_wrlock(&rwlock);
  for (pdcp_map_t::iterator it = pdcp_array.begin(); it != pdcp_array.end(); /* post increment in erase */ ) {
    it->second->reset();
    delete(it->second);
    pdcp_array.erase(it++);
  }
  pthread_rwlock_unlock(&rwlock);
}

/*******************************************************************************
  RRC/GW interface
*******************************************************************************/
bool pdcp::is_lcid_enabled(uint32_t lcid)
{
  pthread_rwlock_rdlock(&rwlock);
  bool ret = false;
  if (valid_lcid(lcid)) {
    ret = pdcp_array.at(lcid)->is_active();
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

void pdcp::write_sdu(uint32_t lcid, unique_byte_buffer_t sdu, bool blocking)
{
  pthread_rwlock_rdlock(&rwlock);
  if (valid_lcid(lcid)) {
    pdcp_array.at(lcid)->write_sdu(std::move(sdu), blocking);
  } else {
    pdcp_log->warning("Writing sdu: lcid=%d. Deallocating sdu\n", lcid);
  }
  pthread_rwlock_unlock(&rwlock);
}

void pdcp::write_sdu_mch(uint32_t lcid, unique_byte_buffer_t sdu)
{
  pthread_rwlock_rdlock(&rwlock);
  if (valid_mch_lcid(lcid)){
    pdcp_array_mrb.at(lcid)->write_sdu(std::move(sdu), true);
  }
  pthread_rwlock_unlock(&rwlock);
}

void pdcp::add_bearer(uint32_t lcid, pdcp_config_t cfg)
{
  pthread_rwlock_wrlock(&rwlock);
  if (not valid_lcid(lcid)) {
    if (not pdcp_array.insert(pdcp_map_pair_t(lcid, new pdcp_entity_lte())).second) {
      pdcp_log->error("Error inserting PDCP entity in to array\n.");
      goto unlock_and_exit;
    }
    pdcp_array.at(lcid)->init(rlc, rrc, gw, pdcp_log, lcid, cfg);
    pdcp_log->info("Add %s (lcid=%d, bearer_id=%d, sn_len=%dbits)\n",
                   rrc->get_rb_name(lcid).c_str(),
                   lcid,
                   cfg.bearer_id,
                   cfg.sn_len);
  } else {
    pdcp_log->warning("Bearer %s already configured. Reconfiguration not supported\n", rrc->get_rb_name(lcid).c_str());
  }
unlock_and_exit:
  pthread_rwlock_unlock(&rwlock);
}

void pdcp::add_bearer_mrb(uint32_t lcid, pdcp_config_t cfg)
{
  pthread_rwlock_wrlock(&rwlock);
  if (not valid_mch_lcid(lcid)) {
    if (not pdcp_array_mrb.insert(pdcp_map_pair_t(lcid, new pdcp_entity_lte())).second) {
      pdcp_log->error("Error inserting PDCP entity in to array\n.");
      goto unlock_and_exit;
    }
    pdcp_array_mrb.at(lcid)->init(rlc, rrc, gw, pdcp_log, lcid, cfg);
    pdcp_log->info("Add %s (lcid=%d, bearer_id=%d, sn_len=%dbits)\n",
                   rrc->get_rb_name(lcid).c_str(),
                   lcid,
                   cfg.bearer_id,
                   cfg.sn_len);
  } else {
    pdcp_log->warning("Bearer %s already configured. Reconfiguration not supported\n", rrc->get_rb_name(lcid).c_str());
  }
unlock_and_exit:
  pthread_rwlock_unlock(&rwlock);
}

void pdcp::del_bearer(uint32_t lcid)
{
  pthread_rwlock_wrlock(&rwlock);
  if (valid_lcid(lcid)) {
    pdcp_map_t::iterator it = pdcp_array.find(lcid);
    delete(it->second);
    pdcp_array.erase(it);
    pdcp_log->warning("Deleted PDCP bearer %s\n", rrc->get_rb_name(lcid).c_str());
  } else {
    pdcp_log->warning("Can't delete bearer %s. Bearer doesn't exist.\n", rrc->get_rb_name(lcid).c_str());
  }
  pthread_rwlock_unlock(&rwlock);
}

void pdcp::change_lcid(uint32_t old_lcid, uint32_t new_lcid)
{
  pthread_rwlock_wrlock(&rwlock);

  // make sure old LCID exists and new LCID is still free
  if (valid_lcid(old_lcid) && not valid_lcid(new_lcid)) {
    // insert old PDCP entity into new LCID
    pdcp_map_t::iterator it          = pdcp_array.find(old_lcid);
    pdcp_entity_lte*     pdcp_entity = it->second;
    if (not pdcp_array.insert(pdcp_map_pair_t(new_lcid, pdcp_entity)).second) {
      pdcp_log->error("Error inserting PDCP entity into array\n.");
      goto exit;
    }
    // erase from old position
    pdcp_array.erase(it);
    pdcp_log->warning("Changed LCID of PDCP bearer from %d to %d\n", old_lcid, new_lcid);
  } else {
    pdcp_log->error(
        "Can't change PDCP of bearer %s from %d to %d. Bearer doesn't exist or new LCID already occupied.\n",
        rrc->get_rb_name(old_lcid).c_str(),
        old_lcid,
        new_lcid);
  }
exit:
  pthread_rwlock_unlock(&rwlock);
}

void pdcp::config_security(uint32_t lcid,
                           uint8_t *k_rrc_enc,
                           uint8_t *k_rrc_int,
                           uint8_t *k_up_enc,
                           CIPHERING_ALGORITHM_ID_ENUM cipher_algo,
                           INTEGRITY_ALGORITHM_ID_ENUM integ_algo)
{
  pthread_rwlock_rdlock(&rwlock);
  if (valid_lcid(lcid)) {
    pdcp_array.at(lcid)->config_security(k_rrc_enc, k_rrc_int, k_up_enc, nullptr, cipher_algo, integ_algo);
  }
  pthread_rwlock_unlock(&rwlock);
}

void pdcp::config_security_all(uint8_t* k_rrc_enc, uint8_t* k_rrc_int, uint8_t* k_up_enc,
                               CIPHERING_ALGORITHM_ID_ENUM cipher_algo, INTEGRITY_ALGORITHM_ID_ENUM integ_algo)
{
  pthread_rwlock_rdlock(&rwlock);
  for (pdcp_map_t::iterator it = pdcp_array.begin(); it != pdcp_array.end(); ++it) {
    it->second->config_security(k_rrc_enc, k_rrc_int, k_up_enc, nullptr, cipher_algo, integ_algo);
  }
  pthread_rwlock_unlock(&rwlock);
}

void pdcp::enable_integrity(uint32_t lcid)
{
  pthread_rwlock_rdlock(&rwlock);
  if (valid_lcid(lcid)) {
    pdcp_array.at(lcid)->enable_integrity();
  }
  pthread_rwlock_unlock(&rwlock);
}

void pdcp::enable_encryption(uint32_t lcid)
{
  pthread_rwlock_rdlock(&rwlock);
  if (valid_lcid(lcid)) {
    pdcp_array.at(lcid)->enable_encryption();
  }
  pthread_rwlock_unlock(&rwlock);
}

/*******************************************************************************
  RLC interface
*******************************************************************************/
void pdcp::write_pdu(uint32_t lcid, unique_byte_buffer_t pdu)
{
  pthread_rwlock_rdlock(&rwlock);
  if (valid_lcid(lcid)) {
    pdcp_array.at(lcid)->write_pdu(std::move(pdu));
  } else {
    pdcp_log->warning("Writing pdu: lcid=%d. Deallocating pdu\n", lcid);
  }
  pthread_rwlock_unlock(&rwlock);
}

void pdcp::write_pdu_bcch_bch(unique_byte_buffer_t sdu)
{
  rrc->write_pdu_bcch_bch(std::move(sdu));
}

void pdcp::write_pdu_bcch_dlsch(unique_byte_buffer_t sdu)
{
  rrc->write_pdu_bcch_dlsch(std::move(sdu));
}

void pdcp::write_pdu_pcch(unique_byte_buffer_t sdu)
{
  rrc->write_pdu_pcch(std::move(sdu));
}

void pdcp::write_pdu_mch(uint32_t lcid, unique_byte_buffer_t sdu)
{
  if (0 == lcid) {
    rrc->write_pdu_mch(lcid, std::move(sdu));
  } else {
    gw->write_pdu_mch(lcid, std::move(sdu));
  }
}

/*******************************************************************************
  Helpers (Lock must be hold when calling those)
*******************************************************************************/
bool pdcp::valid_lcid(uint32_t lcid)
{
  if (lcid >= SRSLTE_N_RADIO_BEARERS) {
    pdcp_log->error("Radio bearer id must be in [0:%d] - %d", SRSLTE_N_RADIO_BEARERS, lcid);
    return false;
  }

  if (pdcp_array.find(lcid) == pdcp_array.end()) {
    return false;
  }

  return true;
}

bool pdcp::valid_mch_lcid(uint32_t lcid)
{
  if (lcid >= SRSLTE_N_MCH_LCIDS) {
    pdcp_log->error("Radio bearer id must be in [0:%d] - %d", SRSLTE_N_RADIO_BEARERS, lcid);
    return false;
  }

  if (pdcp_array_mrb.find(lcid) == pdcp_array_mrb.end()) {
    return false;
  }

  return true;
}

} // namespace srslte
