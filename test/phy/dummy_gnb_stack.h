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

#ifndef SRSRAN_DUMMY_GNB_STACK_H
#define SRSRAN_DUMMY_GNB_STACK_H

#include "dummy_rx_harq_proc.h"
#include "dummy_tx_harq_proc.h"
#include <mutex>
#include <srsenb/hdr/stack/mac/mac_metrics.h>
#include <srsran/adt/circular_array.h>
#include <srsran/common/phy_cfg_nr.h>
#include <srsran/common/standard_streams.h>
#include <srsran/interfaces/gnb_interfaces.h>

class gnb_dummy_stack : public srsenb::stack_interface_phy_nr
{
private:
  srslog::basic_logger&                                                logger = srslog::fetch_basic_logger("GNB STK");
  const uint16_t                                                       rnti   = 0x1234;
  const uint32_t                                                       mcs    = 1;
  srsran::circular_array<srsran_dci_location_t, SRSRAN_NOF_SF_X_FRAME> dci_dl_location;
  srsran::circular_array<srsran_dci_location_t, SRSRAN_NOF_SF_X_FRAME> dci_ul_location;
  srsran::circular_array<uint32_t, SRSRAN_NOF_SF_X_FRAME>              dl_data_to_ul_ack;
  uint32_t                                                             ss_id       = 0;
  uint32_t                                                             dl_freq_res = 0;
  uint32_t                                                             dl_time_res = 0;
  uint32_t                                                             ul_freq_res = 0;
  uint32_t                                                             ul_time_res = 0;
  srsran_random_t                                                      random_gen  = nullptr;
  srsran::phy_cfg_nr_t                                                 phy_cfg     = {};
  bool                                                                 valid       = false;

  std::mutex               mac_metrics_mutex;
  srsenb::mac_ue_metrics_t mac_metrics = {};

  // HARQ feedback
  class pending_ack_t
  {
  private:
    std::mutex            mutex;
    srsran_pdsch_ack_nr_t ack = {};

  public:
    pending_ack_t() = default;
    void push_ack(srsran_harq_ack_resource_t& ack_resource)
    {
      // Prepare ACK information
      srsran_harq_ack_m_t ack_m = {};
      ack_m.resource            = ack_resource;
      ack_m.present             = true;

      std::unique_lock<std::mutex> lock(mutex);

      ack.nof_cc = 1;

      srsran_harq_ack_insert_m(&ack, &ack_m);
    }

    srsran_pdsch_ack_nr_t get_ack()
    {
      std::unique_lock<std::mutex> lock(mutex);
      srsran_pdsch_ack_nr_t        ret = ack;
      ack                              = {};
      return ret;
    }
    uint32_t get_dai()
    {
      std::unique_lock<std::mutex> lock(mutex);
      return ack.cc[0].M % 4;
    }
  };
  std::array<pending_ack_t, TTIMOD_SZ> pending_ack = {};

  // PUSCH state
  class pending_pusch_t
  {
  private:
    std::mutex          mutex;
    srsran_sch_cfg_nr_t pusch = {};
    bool                valid = false;
    uint32_t            pid   = 0;

  public:
    pending_pusch_t() = default;
    void push(const uint32_t& pid_, const srsran_sch_cfg_nr_t& pusch_)
    {
      std::unique_lock<std::mutex> lock(mutex);
      pusch = pusch_;
      pid   = pid_;
      valid = true;
    }

    bool pop(uint32_t& pid_, srsran_sch_cfg_nr_t& pusch_)
    {
      std::unique_lock<std::mutex> lock(mutex);
      bool                         ret = valid;
      pusch_                           = pusch;
      pid_                             = pid;
      valid                            = false;
      return ret;
    }
  };
  std::array<pending_pusch_t, TTIMOD_SZ> pending_pusch = {};

  srsran::circular_array<dummy_tx_harq_proc, SRSRAN_MAX_HARQ_PROC_DL_NR> tx_harq_proc;

  srsran::circular_array<dummy_rx_harq_proc, SRSRAN_MAX_HARQ_PROC_DL_NR> rx_harq_proc;

