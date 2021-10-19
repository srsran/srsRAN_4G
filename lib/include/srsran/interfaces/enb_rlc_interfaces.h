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

#ifndef SRSRAN_ENB_RLC_INTERFACES_H
#define SRSRAN_ENB_RLC_INTERFACES_H

#include "srsran/common/byte_buffer.h"
#include "srsran/interfaces/rlc_interface_types.h"

namespace srsenb {

// RLC interface for MAC
class rlc_interface_mac
{
public:
  /* MAC calls RLC to get RLC segment of nof_bytes length.
   * Segmentation happens in this function. RLC PDU is stored in payload. */
  virtual int read_pdu(uint16_t rnti, uint32_t lcid, uint8_t* payload, uint32_t nof_bytes) = 0;

  /* MAC calls RLC to push an RLC PDU. This function is called from an independent MAC thread.
   * PDU gets placed into the buffer and higher layer thread gets notified. */
  virtual void write_pdu(uint16_t rnti, uint32_t lcid, uint8_t* payload, uint32_t nof_bytes) = 0;
};

// RLC interface for PDCP
class rlc_interface_pdcp
{
public:
  /* PDCP calls RLC to push an RLC SDU. SDU gets placed into the RLC buffer and MAC pulls
   * RLC PDUs according to TB size. */
  virtual void write_sdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t sdu) = 0;
  virtual void discard_sdu(uint16_t rnti, uint32_t lcid, uint32_t sn)                    = 0;
  virtual bool rb_is_um(uint16_t rnti, uint32_t lcid)                                    = 0;
  virtual bool sdu_queue_is_full(uint16_t rnti, uint32_t lcid)                           = 0;
  virtual bool is_suspended(uint16_t rnti, uint32_t lcid)                                = 0;
};

// RLC interface for RRC
class rlc_interface_rrc
{
public:
  virtual void clear_buffer(uint16_t rnti)                                               = 0;
  virtual void add_user(uint16_t rnti)                                                   = 0;
  virtual void rem_user(uint16_t rnti)                                                   = 0;
  virtual void add_bearer(uint16_t rnti, uint32_t lcid, srsran::rlc_config_t cnfg)       = 0;
  virtual void add_bearer_mrb(uint16_t rnti, uint32_t lcid)                              = 0;
  virtual void del_bearer(uint16_t rnti, uint32_t lcid)                                  = 0;
  virtual void write_sdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t sdu) = 0;
  virtual bool has_bearer(uint16_t rnti, uint32_t lcid)                                  = 0;
  virtual bool suspend_bearer(uint16_t rnti, uint32_t lcid)                              = 0;
  virtual bool is_suspended(uint16_t rnti, uint32_t lcid)                                = 0;
  virtual bool resume_bearer(uint16_t rnti, uint32_t lcid)                               = 0;
  virtual void reestablish(uint16_t rnti)                                                = 0;
};

} // namespace srsenb

#endif // SRSRAN_ENB_RLC_INTERFACES_H
