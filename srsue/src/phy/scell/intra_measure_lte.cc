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
#include "srsue/hdr/phy/scell/intra_measure_lte.h"

namespace srsue {
namespace scell {

#define Log(level, fmt, ...)                                                                                           \
  do {                                                                                                                 \
    logger.level("INTRA-%s-%d: " fmt, to_string(get_rat()).c_str(), get_earfcn(), ##__VA_ARGS__);                      \
  } while (false)

intra_measure_lte::intra_measure_lte(srslog::basic_logger& logger_, meas_itf& new_cell_itf_) :
  logger(logger_), scell_rx(logger_), intra_measure_base(logger_, new_cell_itf_)
{}

intra_measure_lte::~intra_measure_lte()
{
  scell_rx.deinit();
  srsran_refsignal_dl_sync_free(&refsignal_dl_sync);
}

void intra_measure_lte::init(uint32_t cc_idx, const args_t& args)
{
  init_generic(cc_idx, args);

  // Initialise Reference signal measurement
  srsran_refsignal_dl_sync_init(&refsignal_dl_sync, SRSRAN_CP_NORM);

  // Start scell
  scell_rx.init(args.len_ms);
}

void intra_measure_lte::set_primary_cell(uint32_t earfcn, srsran_cell_t cell)
{
  {
    std::lock_guard<std::mutex> lock(mutex);
    serving_cell   = cell;
  }
  current_earfcn = earfcn;
  set_current_sf_len((uint32_t)SRSRAN_SF_LEN_PRB(cell.nof_prb));
}

bool intra_measure_lte::measure_rat(const measure_context_t& context, std::vector<cf_t>& buffer, float rx_gain_offset)
{
  std::set<uint32_t> cells_to_measure = context.active_pci;

  srsran_cell_t serving_cell_copy{};
  {
    std::lock_guard<std::mutex> lock(mutex);
    serving_cell_copy = serving_cell;
  }

  // Detect new cells using PSS/SSS
  scell_rx.find_cells(buffer.data(), serving_cell_copy, context.meas_len_ms, cells_to_measure);

  // Initialise empty neighbour cell list
  std::vector<phy_meas_t> neighbour_cells = {};

  context.new_cell_itf.cell_meas_reset(context.cc_idx);

  // Use Cell Reference signal to measure cells in the time domain for all known active PCI
  for (const uint32_t& id : cells_to_measure) {
    // Do not measure serving cell here since it's measured by workers
    if (id == serving_cell_copy.id) {
      continue;
    }
    srsran_cell_t cell = serving_cell_copy;
    cell.id            = id;

    if (srsran_refsignal_dl_sync_set_cell(&refsignal_dl_sync, cell) < SRSRAN_SUCCESS) {
      Log(error, "Error setting refsignal DL cell");
      return false;
    }

    if (srsran_refsignal_dl_sync_run(&refsignal_dl_sync, buffer.data(), context.meas_len_ms * context.sf_len) <
        SRSRAN_SUCCESS) {
      Log(error, "Error running refsignal DL measurements");
      return false;
    }

    if (refsignal_dl_sync.found) {
      phy_meas_t m = {};
      m.rat        = srsran::srsran_rat_t::lte;
      m.pci        = cell.id;
      m.earfcn     = current_earfcn;
      m.rsrp       = refsignal_dl_sync.rsrp_dBfs - rx_gain_offset_db;
      m.rsrq       = refsignal_dl_sync.rsrq_dB;
      m.cfo_hz     = refsignal_dl_sync.cfo_Hz;
      neighbour_cells.push_back(m);

      Log(info,
          "Found neighbour cell: PCI=%03d, RSRP=%5.1f dBm, RSRQ=%5.1f, peak_idx=%5d, "
          "CFO=%+.1fHz",
          m.pci,
          m.rsrp,
          m.rsrq,
          refsignal_dl_sync.peak_index,
          refsignal_dl_sync.cfo_Hz);
    }
  }

  // Send measurements to RRC if any cell found
  if (not neighbour_cells.empty()) {
    context.new_cell_itf.new_cell_meas(context.cc_idx, neighbour_cells);
  }

  return true;
}

} // namespace scell
} // namespace srsue
