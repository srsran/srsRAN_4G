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

#ifndef SRSRAN_DUMMY_NR_CLASSES_H
#define SRSRAN_DUMMY_NR_CLASSES_H

#include "srsran/interfaces/gnb_interfaces.h"

namespace srsenb {

class mac_dummy : public mac_interface_rrc_nr
{
public:
  int cell_cfg(srsenb::sched_interface::cell_cfg_t* cell_cfg_)
  {
    cellcfgobj = *cell_cfg_;
    return SRSRAN_SUCCESS;
  }

  srsenb::sched_interface::cell_cfg_t cellcfgobj;
};

class rlc_dummy : public rlc_interface_rrc_nr
{
public:
  void clear_buffer(uint16_t rnti) override {}
  void add_user(uint16_t rnti) override {}
  void rem_user(uint16_t rnti) override {}
  void add_bearer(uint16_t rnti, uint32_t lcid, srsran::rlc_config_t cnfg) override {}
  void add_bearer_mrb(uint16_t rnti, uint32_t lcid) override {}
  void write_sdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t sdu) override { last_sdu = std::move(sdu); }

  srsran::unique_byte_buffer_t last_sdu;
};

class pdcp_dummy : public pdcp_interface_rrc_nr
{
public:
  void reset(uint16_t rnti) override {}
  void add_user(uint16_t rnti) override {}
  void rem_user(uint16_t rnti) override {}
  void write_sdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t sdu) override {}
  void add_bearer(uint16_t rnti, uint32_t lcid, srsran::pdcp_config_t cnfg) override {}
  void config_security(uint16_t rnti, uint32_t lcid, srsran::as_security_config_t sec_cfg) override {}
  void enable_integrity(uint16_t rnti, uint32_t lcid) override {}
  void enable_encryption(uint16_t rnti, uint32_t lcid) override {}
};

} // namespace srsenb

#endif // SRSRAN_DUMMY_NR_CLASSES_H
