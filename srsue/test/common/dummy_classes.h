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

#ifndef SRSUE_DUMMY_CLASSES_H
#define SRSUE_DUMMY_CLASSES_H

#include "srslte/interfaces/ue_interfaces.h"

namespace srsue {

class stack_dummy : public stack_interface_rrc
{
public:
  srslte::timer_handler::unique_timer get_unique_timer() override { return timers.get_unique_timer(); }
  void                                start_cell_search() override {}
  void                                start_cell_select(const phy_interface_rrc_lte::phy_cell_t* cell) override {}
  srslte::tti_point get_current_tti() override { return srslte::tti_point{timers.get_cur_time() % 10240}; }

  srslte::timer_handler timers{100};
};

} // namespace srsue

#endif // SRSUE_DUMMY_CLASSES_H
