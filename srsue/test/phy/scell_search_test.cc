/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

#include <memory>
#include <srslte/phy/channel/channel.h>
#include <srslte/srslte.h>
#include <srsue/hdr/phy/scell/intra_measure.h>
#include <vector>

// Simulation parameters
static uint16_t      pdsch_rnti       = 0x1234;
static srslte_cell_t cell_base        = {.nof_prb         = 6,
                                  .nof_ports       = 1,
                                  .id              = 0,
                                  .cp              = SRSLTE_CP_NORM,
                                  .phich_length    = SRSLTE_PHICH_NORM,
                                  .phich_resources = SRSLTE_PHICH_R_1_6,
                                  .frame_type      = SRSLTE_FDD};
static uint32_t      nof_enb          = 3;
static uint16_t      cell_id_start    = 0;
static uint16_t      cell_id_step     = 1;
static float         channel_period_s = 7.2f;
static uint32_t      sim_time_periods = 10;
static uint32_t      cfi              = 1;

static srslte::channel::args_t channel_args_base;

// Constant
static const srslte_tm_t transmission_mode = SRSLTE_TM1;

int work_enb(srslte_enb_dl_t*         enb_dl,
             srslte_dl_sf_cfg_t*      dl_sf,
             srslte_dci_cfg_t*        dci_cfg,
             srslte_dci_dl_t*         dci,
             srslte_softbuffer_tx_t** softbuffer_tx,
             uint8_t**                data_tx)
{
  int ret = SRSLTE_ERROR;

  srslte_enb_dl_put_base(enb_dl, dl_sf);

  // Put PDSCH only if required
  if (dci && dci_cfg && data_tx && softbuffer_tx) {
    if (srslte_enb_dl_put_pdcch_dl(enb_dl, dci_cfg, dci)) {
      ERROR("Error putting PDCCH sf_idx=%d\n", dl_sf->tti);
      goto quit;
    }

    // Create pdsch config
    srslte_pdsch_cfg_t pdsch_cfg;
    if (srslte_ra_dl_dci_to_grant(&enb_dl->cell, dl_sf, transmission_mode, false, dci, &pdsch_cfg.grant)) {
      ERROR("Computing DL grant sf_idx=%d\n", dl_sf->tti);
      goto quit;
    }
    char str[512];
    srslte_dci_dl_info(dci, str, 512);
    INFO("eNb PDCCH: rnti=0x%x, %s\n", pdsch_rnti, str);

    for (uint32_t i = 0; i < SRSLTE_MAX_CODEWORDS; i++) {
      pdsch_cfg.softbuffers.tx[i] = softbuffer_tx[i];
    }

    // Enable power allocation
    pdsch_cfg.power_scale  = true;
    pdsch_cfg.p_a          = 0.0f;                                     // 0 dB
    pdsch_cfg.p_b          = (transmission_mode > SRSLTE_TM1) ? 1 : 0; // 0 dB
    pdsch_cfg.rnti         = pdsch_rnti;
    pdsch_cfg.meas_time_en = false;

    if (srslte_enb_dl_put_pdsch(enb_dl, &pdsch_cfg, data_tx) < 0) {
      ERROR("Error putting PDSCH sf_idx=%d\n", dl_sf->tti);
      goto quit;
    }
    srslte_pdsch_tx_info(&pdsch_cfg, str, 512);
    INFO("eNb PDSCH: rnti=0x%x, %s\n", pdsch_rnti, str);
  }

  srslte_enb_dl_gen_signal(enb_dl);

  ret = SRSLTE_SUCCESS;

quit:
  return ret;
}

