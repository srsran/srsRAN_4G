/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#ifndef SRSRAN_GNB_EMULATOR_H
#define SRSRAN_GNB_EMULATOR_H

#include <srsran/phy/channel/channel.h>
#include <srsran/radio/rf_timestamp.h>
#include <srsran/srsran.h>
#include <srsran/support/srsran_assert.h>

class gnb_emulator
{
private:
  uint32_t              sf_len  = 0;
  srsran_carrier_nr_t   carrier = {};
  srsran_ssb_t          ssb     = {};
  srsran::channel       channel;
  std::vector<cf_t>     buffer;
  srslog::basic_logger& logger = srslog::fetch_basic_logger("GNB-EMULATOR");

public:
  struct args_t {
    double                      srate_hz;
    srsran_carrier_nr_t         carrier;
    srsran_subcarrier_spacing_t ssb_scs;
    srsran_ssb_pattern_t        ssb_pattern;
    uint32_t                    ssb_periodicity_ms;
    srsran_duplex_mode_t        duplex_mode;
    srsran::channel::args_t     channel;
    std::string                 log_level = "warning";
  };

  gnb_emulator(const args_t& args) : channel(args.channel, 1, srslog::fetch_basic_logger("GNB-EMULATOR"))
  {
    logger.set_level(srslog::str_to_basic_level(args.log_level));

    srsran_assert(
        std::isnormal(args.srate_hz) and args.srate_hz > 0, "Invalid sampling rate (%.2f MHz)", args.srate_hz);

    // Initialise internals
    sf_len  = args.srate_hz / 1000;
    carrier = args.carrier;
    buffer.resize(sf_len);

    srsran_ssb_args_t ssb_args = {};
    ssb_args.enable_encode     = true;
    srsran_assert(srsran_ssb_init(&ssb, &ssb_args) == SRSRAN_SUCCESS, "SSB initialisation failed");

    srsran_ssb_cfg_t ssb_cfg = {};
    ssb_cfg.srate_hz         = args.srate_hz;
    ssb_cfg.center_freq_hz   = args.carrier.dl_center_frequency_hz;
    ssb_cfg.ssb_freq_hz      = args.carrier.ssb_center_freq_hz;
    ssb_cfg.scs              = args.ssb_scs;
    ssb_cfg.pattern          = args.ssb_pattern;
    ssb_cfg.duplex_mode      = args.duplex_mode;
    ssb_cfg.periodicity_ms   = args.ssb_periodicity_ms;
    srsran_assert(srsran_ssb_set_cfg(&ssb, &ssb_cfg) == SRSRAN_SUCCESS, "SSB set config failed");

    // Configure channel
    channel.set_srate((uint32_t)args.srate_hz);
  }

  int work(uint32_t sf_idx, cf_t* baseband_buffer, const srsran::rf_timestamp_t& ts)
  {
    logger.set_context(sf_idx);

    // Zero buffer
    srsran_vec_cf_zero(buffer.data(), sf_len);

    // Check if SSB needs to be sent
    if (srsran_ssb_send(&ssb, sf_idx)) {
      // Prepare PBCH message
      srsran_pbch_msg_nr_t msg = {};

      // Add SSB
      if (srsran_ssb_add(&ssb, carrier.pci, &msg, buffer.data(), buffer.data()) < SRSRAN_SUCCESS) {
        logger.error("Error adding SSB");
        return SRSRAN_ERROR;
      }
    }

    // Run channel
    cf_t* in[SRSRAN_MAX_CHANNELS]  = {};
    cf_t* out[SRSRAN_MAX_CHANNELS] = {};
    in[0]                          = buffer.data();
    out[0]                         = buffer.data();
    channel.run(in, out, sf_len, ts.get(0));

    // Add buffer to baseband buffer
    srsran_vec_sum_ccc(baseband_buffer, buffer.data(), baseband_buffer, sf_len);

    return SRSRAN_SUCCESS;
  }

  ~gnb_emulator() { srsran_ssb_free(&ssb); }
};

#endif // SRSRAN_GNB_EMULATOR_H
