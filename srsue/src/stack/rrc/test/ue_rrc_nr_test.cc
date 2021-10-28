/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#include "srsran/common/test_common.h"
#include "srsran/interfaces/ue_gw_interfaces.h"
#include "srsran/interfaces/ue_interfaces.h"
#include "srsran/interfaces/ue_pdcp_interfaces.h"
#include "srsran/interfaces/ue_rlc_interfaces.h"
#include "srsran/interfaces/ue_usim_interfaces.h"
#include "srsue/hdr/stack/rrc/rrc_nr.h"

using namespace srsue;

class dummy_phy : public phy_interface_rrc_nr
{
  bool set_config(const srsran::phy_cfg_nr_t& cfg) { return true; }
};

class dummy_mac : public mac_interface_rrc_nr
{
  void reset() {}
  int  setup_lcid(const srsran::logical_channel_config_t& config) { return SRSRAN_SUCCESS; }
  int  set_config(const srsran::bsr_cfg_nr_t& bsr_cfg) { return SRSRAN_SUCCESS; }
  int  set_config(const srsran::sr_cfg_nr_t& sr_cfg) { return SRSRAN_SUCCESS; }
  int  set_config(const srsran::dl_harq_cfg_nr_t& dl_hrq_cfg) { return SRSRAN_SUCCESS; }
  void set_config(const srsran::rach_nr_cfg_t& rach_cfg) {}
  int  add_tag_config(const srsran::tag_cfg_nr_t& tag_cfg) { return SRSRAN_SUCCESS; }
  int  set_config(const srsran::phr_cfg_nr_t& phr_cfg) { return SRSRAN_SUCCESS; }
  int  remove_tag_config(const uint32_t tag_id) { return SRSRAN_SUCCESS; }

  void start_ra_procedure() {}

  void set_contention_id(const uint64_t ue_identity){};

  bool set_crnti(const uint16_t crnti) { return true; };
};

class dummy_rlc : public rlc_interface_rrc
{
  void reset() {}
  void reestablish() {}
  void reestablish(uint32_t lcid) {}
  int  add_bearer(uint32_t lcid, const srsran::rlc_config_t& cnfg) { return SRSRAN_SUCCESS; }
  int  add_bearer_mrb(uint32_t lcid) { return SRSRAN_SUCCESS; }
  void del_bearer(uint32_t lcid) {}
  void suspend_bearer(uint32_t lcid) {}
  void resume_bearer(uint32_t lcid) {}
  void change_lcid(uint32_t old_lcid, uint32_t new_lcid) {}
  bool has_bearer(uint32_t lcid) { return true; }
  bool has_data(const uint32_t lcid) { return true; }
  bool is_suspended(const uint32_t lcid) { return true; }
  void write_sdu(uint32_t lcid, srsran::unique_byte_buffer_t sdu) {}
};

class dummy_pdcp : public pdcp_interface_rrc
{
  void set_enabled(uint32_t lcid, bool enabled){};
  void reestablish(){};
  void reestablish(uint32_t lcid){};
  void reset(){};
  void write_sdu(uint32_t lcid, srsran::unique_byte_buffer_t sdu, int sn = -1){};
  int  add_bearer(uint32_t lcid, const srsran::pdcp_config_t& cnfg) { return SRSRAN_SUCCESS; };
  void del_bearer(uint32_t lcid){};
  void change_lcid(uint32_t old_lcid, uint32_t new_lcid){};
  void config_security(uint32_t lcid, const srsran::as_security_config_t& sec_cfg){};
  void config_security_all(const srsran::as_security_config_t& sec_cfg){};
  void enable_integrity(uint32_t lcid, srsran::srsran_direction_t direction){};
  void enable_encryption(uint32_t                   lcid,
                         srsran::srsran_direction_t direction = srsran::srsran_direction_t::DIRECTION_TXRX){};
  void send_status_report(){};
  void send_status_report(uint32_t lcid){};
};

class dummy_gw : public gw_interface_rrc
{
  void add_mch_port(uint32_t lcid, uint32_t port){};
  bool is_running() { return true; };
};

class dummy_eutra : public rrc_eutra_interface_rrc_nr
{
  void new_cell_meas_nr(const std::vector<phy_meas_nr_t>& meas){};
  void nr_rrc_con_reconfig_complete(bool status){};
  void nr_notify_reconfiguration_failure(){};
  void nr_scg_failure_information(const srsran::scg_failure_cause_t cause){};
};

