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

#ifndef SRSLTE_RLC_INTERFACE_H
#define SRSLTE_RLC_INTERFACE_H

// for custom constructors
#include "srslte/asn1/liblte_rrc.h"

namespace srslte {


typedef enum{
  RLC_UMD_SN_SIZE_5_BITS = 0,
  RLC_UMD_SN_SIZE_10_BITS,
  RLC_UMD_SN_SIZE_N_ITEMS,
}rlc_umd_sn_size_t;
static const char     rlc_umd_sn_size_text[RLC_UMD_SN_SIZE_N_ITEMS][20] = {"5 bits", "10 bits"};
static const uint16_t rlc_umd_sn_size_num[RLC_UMD_SN_SIZE_N_ITEMS] = {5, 10};


typedef struct {
  /****************************************************************************
   * Configurable parameters
   * Ref: 3GPP TS 36.322 v10.0.0 Section 7
   ***************************************************************************/

  // TX configs
  int32_t    t_poll_retx;      // Poll retx timeout (ms)
  int32_t    poll_pdu;         // Insert poll bit after this many PDUs
  int32_t    poll_byte;        // Insert poll bit after this much data (KB)
  uint32_t   max_retx_thresh;  // Max number of retx

  // RX configs
  int32_t   t_reordering;       // Timer used by rx to detect PDU loss  (ms)
  int32_t   t_status_prohibit;  // Timer used by rx to prohibit tx of status PDU (ms)
} srslte_rlc_am_config_t;


typedef struct {
  /****************************************************************************
  * Configurable parameters
  * Ref: 3GPP TS 36.322 v10.0.0 Section 7
  ***************************************************************************/

  int32_t           t_reordering;       // Timer used by rx to detect PDU loss  (ms)
  rlc_umd_sn_size_t tx_sn_field_length; // Number of bits used for tx (UL) sequence number
  rlc_umd_sn_size_t rx_sn_field_length; // Number of bits used for rx (DL) sequence number

  uint32_t          rx_window_size;
  uint32_t          rx_mod;             // Rx counter modulus
  uint32_t          tx_mod;             // Tx counter modulus
} srslte_rlc_um_config_t;


class srslte_rlc_config_t
{
public:
  LIBLTE_RRC_RLC_MODE_ENUM  rlc_mode;
  srslte_rlc_am_config_t    am;
  srslte_rlc_um_config_t    um;

  // Constructor based on liblte's RLC config
  srslte_rlc_config_t(LIBLTE_RRC_RLC_CONFIG_STRUCT *cnfg) : rlc_mode(cnfg->rlc_mode), am(), um()
  {
    switch(rlc_mode)
    {
      case LIBLTE_RRC_RLC_MODE_AM:
        am.t_poll_retx       = liblte_rrc_t_poll_retransmit_num[cnfg->ul_am_rlc.t_poll_retx];
        am.poll_pdu          = liblte_rrc_poll_pdu_num[cnfg->ul_am_rlc.poll_pdu];
        am.poll_byte         = liblte_rrc_poll_byte_num[cnfg->ul_am_rlc.poll_byte]*1000; // KB
        am.max_retx_thresh   = liblte_rrc_max_retx_threshold_num[cnfg->ul_am_rlc.max_retx_thresh];
        am.t_reordering      = liblte_rrc_t_reordering_num[cnfg->dl_am_rlc.t_reordering];
        am.t_status_prohibit = liblte_rrc_t_status_prohibit_num[cnfg->dl_am_rlc.t_status_prohibit];
        break;
      case LIBLTE_RRC_RLC_MODE_UM_BI:
        um.t_reordering        = liblte_rrc_t_reordering_num[cnfg->dl_um_bi_rlc.t_reordering];
        um.rx_sn_field_length  = (rlc_umd_sn_size_t)cnfg->dl_um_bi_rlc.sn_field_len;
        um.rx_window_size      = (RLC_UMD_SN_SIZE_5_BITS == um.rx_sn_field_length) ? 16 : 512;
        um.rx_mod              = (RLC_UMD_SN_SIZE_5_BITS == um.rx_sn_field_length) ? 32 : 1024;
        um.tx_sn_field_length  = (rlc_umd_sn_size_t)cnfg->ul_um_bi_rlc.sn_field_len;
        um.tx_mod              = (RLC_UMD_SN_SIZE_5_BITS == um.tx_sn_field_length) ? 32 : 1024;
        break;
      case LIBLTE_RRC_RLC_MODE_UM_UNI_UL:
        um.tx_sn_field_length  = (rlc_umd_sn_size_t)cnfg->ul_um_uni_rlc.sn_field_len;
        um.tx_mod              = (RLC_UMD_SN_SIZE_5_BITS == um.tx_sn_field_length) ? 32 : 1024;
        break;
      case LIBLTE_RRC_RLC_MODE_UM_UNI_DL:
        um.t_reordering        = liblte_rrc_t_reordering_num[cnfg->dl_um_uni_rlc.t_reordering];
        um.rx_sn_field_length  = (rlc_umd_sn_size_t)cnfg->dl_um_uni_rlc.sn_field_len;
        um.rx_window_size      = (RLC_UMD_SN_SIZE_5_BITS == um.rx_sn_field_length) ? 16 : 512;
        um.rx_mod              = (RLC_UMD_SN_SIZE_5_BITS == um.rx_sn_field_length) ? 32 : 1024;
        break;
      default:
        // Handle default case
        break;
    }
  }
};

} // namespace srslte

#endif // SRSLTE_RLC_INTERFACE_H
