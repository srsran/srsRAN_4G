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

#ifndef SRSUE_SEARCH_H
#define SRSUE_SEARCH_H

#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/radio/radio.h"
#include "srslte/srslte.h"

namespace srsue {

class search_callback
{
public:
  virtual int                          radio_recv_fnc(srslte::rf_buffer_t&, srslte_timestamp_t* rx_time) = 0;
  virtual void set_ue_sync_opts(srslte_ue_sync_t* q, float cfo)                                     = 0;
  virtual srslte::radio_interface_phy* get_radio()                                                  = 0;
  virtual void                         set_rx_gain(float gain)                                      = 0;
};

// Class to run cell search
class search
{
public:
  typedef enum { CELL_NOT_FOUND, CELL_FOUND, ERROR, TIMEOUT } ret_code;

  ~search();
  void     init(srslte::rf_buffer_t& buffer_, srslte::log* log_h, uint32_t nof_rx_channels, search_callback* parent);
  void     reset();
  float    get_last_cfo();
  void     set_agc_enable(bool enable);
  ret_code run(srslte_cell_t* cell, std::array<uint8_t, SRSLTE_BCH_PAYLOAD_LEN>& bch_payload);

private:
  search_callback*       p            = nullptr;
  srslte::log*           log_h        = nullptr;
  srslte::rf_buffer_t    buffer       = {};
  srslte_ue_cellsearch_t cs           = {};
  srslte_ue_mib_sync_t   ue_mib_sync  = {};
  int                    force_N_id_2 = 0;
};

}; // namespace srsue

#endif // SRSUE_SEARCH_H
