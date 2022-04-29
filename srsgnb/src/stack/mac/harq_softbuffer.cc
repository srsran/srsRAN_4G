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

#include "srsgnb/hdr/stack/mac/harq_softbuffer.h"
#include "srsran/adt/pool/obj_pool.h"

namespace srsenb {

void harq_softbuffer_pool::init_pool(uint32_t nof_prb, uint32_t batch_size, uint32_t thres, uint32_t init_size)
{
  srsran_assert(nof_prb <= SRSRAN_MAX_PRB_NR, "Invalid nof prb=%d", nof_prb);
  size_t idx = nof_prb - 1;
  if (tx_pool[idx] != nullptr) {
    return;
  }
  if (thres == 0) {
    thres = batch_size;
  }
  if (init_size == 0) {
    init_size = batch_size;
  }
  auto init_tx_softbuffers    = [nof_prb](void* ptr) { new (ptr) tx_harq_softbuffer(nof_prb); };
  auto recycle_tx_softbuffers = [](tx_harq_softbuffer& softbuffer) { softbuffer.reset(); };
  tx_pool[idx].reset(new srsran::background_obj_pool<tx_harq_softbuffer>(
      batch_size, thres, init_size, init_tx_softbuffers, recycle_tx_softbuffers));

  auto init_rx_softbuffers    = [nof_prb](void* ptr) { new (ptr) rx_harq_softbuffer(nof_prb); };
  auto recycle_rx_softbuffers = [](rx_harq_softbuffer& softbuffer) { softbuffer.reset(); };
  rx_pool[idx].reset(new srsran::background_obj_pool<rx_harq_softbuffer>(
      batch_size, thres, init_size, init_rx_softbuffers, recycle_rx_softbuffers));
}

srsran::unique_pool_ptr<tx_harq_softbuffer> harq_softbuffer_pool::get_tx(uint32_t nof_prb)
{
  srsran_assert(nof_prb <= SRSRAN_MAX_PRB_NR, "Invalid Nprb=%d", nof_prb);
  size_t idx = nof_prb - 1;
  if (tx_pool[idx] == nullptr) {
    init_pool(nof_prb);
  }
  return tx_pool[idx]->make();
}

srsran::unique_pool_ptr<rx_harq_softbuffer> harq_softbuffer_pool::get_rx(uint32_t nof_prb)
{
  srsran_assert(nof_prb <= SRSRAN_MAX_PRB_NR, "Invalid Nprb=%d", nof_prb);
  size_t idx = nof_prb - 1;
  if (rx_pool[idx] == nullptr) {
    init_pool(nof_prb);
  }
  return rx_pool[idx]->make();
}

} // namespace srsenb
