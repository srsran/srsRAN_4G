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

#ifndef SRSENB_PHY_H
#define SRSENB_PHY_H

#include "lte/sf_worker.h"
#include "phy_common.h"
#include "srsenb/hdr/phy/enb_phy_base.h"
#include "srsran/common/trace.h"
#include "srsran/interfaces/enb_metrics_interface.h"
#include "srsran/interfaces/enb_time_interface.h"
#include "srsran/interfaces/radio_interfaces.h"
#include "srsran/radio/radio.h"
#include "srsran/srslog/srslog.h"
#include "txrx.h"

namespace srsenb {

class phy final : public enb_phy_base,
                  public phy_interface_stack_lte,
                  public phy_interface_stack_nr,
                  public srsran::phy_interface_radio
{
public:
  phy(srslog::sink& log_sink);
  ~phy();

  int  init(const phy_args_t&            args,
            const phy_cfg_t&             cfg,
            srsran::radio_interface_phy* radio_,
            stack_interface_phy_lte*     stack_,
            enb_time_interface*          enb_);
  int  init(const phy_args_t&            args,
            const phy_cfg_t&             cfg,
            srsran::radio_interface_phy* radio_,
            stack_interface_phy_lte*     stack_lte_,
            stack_interface_phy_nr&      stack_nr_,
            enb_time_interface*          enb_);
  void stop() override;

  std::string get_type() override { return "lte"; };

  /* MAC->PHY interface */
  void rem_rnti(uint16_t rnti) final;
  void set_mch_period_stop(uint32_t stop) final;
  void set_activation_deactivation_scell(uint16_t                                     rnti,
                                         const std::array<bool, SRSRAN_MAX_CARRIERS>& activation) override;

  /*RRC-PHY interface*/
  void configure_mbsfn(srsran::sib2_mbms_t* sib2, srsran::sib13_t* sib13, const srsran::mcch_msg_t& mcch) override;

  void start_plot() override;
  void set_config(uint16_t rnti, const phy_rrc_cfg_list_t& phy_cfg_list) override;
  void complete_config(uint16_t rnti) override;

  void get_metrics(std::vector<phy_metrics_t>& metrics) override;

  void cmd_cell_gain(uint32_t cell_id, float gain_db) override;

  void radio_overflow() override{};
  void radio_failure() override{};

  void srsran_phy_logger(phy_logger_level_t log_level, char* str);

  int init_nr(const phy_args_t& args, const phy_cfg_t& cfg, stack_interface_phy_nr& stack);
  int set_common_cfg(const common_cfg_t& common_cfg) override;

private:
  srsran::phy_cfg_mbsfn_t mbsfn_config = {};
  uint32_t                nof_workers  = 0;

  const static int MAX_WORKERS = 4;

  const static int PRACH_WORKER_THREAD_PRIO = 5;
  const static int SF_RECV_THREAD_PRIO      = 1;
  const static int WORKERS_THREAD_PRIO      = 2;

  srsran::radio_interface_phy* radio = nullptr;

  srslog::sink&         log_sink;
  srslog::basic_logger& phy_log;
  srslog::basic_logger& phy_lib_log;

  lte::worker_pool                 lte_workers;
  std::unique_ptr<nr::worker_pool> nr_workers;
  phy_common                       workers_common;
  prach_worker_pool                prach;
  txrx                             tx_rx;

  bool initialized = false;

  srsran_prach_cfg_t prach_cfg = {};

  void parse_common_config(const phy_cfg_t& cfg);
};

} // namespace srsenb

#endif // SRSENB_PHY_H
