// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom
#ifndef SRSEPC_S1AP_PAGING_H
#define SRSEPC_S1AP_PAGING_H

#include "s1ap_common.h"
#include "srsran/common/buffer_pool.h"
#include "srsran/common/common.h"
#include "srsran/srslog/srslog.h"

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
  mme*                  m_mme    = nullptr;
  s1ap*                 m_s1ap   = nullptr;
  srslog::basic_logger& m_logger = srslog::fetch_basic_logger("S1AP");

  s1ap_args_t m_s1ap_args;
};

} // namespace srsepc

#endif // SRSEPC_S1AP_PAGING_H
