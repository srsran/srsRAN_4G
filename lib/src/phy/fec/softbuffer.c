/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/fec/softbuffer.h"
#include "srsran/phy/fec/turbo/turbodecoder_gen.h"
#include "srsran/phy/phch/ra.h"
#include "srsran/phy/utils/vector.h"

#define MAX_PDSCH_RE(cp) (2 * SRSRAN_CP_NSYMB(cp) * 12)

int srsran_softbuffer_rx_init(srsran_softbuffer_rx_t* q, uint32_t nof_prb)
{
  int ret = srsran_ra_tbs_from_idx(SRSRAN_RA_NOF_TBS_IDX - 1, nof_prb);

  if (ret == SRSRAN_ERROR) {
    return SRSRAN_ERROR;
  }
  uint32_t max_cb      = (uint32_t)ret / (SRSRAN_TCOD_MAX_LEN_CB - 24) + 1;
  uint32_t max_cb_size = SOFTBUFFER_SIZE;

  return srsran_softbuffer_rx_init_guru(q, max_cb, max_cb_size);
}

int srsran_softbuffer_rx_init_guru(srsran_softbuffer_rx_t* q, uint32_t max_cb, uint32_t max_cb_size)
{
  int ret = SRSRAN_ERROR;

  // Protect pointer
  if (!q) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Initialise object
  SRSRAN_MEM_ZERO(q, srsran_softbuffer_rx_t, 1);

  // Set internal attributes
  q->max_cb      = max_cb;
  q->max_cb_size = max_cb_size;

  q->buffer_f = SRSRAN_MEM_ALLOC(int16_t*, q->max_cb);
  if (!q->buffer_f) {
    perror("malloc");
    goto clean_exit;
  }
  SRSRAN_MEM_ZERO(q->buffer_f, int16_t*, q->max_cb);

  q->data = SRSRAN_MEM_ALLOC(uint8_t*, q->max_cb);
  if (!q->data) {
    perror("malloc");
    goto clean_exit;
  }
  SRSRAN_MEM_ZERO(q->data, uint8_t*, q->max_cb);

  q->cb_crc = SRSRAN_MEM_ALLOC(bool, q->max_cb);
  if (!q->cb_crc) {
    perror("malloc");
    goto clean_exit;
  }

  for (uint32_t i = 0; i < q->max_cb; i++) {
    q->buffer_f[i] = srsran_vec_i16_malloc(q->max_cb_size);
    if (!q->buffer_f[i]) {
      perror("malloc");
      goto clean_exit;
    }

    q->data[i] = srsran_vec_u8_malloc(q->max_cb_size / 8);
    if (!q->data[i]) {
      perror("malloc");
      goto clean_exit;
    }
  }

  srsran_softbuffer_rx_reset(q);

  // Consider success
  ret = SRSRAN_SUCCESS;

clean_exit:
  if (ret) {
    srsran_softbuffer_rx_free(q);
  }

  return ret;
}

void srsran_softbuffer_rx_free(srsran_softbuffer_rx_t* q)
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

    SRSRAN_MEM_ZERO(q, srsran_softbuffer_rx_t, 1);
  }
}

void srsran_softbuffer_rx_reset_tbs(srsran_softbuffer_rx_t* q, uint32_t tbs)
{
  uint32_t nof_cb = (tbs + 24) / (SRSRAN_TCOD_MAX_LEN_CB - 24) + 1;
  srsran_softbuffer_rx_reset_cb(q, SRSRAN_MIN(nof_cb, q->max_cb));
}

void srsran_softbuffer_rx_reset(srsran_softbuffer_rx_t* q)
{
  srsran_softbuffer_rx_reset_cb(q, q->max_cb);
}

