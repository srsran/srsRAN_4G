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

#ifndef SRSUE_SEARCH_H
#define SRSUE_SEARCH_H

#include "srsran/radio/radio.h"
#include "srsran/srslog/srslog.h"
#include "srsran/srsran.h"

namespace srsue {

class search_callback
{
public:
  virtual int                          radio_recv_fnc(srsran::rf_buffer_t&, srsran_timestamp_t* rx_time) = 0;
  virtual void                         set_ue_sync_opts(srsran_ue_sync_t* q, float cfo)                  = 0;
  virtual srsran::radio_interface_phy* get_radio()                                                       = 0;
  virtual void                         set_rx_gain(float gain)                                           = 0;
};

// Class to run cell search
class search
{
public:
  typedef enum { CELL_NOT_FOUND, CELL_FOUND, ERROR, TIMEOUT } ret_code;

  explicit search(srslog::basic_logger& logger) : logger(logger) {}
  ~search();
  void     init(srsran::rf_buffer_t& buffer_, uint32_t nof_rx_channels, search_callback* parent, int force_N_id_2_, int force_N_id_1_);
  void     reset();
  float    get_last_cfo();
  void     set_agc_enable(bool enable);
  ret_code run(srsran_cell_t* cell, std::array<uint8_t, SRSRAN_BCH_PAYLOAD_LEN>& bch_payload);
  void     set_cp_en(bool enable);

private:
  search_callback*       p = nullptr;
  srslog::basic_logger&  logger;
  srsran::rf_buffer_t    buffer       = {};
  srsran_ue_cellsearch_t cs           = {};
  srsran_ue_mib_sync_t   ue_mib_sync  = {};
  int                    force_N_id_2 = 0;
  int                    force_N_id_1 = 0;
};

}; // namespace srsue

#endif // SRSUE_SEARCH_H
