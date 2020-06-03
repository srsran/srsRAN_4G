/*
 * Copyright 2013-2020 Software Radio Systems Limited
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

#ifndef SRSLTE_DUMMY_NR_CLASSES_H
#define SRSLTE_DUMMY_NR_CLASSES_H

#include "srslte/interfaces/gnb_interfaces.h"

namespace srsenb {

class mac_dummy : public mac_interface_rrc_nr
{
public:
  int cell_cfg(srsenb::sched_interface::cell_cfg_t* cell_cfg_)
  {
    cellcfgobj = *cell_cfg_;
    return SRSLTE_SUCCESS;
  }

  srsenb::sched_interface::cell_cfg_t cellcfgobj;
};

class rlc_dummy : public rlc_interface_rrc_nr
{
public:
  void clear_buffer(uint16_t rnti) override {}
  void add_user(uint16_t rnti) override {}
  void rem_user(uint16_t rnti) override {}
  void add_bearer(uint16_t rnti, uint32_t lcid, srslte::rlc_config_t cnfg) override {}
  void add_bearer_mrb(uint16_t rnti, uint32_t lcid) override {}
  void write_sdu(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t sdu) override { last_sdu = std::move(sdu); }

  srslte::unique_byte_buffer_t last_sdu;
};

class pdcp_dummy : public pdcp_interface_rrc_nr
{
public:
  void reset(uint16_t rnti) override {}
  void add_user(uint16_t rnti) override {}
  void rem_user(uint16_t rnti) override {}
  void write_sdu(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t sdu) override {}
  void add_bearer(uint16_t rnti, uint32_t lcid, srslte::pdcp_config_t cnfg) override {}
  void config_security(uint16_t rnti, uint32_t lcid, srslte::as_security_config_t sec_cfg) override {}
  void enable_integrity(uint16_t rnti, uint32_t lcid) override {}
  void enable_encryption(uint16_t rnti, uint32_t lcid) override {}
};

} // namespace srsenb

#endif // SRSLTE_DUMMY_NR_CLASSES_H
