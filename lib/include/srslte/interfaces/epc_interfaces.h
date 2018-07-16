#ifndef SRSLTE_EPC_INTERFACES_H
#define SRSLTE_EPC_INTERFACES_H

#include "srslte/srslte.h"
#include "srslte/common/common.h"

namespace srsepc {

/*
 * S1AP Interfaces
 */
//GTP-C -> S1AP
class s1ap_interface_gtpc
{
public:
  virtual bool send_initial_context_setup_request(uint64_t imsi) = 0;
};

//NAS -> S1AP
class s1ap_interface_nas
{
public:
  virtual bool send_initial_context_setup_request(uint64_t imsi) = 0;
};

//S1AP -> HSS
class hss_interface_s1ap
{
public:
  virtual bool gen_auth_info_answer(uint64_t imsi, uint8_t *k_asme, uint8_t *autn, uint8_t *rand, uint8_t *xres) = 0;
  virtual bool gen_update_loc_answer(uint64_t imsi, uint8_t *qci) = 0;
  virtual bool resync_sqn(uint64_t imsi, uint8_t *auts) = 0;
};

}
#endif // SRSLTE_EPC_INTERFACES_H
