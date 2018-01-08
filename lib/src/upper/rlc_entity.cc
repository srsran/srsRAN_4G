/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srslte/upper/rlc_entity.h"

namespace srslte {

rlc_entity::rlc_entity()
  :rlc(NULL)
{
}

void rlc_entity::init(rlc_mode_t                   mode,
                      log                         *rlc_entity_log_,
                      uint32_t                     lcid_,
                      srsue::pdcp_interface_rlc   *pdcp_,
                      srsue::rrc_interface_rlc    *rrc_,
                      mac_interface_timers        *mac_timers_)
{
  tm.reset();
  um.reset();
  am.reset();

  switch(mode)
  {
  case RLC_MODE_TM:
    rlc = &tm;
    break;
  case RLC_MODE_UM:
    rlc = &um;
    break;
  case RLC_MODE_AM:
    rlc = &am;
    break;
  default:
    rlc_entity_log_->error("Invalid RLC mode - defaulting to TM\n");
    rlc = &tm;
    break;
  }

  rlc->init(rlc_entity_log_, lcid_, pdcp_, rrc_, mac_timers_);
}

void rlc_entity::configure(srslte_rlc_config_t cnfg)
{
  if(rlc)
    rlc->configure(cnfg);
}

void rlc_entity::reestablish() {
  rlc->reset();
}

void rlc_entity::reset()
{
  rlc->reset();
  rlc = NULL;
}

void rlc_entity::stop()
{
  rlc->stop();
  rlc = NULL;
}

void rlc_entity::empty_queue()
{
  rlc->empty_queue();
}

bool rlc_entity::active()
{
  return (rlc != NULL);
}

rlc_mode_t rlc_entity::get_mode()
{
  if(rlc)
    return rlc->get_mode();
  else
    return RLC_MODE_TM;
}

uint32_t rlc_entity::get_bearer()
{
  if(rlc)
    return rlc->get_bearer();
  else
    return 0;
}

// PDCP interface
void rlc_entity::write_sdu(byte_buffer_t *sdu)
{
  if(rlc)
    rlc->write_sdu(sdu);
}

// MAC interface
uint32_t rlc_entity::get_buffer_state()
{
  if(rlc)
    return rlc->get_buffer_state();
  else
    return 0;
}

uint32_t rlc_entity::get_total_buffer_state()
{
  if(rlc)
    return rlc->get_total_buffer_state();
  else
    return 0;
}

int rlc_entity::read_pdu(uint8_t *payload, uint32_t nof_bytes)
{
  if(rlc)
    return rlc->read_pdu(payload, nof_bytes);
  else
    return 0;
}
void rlc_entity::write_pdu(uint8_t *payload, uint32_t nof_bytes)
{
  if(rlc)
    rlc->write_pdu(payload, nof_bytes);
}

} // namespace srsue
