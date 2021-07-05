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

#ifndef SRSRAN_DUMMY_RX_HARQ_PROC_H
#define SRSRAN_DUMMY_RX_HARQ_PROC_H

#include <mutex>
#include <srsenb/hdr/stack/mac/mac_metrics.h>
#include <srsran/adt/circular_array.h>
#include <srsran/common/buffer_pool.h>
#include <srsran/common/phy_cfg_nr.h>
#include <srsran/common/standard_streams.h>

struct dummy_rx_harq_proc {
  static const uint32_t  MAX_TB_SZ = SRSRAN_LDPC_MAX_LEN_CB * SRSRAN_SCH_NR_MAX_NOF_CB_LDPC;
  srsran::byte_buffer_t  data;
  srsran_softbuffer_rx_t softbuffer = {};
  std::atomic<uint32_t>  tbs        = {0};

  dummy_rx_harq_proc() : data(0)
  {
    // Initialise softbuffer
    if (srsran_softbuffer_rx_init_guru(&softbuffer, SRSRAN_SCH_NR_MAX_NOF_CB_LDPC, SRSRAN_LDPC_MAX_LEN_ENCODED_CB) <
        SRSRAN_SUCCESS) {
      ERROR("Error Tx buffer");
    }
  }

  ~dummy_rx_harq_proc() { srsran_softbuffer_rx_free(&softbuffer); }
};

#endif // SRSRAN_DUMMY_RX_HARQ_PROC_H
