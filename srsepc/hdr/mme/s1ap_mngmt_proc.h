// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom
#ifndef SRSEPC_S1AP_MNGMT_PROC_H
#define SRSEPC_S1AP_MNGMT_PROC_H

#include "s1ap_common.h"
#include "srsran/asn1/s1ap.h"
#include "srsran/common/common.h"
#include "srsran/srslog/srslog.h"

namespace srsepc {

class s1ap;

class s1ap_mngmt_proc
{
public:
  static s1ap_mngmt_proc* m_instance;

  static s1ap_mngmt_proc* get_instance(void);
  static void             cleanup(void);
  void                    init(void);

  bool handle_s1_setup_request(const asn1::s1ap::s1_setup_request_s& msg, sctp_sndrcvinfo* enb_sri);

  // Packing/unpacking helper functions
  bool unpack_s1_setup_request(const asn1::s1ap::s1_setup_request_s& msg, enb_ctx_t* enb_ctx);
  bool send_s1_setup_failure(asn1::s1ap::cause_misc_opts::options cause, struct sctp_sndrcvinfo* enb_sri);
  bool send_s1_setup_response(const s1ap_args_t& s1ap_args, struct sctp_sndrcvinfo* enb_sri);

private:
  s1ap_mngmt_proc();
  virtual ~s1ap_mngmt_proc();

  s1ap*                 m_s1ap;
  srslog::basic_logger& m_logger = srslog::fetch_basic_logger("S1AP");

  int         m_s1mme;
  s1ap_args_t m_s1ap_args;
};

} // namespace srsepc

#endif // SRSEPC_S1AP_MNGMT_PROC_H
