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
  cf_t* rx_buffer_c[SRSLTE_MAX_PORTS] = {};

  // Allocate buffers
  buffer_sz = SRSLTE_SF_LEN_PRB(phy->args.dl.nof_max_prb) * 5;
  for (uint32_t i = 0; i < phy_state_->args.dl.nof_rx_antennas; i++) {
    rx_buffer[i]   = srslte_vec_cf_malloc(buffer_sz);
    rx_buffer_c[i] = rx_buffer[i];
    tx_buffer[i]   = srslte_vec_cf_malloc(buffer_sz);
  }

  if (srslte_ue_dl_nr_init(&ue_dl, rx_buffer.data(), &phy_state_->args.dl) < SRSLTE_SUCCESS) {
    ERROR("Error initiating UE DL NR");
    return;
  }

  if (srslte_ue_ul_nr_init(&ue_ul, tx_buffer[0], &phy_state_->args.ul) < SRSLTE_SUCCESS) {
    ERROR("Error initiating UE DL NR");
    return;
  }

  if (srslte_softbuffer_tx_init_guru(&softbuffer_tx, SRSLTE_SCH_NR_MAX_NOF_CB_LDPC, SRSLTE_LDPC_MAX_LEN_ENCODED_CB) <
      SRSLTE_SUCCESS) {
    ERROR("Error init soft-buffer");
    return;
  }

  if (srslte_softbuffer_rx_init_guru(&softbuffer_rx, SRSLTE_SCH_NR_MAX_NOF_CB_LDPC, SRSLTE_LDPC_MAX_LEN_ENCODED_CB) <
      SRSLTE_SUCCESS) {
    ERROR("Error init soft-buffer");
    return;
  }

  // Initialise data with numbers
  tx_data.resize(SRSLTE_SCH_NR_MAX_NOF_CB_LDPC * SRSLTE_LDPC_MAX_LEN_ENCODED_CB / 8);
  for (uint32_t i = 0; i < SRSLTE_SCH_NR_MAX_NOF_CB_LDPC * SRSLTE_LDPC_MAX_LEN_ENCODED_CB / 8; i++) {
    tx_data[i] = i % 255U;
  }
}

