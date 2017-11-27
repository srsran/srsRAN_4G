/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2017 Software Radio Systems Limited
 *
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
#ifndef    MME_GTPC_H
#define    MME_GTPC_H

#include "srslte/common/buffer_pool.h"
#include <boost/thread/mutex.hpp>
#include "srslte/asn1/gtpc.h"

namespace srsepc
{

class spgw;
class s1ap;

class mme_gtpc
{
public:

  static mme_gtpc* get_instance(void);
  static void cleanup(void);

  void init();

  uint64_t get_new_ctrl_teid();
  void send_create_session_request(uint64_t imsi, uint32_t mme_s1ap_id);
  void handle_create_session_response(srslte::gtpc_pdu *cs_resp_pdu);

private:

  mme_gtpc();
  virtual ~mme_gtpc();
  static mme_gtpc *m_instance;

  srslte::byte_buffer_pool *m_pool;

  s1ap* m_s1ap;
  spgw* m_spgw;
  in_addr_t m_mme_gtpc_ip;

  uint64_t m_next_ctrl_teid;
  std::map<uint64_t,uint32_t> m_teid_to_mme_s1ap_id;

};

}
#endif