class test_enb
{
private:
  srslte_enb_dl_t     enb_dl;
  srslte::channel_ptr channel;
  cf_t*               signal_buffer[SRSLTE_MAX_PORTS] = {};

public:
  test_enb(const srslte_cell_t& cell, const srslte::channel::args_t& channel_args)
  {
    channel = srslte::channel_ptr(new srslte::channel(channel_args, cell_base.nof_ports));
    channel->set_srate(srslte_sampling_freq_hz(cell.nof_prb));

    // Allocate buffer for eNb
    for (uint32_t i = 0; i < cell_base.nof_ports; i++) {
      signal_buffer[i] = (cf_t*)srslte_vec_malloc(sizeof(cf_t) * SRSLTE_SF_LEN_PRB(cell_base.nof_prb));
      if (!signal_buffer[i]) {
        ERROR("Error allocating buffer\n");
      }
    }

    if (srslte_enb_dl_init(&enb_dl, signal_buffer, cell.nof_prb)) {
      ERROR("Error initiating eNb downlink\n");
    }

    if (srslte_enb_dl_set_cell(&enb_dl, cell)) {
      ERROR("Error setting eNb DL cell\n");
    }

    if (srslte_enb_dl_add_rnti(&enb_dl, pdsch_rnti)) {
      ERROR("Error adding RNTI\n");
    }
  }

  int work(srslte_dl_sf_cfg_t*       dl_sf,
           srslte_dci_cfg_t*         dci_cfg,
           srslte_dci_dl_t*          dci,
           srslte_softbuffer_tx_t**  softbuffer_tx,
           uint8_t**                 data_tx,
           cf_t*                     baseband_buffer,
           const srslte_timestamp_t& ts)
  {

    int      ret    = SRSLTE_ERROR;
    uint32_t sf_len = SRSLTE_SF_LEN_PRB(enb_dl.cell.nof_prb);

    srslte_enb_dl_put_base(&enb_dl, dl_sf);

    // Put PDSCH only if it is required
    if (dci && dci_cfg && softbuffer_tx && data_tx) {
      if (srslte_enb_dl_put_pdcch_dl(&enb_dl, dci_cfg, dci)) {
        ERROR("Error putting PDCCH sf_idx=%d\n", dl_sf->tti);
        goto quit;
      }

      // Create pdsch config
      srslte_pdsch_cfg_t pdsch_cfg;
      if (srslte_ra_dl_dci_to_grant(&enb_dl.cell, dl_sf, transmission_mode, false, dci, &pdsch_cfg.grant)) {
        ERROR("Computing DL grant sf_idx=%d\n", dl_sf->tti);
        goto quit;
      }
      char str[512];
      srslte_dci_dl_info(dci, str, 512);
      INFO("eNb PDCCH: rnti=0x%x, %s\n", pdsch_rnti, str);

      for (uint32_t i = 0; i < SRSLTE_MAX_CODEWORDS; i++) {
        pdsch_cfg.softbuffers.tx[i] = softbuffer_tx[i];
      }

      // Enable power allocation
      pdsch_cfg.power_scale  = true;
      pdsch_cfg.p_a          = 0.0f;                                     // 0 dB
      pdsch_cfg.p_b          = (transmission_mode > SRSLTE_TM1) ? 1 : 0; // 0 dB
      pdsch_cfg.rnti         = pdsch_rnti;
      pdsch_cfg.meas_time_en = false;

      if (srslte_enb_dl_put_pdsch(&enb_dl, &pdsch_cfg, data_tx) < 0) {
        ERROR("Error putting PDSCH sf_idx=%d\n", dl_sf->tti);
        goto quit;
      }
      srslte_pdsch_tx_info(&pdsch_cfg, str, 512);
      INFO("eNb PDSCH: rnti=0x%x, %s\n", pdsch_rnti, str);
    }

    srslte_enb_dl_gen_signal(&enb_dl);

    // Apply channel
    channel->run(signal_buffer, signal_buffer, sf_len, ts);

    // Add to baseband
    for (uint32_t i = 1; i < enb_dl.cell.nof_ports; i++) {
      srslte_vec_sum_ccc(signal_buffer[0], signal_buffer[i], signal_buffer[0], sf_len);
    }
    srslte_vec_sum_ccc(signal_buffer[0], baseband_buffer, baseband_buffer, sf_len);

    ret = SRSLTE_SUCCESS;

  quit:
    return ret;
  }

