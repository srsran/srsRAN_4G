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

#ifndef SRSUE_MUX_H
#define SRSUE_MUX_H

#include <pthread.h>

#include <vector>

#include "proc_bsr.h"
#include "proc_phr.h"
#include "srslte/common/common.h"
#include "srslte/common/log.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/mac/pdu.h"
#include <mutex>

/* Logical Channel Multiplexing and Prioritization + Msg3 Buffer */

typedef struct {
  uint8_t  lcid;
  uint8_t  lcg;
  int32_t  Bj;
  int32_t  PBR; // in kByte/s, -1 sets to infinity
  uint32_t bucket_size;
  uint32_t BSD;
  uint32_t priority;
  int      sched_len;
  int      buffer_len;
} logical_channel_config_t;

namespace srsue {

class mux
{
public:
  mux(srslte::log_ref log_);
  ~mux(){};
  void reset();
  void init(rlc_interface_mac* rlc, bsr_interface_mux* bsr_procedure, phr_proc* phr_procedure_);

  void step();

  bool is_pending_any_sdu();
  bool is_pending_sdu(uint32_t lcid);

  uint8_t* pdu_get(srslte::byte_buffer_t* payload, uint32_t pdu_sz);
  uint8_t* msg3_get(srslte::byte_buffer_t* payload, uint32_t pdu_sz);

  void msg3_flush();
  bool msg3_is_transmitted();
  void msg3_prepare();
  bool msg3_is_pending();
  bool msg3_is_empty();

  void append_crnti_ce_next_tx(uint16_t crnti);

  void setup_lcid(const logical_channel_config_t& config);

  void print_logical_channel_state(const std::string& info);

private:
  bool has_logical_channel(const uint32_t& lcid);
  bool pdu_move_to_msg3(uint32_t pdu_sz);
  bool allocate_sdu(uint32_t lcid, srslte::sch_pdu* pdu, int max_sdu_sz);
  bool sched_sdu(logical_channel_config_t* ch, int* sdu_space, int max_sdu_sz);

  const static int MAX_NOF_SUBHEADERS = 20;

  std::vector<logical_channel_config_t> logical_channels;

  // Mutex for exclusive access
  std::mutex mutex;

  srslte::log_ref    log_h;
  rlc_interface_mac* rlc              = nullptr;
  bsr_interface_mux* bsr_procedure    = nullptr;
  phr_proc*          phr_procedure    = nullptr;
  uint16_t           pending_crnti_ce = 0;

  /* Msg3 Buffer */
  srslte::byte_buffer_t msg_buff;

  /* PDU Buffer */
  srslte::sch_pdu pdu_msg;

  srslte::byte_buffer_t msg3_buff;
  bool                  msg3_has_been_transmitted = false;
  bool                  msg3_pending              = false;
};

} // namespace srsue

#endif // SRSUE_MUX_H
