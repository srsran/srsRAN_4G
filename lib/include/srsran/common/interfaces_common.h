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

#ifndef SRSRAN_INTERFACES_COMMON_H
#define SRSRAN_INTERFACES_COMMON_H

#include "srsran/common/byte_buffer.h"
#include "srsran/common/security.h"
#include "srsran/phy/common/phy_common.h"
#include <string>

namespace srsran {

struct phy_log_args_t {
  std::string phy_level     = "none";
  std::string phy_lib_level = "none";
  std::string id_preamble   = "";
  int         phy_hex_limit = -1;
};

struct rf_args_band_t {
  float min;
  float max;
};

// RF/radio args
struct rf_args_t {
  std::string type;
  std::string log_level;
  double      srate_hz;
  float       dl_freq;
  float       ul_freq;
  float       freq_offset;
  float       rx_gain;
  float       rx_gain_ch[SRSRAN_MAX_CARRIERS];
  float       tx_gain;
  float       tx_gain_ch[SRSRAN_MAX_CARRIERS];
  float       tx_max_power;
  float       tx_gain_offset;
  float       rx_gain_offset;
  uint32_t    nof_carriers; // Number of RF channels
  uint32_t    nof_antennas; // Number of antennas per RF channel
  std::string device_name;
  std::string device_args;
  std::string time_adv_nsamples;
  std::string continuous_tx;

  std::array<rf_args_band_t, SRSRAN_MAX_CARRIERS> ch_rx_bands;
  std::array<rf_args_band_t, SRSRAN_MAX_CARRIERS> ch_tx_bands;

  FILE** rx_files;  // Array of pre-opened FILE* for rx instead of a real device
  FILE** tx_files;  // Array of pre-opened FILE* for tx instead of a real device
};

class srsran_gw_config_t
{
public:
  explicit srsran_gw_config_t(uint32_t lcid_ = 0) : lcid(lcid_) {}

  uint32_t lcid;
};

class read_pdu_interface
{
public:
  virtual uint32_t read_pdu(uint32_t lcid, uint8_t* payload, uint32_t requested_bytes) = 0;
};


} // namespace srsran

#endif // SRSRAN_INTERFACES_COMMON_H
