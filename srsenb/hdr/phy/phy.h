
#ifndef ENBPHY_H
#define ENBPHY_H

#include "srslte/common/log.h"
#include "phy/txrx.h"
#include "phy/phch_worker.h"
#include "phy/phch_common.h"
#include "srslte/radio/radio.h"
#include "srslte/interfaces/enb_interfaces.h"
#include "srslte/common/task_dispatcher.h"
#include "srslte/common/trace.h"
#include "srslte/interfaces/enb_metrics_interface.h"

namespace srsenb {
 
typedef struct {
  srslte_cell_t cell; 
  LIBLTE_RRC_PRACH_CONFIG_SIB_STRUCT          prach_cnfg;
  LIBLTE_RRC_PDSCH_CONFIG_COMMON_STRUCT       pdsch_cnfg;
  LIBLTE_RRC_PUSCH_CONFIG_COMMON_STRUCT       pusch_cnfg;
  LIBLTE_RRC_PUCCH_CONFIG_COMMON_STRUCT       pucch_cnfg;
  LIBLTE_RRC_SRS_UL_CONFIG_COMMON_STRUCT      srs_ul_cnfg;    
} phy_cfg_t; 

class phy : public phy_interface_mac,
            public phy_interface_rrc
{
public:

  phy();
  bool init(phy_args_t *args, phy_cfg_t *common_cfg, srslte::radio *radio_handler, mac_interface_phy *mac, srslte::log* log_h);
  bool init(phy_args_t *args, phy_cfg_t *common_cfg, srslte::radio *radio_handler, mac_interface_phy *mac, std::vector<void*> log_vec);
  void stop();
  
  /* MAC->PHY interface */
  int  add_rnti(uint16_t rnti);
  void rem_rnti(uint16_t rnti);

  static uint32_t tti_to_SFN(uint32_t tti);
  static uint32_t tti_to_subf(uint32_t tti);
  
  void start_plot();
  void set_config_dedicated(uint16_t rnti, LIBLTE_RRC_PHYSICAL_CONFIG_DEDICATED_STRUCT* dedicated);
  
  void get_metrics(phy_metrics_t metrics[ENB_METRICS_MAX_USERS]);
  
private:
    
  uint32_t nof_workers; 
  
  const static int MAX_WORKERS         = 4;
  const static int DEFAULT_WORKERS     = 2;
  
  const static int PRACH_WORKER_THREAD_PRIO = 80; 
  const static int SF_RECV_THREAD_PRIO = 1;
  const static int WORKERS_THREAD_PRIO = 0; 
  
  srslte::radio         *radio_handler;

  srslte::thread_pool      workers_pool;
  std::vector<phch_worker> workers;
  phch_common              workers_common; 
  prach_worker             prach; 
  txrx                     tx_rx; 
  
  srslte_prach_cfg_t prach_cfg; 
  
  void parse_config(phy_cfg_t* cfg);
  
};

} // namespace srsenb

#endif // UEPHY_H