class dummy_sim : public usim_interface_rrc_nr
{
  bool generate_nr_context(uint16_t sk_counter, srsran::as_security_config_t* sec_cfg) { return true; }
  bool update_nr_context(srsran::as_security_config_t* sec_cfg) { return true; }
};

class dummy_stack : public stack_interface_rrc
{
  srsran::tti_point get_current_tti() final { return srsran::tti_point(); };
  void              add_eps_bearer(uint8_t eps_bearer_id, srsran::srsran_rat_t rat, uint32_t lcid) final{};
  void              remove_eps_bearer(uint8_t eps_bearer_id) final{};
  void              reset_eps_bearers() final{};
};

int rrc_nr_cap_request_test()
{
  srslog::init();
  srslog::basic_logger& logger = srslog::fetch_basic_logger("RRC-NR");
  logger.set_level(srslog::basic_levels::debug);
  logger.set_hex_dump_max_size(-1);
  srsran::task_scheduler    task_sched{512, 100};
  srsran::task_sched_handle task_sched_handle(&task_sched);
  rrc_nr                    rrc_nr(task_sched_handle);
  srsran::byte_buffer_t     caps;

  dummy_phy     dummy_phy;
  dummy_mac     dummy_mac;
  dummy_rlc     dummy_rlc;
  dummy_pdcp    dummy_pdcp;
  dummy_gw      dummy_gw;
  dummy_eutra   dummy_eutra;
  dummy_sim     dummy_sim;
  dummy_stack   dummy_stack;
  rrc_nr_args_t rrc_nr_args;

  rrc_nr_args.supported_bands_eutra.push_back(7);
  rrc_nr_args.supported_bands_nr.push_back(78);

  TESTASSERT(rrc_nr.init(&dummy_phy,
                         &dummy_mac,
                         &dummy_rlc,
                         &dummy_pdcp,
                         &dummy_gw,
                         &dummy_eutra,
                         &dummy_sim,
                         task_sched.get_timer_handler(),
                         &dummy_stack,
                         rrc_nr_args) == SRSRAN_SUCCESS);

  TESTASSERT(rrc_nr.get_eutra_nr_capabilities(&caps) == SRSRAN_SUCCESS);
  TESTASSERT(rrc_nr.get_nr_capabilities(&caps) == SRSRAN_SUCCESS);
  return SRSRAN_SUCCESS;
}

