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

#ifndef SRSUE_MUX_H
#define SRSUE_MUX_H

#include <pthread.h>

#include <vector>

#include "proc_bsr.h"
#include "proc_phr.h"
#include "srsran/common/common.h"
#include "srsran/interfaces/mac_interface_types.h"
#include "srsran/mac/pdu.h"
#include "srsran/srslog/srslog.h"
#include "srsue/hdr/stack/mac_common/mux_base.h"
#include <mutex>

namespace srsue {

class mux : private mux_base
{
public:
  explicit mux(srslog::basic_logger& logger);
  ~mux(){};
  void reset();
  void init(rlc_interface_mac* rlc, bsr_interface_mux* bsr_procedure, phr_proc* phr_procedure_);

  void step();

  bool is_pending_any_sdu();
  bool is_pending_sdu(uint32_t lcid);

  uint8_t* pdu_get(srsran::byte_buffer_t* payload, uint32_t pdu_sz);
  uint8_t* msg3_get(srsran::byte_buffer_t* payload, uint32_t pdu_sz);

  void msg3_flush();
  bool msg3_is_transmitted();
  void msg3_prepare();
  bool msg3_is_pending();
  bool msg3_is_empty();

  void append_crnti_ce_next_tx(uint16_t crnti);

  void setup_lcid(const srsran::logical_channel_config_t& config);

  void print_logical_channel_state(const std::string& info);

private:
  uint8_t* pdu_get_nolock(srsran::byte_buffer_t* payload, uint32_t pdu_sz);
  bool     pdu_move_to_msg3(uint32_t pdu_sz);
  uint32_t allocate_sdu(uint32_t lcid, srsran::sch_pdu* pdu, int max_sdu_sz);
  bool     sched_sdu(srsran::logical_channel_config_t* ch, int* sdu_space, int max_sdu_sz);

  const static int MAX_NOF_SUBHEADERS = 20;

  // Mutex for exclusive access
  std::mutex mutex;

  srslog::basic_logger& logger;
  rlc_interface_mac*    rlc              = nullptr;
  bsr_interface_mux*    bsr_procedure    = nullptr;
  phr_proc*             phr_procedure    = nullptr;
  uint16_t              pending_crnti_ce = 0;

  /* Msg3 Buffer */
  srsran::byte_buffer_t msg_buff;

  /* PDU Buffer */
  srsran::sch_pdu pdu_msg;

  srsran::byte_buffer_t msg3_buff;
  bool                  msg3_has_been_transmitted = false;
  bool                  msg3_pending              = false;
};

} // namespace srsue

#endif // SRSUE_MUX_H
