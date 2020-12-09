/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */
#ifndef SRSEPC_S1AP_ERAB_MNGMT_PROC_H
#define SRSEPC_S1AP_ERAB_MNGMT_PROC_H

#include "mme_gtpc.h"
#include "s1ap_common.h"
#include "srslte/asn1/s1ap.h"
#include "srslte/common/buffer_pool.h"
#include "srslte/common/common.h"
#include "srslte/common/log_filter.h"
#include <netinet/sctp.h>

namespace srsepc {

class s1ap;

class s1ap_erab_mngmt_proc
{
public:
  static s1ap_erab_mngmt_proc* m_instance;
  static s1ap_erab_mngmt_proc* get_instance(void);
  static void                  cleanup(void);

  void init(void);

  bool send_erab_release_command(uint32_t               enb_ue_s1ap_id,
                                 uint32_t               mme_ue_s1ap_id,
                                 std::vector<uint16_t>  erabs_to_release,
                                 struct sctp_sndrcvinfo enb_sri);
  bool handle_erab_release_response(const asn1::s1ap::init_context_setup_resp_s& in_ctxt_resp);

private:
  s1ap_erab_mngmt_proc();
  virtual ~s1ap_erab_mngmt_proc();

  s1ap*               m_s1ap     = nullptr;
  srslte::log_filter* m_s1ap_log = nullptr;

  s1ap_args_t m_s1ap_args;

  mme_gtpc*                 m_mme_gtpc = nullptr;
  srslte::byte_buffer_pool* m_pool     = nullptr;
};

} // namespace srsepc
#endif // SRSEPC_S1AP_CTX_MNGMT_PROC_H
