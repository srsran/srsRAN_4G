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

#include <string.h>

#include "radio_metrics.h"
#include "srslte/common/interfaces_common.h"
#include "srslte/common/log_filter.h"
#include "srslte/common/trace.h"
#include "srslte/interfaces/radio_interfaces.h"
#include "srslte/phy/rf/rf.h"
#include "srslte/srslte.h"
#include <list>

#ifndef SRSLTE_RADIO_H
#define SRSLTE_RADIO_H

namespace srslte {

/**
 * Implemenation of the rf_buffer_interface for the current radio implementation which uses flat arrays.
 * @see rf_buffer_interface
 * @see radio
 *
 */
class rf_buffer_t : public rf_buffer_interface
{
public:
  /**
   * Creates an object and allocates memory for nof_subframes_ assuming the
   * largest system bandwidth
   * @param nof_subframes_ Number of subframes to allocate
   */
  explicit rf_buffer_t(uint32_t nof_subframes_)
  {
    if (nof_subframes_ > 0) {
      // Allocate buffers for an integer number of subframes
      for (uint32_t i = 0; i < SRSLTE_MAX_CHANNELS; i++) {
        sample_buffer[i] = srslte_vec_cf_malloc(nof_subframes_ * SRSLTE_SF_LEN_MAX);
        srslte_vec_cf_zero(sample_buffer[i], SRSLTE_SF_LEN_MAX);
      }
      allocated     = true;
      nof_subframes = nof_subframes_;
    }
  }
  /**
   * Creates an object and sets the buffers to the flat array pointed by data. Note that data must
   * contain up to SRSLTE_MAX_CHANNELS pointers
   * @param data Flat array to use as initializer for the internal buffer pointers
   */
  explicit rf_buffer_t(cf_t* data[SRSLTE_MAX_CHANNELS])
  {
    for (uint32_t i = 0; i < SRSLTE_MAX_CHANNELS; i++) {
      sample_buffer[i] = data[i];
    }
  }
  /**
   * Creates an object from a single array pointer. The rest of the channel pointers will be left to NULL
   * @param data Flat array to use as initializer for the internal buffer pointers
   */
  explicit rf_buffer_t(cf_t* data) { sample_buffer[0] = data; }
  /**
   * Default constructor leaves the internal pointers to NULL
   */
  rf_buffer_t() = default;

  /**
   * The destructor will deallocate memory only if it was allocated passing nof_subframes > 0
   */
  ~rf_buffer_t()
  {
    if (allocated) {
      free_all();
    }
  }
  /**
   * Overrides the = operator such that the lvalue internal buffers point to the pointers inside rvalue.
   * If memory has already been allocated in the lvalue object, it will free it before pointing the
   * buffers to the lvalue.
   * After this operator, when the lvalue is destroyed no memory will be freed.
   * @param other rvalue
   * @return lvalue
   */
  rf_buffer_t& operator=(const rf_buffer_t& other)
  {
    if (this == &other) {
      return *this;
    }
    if (this->allocated) {
      free_all();
      this->allocated = false;
    }
    for (int i = 0; i < SRSLTE_MAX_CHANNELS; i++) {
      this->sample_buffer[i] = other.sample_buffer[i];
    }
    return *this;
  }

