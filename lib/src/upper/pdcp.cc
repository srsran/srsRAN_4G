/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
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

pdcp::pdcp()
{
  rlc = NULL;
  rrc = NULL;
  gw = NULL;
  pdcp_log = NULL;
  lcid = 0;
  direction = 0;
}

void pdcp::init(srsue::rlc_interface_pdcp *rlc_, srsue::rrc_interface_pdcp *rrc_, srsue::gw_interface_pdcp *gw_, log *pdcp_log_, uint32_t lcid_, uint8_t direction_)
{
  rlc       = rlc_;
  rrc       = rrc_;
  gw        = gw_;
  pdcp_log  = pdcp_log_;
  lcid      = lcid_;
  direction = direction_;

  // Default config
  srslte_pdcp_config_t cnfg;
  cnfg.is_control = false;
  cnfg.is_data = false;
  cnfg.direction = direction_;

  pdcp_array[0].init(rlc, rrc, gw, pdcp_log, lcid, cnfg);
}

void pdcp::stop()
{
}

void pdcp::reestablish() {
  for(uint32_t i=0;i<SRSLTE_N_RADIO_BEARERS;i++) {
    if (pdcp_array[i].is_active()) {
      pdcp_array[i].reestablish();
    }
  }
}

void pdcp::reset()
{
  for(uint32_t i=0;i<SRSLTE_N_RADIO_BEARERS;i++) {
    pdcp_array[i].reset();
  }

  pdcp_array[0].init(rlc, rrc, gw, pdcp_log, lcid, direction);
}

/*******************************************************************************
  RRC/GW interface
*******************************************************************************/
bool pdcp::is_drb_enabled(uint32_t lcid)
{
  if(lcid >= SRSLTE_N_RADIO_BEARERS) {
    pdcp_log->error("Radio bearer id must be in [0:%d] - %d\n", SRSLTE_N_RADIO_BEARERS, lcid);
    return false;
  }
  return pdcp_array[lcid].is_active();
}

void pdcp::write_sdu(uint32_t lcid, byte_buffer_t *sdu)
{
  if(valid_lcid(lcid)) {
    pdcp_array[lcid].write_sdu(sdu);
  } else {
    pdcp_log->warning("Writing sdu: lcid=%d. Deallocating sdu\n", lcid);
    byte_buffer_pool::get_instance()->deallocate(sdu);
  }
}

void pdcp::add_bearer(uint32_t lcid, srslte_pdcp_config_t cfg)
{
  if(lcid >= SRSLTE_N_RADIO_BEARERS) {
    pdcp_log->error("Radio bearer id must be in [0:%d] - %d\n", SRSLTE_N_RADIO_BEARERS, lcid);
    return;
  }
  if (!pdcp_array[lcid].is_active()) {
    pdcp_array[lcid].init(rlc, rrc, gw, pdcp_log, lcid, cfg);
    pdcp_log->info("Added bearer %s\n", rrc->get_rb_name(lcid).c_str());
  } else {
    pdcp_log->warning("Bearer %s already configured. Reconfiguration not supported\n", rrc->get_rb_name(lcid).c_str());
  }
}

void pdcp::config_security(uint32_t lcid,
                           uint8_t *k_enc,
                           uint8_t *k_int,
                           CIPHERING_ALGORITHM_ID_ENUM cipher_algo,
                           INTEGRITY_ALGORITHM_ID_ENUM integ_algo)
{
  if(valid_lcid(lcid))
    pdcp_array[lcid].config_security(k_enc, k_int, cipher_algo, integ_algo);
}

void pdcp::config_security_all(uint8_t *k_enc,
                               uint8_t *k_int,
                               CIPHERING_ALGORITHM_ID_ENUM cipher_algo,
                               INTEGRITY_ALGORITHM_ID_ENUM integ_algo)
{
  for(uint32_t i=0;i<SRSLTE_N_RADIO_BEARERS;i++) {
    if (pdcp_array[i].is_active()) {
      pdcp_array[i].config_security(k_enc, k_int, cipher_algo, integ_algo);
    }
  }
}

void pdcp::enable_integrity(uint32_t lcid)
{
  if(valid_lcid(lcid))
    pdcp_array[lcid].enable_integrity();
}

void pdcp::enable_encryption(uint32_t lcid)
{
  if(valid_lcid(lcid))
    pdcp_array[lcid].enable_encryption();
}

/*******************************************************************************
  RLC interface
*******************************************************************************/
void pdcp::write_pdu(uint32_t lcid, byte_buffer_t *pdu)
{
  if(valid_lcid(lcid)) {
    pdcp_array[lcid].write_pdu(pdu);
  } else {
    pdcp_log->warning("Writing pdu: lcid=%d. Deallocating pdu\n", lcid);
    byte_buffer_pool::get_instance()->deallocate(pdu);
  }
}

void pdcp::write_pdu_bcch_bch(byte_buffer_t *sdu)
{
  rrc->write_pdu_bcch_bch(sdu);
}
void pdcp::write_pdu_bcch_dlsch(byte_buffer_t *sdu)
{
  rrc->write_pdu_bcch_dlsch(sdu);
}

void pdcp::write_pdu_pcch(byte_buffer_t *sdu)
{
  rrc->write_pdu_pcch(sdu);
}

/*******************************************************************************
  Helpers
*******************************************************************************/
bool pdcp::valid_lcid(uint32_t lcid)
{
  if(lcid >= SRSLTE_N_RADIO_BEARERS) {
    pdcp_log->error("Radio bearer id must be in [0:%d] - %d", SRSLTE_N_RADIO_BEARERS, lcid);
    return false;
  }
  if(!pdcp_array[lcid].is_active()) {
    pdcp_log->error("PDCP entity for logical channel %d has not been activated\n", lcid);
    return false;
  }
  return true;
}

} // namespace srsue
