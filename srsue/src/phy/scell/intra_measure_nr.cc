/**
 * Copyright 2013-2022 Software Radio Systems Limited
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
#include "srsue/hdr/phy/scell/intra_measure_nr.h"

#define Log(level, fmt, ...)                                                                                           \
  do {                                                                                                                 \
    logger.level("INTRA-%s-%d: " fmt, to_string(get_rat()).c_str(), get_earfcn(), ##__VA_ARGS__);                      \
  } while (false)

namespace srsue {
namespace scell {

intra_measure_nr::intra_measure_nr(srslog::basic_logger& logger_, meas_itf& new_meas_itf_) :
  logger(logger_), intra_measure_base(logger_, new_meas_itf_)
{}

intra_measure_nr::~intra_measure_nr()
{
  srsran_ssb_free(&ssb);
}

bool intra_measure_nr::init(uint32_t cc_idx_, const args_t& args)
{
  cc_idx     = cc_idx_;
  thr_snr_db = args.thr_snr_db;

  // Initialise generic side
  intra_measure_base::args_t base_args = {};
  base_args.srate_hz                   = args.max_srate_hz;
  base_args.len_ms                     = args.max_len_ms;
  base_args.period_ms                  = 20; // Hard-coded, it does not make a difference at this stage
  base_args.rx_gain_offset_db          = args.rx_gain_offset_dB;
  init_generic(cc_idx, base_args);

  // Initialise SSB
  srsran_ssb_args_t ssb_args = {};
  ssb_args.max_srate_hz      = args.max_srate_hz;
  ssb_args.min_scs           = args.min_scs;
  ssb_args.enable_search     = true;
  if (srsran_ssb_init(&ssb, &ssb_args) < SRSRAN_SUCCESS) {
    Log(error, "Error initiating SSB");
    return false;
  }

  return true;
}

bool intra_measure_nr::set_config(const config_t& cfg)
{
  // Update ARFCN
  current_arfcn    = cfg.arfcn;
  serving_cell_pci = cfg.serving_cell_pci;

  // Reset performance measurement
  perf_count_samples = 0;
  perf_count_us      = 0;

  // Re-configure generic side
  init_generic(cc_idx, cfg);

  // Configure SSB
  srsran_ssb_cfg_t ssb_cfg = {};
  ssb_cfg.srate_hz         = cfg.srate_hz;
  ssb_cfg.center_freq_hz   = cfg.center_freq_hz;
  ssb_cfg.ssb_freq_hz      = cfg.ssb_freq_hz;
  ssb_cfg.scs              = cfg.scs;
  if (srsran_ssb_set_cfg(&ssb, &ssb_cfg) < SRSRAN_SUCCESS) {
    Log(error, "Error configuring SSB");
    return false;
  }

  return true;
}

bool intra_measure_nr::measure_rat(const measure_context_t& context, std::vector<cf_t>& buffer, float rx_gain_offset)
{
  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

  // Search and measure the best cell
  srsran_csi_trs_measurements_t meas = {};
  uint32_t                      N_id = 0;
  if (srsran_ssb_csi_search(&ssb, buffer.data(), context.sf_len * context.meas_len_ms, &N_id, &meas) < SRSRAN_SUCCESS) {
    Log(error, "Error searching for SSB");
    return false;
  }

  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
  perf_count_us += std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
  perf_count_samples += (uint64_t)context.sf_len * (uint64_t)context.meas_len_ms;

  // Early return if the found PCI matches with the serving cell ID
  if (serving_cell_pci == (int)N_id) {
    return true;
  }

  // Take valid decision if SNR threshold is exceeded
  bool valid = (meas.snr_dB >= thr_snr_db);

  // Log finding
  if ((logger.info.enabled() and valid) or logger.debug.enabled()) {
    std::array<char, 512> str_info = {};
    srsran_csi_rs_measure_info(&meas, str_info.data(), (uint32_t)str_info.size());
    Log(info, "%s neighbour cell: PCI=%03d %s", valid ? "Found" : "Best", N_id, str_info.data());
  }

  // Check threshold
  if (valid) {
    // Prepare found measurements
    std::vector<phy_meas_t> meas_list(1);
    meas_list[0].rat    = get_rat();
    meas_list[0].rsrp   = meas.rsrp_dB + rx_gain_offset_db;
    meas_list[0].cfo_hz = meas.cfo_hz;
    meas_list[0].earfcn = get_earfcn();
    meas_list[0].pci    = N_id;

    // Push measurements to higher layers
    context.new_cell_itf.new_cell_meas(cc_idx, meas_list);
  }

  return true;
}

} // namespace scell
} // namespace srsue