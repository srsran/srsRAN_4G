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

#include "srslte/phy/ue/ue_mib_nbiot.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"
#include <stdlib.h>
#include <string.h>
#include <strings.h>

int srslte_ue_mib_nbiot_init(srslte_ue_mib_nbiot_t* q, cf_t** in_buffer, uint32_t max_prb)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q != NULL) {
    ret = SRSLTE_ERROR;
    bzero(q, sizeof(srslte_ue_mib_nbiot_t));

    if (srslte_npbch_init(&q->npbch)) {
      fprintf(stderr, "Error initiating NPBCH\n");
      goto clean_exit;
    }

    q->sf_symbols = srslte_vec_cf_malloc(SRSLTE_SF_LEN_RE(max_prb, SRSLTE_CP_NORM));
    if (!q->sf_symbols) {
      perror("malloc");
      goto clean_exit;
    }

    for (uint32_t i = 0; i < SRSLTE_MAX_PORTS; i++) {
      q->ce[i] = srslte_vec_cf_malloc(SRSLTE_SF_LEN_RE(max_prb, SRSLTE_CP_NORM));
      if (!q->ce[i]) {
        perror("malloc");
        goto clean_exit;
      }
    }

    if (srslte_ofdm_rx_init(&q->fft, SRSLTE_CP_NORM, in_buffer[0], q->sf_symbols, max_prb)) {
      fprintf(stderr, "Error initializing FFT\n");
      goto clean_exit;
    }
    srslte_ofdm_set_freq_shift(&q->fft, SRSLTE_NBIOT_FREQ_SHIFT_FACTOR);

    if (srslte_chest_dl_nbiot_init(&q->chest, max_prb)) {
      fprintf(stderr, "Error initializing reference signal\n");
      goto clean_exit;
    }
    srslte_ue_mib_nbiot_reset(q);

    ret = SRSLTE_SUCCESS;
  }

clean_exit:
  if (ret == SRSLTE_ERROR) {
    srslte_ue_mib_nbiot_free(q);
  }
  return ret;
}

void srslte_ue_mib_nbiot_free(srslte_ue_mib_nbiot_t* q)
{
  if (q->sf_symbols) {
    free(q->sf_symbols);
  }
  for (int i = 0; i < SRSLTE_MAX_PORTS; i++) {
    if (q->ce[i]) {
      free(q->ce[i]);
    }
  }
  srslte_sync_nbiot_free(&q->sfind);
  srslte_chest_dl_nbiot_free(&q->chest);
  srslte_npbch_free(&q->npbch);
  srslte_ofdm_rx_free(&q->fft);

  bzero(q, sizeof(srslte_ue_mib_nbiot_t));
}

int srslte_ue_mib_nbiot_set_cell(srslte_ue_mib_nbiot_t* q, srslte_nbiot_cell_t cell)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q != NULL && cell.nof_ports <= SRSLTE_NBIOT_MAX_PORTS) {
    ret = SRSLTE_ERROR;

    if (srslte_npbch_set_cell(&q->npbch, cell)) {
      fprintf(stderr, "Error setting cell in NPBCH\n");
      goto clean_exit;
    }

    if (cell.nof_ports == 0) {
      cell.nof_ports = SRSLTE_NBIOT_MAX_PORTS;
    }

    if (srslte_chest_dl_nbiot_set_cell(&q->chest, cell)) {
      fprintf(stderr, "Error initializing reference signal\n");
      goto clean_exit;
    }
    srslte_ue_mib_nbiot_reset(q);

    ret = SRSLTE_SUCCESS;
  }

clean_exit:
  if (ret == SRSLTE_ERROR) {
    srslte_ue_mib_nbiot_free(q);
  }
  return ret;
}

void srslte_ue_mib_nbiot_reset(srslte_ue_mib_nbiot_t* q)
{
  q->frame_cnt = 0;
  srslte_npbch_decode_reset(&q->npbch);
}

int srslte_ue_mib_nbiot_decode(srslte_ue_mib_nbiot_t* q,
                               cf_t*                  input,
                               uint8_t*               bch_payload,
                               uint32_t*              nof_tx_ports,
                               int*                   sfn_offset)
{
  // Run FFT for the symbols
  srslte_ofdm_rx_sf(&q->fft);

  // Get channel estimates of sf idx #0 for each port
  if (srslte_chest_dl_nbiot_estimate(&q->chest, q->sf_symbols, q->ce, 0) < 0) {
    return SRSLTE_ERROR;
  }

  // Reset decoder if we missed a NPBCH TTI
  if (q->frame_cnt > SRSLTE_NPBCH_NUM_FRAMES) {
    INFO("Resetting NPBCH decoder after %d frames\n", q->frame_cnt);
    srslte_ue_mib_nbiot_reset(q);
    return SRSLTE_UE_MIB_NBIOT_NOTFOUND;
  }

  // Decode NPBCH
  if (SRSLTE_SUCCESS == srslte_npbch_decode_nf(&q->npbch,
                                               q->sf_symbols,
                                               q->ce,
                                               srslte_chest_dl_nbiot_get_noise_estimate(&q->chest),
                                               bch_payload,
                                               nof_tx_ports,
                                               sfn_offset,
                                               0)) {
    DEBUG("BCH decoded ok with offset %d\n", *sfn_offset);
    if (memcmp(bch_payload, q->last_bch_payload, SRSLTE_MIB_NB_LEN) == 0) {
      DEBUG("BCH content equals last BCH, new counter %d\n", q->frame_cnt);
    } else {
      // new BCH transmitted
      if (q->frame_cnt != 0) {
        INFO("MIB-NB decoded: %u with offset %d\n", q->frame_cnt, *sfn_offset);
        if (*sfn_offset != 0) {
          INFO("New BCH was decoded at block offset %d. SFN may be corrupted.", *sfn_offset);
        }
        srslte_ue_mib_nbiot_reset(q);
        return SRSLTE_UE_MIB_NBIOT_FOUND;
      } else {
        // store new BCH
        DEBUG("New BCH transmitted after %d frames\n", q->frame_cnt);
        memcpy(q->last_bch_payload, bch_payload, SRSLTE_MIB_NB_LEN);
      }
    }
    q->frame_cnt++;
  }
  return SRSLTE_UE_MIB_NBIOT_NOTFOUND;
}

