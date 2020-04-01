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

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/fec/rm_turbo.h"
#include "srslte/phy/fec/softbuffer.h"
#include "srslte/phy/fec/turbodecoder_gen.h"
#include "srslte/phy/phch/ra.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"

#define MAX_PDSCH_RE(cp) (2 * SRSLTE_CP_NSYMB(cp) * 12)

int srslte_softbuffer_rx_init(srslte_softbuffer_rx_t* q, uint32_t nof_prb)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q != NULL) {
    bzero(q, sizeof(srslte_softbuffer_rx_t));

    ret = srslte_ra_tbs_from_idx(SRSLTE_RA_NOF_TBS_IDX - 1, nof_prb);
    if (ret != SRSLTE_ERROR) {
      q->max_cb = (uint32_t)ret / (SRSLTE_TCOD_MAX_LEN_CB - 24) + 1;
      ret       = SRSLTE_ERROR;

      q->buffer_f = srslte_vec_malloc(sizeof(int16_t*) * q->max_cb);
      if (!q->buffer_f) {
        perror("malloc");
        goto clean_exit;
      }

      q->data = srslte_vec_malloc(sizeof(uint8_t*) * q->max_cb);
      if (!q->data) {
        perror("malloc");
        goto clean_exit;
      }

      q->cb_crc = srslte_vec_malloc(sizeof(bool) * q->max_cb);
      if (!q->cb_crc) {
        perror("malloc");
        goto clean_exit;
      }
      bzero(q->cb_crc, sizeof(bool) * q->max_cb);

      // TODO: Use HARQ buffer limitation based on UE category
      for (uint32_t i = 0; i < q->max_cb; i++) {
        q->buffer_f[i] = srslte_vec_i16_malloc(SOFTBUFFER_SIZE);
        if (!q->buffer_f[i]) {
          perror("malloc");
          goto clean_exit;
        }

        q->data[i] = srslte_vec_u8_malloc(6144 / 8);
        if (!q->data[i]) {
          perror("malloc");
          goto clean_exit;
        }
      }
      // srslte_softbuffer_rx_reset(q);
      ret = SRSLTE_SUCCESS;
    }
  }

clean_exit:
  if (ret != SRSLTE_SUCCESS) {
    srslte_softbuffer_rx_free(q);
  }

  return ret;
}

void srslte_softbuffer_rx_free(srslte_softbuffer_rx_t* q)
{
  if (q) {
    if (q->buffer_f) {
      for (uint32_t i = 0; i < q->max_cb; i++) {
        if (q->buffer_f[i]) {
          free(q->buffer_f[i]);
        }
      }
      free(q->buffer_f);
    }
    if (q->data) {
      for (uint32_t i = 0; i < q->max_cb; i++) {
        if (q->data[i]) {
          free(q->data[i]);
        }
      }
      free(q->data);
    }
    if (q->cb_crc) {
      free(q->cb_crc);
    }
    bzero(q, sizeof(srslte_softbuffer_rx_t));
  }
}

void srslte_softbuffer_rx_reset_tbs(srslte_softbuffer_rx_t* q, uint32_t tbs)
{
  uint32_t nof_cb = (tbs + 24) / (SRSLTE_TCOD_MAX_LEN_CB - 24) + 1;
  srslte_softbuffer_rx_reset_cb(q, nof_cb);
}

void srslte_softbuffer_rx_reset(srslte_softbuffer_rx_t* q)
{
  srslte_softbuffer_rx_reset_cb(q, q->max_cb);
}

void srslte_softbuffer_rx_reset_cb(srslte_softbuffer_rx_t* q, uint32_t nof_cb)
{
  if (q->buffer_f) {
    if (nof_cb > q->max_cb) {
      nof_cb = q->max_cb;
    }
    for (uint32_t i = 0; i < nof_cb; i++) {
      if (q->buffer_f[i]) {
        bzero(q->buffer_f[i], SOFTBUFFER_SIZE * sizeof(int16_t));
      }
      if (q->data[i]) {
        bzero(q->data[i], sizeof(uint8_t) * 6144 / 8);
      }
    }
  }
  if (q->cb_crc) {
    bzero(q->cb_crc, sizeof(bool) * q->max_cb);
  }
  q->tb_crc = false;
}

int srslte_softbuffer_tx_init(srslte_softbuffer_tx_t* q, uint32_t nof_prb)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q != NULL) {
    bzero(q, sizeof(srslte_softbuffer_tx_t));

    ret = srslte_ra_tbs_from_idx(SRSLTE_RA_NOF_TBS_IDX - 1, nof_prb);
    if (ret != SRSLTE_ERROR) {
      q->max_cb = (uint32_t)ret / (SRSLTE_TCOD_MAX_LEN_CB - 24) + 1;

      q->buffer_b = srslte_vec_malloc(sizeof(uint8_t*) * q->max_cb);
      if (!q->buffer_b) {
        perror("malloc");
        return SRSLTE_ERROR;
      }

      // TODO: Use HARQ buffer limitation based on UE category
      for (uint32_t i = 0; i < q->max_cb; i++) {
        q->buffer_b[i] = srslte_vec_u8_malloc(SOFTBUFFER_SIZE);
        if (!q->buffer_b[i]) {
          perror("malloc");
          return SRSLTE_ERROR;
        }
      }
      srslte_softbuffer_tx_reset(q);
      ret = SRSLTE_SUCCESS;
    }
  }
  return ret;
}

void srslte_softbuffer_tx_free(srslte_softbuffer_tx_t* q)
{
  if (q) {
    if (q->buffer_b) {
      for (uint32_t i = 0; i < q->max_cb; i++) {
        if (q->buffer_b[i]) {
          free(q->buffer_b[i]);
        }
      }
      free(q->buffer_b);
    }
    bzero(q, sizeof(srslte_softbuffer_tx_t));
  }
}

void srslte_softbuffer_tx_reset_tbs(srslte_softbuffer_tx_t* q, uint32_t tbs)
{
  uint32_t nof_cb = (tbs + 24) / (SRSLTE_TCOD_MAX_LEN_CB - 24) + 1;
  srslte_softbuffer_tx_reset_cb(q, nof_cb);
}

void srslte_softbuffer_tx_reset(srslte_softbuffer_tx_t* q)
{
  srslte_softbuffer_tx_reset_cb(q, q->max_cb);
}

void srslte_softbuffer_tx_reset_cb(srslte_softbuffer_tx_t* q, uint32_t nof_cb)
{
  int i;
  if (q->buffer_b) {
    if (nof_cb > q->max_cb) {
      nof_cb = q->max_cb;
    }
    for (i = 0; i < nof_cb; i++) {
      if (q->buffer_b[i]) {
        bzero(q->buffer_b[i], sizeof(uint8_t) * SOFTBUFFER_SIZE);
      }
    }
  }
}
