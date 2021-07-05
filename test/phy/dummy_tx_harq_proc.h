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

#ifndef SRSRAN_TX_DUMMY_HARQ_PROC_H
#define SRSRAN_TX_DUMMY_HARQ_PROC_H

#include <mutex>
#include <srsenb/hdr/stack/mac/mac_metrics.h>
#include <srsran/adt/circular_array.h>
#include <srsran/common/buffer_pool.h>
#include <srsran/common/phy_cfg_nr.h>
#include <srsran/common/standard_streams.h>

struct dummy_tx_harq_proc {
  static const uint32_t  MAX_TB_SZ = SRSRAN_LDPC_MAX_LEN_CB * SRSRAN_SCH_NR_MAX_NOF_CB_LDPC;
  srsran::byte_buffer_t  data;
  srsran_softbuffer_tx_t softbuffer = {};
  std::atomic<uint32_t>  tbs        = {0};

  dummy_tx_harq_proc()
  {
    // Initialise softbuffer
    if (srsran_softbuffer_tx_init_guru(&softbuffer, SRSRAN_SCH_NR_MAX_NOF_CB_LDPC, SRSRAN_LDPC_MAX_LEN_ENCODED_CB) <
        SRSRAN_SUCCESS) {
      ERROR("Error Tx buffer");
    }
  }

  ~dummy_tx_harq_proc() { srsran_softbuffer_tx_free(&softbuffer); }
};

#endif // SRSRAN_TX_DUMMY_HARQ_PROC_H
