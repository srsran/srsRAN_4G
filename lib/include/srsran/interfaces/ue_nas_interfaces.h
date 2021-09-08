/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSRAN_UE_NAS_INTERFACES_H
#define SRSRAN_UE_NAS_INTERFACES_H

#include "srsran/interfaces/rrc_interface_types.h"

namespace srsue {

class nas_interface_rrc
{
public:
  const static int MAX_FOUND_PLMNS = 16;
  struct found_plmn_t {
    srsran::plmn_id_t plmn_id;
    uint16_t          tac;
  };

  virtual void     left_rrc_connected()                                                                  = 0;
  virtual void     set_barring(srsran::barring_t barring)                                                = 0;
  virtual bool     paging(srsran::s_tmsi_t* ue_identity)                                                 = 0;
  virtual bool     is_registered()                                                                       = 0;
  virtual void     write_pdu(uint32_t lcid, srsran::unique_byte_buffer_t pdu)                            = 0;
  virtual uint32_t get_k_enb_count()                                                                     = 0;
  virtual bool     get_k_asme(uint8_t* k_asme_, uint32_t n)                                              = 0;
  virtual uint32_t get_ipv4_addr()                                                                       = 0;
  virtual bool     get_ipv6_addr(uint8_t* ipv6_addr)                                                     = 0;
  virtual void     plmn_search_completed(const found_plmn_t found_plmns[MAX_FOUND_PLMNS], int nof_plmns) = 0;
  virtual bool     connection_request_completed(bool outcome)                                            = 0;
};

class nas_5g_interface_rrc_nr
{
public:
  virtual int write_pdu(srsran::unique_byte_buffer_t pdu) = 0;
};

class nas_5g_interface_procedures
{
public:
  virtual int send_registration_request() = 0;
};

} // namespace srsue

#endif // SRSRAN_UE_NAS_INTERFACES_H