  bool schedule_pdsch(const srsran_slot_cfg_t& slot_cfg, dl_sched_t& dl_sched)
  {
    // Instantiate PDCCH and PDSCH
    pdcch_dl_t pdcch = {};
    pdsch_t    pdsch = {};

    // Second TB is not used
    pdsch.data[1] = nullptr;

    // Fill DCI configuration
    pdcch.dci_cfg = phy_cfg.get_dci_cfg();

    // Fill DCI context
    if (not phy_cfg.get_dci_ctx_pdsch_rnti_c(ss_id, dci_dl_location[slot_cfg.idx], rnti, pdcch.dci.ctx)) {
      logger.error("Error filling PDSCH DCI context");
      return false;
    }

    uint32_t harq_feedback     = dl_data_to_ul_ack[slot_cfg.idx];
    uint32_t harq_ack_slot_idx = TTI_ADD(slot_cfg.idx, harq_feedback);

    // Fill DCI fields
    srsran_dci_dl_nr_t& dci   = pdcch.dci;
    dci.freq_domain_assigment = dl_freq_res;
    dci.time_domain_assigment = dl_time_res;
    dci.mcs                   = mcs;
    dci.rv                    = 0;
    dci.ndi                   = (slot_cfg.idx / SRSRAN_NOF_SF_X_FRAME) % 2;
    dci.pid                   = slot_cfg.idx % SRSRAN_NOF_SF_X_FRAME;
    dci.dai                   = pending_ack[harq_ack_slot_idx % pending_ack.size()].get_dai();
    dci.tpc                   = 1;
    dci.pucch_resource        = 0;
    if (dci.ctx.format == srsran_dci_format_nr_1_0) {
      dci.harq_feedback = dl_data_to_ul_ack[slot_cfg.idx] - 1;
    } else {
      dci.harq_feedback = slot_cfg.idx;
    }

    // Create PDSCH configuration
    if (not phy_cfg.get_pdsch_cfg(slot_cfg, dci, pdsch.sch)) {
      logger.error("Error converting DCI to grant");
      return false;
    }

    // Set TBS
    // Select grant and set data
    pdsch.data[0] = tx_harq_proc[slot_cfg.idx].get_tb(pdsch.sch.grant.tb[0].tbs).data();

    // Generate random data
    srsran_random_byte_vector(random_gen, pdsch.data[0], pdsch.sch.grant.tb[0].tbs / 8);

    // Set softbuffer
    pdsch.sch.grant.tb[0].softbuffer.tx = &tx_harq_proc[slot_cfg.idx].get_softbuffer(dci.ndi);

    // Reset Tx softbuffer always
    srsran_softbuffer_tx_reset(pdsch.sch.grant.tb[0].softbuffer.tx);

    // Push scheduling results
    dl_sched.pdcch_dl.push_back(pdcch);
    dl_sched.pdsch.push_back(pdsch);

    // Generate PDSCH HARQ Feedback
    srsran_harq_ack_resource_t ack_resource = {};
    if (not phy_cfg.get_pdsch_ack_resource(dci, ack_resource)) {
      logger.error("Error getting ack resource");
      return false;
    }

    // Calculate PUCCH slot and push resource
    pending_ack[harq_ack_slot_idx % pending_ack.size()].push_ack(ack_resource);

    return true;
  }

  bool schedule_pusch(const srsran_slot_cfg_t& slot_cfg, dl_sched_t& dl_sched)
  {
    // Instantiate PDCCH
    pdcch_ul_t pdcch = {};

    // Fill DCI configuration
    pdcch.dci_cfg = phy_cfg.get_dci_cfg();

    // Fill DCI context
    if (not phy_cfg.get_dci_ctx_pusch_rnti_c(ss_id, dci_ul_location[slot_cfg.idx], rnti, pdcch.dci.ctx)) {
      logger.error("Error filling PDSCH DCI context");
      return false;
    }

    // Fill DCI fields
    srsran_dci_ul_nr_t& dci   = pdcch.dci;
    dci.freq_domain_assigment = ul_freq_res;
    dci.time_domain_assigment = ul_time_res;
    dci.freq_hopping_flag     = 0;
    dci.mcs                   = mcs;
    dci.rv                    = 0;
    dci.ndi                   = (slot_cfg.idx / SRSRAN_NOF_SF_X_FRAME) % 2;
    dci.pid                   = slot_cfg.idx % SRSRAN_NOF_SF_X_FRAME;
    dci.tpc                   = 1;

    // Create PDSCH configuration
    srsran_sch_cfg_nr_t pusch_cfg = {};
    if (not phy_cfg.get_pusch_cfg(slot_cfg, dci, pusch_cfg)) {
      logger.error("Error converting DCI to grant");
      return false;
    }

    // Set softbuffer
    pusch_cfg.grant.tb[0].softbuffer.rx = &rx_harq_proc[slot_cfg.idx].get_softbuffer(dci.ndi);

    // Push scheduling results
    dl_sched.pdcch_ul.push_back(pdcch);

    // Set pending PUSCH
    pending_pusch[TTI_TX(slot_cfg.idx) % pending_pusch.size()].push(dci.pid, pusch_cfg);

    return true;
  }