int rrc_nr_reconfig_test()
{
  srslog::init();
  srslog::basic_logger& logger = srslog::fetch_basic_logger("RRC-NR");
  logger.set_level(srslog::basic_levels::debug);
  logger.set_hex_dump_max_size(-1);
  srsran::task_scheduler    task_sched{512, 100};
  srsran::task_sched_handle task_sched_handle(&task_sched);
  rrc_nr                    rrc_nr(task_sched_handle);

  dummy_phy     dummy_phy;
  dummy_mac     dummy_mac;
  dummy_rlc     dummy_rlc;
  dummy_pdcp    dummy_pdcp;
  dummy_gw      dummy_gw;
  dummy_eutra   dummy_eutra;
  dummy_sim     dummy_sim;
  dummy_stack   dummy_stack;
  rrc_nr_args_t rrc_nr_args;
  TESTASSERT(rrc_nr.init(&dummy_phy,
                         &dummy_mac,
                         &dummy_rlc,
                         &dummy_pdcp,
                         &dummy_gw,
                         &dummy_eutra,
                         &dummy_sim,
                         task_sched.get_timer_handler(),
                         &dummy_stack,
                         rrc_nr_args) == SRSRAN_SUCCESS);

  uint8_t nr_secondary_cell_group_cfg_r15_bytes[] = {
      0x08, 0x81, 0x19, 0x5c, 0x40, 0xb1, 0x42, 0x7e, 0x08, 0x30, 0xf3, 0x20, 0x3e, 0x00, 0x80, 0x34, 0x1e, 0x00, 0x80,
      0x02, 0xe8, 0x5b, 0x98, 0xc0, 0x06, 0x93, 0x5a, 0x40, 0x04, 0xd2, 0x6b, 0x00, 0x00, 0x00, 0x00, 0xcd, 0x8d, 0xb2,
      0x45, 0xe2, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x1b, 0x82, 0x21, 0x00, 0x00, 0x44, 0x04, 0x00, 0xd0,
      0x1a, 0xe2, 0x00, 0x00, 0x01, 0x98, 0x71, 0xb6, 0x48, 0x95, 0x00, 0x20, 0x07, 0xb7, 0x25, 0x58, 0xf0, 0x00, 0x00,
      0x13, 0x8c, 0x21, 0xb8, 0x83, 0x69, 0x92, 0xa0, 0xb8, 0x75, 0x01, 0x08, 0x1c, 0x0c, 0x00, 0x30, 0x78, 0x00, 0x03,
      0x49, 0xa9, 0xe0, 0x07, 0xb7, 0x25, 0x58, 0x00, 0x25, 0x06, 0xa0, 0x00, 0x80, 0xe0, 0x12, 0xd8, 0x0c, 0x88, 0x03,
      0x70, 0x84, 0x20, 0x00, 0x11, 0x11, 0x6d, 0x00, 0x00, 0x00, 0x12, 0x08, 0x00, 0x00, 0x83, 0xa6, 0x02, 0x66, 0xaa,
      0xe9, 0x28, 0x38, 0x00, 0x20, 0x81, 0x84, 0x0a, 0x18, 0x39, 0x38, 0x81, 0x22, 0x85, 0x8c, 0x1a, 0x38, 0x78, 0xfc,
      0x00, 0x00, 0x66, 0x02, 0x18, 0x10, 0x00, 0xcc, 0x04, 0xb0, 0x40, 0x01, 0x98, 0x0a, 0x60, 0xc0, 0x03, 0x30, 0x16,
      0xc2, 0x00, 0x06, 0x60, 0x31, 0x85, 0x00, 0x0c, 0xc0, 0x6b, 0x0c, 0x00, 0x19, 0x80, 0xe6, 0x1c, 0x00, 0x33, 0x21,
      0x40, 0x31, 0x00, 0x01, 0x72, 0x58, 0x62, 0x40, 0x02, 0xe4, 0xb2, 0xc5, 0x00, 0x05, 0xc9, 0x69, 0x8b, 0x00, 0x0b,
      0x92, 0xdb, 0x18, 0x00, 0x17, 0x25, 0xc6, 0x34, 0x00, 0x2e, 0x4b, 0xac, 0x70, 0x00, 0x5c, 0x97, 0x98, 0xf0, 0x00,
      0xcd, 0x85, 0x07, 0x95, 0xe5, 0x79, 0x43, 0x01, 0xe4, 0x07, 0x23, 0x45, 0x67, 0x89, 0x7d, 0x42, 0x10, 0x84, 0x00,
      0x0c, 0xd0, 0x1a, 0x41, 0x07, 0x82, 0xb8, 0x03, 0x04, 0x28, 0x01, 0x63, 0xff, 0x4a, 0x52, 0x63, 0x18, 0xdc, 0xa0,
      0x50, 0x00, 0x08, 0x72, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x68, 0x12, 0x00, 0x00, 0x00, 0x04, 0x8a, 0x80};

  asn1::dyn_octstring nr_secondary_cell_group_cfg_r15;
  nr_secondary_cell_group_cfg_r15.resize(sizeof(nr_secondary_cell_group_cfg_r15_bytes));
  memcpy(nr_secondary_cell_group_cfg_r15.data(),
         nr_secondary_cell_group_cfg_r15_bytes,
         sizeof(nr_secondary_cell_group_cfg_r15_bytes));

  asn1::dyn_octstring nr_radio_bearer_cfg1_r15;
  rrc_nr.rrc_reconfiguration(true, true, nr_secondary_cell_group_cfg_r15, false, 0, false, nr_radio_bearer_cfg1_r15);
  task_sched.run_pending_tasks();
  return SRSRAN_SUCCESS;
}

int main(int argc, char** argv)
{
  TESTASSERT(rrc_nr_cap_request_test() == SRSRAN_SUCCESS);
  TESTASSERT(rrc_nr_reconfig_test() == SRSRAN_SUCCESS);
  return SRSRAN_SUCCESS;
}
