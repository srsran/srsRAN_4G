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

#include <cstdlib>
#include <srslte/phy/channel/channel.h>
#include <srslte/srslte.h>

using namespace srslte;

channel::channel(const channel::args_t& channel_args, uint32_t _nof_ports)
{
  int      ret         = SRSLTE_SUCCESS;
  uint32_t srate_max   = (uint32_t)srslte_symbol_sz(SRSLTE_MAX_PRB) * 15000;
  uint32_t buffer_size = (uint32_t)SRSLTE_SF_LEN_PRB(SRSLTE_MAX_PRB) * 5; // be safe, 5 Subframes

  // Copy args
  args = channel_args;

  // Allocate internal buffers
  buffer_in  = (cf_t*)srslte_vec_malloc(sizeof(cf_t) * buffer_size);
  buffer_out = (cf_t*)srslte_vec_malloc(sizeof(cf_t) * buffer_size);
  if (!buffer_out || !buffer_in) {
    ret = SRSLTE_ERROR;
  }

  nof_ports = _nof_ports;
  for (uint32_t i = 0; i < nof_ports; i++) {
    // Create fading channel
    if (channel_args.fading_enable && !channel_args.fading_model.empty() && channel_args.fading_model != "none" &&
        ret == SRSLTE_SUCCESS) {
      fading[i] = (srslte_channel_fading_t*)calloc(sizeof(srslte_channel_fading_t), 1);
      ret       = srslte_channel_fading_init(fading[i], srate_max, channel_args.fading_model.c_str(), 0x1234 * i);
    } else {
      fading[i] = nullptr;
    }

    // Create delay
    if (channel_args.delay_enable && ret == SRSLTE_SUCCESS) {
      delay[i] = (srslte_channel_delay_t*)calloc(sizeof(srslte_channel_delay_t), 1);
      ret      = srslte_channel_delay_init(delay[i],
                                      channel_args.delay_min_us,
                                      channel_args.delay_max_us,
                                      channel_args.delay_period_s,
                                      channel_args.delay_init_time_s,
                                      srate_max);
    } else {
      delay[i] = nullptr;
    }
  }

  // Create high speed train
  if (channel_args.hst_enable && ret == SRSLTE_SUCCESS) {
    hst = (srslte_channel_hst_t*)calloc(sizeof(srslte_channel_hst_t), 1);
    srslte_channel_hst_init(hst, channel_args.hst_fd_hz, channel_args.hst_period_s, channel_args.hst_init_time_s);
  }

  // Create Radio Link Failure simulator
  if (channel_args.rlf_enable && ret == SRSLTE_SUCCESS) {
    rlf = (srslte_channel_rlf_t*)calloc(sizeof(srslte_channel_rlf_t), 1);
    srslte_channel_rlf_init(rlf, channel_args.rlf_t_on_ms, channel_args.rlf_t_off_ms);
  }

  if (ret != SRSLTE_SUCCESS) {
    fprintf(stderr, "Error: Creating channel\n\n");
  }
}

channel::~channel()
{
  if (buffer_in) {
    free(buffer_in);
  }

  if (buffer_out) {
    free(buffer_out);
  }

  if (hst) {
    srslte_channel_hst_free(hst);
    free(hst);
  }

  if (rlf) {
    srslte_channel_rlf_free(rlf);
    free(rlf);
  }

  for (uint32_t i = 0; i < nof_ports; i++) {
    if (fading[i]) {
      srslte_channel_fading_free(fading[i]);
      free(fading[i]);
    }

    if (delay[i]) {
      srslte_channel_delay_free(delay[i]);
      free(delay[i]);
    }
  }
}

void channel::set_logger(log_filter* _log_h)
{
  log_h = _log_h;
}

void channel::run(cf_t* in[SRSLTE_MAX_PORTS], cf_t* out[SRSLTE_MAX_PORTS], uint32_t len, const srslte_timestamp_t& t)
{
  // check input pointers
  if (in != nullptr && out != nullptr) {
    if (current_srate) {
      for (uint32_t i = 0; i < nof_ports; i++) {
        // Check buffers are not null
        if (in[i] != nullptr && out[i] != nullptr) {
          // Copy input buffer
          memcpy(buffer_in, in[i], sizeof(cf_t) * len);

          if (fading[i]) {
            srslte_channel_fading_execute(fading[i], buffer_in, buffer_out, len, t.full_secs + t.frac_secs);
            memcpy(buffer_in, buffer_out, sizeof(cf_t) * len);
          }

          if (delay[i]) {
            srslte_channel_delay_execute(delay[i], buffer_in, buffer_out, len, &t);
            memcpy(buffer_in, buffer_out, sizeof(cf_t) * len);
          }

          if (hst) {
            srslte_channel_hst_execute(hst, buffer_in, buffer_out, len, &t);
            memcpy(buffer_in, buffer_out, sizeof(cf_t) * len);
          }

          if (rlf) {
            srslte_channel_rlf_execute(rlf, buffer_in, buffer_out, len, &t);
            memcpy(buffer_in, buffer_out, sizeof(cf_t) * len);
          }

          // Copy output buffer
          memcpy(out[i], buffer_in, sizeof(cf_t) * len);
        }
      }

      if (log_h) {
        // Logging
        std::stringstream str;

        str << "t=" << t.full_secs + t.frac_secs << "s; ";

        if (delay[0]) {
          str << "delay=" << delay[0]->delay_us << "us; ";
        }

        if (hst) {
          str << "hst=" << hst->fs_hz << "Hz; ";
        }

        log_h->debug("%s\n", str.str().c_str());
      }

    } else {
      for (uint32_t i = 0; i < nof_ports; i++) {
        // Check buffers are not null
        if (in[i] != nullptr && out[i] != nullptr && in[i] != out[i]) {
          memcpy(out[i], in[i], sizeof(cf_t) * len);
        }
      }
    }
  }
}

void channel::set_srate(uint32_t srate)
{
  if (current_srate != srate) {
    for (uint32_t i = 0; i < nof_ports; i++) {
      if (fading[i]) {
        srslte_channel_fading_free(fading[i]);

        srslte_channel_fading_init(fading[i], srate, args.fading_model.c_str(), 0x1234 * i);
      }

      if (delay[i]) {
        srslte_channel_delay_update_srate(delay[i], srate);
      }
      current_srate = srate;
    }

    if (hst) {
      srslte_channel_hst_update_srate(hst, srate);
    }
  }
}
