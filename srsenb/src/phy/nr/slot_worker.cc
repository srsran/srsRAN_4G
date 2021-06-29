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

#include "srsenb/hdr/phy/nr/slot_worker.h"
#include <srsran/common/common.h>

namespace srsenb {
namespace nr {
slot_worker::slot_worker(srsran::phy_common_interface& common_,
                         stack_interface_phy_nr&       stack_,
                         srslog::basic_logger&         logger_) :
  common(common_), stack(stack_), logger(logger_)
{
  // Do nothing
}

bool slot_worker::init(const args_t& args)
{
  // Calculate subframe length
  sf_len = SRSRAN_SF_LEN_PRB_NR(args.carrier.nof_prb);

  // Copy common configurations
  cell_index = args.cell_index;
  pdcch_cfg  = args.pdcch_cfg;

  // Allocate Tx buffers
  tx_buffer.resize(args.nof_tx_ports);
  for (uint32_t i = 0; i < args.nof_tx_ports; i++) {
    tx_buffer[i] = srsran_vec_cf_malloc(sf_len);
    if (tx_buffer[i] == nullptr) {
      logger.error("Error allocating Tx buffer");
      return false;
    }
  }

  // Allocate Tx buffers
  rx_buffer.resize(args.nof_rx_ports);
  for (uint32_t i = 0; i < args.nof_rx_ports; i++) {
    rx_buffer[i] = srsran_vec_cf_malloc(sf_len);
    if (rx_buffer[i] == nullptr) {
      logger.error("Error allocating Rx buffer");
      return false;
    }
  }

  // Prepare DL arguments
  srsran_gnb_dl_args_t dl_args = {};
  dl_args.pdsch.measure_time   = true;
  dl_args.pdsch.max_layers     = args.carrier.max_mimo_layers;
  dl_args.pdsch.max_prb        = args.carrier.nof_prb;
  dl_args.nof_tx_antennas      = args.nof_tx_ports;
  dl_args.nof_max_prb          = args.carrier.nof_prb;

  // Initialise DL
  if (srsran_gnb_dl_init(&gnb_dl, tx_buffer.data(), &dl_args) < SRSRAN_SUCCESS) {
    logger.error("Error gNb DL init");
    return false;
  }

  // Set gNb DL carrier
  if (srsran_gnb_dl_set_carrier(&gnb_dl, &args.carrier) < SRSRAN_SUCCESS) {
    logger.error("Error setting DL carrier");
    return false;
  }

  // Prepare UL arguments
  srsran_gnb_ul_args_t ul_args = {};
  ul_args.pusch.measure_time   = true;
  ul_args.pusch.max_layers     = args.carrier.max_mimo_layers;
  ul_args.pusch.max_prb        = args.carrier.nof_prb;
  ul_args.nof_max_prb          = args.carrier.nof_prb;

  // Initialise UL
  if (srsran_gnb_ul_init(&gnb_ul, rx_buffer[0], &ul_args) < SRSRAN_SUCCESS) {
    logger.error("Error gNb DL init");
    return false;
  }

  // Set gNb UL carrier
  if (srsran_gnb_ul_set_carrier(&gnb_ul, &args.carrier) < SRSRAN_SUCCESS) {
    logger.error("Error setting UL carrier");
    return false;
  }

  return true;
}

slot_worker::~slot_worker()
{
  for (auto& b : tx_buffer) {
    if (b) {
      free(b);
      b = nullptr;
    }
  }
  for (auto& b : rx_buffer) {
    if (b) {
      free(b);
      b = nullptr;
    }
  }
  srsran_gnb_dl_free(&gnb_dl);
}

cf_t* slot_worker::get_buffer_rx(uint32_t antenna_idx)
{
  if (antenna_idx >= (uint32_t)rx_buffer.size()) {
    return nullptr;
  }

  return rx_buffer[antenna_idx];
}

cf_t* slot_worker::get_buffer_tx(uint32_t antenna_idx)
{
  if (antenna_idx >= (uint32_t)tx_buffer.size()) {
    return nullptr;
  }

  return tx_buffer[antenna_idx];
}

uint32_t slot_worker::get_buffer_len()
{
  return sf_len;
}

void slot_worker::set_time(const uint32_t& tti, const srsran::rf_timestamp_t& timestamp)
{
  logger.set_context(tti);
  ul_slot_cfg.idx = tti;
  dl_slot_cfg.idx = TTI_ADD(tti, FDD_HARQ_DELAY_UL_MS);
  tx_time.copy(timestamp);
}

bool slot_worker::work_ul()
{
  stack_interface_phy_nr::ul_sched_t ul_sched = {};
  if (stack.get_ul_sched(ul_slot_cfg, ul_sched) < SRSRAN_SUCCESS) {
    logger.error("Error retrieving UL scheduling");
    return false;
  }

  // Demodulate
  if (srsran_gnb_ul_fft(&gnb_ul) < SRSRAN_SUCCESS) {
    logger.error("Error in demodulation");
    return false;
  }

  // Decode PUCCH
  for (stack_interface_phy_nr::pucch_t& pucch : ul_sched.pucch) {
    srsran_uci_value_nr_t uci_value;
    if (srsran_gnb_ul_get_pucch(&gnb_ul, &ul_slot_cfg, &pucch.pucch_cfg, &pucch.resource, &pucch.uci_cfg, &uci_value) <
        SRSRAN_SUCCESS) {
      logger.error("Error getting PUCCH");
      return false;
    }

    if (logger.info.enabled()) {
      std::array<char, 512> str;
      srsran_uci_data_nr_t  uci_data = {.cfg = pucch.uci_cfg, .value = uci_value};
      srsran_gnb_ul_pucch_info(&gnb_ul, &pucch.resource, &uci_data, str.data(), (uint32_t)str.size());
    }
  }

  // Decode PUSCH
  for (stack_interface_phy_nr::pusch_t& pusch : ul_sched.pusch) {
    // ...
  }

  return true;
}

bool slot_worker::work_dl()
{
  // Retrieve Scheduling for the current processing DL slot
  stack_interface_phy_nr::dl_sched_t dl_sched = {};
  if (stack.get_dl_sched(ul_slot_cfg, dl_sched) < SRSRAN_SUCCESS) {
    logger.error("Error retrieving DL scheduling");
    return false;
  }

  if (srsran_gnb_dl_base_zero(&gnb_dl) < SRSRAN_SUCCESS) {
    logger.error("Error zeroeing RE grid");
    return false;
  }

  // Encode PDCCH for DL transmissions
  for (const stack_interface_phy_nr::pdcch_dl_t& pdcch : dl_sched.pdcch_dl) {
    // Set PDCCH configuration, including DCI dedicated
    if (srsran_gnb_dl_set_pdcch_config(&gnb_dl, &pdcch_cfg, &pdcch.dci_cfg) < SRSRAN_SUCCESS) {
      logger.error("PDCCH: Error setting DL configuration");
      return false;
    }

    // Put PDCCH message
    if (srsran_gnb_dl_pdcch_put_dl(&gnb_dl, &dl_slot_cfg, &pdcch.dci) < SRSRAN_SUCCESS) {
      logger.error("PDCCH: Error putting DL message");
      return false;
    }

    // Log PDCCH information
    if (logger.info.enabled()) {
      std::array<char, 512> str = {};
      srsran_gnb_dl_pdcch_dl_info(&gnb_dl, &pdcch.dci, str.data(), (uint32_t)str.size());
      logger.info("PDCCH: cc=%d %s tti_tx=%d", cell_index, str.data(), dl_slot_cfg.idx);
    }
  }

  // Encode PDCCH for UL transmissions
  for (const stack_interface_phy_nr::pdcch_ul_t& pdcch : dl_sched.pdcch_ul) {
    // Set PDCCH configuration, including DCI dedicated
    if (srsran_gnb_dl_set_pdcch_config(&gnb_dl, &pdcch_cfg, &pdcch.dci_cfg) < SRSRAN_SUCCESS) {
      logger.error("PDCCH: Error setting DL configuration");
      return false;
    }

    // Put PDCCH message
    if (srsran_gnb_dl_pdcch_put_ul(&gnb_dl, &dl_slot_cfg, &pdcch.dci) < SRSRAN_SUCCESS) {
      logger.error("PDCCH: Error putting DL message");
      return false;
    }

    // Log PDCCH information
    if (logger.info.enabled()) {
      std::array<char, 512> str = {};
      srsran_gnb_dl_pdcch_ul_info(&gnb_dl, &pdcch.dci, str.data(), (uint32_t)str.size());
      logger.info("PDCCH: cc=%d %s tti_tx=%d", cell_index, str.data(), dl_slot_cfg.idx);
    }
  }

  // Encode PDSCH
  for (stack_interface_phy_nr::pdsch_t& pdsch : dl_sched.pdsch) {
    // Put PDSCH message
    if (srsran_gnb_dl_pdsch_put(&gnb_dl, &dl_slot_cfg, &pdsch.sch, pdsch.data.data()) < SRSRAN_SUCCESS) {
      logger.error("PDSCH: Error putting DL message");
      return false;
    }

    // Log PDSCH information
    if (logger.info.enabled()) {
      std::array<char, 512> str = {};
      srsran_gnb_dl_pdsch_info(&gnb_dl, &pdsch.sch, str.data(), (uint32_t)str.size());
      logger.info("PDSCH: cc=%d %s tti_tx=%d", cell_index, str.data(), dl_slot_cfg.idx);
    }
  }

  // Put NZP-CSI-RS
  for (srsran_csi_rs_nzp_resource_t& pdsch : dl_sched.nzp_csi_rs) {
    // ...
  }

  // Generate baseband signal
  srsran_gnb_dl_gen_signal(&gnb_dl);

  // Add SSB to the baseband signal
  for (const stack_interface_phy_nr::ssb_t& ssb : dl_sched.ssb) {
    // ...
  }

  return true;
}

void slot_worker::work_imp()
{
  // Inform Scheduler about new slot
  stack.slot_indication(dl_slot_cfg);

  // Get Transmission buffers
  srsran::rf_buffer_t tx_rf_buffer = {};
  for (uint32_t i = 0; i < (uint32_t)tx_buffer.size(); i++) {
    tx_rf_buffer.set(i, tx_buffer[i]);
  }

  // Set number of samples
  tx_rf_buffer.set_nof_samples(sf_len);

  // Process uplink
  if (not work_ul()) {
    common.worker_end(this, false, tx_rf_buffer, tx_time, true);
    return;
  }

  // Process downlink
  if (not work_dl()) {
    common.worker_end(this, false, tx_rf_buffer, tx_time, true);
    return;
  }

  common.worker_end(this, true, tx_rf_buffer, tx_time, true);
}

} // namespace nr
} // namespace srsenb