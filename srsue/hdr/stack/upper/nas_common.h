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

#ifndef SRSUE_NAS_COMMON_H
#define SRSUE_NAS_COMMON_H

namespace srsue {

typedef struct {
  int airplane_t_on_ms  = -1;
  int airplane_t_off_ms = -1;
} nas_sim_args_t;

class nas_args_t
{
public:
  nas_args_t() : force_imsi_attach(false) {}

  std::string    apn_name;
  std::string    apn_protocol;
  std::string    apn_user;
  std::string    apn_pass;
  bool           force_imsi_attach;
  std::string    eia;
  std::string    eea;
  nas_sim_args_t sim;
};

// EMM states (3GPP 24.302 v10.0.0)
typedef enum {
  EMM_STATE_NULL = 0,
  EMM_STATE_DEREGISTERED,
  EMM_STATE_REGISTERED,
  EMM_STATE_DEREGISTERED_INITIATED,
  EMM_STATE_TAU_INITIATED,
  EMM_STATE_N_ITEMS,
} emm_state_t;
static const char emm_state_text[EMM_STATE_N_ITEMS][100] = {"NULL",
                                                            "DEREGISTERED",
                                                            "REGISTERED",
                                                            "DEREGISTERED INITIATED",
                                                            "TRACKING AREA UPDATE INITIATED"};

} // namespace srsue

#endif // SRSUE_NAS_COMMON_H
