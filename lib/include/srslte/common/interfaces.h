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

/******************************************************************************
 * File:        interfaces.h
 * Description: Abstract base class interfaces provided by layers
 *              to other layers.
 *****************************************************************************/

#ifndef INTERFACES_H
#define INTERFACES_H

#include "liblte_rrc.h"
#include "common/interfaces_common.h"
#include "common/common.h"
#include "common/security.h"
#include "mac_interface.h"
#include "phy_interface.h"

namespace srsue {

// UE interface
class ue_interface
{
};

// USIM interface for NAS
class usim_interface_nas
{
public:
  virtual void get_imsi_vec(uint8_t* imsi_, uint32_t n) = 0;
  virtual void get_imei_vec(uint8_t* imei_, uint32_t n) = 0;
  virtual void generate_authentication_response(uint8_t  *rand,
                                                uint8_t  *autn_enb,
                                                uint16_t  mcc,
                                                uint16_t  mnc,
                                                bool     *net_valid,
                                                uint8_t  *res) = 0;
  virtual void generate_nas_keys(uint8_t *k_nas_enc,
                                 uint8_t *k_nas_int,
                                 CIPHERING_ALGORITHM_ID_ENUM cipher_algo,
                                 INTEGRITY_ALGORITHM_ID_ENUM integ_algo) = 0;
};

// USIM interface for RRC
class usim_interface_rrc
{
public:
  virtual void generate_as_keys(uint32_t count_ul,
                                uint8_t *k_rrc_enc,
                                uint8_t *k_rrc_int,
                                uint8_t *k_up_enc,
                                uint8_t *k_up_int,
                                CIPHERING_ALGORITHM_ID_ENUM cipher_algo,
                                INTEGRITY_ALGORITHM_ID_ENUM integ_algo) = 0;
};

// GW interface for NAS
class gw_interface_nas
{
public:
  virtual srslte::error_t setup_if_addr(uint32_t ip_addr, char *err_str) = 0;
};

// GW interface for PDCP
class gw_interface_pdcp
{
public:
  virtual void write_pdu(uint32_t lcid, srslte::byte_buffer_t *pdu) = 0;
};

// NAS interface for RRC
class nas_interface_rrc
{
public:
  virtual bool      is_attached() = 0;
  virtual void      notify_connection_setup() = 0;
  virtual void      write_pdu(uint32_t lcid, srslte::byte_buffer_t *pdu) = 0;
  virtual uint32_t  get_ul_count() = 0;
  virtual bool      get_s_tmsi(LIBLTE_RRC_S_TMSI_STRUCT *s_tmsi) = 0;
};

// RRC interface for MAC
class rrc_interface_mac
{
public:
  virtual void release_pucch_srs() = 0;
  virtual void ra_problem() = 0; 
};

// RRC interface for PHY
class rrc_interface_phy
{
public:
  virtual void in_sync() = 0;
  virtual void out_of_sync() = 0;
};

// RRC interface for NAS
class rrc_interface_nas
{
public:
  virtual void write_sdu(uint32_t lcid, srslte::byte_buffer_t *sdu) = 0;
  virtual uint16_t get_mcc() = 0;
  virtual uint16_t get_mnc() = 0;
  virtual void enable_capabilities() = 0;
};

// RRC interface for GW
class rrc_interface_gw
{
public:
  virtual bool rrc_connected() = 0;
  virtual void rrc_connect() = 0; 
  virtual bool have_drb() = 0;
};

// RRC interface for PDCP
class rrc_interface_pdcp
{
public:
  virtual void write_pdu(uint32_t lcid, srslte::byte_buffer_t *pdu) = 0;
  virtual void write_pdu_bcch_bch(srslte::byte_buffer_t *pdu) = 0;
  virtual void write_pdu_bcch_dlsch(srslte::byte_buffer_t *pdu) = 0;
  virtual void write_pdu_pcch(srslte::byte_buffer_t *pdu) = 0;
};

// RRC interface for RLC
class rrc_interface_rlc
{
public:
  virtual void max_retx_attempted() = 0;
};

// PDCP interface for GW
class pdcp_interface_gw
{
public:
  virtual void write_sdu(uint32_t lcid, srslte::byte_buffer_t *sdu) = 0;
};

// PDCP interface for RRC
class pdcp_interface_rrc
{
public:
  virtual void reset() = 0;
  virtual void write_sdu(uint32_t lcid, srslte::byte_buffer_t *sdu) = 0;
  virtual void add_bearer(uint32_t lcid, LIBLTE_RRC_PDCP_CONFIG_STRUCT *cnfg=NULL) = 0;
  virtual void config_security(uint32_t lcid,
                               uint8_t *k_rrc_enc_,
                               uint8_t *k_rrc_int_,
                               CIPHERING_ALGORITHM_ID_ENUM cipher_algo_,
                               INTEGRITY_ALGORITHM_ID_ENUM integ_algo_) = 0;
};

// PDCP interface for RLC
class pdcp_interface_rlc
{
public:
  /* RLC calls PDCP to push a PDCP PDU. */
  virtual void write_pdu(uint32_t lcid, srslte::byte_buffer_t *sdu) = 0;
  virtual void write_pdu_bcch_bch(srslte::byte_buffer_t *sdu) = 0;
  virtual void write_pdu_bcch_dlsch(srslte::byte_buffer_t *sdu) = 0;
  virtual void write_pdu_pcch(srslte::byte_buffer_t *sdu) = 0;
};

// RLC interface for RRC
class rlc_interface_rrc
{
public:
  virtual void reset() = 0;
  virtual void add_bearer(uint32_t lcid) = 0;
  virtual void add_bearer(uint32_t lcid, LIBLTE_RRC_RLC_CONFIG_STRUCT *cnfg) = 0;
};

// RLC interface for PDCP
class rlc_interface_pdcp
{
public:
  /* PDCP calls RLC to push an RLC SDU. SDU gets placed into the RLC buffer and MAC pulls
   * RLC PDUs according to TB size. */
  virtual void write_sdu(uint32_t lcid,  srslte::byte_buffer_t *sdu) = 0;
};

//RLC interface for MAC
class rlc_interface_mac : public srslte::read_pdu_interface
{
public:
  /* MAC calls RLC to get buffer state for a logical channel.
   * This function should return quickly. */
  virtual uint32_t get_buffer_state(uint32_t lcid) = 0;
  virtual uint32_t get_total_buffer_state(uint32_t lcid) = 0; 


  const static int MAX_PDU_SEGMENTS = 20;

  /* MAC calls RLC to get RLC segment of nof_bytes length.
   * Segmentation happens in this function. RLC PDU is stored in payload. */
  virtual int     read_pdu(uint32_t lcid, uint8_t *payload, uint32_t nof_bytes) = 0;

  /* MAC calls RLC to push an RLC PDU. This function is called from an independent MAC thread.
   * PDU gets placed into the buffer and higher layer thread gets notified. */
  virtual void write_pdu(uint32_t lcid, uint8_t *payload, uint32_t nof_bytes) = 0;
  virtual void write_pdu_bcch_bch(uint8_t *payload, uint32_t nof_bytes) = 0;
  virtual void write_pdu_bcch_dlsch(uint8_t *payload, uint32_t nof_bytes) = 0;
  virtual void write_pdu_pcch(uint8_t *payload, uint32_t nof_bytes) = 0;
};

} // namespace srsue

#endif // INTERFACES_H
