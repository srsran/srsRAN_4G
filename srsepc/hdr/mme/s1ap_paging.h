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
#ifndef SRSEPC_S1AP_PAGING_H
#define SRSEPC_S1AP_PAGING_H

#include "s1ap_common.h"
#include "srslte/common/buffer_pool.h"
#include "srslte/common/common.h"
#include "srslte/common/log_filter.h"
#include "srslte/srslog/srslog.h"

namespace srsepc {

class s1ap;
class mme;

class s1ap_paging
{
  s1ap_paging() = default;

public:
  virtual ~s1ap_paging() = default;

  static s1ap_paging* get_instance();
  void                init();

  // Packing/unpacking helper functions
  bool send_paging(uint64_t imsi, uint16_t erab_to_setup);

private:
  mme*                  m_mme;
  s1ap*                 m_s1ap;
  srslog::basic_logger& m_logger = srslog::fetch_basic_logger("S1AP");

  s1ap_args_t               m_s1ap_args;
  srslte::byte_buffer_pool* m_pool;
};

} // namespace srsepc

#endif // SRSEPC_S1AP_PAGING_H
