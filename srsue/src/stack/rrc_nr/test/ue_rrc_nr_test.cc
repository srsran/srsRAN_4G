/**
 * Copyright 2013-2023 Software Radio Systems Limited
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
#include "srsue/hdr/stack/rrc/rrc.h"
#include "srsue/hdr/stack/rrc_nr/rrc_nr.h"

using namespace srsue;

class dummy_phy : public phy_interface_rrc_nr
{
  bool           set_config(const srsran::phy_cfg_nr_t& cfg) override { return true; }
  phy_nr_state_t get_state() override { return PHY_NR_STATE_IDLE; };
  void           reset_nr() override{};
  bool           start_cell_search(const cell_search_args_t& req) override { return false; };
  bool           start_cell_select(const cell_select_args_t& req) override { return false; };
};

class dummy_mac : public mac_interface_rrc_nr
{
  void reset() {}
  int  setup_lcid(const srsran::logical_channel_config_t& config) { return SRSRAN_SUCCESS; }
  int  set_config(const srsran::bsr_cfg_nr_t& bsr_cfg) { return SRSRAN_SUCCESS; }
  int  set_config(const srsran::sr_cfg_nr_t& sr_cfg) { return SRSRAN_SUCCESS; }
  int  set_config(const srsran::dl_harq_cfg_nr_t& dl_hrq_cfg) { return SRSRAN_SUCCESS; }
  void set_config(const srsran::rach_cfg_nr_t& rach_cfg) {}
  int  add_tag_config(const srsran::tag_cfg_nr_t& tag_cfg) { return SRSRAN_SUCCESS; }
  int  set_config(const srsran::phr_cfg_nr_t& phr_cfg) { return SRSRAN_SUCCESS; }
  int  remove_tag_config(const uint32_t tag_id) { return SRSRAN_SUCCESS; }
  void bcch_search(bool) {}

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
  void write_sdu(uint32_t lcid, srsran::unique_byte_buffer_t sdu)
  {
    last_lcid = lcid;
    last_sdu  = std::move(sdu);
  }

public:
  uint32_t                     last_lcid = 99;
  srsran::unique_byte_buffer_t last_sdu;
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

class dummy_sdap : public sdap_interface_pdcp_nr, public sdap_interface_gw_nr, public sdap_interface_rrc
{
  void write_pdu(uint32_t lcid, srsran::unique_byte_buffer_t pdu) final{};
  void write_sdu(uint32_t lcid, srsran::unique_byte_buffer_t pdu) final{};
  bool set_bearer_cfg(uint32_t lcid, const sdap_interface_rrc::bearer_cfg_t& cfg) final { return true; };
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

class dummy_nas : public nas_5g_interface_rrc_nr
{
  int      write_pdu(srsran::unique_byte_buffer_t pdu) { return SRSRAN_SUCCESS; };
  int      get_k_amf(srsran::as_key_t& k_amf) { return SRSRAN_SUCCESS; };
  uint32_t get_ul_nas_count() { return SRSRAN_SUCCESS; };
};

class dummy_sim : public usim_interface_rrc_nr
{
  void generate_nr_as_keys(srsran::as_key_t& k_amf, uint32_t count_ul, srsran::as_security_config_t* sec_cfg){};
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
  srslog::basic_logger& logger = srslog::fetch_basic_logger("RRC-NR");
  logger.set_level(srslog::basic_levels::debug);
  logger.set_hex_dump_max_size(-1);
  srsran::task_scheduler    task_sched{512, 100};
  srsran::task_sched_handle task_sched_handle(&task_sched);
  srsue::rrc_nr             rrc_nr(task_sched_handle);
  srsran::byte_buffer_t     caps;

  dummy_phy     dummy_phy;
  dummy_mac     dummy_mac;
  dummy_rlc     dummy_rlc;
  dummy_pdcp    dummy_pdcp;
  dummy_sdap    dummy_sdap;
  dummy_gw      dummy_gw;
  dummy_nas     dummy_nas;
  dummy_eutra   dummy_eutra;
  dummy_sim     dummy_sim;
  dummy_stack   dummy_stack;
  rrc_nr_args_t rrc_nr_args = {};

  rrc_nr_args.supported_bands_eutra.push_back(7);
  rrc_nr_args.supported_bands_nr.push_back(78);

  TESTASSERT(rrc_nr.init(&dummy_phy,
                         &dummy_mac,
                         &dummy_rlc,
                         &dummy_pdcp,
                         &dummy_sdap,
                         &dummy_gw,
                         &dummy_nas,
                         &dummy_eutra,
                         &dummy_sim,
                         task_sched.get_timer_handler(),
                         &dummy_stack,
                         rrc_nr_args) == SRSRAN_SUCCESS);

  TESTASSERT(rrc_nr.get_eutra_nr_capabilities(&caps) == SRSRAN_SUCCESS);
  TESTASSERT(rrc_nr.get_nr_capabilities(&caps) == SRSRAN_SUCCESS);
  return SRSRAN_SUCCESS;
}

int rrc_nsa_reconfig_tdd_test()
{
  srslog::basic_logger& logger = srslog::fetch_basic_logger("RRC-NR");
  logger.set_level(srslog::basic_levels::debug);
  logger.set_hex_dump_max_size(-1);
  srsran::task_scheduler    task_sched{512, 100};
  srsran::task_sched_handle task_sched_handle(&task_sched);
  srsue::rrc_nr             rrc_nr(task_sched_handle);

  dummy_phy     dummy_phy;
  dummy_mac     dummy_mac;
  dummy_rlc     dummy_rlc;
  dummy_pdcp    dummy_pdcp;
  dummy_sdap    dummy_sdap;
  dummy_gw      dummy_gw;
  dummy_nas     dummy_nas;
  dummy_eutra   dummy_eutra;
  dummy_sim     dummy_sim;
  dummy_stack   dummy_stack;
  rrc_nr_args_t rrc_nr_args = {};
  TESTASSERT(rrc_nr.init(&dummy_phy,
                         &dummy_mac,
                         &dummy_rlc,
                         &dummy_pdcp,
                         &dummy_sdap,
                         &dummy_gw,
                         &dummy_nas,
                         &dummy_eutra,
                         &dummy_sim,
                         task_sched.get_timer_handler(),
                         &dummy_stack,
                         rrc_nr_args) == SRSRAN_SUCCESS);

  uint8_t msg[] = {
      0x20, 0x12, 0xaa, 0x00, 0x02, 0x00, 0x80, 0x23, 0x00, 0x01, 0xfb, 0x54, 0x94, 0x10, 0x43, 0xc6, 0x40, 0x62, 0x04,
      0x40, 0x60, 0xae, 0x20, 0x58, 0xe0, 0x3e, 0xa4, 0x1d, 0x02, 0x60, 0x19, 0x00, 0x82, 0x28, 0x01, 0x64, 0x29, 0xdc,
      0x6f, 0xa3, 0x49, 0xad, 0x40, 0x02, 0x69, 0x35, 0x89, 0x00, 0x00, 0x00, 0x66, 0xc6, 0xd9, 0x22, 0x51, 0x00, 0xff,
      0x80, 0x00, 0x00, 0x00, 0x00, 0x8d, 0xc1, 0x10, 0x80, 0x01, 0x24, 0x42, 0x00, 0x68, 0x0a, 0x36, 0x00, 0x9a, 0x4d,
      0x62, 0x40, 0x00, 0x00, 0x19, 0xb8, 0xdb, 0x24, 0x48, 0x01, 0x00, 0x04, 0x17, 0x12, 0x8c, 0x78, 0x01, 0x25, 0x18,
      0x83, 0x70, 0xc6, 0xe3, 0xa2, 0x47, 0x01, 0x80, 0x22, 0x07, 0x03, 0x00, 0x10, 0x1e, 0x23, 0x00, 0xd2, 0x4b, 0x81,
      0xb5, 0x00, 0x02, 0xff, 0x00, 0x00, 0x00, 0x00, 0x01, 0x06, 0xe1, 0x10, 0x40, 0x00, 0x92, 0x22, 0x4a, 0x00, 0x00,
      0x90, 0x40, 0x00, 0x04, 0x0d, 0x3a, 0x00, 0x08, 0x02, 0x91, 0x8a, 0x92, 0x42, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x4e, 0x04, 0x08, 0x10, 0x20, 0x40, 0x81, 0x02, 0x08, 0x00, 0x00, 0x21, 0x40, 0x00, 0x23, 0x34, 0x1c,
      0x01, 0x0c, 0xc8, 0x50, 0x09, 0x08, 0x60, 0x51, 0x00, 0xab, 0x2a, 0x22, 0x24, 0x40, 0x02, 0x90, 0x5f, 0xfd, 0x29,
      0x49, 0x8c, 0x63, 0x62, 0x45, 0x6a, 0x00, 0x00, 0x01, 0x80, 0x0c, 0x00, 0x04, 0x10, 0x00, 0x71, 0x00, 0x04, 0x80,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x8a, 0x04, 0x94, 0x0b, 0xc3, 0xe0, 0x06, 0x80, 0x40, 0x00};

  asn1::cbit_ref           bref(msg, sizeof(msg));
  asn1::rrc::dl_dcch_msg_s dl_dcch_msg;

  TESTASSERT(dl_dcch_msg.unpack(bref) == asn1::SRSASN_SUCCESS);
  TESTASSERT(dl_dcch_msg.msg.type().value == dl_dcch_msg_type_c::types_opts::c1);

  dl_dcch_msg_type_c::c1_c_* c1       = &dl_dcch_msg.msg.c1();
  rrc_conn_recfg_r8_ies_s    rx_recfg = c1->rrc_conn_recfg().crit_exts.c1().rrc_conn_recfg_r8();

  const asn1::rrc::rrc_conn_recfg_v1510_ies_s rrc_conn_recfg_v1510_ies =
      rx_recfg.non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext;

  bool endc_release_and_add_r15 = false;

  asn1::rrc_nr::rrc_recfg_s rrc_nr_reconf = {};
  rrc_nr_reconf.crit_exts.set_rrc_recfg();

  TESTASSERT(rrc_conn_recfg_v1510_ies.nr_cfg_r15.type() == setup_opts::options::setup);

  endc_release_and_add_r15 = rrc_conn_recfg_v1510_ies.nr_cfg_r15.setup().endc_release_and_add_r15;

  TESTASSERT(rrc_conn_recfg_v1510_ies.nr_cfg_r15.setup().nr_secondary_cell_group_cfg_r15_present);

  asn1::cbit_ref bref0(rrc_conn_recfg_v1510_ies.nr_cfg_r15.setup().nr_secondary_cell_group_cfg_r15.data(),
                       rrc_conn_recfg_v1510_ies.nr_cfg_r15.setup().nr_secondary_cell_group_cfg_r15.size());

  asn1::rrc_nr::rrc_recfg_s secondary_cell_group_r15;
  TESTASSERT(secondary_cell_group_r15.unpack(bref0) == asn1::SRSASN_SUCCESS);
  TESTASSERT(secondary_cell_group_r15.crit_exts.rrc_recfg().secondary_cell_group.size() > 0);

  rrc_nr_reconf.crit_exts.rrc_recfg().secondary_cell_group =
      secondary_cell_group_r15.crit_exts.rrc_recfg().secondary_cell_group;

  TESTASSERT(rrc_conn_recfg_v1510_ies.sk_counter_r15_present);
  rrc_nr_reconf.crit_exts.rrc_recfg().non_crit_ext_present                                      = true;
  rrc_nr_reconf.crit_exts.rrc_recfg().non_crit_ext.non_crit_ext_present                         = true;
  rrc_nr_reconf.crit_exts.rrc_recfg().non_crit_ext.non_crit_ext.non_crit_ext_present            = true;
  rrc_nr_reconf.crit_exts.rrc_recfg().non_crit_ext.non_crit_ext.non_crit_ext.sk_counter_present = true;
  rrc_nr_reconf.crit_exts.rrc_recfg().non_crit_ext.non_crit_ext.non_crit_ext.sk_counter =
      rrc_conn_recfg_v1510_ies.sk_counter_r15;

  TESTASSERT(rrc_conn_recfg_v1510_ies.nr_radio_bearer_cfg1_r15_present);
  rrc_nr_reconf.crit_exts.rrc_recfg().radio_bearer_cfg_present = true;
  asn1::rrc_nr::radio_bearer_cfg_s radio_bearer_conf           = {};
  asn1::cbit_ref                   bref2(rrc_conn_recfg_v1510_ies.nr_radio_bearer_cfg1_r15.data(),
                       rrc_conn_recfg_v1510_ies.nr_radio_bearer_cfg1_r15.size());
  TESTASSERT(radio_bearer_conf.unpack(bref2) == asn1::SRSASN_SUCCESS);

  rrc_nr_reconf.crit_exts.rrc_recfg().radio_bearer_cfg = radio_bearer_conf;

  rrc_nr.rrc_reconfiguration(endc_release_and_add_r15, rrc_nr_reconf);

  task_sched.run_pending_tasks();
  return SRSRAN_SUCCESS;
}

int rrc_nsa_reconfig_fdd_test()
{
  srslog::basic_logger& logger = srslog::fetch_basic_logger("RRC-NR");
  logger.set_level(srslog::basic_levels::debug);
  logger.set_hex_dump_max_size(-1);
  srsran::task_scheduler    task_sched{512, 100};
  srsran::task_sched_handle task_sched_handle(&task_sched);
  srsue::rrc_nr             rrc_nr(task_sched_handle);

  dummy_phy     dummy_phy;
  dummy_mac     dummy_mac;
  dummy_rlc     dummy_rlc;
  dummy_pdcp    dummy_pdcp;
  dummy_sdap    dummy_sdap;
  dummy_gw      dummy_gw;
  dummy_nas     dummy_nas;
  dummy_eutra   dummy_eutra;
  dummy_sim     dummy_sim;
  dummy_stack   dummy_stack;
  rrc_nr_args_t rrc_nr_args = {};
  TESTASSERT(rrc_nr.init(&dummy_phy,
                         &dummy_mac,
                         &dummy_rlc,
                         &dummy_pdcp,
                         &dummy_sdap,
                         &dummy_gw,
                         &dummy_nas,
                         &dummy_eutra,
                         &dummy_sim,
                         task_sched.get_timer_handler(),
                         &dummy_stack,
                         rrc_nr_args) == SRSRAN_SUCCESS);

  uint8_t msg[] = {
      0x20, 0x12, 0xaa, 0x00, 0x02, 0x00, 0x80, 0x23, 0x00, 0x01, 0xfb, 0x54, 0x94, 0x10, 0x43, 0xc6, 0x40, 0x62, 0x04,
      0x40, 0x60, 0xae, 0x20, 0x58, 0xe0, 0x3e, 0xa4, 0x1d, 0x02, 0x60, 0x19, 0x00, 0x82, 0x28, 0x01, 0x64, 0x29, 0xdc,
      0x6f, 0xa3, 0x49, 0xad, 0x40, 0x02, 0x69, 0x35, 0x89, 0x00, 0x00, 0x00, 0x66, 0xc6, 0xd9, 0x22, 0x51, 0x00, 0xff,
      0x80, 0x00, 0x00, 0x00, 0x00, 0x8d, 0xc1, 0x10, 0x80, 0x01, 0x24, 0x42, 0x00, 0x68, 0x0a, 0x36, 0x00, 0x9a, 0x4d,
      0x62, 0x40, 0x00, 0x00, 0x19, 0xb8, 0xdb, 0x24, 0x48, 0x01, 0x00, 0x04, 0x17, 0x12, 0x8c, 0x78, 0x01, 0x25, 0x18,
      0x83, 0x70, 0xc6, 0xe3, 0xa2, 0x47, 0x01, 0x80, 0x22, 0x07, 0x03, 0x00, 0x10, 0x1e, 0x23, 0x00, 0xd2, 0x4b, 0x81,
      0xb5, 0x00, 0x02, 0xff, 0x00, 0x00, 0x00, 0x00, 0x01, 0x06, 0xe1, 0x10, 0x40, 0x00, 0x92, 0x22, 0x4a, 0x00, 0x00,
      0x90, 0x40, 0x00, 0x04, 0x0d, 0x3a, 0x00, 0x08, 0x02, 0x91, 0x8a, 0x92, 0x42, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x4e, 0x04, 0x08, 0x10, 0x20, 0x40, 0x81, 0x02, 0x08, 0x00, 0x00, 0x21, 0x40, 0x00, 0x23, 0x34, 0x1c,
      0x01, 0x0c, 0xc8, 0x50, 0x09, 0x08, 0x60, 0x51, 0x00, 0xab, 0x2a, 0x22, 0x24, 0x40, 0x02, 0x90, 0x5f, 0xfd, 0x29,
      0x49, 0x8c, 0x63, 0x62, 0x45, 0x6a, 0x00, 0x00, 0x01, 0x80, 0x0c, 0x00, 0x04, 0x10, 0x00, 0x71, 0x00, 0x04, 0x80,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x8a, 0x04, 0x94, 0x0b, 0xc3, 0xe0, 0x06, 0x80, 0x40, 0x00};

  asn1::cbit_ref           bref(msg, sizeof(msg));
  asn1::rrc::dl_dcch_msg_s dl_dcch_msg;

  TESTASSERT(dl_dcch_msg.unpack(bref) == asn1::SRSASN_SUCCESS);
  TESTASSERT(dl_dcch_msg.msg.type().value == dl_dcch_msg_type_c::types_opts::c1);

  dl_dcch_msg_type_c::c1_c_* c1       = &dl_dcch_msg.msg.c1();
  rrc_conn_recfg_r8_ies_s    rx_recfg = c1->rrc_conn_recfg().crit_exts.c1().rrc_conn_recfg_r8();

  const asn1::rrc::rrc_conn_recfg_v1510_ies_s rrc_conn_recfg_v1510_ies =
      rx_recfg.non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext.non_crit_ext;

  bool endc_release_and_add_r15 = false;

  asn1::rrc_nr::rrc_recfg_s rrc_nr_reconf = {};
  rrc_nr_reconf.crit_exts.set_rrc_recfg();

  TESTASSERT(rrc_conn_recfg_v1510_ies.nr_cfg_r15.type() == setup_opts::options::setup);

  endc_release_and_add_r15 = rrc_conn_recfg_v1510_ies.nr_cfg_r15.setup().endc_release_and_add_r15;

  TESTASSERT(rrc_conn_recfg_v1510_ies.nr_cfg_r15.setup().nr_secondary_cell_group_cfg_r15_present);

  asn1::cbit_ref bref0(rrc_conn_recfg_v1510_ies.nr_cfg_r15.setup().nr_secondary_cell_group_cfg_r15.data(),
                       rrc_conn_recfg_v1510_ies.nr_cfg_r15.setup().nr_secondary_cell_group_cfg_r15.size());

  asn1::rrc_nr::rrc_recfg_s secondary_cell_group_r15;
  TESTASSERT(secondary_cell_group_r15.unpack(bref0) == asn1::SRSASN_SUCCESS);
  TESTASSERT(secondary_cell_group_r15.crit_exts.rrc_recfg().secondary_cell_group.size() > 0);

  rrc_nr_reconf.crit_exts.rrc_recfg().secondary_cell_group =
      secondary_cell_group_r15.crit_exts.rrc_recfg().secondary_cell_group;

  TESTASSERT(rrc_conn_recfg_v1510_ies.sk_counter_r15_present);
  rrc_nr_reconf.crit_exts.rrc_recfg().non_crit_ext_present                                      = true;
  rrc_nr_reconf.crit_exts.rrc_recfg().non_crit_ext.non_crit_ext_present                         = true;
  rrc_nr_reconf.crit_exts.rrc_recfg().non_crit_ext.non_crit_ext.non_crit_ext_present            = true;
  rrc_nr_reconf.crit_exts.rrc_recfg().non_crit_ext.non_crit_ext.non_crit_ext.sk_counter_present = true;
  rrc_nr_reconf.crit_exts.rrc_recfg().non_crit_ext.non_crit_ext.non_crit_ext.sk_counter =
      rrc_conn_recfg_v1510_ies.sk_counter_r15;

  TESTASSERT(rrc_conn_recfg_v1510_ies.nr_radio_bearer_cfg1_r15_present);
  rrc_nr_reconf.crit_exts.rrc_recfg().radio_bearer_cfg_present = true;
  asn1::rrc_nr::radio_bearer_cfg_s radio_bearer_conf           = {};
  asn1::cbit_ref                   bref2(rrc_conn_recfg_v1510_ies.nr_radio_bearer_cfg1_r15.data(),
                       rrc_conn_recfg_v1510_ies.nr_radio_bearer_cfg1_r15.size());
  TESTASSERT(radio_bearer_conf.unpack(bref2) == asn1::SRSASN_SUCCESS);

  rrc_nr_reconf.crit_exts.rrc_recfg().radio_bearer_cfg = radio_bearer_conf;

  rrc_nr.rrc_reconfiguration(endc_release_and_add_r15, rrc_nr_reconf);

  task_sched.run_pending_tasks();
  return SRSRAN_SUCCESS;
}

int rrc_nr_setup_request_test()
{
  srslog::basic_logger& logger = srslog::fetch_basic_logger("RRC-NR");
  logger.set_level(srslog::basic_levels::debug);
  logger.set_hex_dump_max_size(-1);
  srsran::task_scheduler    task_sched{512, 100};
  srsran::task_sched_handle task_sched_handle(&task_sched);
  srsue::rrc_nr             rrc_nr(task_sched_handle);
  srsran::byte_buffer_t     caps;

  dummy_phy     dummy_phy;
  dummy_mac     dummy_mac;
  dummy_rlc     dummy_rlc;
  dummy_pdcp    dummy_pdcp;
  dummy_sdap    dummy_sdap;
  dummy_gw      dummy_gw;
  dummy_nas     dummy_nas;
  dummy_eutra   dummy_eutra;
  dummy_sim     dummy_sim;
  dummy_stack   dummy_stack;
  rrc_nr_args_t rrc_nr_args = {};

  rrc_nr_args.supported_bands_nr.push_back(78);

  TESTASSERT(rrc_nr.init(&dummy_phy,
                         &dummy_mac,
                         &dummy_rlc,
                         &dummy_pdcp,
                         &dummy_sdap,
                         &dummy_gw,
                         &dummy_nas,
                         &dummy_eutra,
                         &dummy_sim,
                         task_sched.get_timer_handler(),
                         &dummy_stack,
                         rrc_nr_args) == SRSRAN_SUCCESS);
  rrc_nr.connection_request(srsran::nr_establishment_cause_t::mt_Access, nullptr);
  task_sched.run_pending_tasks();

  TESTASSERT(dummy_rlc.last_lcid == 0);         // SRB0 transmission
  TESTASSERT(dummy_rlc.last_sdu->N_bytes == 6); // RRC Setup Request is 6 Bytes long

  return SRSRAN_SUCCESS;
}

int rrc_nr_sib1_decoding_test()
{
  srslog::basic_logger& logger = srslog::fetch_basic_logger("RRC-NR");
  logger.set_level(srslog::basic_levels::debug);
  logger.set_hex_dump_max_size(-1);
  srsran::task_scheduler    task_sched{512, 100};
  srsran::task_sched_handle task_sched_handle(&task_sched);
  srsue::rrc_nr             rrc_nr(task_sched_handle);

  dummy_phy     dummy_phy;
  dummy_mac     dummy_mac;
  dummy_rlc     dummy_rlc;
  dummy_pdcp    dummy_pdcp;
  dummy_sdap    dummy_sdap;
  dummy_gw      dummy_gw;
  dummy_nas     dummy_nas;
  dummy_eutra   dummy_eutra;
  dummy_sim     dummy_sim;
  dummy_stack   dummy_stack;
  rrc_nr_args_t rrc_nr_args = {};
  TESTASSERT(rrc_nr.init(&dummy_phy,
                         &dummy_mac,
                         &dummy_rlc,
                         &dummy_pdcp,
                         &dummy_sdap,
                         &dummy_gw,
                         &dummy_nas,
                         &dummy_eutra,
                         &dummy_sim,
                         task_sched.get_timer_handler(),
                         &dummy_stack,
                         rrc_nr_args) == SRSRAN_SUCCESS);

  uint8_t msg[] = {0x74, 0x81, 0x01, 0x70, 0x10, 0x40, 0x04, 0x02, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x33, 0x60, 0x38,
                   0x05, 0x01, 0x00, 0x40, 0x1a, 0x00, 0x00, 0x06, 0x6c, 0x6d, 0x92, 0x21, 0xf3, 0x70, 0x40, 0x20,
                   0x00, 0x00, 0x80, 0x80, 0x00, 0x41, 0x06, 0x80, 0xa0, 0x90, 0x9c, 0x20, 0x08, 0x55, 0x19, 0x40,
                   0x00, 0x00, 0x33, 0xa1, 0xc6, 0xd9, 0x22, 0x40, 0x00, 0x00, 0x20, 0xb8, 0x94, 0x63, 0xc0, 0x09,
                   0x28, 0x44, 0x1b, 0x7e, 0xad, 0x8e, 0x1d, 0x00, 0x9e, 0x2d, 0xa3, 0x0a};

  srsran::unique_byte_buffer_t pdu = srsran::make_byte_buffer();
  memcpy(pdu->msg, msg, sizeof(msg));
  pdu->N_bytes = sizeof(msg);

  rrc_nr.write_pdu_bcch_dlsch(std::move(pdu));
  task_sched.run_pending_tasks();

  return SRSRAN_SUCCESS;
}

int rrc_nr_setup_test()
{
  srslog::basic_logger& logger = srslog::fetch_basic_logger("RRC-NR");
  logger.set_level(srslog::basic_levels::debug);
  logger.set_hex_dump_max_size(-1);
  srsran::task_scheduler    task_sched{512, 100};
  srsran::task_sched_handle task_sched_handle(&task_sched);
  srsue::rrc_nr             rrc_nr(task_sched_handle);

  dummy_phy     dummy_phy;
  dummy_mac     dummy_mac;
  dummy_rlc     dummy_rlc;
  dummy_pdcp    dummy_pdcp;
  dummy_sdap    dummy_sdap;
  dummy_gw      dummy_gw;
  dummy_nas     dummy_nas;
  dummy_eutra   dummy_eutra;
  dummy_sim     dummy_sim;
  dummy_stack   dummy_stack;
  rrc_nr_args_t rrc_nr_args = {};
  TESTASSERT(rrc_nr.init(&dummy_phy,
                         &dummy_mac,
                         &dummy_rlc,
                         &dummy_pdcp,
                         &dummy_sdap,
                         &dummy_gw,
                         &dummy_nas,
                         &dummy_eutra,
                         &dummy_sim,
                         task_sched.get_timer_handler(),
                         &dummy_stack,
                         rrc_nr_args) == SRSRAN_SUCCESS);

  uint8_t msg[] = {0x20, 0x40, 0x04, 0x05, 0x9a, 0xe0, 0x05, 0x80, 0x08, 0x8b, 0xd7, 0x63, 0x80, 0x83, 0x0f, 0x00, 0x03,
                   0xa0, 0x10, 0x45, 0x41, 0xc2, 0x0a, 0x20, 0x92, 0x40, 0x0c, 0xa8, 0x00, 0x17, 0xf8, 0x00, 0x00, 0x00,
                   0x00, 0x08, 0x37, 0x08, 0x82, 0x00, 0x04, 0x91, 0x12, 0x50, 0x00, 0x04, 0x82, 0x00, 0x00, 0x20, 0x69,
                   0x84, 0x0c, 0x55, 0x92, 0x10, 0x70, 0x00, 0x41, 0x03, 0x08, 0x14, 0x30, 0x72, 0x71, 0x02, 0x45, 0x0b,
                   0x18, 0x34, 0x70, 0xf2, 0x38, 0x01, 0x98, 0x00, 0x85, 0x00, 0xc0, 0x8c, 0xc0, 0x05, 0x28, 0x06, 0x08,
                   0x66, 0x00, 0x31, 0x40, 0x30, 0x63, 0x30, 0x01, 0x0a, 0x03, 0x84, 0x19, 0x80, 0x0a, 0x50, 0x1c, 0x28,
                   0xcc, 0x00, 0x62, 0x80, 0xe1, 0x86, 0x60, 0x02, 0x14, 0x0b, 0x0e, 0x33, 0x00, 0x14, 0xa0, 0x58, 0x80,
                   0x08, 0xc9, 0x04, 0x31, 0x20, 0x11, 0x92, 0x09, 0x62, 0x80, 0x23, 0x24, 0x14, 0xc5, 0x80, 0x46, 0x48,
                   0x2d, 0x8c, 0x00, 0x8c, 0x90, 0x63, 0x1a, 0x01, 0x19, 0x20, 0xd6, 0x38, 0x02, 0x32, 0x41, 0xcc, 0x78,
                   0xc8, 0x02, 0x82, 0x19, 0x01, 0x98, 0x00, 0xc5, 0x02, 0xc8, 0x8c, 0x80, 0x28, 0x25, 0x02, 0x42, 0x18,
                   0x14, 0x40, 0x20, 0x91, 0x00, 0x0a, 0x41, 0x7f, 0xf4, 0xa5, 0x26, 0x31, 0x8d, 0x80};

  srsran::unique_byte_buffer_t pdu = srsran::make_byte_buffer();
  memcpy(pdu->msg, msg, sizeof(msg));
  pdu->N_bytes = sizeof(msg);

  rrc_nr.write_pdu(0, std::move(pdu));
  task_sched.run_pending_tasks();

  return SRSRAN_SUCCESS;
}

int rrc_nr_reconfig_test()
{
  srslog::basic_logger& logger = srslog::fetch_basic_logger("RRC-NR");
  logger.set_level(srslog::basic_levels::debug);
  logger.set_hex_dump_max_size(-1);
  srsran::task_scheduler    task_sched{512, 100};
  srsran::task_sched_handle task_sched_handle(&task_sched);
  srsue::rrc_nr             rrc_nr(task_sched_handle);

  dummy_phy     dummy_phy;
  dummy_mac     dummy_mac;
  dummy_rlc     dummy_rlc;
  dummy_pdcp    dummy_pdcp;
  dummy_sdap    dummy_sdap;
  dummy_gw      dummy_gw;
  dummy_nas     dummy_nas;
  dummy_eutra   dummy_eutra;
  dummy_sim     dummy_sim;
  dummy_stack   dummy_stack;
  rrc_nr_args_t rrc_nr_args = {};
  TESTASSERT(rrc_nr.init(&dummy_phy,
                         &dummy_mac,
                         &dummy_rlc,
                         &dummy_pdcp,
                         &dummy_sdap,
                         &dummy_gw,
                         &dummy_nas,
                         &dummy_eutra,
                         &dummy_sim,
                         task_sched.get_timer_handler(),
                         &dummy_stack,
                         rrc_nr_args) == SRSRAN_SUCCESS);

  uint8_t msg[] = {0x04, 0x8a, 0x80, 0x40, 0x9a, 0x01, 0xe0, 0x02, 0x05, 0xe1, 0xf0, 0x05, 0x00, 0x9a, 0x00, 0x15, 0x84,
                   0x88, 0x8b, 0xd7, 0x63, 0x80, 0x83, 0x2f, 0x00, 0x05, 0x8e, 0x03, 0xea, 0x41, 0xd0, 0x23, 0x00, 0x20,
                   0x25, 0x5f, 0x80, 0xa2, 0xef, 0x22, 0xc8, 0x40, 0xdf, 0x80, 0x1a, 0x00, 0x40, 0x21, 0x8b, 0x80, 0x40,
                   0x70, 0x84, 0xc0, 0x02, 0x40, 0x40, 0x01, 0x8c, 0x4c, 0x40, 0x40, 0x7f, 0xc0, 0x41, 0x82, 0xc0, 0x00,
                   0x42, 0xc0, 0x00, 0x4a, 0x43, 0x40, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x82, 0x8b, 0x40,
                   0x01, 0x88, 0x80, 0x40, 0x5e, 0x40, 0x01, 0x80, 0x48, 0x10, 0x40, 0x40, 0x42, 0x5e, 0xc0, 0x12, 0x20,
                   0x20, 0x08, 0x44, 0x00, 0x80, 0x00, 0x04, 0x20, 0x41, 0x82, 0x02, 0x02, 0x02, 0x20, 0xc1, 0x82, 0x02,
                   0x01, 0x01, 0x00, 0x00, 0xc4, 0x08, 0x00, 0x52, 0x18, 0x12, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                   0x00, 0x00, 0x22, 0x22, 0x00, 0x00, 0xc4, 0x08, 0x00, 0x52, 0x18, 0x12, 0x18, 0x00, 0x00, 0x00, 0x00,
                   0x00, 0x00, 0x00, 0x00, 0x22, 0x11, 0x00, 0x03, 0x41, 0x02, 0x02, 0x02, 0x02, 0x00, 0x03, 0x41, 0x02,
                   0x02, 0x01, 0x01, 0x09, 0x41, 0xc1, 0x9c, 0xdc, 0x9c, 0xd8, 0x5c, 0x1b, 0x84, 0x80, 0x40};

  srsran::unique_byte_buffer_t pdu = srsran::make_byte_buffer();
  memcpy(pdu->msg, msg, sizeof(msg));
  pdu->N_bytes = sizeof(msg);

  rrc_nr.write_pdu(1, std::move(pdu));
  task_sched.run_pending_tasks();

  return SRSRAN_SUCCESS;
}

int main(int argc, char** argv)
{
  srslog::init();

  TESTASSERT(rrc_nr_cap_request_test() == SRSRAN_SUCCESS);
  TESTASSERT(rrc_nsa_reconfig_tdd_test() == SRSRAN_SUCCESS);
  TESTASSERT(rrc_nsa_reconfig_fdd_test() == SRSRAN_SUCCESS);
  TESTASSERT(rrc_nr_setup_request_test() == SRSRAN_SUCCESS);
  TESTASSERT(rrc_nr_sib1_decoding_test() == SRSRAN_SUCCESS);
  TESTASSERT(rrc_nr_setup_test() == SRSRAN_SUCCESS);
  TESTASSERT(rrc_nr_reconfig_test() == SRSRAN_SUCCESS);

  return SRSRAN_SUCCESS;
}