  rf_buffer_t(const rf_buffer_t& other) = delete;
  cf_t* get(const uint32_t& channel_idx) const override { return sample_buffer.at(channel_idx); }
  void  set(const uint32_t& channel_idx, cf_t* ptr) override { sample_buffer.at(channel_idx) = ptr; }
  cf_t* get(const uint32_t& logical_ch, const uint32_t& port_idx, const uint32_t& nof_antennas) const override
  {
    return sample_buffer.at(logical_ch * nof_antennas + port_idx);
  }
  void set(const uint32_t& logical_ch, const uint32_t& port_idx, const uint32_t& nof_antennas, cf_t* ptr) override
  {
    sample_buffer.at(logical_ch * nof_antennas + port_idx) = ptr;
  }
  void**   to_void() override { return (void**)sample_buffer.data(); }
  cf_t**   to_cf_t() override { return sample_buffer.data(); }
  uint32_t size() override { return nof_subframes * SRSLTE_SF_LEN_MAX; }

private:
  std::array<cf_t*, SRSLTE_MAX_CHANNELS> sample_buffer = {};
  bool                                   allocated     = false;
  uint32_t                               nof_subframes = 0;
  void                                   free_all()
  {
    for (uint32_t i = 0; i < SRSLTE_MAX_CHANNELS; i++) {
      if (sample_buffer[i]) {
        free(sample_buffer[i]);
      }
    }
  }
};

/**
 * Implementation of the radio interface for the PHY
 *
 * It uses the rf C library object to access the underlying radio. This implementation uses a flat array to
 * transmit/receive samples for all RF channels. The N carriers and P antennas are mapped into M=NP RF channels (M <=
 * SRSLTE_MAX_CHANNELS). Note that all carriers must have the same number of antennas.
 *
 * The underlying radio receives and transmits M RF channels synchronously from possibly multiple radios using the same
 * rf driver object. In the current implementation, the mapping between N carriers and P antennas is sequentially, eg:
 * [carrier_0_port_0, carrier_0_port_1, carrier_1_port_0, carrier_1_port_1, ..., carrier_N_port_N]
 */
class radio : public radio_interface_phy
{
public:
  radio(srslte::log_filter* log_h);
  radio(srslte::logger* logger_h);
  virtual ~radio();

  int  init(const rf_args_t& args_, phy_interface_radio* phy_);
  void stop();

  // ==== PHY interface ===

  // trx functions
  void tx_end() override;
  bool tx(rf_buffer_interface& buffer, const uint32_t& nof_samples, const srslte_timestamp_t& tx_time) override;
  bool rx_now(rf_buffer_interface& buffer, const uint32_t& nof_samples, srslte_timestamp_t* rxd_time) override;

  // setter
  void set_tx_freq(const uint32_t& carrier_idx, const double& freq) override;
  void set_rx_freq(const uint32_t& carrier_idx, const double& freq) override;
  void release_freq(const uint32_t& carrier_idx) override;

  void set_tx_gain(const float& gain) override;
  void set_rx_gain_th(const float& gain) override;
  void set_rx_gain(const float& gain) override;
  void set_tx_srate(const double& srate) override;
  void set_rx_srate(const double& srate) override;

  // getter
  double            get_freq_offset() override;
  float             get_rx_gain() override;
  bool              is_continuous_tx() override;
  bool              get_is_start_of_burst() override;
  bool              is_init() override;
  void              reset() override;
  srslte_rf_info_t* get_info() override;

  // Other functions
  bool  get_metrics(rf_metrics_t* metrics);
  float get_rssi();
  bool  has_rssi();
  void  get_time(srslte_timestamp_t* now);

  void        handle_rf_msg(srslte_rf_error_t error);
  static void rf_msg_callback(void* arg, srslte_rf_error_t error);

private:
  srslte_rf_t          rf_device  = {};
  srslte_rf_info_t     rf_info    = {};
  rf_metrics_t         rf_metrics = {};
  log_filter           log_local  = {};
  log_filter*          log_h      = nullptr;
  srslte::logger*      logger     = nullptr;
  phy_interface_radio* phy        = nullptr;
  cf_t*                zeros      = nullptr;

  srslte_timestamp_t end_of_burst_time  = {};
  bool               is_start_of_burst  = 0;
  uint32_t           tx_adv_nsamples    = 0;
  double             tx_adv_sec         = 0.0f; // Transmission time advance to compensate for antenna->timestamp delay
  bool               tx_adv_auto        = false;
  bool               tx_adv_negative    = false;
  bool               is_initialized     = false;
  bool               radio_is_streaming = false;
  bool               continuous_tx      = false;
  double             freq_offset        = 0.0f;
  double             cur_tx_srate       = 0.0f;
  uint32_t           nof_antennas       = 0;
  uint32_t           nof_channels       = 0;
  uint32_t           nof_carriers       = 0;

  std::vector<double> cur_tx_freqs = {};
  std::vector<double> cur_rx_freqs = {};

