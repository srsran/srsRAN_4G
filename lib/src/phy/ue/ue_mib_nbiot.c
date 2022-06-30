/**
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srsran/phy/ue/ue_mib_nbiot.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"
#include <stdlib.h>
#include <string.h>
#include <strings.h>

int srsran_ue_mib_nbiot_init(srsran_ue_mib_nbiot_t* q, cf_t** in_buffer, uint32_t max_prb)
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;

  if (q != NULL) {
    ret = SRSRAN_ERROR;
    bzero(q, sizeof(srsran_ue_mib_nbiot_t));

    if (srsran_npbch_init(&q->npbch)) {
      fprintf(stderr, "Error initiating NPBCH\n");
      goto clean_exit;
    }

    q->sf_symbols = srsran_vec_cf_malloc(SRSRAN_SF_LEN_RE(max_prb, SRSRAN_CP_NORM));
    if (!q->sf_symbols) {
      perror("malloc");
      goto clean_exit;
    }

    for (uint32_t i = 0; i < SRSRAN_MAX_PORTS; i++) {
      q->ce[i] = srsran_vec_cf_malloc(SRSRAN_SF_LEN_RE(max_prb, SRSRAN_CP_NORM));
      if (!q->ce[i]) {
        perror("malloc");
        goto clean_exit;
      }
    }

    if (srsran_ofdm_rx_init(&q->fft, SRSRAN_CP_NORM, in_buffer[0], q->sf_symbols, max_prb)) {
      fprintf(stderr, "Error initializing FFT\n");
      goto clean_exit;
    }
    srsran_ofdm_set_freq_shift(&q->fft, SRSRAN_NBIOT_FREQ_SHIFT_FACTOR);

    if (srsran_chest_dl_nbiot_init(&q->chest, max_prb)) {
      fprintf(stderr, "Error initializing reference signal\n");
      goto clean_exit;
    }
    srsran_ue_mib_nbiot_reset(q);

    ret = SRSRAN_SUCCESS;
  }

clean_exit:
  if (ret == SRSRAN_ERROR) {
    srsran_ue_mib_nbiot_free(q);
  }
  return ret;
}

void srsran_ue_mib_nbiot_free(srsran_ue_mib_nbiot_t* q)
{
  if (q->sf_symbols) {
    free(q->sf_symbols);
  }
  for (int i = 0; i < SRSRAN_MAX_PORTS; i++) {
    if (q->ce[i]) {
      free(q->ce[i]);
    }
  }
  srsran_sync_nbiot_free(&q->sfind);
  srsran_chest_dl_nbiot_free(&q->chest);
  srsran_npbch_free(&q->npbch);
  srsran_ofdm_rx_free(&q->fft);

  bzero(q, sizeof(srsran_ue_mib_nbiot_t));
}

int srsran_ue_mib_nbiot_set_cell(srsran_ue_mib_nbiot_t* q, srsran_nbiot_cell_t cell)
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;

  if (q != NULL && cell.nof_ports <= SRSRAN_NBIOT_MAX_PORTS) {
    ret = SRSRAN_ERROR;

    if (srsran_npbch_set_cell(&q->npbch, cell)) {
      fprintf(stderr, "Error setting cell in NPBCH\n");
      goto clean_exit;
    }

    if (cell.nof_ports == 0) {
      cell.nof_ports = SRSRAN_NBIOT_MAX_PORTS;
    }

    if (srsran_chest_dl_nbiot_set_cell(&q->chest, cell)) {
      fprintf(stderr, "Error initializing reference signal\n");
      goto clean_exit;
    }
    srsran_ue_mib_nbiot_reset(q);

    ret = SRSRAN_SUCCESS;
  }

clean_exit:
  if (ret == SRSRAN_ERROR) {
    srsran_ue_mib_nbiot_free(q);
  }
  return ret;
}

void srsran_ue_mib_nbiot_reset(srsran_ue_mib_nbiot_t* q)
{
  q->frame_cnt = 0;
  srsran_npbch_decode_reset(&q->npbch);
}

int srsran_ue_mib_nbiot_decode(srsran_ue_mib_nbiot_t* q,
                               cf_t*                  input,
                               uint8_t*               bch_payload,
                               uint32_t*              nof_tx_ports,
                               int*                   sfn_offset)
{
  // Run FFT for the symbols
  srsran_ofdm_rx_sf(&q->fft);

  // Get channel estimates of sf idx #0 for each port
  if (srsran_chest_dl_nbiot_estimate(&q->chest, q->sf_symbols, q->ce, 0) < 0) {
    return SRSRAN_ERROR;
  }

  // Reset decoder if we missed a NPBCH TTI
  if (q->frame_cnt > SRSRAN_NPBCH_NUM_FRAMES) {
    INFO("Resetting NPBCH decoder after %d frames", q->frame_cnt);
    srsran_ue_mib_nbiot_reset(q);
    return SRSRAN_UE_MIB_NBIOT_NOTFOUND;
  }

  // Decode NPBCH
  if (SRSRAN_SUCCESS == srsran_npbch_decode_nf(&q->npbch,
                                               q->sf_symbols,
                                               q->ce,
                                               srsran_chest_dl_nbiot_get_noise_estimate(&q->chest),
                                               bch_payload,
                                               nof_tx_ports,
                                               sfn_offset,
                                               0)) {
    DEBUG("BCH decoded ok with offset %d", *sfn_offset);
    if (memcmp(bch_payload, q->last_bch_payload, SRSRAN_MIB_NB_LEN) == 0) {
      DEBUG("BCH content equals last BCH, new counter %d", q->frame_cnt);
    } else {
      // new BCH transmitted
      if (q->frame_cnt != 0) {
        INFO("MIB-NB decoded: %u with offset %d", q->frame_cnt, *sfn_offset);
        if (*sfn_offset != 0) {
          INFO("New BCH was decoded at block offset %d. SFN may be corrupted.", *sfn_offset);
        }
        srsran_ue_mib_nbiot_reset(q);
        return SRSRAN_UE_MIB_NBIOT_FOUND;
      } else {
        // store new BCH
        DEBUG("New BCH transmitted after %d frames", q->frame_cnt);
        memcpy(q->last_bch_payload, bch_payload, SRSRAN_MIB_NB_LEN);
      }
    }
    q->frame_cnt++;
  }
  return SRSRAN_UE_MIB_NBIOT_NOTFOUND;
}

int srsran_ue_mib_sync_nbiot_init_multi(
    srsran_ue_mib_sync_nbiot_t* q,
    int(recv_callback)(void*, cf_t* [SRSRAN_MAX_PORTS], uint32_t, srsran_timestamp_t*),
    uint32_t nof_rx_antennas,
    void*    stream_handler)
{
  // Allocate memory for time re-alignment and MIB detection
  for (int i = 0; i < SRSRAN_MAX_PORTS; i++) {
    q->sf_buffer[i] = NULL;
  }
  for (int i = 0; i < nof_rx_antennas; i++) {
    q->sf_buffer[i] = srsran_vec_cf_malloc(SRSRAN_SF_LEN_PRB_NBIOT * SRSRAN_NOF_SF_X_FRAME);
  }
  q->nof_rx_antennas = nof_rx_antennas;

  if (srsran_ue_mib_nbiot_init(&q->ue_mib, q->sf_buffer, SRSRAN_NBIOT_MAX_PRB)) {
    fprintf(stderr, "Error initiating ue_mib\n");
    return SRSRAN_ERROR;
  }
  if (srsran_ue_sync_nbiot_init_multi(
          &q->ue_sync, SRSRAN_NBIOT_MAX_PRB, recv_callback, q->nof_rx_antennas, stream_handler)) {
    fprintf(stderr, "Error initiating ue_sync\n");
    srsran_ue_mib_nbiot_free(&q->ue_mib);
    return SRSRAN_ERROR;
  }
  return SRSRAN_SUCCESS;
}

int srsran_ue_mib_sync_nbiot_set_cell(srsran_ue_mib_sync_nbiot_t* q, srsran_nbiot_cell_t cell)
{
  if (srsran_ue_mib_nbiot_set_cell(&q->ue_mib, cell)) {
    fprintf(stderr, "Error initiating ue_mib\n");
    return SRSRAN_ERROR;
  }
  if (srsran_ue_sync_nbiot_set_cell(&q->ue_sync, cell)) {
    fprintf(stderr, "Error initiating ue_sync\n");
    srsran_ue_mib_nbiot_free(&q->ue_mib);
    return SRSRAN_ERROR;
  }
  return SRSRAN_SUCCESS;
}

void srsran_ue_mib_sync_nbiot_free(srsran_ue_mib_sync_nbiot_t* q)
{
  srsran_ue_mib_nbiot_free(&q->ue_mib);
  srsran_ue_sync_nbiot_free(&q->ue_sync);
}

void srsran_ue_mib_sync_nbiot_reset(srsran_ue_mib_sync_nbiot_t* q)
{
  srsran_ue_mib_nbiot_reset(&q->ue_mib);
  srsran_ue_sync_nbiot_reset(&q->ue_sync);
}

int srsran_ue_mib_sync_nbiot_decode(srsran_ue_mib_sync_nbiot_t* q,
                                    uint32_t                    max_frames_timeout,
                                    uint8_t*                    bch_payload,
                                    uint32_t*                   nof_tx_ports,
                                    int*                        sfn_offset)
{
  int      ret        = SRSRAN_ERROR_INVALID_INPUTS;
  uint32_t nof_frames = 0;
  int      mib_ret    = SRSRAN_UE_MIB_NBIOT_NOTFOUND;

  if (q == NULL) {
    return ret;
  }

  do {
    mib_ret = SRSRAN_UE_MIB_NBIOT_NOTFOUND;
    ret     = srsran_ue_sync_nbiot_zerocopy_multi(&q->ue_sync, q->sf_buffer);
    if (ret < 0) {
      fprintf(stderr, "Error calling srsran_ue_sync_nbiot_zerocopy_multi()\n");
      break;
    }

    if (srsran_ue_sync_nbiot_get_sfidx(&q->ue_sync) == 0) {
      mib_ret = srsran_ue_mib_nbiot_decode(&q->ue_mib, NULL, bch_payload, nof_tx_ports, sfn_offset);
      if (mib_ret < 0) {
        DEBUG("Resetting NPBCH decoder after %d frames", q->ue_mib.frame_cnt);
        srsran_ue_mib_nbiot_reset(&q->ue_mib);
      }
      nof_frames++;
    }
  } while (mib_ret == SRSRAN_UE_MIB_NBIOT_NOTFOUND && nof_frames < max_frames_timeout);

  return mib_ret;
}