  bool handle_uci_data(const srsran_uci_cfg_nr_t& cfg, const srsran_uci_value_nr_t& value)
  {
    std::unique_lock<std::mutex> lock(mac_metrics_mutex);

    for (uint32_t i = 0; i < cfg.ack.count; i++) {
      const srsran_harq_ack_bit_t* ack_bit  = &cfg.ack.bits[i];
      bool                         is_ok    = (value.ack[i] == 1) and value.valid;
      uint32_t                     tb_count = (ack_bit->tb0 ? 1 : 0) + (ack_bit->tb1 ? 1 : 0);
      mac_metrics.tx_brate += tx_harq_proc[ack_bit->pid].get_tbs();
      mac_metrics.tx_pkts += tb_count;
      if (not is_ok) {
        mac_metrics.tx_errors += tb_count;
        logger.debug("NACK received!");
      }
    }

    return true;
  }

public:
  struct args_t {
    srsran::phy_cfg_nr_t phy_cfg;                           ///< Physical layer configuration
    uint16_t             rnti                     = 0x1234; ///< C-RNTI
    uint32_t             mcs                      = 10;     ///< Modulation code scheme
    uint32_t             ss_id                    = 1;      ///< Search Space identifier
    uint32_t             pdcch_aggregation_level  = 0;      ///< PDCCH aggregation level
    uint32_t             pdcch_dl_candidate_index = 0;      ///< PDCCH DL DCI candidate index
    uint32_t             pdcch_ul_candidate_index = 1;      ///< PDCCH UL DCI candidate index
    uint32_t             dl_start_rb              = 0;      ///< Start resource block
    uint32_t             dl_length_rb             = 0;      ///< Number of resource blocks
    uint32_t             ul_start_rb              = 0;      ///< Start resource block
    uint32_t             ul_length_rb             = 0;      ///< Number of resource blocks
    uint32_t             dl_time_res              = 0;      ///< PDSCH time resource
    std::string          log_level                = "debug";
  };

  gnb_dummy_stack(args_t args) :
    mcs(args.mcs), rnti(args.rnti), dl_time_res(args.dl_time_res), phy_cfg(args.phy_cfg), ss_id(args.ss_id)
  {
    random_gen = srsran_random_init(0x1234);
    logger.set_level(srslog::str_to_basic_level(args.log_level));

    // Select DCI locations
    for (uint32_t slot = 0; slot < SRSRAN_NOF_SF_X_FRAME; slot++) {
      srsran::bounded_vector<srsran_dci_location_t, SRSRAN_SEARCH_SPACE_MAX_NOF_CANDIDATES_NR> locations;

      if (not phy_cfg.get_dci_locations(slot, rnti, args.ss_id, args.pdcch_aggregation_level, locations)) {
        logger.error(
            "Error generating locations for slot %d and aggregation level %d", slot, args.pdcch_aggregation_level);
        return;
      }

      // DCI DL
      if (args.pdcch_dl_candidate_index >= locations.size()) {
        logger.error("Candidate index %d exceeds the number of candidates %d for aggregation level %d",
                     args.pdcch_dl_candidate_index,
                     (uint32_t)locations.size(),
                     args.pdcch_aggregation_level);
        return;
      }
      dci_dl_location[slot] = locations[args.pdcch_dl_candidate_index];

      // DCI UL
      if (args.pdcch_ul_candidate_index >= locations.size()) {
        logger.error("Candidate index %d exceeds the number of candidates %d for aggregation level %d",
                     args.pdcch_ul_candidate_index,
                     (uint32_t)locations.size(),
                     args.pdcch_aggregation_level);
        return;
      }
      dci_ul_location[slot] = locations[args.pdcch_ul_candidate_index];
    }

    // Select DL frequency domain resources
    dl_freq_res = srsran_ra_nr_type1_riv(args.phy_cfg.carrier.nof_prb, args.dl_start_rb, args.dl_length_rb);

    // Select DL frequency domain resources
    ul_freq_res = srsran_ra_nr_type1_riv(args.phy_cfg.carrier.nof_prb, args.ul_start_rb, args.ul_length_rb);

    // Setup DL Data to ACK timing
    for (uint32_t i = 0; i < SRSRAN_NOF_SF_X_FRAME; i++) {
      dl_data_to_ul_ack[i] = args.phy_cfg.harq_ack.dl_data_to_ul_ack[i % SRSRAN_MAX_NOF_DL_DATA_TO_UL];
    }

    // If reached this point the configuration is valid
    valid = true;
  }

  ~gnb_dummy_stack() { srsran_random_free(random_gen); }
  bool is_valid() const { return valid; }

  int rx_data_indication(rx_data_ind_t& grant) override { return 0; }

  int slot_indication(const srsran_slot_cfg_t& slot_cfg) override { return 0; }

