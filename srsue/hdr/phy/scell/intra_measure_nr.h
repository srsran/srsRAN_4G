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
  struct config_t {
    uint32_t                    arfcn;                                 ///< Carrier frequency in ARFCN
    double                      srate_hz          = 0.0;               ///< Sampling rate in Hz, set to 0.0 for maximum
    uint32_t                    len_ms            = 1;                 ///< Amount of time to accumulate
    uint32_t                    periodicity_ms    = 20;                ///< Accumulation trigger period
    float                       rx_gain_offset_db = 0.0f;              ///< Gain offset, for calibrated measurements
    double                      center_freq_hz    = 0.0;               ///< Base-band center frequency in Hz
    double                      ssb_freq_hz       = 0.0;               ///< SSB center frequency
    srsran_subcarrier_spacing_t scs = srsran_subcarrier_spacing_30kHz; ///< SSB configured Subcarrier spacing
    int                         serving_cell_pci = -1;                 ///< Current serving cell PCI, set to -1 if no
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
   * @param arfcn Frequency the component is receiving base-band from. Used only for reporting the ARFCN to the RRC
   * @param cfg Actual configuration
   * @return True if configuration is successful, false otherwise
   */
  bool set_config(uint32_t arfcn, const config_t& cfg);

  /**
   * @brief Get current frequency number
   * @return the current ARFCN
   */
  uint32_t get_earfcn() const override { return current_arfcn; };

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
   */
  void measure_rat(const measure_context_t& context, std::vector<cf_t>& buffer) override;

  srslog::basic_logger& logger;
  uint32_t              cc_idx           = 0;
  uint32_t              current_arfcn    = 0;
  float                 thr_snr_db       = 5.0f;
  int                   serving_cell_pci = -1;

  /// NR-based measuring objects
  srsran_ssb_t ssb = {}; ///< SS/PBCH Block
};

} // namespace scell
} // namespace srsue

#endif // SRSRAN_INTRA_MEASURE_NR_H
