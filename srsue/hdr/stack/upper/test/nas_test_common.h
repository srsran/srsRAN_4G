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

#ifndef SRSUE_NAS_TEST_COMMON
#define SRSUE_NAS_TEST_COMMON

#include "srsran/common/bcd_helpers.h"
#include "srsran/common/test_common.h"
#include "srsran/common/tsan_options.h"
#include "srsran/interfaces/ue_pdcp_interfaces.h"
#include "srsran/interfaces/ue_rrc_interfaces.h"
#include "srsran/srslog/srslog.h"
#include "srsran/test/ue_test_interfaces.h"
#include "srsue/hdr/stack/upper/gw.h"
#include "srsue/hdr/stack/upper/nas.h"
#include "srsue/hdr/stack/upper/nas_5g.h"
#include "srsue/hdr/stack/upper/usim.h"
#include "srsue/hdr/stack/upper/usim_base.h"

#define LCID 1

uint8_t auth_request_pdu[] = {0x07, 0x52, 0x01, 0x0c, 0x63, 0xa8, 0x54, 0x13, 0xe6, 0xa4, 0xce, 0xd9,
                              0x86, 0xfb, 0xe5, 0xce, 0x9b, 0x62, 0x5e, 0x10, 0x67, 0x57, 0xb3, 0xc2,
                              0xb9, 0x70, 0x90, 0x01, 0x0c, 0x72, 0x8a, 0x67, 0x57, 0x92, 0x52, 0xb8};

uint8_t sec_mode_command_pdu[] = {0x37, 0x4e, 0xfd, 0x57, 0x11, 0x00, 0x07, 0x5d, 0x02, 0x01, 0x02, 0xf0, 0x70, 0xc1};

uint8_t attach_accept_pdu[] = {0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x42, 0x01, 0x3e, 0x06, 0x00, 0x00,
                               0xf1, 0x10, 0x00, 0x01, 0x00, 0x2a, 0x52, 0x01, 0xc1, 0x01, 0x04, 0x1b, 0x07,
                               0x74, 0x65, 0x73, 0x74, 0x31, 0x32, 0x33, 0x06, 0x6d, 0x6e, 0x63, 0x30, 0x30,
                               0x31, 0x06, 0x6d, 0x63, 0x63, 0x30, 0x30, 0x31, 0x04, 0x67, 0x70, 0x72, 0x73,
                               0x05, 0x01, 0xc0, 0xa8, 0x05, 0x02, 0x27, 0x01, 0x80, 0x50, 0x0b, 0xf6, 0x00,
                               0xf1, 0x10, 0x80, 0x01, 0x01, 0x35, 0x16, 0x6d, 0xbc, 0x64, 0x01, 0x00};

uint8_t esm_info_req_pdu[] = {0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x5a, 0xd9};

uint8_t activate_dedicated_eps_bearer_pdu[] = {0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x62, 0x00, 0xc5, 0x05,
                                               0x01, 0x01, 0x07, 0x21, 0x31, 0x00, 0x03, 0x40, 0x08, 0xae,
                                               0x5d, 0x02, 0x00, 0xc2, 0x81, 0x34, 0x01, 0x4d};

uint8_t deactivate_eps_bearer_pdu[] = {0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x62, 0x00, 0xcd, 0x24};

uint16 mcc = 61441;
uint16 mnc = 65281;

using namespace srsue;

namespace srsran {

// fake classes
class pdcp_dummy : public rrc_interface_pdcp, public pdcp_interface_stack
{
public:
  void        write_pdu(uint32_t lcid, unique_byte_buffer_t pdu) override {}
  void        write_pdu_bcch_bch(unique_byte_buffer_t pdu) override {}
  void        write_pdu_bcch_dlsch(unique_byte_buffer_t pdu) override {}
  void        write_pdu_pcch(unique_byte_buffer_t pdu) override {}
  void        write_pdu_mch(uint32_t lcid, srsran::unique_byte_buffer_t sdu) override {}
  const char* get_rb_name(uint32_t lcid) override { return "lcid"; }
  void        write_sdu(uint32_t lcid, unique_byte_buffer_t sdu, int sn = -1) override {}
  bool        is_eps_bearer_id_enabled(uint32_t eps_bearer_id) { return false; }
  void        notify_pdcp_integrity_error(uint32_t lcid) override {}
  bool        is_lcid_enabled(uint32_t lcid) override { return false; }
};

class rrc_dummy : public rrc_interface_nas
{
public:
  rrc_dummy() : last_sdu_len(0)
  {
    plmns[0].plmn_id.from_number(mcc, mnc);
    plmns[0].tac = 0xffff;
  }
  void init(nas* nas_) { nas_ptr = nas_; }
  void write_sdu(unique_byte_buffer_t sdu)
  {
    last_sdu_len = sdu->N_bytes;
    // printf("NAS generated SDU (len=%d):\n", sdu->N_bytes);
    // srsran_vec_fprint_byte(stdout, sdu->msg, sdu->N_bytes);
  }
  const char* get_rb_name(uint32_t lcid) { return "lcid"; }
  uint32_t    get_last_sdu_len() { return last_sdu_len; }
  void        reset() { last_sdu_len = 0; }

