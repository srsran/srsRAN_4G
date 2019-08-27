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

#ifndef SRSLTE_CHANNEL_H
#define SRSLTE_CHANNEL_H

#include "delay.h"
#include "fading.h"
#include "hst.h"
#include "rlf.h"
#include <memory>
#include <srslte/common/log_filter.h>
#include <string>

namespace srslte {

class channel
{
public:
  typedef struct {
    // General
    bool enable = false;

    // Fading options
    bool        fading_enable = false;
    std::string fading_model  = "none";

    // High Speed Train options
    bool  hst_enable      = false;
    float hst_fd_hz       = 750.0f;
    float hst_period_s    = 7.2f;
    float hst_init_time_s = 0.0f;

    // Delay options
    bool  delay_enable      = false;
    float delay_min_us      = 10;
    float delay_max_us      = 100;
    float delay_period_s    = 3600;
    float delay_init_time_s = 0;

    // RLF options
    bool     rlf_enable   = false;
    uint32_t rlf_t_on_ms  = 10000;
    uint32_t rlf_t_off_ms = 2000;
  } args_t;

  channel(const args_t& channel_args, uint32_t _nof_ports);
  ~channel();
  void set_logger(log_filter* _log_h);
  void set_srate(uint32_t srate);
  void run(cf_t* in[SRSLTE_MAX_PORTS], cf_t* out[SRSLTE_MAX_PORTS], uint32_t len, const srslte_timestamp_t& t);

private:
  srslte_channel_fading_t* fading[SRSLTE_MAX_PORTS] = {};
  srslte_channel_delay_t*  delay[SRSLTE_MAX_PORTS]  = {};
  srslte_channel_hst_t*    hst                      = nullptr; // HST has no buffers / no multiple instance is required
  srslte_channel_rlf_t*    rlf                      = nullptr; // RLF has no buffers / no multiple instance is required
  cf_t*                    buffer_in                = nullptr;
  cf_t*                    buffer_out               = nullptr;
  log_filter*              log_h                    = nullptr;
  uint32_t                 nof_ports                = 0;
  uint32_t                 current_srate            = 0;
  args_t                   args                     = {};
};

typedef std::unique_ptr<channel> channel_ptr;

} // namespace srslte

#endif // SRSLTE_CHANNEL_H
