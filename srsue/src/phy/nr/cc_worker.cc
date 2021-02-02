/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsue/hdr/phy/nr/cc_worker.h"
#include "srslte/srslte.h"

namespace srsue {
namespace nr {
cc_worker::cc_worker(uint32_t cc_idx_, srslog::basic_logger& log, state* phy_state_) :
  cc_idx(cc_idx_), phy(phy_state_), logger(log)
{
  cf_t* buffer_c[SRSLTE_MAX_PORTS] = {};

  // Allocate buffers
  buffer_sz = SRSLTE_SF_LEN_PRB(phy->args.dl.nof_max_prb) * 5;
  for (uint32_t i = 0; i < phy_state_->args.dl.nof_rx_antennas; i++) {
    rx_buffer[i] = srslte_vec_cf_malloc(buffer_sz);
    tx_buffer[i] = srslte_vec_cf_malloc(buffer_sz);
    buffer_c[i]  = rx_buffer[i];
  }

  if (srslte_ue_dl_nr_init(&ue_dl, buffer_c, &phy_state_->args.dl) < SRSLTE_SUCCESS) {
    ERROR("Error initiating UE DL NR");
    return;
  }

  if (srslte_softbuffer_rx_init_guru(&softbuffer_rx, SRSLTE_SCH_NR_MAX_NOF_CB_LDPC, SRSLTE_LDPC_MAX_LEN_ENCODED_CB) <
      SRSLTE_SUCCESS) {
    ERROR("Error init soft-buffer");
    return;
  }
  data.resize(SRSLTE_SCH_NR_MAX_NOF_CB_LDPC * SRSLTE_LDPC_MAX_LEN_ENCODED_CB / 8);
}

cc_worker::~cc_worker()
{
  srslte_ue_dl_nr_free(&ue_dl);
  srslte_softbuffer_rx_free(&softbuffer_rx);
  for (cf_t* p : rx_buffer) {
    if (p != nullptr) {
      free(p);
    }
  }
  for (cf_t* p : tx_buffer) {
    if (p != nullptr) {
      free(p);
    }
  }
}

bool cc_worker::set_carrier(const srslte_carrier_nr_t* carrier)
{
  if (srslte_ue_dl_nr_set_carrier(&ue_dl, carrier) < SRSLTE_SUCCESS) {
    ERROR("Error setting carrier");
    return false;
  }

  if (srslte_ue_dl_nr_set_config(&ue_dl, &phy->cfg.pdcch) < SRSLTE_SUCCESS) {
    ERROR("Error setting carrier");
    return false;
  }

  // Set default PDSCH config
  phy->cfg.pdsch.rbg_size_cfg_1 = false;

  return true;
}

void cc_worker::set_tti(uint32_t tti)
{
  dl_slot_cfg.idx = tti;
}

cf_t* cc_worker::get_rx_buffer(uint32_t antenna_idx)
{
  if (antenna_idx >= phy->args.dl.nof_rx_antennas) {
    return nullptr;
  }

  return rx_buffer.at(antenna_idx);
}

uint32_t cc_worker::get_buffer_len()
{
  return buffer_sz;
}

bool cc_worker::work_dl()
{
  srslte_pdsch_cfg_nr_t pdsch_hl_cfg = phy->cfg.pdsch;

  // Run FFT
  srslte_ue_dl_nr_estimate_fft(&ue_dl, &dl_slot_cfg);

  // Initialise grants
  std::array<srslte_dci_dl_nr_t, 5> dci_dl_rx     = {};
  uint32_t                          nof_found_dci = 0;

  // Search for RA DCI
  if (phy->cfg.pdcch.ra_search_space_present) {
    int n_ra = srslte_ue_dl_nr_find_dl_dci(&ue_dl,
                                           &dl_slot_cfg,
                                           phy->cfg.pdcch.ra_rnti,
                                           &dci_dl_rx[nof_found_dci],
                                           (uint32_t)dci_dl_rx.size() - nof_found_dci);
    if (n_ra < SRSLTE_SUCCESS) {
      ERROR("Error decoding");
      return false;
    }
    nof_found_dci += n_ra;
  }

  // Search for test RNTI
  if (phy->test_rnti > 0) {
    int n_test = srslte_ue_dl_nr_find_dl_dci(&ue_dl,
                                             &dl_slot_cfg,
                                             (uint16_t)phy->test_rnti,
                                             &dci_dl_rx[nof_found_dci],
                                             (uint32_t)dci_dl_rx.size() - nof_found_dci);
    if (n_test < SRSLTE_SUCCESS) {
      ERROR("Error decoding");
      return false;
    }
    nof_found_dci += n_test;
  }

  // Notify MAC about PDCCH found grants
  // ...

  // Iterate over all received grants
  for (uint32_t i = 0; i < nof_found_dci; i++) {
    // Select Received DCI
    const srslte_dci_dl_nr_t* dci_dl = &dci_dl_rx[i];

    // Log found DCI
    if (logger.info.enabled()) {
      std::array<char, 512> str;
      srslte_dci_nr_to_str(dci_dl, str.data(), str.size());
      logger.info("PDCCH: cc=%d, %s", cc_idx, str.data());
    }

    // Compute DL grant
    srslte_sch_cfg_nr_t pdsch_cfg = {};
    if (srslte_ra_dl_dci_to_grant_nr(&ue_dl.carrier, &pdsch_hl_cfg, dci_dl, &pdsch_cfg, &pdsch_cfg.grant)) {
      ERROR("Computing DL grant");
      return false;
    }

    // Initialise PDSCH Result
    std::array<srslte_pdsch_res_nr_t, SRSLTE_MAX_CODEWORDS> pdsch_res = {};
    pdsch_res[0].payload                                              = data.data();
    pdsch_cfg.grant.tb[0].softbuffer.rx                               = &softbuffer_rx;
    srslte_softbuffer_rx_reset(pdsch_cfg.grant.tb[0].softbuffer.rx);

    // Decode actual PDSCH transmission
    if (srslte_ue_dl_nr_decode_pdsch(&ue_dl, &dl_slot_cfg, &pdsch_cfg, pdsch_res.data()) < SRSLTE_SUCCESS) {
      ERROR("Error decoding PDSCH");
      return false;
    }

    // Notify MAC about PDSCH decoding result
    // ...

    // Logging
    if (logger.info.enabled()) {
      std::array<char, 512> str;
      srslte_ue_dl_nr_pdsch_info(&ue_dl, &pdsch_cfg, pdsch_res.data(), str.data(), str.size());
      logger.info("PDSCH: cc=%d, %s", cc_idx, str.data());
    }
  }

  return true;
}

int cc_worker::read_pdsch_d(cf_t* pdsch_d)
{
  uint32_t nof_re = ue_dl.carrier.nof_prb * SRSLTE_NRE * 12;
  srslte_vec_cf_copy(pdsch_d, ue_dl.pdsch.d[0], nof_re);
  return nof_re;
}

} // namespace nr
} // namespace srsue
