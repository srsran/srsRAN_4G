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

#include "srsue/hdr/phy/scell/measure.h"

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

namespace srsue {
namespace scell {
void measure::init(cf_t*        buffer[SRSLTE_MAX_PORTS],
                   srslte::log* log_h,
                   uint32_t     nof_rx_antennas,
                   phy_common*  worker_com,
                   uint32_t     nof_subframes)

{
  this->log_h         = log_h;
  this->nof_subframes = nof_subframes;
  for (int i = 0; i < SRSLTE_MAX_PORTS; i++) {
    this->buffer[i] = buffer[i];
  }

  if (srslte_ue_dl_init(&ue_dl, this->buffer, SRSLTE_MAX_PRB, nof_rx_antennas)) {
    Error("SYNC:  Initiating ue_dl_measure\n");
    return;
  }
  worker_com->set_ue_dl_cfg(&ue_dl_cfg);
  ue_dl_cfg.chest_cfg.rsrp_neighbour = true;
  reset();
}

measure::~measure()
{
  srslte_ue_dl_free(&ue_dl);
}

void measure::reset()
{
  cnt       = 0;
  mean_rsrp = 0;
  mean_rsrq = 0;
  mean_snr  = 0;
  mean_rssi = 0;
  mean_cfo  = 0;
}

void measure::set_cell(srslte_cell_t cell)
{
  current_prb = cell.nof_prb;
  if (srslte_ue_dl_set_cell(&ue_dl, cell)) {
    Error("SYNC:  Setting cell: initiating ue_dl_measure\n");
  }
  reset();
}

float measure::rssi()
{
  return 10 * log10(mean_rssi);
}

float measure::rsrp()
{
  return 10 * log10(mean_rsrp) + 30 - rx_gain_offset;
}

float measure::rsrq()
{
  return 10 * log10(mean_rsrq);
}

float measure::snr()
{
  return mean_snr;
}

float measure::cfo()
{
  return mean_cfo;
}

uint32_t measure::frame_st_idx()
{
  return final_offset;
}

void measure::set_rx_gain_offset(float rx_gain_offset)
{
  this->rx_gain_offset = rx_gain_offset;
}

measure::ret_code measure::run_multiple_subframes(cf_t* input_buffer, uint32_t offset, uint32_t sf_idx, uint32_t max_sf)
{
  uint32_t sf_len = (uint32_t)SRSLTE_SF_LEN_PRB(current_prb);

  ret_code ret = IDLE;

  int sf_start = offset - sf_len / 2;
  while (sf_start < 0 && sf_idx < max_sf) {
    Info("INTRA: sf_start=%d, sf_idx=%d\n", sf_start, sf_idx);
    sf_start += sf_len;
    sf_idx++;
  }

#ifdef FINE_TUNE_OFFSET_WITH_RS
  float max_rsrp           = -200;
  int   best_test_sf_start = 0;
  int   test_sf_start      = 0;
  bool  found_best         = false;

  // Fine-tune sf_start using RS
  for (uint32_t n = 0; n < 5; n++) {

    test_sf_start = sf_start - 2 + n;
    if (test_sf_start >= 0) {

      cf_t* buf_m[SRSLTE_MAX_PORTS];
      buf_m[0] = &input_buffer[test_sf_start];

      uint32_t cfi;
      if (srslte_ue_dl_decode_fft_estimate_noguru(&ue_dl, buf_m, sf_idx, &cfi)) {
        Error("MEAS:  Measuring RSRP: Estimating channel\n");
        return ERROR;
      }

      float rsrp = srslte_chest_dl_get_rsrp(&ue_dl.chest);
      if (rsrp > max_rsrp) {
        max_rsrp           = rsrp;
        best_test_sf_start = test_sf_start;
        found_best         = true;
      }
    }
  }

  Debug("INTRA: fine-tuning sf_start: %d, found_best=%d, rem_sf=%d\n", sf_start, found_best, nof_sf);

  sf_start = found_best ? best_test_sf_start : sf_start;
#endif

  if (sf_start >= 0 && sf_start < (int)(sf_len * max_sf)) {

    uint32_t nof_sf = (sf_len * max_sf - sf_start) / sf_len;

    final_offset = (uint32_t)sf_start;

    for (uint32_t i = 0; i < nof_sf; i++) {
      memcpy(buffer[0], &input_buffer[sf_start + i * sf_len], sizeof(cf_t) * sf_len);
      ret = run_subframe((sf_idx + i) % 10);
      if (ret != IDLE) {
        return ret;
      }
    }
    if (ret != ERROR) {
      return MEASURE_OK;
    }
  } else {
    Error("INTRA: not running because sf_start=%d, offset=%d, sf_len*max_sf=%d*%d\n", sf_start, offset, sf_len, max_sf);
    ret = ERROR;
  }
  return ret;
}

measure::ret_code measure::run_subframe(uint32_t sf_idx)
{
  srslte_dl_sf_cfg_t sf_cfg;
  ZERO_OBJECT(sf_cfg);
  sf_cfg.tti = sf_idx;

  if (srslte_ue_dl_decode_fft_estimate(&ue_dl, &sf_cfg, &ue_dl_cfg)) {
    log_h->error("SYNC:  Measuring RSRP: Estimating channel\n");
    return ERROR;
  }

  float rsrp = ue_dl.chest_res.rsrp_neigh;
  float rsrq = ue_dl.chest_res.rsrq;
  float snr  = ue_dl.chest_res.snr_db;
  float cfo  = ue_dl.chest_res.cfo;
  float rssi = srslte_vec_avg_power_cf(buffer[0], (uint32_t)SRSLTE_SF_LEN_PRB(current_prb));

  if (cnt == 0) {
    mean_rsrp = rsrp;
    mean_rsrq = rsrq;
    mean_snr  = snr;
    mean_rssi = rssi;
    mean_cfo  = cfo;
  } else {
    mean_rsrp = SRSLTE_VEC_CMA(rsrp, mean_rsrp, cnt);
    mean_rsrq = SRSLTE_VEC_CMA(rsrq, mean_rsrq, cnt);
    mean_snr  = SRSLTE_VEC_CMA(snr, mean_snr, cnt);
    mean_rssi = SRSLTE_VEC_CMA(rssi, mean_rssi, cnt);
    mean_cfo  = SRSLTE_VEC_CMA(cfo, mean_cfo, cnt);
  }
  cnt++;

  log_h->debug(
      "SYNC:  Measuring RSRP %d/%d, sf_idx=%d, RSRP=%.1f dBm, SNR=%.1f dB\n", cnt, nof_subframes, sf_idx, rsrp, snr);

  if (cnt >= nof_subframes) {
    return MEASURE_OK;
  } else {
    return IDLE;
  }
}
} // namespace scell
} // namespace srsue
