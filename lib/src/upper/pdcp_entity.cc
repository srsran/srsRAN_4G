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


#include "srslte/upper/pdcp_entity.h"
#include "srslte/common/security.h"

namespace srslte {

pdcp_entity::pdcp_entity()
  :active(false)
  ,tx_count(0)
  ,rx_count(0)
  ,do_security(false)
  ,sn_len(12)
{
  pool = byte_buffer_pool::get_instance();
}

void pdcp_entity::init(srsue::rlc_interface_pdcp      *rlc_,
                       srsue::rrc_interface_pdcp      *rrc_,
                       srsue::gw_interface_pdcp       *gw_,
                       srslte::log                    *log_,
                       uint32_t                       lcid_,
                       u_int8_t                       direction_,
                       LIBLTE_RRC_PDCP_CONFIG_STRUCT *cnfg)
{
  rlc       = rlc_;
  rrc       = rrc_;
  gw        = gw_;
  log       = log_;
  lcid      = lcid_;
  direction = direction_;
  active    = true;

  tx_count    = 0;
  rx_count    = 0;
  do_security = false;

  if(cnfg)
  {
    if(cnfg->rlc_um_pdcp_sn_size_present) {
      if(LIBLTE_RRC_PDCP_SN_SIZE_7_BITS == cnfg->rlc_um_pdcp_sn_size) {
        sn_len = 7;
      }
    }
    // TODO: handle remainder of cnfg
  }
  log->debug("Init %s\n", rb_id_text[lcid]);
}

void pdcp_entity::reset()
{
  active      = false;
  if(log)
    log->debug("Reset %s\n", rb_id_text[lcid]);
}

bool pdcp_entity::is_active()
{
  return active;
}

// RRC interface
void pdcp_entity::write_sdu(byte_buffer_t *sdu)
{
  log->info_hex(sdu->msg, sdu->N_bytes, "TX %s SDU, do_security = %s", rb_id_text[lcid], (do_security)?"true":"false");

  // Handle SRB messages
  switch(lcid)
  {
  case RB_ID_SRB0:
    rlc->write_sdu(lcid, sdu);
    break;
  case RB_ID_SRB1:  // Intentional fall-through
  case RB_ID_SRB2:
    pdcp_pack_control_pdu(tx_count, sdu);
    if(do_security)
    {
      integrity_generate(&k_rrc_int[16],
                         tx_count,
                         lcid-1,
                         direction,
                         sdu->msg,
                         sdu->N_bytes-4,
                         &sdu->msg[sdu->N_bytes-4]);
    }
    tx_count++;
    rlc->write_sdu(lcid, sdu);

    break;
  }

  // Handle DRB messages
  if(lcid >= RB_ID_DRB1)
  {
    if(12 == sn_len)
    {
      pdcp_pack_data_pdu_long_sn(tx_count++, sdu);
    } else {
      pdcp_pack_data_pdu_short_sn(tx_count++, sdu);
    }
    rlc->write_sdu(lcid, sdu);
  }
}

void pdcp_entity::config_security(uint8_t *k_rrc_enc_,
                                  uint8_t *k_rrc_int_,
                                  CIPHERING_ALGORITHM_ID_ENUM cipher_algo_,
                                  INTEGRITY_ALGORITHM_ID_ENUM integ_algo_)
{
  do_security = true;
  for(int i=0; i<32; i++)
  {
    k_rrc_enc[i] = k_rrc_enc_[i];
    k_rrc_int[i] = k_rrc_int_[i];
  }
  cipher_algo = cipher_algo_;
  integ_algo  = integ_algo_;
}

// RLC interface
void pdcp_entity::write_pdu(byte_buffer_t *pdu)
{
  // Handle SRB messages
  switch(lcid)
  {
  case RB_ID_SRB0:
    // Simply pass on to RRC
    log->info_hex(pdu->msg, pdu->N_bytes, "RX %s PDU", rb_id_text[lcid]);
    rrc->write_pdu(RB_ID_SRB0, pdu);
    break;
  case RB_ID_SRB1: // Intentional fall-through
  case RB_ID_SRB2:
    uint32_t sn;
    log->info_hex(pdu->msg, pdu->N_bytes, "RX %s PDU", rb_id_text[lcid]);
    pdcp_unpack_control_pdu(pdu, &sn);
    log->info_hex(pdu->msg, pdu->N_bytes, "RX %s SDU SN: %d",
                  rb_id_text[lcid], sn);
    rrc->write_pdu(lcid, pdu);
    break;
  }

  // Handle DRB messages
  if(lcid >= RB_ID_DRB1)
  {
    uint32_t sn;
    if(12 == sn_len)
    {
      pdcp_unpack_data_pdu_long_sn(pdu, &sn);
    } else {
      pdcp_unpack_data_pdu_short_sn(pdu, &sn);
    }
    log->info_hex(pdu->msg, pdu->N_bytes, "RX %s PDU: %d", rb_id_text[lcid], sn);
    gw->write_pdu(lcid, pdu);
  }
}

void pdcp_entity::integrity_generate( uint8_t  *key_128,
                                      uint32_t  count,
                                      uint8_t   rb_id,
                                      uint8_t   direction,
                                      uint8_t  *msg,
                                      uint32_t  msg_len,
                                      uint8_t  *mac)
{
  switch(integ_algo)
  {
  case INTEGRITY_ALGORITHM_ID_EIA0:
    break;
  case INTEGRITY_ALGORITHM_ID_128_EIA1:
    security_128_eia1(key_128,
                      count,
                      rb_id,
                      direction,
                      msg,
                      msg_len,
                      mac);
    break;
  case INTEGRITY_ALGORITHM_ID_128_EIA2:
    security_128_eia2(key_128,
                      count,
                      rb_id,
                      direction,
                      msg,
                      msg_len,
                      mac);
    break;
  default:
    break;
  }
}

/****************************************************************************
 * Pack/Unpack helper functions
 * Ref: 3GPP TS 36.323 v10.1.0
 ***************************************************************************/

void pdcp_pack_control_pdu(uint32_t sn, byte_buffer_t *sdu)
{
  // Make room and add header
  sdu->msg--;
  sdu->N_bytes++;
  *sdu->msg = sn & 0x1F;

  // Add MAC
  sdu->msg[sdu->N_bytes++] = (PDCP_CONTROL_MAC_I >> 24) & 0xFF;
  sdu->msg[sdu->N_bytes++] = (PDCP_CONTROL_MAC_I >> 16) & 0xFF;
  sdu->msg[sdu->N_bytes++] = (PDCP_CONTROL_MAC_I >>  8) & 0xFF;
  sdu->msg[sdu->N_bytes++] =  PDCP_CONTROL_MAC_I        & 0xFF;

}

void pdcp_unpack_control_pdu(byte_buffer_t *pdu, uint32_t *sn)
{
  // Strip header
  *sn = *pdu->msg & 0x1F;
  pdu->msg++;
  pdu->N_bytes--;

  // Strip MAC
  pdu->N_bytes -= 4;

  // TODO: integrity check MAC
}

void pdcp_pack_data_pdu_short_sn(uint32_t sn, byte_buffer_t *sdu)
{
  // Make room and add header
  sdu->msg--;
  sdu->N_bytes++;
  sdu->msg[0] = (PDCP_D_C_DATA_PDU << 7) | (sn & 0x7F);
}

void pdcp_unpack_data_pdu_short_sn(byte_buffer_t *sdu, uint32_t *sn)
{
  // Strip header
  *sn  = sdu->msg[0] & 0x7F;
  sdu->msg++;
  sdu->N_bytes--;
}

void pdcp_pack_data_pdu_long_sn(uint32_t sn, byte_buffer_t *sdu)
{
  // Make room and add header
  sdu->msg     -= 2;
  sdu->N_bytes += 2;
  sdu->msg[0] = (PDCP_D_C_DATA_PDU << 7) | ((sn >> 8) & 0x0F);
  sdu->msg[1] = sn & 0xFF;
}

void pdcp_unpack_data_pdu_long_sn(byte_buffer_t *sdu, uint32_t *sn)
{
  // Strip header
  *sn  = (sdu->msg[0] & 0x0F) << 8;
  *sn |= sdu->msg[1];
  sdu->msg     += 2;
  sdu->N_bytes -= 2;
}

}
