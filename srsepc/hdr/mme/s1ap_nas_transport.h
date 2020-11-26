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
#ifndef SRSEPC_S1AP_NAS_TRANSPORT_H
#define SRSEPC_S1AP_NAS_TRANSPORT_H

#include "mme_gtpc.h"
#include "s1ap_common.h"
#include "srsepc/hdr/hss/hss.h"
#include "srslte/asn1/gtpc.h"
#include "srslte/asn1/s1ap.h"
#include "srslte/common/buffer_pool.h"

namespace srsepc {

class s1ap_nas_transport
{
public:
  static s1ap_nas_transport* m_instance;
  static s1ap_nas_transport* get_instance();
  static void                cleanup();
  void                       init();

  bool handle_initial_ue_message(const asn1::s1ap::init_ue_msg_s& init_ue, struct sctp_sndrcvinfo* enb_sri);
  bool handle_uplink_nas_transport(const asn1::s1ap::ul_nas_transport_s& ul_xport, struct sctp_sndrcvinfo* enb_sri);
  bool send_downlink_nas_transport(uint32_t               enb_ue_s1ap_id,
                                   uint32_t               mme_ue_s1ap_id,
                                   srslte::byte_buffer_t* nas_msg,
                                   struct sctp_sndrcvinfo enb_sri);

private:
  s1ap_nas_transport();
  virtual ~s1ap_nas_transport();

  srslte::log*              m_s1ap_log;
  srslte::byte_buffer_pool* m_pool;

  s1ap* m_s1ap;

  nas_init_t m_nas_init;
  nas_if_t   m_nas_if;
};

} // namespace srsepc
#endif // SRSEPC_S1AP_NAS_TRANSPORT_H
