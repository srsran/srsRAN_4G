/**
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
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
#include <srsran/phy/channel/channel.h>
#include <srsran/srsran.h>

using namespace srsran;

channel::channel(const channel::args_t& channel_args, uint32_t _nof_channels, srslog::basic_logger& logger) :
  logger(logger)
{
  int      ret         = SRSRAN_SUCCESS;
  uint32_t srate_max   = (uint32_t)srsran_symbol_sz(SRSRAN_MAX_PRB) * 15000;
  uint32_t buffer_size = (uint32_t)SRSRAN_SF_LEN_PRB(SRSRAN_MAX_PRB) * 5; // be safe, 5 Subframes

  if (_nof_channels > SRSRAN_MAX_CHANNELS) {
    fprintf(stderr,
            "Error creating channel object: maximum number of channels exceeded (%d > %d)\n",
            _nof_channels,
            SRSRAN_MAX_CHANNELS);
    return;
  }

  // Copy args
  args = channel_args;

  // Allocate internal buffers
  buffer_in  = srsran_vec_cf_malloc(buffer_size);
  buffer_out = srsran_vec_cf_malloc(buffer_size);
  if (!buffer_out || !buffer_in) {
    ret = SRSRAN_ERROR;
  }

  nof_channels = _nof_channels;
  for (uint32_t i = 0; i < nof_channels; i++) {
    // Create fading channel
    if (channel_args.fading_enable && !channel_args.fading_model.empty() && channel_args.fading_model != "none" &&
        ret == SRSRAN_SUCCESS) {
      fading[i] = (srsran_channel_fading_t*)calloc(sizeof(srsran_channel_fading_t), 1);
      ret       = srsran_channel_fading_init(fading[i], srate_max, channel_args.fading_model.c_str(), 0x1234 * i);
    } else {
      fading[i] = nullptr;
    }

    // Create delay
    if (channel_args.delay_enable && ret == SRSRAN_SUCCESS) {
      delay[i] = (srsran_channel_delay_t*)calloc(sizeof(srsran_channel_delay_t), 1);
      ret      = srsran_channel_delay_init(delay[i],
                                      channel_args.delay_min_us,
                                      channel_args.delay_max_us,
                                      channel_args.delay_period_s,
                                      channel_args.delay_init_time_s,
                                      srate_max);
    } else {
      delay[i] = nullptr;
    }
  }

  // Create AWGN channnel
  if (channel_args.awgn_enable && ret == SRSRAN_SUCCESS) {
    awgn = (srsran_channel_awgn_t*)calloc(sizeof(srsran_channel_awgn_t), 1);
    ret  = srsran_channel_awgn_init(awgn, 1234);
    srsran_channel_awgn_set_n0(awgn, args.awgn_signal_power_dBfs - args.awgn_snr_dB);
  }

  // Create high speed train
  if (channel_args.hst_enable && ret == SRSRAN_SUCCESS) {
    hst = (srsran_channel_hst_t*)calloc(sizeof(srsran_channel_hst_t), 1);
    srsran_channel_hst_init(hst, channel_args.hst_fd_hz, channel_args.hst_period_s, channel_args.hst_init_time_s);
  }

  // Create Radio Link Failure simulator
  if (channel_args.rlf_enable && ret == SRSRAN_SUCCESS) {
    rlf = (srsran_channel_rlf_t*)calloc(sizeof(srsran_channel_rlf_t), 1);
    srsran_channel_rlf_init(rlf, channel_args.rlf_t_on_ms, channel_args.rlf_t_off_ms);
  }

  if (ret != SRSRAN_SUCCESS) {
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

  if (awgn) {
    srsran_channel_awgn_free(awgn);
    free(awgn);
  }

  if (hst) {
    srsran_channel_hst_free(hst);
    free(hst);
  }

  if (rlf) {
    srsran_channel_rlf_free(rlf);
    free(rlf);
  }

  for (uint32_t i = 0; i < nof_channels; i++) {
    if (fading[i]) {
      srsran_channel_fading_free(fading[i]);
      free(fading[i]);
    }

    if (delay[i]) {
      srsran_channel_delay_free(delay[i]);
      free(delay[i]);
    }
  }
}

extern "C" {
static inline cf_t local_cexpf(float phase)
{
  cf_t ret;
  __real__ ret = cosf(phase);
  __imag__ ret = sinf(phase);
  return ret;
}
}

void channel::run(cf_t*                     in[SRSRAN_MAX_CHANNELS],
                  cf_t*                     out[SRSRAN_MAX_CHANNELS],
                  uint32_t                  len,
                  const srsran_timestamp_t& t)
{
  // Early return if pointers are not enabled
  if (in == nullptr || out == nullptr) {
    return;
  }

  // For each channel
  for (uint32_t i = 0; i < nof_channels; i++) {
    // Skip iteration if any buffer is null
    if (in[i] == nullptr || out[i] == nullptr) {
      continue;
    }

    // If sampling rate is not set, copy input and skip rest of channel
    if (current_srate == 0) {
      if (in[i] != out[i]) {
        srsran_vec_cf_copy(out[i], in[i], len);
      }
      continue;
    }

    // Copy input buffer
    srsran_vec_cf_copy(buffer_in, in[i], len);

    if (hst) {
      srsran_channel_hst_execute(hst, buffer_in, buffer_out, len, &t);
      srsran_vec_sc_prod_ccc(buffer_out, local_cexpf(hst_init_phase), buffer_in, len);
    }

    if (awgn) {
      srsran_channel_awgn_run_c(awgn, buffer_in, buffer_out, len);
      srsran_vec_cf_copy(buffer_in, buffer_out, len);
    }

    if (fading[i]) {
      srsran_channel_fading_execute(fading[i], buffer_in, buffer_out, len, t.full_secs + t.frac_secs);
      srsran_vec_cf_copy(buffer_in, buffer_out, len);
    }

    if (delay[i]) {
      srsran_channel_delay_execute(delay[i], buffer_in, buffer_out, len, &t);
      srsran_vec_cf_copy(buffer_in, buffer_out, len);
    }

    if (rlf) {
      srsran_channel_rlf_execute(rlf, buffer_in, buffer_out, len, &t);
      srsran_vec_cf_copy(buffer_in, buffer_out, len);
    }

    // Copy output buffer
    srsran_vec_cf_copy(out[i], buffer_in, len);
  }

  if (hst) {
    // Increment phase to keep it coherent between frames
    hst_init_phase += (2 * M_PI * len * hst->fs_hz / hst->srate_hz);

    // Positive Remainder
    while (hst_init_phase > 2 * M_PI) {
      hst_init_phase -= 2 * M_PI;
    }

    // Negative Remainder
    while (hst_init_phase < -2 * M_PI) {
      hst_init_phase += 2 * M_PI;
    }
  }

  // Logging
  std::stringstream str;
  str << "t=" << t.full_secs + t.frac_secs << "s; ";
  if (delay[0]) {
    str << "delay=" << delay[0]->delay_us << "us; ";
  }
  if (hst) {
    str << "hst=" << hst->fs_hz << "Hz; ";
  }
  logger.debug("%s", str.str().c_str());
}

void channel::set_srate(uint32_t srate)
{
  if (current_srate != srate) {
    for (uint32_t i = 0; i < nof_channels; i++) {
      if (fading[i]) {
        srsran_channel_fading_free(fading[i]);

        srsran_channel_fading_init(fading[i], srate, args.fading_model.c_str(), 0x1234 * i);
      }

      if (delay[i]) {
        srsran_channel_delay_update_srate(delay[i], srate);
      }
    }

    if (hst) {
      srsran_channel_hst_update_srate(hst, srate);
    }

    // Update sampling rate
    current_srate = srate;
  }
}

void channel::set_signal_power_dBfs(float power_dBfs)
{
  if (awgn != nullptr) {
    srsran_channel_awgn_set_n0(awgn, power_dBfs - args.awgn_snr_dB);
  }
}
