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

#ifndef SRSUE_MUX_NR_H
#define SRSUE_MUX_NR_H

#include "mac_nr_interfaces.h"
#include "proc_bsr_nr.h"
#include "srsran/common/byte_buffer.h"
#include "srsran/common/common.h"
#include "srsran/mac/mac_sch_pdu_nr.h"
#include "srsran/srslog/srslog.h"
#include "srsran/srsran.h"
#include "srsue/hdr/stack/mac_common/mux_base.h"
#include <mutex>

namespace srsue {

class mux_nr final : mux_base, public mux_interface_bsr_nr
{
public:
  explicit mux_nr(mac_interface_mux_nr& mac_, srslog::basic_logger& logger);
  ~mux_nr(){};
  void reset();
  int32_t init(rlc_interface_mac* rlc_);

  void msg3_flush();
  void msg3_prepare();
  void msg3_transmitted();
  bool msg3_is_transmitted();
  bool msg3_is_pending();
  bool msg3_is_empty();

  // MAC interface
  int setup_lcid(const srsran::logical_channel_config_t& config);

  // Interface of UL HARQ
  srsran::unique_byte_buffer_t get_pdu(uint32_t max_pdu_len);

  // Interface for BSR procedure
  void generate_bsr_mac_ce(const srsran::bsr_format_nr_t& format);

private:
  // internal helper methods

  // ctor configured members
  mac_interface_mux_nr&        mac;
  rlc_interface_mac* rlc = nullptr;
  srslog::basic_logger&        logger;

  // Msg3 related
  srsran::unique_byte_buffer_t msg3_buff = nullptr;
  typedef enum { none, pending, transmitted } msg3_state_t;
  msg3_state_t msg3_state = none;

  static constexpr int32_t MIN_RLC_PDU_LEN =
      5; ///< minimum bytes that need to be available in a MAC PDU for attempting to add another RLC SDU

  srsran::unique_byte_buffer_t rlc_buff = nullptr;

  srsran::mac_sch_pdu_nr tx_pdu; /// single MAC PDU for packing

  enum { no_bsr, sbsr_ce, lbsr_ce } add_bsr_ce = no_bsr; /// BSR procedure requests MUX to add a BSR CE

  // Mutex for exclusive access
  std::mutex mutex;
};

} // namespace srsue

#endif // SRSUE_MUX_NR_H