  bool plmn_search()
  {
    nas_ptr->plmn_search_completed(plmns, 1);
    return true;
  }
  void plmn_select(srsran::plmn_id_t plmn_id){};
  void set_ue_identity(srsran::s_tmsi_t s_tmsi) {}
  bool connection_request(srsran::establishment_cause_t cause, srsran::unique_byte_buffer_t sdu)
  {
    printf("NAS generated SDU (len=%d):\n", sdu->N_bytes);
    last_sdu_len = sdu->N_bytes;
    srsran_vec_fprint_byte(stdout, sdu->msg, sdu->N_bytes);
    is_connected_flag = true;
    nas_ptr->connection_request_completed(true);
    return true;
  }
  bool is_connected() { return is_connected_flag; }

  uint16_t get_mcc() { return mcc; }
  uint16_t get_mnc() { return mnc; }
  void     enable_capabilities() {}
  uint32_t get_lcid_for_eps_bearer(const uint32_t& eps_bearer_id) { return 0; }
  void     paging_completed(bool outcome) {}
  bool     has_nr_dc() { return false; }

private:
  nas*                            nas_ptr;
  uint32_t                        last_sdu_len;
  nas_interface_rrc::found_plmn_t plmns[nas_interface_rrc::MAX_FOUND_PLMNS];
  bool                            is_connected_flag = false;
};

class rrc_nr_dummy : public rrc_nr_interface_nas_5g
{
public:
  rrc_nr_dummy() : last_sdu_len(0)
  {
    plmns[0].plmn_id.from_number(mcc, mnc);
    plmns[0].tac = 0xffff;
  }
  void init(srsue::nas_5g* nas_5g_) { nas_5g_ptr = nas_5g_; }
  int  write_sdu(unique_byte_buffer_t sdu)
  {
    last_sdu_len = sdu->N_bytes;
    // printf("NAS generated SDU (len=%d):\n", sdu->N_bytes);
    return SRSRAN_SUCCESS;
  }
  virtual bool is_connected() { return true; }
  virtual int  connection_request(srsran::nr_establishment_cause_t cause, srsran::unique_byte_buffer_t sdu)
  {
    return SRSRAN_SUCCESS;
  }
  uint16_t get_mcc() { return 0x0000; }
  uint16_t get_mnc() { return 0x0000; }

private:
  srsue::nas_5g*                  nas_5g_ptr;
  uint32_t                        last_sdu_len;
  nas_interface_rrc::found_plmn_t plmns[nas_interface_rrc::MAX_FOUND_PLMNS];
};
template <typename T>
class test_stack_dummy : public srsue::stack_test_dummy, public stack_interface_gw, public thread
{
public:
  test_stack_dummy(pdcp_interface_stack* pdcp_) : pdcp(pdcp_), thread("DUMMY STACK") {}
  void init(T* nas_)
  {
    nas = nas_;
    start(-1);
  }
  bool switch_on()
  {
    task_sched.defer_task([this]() { nas->switch_on(); });
    return true;
  }
  void write_sdu(uint32_t lcid, srsran::unique_byte_buffer_t sdu) { pdcp->write_sdu(lcid, std::move(sdu)); }
  bool has_active_radio_bearer(uint32_t eps_bearer_id) { return true; }

  bool is_registered() { return true; }

  bool start_service_request() { return true; }

  void run_thread()
  {
    running = true;
    while (running) {
      task_sched.tic();
      task_sched.run_pending_tasks();
      nas->run_tti();
    }
  }
  void stop()
  {
    while (not running) {
      usleep(1000);
    }
    running = false;
    wait_thread_finish();
  }
  pdcp_interface_stack* pdcp    = nullptr;
  T*                    nas     = nullptr;
  std::atomic<bool>     running = {false};
};

class gw_dummy : public gw_interface_nas, public gw_interface_pdcp
{
  int setup_if_addr(uint32_t eps_bearer_id, uint8_t pdn_type, uint32_t ip_addr, uint8_t* ipv6_if_id, char* err_str)
  {
    return SRSRAN_SUCCESS;
  }
  int deactivate_eps_bearer(const uint32_t eps_bearer_id) { return SRSRAN_SUCCESS; }
  int apply_traffic_flow_template(const uint8_t& eps_bearer_id, const LIBLTE_MME_TRAFFIC_FLOW_TEMPLATE_STRUCT* tft)
  {
    return SRSRAN_SUCCESS;
  }
  void write_pdu(uint32_t lcid, unique_byte_buffer_t pdu) {}
  void write_pdu_mch(uint32_t lcid, srsran::unique_byte_buffer_t sdu) {}
  void set_test_loop_mode(const test_loop_mode_state_t mode, const uint32_t ip_pdu_delay_ms = 0) {}
};

} // namespace srsran

#endif // SRSUE_NAS_TEST_COMMON