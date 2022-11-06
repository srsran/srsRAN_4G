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
#ifndef SRSRAN_INTRA_MEASURE_LTE_H
#define SRSRAN_INTRA_MEASURE_LTE_H

#include "intra_measure_base.h"
#include "scell_recv.h"
#include <srsran/srsran.h>

namespace srsue {
namespace scell {

/**
 * @brief Describes a class for performing LTE intra-frequency cell search and measurement
 */
class intra_measure_lte : public intra_measure_base
{
public:
  /**
   * @brief Constructor
   * @param logger Logging object
   * @param new_meas_itf_ Interface to report measurement to higher layers
   */
  intra_measure_lte(srslog::basic_logger& logger, meas_itf& new_meas_itf_);

  /**
   * @brief Destructor
   */
  ~intra_measure_lte() override;

  /**
   * @brief Initialises LTE specific measurement objects
   * @param args Configuration arguments
   */
  void init(uint32_t cc_idx, const args_t& args);

  /**
   * @brief Sets the primary cell and selects LTE operation mode, configures the cell bandwidth and sampling rate
   * @param earfcn Frequency the component is receiving base-band from. Used only for reporting the EARFCN to the RRC
   * @param cell Actual cell configuration
   */
  void set_primary_cell(uint32_t earfcn, srsran_cell_t cell);

  /**
   * @brief Get EARFCN of this component
   * @return EARFCN
   */
  uint32_t get_earfcn() const override { return current_earfcn; };

private:
  /**
   * @brief Provides with the RAT to the base class
   * @return The RAT measured by this class which is LTE
   */
  srsran::srsran_rat_t get_rat() const override { return srsran::srsran_rat_t::lte; }

  /**
   * @brief LTE specific measurement process
   * @param context Measurement context
   * @param buffer Provides the baseband buffer to perform the measurements
   * @param rx_gain_offset Provides last received rx_gain_offset
   * @return True if no error happens, otherwise false
   */
  bool measure_rat(const measure_context_t& context, std::vector<cf_t>& buffer, float rx_gain_offset) override;

  srslog::basic_logger& logger;
  srsran_cell_t         serving_cell   = {};  ///< Current serving cell in the EARFCN, to avoid reporting it
  std::atomic<uint32_t> current_earfcn = {0}; ///< Current EARFCN
  std::mutex            mutex;

  /// LTE-based measuring objects
  scell_recv                 scell_rx;               ///< Secondary cell searcher
  srsran_refsignal_dl_sync_t refsignal_dl_sync = {}; ///< Reference signal based measurement
};

} // namespace scell
} // namespace srsue

#endif // SRSRAN_INTRA_MEASURE_LTE_H
