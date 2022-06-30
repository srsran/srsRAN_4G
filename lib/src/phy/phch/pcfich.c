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

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/phch/pcfich.h"
#include "srsran/phy/phch/regs.h"
#include "srsran/phy/utils/bit.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"

// Table 5.3.4-1
static uint8_t cfi_table[4][PCFICH_CFI_LEN] = {
    {0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1},
    {1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0},
    {1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} // reserved
};

bool srsran_pcfich_exists(int nframe, int nslot)
{
  return true;
}

/** Initializes the pcfich channel receiver.
 * On ERROR returns -1 and frees the structrure
 */
int srsran_pcfich_init(srsran_pcfich_t* q, uint32_t nof_rx_antennas)
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;

  if (q != NULL) {
    ret = SRSRAN_ERROR;

    bzero(q, sizeof(srsran_pcfich_t));
    q->nof_rx_antennas = nof_rx_antennas;
    q->nof_symbols     = PCFICH_RE;

    if (srsran_modem_table_lte(&q->mod, SRSRAN_MOD_QPSK)) {
      goto clean;
    }

    /* convert cfi bit tables to floats for demodulation */
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < PCFICH_CFI_LEN; j++) {
        q->cfi_table_float[i][j] = (float)2.0 * cfi_table[i][j] - 1.0;
      }
    }

    ret = SRSRAN_SUCCESS;
  }

clean:
  if (ret == SRSRAN_ERROR) {
    srsran_pcfich_free(q);
  }
  return ret;
}

void srsran_pcfich_free(srsran_pcfich_t* q)
{
  for (int ns = 0; ns < SRSRAN_NOF_SF_X_FRAME; ns++) {
    srsran_sequence_free(&q->seq[ns]);
  }
  srsran_modem_table_free(&q->mod);

  bzero(q, sizeof(srsran_pcfich_t));
}

int srsran_pcfich_set_cell(srsran_pcfich_t* q, srsran_regs_t* regs, srsran_cell_t cell)
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;

  if (q != NULL && regs != NULL && srsran_cell_isvalid(&cell)) {
    q->regs = regs;
    if (cell.id != q->cell.id || q->cell.nof_prb == 0) {
      q->cell = cell;
      for (int nsf = 0; nsf < SRSRAN_NOF_SF_X_FRAME; nsf++) {
        if (srsran_sequence_pcfich(&q->seq[nsf], 2 * nsf, q->cell.id)) {
          return SRSRAN_ERROR;
        }
      }
    }
    ret = SRSRAN_SUCCESS;
  }
  return ret;
}

/** Finds the CFI with minimum distance with the vector of received 32 bits.
 * Saves the CFI value in the cfi pointer and returns the distance.
 */
float srsran_pcfich_cfi_decode(srsran_pcfich_t* q, uint32_t* cfi)
{
  int   i;
  int   index    = 0;
  float max_corr = 0;
  float corr[3];

  for (i = 0; i < 3; i++) {
    corr[i] = srsran_vec_dot_prod_fff(q->cfi_table_float[i], q->data_f, PCFICH_CFI_LEN);
    if (corr[i] > max_corr) {
      max_corr = corr[i];
      index    = i;
    }
  }

  if (cfi) {
    *cfi = index + 1;
  }
  return max_corr;
}

/** Encodes the CFI producing a vector of 32 bits.
 *  36.211 10.3 section 5.3.4
 */
int srsran_pcfich_cfi_encode(uint32_t cfi, uint8_t bits[PCFICH_CFI_LEN])
{
  if (cfi < 1 || cfi > 3) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  } else {
    memcpy(bits, cfi_table[cfi - 1], PCFICH_CFI_LEN * sizeof(uint8_t));
    return SRSRAN_SUCCESS;
  }
}

/* Decodes the PCFICH channel and saves the CFI in the cfi pointer.
 *
 * Returns 1 if successfully decoded the CFI, 0 if not and -1 on error
 */
