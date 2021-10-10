/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#ifndef SRSRAN_DUMMY_RX_HARQ_PROC_H
#define SRSRAN_DUMMY_RX_HARQ_PROC_H

#include <mutex>
#include <srsenb/hdr/stack/mac/common/mac_metrics.h>
#include <srsran/adt/circular_array.h>
#include <srsran/common/buffer_pool.h>
#include <srsran/common/phy_cfg_nr.h>
#include <srsran/common/standard_streams.h>

class dummy_rx_harq_proc
{
private:
  srsran::byte_buffer_t  data;
  srsran_softbuffer_rx_t softbuffer = {};
  std::atomic<uint32_t>  tbs        = {0};
  bool                   first      = true;
  uint32_t               ndi        = 0;

public:
  dummy_rx_harq_proc() : data(0)
  {
    // Initialise softbuffer
    if (srsran_softbuffer_rx_init_guru(&softbuffer, SRSRAN_SCH_NR_MAX_NOF_CB_LDPC, SRSRAN_LDPC_MAX_LEN_ENCODED_CB) <
        SRSRAN_SUCCESS) {
      ERROR("Error Tx buffer");
    }
  }

  ~dummy_rx_harq_proc() { srsran_softbuffer_rx_free(&softbuffer); }

  srsran_softbuffer_rx_t& get_softbuffer(uint32_t ndi_, uint32_t tbs_)
  {
    if (ndi != ndi_ || first) {
      srsran_softbuffer_rx_reset(&softbuffer);
      ndi   = ndi_;
      tbs   = tbs_;
      first = false;
    }

    return softbuffer;
  }

  uint32_t get_tbs() const { return tbs; }
};

class dummy_rx_harq_entity : public srsran::circular_array<dummy_rx_harq_proc, SRSRAN_MAX_HARQ_PROC_DL_NR>
{};

#endif // SRSRAN_DUMMY_RX_HARQ_PROC_H
