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

#include "srslte/interfaces/enb_interfaces.h"
#include "srslte/interfaces/enb_metrics_interface.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/srslog/srslog.h"
#include "srslte/upper/rlc.h"
#include <map>

#ifndef SRSENB_RLC_H
#define SRSENB_RLC_H

typedef struct {
  uint32_t lcid;
  uint32_t plmn;
  uint16_t mtch_stop;
  uint8_t* payload;
} mch_service_t;

namespace srsenb {

class rlc : public rlc_interface_mac, public rlc_interface_rrc, public rlc_interface_pdcp
{
public:
  explicit rlc(srslog::basic_logger& logger) : logger(logger) {}
  void
       init(pdcp_interface_rlc* pdcp_, rrc_interface_rlc* rrc_, mac_interface_rlc* mac_, srslte::timer_handler* timers_);
  void stop();
  void get_metrics(rlc_metrics_t& m, const uint32_t nof_tti);

  // rlc_interface_rrc
  void clear_buffer(uint16_t rnti);
  void add_user(uint16_t rnti);
  void rem_user(uint16_t rnti);
  void add_bearer(uint16_t rnti, uint32_t lcid, srslte::rlc_config_t cnfg);
  void add_bearer_mrb(uint16_t rnti, uint32_t lcid);
  void del_bearer(uint16_t rnti, uint32_t lcid);
  bool has_bearer(uint16_t rnti, uint32_t lcid);
  bool suspend_bearer(uint16_t rnti, uint32_t lcid);
  bool resume_bearer(uint16_t rnti, uint32_t lcid);
  void reestablish(uint16_t rnti) final;

  // rlc_interface_pdcp
  void        write_sdu(uint16_t rnti, uint32_t lcid, srslte::unique_byte_buffer_t sdu);
  void        discard_sdu(uint16_t rnti, uint32_t lcid, uint32_t discard_sn);
  bool        rb_is_um(uint16_t rnti, uint32_t lcid);
  std::string get_rb_name(uint32_t lcid);
  bool        sdu_queue_is_full(uint16_t rnti, uint32_t lcid);

  // rlc_interface_mac
  int  read_pdu(uint16_t rnti, uint32_t lcid, uint8_t* payload, uint32_t nof_bytes);
  void write_pdu(uint16_t rnti, uint32_t lcid, uint8_t* payload, uint32_t nof_bytes);
  void read_pdu_pcch(uint8_t* payload, uint32_t buffer_size);

private:
  class user_interface : public srsue::pdcp_interface_rlc, public srsue::rrc_interface_rlc
  {
  public:
    void        write_pdu(uint32_t lcid, srslte::unique_byte_buffer_t sdu);
    void        write_pdu_bcch_bch(srslte::unique_byte_buffer_t sdu);
    void        write_pdu_bcch_dlsch(srslte::unique_byte_buffer_t sdu);
    void        write_pdu_pcch(srslte::unique_byte_buffer_t sdu);
    void        write_pdu_mch(uint32_t lcid, srslte::unique_byte_buffer_t sdu) {}
    void        max_retx_attempted();
    std::string get_rb_name(uint32_t lcid);
    uint16_t    rnti;

    srsenb::pdcp_interface_rlc*  pdcp;
    srsenb::rrc_interface_rlc*   rrc;
    std::unique_ptr<srslte::rlc> rlc;
    srsenb::rlc*                 parent;
  };

  void update_bsr(uint32_t rnti, uint32_t lcid, uint32_t tx_queue, uint32_t retx_queue);

  pthread_rwlock_t rwlock;

  std::map<uint32_t, user_interface> users;
  std::vector<mch_service_t>         mch_services;

  mac_interface_rlc*        mac;
  pdcp_interface_rlc*       pdcp;
  rrc_interface_rlc*        rrc;
  srslog::basic_logger&     logger;
  srslte::byte_buffer_pool* pool;
  srslte::timer_handler*    timers;
};

} // namespace srsenb

#endif // SRSENB_RLC_H
