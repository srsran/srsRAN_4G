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

#ifndef SRSRAN_ENB_GTPU_INTERFACES_H
#define SRSRAN_ENB_GTPU_INTERFACES_H

#include "srsran/adt/expected.h"
#include "srsran/common/byte_buffer.h"

namespace srsenb {

struct gtpu_args_t {
  std::string gtp_bind_addr;
  std::string mme_addr;
  std::string embms_m1u_multiaddr;
  std::string embms_m1u_if_addr;
  bool        embms_enable                 = false;
  uint32_t    indirect_tunnel_timeout_msec = 0;
};

// GTPU interface for PDCP
class gtpu_interface_pdcp
{
public:
  virtual void write_pdu(uint16_t rnti, uint32_t bearer_id, srsran::unique_byte_buffer_t pdu) = 0;
};

// GTPU interface for RRC
class gtpu_interface_rrc
{
public:
  struct bearer_props {
    bool     forward_from_teidin_present = false;
    bool     flush_before_teidin_present = false;
    uint32_t forward_from_teidin         = 0;
    uint32_t flush_before_teidin         = 0;
  };

  virtual srsran::expected<uint32_t> add_bearer(uint16_t            rnti,
                                                uint32_t            eps_bearer_id,
                                                uint32_t            addr,
                                                uint32_t            teid_out,
                                                uint32_t&           addr_in,
                                                const bearer_props* props = nullptr)       = 0;
  virtual void                       set_tunnel_status(uint32_t teidin, bool dl_active)    = 0;
  virtual void                       rem_bearer(uint16_t rnti, uint32_t eps_bearer_id)     = 0;
  virtual void                       mod_bearer_rnti(uint16_t old_rnti, uint16_t new_rnti) = 0;
  virtual void                       rem_user(uint16_t rnti)                               = 0;
};

} // namespace srsenb

#endif // SRSRAN_ENB_GTPU_INTERFACES_H