  constexpr static double tx_max_gap_zeros = 4e-3; ///< Maximum transmission gap to fill with zeros, otherwise the burst
                                                   ///< shall be stopped

  // Define default values for known radios
  constexpr static double uhd_default_tx_adv_samples    = 98;
  constexpr static double uhd_default_tx_adv_offset_sec = 4 * 1e-6;

  constexpr static int    blade_default_tx_adv_samples    = 27;
  constexpr static double blade_default_tx_adv_offset_sec = 1e-6;

  /**
   * This class manages the mapping between logical and physical channels.
   * A physical channel in this class is a carrier index in the radio class, which
   * has multiple antenna ports all tuned to the same frequency.
   *
   * Every group of channels tuned associated with a carrier go through the same band-pass filter. This
   * class then manages the allocation of frequencies to these group of channels.
   *
   * The same object is reused for the reception and transmission.
   *
   * When the UE wants to tune a logical channel to a new frequency it requests this class
   * to provide an available channel that supports this frequency. At that point,
   * that channel can not be used anymore until a call to release().
   *
   */
  class channel_mapping
  {
  public:
    /** Configures a band. A band is defined by an upper and lower frequency boundaries.
     * If the upper and lower frequencies are not configured (default is zero), it means
     * that they support any frequency
     */
    class band_cfg
    {
    public:
      void set(float low_freq_, float high_freq_)
      {
        low_freq  = low_freq_;
        high_freq = high_freq_;
      }
      bool contains(float freq)
      {
        if (low_freq == 0 && high_freq == 0) {
          return true;
        } else {
          return freq >= low_freq && freq <= high_freq;
        }
      }
      float get_low() { return low_freq; }
      float get_high() { return high_freq; }

    private:
      float low_freq  = 0;
      float high_freq = 0;
    };

    /** Each channel is defined by the band it supports and the physical carrier index in the radio
     */
    typedef struct {
      band_cfg band;
      uint32_t carrier_idx;
    } channel_cfg_t;

    /**
     * Sets the channel configuration. If no channels are configured no physical channels can be allocated
     * @param channels_
     */
    void set_channels(const std::list<channel_cfg_t>& channels_) { available_channels = channels_; }

    /**
     * Finds an unused physical channel that supports the provided frequency and assigns it to logical channel
     * logical_ch
     * @param logical_ch logical channel index
     * @param freq Frequency (in Hz) that we want to receive/transmitt
     * @return true if a physical channel supporting this frequency was found or false otherwise
     */
    bool allocate_freq(const uint32_t& logical_ch, const float& freq);

    /**
     * Releases the allocation of a logical channel allowing to be used in the next call to allocate_freq
     * @param logical_ch logical channel index
     * @return false if logical_ch is not allocated, true otherwise
     */
    bool release_freq(const uint32_t& logical_ch);

    /**
     * Obtains the carrier index configured in set_channels() in the radio to which the logical channel logical_ch has
     * been mapped to
     * @param logical_ch logical channel index
     * @return <0 if logical_ch is not allocated, true otherwise
     *
     * @see channel_cfg_t
     */
    int get_carrier_idx(const uint32_t& logical_ch);

    /**
     * Checks if the channel has been allocated using allocate_freq()
     *
     * @param logical_ch logical channel index
     * @return true if the channel is allocated, false otherwise
     */
    bool is_allocated(const uint32_t& logical_ch);

  private:
    std::list<channel_cfg_t>          available_channels = {};
    std::map<uint32_t, channel_cfg_t> allocated_channels = {};
    std::mutex                        mutex              = {};
  };

  channel_mapping rx_channel_mapping = {}, tx_channel_mapping = {};

  bool map_channels(channel_mapping&           map,
                    uint32_t                   sample_offset,
                    const rf_buffer_interface& buffer,
                    void*                      radio_buffers[SRSLTE_MAX_CHANNELS]);
  bool start_agc(bool tx_gain_same_rx = false);
  void set_tx_adv(int nsamples);
  void set_tx_adv_neg(bool tx_adv_is_neg);
  bool config_rf_channels(const rf_args_t& args);
};

} // namespace srslte

#endif // SRSLTE_RADIO_H
