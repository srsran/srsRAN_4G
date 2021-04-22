/**
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
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

} // namespace srsue

#endif // SRSRAN_UE_NAS_INTERFACES_H
