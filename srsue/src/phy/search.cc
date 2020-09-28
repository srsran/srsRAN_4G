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

#include "srsue/hdr/phy/search.h"

#define Error(fmt, ...)                                                                                                \
  if (SRSLTE_DEBUG_ENABLED)                                                                                            \
  log_h->error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...)                                                                                              \
  if (SRSLTE_DEBUG_ENABLED)                                                                                            \
  log_h->warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...)                                                                                                 \
  if (SRSLTE_DEBUG_ENABLED)                                                                                            \
  log_h->info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)                                                                                                \
  if (SRSLTE_DEBUG_ENABLED)                                                                                            \
  log_h->debug(fmt, ##__VA_ARGS__)

namespace srsue {

static int
radio_recv_callback(void* obj, cf_t* data[SRSLTE_MAX_CHANNELS], uint32_t nsamples, srslte_timestamp_t* rx_time)
{
  srslte::rf_buffer_t x(data, nsamples);
  return ((search_callback*)obj)->radio_recv_fnc(x, rx_time);
}

static SRSLTE_AGC_CALLBACK(callback_set_rx_gain)
{
  ((search_callback*)h)->set_rx_gain(gain_db);
}

search::~search()
{
  srslte_ue_mib_sync_free(&ue_mib_sync);
  srslte_ue_cellsearch_free(&cs);
}

void search::init(srslte::rf_buffer_t& buffer_, srslte::log* log_h_, uint32_t nof_rx_channels, search_callback* parent)
{
  log_h = log_h_;
  p     = parent;

  buffer = buffer_;

  if (srslte_ue_cellsearch_init_multi(&cs, 8, radio_recv_callback, nof_rx_channels, parent)) {
    Error("SYNC:  Initiating UE cell search\n");
  }
  srslte_ue_cellsearch_set_nof_valid_frames(&cs, 4);

  if (srslte_ue_mib_sync_init_multi(&ue_mib_sync, radio_recv_callback, nof_rx_channels, parent)) {
    Error("SYNC:  Initiating UE MIB synchronization\n");
  }

  // Set options defined in expert section
  p->set_ue_sync_opts(&cs.ue_sync, 0);

  force_N_id_2 = -1;
}

void search::reset()
{
  srslte_ue_sync_reset(&ue_mib_sync.ue_sync);
}

float search::get_last_cfo()
{
  return srslte_ue_sync_get_cfo(&ue_mib_sync.ue_sync);
}

void search::set_agc_enable(bool enable)
{
  if (enable) {
    srslte_rf_info_t* rf_info = p->get_radio()->get_info();
    srslte_ue_sync_start_agc(&ue_mib_sync.ue_sync,
                             callback_set_rx_gain,
                             rf_info->min_rx_gain,
                             rf_info->max_rx_gain,
                             p->get_radio()->get_rx_gain());
  } else {
    ERROR("Error stop AGC not implemented\n");
  }
}

search::ret_code search::run(srslte_cell_t* cell_, std::array<uint8_t, SRSLTE_BCH_PAYLOAD_LEN>& bch_payload)
{
  srslte_cell_t new_cell = {};

  srslte_ue_cellsearch_result_t found_cells[3];

  bzero(found_cells, 3 * sizeof(srslte_ue_cellsearch_result_t));

  /* Find a cell in the given N_id_2 or go through the 3 of them to find the strongest */
  uint32_t max_peak_cell = 0;
  int      ret           = SRSLTE_ERROR;

  Info("SYNC:  Searching for cell...\n");
  srslte::console(".");

  if (force_N_id_2 >= 0 && force_N_id_2 < 3) {
    ret           = srslte_ue_cellsearch_scan_N_id_2(&cs, force_N_id_2, &found_cells[force_N_id_2]);
    max_peak_cell = force_N_id_2;
  } else {
    ret = srslte_ue_cellsearch_scan(&cs, found_cells, &max_peak_cell);
  }

  if (ret < 0) {
    Error("SYNC:  Error decoding MIB: Error searching PSS\n");
    return ERROR;
  } else if (ret == 0) {
    Info("SYNC:  Could not find any cell in this frequency\n");
    return CELL_NOT_FOUND;
  }
  // Save result
  new_cell.id         = found_cells[max_peak_cell].cell_id;
  new_cell.cp         = found_cells[max_peak_cell].cp;
  new_cell.frame_type = found_cells[max_peak_cell].frame_type;
  float cfo           = found_cells[max_peak_cell].cfo;

  srslte::console("\n");
  Info("SYNC:  PSS/SSS detected: Mode=%s, PCI=%d, CFO=%.1f KHz, CP=%s\n",
       new_cell.frame_type ? "TDD" : "FDD",
       new_cell.id,
       cfo / 1000,
       srslte_cp_string(new_cell.cp));

  if (srslte_ue_mib_sync_set_cell(&ue_mib_sync, new_cell)) {
    Error("SYNC:  Setting UE MIB cell\n");
    return ERROR;
  }

  // Set options defined in expert section
  p->set_ue_sync_opts(&ue_mib_sync.ue_sync, cfo);

  srslte_ue_sync_reset(&ue_mib_sync.ue_sync);

  /* Find and decode MIB */
  int sfn_offset;
  ret = srslte_ue_mib_sync_decode(&ue_mib_sync, 40, bch_payload.data(), &new_cell.nof_ports, &sfn_offset);
  if (ret == 1) {
    srslte_pbch_mib_unpack(bch_payload.data(), &new_cell, NULL);
    // pack MIB and store inplace for PCAP dump
    std::array<uint8_t, SRSLTE_BCH_PAYLOAD_LEN / 8> mib_packed;
    srslte_bit_pack_vector(bch_payload.data(), mib_packed.data(), SRSLTE_BCH_PAYLOAD_LEN);
    std::copy(std::begin(mib_packed), std::end(mib_packed), std::begin(bch_payload));

    fprintf(stdout,
            "Found Cell:  Mode=%s, PCI=%d, PRB=%d, Ports=%d, CFO=%.1f KHz\n",
            new_cell.frame_type ? "TDD" : "FDD",
            new_cell.id,
            new_cell.nof_prb,
            new_cell.nof_ports,
            cfo / 1000);

    Info("SYNC:  MIB Decoded: Mode=%s, PCI=%d, PRB=%d, Ports=%d, CFO=%.1f KHz\n",
         new_cell.frame_type ? "TDD" : "FDD",
         new_cell.id,
         new_cell.nof_prb,
         new_cell.nof_ports,
         cfo / 1000);

    if (!srslte_cell_isvalid(&new_cell)) {
      Error("SYNC:  Detected invalid cell.\n");
      return CELL_NOT_FOUND;
    }

    // Save cell pointer
    if (cell_) {
      *cell_ = new_cell;
    }

    return CELL_FOUND;
  } else if (ret == 0) {
    Warning("SYNC:  Found PSS but could not decode PBCH\n");
    return CELL_NOT_FOUND;
  } else {
    Error("SYNC:  Receiving MIB\n");
    return ERROR;
  }
}

}; // namespace srsue