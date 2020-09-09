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

#include "srsenb/hdr/phy/txrx.h"
#include "srslte/asn1/rrc_asn1.h"
#include "srslte/common/log.h"
#include "srslte/common/threads.h"
#include "srslte/phy/channel/channel.h"
#include <sstream>

#include <assert.h>

#define Error(fmt, ...)                                                                                                \
  if (SRSLTE_DEBUG_ENABLED)                                                                                            \
  log_h->error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...)                                                                                              \
  if (SRSLTE_DEBUG_ENABLED)                                                                                            \
  log_h->warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...)                                                                                                 \
  if (SRSLTE_DEBUG_ENABLED)                                                                                            \
  log_h->info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)                                                                                                \
  if (SRSLTE_DEBUG_ENABLED)                                                                                            \
  log_h->debug(fmt, ##__VA_ARGS__)

using namespace std;
using namespace asn1::rrc;

namespace srsenb {

void phy_common::reset()
{
  for (auto& q : ul_grants) {
    for (auto& g : q) {
      g = {};
    }
  }
}

bool phy_common::init(const phy_cell_cfg_list_t&   cell_list_,
                      srslte::radio_interface_phy* radio_h_,
                      stack_interface_phy_lte*     stack_)
{
  radio     = radio_h_;
  stack     = stack_;
  cell_list = cell_list_;

  pthread_mutex_init(&mtch_mutex, nullptr);
  pthread_cond_init(&mtch_cvar, nullptr);

  // Instantiate DL channel emulator
  if (params.dl_channel_args.enable) {
    dl_channel = srslte::channel_ptr(new srslte::channel(params.dl_channel_args, get_nof_rf_channels()));
    dl_channel->set_srate((uint32_t)srslte_sampling_freq_hz(cell_list[0].cell.nof_prb));
    dl_channel->set_signal_power_dBfs(srslte_enb_dl_get_maximum_signal_power_dBfs(cell_list[0].cell.nof_prb));
  }

  // Create grants
  for (auto& q : ul_grants) {
    q.resize(cell_list.size());
  }

  // Set UE PHY data-base stack and configuration
  ue_db.init(stack, params, cell_list);

  reset();
  return true;
}

void phy_common::stop()
{
  semaphore.wait_all();
}

void phy_common::clear_grants(uint16_t rnti)
{
  std::lock_guard<std::mutex> lock(grant_mutex);

  // remove any pending dci for each subframe
  for (auto& list : ul_grants) {
    for (auto& q : list) {
      for (uint32_t j = 0; j < q.nof_grants; j++) {
        if (q.pusch[j].dci.rnti == rnti) {
          q.pusch[j].dci.rnti = 0;
        }
      }
    }
  }
}

const stack_interface_phy_lte::ul_sched_list_t& phy_common::get_ul_grants(uint32_t tti)
{
  std::lock_guard<std::mutex> lock(grant_mutex);
  return ul_grants[tti];
}

void phy_common::set_ul_grants(uint32_t tti, const stack_interface_phy_lte::ul_sched_list_t& ul_grant_list)
{
  std::lock_guard<std::mutex> lock(grant_mutex);
  ul_grants[tti] = ul_grant_list;
}

/* The transmission of UL subframes must be in sequence. The correct sequence is guaranteed by a chain of N semaphores,
 * one per TTI%nof_workers. Each threads waits for the semaphore for the current thread and after transmission allows
 * next TTI to be transmitted
 *
 * Each worker uses this function to indicate that all processing is done and data is ready for transmission or
 * there is no transmission at all (tx_enable). In that case, the end of burst message will be sent to the radio
 */
void phy_common::worker_end(void* tx_sem_id, srslte::rf_buffer_t& buffer, srslte::rf_timestamp_t& tx_time)
{
  // Wait for the green light to transmit in the current TTI
  semaphore.wait(tx_sem_id);

  // Run DL channel emulator if created
  if (dl_channel) {
    dl_channel->run(buffer.to_cf_t(), buffer.to_cf_t(), buffer.get_nof_samples(), tx_time.get(0));
  }

  // Always transmit on single radio
  radio->tx(buffer, tx_time);

  // Allow next TTI to transmit
  semaphore.release();
}

void phy_common::set_mch_period_stop(uint32_t stop)
{
  pthread_mutex_lock(&mtch_mutex);
  have_mtch_stop  = true;
  mch_period_stop = stop;
  pthread_cond_signal(&mtch_cvar);
  pthread_mutex_unlock(&mtch_mutex);
}

void phy_common::configure_mbsfn(phy_interface_stack_lte::phy_cfg_mbsfn_t* cfg)
{
  mbsfn = *cfg;

  build_mch_table();
  build_mcch_table();
  sib13_configured = true;
  mcch_configured  = true;
}

void phy_common::build_mch_table()
{
  // First reset tables
  ZERO_OBJECT(mcch_table);

  // 40 element table represents 4 frames (40 subframes)
  uint32_t nof_sfs = 0;
  if (mbsfn.mbsfn_subfr_cnfg.sf_alloc.type().value == mbsfn_sf_cfg_s::sf_alloc_c_::types::one_frame) {
    generate_mch_table(&mch_table[0], (uint32_t)mbsfn.mbsfn_subfr_cnfg.sf_alloc.one_frame().to_number(), 1);
    nof_sfs = 10;
  } else if (mbsfn.mbsfn_subfr_cnfg.sf_alloc.type().value == mbsfn_sf_cfg_s::sf_alloc_c_::types::four_frames) {
    generate_mch_table(&mch_table[0], (uint32_t)mbsfn.mbsfn_subfr_cnfg.sf_alloc.four_frames().to_number(), 4);
    nof_sfs = 40;
  } else {
    fprintf(stderr, "No valid SF alloc\n");
  }
  // Debug
  std::stringstream ss;
  ss << "|";
  for (uint32_t j = 0; j < 40; j++) {
    ss << (int)mch_table[j] << "|";
  }

  stack->set_sched_dl_tti_mask(mch_table, nof_sfs);
}

void phy_common::build_mcch_table()
{
  ZERO_OBJECT(mcch_table);

  generate_mcch_table(mcch_table,
                      static_cast<uint32_t>(mbsfn.mbsfn_area_info.mcch_cfg_r9.sf_alloc_info_r9.to_number()));

  std::stringstream ss;
  ss << "|";
  for (uint32_t j = 0; j < 10; j++) {
    ss << (int)mcch_table[j] << "|";
  }
}

bool phy_common::is_mcch_subframe(srslte_mbsfn_cfg_t* cfg, uint32_t phy_tti)
{
  uint32_t sfn; // System Frame Number
  uint8_t  sf;  // Subframe
  uint8_t  offset;
  uint8_t  period;

  sfn = phy_tti / 10;
  sf  = phy_tti % 10;

  if (sib13_configured) {
    mbsfn_area_info_r9_s* area_info = &mbsfn.mbsfn_area_info;

    offset = area_info->mcch_cfg_r9.mcch_offset_r9;
    period = area_info->mcch_cfg_r9.mcch_repeat_period_r9.to_number();

    if ((sfn % period == offset) && mcch_table[sf] > 0) {
      cfg->mbsfn_area_id           = area_info->mbsfn_area_id_r9;
      cfg->non_mbsfn_region_length = area_info->non_mbsfn_region_len.to_number();
      cfg->mbsfn_mcs               = area_info->mcch_cfg_r9.sig_mcs_r9.to_number();
      cfg->enable                  = true;
      cfg->is_mcch                 = true;
      have_mtch_stop               = false;
      return true;
    }
  }
  return false;
}

bool phy_common::is_mch_subframe(srslte_mbsfn_cfg_t* cfg, uint32_t phy_tti)
{
  uint32_t sfn; // System Frame Number
  uint8_t  sf;  // Subframe
  uint8_t  offset;
  uint8_t  period;

  sfn = phy_tti / 10;
  sf  = phy_tti % 10;

  // Set some defaults
  cfg->mbsfn_area_id           = 0;
  cfg->non_mbsfn_region_length = 1;
  cfg->mbsfn_mcs               = 2;
  cfg->enable                  = false;
  cfg->is_mcch                 = false;
  // Check for MCCH
  if (is_mcch_subframe(cfg, phy_tti)) {
    return true;
  }
  if (not mcch_configured) {
    return false;
  }

  // Not MCCH, check for MCH
  mbsfn_sf_cfg_s*       subfr_cnfg = &mbsfn.mbsfn_subfr_cnfg;
  mbsfn_area_info_r9_s* area_info  = &mbsfn.mbsfn_area_info;

  offset = subfr_cnfg->radioframe_alloc_offset;
  period = subfr_cnfg->radioframe_alloc_period.to_number();

  if (subfr_cnfg->sf_alloc.type() == mbsfn_sf_cfg_s::sf_alloc_c_::types::one_frame) {
    if ((sfn % period == offset) && (mch_table[sf] > 0)) {
      if (sib13_configured) {
        cfg->mbsfn_area_id           = area_info->mbsfn_area_id_r9;
        cfg->non_mbsfn_region_length = area_info->non_mbsfn_region_len.to_number();
        if (mcch_configured) {
          // Iterate through PMCH configs to see which one applies in the current frame
          mbsfn_area_cfg_r9_s* area_r9 = &mbsfn.mcch.msg.c1().mbsfn_area_cfg_r9();

          uint32_t frame_alloc_idx = sfn % area_r9->common_sf_alloc_period_r9.to_number();
          uint32_t mbsfn_per_frame = area_r9->pmch_info_list_r9[0].pmch_cfg_r9.sf_alloc_end_r9 /
                                     +area_r9->pmch_info_list_r9[0].pmch_cfg_r9.mch_sched_period_r9.to_number();
          uint32_t sf_alloc_idx = frame_alloc_idx * mbsfn_per_frame + ((sf < 4) ? sf - 1 : sf - 3);
          while (!have_mtch_stop) {
            pthread_cond_wait(&mtch_cvar, &mtch_mutex);
          }

          for (uint32_t i = 0; i < area_r9->pmch_info_list_r9.size(); i++) {
            if (sf_alloc_idx <= mch_period_stop) {
              cfg->mbsfn_mcs = mbsfn.mcch.msg.c1().mbsfn_area_cfg_r9().pmch_info_list_r9[i].pmch_cfg_r9.data_mcs_r9;
              cfg->enable    = true;
            }
          }
        }
      }
      return true;
    }
  } else if (subfr_cnfg->sf_alloc.type() == mbsfn_sf_cfg_s::sf_alloc_c_::types::four_frames) {
    uint8_t idx = sfn % period;
    if ((idx >= offset) && (idx < offset + 4)) {
      if (mch_table[(idx * 10) + sf] > 0) {
        if (sib13_configured) {
          cfg->mbsfn_area_id           = area_info->mbsfn_area_id_r9;
          cfg->non_mbsfn_region_length = area_info->non_mbsfn_region_len.to_number();
          // TODO: check for MCCH configuration, set MCS and decode
        }
        return true;
      }
    }
  }

  return false;
}

bool phy_common::is_mbsfn_sf(srslte_mbsfn_cfg_t* cfg, uint32_t phy_tti)
{
  return is_mch_subframe(cfg, phy_tti);
}
} // namespace srsenb
