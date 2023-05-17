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
#ifndef SRSRAN_INTRA_MEASURE_NR_H
#define SRSRAN_INTRA_MEASURE_NR_H

#include "intra_measure_base.h"
#include <srsran/srsran.h>

namespace srsue {
namespace scell {

/**
 * @brief Describes a class for performing LTE intra-frequency cell search and measurement
 */
class intra_measure_nr : public intra_measure_base
{
public:
  /**
   * @brief Describes initialization arguments. It is used to preallocate all memory and avoiding performing memory
   * allocation when the configuration is set
   */
  struct args_t {
    float                       rx_gain_offset_dB = 0.0f;
    uint32_t                    max_len_ms        = 1;
    double                      max_srate_hz      = 61.44e6;
    srsran_subcarrier_spacing_t min_scs           = srsran_subcarrier_spacing_15kHz;
    float                       thr_snr_db        = 5.0f; ///< minimum SNR threshold
  };

  /**
   * @brief Describes the required configuration arguments to start measurements
   */
  struct config_t : public intra_measure_base::args_t {
    /// Additional fields to the base arguments
    uint32_t                    arfcn;                                            ///< Carrier frequency in ARFCN
    double                      center_freq_hz = 0.0;                             ///< Base-band center frequency in Hz
    double                      ssb_freq_hz    = 0.0;                             ///< SSB center frequency
    srsran_subcarrier_spacing_t scs            = srsran_subcarrier_spacing_30kHz; ///< SSB configured Subcarrier spacing
    int                         serving_cell_pci = -1; ///< Current serving cell PCI, set to -1 if no
                                                       ///< serving cell has been configured for this
                                                       ///< carrier
  };

  /**
   * @brief Constructor
   * @param logger Logging object
   * @param new_meas_itf_ Interface to report measurement to higher layers
   */
  intra_measure_nr(srslog::basic_logger& logger, meas_itf& new_meas_itf_);

  /**
   * @brief Destructor
   */
  ~intra_measure_nr() override;

  /**
   * @brief Initialises LTE specific measurement objects
   * @param args Configuration arguments
   * @return True if initialization is successful, false otherwise
   */
  bool init(uint32_t cc_idx, const args_t& args);

  /**
   * @brief Sets the primary cell and selects NR operation mode, configures the cell bandwidth and sampling rate
   * @param cfg Actual configuration
   * @return True if configuration is successful, false otherwise
   */
  bool set_config(const config_t& cfg);

  /**
   * @brief Get current frequency number
   * @return the current ARFCN
   */
  uint32_t get_earfcn() const override { return current_arfcn; };

  /**
   * @brief Computes the average measurement performance since last configuration
   * @return The performance in Millions of samples per second
   */
  uint32_t get_perf() const
  {
    if (perf_count_us == 0) {
      return 0;
    }
    return (uint32_t)(perf_count_samples / perf_count_us);
  };

private:
  /**
   * @brief Provides with the RAT to the base class
   * @return The RAT measured by this class which is NR
   */
  srsran::srsran_rat_t get_rat() const override { return srsran::srsran_rat_t::nr; }

  /**
   * @brief NR specific measurement process
   * @attention It searches and measures the SSB with best SNR
   * @param context Measurement context
   * @param buffer Provides the baseband buffer to perform the measurements
   * @param rx_gain_offset Provides last received rx_gain_offset
   * @return True if no error happen, otherwise false
   */
  bool measure_rat(const measure_context_t& context, std::vector<cf_t>& buffer, float rx_gain_offset) override;

  srslog::basic_logger& logger;
  uint32_t              cc_idx           = 0;
  uint32_t              current_arfcn    = 0;
  float                 thr_snr_db       = 5.0f;
  int                   serving_cell_pci = -1;

  /// Performance
  uint64_t perf_count_us      = 0; ///< Counts execution time in microseconds
  uint64_t perf_count_samples = 0; ///< Counts the number samples

  /// NR-based measuring objects
  srsran_ssb_t ssb = {}; ///< SS/PBCH Block
};

} // namespace scell
} // namespace srsue

#endif // SRSRAN_INTRA_MEASURE_NR_H