  ~test_enb()
  {
    for (uint32_t i = 0; i < enb_dl.cell.nof_ports; i++) {
      if (signal_buffer[i]) {
        free(signal_buffer[i]);
        signal_buffer[i] = nullptr;
      }
    }

    srslte_enb_dl_free(&enb_dl);
  }
};

class dummy_rrc : public srsue::rrc_interface_phy_lte
{
public:
  void in_sync() override {}
  void out_of_sync() override {}
  void new_phy_meas(float rsrp, float rsrq, uint32_t tti, int earfcn, int pci) override
  {
    printf("[new_phy_meas] tti=%d earfcn=%d; pci=%d; rsrp=%+.1f; rsrq=%+.1f;\n", tti, earfcn, pci, rsrp, rsrq);
  }
};

int main(int argc, char** argv)
{
  auto                                    baseband_buffer = (cf_t*)srslte_vec_malloc(sizeof(cf_t) * SRSLTE_SF_LEN_MAX);
  std::vector<std::unique_ptr<test_enb> > test_enb_v;
  srslte_timestamp_t                      ts = {};
  srsue::scell::intra_measure             intra_measure;
  srslte::log_filter                      logger("intra_measure");
  dummy_rrc                               rrc;

  srsue::phy_common common(1);
  srsue::phy_args_t phy_args;
  common.args                        = &phy_args;
  phy_args.estimator_fil_auto        = false;
  phy_args.estimator_fil_order       = 4;
  phy_args.estimator_fil_stddev      = 1.0f;
  phy_args.sic_pss_enabled           = false;
  phy_args.intra_freq_meas_len_ms    = 20;
  phy_args.intra_freq_meas_period_ms = 200;

  intra_measure.init(&common, &rrc, &logger);
  intra_measure.set_primay_cell(0, cell_base);
  intra_measure.add_cell(cell_id_start);

  for (uint32_t enb_idx = 0; enb_idx < nof_enb; enb_idx++) {
    // Initialise cell
    srslte_cell_t cell = cell_base;
    cell.id            = cell_id_start + enb_idx * cell_id_step;

    // Initialise channel and push back
    srslte::channel::args_t channel_args;
    channel_args.enable          = true;
    channel_args.hst_enable      = true;
    channel_args.hst_init_time_s = 0.0f; //(float) channel_period_s / (float) nof_enb * enb_idx;
    channel_args.hst_period_s    = 7.2f; //(float) channel_period_s;
    channel_args.hst_fd_hz       = 750.0f;
    channel_args.delay_enable    = enb_idx != 0;
    channel_args.delay_min_us    = 10.0;
    channel_args.delay_max_us    = 300.0;
    channel_args.delay_period_s  = channel_period_s;
    test_enb_v.push_back(std::unique_ptr<test_enb>(new test_enb(cell, channel_args)));
  }

  for (uint32_t period = 0; period < sim_time_periods; period++) {
    for (uint32_t k = 0, sf_idx = 0; k < channel_period_s * 1000; k++, sf_idx++) {
      srslte_dl_sf_cfg_t sf_cfg_dl = {};
      sf_cfg_dl.tti                = sf_idx % 10;
      sf_cfg_dl.cfi                = cfi;
      sf_cfg_dl.sf_type            = SRSLTE_SF_NORM;

      // Clean buffer
      bzero(baseband_buffer, sizeof(cf_t) * SRSLTE_SF_LEN_MAX);

      for (auto& enb : test_enb_v) {
        enb->work(&sf_cfg_dl, nullptr, nullptr, nullptr, nullptr, baseband_buffer, ts);
      }

      srslte_timestamp_add(&ts, 0, 0.001f);

      intra_measure.write(k, baseband_buffer, SRSLTE_SF_LEN_PRB(cell_base.nof_prb));
      if (k % 1000 == 0) {
        printf("Done %.1f%%\n", (double)k * 100.0 / ((double)sim_time_periods * channel_period_s * 1000.0));
      }
    }
  }
  intra_measure.stop();
  srslte_dft_exit();
}