void srsran_softbuffer_rx_reset_cb(srsran_softbuffer_rx_t* q, uint32_t nof_cb)
{
  if (q->buffer_f) {
    if (nof_cb > q->max_cb) {
      nof_cb = q->max_cb;
    }
    for (uint32_t i = 0; i < nof_cb; i++) {
      if (q->buffer_f[i]) {
        srsran_vec_i16_zero(q->buffer_f[i], q->max_cb_size);
      }
      if (q->data[i]) {
        srsran_vec_u8_zero(q->data[i], q->max_cb_size / 8);
      }
    }
  }
  if (q->cb_crc) {
    SRSRAN_MEM_ZERO(q->cb_crc, bool, q->max_cb);
  }
  q->tb_crc = false;
}

void srsran_softbuffer_rx_reset_cb_crc(srsran_softbuffer_rx_t* q, uint32_t nof_cb)
{
  if (q == NULL || nof_cb == 0) {
    return;
  }

  SRSRAN_MEM_ZERO(q->cb_crc, bool, SRSRAN_MIN(q->max_cb, nof_cb));
}

int srsran_softbuffer_tx_init(srsran_softbuffer_tx_t* q, uint32_t nof_prb)
{
  int ret = srsran_ra_tbs_from_idx(SRSRAN_RA_NOF_TBS_IDX - 1, nof_prb);
  if (ret == SRSRAN_ERROR) {
    return SRSRAN_ERROR;
  }
  uint32_t max_cb      = (uint32_t)ret / (SRSRAN_TCOD_MAX_LEN_CB - 24) + 1;
  uint32_t max_cb_size = SOFTBUFFER_SIZE;

  return srsran_softbuffer_tx_init_guru(q, max_cb, max_cb_size);
}

int srsran_softbuffer_tx_init_guru(srsran_softbuffer_tx_t* q, uint32_t max_cb, uint32_t max_cb_size)
{
  // Protect pointer
  if (!q) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Initialise object
  SRSRAN_MEM_ZERO(q, srsran_softbuffer_tx_t, 1);

  // Set internal attributes
  q->max_cb      = max_cb;
  q->max_cb_size = max_cb_size;

  q->buffer_b = SRSRAN_MEM_ALLOC(uint8_t*, q->max_cb);
  if (!q->buffer_b) {
    perror("malloc");
    return SRSRAN_ERROR;
  }
  SRSRAN_MEM_ZERO(q->buffer_b, uint8_t*, q->max_cb);

  // TODO: Use HARQ buffer limitation based on UE category
  for (uint32_t i = 0; i < q->max_cb; i++) {
    q->buffer_b[i] = srsran_vec_u8_malloc(q->max_cb_size);
    if (!q->buffer_b[i]) {
      perror("malloc");
      return SRSRAN_ERROR;
    }
  }

  srsran_softbuffer_tx_reset(q);

  return SRSRAN_SUCCESS;
}

void srsran_softbuffer_tx_free(srsran_softbuffer_tx_t* q)
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
    SRSRAN_MEM_ZERO(q, srsran_softbuffer_tx_t, 1);
  }
}

void srsran_softbuffer_tx_reset_tbs(srsran_softbuffer_tx_t* q, uint32_t tbs)
{
  uint32_t nof_cb = (tbs + 24) / (SRSRAN_TCOD_MAX_LEN_CB - 24) + 1;
  srsran_softbuffer_tx_reset_cb(q, nof_cb);
}

void srsran_softbuffer_tx_reset(srsran_softbuffer_tx_t* q)
{
  srsran_softbuffer_tx_reset_cb(q, q->max_cb);
}

void srsran_softbuffer_tx_reset_cb(srsran_softbuffer_tx_t* q, uint32_t nof_cb)
{
  if (q->buffer_b) {
    if (nof_cb > q->max_cb) {
      nof_cb = q->max_cb;
    }
    for (uint32_t i = 0; i < nof_cb; i++) {
      if (q->buffer_b[i]) {
        srsran_vec_u8_zero(q->buffer_b[i], q->max_cb_size);
      }
    }
  }
}
