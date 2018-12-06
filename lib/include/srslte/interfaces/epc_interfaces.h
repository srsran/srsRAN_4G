#ifndef SRSLTE_EPC_INTERFACES_H
#define SRSLTE_EPC_INTERFACES_H

#include "srslte/srslte.h"

#include "srslte/common/common.h"

#include "srsepc/hdr/mme/s1ap_common.h"

namespace srsepc {

class hss_interface_s1ap
{
public:
  virtual bool gen_auth_info_answer(uint64_t imsi, uint8_t *k_asme, uint8_t *autn, uint8_t *rand, uint8_t *xres) = 0;
  virtual bool gen_update_loc_answer(uint64_t imsi, uint8_t *qci) = 0;
  virtual bool resync_sqn(uint64_t imsi, uint8_t *auts) = 0;
};

class s1ap_interface_nas
{
public:
  virtual ue_ctx_t* find_ue_ctx_from_mme_ue_s1ap_id(uint32_t mme_ue_s1ap_id) = 0;
  virtual ue_ctx_t* find_ue_ctx_from_imsi(uint64_t imsi) = 0;

  virtual bool add_ue_ctx_to_imsi_map(ue_ctx_t *ue_ctx) = 0;
  virtual bool add_ue_ctx_to_mme_ue_s1ap_id_map(ue_ctx_t *ue_ctx) = 0;
  virtual bool add_ue_to_enb_set(int32_t enb_assoc, uint32_t mme_ue_s1ap_id) = 0;

  virtual uint32_t get_plmn() = 0;
  virtual uint32_t get_next_mme_ue_s1ap_id() = 0;
  virtual uint32_t allocate_m_tmsi(uint64_t imsi) = 0;
  virtual bool find_imsi_by_tmsi(uint32_t tmsi, uint64_t * imsi) = 0;

  virtual bool delete_ue_ctx(uint64_t imsi) = 0;
  virtual bool release_ue_ecm_ctx(uint32_t mme_ue_s1ap_id) = 0;

  virtual bool send_initial_context_setup_request(ue_emm_ctx_t *emm_ctx, ue_ecm_ctx_t *ecm_ctx, erab_ctx_t *erab_ctx) = 0;
  virtual bool send_ue_context_release_command(ue_ecm_ctx_t *ecm_ctx, srslte::byte_buffer_t *reply_buffer) = 0;
 
};
}
#endif // SRSLTE_EPC_INTERFACES_H
