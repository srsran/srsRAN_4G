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

#include "srsran/interfaces/gnb_interfaces.h"
#include "srsran/interfaces/ue_gw_interfaces.h"
#include "srsran/interfaces/ue_rlc_interfaces.h"
#include "srsran/upper/pdcp.h"
#include <map>

#ifndef SRSENB_PDCP_NR_H
#define SRSENB_PDCP_NR_H

namespace srsenb {

struct pdcp_nr_args_t {
  std::string log_level;
  uint32_t    log_hex_limit;
};

class pdcp_nr : public pdcp_interface_rlc_nr, public pdcp_interface_sdap_nr, public pdcp_interface_rrc_nr
{
public:
  explicit pdcp_nr(srsran::task_sched_handle task_sched_, const char* logname);
  virtual ~pdcp_nr() = default;
  void init(const pdcp_nr_args_t&   args_,
            rlc_interface_pdcp_nr*  rlc_,
            rrc_interface_pdcp_nr*  rrc_,
            sdap_interface_pdcp_nr* gtpu_);
  void stop();

  // pdcp_interface_rlc_nr
  void write_pdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t sdu);
  void notify_delivery(uint16_t rnti, uint32_t lcid, const srsran::pdcp_sn_vector_t& pdcp_sn);
  void notify_failure(uint16_t rnti, uint32_t lcid, const srsran::pdcp_sn_vector_t& pdcp_sn);
  void write_pdu_mch(uint32_t lcid, srsran::unique_byte_buffer_t sdu) {}

  // pdcp_interface_rrc_nr
  void reset(uint16_t rnti) final;
  void add_user(uint16_t rnti) final;
  void rem_user(uint16_t rnti) final;
  void write_sdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t sdu) final;
  void add_bearer(uint16_t rnti, uint32_t lcid, srsran::pdcp_config_t cnfg) final;
  void config_security(uint16_t rnti, uint32_t lcid, srsran::as_security_config_t sec_cfg) final;
  void enable_integrity(uint16_t rnti, uint32_t lcid) final;
  void enable_encryption(uint16_t rnti, uint32_t lcid) final;

private:
  class user_interface_rlc final : public srsue::rlc_interface_pdcp
  {
  public:
    uint16_t                       rnti;
    srsenb::rlc_interface_pdcp_nr* rlc;
    // rlc_interface_pdcp_nr
    void write_sdu(uint32_t lcid, srsran::unique_byte_buffer_t sdu) final;
    void discard_sdu(uint32_t lcid, uint32_t discard_sn) final;
    bool rb_is_um(uint32_t lcid) final;
    bool sdu_queue_is_full(uint32_t lcid) final;
  };

  class user_interface_sdap : public srsue::gw_interface_pdcp
  {
  public:
    uint16_t                        rnti;
    srsenb::sdap_interface_pdcp_nr* sdap;
    // gw_interface_pdcp
    void write_pdu(uint32_t lcid, srsran::unique_byte_buffer_t pdu) final;
    void write_pdu_mch(uint32_t lcid, srsran::unique_byte_buffer_t sdu) final {}
  };

  class user_interface_rrc : public srsue::rrc_interface_pdcp
  {
  public:
    uint16_t                       rnti;
    srsenb::rrc_interface_pdcp_nr* rrc;
    // rrc_interface_pdcp_nr
    void        write_pdu(uint32_t lcid, srsran::unique_byte_buffer_t pdu) final;
    void        write_pdu_bcch_bch(srsran::unique_byte_buffer_t pdu) final;
    void        write_pdu_bcch_dlsch(srsran::unique_byte_buffer_t pdu) final;
    void        write_pdu_pcch(srsran::unique_byte_buffer_t pdu) final;
    void        write_pdu_mch(uint32_t lcid, srsran::unique_byte_buffer_t pdu) final {}
    std::string get_rb_name(uint32_t lcid) final;
  };

  class user_interface
  {
  public:
    user_interface_rlc            rlc_itf;
    user_interface_sdap           sdap_itf;
    user_interface_rrc            rrc_itf;
    std::unique_ptr<srsran::pdcp> pdcp;
  };

  // args
  pdcp_nr_args_t          m_args = {};
  rlc_interface_pdcp_nr*  m_rlc  = nullptr;
  rrc_interface_pdcp_nr*  m_rrc  = nullptr;
  sdap_interface_pdcp_nr* m_sdap = nullptr;

  std::map<uint32_t, user_interface> users;

  srsran::task_sched_handle task_sched;
  srslog::basic_logger&     logger;
};

} // namespace srsenb

#endif // SRSENB_PDCP_NR_H
