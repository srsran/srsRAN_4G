/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
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
#include "srslte/common/common.h"
#include "srslte/common/log.h"
#include "srslte/interfaces/mac_interface_types.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/mac/pdu.h"
#include "srslte/srslog/srslog.h"
#include <mutex>

namespace srsue {

class mux
{
public:
  explicit mux(srslog::basic_logger& logger);
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

  void setup_lcid(const srslte::logical_channel_config_t& config);

  void print_logical_channel_state(const std::string& info);

private:
  bool     has_logical_channel(const uint32_t& lcid);
  bool     pdu_move_to_msg3(uint32_t pdu_sz);
  uint32_t allocate_sdu(uint32_t lcid, srslte::sch_pdu* pdu, int max_sdu_sz);
  bool     sched_sdu(srslte::logical_channel_config_t* ch, int* sdu_space, int max_sdu_sz);

  const static int MAX_NOF_SUBHEADERS = 20;

  std::vector<srslte::logical_channel_config_t> logical_channels;

  // Mutex for exclusive access
  std::mutex mutex;

  srslog::basic_logger& logger;
  rlc_interface_mac*    rlc              = nullptr;
  bsr_interface_mux*    bsr_procedure    = nullptr;
  phr_proc*             phr_procedure    = nullptr;
  uint16_t              pending_crnti_ce = 0;

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