cc_worker::~cc_worker()
{
  srslte_ue_dl_nr_free(&ue_dl);
  srslte_ue_ul_nr_free(&ue_ul);
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

  if (srslte_ue_dl_nr_set_pdcch_config(&ue_dl, &phy->cfg.pdcch) < SRSLTE_SUCCESS) {
    ERROR("Error setting carrier");
    return false;
  }

  if (srslte_ue_ul_nr_set_carrier(&ue_ul, carrier) < SRSLTE_SUCCESS) {
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
  ul_slot_cfg.idx = TTI_TX(tti);
}

cf_t* cc_worker::get_rx_buffer(uint32_t antenna_idx)
{
  if (antenna_idx >= phy->args.dl.nof_rx_antennas) {
    return nullptr;
  }

  return rx_buffer.at(antenna_idx);
}

cf_t* cc_worker::get_tx_buffer(uint32_t antenna_idx)
{
  if (antenna_idx >= phy->args.dl.nof_rx_antennas) {
    return nullptr;
  }

  return tx_buffer.at(antenna_idx);
}

uint32_t cc_worker::get_buffer_len()
{
  return buffer_sz;
}

bool cc_worker::work_dl()
{
  // Run FFT
  srslte_ue_dl_nr_estimate_fft(&ue_dl, &dl_slot_cfg);

  // Initialise grants
  std::array<srslte_dci_dl_nr_t, 5> dci_dl_rx        = {};
  std::array<srslte_dci_ul_nr_t, 5> dci_ul_rx        = {};
  uint32_t                          nof_found_dci_dl = 0;
  uint32_t                          nof_found_dci_ul = 0;

  // Search for RA DCI
  if (phy->cfg.pdcch.ra_search_space_present) {
    int n_ra = srslte_ue_dl_nr_find_dl_dci(&ue_dl,
                                           &dl_slot_cfg,
                                           phy->cfg.pdcch.ra_rnti,
                                           &dci_dl_rx[nof_found_dci_dl],
                                           (uint32_t)dci_dl_rx.size() - nof_found_dci_dl);
    if (n_ra < SRSLTE_SUCCESS) {
      ERROR("Error decoding");
      return false;
    }
    nof_found_dci_dl += n_ra;
  }

  // Search for test RNTI
  if (phy->test_rnti > 0) {
    // Search for test DL grants
    int n_dl = srslte_ue_dl_nr_find_dl_dci(&ue_dl,
                                           &dl_slot_cfg,
                                           (uint16_t)phy->test_rnti,
                                           &dci_dl_rx[nof_found_dci_dl],
                                           (uint32_t)dci_dl_rx.size() - nof_found_dci_dl);
    if (n_dl < SRSLTE_SUCCESS) {
      ERROR("Error decoding");
      return false;
    }
    nof_found_dci_dl += n_dl;

    // Search for test UL grants
    int n_ul = srslte_ue_dl_nr_find_ul_dci(&ue_dl,
                                           &dl_slot_cfg,
                                           (uint16_t)phy->test_rnti,
                                           &dci_ul_rx[nof_found_dci_ul],
                                           (uint32_t)dci_ul_rx.size() - nof_found_dci_ul);
    if (n_ul < SRSLTE_SUCCESS) {
      ERROR("Error decoding");
      return false;
    }
    nof_found_dci_ul += n_ul;
  }

  // Iterate over all UL received grants
  for (uint32_t i = 0; i < nof_found_dci_ul; i++) {
    // Log found DCI
    if (logger.info.enabled()) {
      std::array<char, 512> str;
      srslte_dci_ul_nr_to_str(&dci_ul_rx[i], str.data(), str.size());
      logger.info("PDCCH: cc=%d, %s", cc_idx, str.data());
    }

    // Enqueue UL grants
    phy->set_ul_pending_grant(dl_slot_cfg.idx, dci_ul_rx[i]);
  }

  // Iterate over all DL received grants
  for (uint32_t i = 0; i < nof_found_dci_dl; i++) {
    // Notify MAC about PDCCH found grant
    // ... At the moment reset softbuffer locally
    srslte_softbuffer_rx_reset(&softbuffer_rx);

    // Log found DCI
    if (logger.info.enabled()) {
      std::array<char, 512> str;
      srslte_dci_dl_nr_to_str(&dci_dl_rx[i], str.data(), str.size());
      logger.info("PDCCH: cc=%d, %s", cc_idx, str.data());
    }

    // Compute DL grant
    srslte_sch_cfg_nr_t pdsch_cfg = {};
    if (srslte_ra_dl_dci_to_grant_nr(&ue_dl.carrier, &phy->cfg.pdsch, &dci_dl_rx[i], &pdsch_cfg, &pdsch_cfg.grant)) {
      ERROR("Computing DL grant");
      return false;
    }

    // Get data buffer
    srslte::unique_byte_buffer_t data = srslte::make_byte_buffer();
    data->N_bytes                     = pdsch_cfg.grant.tb[0].tbs / 8U;

    // Initialise PDSCH Result
    std::array<srslte_pdsch_res_nr_t, SRSLTE_MAX_CODEWORDS> pdsch_res = {};
    pdsch_res[0].payload                                              = data->buffer;
    pdsch_cfg.grant.tb[0].softbuffer.rx                               = &softbuffer_rx;

    // Decode actual PDSCH transmission
    if (srslte_ue_dl_nr_decode_pdsch(&ue_dl, &dl_slot_cfg, &pdsch_cfg, pdsch_res.data()) < SRSLTE_SUCCESS) {
      ERROR("Error decoding PDSCH");
      return false;
    }

    // Logging
    if (logger.info.enabled()) {
      std::array<char, 512> str;
      srslte_ue_dl_nr_pdsch_info(&ue_dl, &pdsch_cfg, pdsch_res.data(), str.data(), str.size());
      logger.info(pdsch_res[0].payload, pdsch_cfg.grant.tb[0].tbs / 8, "PDSCH: cc=%d, %s", cc_idx, str.data());
    }

    // Notify MAC about PDSCH decoding result
    if (pdsch_res[0].crc) {
      // Prepare grant
      mac_interface_phy_nr::mac_nr_grant_dl_t mac_nr_grant = {};
      mac_nr_grant.tb[0]                                   = std::move(data);
      mac_nr_grant.pid                                     = dci_dl_rx[i].pid;
      mac_nr_grant.rnti                                    = dci_dl_rx[i].rnti;
      mac_nr_grant.tti                                     = dl_slot_cfg.idx;

      // Send data to MAC
      phy->stack->tb_decoded(cc_idx, mac_nr_grant);
    }
  }

  return true;
}

bool cc_worker::work_ul()
{
  srslte_sch_cfg_nr_t pusch_cfg = {};
  uint32_t            pid       = 0;

  // Request grant to PHY state for this transmit TTI
  if (not phy->get_ul_pending_grant(ul_slot_cfg.idx, pusch_cfg, pid)) {
    // If no grant, return earlier
    return true;
  }

  // Notify MAC about PUSCH found grant
  // ...
  srslte_softbuffer_tx_reset(&softbuffer_tx);
  pusch_cfg.grant.tb[0].softbuffer.tx = &softbuffer_tx;

  // Encode PUSCH transmission
  if (srslte_ue_ul_nr_encode_pusch(&ue_ul, &ul_slot_cfg, &pusch_cfg, tx_data.data()) < SRSLTE_SUCCESS) {
    ERROR("Encoding PUSCH");
    return false;
  }

  // Logging
  if (logger.info.enabled()) {
    std::array<char, 512> str;
    srslte_ue_ul_nr_pusch_info(&ue_ul, &pusch_cfg, str.data(), str.size());
    logger.info(tx_data.data(),
                pusch_cfg.grant.tb[0].tbs / 8,
                "PUSCH: cc=%d, %s, tti_tx=%d",
                cc_idx,
                str.data(),
                ul_slot_cfg.idx);
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