int srslte_ue_mib_sync_nbiot_init_multi(
    srslte_ue_mib_sync_nbiot_t* q,
    int(recv_callback)(void*, cf_t * [SRSLTE_MAX_PORTS], uint32_t, srslte_timestamp_t*),
    uint32_t nof_rx_antennas,
    void*    stream_handler)
{
  // Allocate memory for time re-alignment and MIB detection
  for (int i = 0; i < SRSLTE_MAX_PORTS; i++) {
    q->sf_buffer[i] = NULL;
  }
  for (int i = 0; i < nof_rx_antennas; i++) {
    q->sf_buffer[i] = srslte_vec_cf_malloc(SRSLTE_SF_LEN_PRB_NBIOT * SRSLTE_NOF_SF_X_FRAME);
  }
  q->nof_rx_antennas = nof_rx_antennas;

  if (srslte_ue_mib_nbiot_init(&q->ue_mib, q->sf_buffer, SRSLTE_NBIOT_MAX_PRB)) {
    fprintf(stderr, "Error initiating ue_mib\n");
    return SRSLTE_ERROR;
  }
  if (srslte_ue_sync_nbiot_init_multi(
          &q->ue_sync, SRSLTE_NBIOT_MAX_PRB, recv_callback, q->nof_rx_antennas, stream_handler)) {
    fprintf(stderr, "Error initiating ue_sync\n");
    srslte_ue_mib_nbiot_free(&q->ue_mib);
    return SRSLTE_ERROR;
  }
  return SRSLTE_SUCCESS;
}

int srslte_ue_mib_sync_nbiot_set_cell(srslte_ue_mib_sync_nbiot_t* q, srslte_nbiot_cell_t cell)
{
  if (srslte_ue_mib_nbiot_set_cell(&q->ue_mib, cell)) {
    fprintf(stderr, "Error initiating ue_mib\n");
    return SRSLTE_ERROR;
  }
  if (srslte_ue_sync_nbiot_set_cell(&q->ue_sync, cell)) {
    fprintf(stderr, "Error initiating ue_sync\n");
    srslte_ue_mib_nbiot_free(&q->ue_mib);
    return SRSLTE_ERROR;
  }
  return SRSLTE_SUCCESS;
}

void srslte_ue_mib_sync_nbiot_free(srslte_ue_mib_sync_nbiot_t* q)
{
  srslte_ue_mib_nbiot_free(&q->ue_mib);
  srslte_ue_sync_nbiot_free(&q->ue_sync);
}

void srslte_ue_mib_sync_nbiot_reset(srslte_ue_mib_sync_nbiot_t* q)
{
  srslte_ue_mib_nbiot_reset(&q->ue_mib);
  srslte_ue_sync_nbiot_reset(&q->ue_sync);
}

int srslte_ue_mib_sync_nbiot_decode(srslte_ue_mib_sync_nbiot_t* q,
                                    uint32_t                    max_frames_timeout,
                                    uint8_t*                    bch_payload,
                                    uint32_t*                   nof_tx_ports,
                                    int*                        sfn_offset)
{
  int      ret        = SRSLTE_ERROR_INVALID_INPUTS;
  uint32_t nof_frames = 0;
  int      mib_ret    = SRSLTE_UE_MIB_NBIOT_NOTFOUND;

  if (q == NULL) {
      return ret;
  }

  do {
    mib_ret = SRSLTE_UE_MIB_NBIOT_NOTFOUND;
    ret     = srslte_ue_sync_nbiot_zerocopy_multi(&q->ue_sync, q->sf_buffer);
    if (ret < 0) {
      fprintf(stderr, "Error calling srslte_ue_sync_nbiot_zerocopy_multi()\n");
      break;
    }

    if (srslte_ue_sync_nbiot_get_sfidx(&q->ue_sync) == 0) {
      mib_ret = srslte_ue_mib_nbiot_decode(&q->ue_mib, NULL, bch_payload, nof_tx_ports, sfn_offset);
      if (mib_ret < 0) {
        DEBUG("Resetting NPBCH decoder after %d frames\n", q->ue_mib.frame_cnt);
        srslte_ue_mib_nbiot_reset(&q->ue_mib);
      }
      nof_frames++;
    }
  } while (mib_ret == SRSLTE_UE_MIB_NBIOT_NOTFOUND && nof_frames < max_frames_timeout);

  return mib_ret;
}