  int get_dl_sched(const srsran_slot_cfg_t& slot_cfg, dl_sched_t& dl_sched) override
  {
    logger.set_context(slot_cfg.idx);

    // Check if it is TDD DL slot and PDSCH mask, if no PDSCH shall be scheduled, do not set any grant and skip
    if (not srsran_tdd_nr_is_dl(&phy_cfg.tdd, phy_cfg.carrier.scs, slot_cfg.idx)) {
      return SRSRAN_SUCCESS;
    }

    if (not schedule_pdsch(slot_cfg, dl_sched)) {
      logger.error("Error scheduling PDSCH");
      return SRSRAN_ERROR;
    }

    // Check if the UL slot is valid, if not skip UL scheduling
    if (not srsran_tdd_nr_is_ul(&phy_cfg.tdd, phy_cfg.carrier.scs, TTI_TX(slot_cfg.idx))) {
      return SRSRAN_SUCCESS;
    }

    if (not schedule_pusch(slot_cfg, dl_sched)) {
      logger.error("Error scheduling PUSCH");
      return SRSRAN_ERROR;
    }

    return SRSRAN_SUCCESS;
  }

  int get_ul_sched(const srsran_slot_cfg_t& slot_cfg, ul_sched_t& ul_sched) override
  {
    logger.set_context(slot_cfg.idx);

    // Get ACK information
    srsran_pdsch_ack_nr_t ack     = pending_ack[slot_cfg.idx % pending_ack.size()].get_ack();
    bool                  has_ack = ack.nof_cc > 0;

    if (has_ack) {
      if (logger.debug.enabled()) {
        std::array<char, 512> str = {};
        if (srsran_harq_ack_info(&ack, str.data(), (uint32_t)str.size()) > 0) {
          logger.debug("HARQ feedback:\n%s", str.data());
        }
      }
    }
    mac_interface_phy_nr::pusch_t pusch = {};
    bool has_pusch                      = pending_pusch[slot_cfg.idx % pending_pusch.size()].pop(pusch.pid, pusch.sch);

    srsran_uci_cfg_nr_t uci_cfg = {};
    if (not phy_cfg.get_uci_cfg(slot_cfg, ack, uci_cfg)) {
      logger.error("Error getting UCI configuration");
      return SRSRAN_ERROR;
    }

    // Schedule PUSCH
    if (has_pusch) {
      // Generate data
      pusch.data[0] = rx_harq_proc[pusch.pid].get_tb(pusch.sch.grant.tb[0].tbs).data();
      pusch.data[1] = nullptr;
      srsran_random_byte_vector(random_gen, pusch.data[0], pusch.sch.grant.tb[0].tbs / 8);

      // Put UCI configuration in PUSCH config
      if (not phy_cfg.get_pusch_uci_cfg(slot_cfg, uci_cfg, pusch.sch)) {
        logger.error("Error setting UCI configuration in PUSCH");
        return SRSRAN_ERROR;
      }

      ul_sched.pusch.push_back(pusch);
      return SRSRAN_SUCCESS;
    }

    // If any UCI information is triggered, schedule PUCCH
    if (uci_cfg.ack.count > 0 || uci_cfg.nof_csi > 0 || uci_cfg.o_sr > 0) {
      mac_interface_phy_nr::pucch_t pucch = {};
      pucch.uci_cfg                       = uci_cfg;
      if (not phy_cfg.get_pucch_uci_cfg(slot_cfg, uci_cfg, pucch.pucch_cfg, pucch.resource)) {
        logger.error("Error getting UCI CFG");
        return SRSRAN_ERROR;
      }

      ul_sched.pucch.push_back(pucch);
      return SRSRAN_SUCCESS;
    }

    // Otherwise no UL scheduling
    return SRSRAN_SUCCESS;
  }

  int pucch_info(const srsran_slot_cfg_t& slot_cfg, const pucch_info_t& pucch_info) override
  {
    // Handle UCI data
    if (not handle_uci_data(pucch_info.uci_data.cfg, pucch_info.uci_data.value)) {
      logger.error("Error handling UCI data from PUCCH reception");
      return SRSRAN_ERROR;
    }

    // Handle PHY metrics
    // ...

    return SRSRAN_SUCCESS;
  }

  int pusch_info(const srsran_slot_cfg_t& slot_cfg, const pusch_info_t& pusch_info) override
  {
    // Handle UCI data
    if (not handle_uci_data(pusch_info.uci_cfg, pusch_info.pusch_data.uci)) {
      logger.error("Error handling UCI data from PUCCH reception");
      return SRSRAN_ERROR;
    }

    if (not pusch_info.pusch_data.tb[0].crc) {
      mac_metrics.rx_errors++;
    }
    mac_metrics.rx_brate += rx_harq_proc[pusch_info.pid].get_tbs();
    mac_metrics.rx_pkts++;

    return SRSRAN_SUCCESS;
  }

  srsenb::mac_ue_metrics_t get_metrics()
  {
    std::unique_lock<std::mutex> lock(mac_metrics_mutex);

    return mac_metrics;
  }
};

#endif // SRSRAN_DUMMY_GNB_STACK_H
