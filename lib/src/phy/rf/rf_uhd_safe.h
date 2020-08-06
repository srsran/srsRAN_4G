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
#ifndef SRSLTE_RF_UHD_SAFE_H
#define SRSLTE_RF_UHD_SAFE_H

#include <set>
#include <uhd/utils/log.hpp>

#ifdef UHD_LOG_INFO
#define Warning(message) UHD_LOG_WARNING("UHD RF", message)
#define Info(message) UHD_LOG_INFO("UHD RF", message)
#define Debug(message) UHD_LOG_DEBUG("UHD RF", message)
#define Trace(message) UHD_LOG_TRACE("UHD RF", message)
#else
#define Warning(message) UHD_LOG << message << std::endl
#define Info(message) UHD_LOG << message << std::endl
#define Debug(message) UHD_LOG << message << std::endl
#define Trace(message) UHD_LOG << message << std::endl
#endif

#ifdef ENABLE_UHD_X300_FW_RESET
#include <uhd/transport/udp_simple.hpp>

uhd::wb_iface::sptr x300_make_ctrl_iface_enet(uhd::transport::udp_simple::sptr udp, bool enable_errors);
#endif /* ENABLE_UHD_X300_FW_RESET */

class rf_uhd_safe_interface
{
private:
#ifdef ENABLE_UHD_X300_FW_RESET
  const std::chrono::milliseconds X300_SLEEP_TIME_MS = std::chrono::milliseconds(5000UL);
#endif /* ENABLE_UHD_X300_FW_RESET */

  virtual uhd_error usrp_make_internal(const uhd::device_addr_t& dev_addr) = 0;

#ifdef ENABLE_UHD_X300_FW_RESET
  uhd_error try_usrp_x300_reset(const uhd::device_addr_t& dev_addr)
  {
    UHD_SAFE_C_SAVE_ERROR(
        this,
        // It is not possible to reset device if IP address is not provided
        if (not dev_addr.has_key("addr")) { return UHD_ERROR_NONE; }

        Warning("Reseting X300 in address " << dev_addr["addr"]);

        { // Reset Scope
          // Create UDP connection
          uhd::transport::udp_simple::sptr udp_simple =
              uhd::transport::udp_simple::make_connected(dev_addr["addr"], "49152");

          // Create X300 control
          uhd::wb_iface::sptr x300_ctrl = x300_make_ctrl_iface_enet(udp_simple, true);

          // Reset FPGA firmware
          x300_ctrl->poke32(0x100058, 1);

          Info("Reset Done!");
          x300_ctrl  = nullptr;
          udp_simple = nullptr;
        }

        return UHD_ERROR_NONE;)
  }
#endif /* ENABLE_UHD_X300_FW_RESET */

protected:
  // List of errors that can happen in the USRP make that need to restart the device
  const std::set<uhd_error> USRP_MAKE_RESET_ERR = {UHD_ERROR_IO};

  // UHD pointers
  uhd::rx_streamer::sptr rx_stream = nullptr;
  uhd::tx_streamer::sptr tx_stream = nullptr;

  uhd_error usrp_multi_make(const uhd::device_addr_t& dev_addr)
  {
    uhd_error err = usrp_make_internal(dev_addr);

#ifdef ENABLE_UHD_X300_FW_RESET
    // Looks up error in reset table
    if (USRP_MAKE_RESET_ERR.count(err) == 0 or not dev_addr.has_key("addr")) {
      // If no error in table or not specified type, returns
      return err;
    }

    // Reset device
    err = try_usrp_x300_reset(dev_addr);
    if (err != UHD_ERROR_NONE) {
      return err;
    }

    // Sleep for some time
    Info("Wait " << X300_SLEEP_TIME_MS.count() << " ms");
    std::this_thread::sleep_for(X300_SLEEP_TIME_MS);

    // Try opening the device one more time
    return usrp_make_internal(dev_addr);
#else  /* ENABLE_UHD_X300_FW_RESET */
    return err;
#endif /* ENABLE_UHD_X300_FW_RESET */
  }

public:
  std::string last_error;

