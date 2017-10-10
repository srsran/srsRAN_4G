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

#ifndef NAS_H
#define NAS_H

#include "srslte/common/buffer_pool.h"
#include "srslte/common/log.h"
#include "srslte/common/common.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/common/security.h"
#include "srslte/asn1/liblte_mme.h"

using srslte::byte_buffer_t;

namespace srsue {

// EMM states (3GPP 24.302 v10.0.0)
typedef enum {
  EMM_STATE_NULL = 0,
  EMM_STATE_DEREGISTERED,
  EMM_STATE_REGISTERED_INITIATED,
  EMM_STATE_REGISTERED,
  EMM_STATE_SERVICE_REQUEST_INITIATED,
  EMM_STATE_DEREGISTERED_INITIATED,
  EMM_STATE_TAU_INITIATED,
  EMM_STATE_N_ITEMS,
} emm_state_t;
static const char emm_state_text[EMM_STATE_N_ITEMS][100] = {"NULL",
                                                            "DEREGISTERED",
                                                            "REGISTERED INITIATED",
                                                            "REGISTERED",
                                                            "SERVICE REQUEST INITIATED",
                                                            "DEREGISTERED INITIATED",
                                                            "TRACKING AREA UPDATE INITIATED"};

typedef enum {
  PLMN_NOT_SELECTED = 0,
  PLMN_SELECTED
} plmn_selection_state_t;

class nas
  : public nas_interface_rrc,
    public nas_interface_ue,
    public nas_interface_gw
{
public:
  nas();
  void init(usim_interface_nas  *usim_,
            rrc_interface_nas   *rrc_,
            gw_interface_nas    *gw_,
            srslte::log         *nas_log_,
            srslte::srslte_nas_config_t cfg_);
  void stop();

  emm_state_t get_state();

  // RRC interface
  void notify_connection_setup();

  void write_pdu(uint32_t lcid, byte_buffer_t *pdu);

  uint32_t get_ul_count();

  bool is_attached();
  bool is_attaching();

  bool get_s_tmsi(LIBLTE_RRC_S_TMSI_STRUCT *s_tmsi);

  void plmn_found(LIBLTE_RRC_PLMN_IDENTITY_STRUCT plmn_id, uint16_t tracking_area_code);
  void plmn_search_end();

  // UE interface
  void attach_request();
  void deattach_request();

private:
  srslte::byte_buffer_pool *pool;
  srslte::log *nas_log;
  rrc_interface_nas *rrc;
  usim_interface_nas *usim;
  gw_interface_nas *gw;

  srslte::srslte_nas_config_t cfg;

  emm_state_t state;

  plmn_selection_state_t plmn_selection;
  LIBLTE_RRC_PLMN_IDENTITY_STRUCT current_plmn;
  LIBLTE_RRC_PLMN_IDENTITY_STRUCT selecting_plmn;
  LIBLTE_RRC_PLMN_IDENTITY_STRUCT home_plmn;

  std::vector<LIBLTE_RRC_PLMN_IDENTITY_STRUCT > known_plmns;

  // Save short MAC

  // Identifiers
  LIBLTE_MME_EPS_MOBILE_ID_GUTI_STRUCT guti;
  bool is_guti_set;

  uint32_t ip_addr;
  uint8_t eps_bearer_id;

  uint8_t transaction_id;

  // NAS counters - incremented for each security-protected message recvd/sent
  uint32_t count_ul;
  uint32_t count_dl;

  // Security
  uint8_t ksi;
  uint8_t k_nas_enc[32];
  uint8_t k_nas_int[32];

  srslte::CIPHERING_ALGORITHM_ID_ENUM cipher_algo;
  srslte::INTEGRITY_ALGORITHM_ID_ENUM integ_algo;

  void integrity_generate(uint8_t *key_128,
                          uint32_t count,
                          uint8_t rb_id,
                          uint8_t direction,
                          uint8_t *msg,
                          uint32_t msg_len,
                          uint8_t *mac);

  void integrity_check();

  void cipher_encrypt();

  void cipher_decrypt();

  // Parsers
  void parse_attach_accept(uint32_t lcid, byte_buffer_t *pdu);

  void parse_attach_reject(uint32_t lcid, byte_buffer_t *pdu);

  void parse_authentication_request(uint32_t lcid, byte_buffer_t *pdu);

  void parse_authentication_reject(uint32_t lcid, byte_buffer_t *pdu);

  void parse_identity_request(uint32_t lcid, byte_buffer_t *pdu);

  void parse_security_mode_command(uint32_t lcid, byte_buffer_t *pdu);

  void parse_service_reject(uint32_t lcid, byte_buffer_t *pdu);

  void parse_esm_information_request(uint32_t lcid, byte_buffer_t *pdu);

  void parse_emm_information(uint32_t lcid, byte_buffer_t *pdu);

  // Senders
  void send_attach_request();

  void send_identity_response();

  void send_service_request();

  void send_esm_information_response();

  void gen_pdn_connectivity_request(LIBLTE_BYTE_MSG_STRUCT *msg);
};

} // namespace srsue


#endif // NAS_H
