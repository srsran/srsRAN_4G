/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsue/hdr/phy/nr/cell_search.h"
#include "srsran/common/band_helper.h"
#include "srsran/common/buffer_pool.h"
#include "srsran/radio/rf_buffer.h"
#include "srsran/radio/rf_timestamp.h"

namespace srsue {
namespace nr {
cell_search::cell_search(stack_interface_phy_sa_nr& stack_, srsran::radio_interface_phy& radio_) :
  logger(srslog::fetch_basic_logger("PHY")),
  stack(stack_),
  radio(radio_)
{}

cell_search::~cell_search()
{
  srsran_ssb_free(&ssb);
  if (buffer != nullptr) {
    free(buffer);
  }
}

bool cell_search::init(const args_t& args)
{
  // Compute subframe size
  sf_sz = (uint32_t)(args.max_srate_hz / 1000.0f);

  // Allocate receive buffer
  buffer = srsran_vec_cf_malloc(2 * sf_sz);
  if (buffer == nullptr) {
    logger.error("Error allocating buffer");
    return false;
  }

  // Prepare SSB initialization arguments
  srsran_ssb_args_t ssb_args = {};
  ssb_args.max_srate_hz      = args.max_srate_hz;
  ssb_args.min_scs           = args.ssb_min_scs;
  ssb_args.enable_search     = true;
  ssb_args.enable_decode     = true;

  // Initialise SSB
  if (srsran_ssb_init(&ssb, &ssb_args) < SRSRAN_SUCCESS) {
    logger.error("Cell search: Error initiating SSB");
    return false;
  }

  return true;
}

bool cell_search::start(const cfg_t& cfg)
{
  // Prepare SSB configuration
  srsran_ssb_cfg_t ssb_cfg = {};
  ssb_cfg.srate_hz         = cfg.srate_hz;
  ssb_cfg.center_freq_hz   = cfg.center_freq_hz;
  ssb_cfg.ssb_freq_hz      = cfg.ssb_freq_hz;
  ssb_cfg.scs              = cfg.ssb_scs;
  ssb_cfg.pattern          = cfg.ssb_pattern;
  ssb_cfg.duplex_mode      = cfg.duplex_mode;

  // Configure SSB
  if (srsran_ssb_set_cfg(&ssb, &ssb_cfg) < SRSRAN_SUCCESS) {
    logger.error("Cell search: Error setting SSB configuration");
    return false;
  }

  // Set RX frequency
  radio.set_rx_freq(0, cfg.center_freq_hz);

  // Zero receive buffer
  srsran_vec_zero(buffer, sf_sz);

  return true;
}

bool cell_search::run()
{
  // Setup RF buffer for 1ms worth of samples
  srsran::rf_buffer_t rf_buffer = {};
  rf_buffer.set_nof_samples(sf_sz);
  rf_buffer.set(0, buffer + ssb.ssb_sz);

  // Receive
  srsran::rf_timestamp_t rf_timestamp = {};
  if (not radio.rx_now(rf_buffer, rf_timestamp)) {
    return false;
  }

  // Search for SSB
  srsran_ssb_search_res_t res = {};
  if (srsran_ssb_search(&ssb, buffer, sf_sz + ssb.ssb_sz, &res) < SRSRAN_SUCCESS) {
    logger.error("Error occurred searching SSB");
    return false;
  }

  // Consider the SSB is found and decoded if the PBCH CRC matched
  if (res.pbch_msg.crc) {
    rrc_interface_phy_sa_nr::cell_search_result_t cs_res = {};
    cs_res.pci                                           = res.N_id;
    cs_res.barred                                        = false;
    cs_res.intra_freq_meas                               = false;
    cs_res.measurements                                  = res.measurements;
    stack.cell_search_found_cell(cs_res);
  }

  // Advance stack TTI
  stack.run_tti(0);

  return true;
}

} // namespace nr
} // namespace srsue