/*
 * \section LICENSE
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
#ifndef SRSLTE_EPC_INTERFACES_H
#define SRSLTE_EPC_INTERFACES_H
#include "srslte/asn1/gtpc_ies.h"
#include <netinet/sctp.h>

namespace srsepc {

class nas;

//NAS -> GTP-C
class gtpc_interface_nas
{
public:
  virtual bool send_create_session_request(uint64_t imsi) = 0;
  virtual bool send_modify_bearer_request(uint64_t imsi, uint16_t erab_to_modify, struct srslte::gtpc_f_teid_ie *enb_fteid) = 0;
  virtual bool send_delete_session_request(uint64_t imsi) = 0;
};

//GTP-C -> S1AP
class s1ap_interface_gtpc
{
public:
  virtual bool send_initial_context_setup_request(uint64_t imsi, uint16_t erab_to_setup) = 0;
};

//NAS -> S1AP
class s1ap_interface_nas
{
public:
  virtual uint32_t allocate_m_tmsi(uint64_t imsi) = 0;
  virtual uint32_t get_next_mme_ue_s1ap_id() = 0;
  virtual bool     add_nas_ctx_to_imsi_map(nas *nas_ctx) = 0;
  virtual bool     add_nas_ctx_to_mme_ue_s1ap_id_map(nas *nas_ctx) = 0;
  virtual bool     add_ue_to_enb_set(int32_t enb_assoc, uint32_t mme_ue_s1ap_id) = 0;
  virtual bool     release_ue_ecm_ctx(uint32_t mme_ue_s1ap_id) = 0;
  virtual bool     delete_ue_ctx(uint64_t imsi) = 0;
  virtual uint64_t find_imsi_from_m_tmsi(uint32_t m_tmsi) = 0;
  virtual nas*     find_nas_ctx_from_imsi(uint64_t imsi) = 0;
  virtual bool     send_initial_context_setup_request(uint64_t imsi, uint16_t erab_to_setup) = 0;
  virtual bool     send_ue_context_release_command(uint32_t mme_ue_s1ap_id) = 0;
  virtual bool     send_downlink_nas_transport(uint32_t enb_ue_s1ap_id, uint32_t mme_ue_s1ap_id, srslte::byte_buffer_t *nas_msg, struct sctp_sndrcvinfo enb_sri) = 0;
};

//NAS -> HSS
class hss_interface_nas
{
public:
  virtual bool gen_auth_info_answer(uint64_t imsi, uint8_t *k_asme, uint8_t *autn, uint8_t *rand, uint8_t *xres) = 0;
  virtual bool gen_update_loc_answer(uint64_t imsi, uint8_t *qci) = 0;
  virtual bool resync_sqn(uint64_t imsi, uint8_t *auts) = 0;
};

}
#endif // SRSLTE_EPC_INTERFACES_H
