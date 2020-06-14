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

/******************************************************************************
 * File:        radio_interfaces.h
 * Description: Common interface for eNB/UE for PHY and radio
 *****************************************************************************/

#ifndef SRSLTE_RADIO_INTERFACES_H
#define SRSLTE_RADIO_INTERFACES_H

#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/common/timestamp.h"
#include "srslte/phy/rf/rf.h"
#include "srslte/phy/utils/vector.h"
#include <array>

namespace srslte {

/**
 * Class used to pass buffers for all channels and antennas to the radio
 *
 * The class provides an abstraction to map carriers and antennas to the underlying
 * RF channels. It provides getters and setters to access the raw pointers where the signal
 * is stored.
 *
 * It can automatically allocate and deallocate memory for the buffers if
 * a non-zero number of subframes is passed to the constructor.
 *
 */
class rf_buffer_interface
{
public:
  virtual cf_t*    get(const uint32_t& channel_idx) const                                                         = 0;
  virtual void     set(const uint32_t& channel_idx, cf_t* ptr)                                                    = 0;
  virtual cf_t*    get(const uint32_t& cc_idx, const uint32_t& port_idx, const uint32_t& nof_antennas) const      = 0;
  virtual void     set(const uint32_t& cc_idx, const uint32_t& port_idx, const uint32_t& nof_antennas, cf_t* ptr) = 0;
  virtual void**   to_void()                                                                                      = 0;
  virtual cf_t**   to_cf_t()                                                                                      = 0;
  virtual uint32_t size()                                                                                         = 0;
  virtual void     set_nof_samples(uint32_t n)                                                                    = 0;
  virtual uint32_t get_nof_samples() const                                                                        = 0;
};

/**
 * Class used to pass RF devices timestamps to the radio instance
 *
 * The class provides an abstraction to store a number of timestamps underlying RF devices.
 */
class rf_timestamp_interface
{
public:
  virtual const srslte_timestamp_t& get(uint32_t idx) const = 0;
  virtual srslte_timestamp_t*       get_ptr(uint32_t idx)   = 0;
  virtual void                      add(double secs)        = 0;
  virtual void                      sub(double secs)        = 0;

  void copy(const rf_timestamp_interface& other)
  {
    // Nothing to copy
    if (this == &other) {
      return;
    }

    // Copy timestamps
    for (uint32_t i = 0; i < SRSLTE_MAX_CHANNELS; i++) {
      *this->get_ptr(i) = other.get(i);
    }
  }

  srslte_timestamp_t& operator[](uint32_t idx) { return *this->get_ptr(idx); }
};

/**
 * Radio interface for the PHY.
 *
 * The main functionality is to allow TX and RX of samples to the radio.
 * It also provides functions to change the radio settings such as carrier frequency,
 * sampling rate, gains, etc.
 *
 * The radio interface supports multiple carriers and multiple antennas per carrier. This interface presents an
 * abstract access to carries and ports to the PHY, regardless of the mapping to RF channels in the underlying radio.
 * number of carriers <= SRSLTE_MAX_CARRIERS
 * number of ports <= SRSLTE_MAX_PORTS
 *
 * Changing the tx/rx frequency is done on a carrier level and the underlying implementation
 * will set it for all ports when necessary.
 *
 * Samples are passed and received to/from the radio using rf_buffer_t object
 * @see rf_buffer_t
 * @see radio
 */
class radio_interface_phy
{
public:
  /**
   * Indicates the end of a burst in the current TX stream. Usually called after a call to tx() if
   * no more samples are needed to be transmitted.
   */
  virtual void tx_end() = 0;
  /**
   * Indicates the radio to transmit on all antennas and carriers synchronously the samples contained in
   * the buffer object.
   *
   * @param buffer Is the object that contains the pointers to all RF channels
   * @param tx_time Time to transmit all signals
   * @return it returns true if the transmission was successful, otherwise it returns false
   */
  virtual bool tx(rf_buffer_interface& buffer, const rf_timestamp_interface& tx_time) = 0;

  /**
   * Indicates the radio to receive from all antennas and carriers synchronously and store the samples
   * in the buffer object
   *
   * @param buffer Is the object where the samples will be stored
   * @param tx_time Time at which the samples were received. Note the time is the same for all carriers
   * @return
   */
  virtual bool rx_now(rf_buffer_interface& buffer, rf_timestamp_interface& rxd_time) = 0;

  /**
   * Sets the TX frequency for all antennas in the provided carrier index
   * @param carrier_idx Index of the carrier to change the frequency
   * @param freq Frequency to set to in Hz
   */
  virtual void set_tx_freq(const uint32_t& carrier_idx, const double& freq) = 0;

  /**
   * Sets the RX frequency for all antennas in the provided carrier index
   * @param carrier_idx Index of the carrier to change the frequency
   * @param freq Frequency to set to in Hz
   */
  virtual void set_rx_freq(const uint32_t& carrier_idx, const double& freq) = 0;

  /**
   * Releases any mapping between frequency and carrier done when calling set_tx_freq() or set_rx_freq()
   * @param carrier_idx Index of the carrier to release the mapping
   */
  virtual void release_freq(const uint32_t& carrier_idx) = 0;

  /**
   * Sets the transmit gain for all carriers and antennas
   * @param gain Gain in dB
   */
  virtual void set_tx_gain(const float& gain) = 0;

  /**
   * Sets the receive gain for all carriers and antennas in a background
   * @param gain Gain in dB
   */
  virtual void set_rx_gain_th(const float& gain) = 0;

  /**
   * Sets the receive gain for all carriers and antennas
   * @param gain Gain in dB
   */
  virtual void set_rx_gain(const float& gain) = 0;

  /**
   * Sets the sampling rate of the D/A converter simultaneously for all carriers and antennas
   * @param srate Sampling rate in Hz
   */
  virtual void set_tx_srate(const double& srate) = 0;

  /**
   * Sets the sampling rate of the A/D converter simultaneously for all carriers and antennas
   * @param srate Sampling rate in Hz
   */
  virtual void set_rx_srate(const double& srate) = 0;

  /**
   * Sets relative offset between receiver channels. It does not guarantee that the offset is corrected by the radio
   * implementation.
   * @param ch logical channel index
   * @param offset_samples Offset in samples, it can be negative
   */
  virtual void set_channel_rx_offset(uint32_t ch, int32_t offset_samples) = 0;

  // getter
  virtual double            get_freq_offset()       = 0;
  virtual float             get_rx_gain()           = 0;
  virtual bool              is_continuous_tx()      = 0;
  virtual bool              get_is_start_of_burst() = 0;
  virtual bool              is_init()               = 0;
  virtual void              reset()                 = 0;
  virtual srslte_rf_info_t* get_info()              = 0;
};

class phy_interface_radio
{
public:
  virtual void radio_overflow() = 0;
  virtual void radio_failure()  = 0;
};

} // namespace srslte

#endif // SRSLTE_RADIO_INTERFACES_H
