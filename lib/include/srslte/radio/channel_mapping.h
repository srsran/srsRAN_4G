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

#ifndef SRSLTE_CHANNEL_MAPPING_H
#define SRSLTE_CHANNEL_MAPPING_H

#include <cinttypes>
#include <list>
#include <map>
#include <mutex>

namespace srslte {

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
    bool contains(float freq) const
    {
      if (low_freq == 0 && high_freq == 0) {
        return true;
      } else {
        return freq >= low_freq && freq <= high_freq;
      }
    }
    float get_low() const { return low_freq; }
    float get_high() const { return high_freq; }

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

  typedef struct {
    uint32_t carrier_idx; // Physical channel index of all channels
    uint32_t device_idx;  // RF Device index
    uint32_t channel_idx; // Channel index in the RF Device
  } device_mapping_t;

  /**
   * Sets the number of the RF device channels and antennas per carrier
   * @param nof_channels_x_dev_ Number of RF channels per device
   * @param nof_antennas_ number of antennas per carrrier
   */
  void set_config(const uint32_t& nof_channels_x_dev_, const uint32_t& nof_antennas_);

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
   * Obtains the physical information configured in set_channels() in the radio to which the logical channel logical_ch
   * has been mapped to
   * @param logical_ch logical channel index
   * @return A device mapping structure carrying the mapping information
   *
   * @see channel_cfg_t
   */
  device_mapping_t get_device_mapping(const uint32_t& logical_ch, const uint32_t& antenna_idx = 0) const;

  /**
   * Checks if the channel has been allocated using allocate_freq()
   *
   * @param logical_ch logical channel index
   * @return true if the channel is allocated, false otherwise
   */
  bool is_allocated(const uint32_t& logical_ch) const;

  /**
   * Represents the channel mapping into a string
   * @return a string representing the current channel mapping
   */
  std::string to_string() const;

private:
  std::list<channel_cfg_t>          available_channels = {};
  std::map<uint32_t, channel_cfg_t> allocated_channels = {};
  mutable std::mutex                mutex              = {};
  uint32_t                          nof_antennas       = 1;
  uint32_t                          nof_channels_x_dev = 1;
};

} // namespace srslte

#endif // SRSLTE_CHANNEL_MAPPING_H
