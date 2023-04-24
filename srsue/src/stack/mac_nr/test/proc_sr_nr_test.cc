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
#include "srsran/common/buffer_pool.h"
#include "srsran/common/common.h"
#include "srsran/common/test_common.h"
#include "srsue/hdr/stack/mac_nr/proc_sr_nr.h"
#include "srsran/interfaces/ue_rrc_interfaces.h"

using namespace srsue;

class dummy_phy : public phy_interface_mac_nr
{
public:
  dummy_phy() {}
  void send_prach(const uint32_t prach_occasion_,
                  const int      preamble_index_,
                  const float    preamble_received_target_power_,
                  const float    ta_base_sec_ = 0.0f) override
  {
    prach_occasion                 = prach_occasion_;
    preamble_index                 = preamble_index_;
    preamble_received_target_power = preamble_received_target_power_;
  }
  int set_rar_grant(uint32_t rar_slot_idx,
                    std::array<uint8_t, SRSRAN_RAR_UL_GRANT_NBITS>,
                    uint16_t           rnti,
                    srsran_rnti_type_t rnti_type) override
  {
    return 0;
  }

  void get_last_send_prach(uint32_t* prach_occasion_, uint32_t* preamble_index_, int* preamble_received_target_power_)
  {
    *prach_occasion_                 = prach_occasion;
    *preamble_index_                 = preamble_index;
    *preamble_received_target_power_ = preamble_received_target_power;
  }
  bool has_valid_sr_resource(uint32_t sr_id) override { return false; }
  void clear_pending_grants() override {}

  void set_timeadv_rar(uint32_t tti, uint32_t ta_cmd) final{};
  void set_timeadv(uint32_t tti, uint32_t ta_cmd) final{};

private:
  uint32_t prach_occasion                 = 0;
  uint32_t preamble_index                 = 0;
  int      preamble_received_target_power = 0;
};

class dummy_rrc : public rrc_interface_mac
{
  void ra_completed() {}
  void ra_problem() {}
  void release_pucch_srs() {}
};

class dummy_mac : public mac_interface_sr_nr
{
public:
  void start_ra() { ra_started = true; }
  bool check_ra_started() { return ra_started; }
  void reset_ra_started() { ra_started = false; }

private:
  bool ra_started = false;
};

int proc_sr_basic_test()
{
  proc_sr_nr proc_sr_nr(srslog::fetch_basic_logger("MAC"));
  dummy_rrc dummy_rrc; 
  dummy_phy dummy_phy;
  dummy_mac dummy_mac; 
  
  srsran::sr_cfg_nr_t cfg; 
  cfg.enabled = true;  
  cfg.num_items = 1; 
  cfg.item[0].prohibit_timer = 0; 
  cfg.item[0].sched_request_id = 0; 
  cfg.item[0].trans_max = 64; 
  proc_sr_nr.init(&dummy_mac, &dummy_phy, &dummy_rrc);
  proc_sr_nr.set_config(cfg);
  proc_sr_nr.start();

  proc_sr_nr.step(0);
  TESTASSERT(dummy_mac.check_ra_started() == true);

  return SRSRAN_SUCCESS;
}

int main()
{
  srslog::init();

  auto& mac_logger = srslog::fetch_basic_logger("MAC");
  mac_logger.set_level(srslog::basic_levels::debug);
  mac_logger.set_hex_dump_max_size(-1);
  TESTASSERT(proc_sr_basic_test() == SRSRAN_SUCCESS);
  return SRSRAN_SUCCESS;
}