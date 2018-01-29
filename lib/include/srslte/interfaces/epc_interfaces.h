#ifndef EPC_INTERFACE_H
#define EPC_INTERFACE_H

#include "srslte/srslte.h"

#include "srslte/common/common.h"

namespace srsepc {

class hss_interface_s1ap
{
public:
  virtual bool gen_auth_info_answer(uint64_t imsi, uint8_t *k_asme, uint8_t *autn, uint8_t *rand, uint8_t *xres) = 0;
  virtual bool resync_sqn(uint64_t imsi, uint8_t *auts) = 0;
};

}
#endif // ENB_METRICS_INTERFACE_H
