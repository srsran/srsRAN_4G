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

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/fec/softbuffer.h"
#include "srslte/phy/fec/turbo/turbodecoder_gen.h"
#include "srslte/phy/phch/ra.h"
#include "srslte/phy/utils/vector.h"

#define MAX_PDSCH_RE(cp) (2 * SRSLTE_CP_NSYMB(cp) * 12)

int srslte_softbuffer_rx_init(srslte_softbuffer_rx_t* q, uint32_t nof_prb)
{
  int ret = srslte_ra_tbs_from_idx(SRSLTE_RA_NOF_TBS_IDX - 1, nof_prb);

  if (ret == SRSLTE_ERROR) {
    return SRSLTE_ERROR;
  }
  uint32_t max_cb      = (uint32_t)ret / (SRSLTE_TCOD_MAX_LEN_CB - 24) + 1;
  uint32_t max_cb_size = SOFTBUFFER_SIZE;

  return srslte_softbuffer_rx_init_guru(q, max_cb, max_cb_size);
}

int srslte_softbuffer_rx_init_guru(srslte_softbuffer_rx_t* q, uint32_t max_cb, uint32_t max_cb_size)
{
  int ret = SRSLTE_ERROR;

  // Protect pointer
  if (!q) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  // Initialise object
  SRSLTE_MEM_ZERO(q, srslte_softbuffer_rx_t, 1);

  // Set internal attributes
  q->max_cb      = max_cb;
  q->max_cb_size = max_cb_size;

  q->buffer_f = SRSLTE_MEM_ALLOC(int16_t*, q->max_cb);
  if (!q->buffer_f) {
    perror("malloc");
    goto clean_exit;
  }
  SRSLTE_MEM_ZERO(q->buffer_f, int16_t*, q->max_cb);

  q->data = SRSLTE_MEM_ALLOC(uint8_t*, q->max_cb);
  if (!q->data) {
    perror("malloc");
    goto clean_exit;
  }
  SRSLTE_MEM_ZERO(q->data, uint8_t*, q->max_cb);

  q->cb_crc = SRSLTE_MEM_ALLOC(bool, q->max_cb);
  if (!q->cb_crc) {
    perror("malloc");
    goto clean_exit;
  }

  for (uint32_t i = 0; i < q->max_cb; i++) {
    q->buffer_f[i] = srslte_vec_i16_malloc(q->max_cb_size);
    if (!q->buffer_f[i]) {
      perror("malloc");
      goto clean_exit;
    }

    q->data[i] = srslte_vec_u8_malloc(q->max_cb_size / 8);
    if (!q->data[i]) {
      perror("malloc");
      goto clean_exit;
    }
  }

  srslte_softbuffer_rx_reset(q);

  // Consider success
  ret = SRSLTE_SUCCESS;

clean_exit:
  if (ret) {
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

    SRSLTE_MEM_ZERO(q, srslte_softbuffer_rx_t, 1);
  }
}

void srslte_softbuffer_rx_reset_tbs(srslte_softbuffer_rx_t* q, uint32_t tbs)
{
  uint32_t nof_cb = (tbs + 24) / (SRSLTE_TCOD_MAX_LEN_CB - 24) + 1;
  srslte_softbuffer_rx_reset_cb(q, SRSLTE_MIN(nof_cb, q->max_cb));
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
        srslte_vec_i16_zero(q->buffer_f[i], q->max_cb_size);
      }
      if (q->data[i]) {
        srslte_vec_u8_zero(q->data[i], q->max_cb_size / 8);
      }
    }
  }
  if (q->cb_crc) {
    SRSLTE_MEM_ZERO(q->cb_crc, bool, q->max_cb);
  }
  q->tb_crc = false;
}

int srslte_softbuffer_tx_init(srslte_softbuffer_tx_t* q, uint32_t nof_prb)
{
  int ret = srslte_ra_tbs_from_idx(SRSLTE_RA_NOF_TBS_IDX - 1, nof_prb);
  if (ret == SRSLTE_ERROR) {
    return SRSLTE_ERROR;
  }
  uint32_t max_cb      = (uint32_t)ret / (SRSLTE_TCOD_MAX_LEN_CB - 24) + 1;
  uint32_t max_cb_size = SOFTBUFFER_SIZE;

  return srslte_softbuffer_tx_init_guru(q, max_cb, max_cb_size);
}

int srslte_softbuffer_tx_init_guru(srslte_softbuffer_tx_t* q, uint32_t max_cb, uint32_t max_cb_size)
{
  // Protect pointer
  if (!q) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  // Initialise object
  SRSLTE_MEM_ZERO(q, srslte_softbuffer_tx_t, 1);

  // Set internal attributes
  q->max_cb      = max_cb;
  q->max_cb_size = max_cb_size;

  q->buffer_b = SRSLTE_MEM_ALLOC(uint8_t*, q->max_cb);
  if (!q->buffer_b) {
    perror("malloc");
    return SRSLTE_ERROR;
  }
  SRSLTE_MEM_ZERO(q->buffer_b, uint8_t*, q->max_cb);

  // TODO: Use HARQ buffer limitation based on UE category
  for (uint32_t i = 0; i < q->max_cb; i++) {
    q->buffer_b[i] = srslte_vec_u8_malloc(q->max_cb_size);
    if (!q->buffer_b[i]) {
      perror("malloc");
      return SRSLTE_ERROR;
    }
  }

  srslte_softbuffer_tx_reset(q);

  return SRSLTE_SUCCESS;
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
    SRSLTE_MEM_ZERO(q, srslte_softbuffer_tx_t, 1);
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
  if (q->buffer_b) {
    if (nof_cb > q->max_cb) {
      nof_cb = q->max_cb;
    }
    for (uint32_t i = 0; i < nof_cb; i++) {
      if (q->buffer_b[i]) {
        srslte_vec_u8_zero(q->buffer_b[i], q->max_cb_size);
      }
    }
  }
}
