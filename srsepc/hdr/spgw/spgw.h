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

/******************************************************************************
 * File:        spgw.h
 * Description: Top-level SP-GW class. Creates and links all
 *              interfaces and helpers.
 *****************************************************************************/

#ifndef SRSEPC_SPGW_H
#define SRSEPC_SPGW_H

#include "srsran/asn1/gtpc.h"
#include "srsran/common/buffer_pool.h"
#include "srsran/common/threads.h"
#include "srsran/srslog/srslog.h"
#include <cstddef>
#include <queue>

namespace srsepc {

class mme_gtpc;

const uint16_t GTPU_RX_PORT = 2152;

typedef struct {
  std::string gtpu_bind_addr;
  std::string sgi_if_addr;
  std::string sgi_if_name;
  uint32_t    max_paging_queue;
} spgw_args_t;

typedef struct spgw_tunnel_ctx {
  uint64_t                                 imsi;
  in_addr_t                                ue_ipv4;
  uint8_t                                  ebi;
  srsran::gtp_fteid_t                      up_ctrl_fteid;
  srsran::gtp_fteid_t                      up_user_fteid;
  srsran::gtp_fteid_t                      dw_ctrl_fteid;
  srsran::gtp_fteid_t                      dw_user_fteid;
  bool                                     paging_pending;
  std::queue<srsran::unique_byte_buffer_t> paging_queue;
} spgw_tunnel_ctx_t;

class spgw : public srsran::thread
{
  class gtpc;
  class gtpu;

public:
  static spgw* get_instance(void);
  static void  cleanup(void);
  int          init(spgw_args_t* args, const std::map<std::string, uint64_t>& ip_to_imsi);
  void         stop();
  void         run_thread();

private:
  spgw();
  virtual ~spgw();
  static spgw* m_instance;

  spgw_tunnel_ctx_t* create_gtp_ctx(struct srsran::gtpc_create_session_request* cs_req);
  bool               delete_gtp_ctx(uint32_t ctrl_teid);

  bool      m_running;
  mme_gtpc* m_mme_gtpc;

  // GTP-C and GTP-U handlers
  gtpc* m_gtpc;
  gtpu* m_gtpu;

  // Logs
  srslog::basic_logger& m_logger = srslog::fetch_basic_logger("SPGW");
};

} // namespace srsepc
#endif // SRSEPC_SPGW_H
