// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

#ifndef SRSENB_NGAP_INTERFACES_H
#define SRSENB_NGAP_INTERFACES_H

#include "srsran/asn1/ngap_utils.h"

namespace srsenb {
class ngap_interface_ngap_proc
{
public:
  virtual bool send_initial_ctxt_setup_response() = 0;
  virtual bool
               send_pdu_session_resource_setup_response(uint16_t                                    pdu_session_id,
                                                        uint32_t                                    teid_out,
                                                        asn1::bounded_bitstring<1, 160, true, true> transport_layer_address) = 0;
  virtual bool send_ue_ctxt_release_complete() = 0;
};
} // namespace srsenb

#endif