/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

#ifndef SRSENB_RRC_MOBILITY_H
#define SRSENB_RRC_MOBILITY_H

#include "rrc.h"
#include <map>

namespace srsenb {

class rrc::mobility_cfg
{
public:
  explicit mobility_cfg(rrc* outer_rrc);

private:
  rrc* rrc_enb = nullptr;
};

class rrc::ue::rrc_mobility
{
public:
  rrc_mobility(srsenb::rrc::ue* outer_ue);

private:
  rrc::ue*                  rrc_ue;
  rrc*                      rrc_enb;
  rrc::mobility_cfg*        cfg;
  srslte::byte_buffer_pool* pool;
  srslte::log*              rrc_log;
};

} // namespace srsenb
#endif // SRSENB_RRC_MOBILITY_H
