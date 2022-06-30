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

#ifndef SRSRAN_HARQ_SOFTBUFFER_H
#define SRSRAN_HARQ_SOFTBUFFER_H

#include "srsran/adt/pool/pool_interface.h"
#include "srsran/adt/span.h"
extern "C" {
#include "srsran/phy/common/phy_common_nr.h"
#include "srsran/phy/fec/softbuffer.h"
#include "srsran/phy/phch/sch_nr.h"
#include "srsran/phy/utils/vector.h"
}

namespace srsenb {

class tx_harq_softbuffer
{
public:
  tx_harq_softbuffer() { bzero(&buffer, sizeof(buffer)); }
  explicit tx_harq_softbuffer(uint32_t nof_prb_)
  {
    // Note: for now we use same size regardless of nof_prb_
    srsran_softbuffer_tx_init_guru(&buffer, SRSRAN_SCH_NR_MAX_NOF_CB_LDPC, SRSRAN_LDPC_MAX_LEN_ENCODED_CB);
  }
  tx_harq_softbuffer(const tx_harq_softbuffer&) = delete;
  tx_harq_softbuffer(tx_harq_softbuffer&& other) noexcept
  {
    memcpy(&buffer, &other.buffer, sizeof(other.buffer));
    bzero(&other.buffer, sizeof(other.buffer));
  }
  tx_harq_softbuffer& operator=(const tx_harq_softbuffer&) = delete;
  tx_harq_softbuffer& operator                             =(tx_harq_softbuffer&& other) noexcept
  {
    if (this != &other) {
      destroy();
      memcpy(&buffer, &other.buffer, sizeof(other.buffer));
      bzero(&other.buffer, sizeof(other.buffer));
    }
    return *this;
  }
  ~tx_harq_softbuffer() { destroy(); }

  void reset() { srsran_softbuffer_tx_reset(&buffer); }

  srsran_softbuffer_tx_t&       operator*() { return buffer; }
  const srsran_softbuffer_tx_t& operator*() const { return buffer; }
  srsran_softbuffer_tx_t*       operator->() { return &buffer; }
  const srsran_softbuffer_tx_t* operator->() const { return &buffer; }
  srsran_softbuffer_tx_t*       get() { return &buffer; }
  const srsran_softbuffer_tx_t* get() const { return &buffer; }

private:
  void destroy() { srsran_softbuffer_tx_free(&buffer); }

  srsran_softbuffer_tx_t buffer;
};

class rx_harq_softbuffer
{
public:
  rx_harq_softbuffer() { bzero(&buffer, sizeof(buffer)); }
  explicit rx_harq_softbuffer(uint32_t nof_prb_)
  {
    // Note: for now we use same size regardless of nof_prb_
    srsran_softbuffer_rx_init_guru(&buffer, SRSRAN_SCH_NR_MAX_NOF_CB_LDPC, SRSRAN_LDPC_MAX_LEN_ENCODED_CB);
  }
  rx_harq_softbuffer(const rx_harq_softbuffer&) = delete;
  rx_harq_softbuffer(rx_harq_softbuffer&& other) noexcept
  {
    memcpy(&buffer, &other.buffer, sizeof(other.buffer));
    bzero(&other.buffer, sizeof(other.buffer));
  }
  rx_harq_softbuffer& operator=(const rx_harq_softbuffer&) = delete;
  rx_harq_softbuffer& operator                             =(rx_harq_softbuffer&& other) noexcept
  {
    if (this != &other) {
      destroy();
      memcpy(&buffer, &other.buffer, sizeof(other.buffer));
      bzero(&other.buffer, sizeof(other.buffer));
    }
    return *this;
  }
  ~rx_harq_softbuffer() { destroy(); }

  void reset() { srsran_softbuffer_rx_reset(&buffer); }
  void reset(uint32_t tbs_bits) { srsran_softbuffer_rx_reset_tbs(&buffer, tbs_bits); }

  srsran_softbuffer_rx_t&       operator*() { return buffer; }
  const srsran_softbuffer_rx_t& operator*() const { return buffer; }
  srsran_softbuffer_rx_t*       operator->() { return &buffer; }
  const srsran_softbuffer_rx_t* operator->() const { return &buffer; }
  srsran_softbuffer_rx_t*       get() { return &buffer; }
  const srsran_softbuffer_rx_t* get() const { return &buffer; }

private:
  void destroy() { srsran_softbuffer_rx_free(&buffer); }

  srsran_softbuffer_rx_t buffer;
};

class harq_softbuffer_pool
{
public:
  harq_softbuffer_pool(const harq_softbuffer_pool&) = delete;
  harq_softbuffer_pool(harq_softbuffer_pool&&)      = delete;
  harq_softbuffer_pool& operator=(const harq_softbuffer_pool&) = delete;
  harq_softbuffer_pool& operator=(harq_softbuffer_pool&&) = delete;

  void init_pool(uint32_t nof_prb, uint32_t batch_size = MAX_HARQ * 4, uint32_t thres = 0, uint32_t init_size = 0);

  srsran::unique_pool_ptr<tx_harq_softbuffer> get_tx(uint32_t nof_prb);
  srsran::unique_pool_ptr<rx_harq_softbuffer> get_rx(uint32_t nof_prb);

  static harq_softbuffer_pool& get_instance()
  {
    static harq_softbuffer_pool pool;
    return pool;
  }

private:
  const static uint32_t MAX_HARQ = 16;

  harq_softbuffer_pool() = default;

  std::array<std::unique_ptr<srsran::obj_pool_itf<tx_harq_softbuffer> >, SRSRAN_MAX_PRB_NR> tx_pool;
  std::array<std::unique_ptr<srsran::obj_pool_itf<rx_harq_softbuffer> >, SRSRAN_MAX_PRB_NR> rx_pool;
};

} // namespace srsenb

#endif // SRSRAN_HARQ_SOFTBUFFER_H
