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

#ifndef SRSLTE_ENB_GTPU_INTERFACES_H
#define SRSLTE_ENB_GTPU_INTERFACES_H

#include "srslte/common/byte_buffer.h"

namespace srsenb {

// GTPU interface for PDCP
class gtpu_interface_pdcp
{
public:
  virtual void write_pdu(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t pdu) = 0;
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

  virtual uint32_t
               add_bearer(uint16_t rnti, uint32_t lcid, uint32_t addr, uint32_t teid_out, const bearer_props* props = nullptr) = 0;
  virtual void set_tunnel_status(uint32_t teidin, bool dl_active)    = 0;
  virtual void rem_bearer(uint16_t rnti, uint32_t lcid)              = 0;
  virtual void mod_bearer_rnti(uint16_t old_rnti, uint16_t new_rnti) = 0;
  virtual void rem_user(uint16_t rnti)                               = 0;
};

} // namespace srsenb

#endif // SRSLTE_ENB_GTPU_INTERFACES_H
