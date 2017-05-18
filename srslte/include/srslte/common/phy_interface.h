/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

/******************************************************************************
 *  File:         phy_interface.h
 *  Description:  PHY layer interfaces provided to other layers
 *  Reference:
 *****************************************************************************/

#ifndef PHY_INTERFACE_H
#define PHY_INTERFACE_H

#include <stdint.h>
#include <string>
#include "srslte/srslte.h"

#include "liblte_rrc.h"

namespace srsue {
  
  
typedef struct {
  bool ul_pwr_ctrl_en; 
  float prach_gain;
  int pdsch_max_its;
  bool attach_enable_64qam; 
  int nof_phy_threads;
  
  int worker_cpu_mask;
  int sync_cpu_affinity;
  
  uint32_t nof_rx_ant;   
  std::string equalizer_mode; 
  int cqi_max; 
  int cqi_fixed; 
  float snr_ema_coeff; 
  std::string snr_estim_alg; 
  bool cfo_integer_enabled; 
  float cfo_correct_tol_hz; 
  int time_correct_period; 
  bool sfo_correct_disable; 
  std::string sss_algorithm; 
  float estimator_fil_w;   
  bool rssi_sensor_enabled;
} phy_args_t; 
  
/* Interface MAC -> PHY */
class phy_interface_mac
{
public:
  /* Configure PRACH using parameters written by RRC */
  virtual void configure_prach_params() = 0;
  
  /* Start synchronization with strongest cell in the current carrier frequency */
  virtual void sync_start() = 0; 
  virtual void sync_stop() = 0;
  
  /* Sets a C-RNTI allowing the PHY to pregenerate signals if necessary */
  virtual void set_crnti(uint16_t rnti) = 0; 
  
  virtual void prach_send(uint32_t preamble_idx, int allowed_subframe, float target_power_dbm) = 0;  
  virtual int  prach_tx_tti() = 0; 
  
  /* Indicates the transmission of a SR signal in the next opportunity */
  virtual void sr_send() = 0;  
  virtual int  sr_last_tx_tti() = 0; 
  
  /* Time advance commands */
  virtual void set_timeadv_rar(uint32_t ta_cmd) = 0;
  virtual void set_timeadv(uint32_t ta_cmd) = 0;
  
  /* Sets RAR grant payload */
  virtual void set_rar_grant(uint32_t tti, uint8_t grant_payload[SRSLTE_RAR_GRANT_LEN]) = 0; 

  /* Instruct the PHY to decode PDCCH with the CRC scrambled with given RNTI */
  virtual void pdcch_ul_search(srslte_rnti_type_t rnti_type, uint16_t rnti, int tti_start = -1, int tti_end = -1) = 0;
  virtual void pdcch_dl_search(srslte_rnti_type_t rnti_type, uint16_t rnti, int tti_start = -1, int tti_end = -1) = 0;
  virtual void pdcch_ul_search_reset() = 0;
  virtual void pdcch_dl_search_reset() = 0;
  
  virtual uint32_t get_current_tti() = 0;
  
  virtual float get_phr() = 0; 
  virtual float get_pathloss_db() = 0;
    
};

class phy_interface_rrc
{
public:

  typedef struct {
    LIBLTE_RRC_PRACH_CONFIG_SIB_STRUCT          prach_cnfg;
    LIBLTE_RRC_PDSCH_CONFIG_COMMON_STRUCT       pdsch_cnfg;
    LIBLTE_RRC_PUSCH_CONFIG_COMMON_STRUCT       pusch_cnfg;
    LIBLTE_RRC_PHICH_CONFIG_STRUCT              phich_cnfg;
    LIBLTE_RRC_PUCCH_CONFIG_COMMON_STRUCT       pucch_cnfg;
    LIBLTE_RRC_SRS_UL_CONFIG_COMMON_STRUCT      srs_ul_cnfg;
    LIBLTE_RRC_UL_POWER_CONTROL_COMMON_STRUCT   ul_pwr_ctrl;
    LIBLTE_RRC_TDD_CONFIG_STRUCT                tdd_cnfg;
    LIBLTE_RRC_ANTENNA_PORTS_COUNT_ENUM         ant_info;      
  } phy_cfg_common_t; 
  
  typedef struct {
    LIBLTE_RRC_PHYSICAL_CONFIG_DEDICATED_STRUCT dedicated;
    phy_cfg_common_t                            common; 
    bool                                        enable_64qam; 
  } phy_cfg_t; 

  virtual void get_current_cell(srslte_cell_t *cell) = 0;
  virtual void get_config(phy_cfg_t *phy_cfg) = 0; 
  virtual void set_config(phy_cfg_t *phy_cfg) = 0; 
  virtual void set_config_dedicated(LIBLTE_RRC_PHYSICAL_CONFIG_DEDICATED_STRUCT *dedicated) = 0;
  virtual void set_config_common(phy_cfg_common_t *common) = 0; 
  virtual void set_config_tdd(LIBLTE_RRC_TDD_CONFIG_STRUCT *tdd) = 0; 
  virtual void set_config_64qam_en(bool enable) = 0;
  
  /* Is the PHY downlink synchronized? */
  virtual bool status_is_sync() = 0;

  /* Configure UL using parameters written with set_param() */
  virtual void configure_ul_params(bool pregen_disabled = false) = 0;

  virtual void reset() = 0;
  
  virtual void resync_sfn() = 0;   

};
  
}

#endif
  
