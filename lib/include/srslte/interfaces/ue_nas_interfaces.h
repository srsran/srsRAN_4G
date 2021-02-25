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

#ifndef SRSLTE_UE_NAS_INTERFACES_H
#define SRSLTE_UE_NAS_INTERFACES_H

#include "srslte/interfaces/rrc_interface_types.h"

namespace srsue {

class nas_interface_rrc
{
public:
  const static int MAX_FOUND_PLMNS = 16;
  struct found_plmn_t {
    srslte::plmn_id_t plmn_id;
    uint16_t          tac;
  };

  virtual void     left_rrc_connected()                                                                  = 0;
  virtual void     set_barring(srslte::barring_t barring)                                                = 0;
  virtual bool     paging(srslte::s_tmsi_t* ue_identity)                                                 = 0;
  virtual bool     is_registered()                                                                       = 0;
  virtual void     write_pdu(uint32_t lcid, srslte::unique_byte_buffer_t pdu)                            = 0;
  virtual uint32_t get_k_enb_count()                                                                     = 0;
  virtual bool     get_k_asme(uint8_t* k_asme_, uint32_t n)                                              = 0;
  virtual uint32_t get_ipv4_addr()                                                                       = 0;
  virtual bool     get_ipv6_addr(uint8_t* ipv6_addr)                                                     = 0;
  virtual void     plmn_search_completed(const found_plmn_t found_plmns[MAX_FOUND_PLMNS], int nof_plmns) = 0;
  virtual bool     connection_request_completed(bool outcome)                                            = 0;
};

} // namespace srsue

#endif // SRSLTE_UE_NAS_INTERFACES_H