  virtual uhd_error usrp_make(const uhd::device_addr_t& dev_addr, uint32_t nof_channels)             = 0;
  virtual uhd_error get_mboard_name(std::string& mboard_name)                                        = 0;
  virtual uhd_error get_mboard_sensor_names(std::vector<std::string>& sensors)                       = 0;
  virtual uhd_error get_rx_sensor_names(std::vector<std::string>& sensors)                           = 0;
  virtual uhd_error get_sensor(const std::string& sensor_name, double& sensor_value)                 = 0;
  virtual uhd_error get_sensor(const std::string& sensor_name, bool& sensor_value)                   = 0;
  virtual uhd_error get_rx_sensor(const std::string& sensor_name, bool& sensor_value)                = 0;
  virtual uhd_error set_time_unknown_pps(const uhd::time_spec_t& timespec)                           = 0;
  virtual uhd_error get_time_now(uhd::time_spec_t& timespec)                                         = 0;
  uhd_error         start_rx_stream(double delay)
  {
    uhd::time_spec_t time_spec;
    uhd_error        err = get_time_now(time_spec);
    if (err != UHD_ERROR_NONE) {
      return err;
    }

    UHD_SAFE_C_SAVE_ERROR(this, uhd::stream_cmd_t stream_cmd(uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS);
                          stream_cmd.time_spec = time_spec;
                          stream_cmd.time_spec += delay;
                          stream_cmd.stream_now = not std::isnormal(delay);

                          rx_stream->issue_stream_cmd(stream_cmd);)
  }
  uhd_error stop_rx_stream()
  {
    UHD_SAFE_C_SAVE_ERROR(this, uhd::stream_cmd_t stream_cmd(uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS);
                          rx_stream->issue_stream_cmd(stream_cmd);)
  }
  virtual uhd_error set_sync_source(const std::string& source)                                         = 0;
  virtual uhd_error get_gain_range(uhd::gain_range_t& tx_gain_range, uhd::gain_range_t& rx_gain_range) = 0;
  virtual uhd_error set_master_clock_rate(double rate)                                                 = 0;
  virtual uhd_error set_rx_rate(double rate)                                                           = 0;
  virtual uhd_error set_tx_rate(double rate)                                                           = 0;
  virtual uhd_error set_command_time(const uhd::time_spec_t& timespec)                                 = 0;
  virtual uhd_error get_rx_stream(size_t& max_num_samps)                                               = 0;
  virtual uhd_error destroy_rx_stream() { UHD_SAFE_C_SAVE_ERROR(this, rx_stream = nullptr;) }
  virtual uhd_error get_tx_stream(size_t& max_num_samps) = 0;
  virtual uhd_error destroy_tx_stream() { UHD_SAFE_C_SAVE_ERROR(this, rx_stream = nullptr;) }
  virtual uhd_error set_tx_gain(size_t ch, double gain)                               = 0;
  virtual uhd_error set_rx_gain(size_t ch, double gain)                               = 0;
  virtual uhd_error get_rx_gain(double& gain)                                         = 0;
  virtual uhd_error get_tx_gain(double& gain)                                         = 0;
  virtual uhd_error set_tx_freq(uint32_t ch, double target_freq, double& actual_freq) = 0;
  virtual uhd_error set_rx_freq(uint32_t ch, double target_freq, double& actual_freq) = 0;
  uhd_error         receive(void**              buffs,
                            const size_t        nsamps_per_buff,
                            uhd::rx_metadata_t& metadata,
                            const double        timeout,
                            const bool          one_packet,
                            size_t&             nof_rxd_samples)
  {
    UHD_SAFE_C_SAVE_ERROR(this, uhd::rx_streamer::buffs_type buffs_cpp(buffs, rx_stream->get_num_channels());
                          nof_rxd_samples = rx_stream->recv(buffs_cpp, nsamps_per_buff, metadata, timeout, one_packet);)
  }
  virtual uhd_error recv_async_msg(uhd::async_metadata_t& async_metadata, double timeout, bool& valid)
  {
    UHD_SAFE_C_SAVE_ERROR(this, valid = tx_stream->recv_async_msg(async_metadata, timeout);)
  }
  uhd_error send(void**                    buffs,
                 const size_t              nsamps_per_buff,
                 const uhd::tx_metadata_t& metadata,
                 const double              timeout,
                 size_t&                   nof_txd_samples)
  {
    UHD_SAFE_C_SAVE_ERROR(this, uhd::tx_streamer::buffs_type buffs_cpp(buffs, tx_stream->get_num_channels());
                          nof_txd_samples = tx_stream->send(buffs_cpp, nsamps_per_buff, metadata, timeout);)
  }
  virtual bool is_rx_ready() { return rx_stream != nullptr; }
  virtual bool is_tx_ready() { return tx_stream != nullptr; }
};

#endif // SRSLTE_RF_UHD_SAFE_H
