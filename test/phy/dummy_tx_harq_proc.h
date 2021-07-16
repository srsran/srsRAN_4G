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
#include <srsenb/hdr/stack/mac/common/mac_metrics.h>
#include <srsran/adt/circular_array.h>
#include <srsran/common/buffer_pool.h>
#include <srsran/common/phy_cfg_nr.h>
#include <srsran/common/standard_streams.h>

class dummy_tx_harq_proc
{
private:
  srsran::byte_buffer_t  data;
  srsran_softbuffer_tx_t softbuffer = {};
  std::atomic<uint32_t>  tbs        = {0};
  bool                   first      = true;
  uint32_t               ndi        = 0;

public:
  dummy_tx_harq_proc()
  {
    // Initialise softbuffer
    if (srsran_softbuffer_tx_init_guru(&softbuffer, SRSRAN_SCH_NR_MAX_NOF_CB_LDPC, SRSRAN_LDPC_MAX_LEN_ENCODED_CB) <
        SRSRAN_SUCCESS) {
      ERROR("Error Tx buffer");
    }
  }

  ~dummy_tx_harq_proc() { srsran_softbuffer_tx_free(&softbuffer); }

  srsran::byte_buffer_t& get_tb(uint32_t tbs_)
  {
    tbs = tbs_;
    return data;
  }

  srsran_softbuffer_tx_t& get_softbuffer(uint32_t ndi_)
  {
    if (ndi_ != ndi || first) {
      srsran_softbuffer_tx_reset(&softbuffer);
      ndi   = ndi_;
      first = false;
    }

    return softbuffer;
  }

  uint32_t get_tbs() const { return tbs; }
};

#endif // SRSRAN_TX_DUMMY_HARQ_PROC_H