int srsran_pcfich_decode(srsran_pcfich_t*       q,
                         srsran_dl_sf_cfg_t*    sf,
                         srsran_chest_dl_res_t* channel,
                         cf_t*                  sf_symbols[SRSRAN_MAX_PORTS],
                         float*                 corr_result)
{
  /* Set pointers for layermapping & precoding */
  int   i;
  cf_t* x[SRSRAN_MAX_LAYERS];

  if (q != NULL && sf_symbols != NULL) {
    uint32_t sf_idx = sf->tti % 10;

    /* number of layers equals number of ports */
    for (i = 0; i < SRSRAN_MAX_PORTS; i++) {
      x[i] = q->x[i];
    }

    cf_t* q_symbols[SRSRAN_MAX_PORTS];
    cf_t* q_ce[SRSRAN_MAX_PORTS][SRSRAN_MAX_PORTS];

    /* extract symbols */
    for (int j = 0; j < q->nof_rx_antennas; j++) {
      if (q->nof_symbols != srsran_regs_pcfich_get(q->regs, sf_symbols[j], q->symbols[j])) {
        ERROR("There was an error getting the PCFICH symbols");
        return SRSRAN_ERROR;
      }

      q_symbols[j] = q->symbols[j];

      /* extract channel estimates */
      for (i = 0; i < q->cell.nof_ports; i++) {
        if (q->nof_symbols != srsran_regs_pcfich_get(q->regs, channel->ce[i][j], q->ce[i][j])) {
          ERROR("There was an error getting the PCFICH symbols");
          return SRSRAN_ERROR;
        }
        q_ce[i][j] = q->ce[i][j];
      }
    }

    /* in control channels, only diversity is supported */
    if (q->cell.nof_ports == 1) {
      /* no need for layer demapping */
      srsran_predecoding_single_multi(
          q_symbols, q_ce[0], q->d, NULL, q->nof_rx_antennas, q->nof_symbols, 1.0f, channel->noise_estimate);
    } else {
      srsran_predecoding_diversity_multi(
          q_symbols, q_ce, x, NULL, q->nof_rx_antennas, q->cell.nof_ports, q->nof_symbols, 1.0f);
      srsran_layerdemap_diversity(x, q->d, q->cell.nof_ports, q->nof_symbols / q->cell.nof_ports);
    }

    /* demodulate symbols */
    srsran_demod_soft_demodulate(SRSRAN_MOD_QPSK, q->d, q->data_f, q->nof_symbols);

    /* Scramble with the sequence for slot nslot */
    srsran_scrambling_f(&q->seq[sf_idx], q->data_f);

    /* decode CFI */
    float corr = srsran_pcfich_cfi_decode(q, &sf->cfi);
    if (corr_result) {
      *corr_result = corr;
    }
    return 1;
  } else {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }
}

/** Encodes CFI and maps symbols to the slot
 */
int srsran_pcfich_encode(srsran_pcfich_t* q, srsran_dl_sf_cfg_t* sf, cf_t* slot_symbols[SRSRAN_MAX_PORTS])
{
  int i;

  if (q != NULL && slot_symbols != NULL) {
    uint32_t sf_idx = sf->tti % 10;

    /* Set pointers for layermapping & precoding */
    cf_t* x[SRSRAN_MAX_LAYERS];
    cf_t* q_symbols[SRSRAN_MAX_PORTS];

    /* number of layers equals number of ports */
    for (i = 0; i < q->cell.nof_ports; i++) {
      x[i] = q->x[i];
    }
    for (i = 0; i < SRSRAN_MAX_PORTS; i++) {
      q_symbols[i] = q->symbols[i];
    }

    /* pack CFI */
    srsran_pcfich_cfi_encode(sf->cfi, q->data);

    /* scramble for slot sequence nslot */
    srsran_scrambling_b(&q->seq[sf_idx], q->data);

    srsran_mod_modulate(&q->mod, q->data, q->d, PCFICH_CFI_LEN);

    /* layer mapping & precoding */
    if (q->cell.nof_ports > 1) {
      srsran_layermap_diversity(q->d, x, q->cell.nof_ports, q->nof_symbols);
      srsran_precoding_diversity(x, q_symbols, q->cell.nof_ports, q->nof_symbols / q->cell.nof_ports, 1.0f);
    } else {
      memcpy(q->symbols[0], q->d, q->nof_symbols * sizeof(cf_t));
    }

    /* mapping to resource elements */
    for (i = 0; i < q->cell.nof_ports; i++) {
      if (srsran_regs_pcfich_put(q->regs, q->symbols[i], slot_symbols[i]) < 0) {
        ERROR("Error putting PCHICH resource elements");
        return SRSRAN_ERROR;
      }
    }
    return SRSRAN_SUCCESS;
  } else {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }
}
