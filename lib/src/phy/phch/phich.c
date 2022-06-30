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
#include <complex.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/phch/phich.h"
#include "srsran/phy/phch/regs.h"
#include "srsran/phy/utils/bit.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"

/** Table 6.9.1-2 */
const cf_t w_normal[SRSRAN_PHICH_NORM_NSEQUENCES][4] = {{1, 1, 1, 1},
                                                        {1, -1, 1, -1},
                                                        {1, 1, -1, -1},
                                                        {1, -1, -1, 1},
                                                        {I, I, I, I},
                                                        {I, -I, I, -I},
                                                        {I, I, -I, -I},
                                                        {I, -I, -I, I}};
const cf_t w_ext[SRSRAN_PHICH_EXT_NSEQUENCES][2]     = {{1, 1}, {1, -1}, {I, I}, {I, -I}};

uint32_t srsran_phich_ngroups(srsran_phich_t* q)
{
  return srsran_regs_phich_ngroups(q->regs);
}

uint32_t srsran_phich_nsf(srsran_phich_t* q)
{
  if (SRSRAN_CP_ISNORM(q->cell.cp)) {
    return SRSRAN_PHICH_NORM_NSF;
  } else {
    return SRSRAN_PHICH_EXT_NSF;
  }
}

void srsran_phich_reset(srsran_phich_t* q, cf_t* slot_symbols[SRSRAN_MAX_PORTS])
{
  int i;
  for (i = 0; i < SRSRAN_MAX_PORTS; i++) {
    srsran_regs_phich_reset(q->regs, slot_symbols[i]);
  }
}

/** Initializes the phich channel receiver */
int srsran_phich_init(srsran_phich_t* q, uint32_t nof_rx_antennas)
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;

  if (q != NULL) {
    bzero(q, sizeof(srsran_phich_t));
    ret = SRSRAN_ERROR;

    q->nof_rx_antennas = nof_rx_antennas;

    if (srsran_modem_table_lte(&q->mod, SRSRAN_MOD_BPSK)) {
      goto clean;
    }
    ret = SRSRAN_SUCCESS;
  }
clean:
  if (ret == SRSRAN_ERROR) {
    srsran_phich_free(q);
  }
  return ret;
}

void srsran_phich_free(srsran_phich_t* q)
{
  for (int ns = 0; ns < SRSRAN_NOF_SF_X_FRAME; ns++) {
    srsran_sequence_free(&q->seq[ns]);
  }
  srsran_modem_table_free(&q->mod);

  bzero(q, sizeof(srsran_phich_t));
}

void srsran_phich_set_regs(srsran_phich_t* q, srsran_regs_t* regs)
{
  q->regs = regs;
}

int srsran_phich_set_cell(srsran_phich_t* q, srsran_regs_t* regs, srsran_cell_t cell)
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;

  if (q != NULL && regs != NULL && srsran_cell_isvalid(&cell)) {
    q->regs = regs;

    if (cell.id != q->cell.id || q->cell.nof_prb == 0) {
      q->cell = cell;
      for (int nsf = 0; nsf < SRSRAN_NOF_SF_X_FRAME; nsf++) {
        if (srsran_sequence_phich(&q->seq[nsf], 2 * nsf, q->cell.id)) {
          return SRSRAN_ERROR;
        }
      }
    }
    ret = SRSRAN_SUCCESS;
  }
  return ret;
}

/* Computes n_group and n_seq according to Section 9.1.2 in 36.213 */
void srsran_phich_calc(srsran_phich_t* q, srsran_phich_grant_t* grant, srsran_phich_resource_t* n_phich)
{
  uint32_t Ngroups = srsran_regs_phich_ngroups_m1(q->regs);
  if (Ngroups) {
    if (n_phich) {
      n_phich->ngroup = (grant->n_prb_lowest + grant->n_dmrs) % Ngroups + grant->I_phich * Ngroups;
      n_phich->nseq   = ((grant->n_prb_lowest / Ngroups) + grant->n_dmrs) % (2 * srsran_phich_nsf(q));
    }
  } else {
    ERROR("PHICH: Error computing PHICH groups. Ngroups is zero");
  }
}

/* Decodes ACK
 *
 */
uint8_t srsran_phich_ack_decode(float bits[SRSRAN_PHICH_NBITS], float* distance)
{
  int     i;
  float   ack_table[2][3] = {{-1.0, -1.0, -1.0}, {1.0, 1.0, 1.0}};
  float   max_corr        = -9999;
  uint8_t index           = 0;

  if (SRSRAN_VERBOSE_ISINFO()) {
    INFO("Received bits: ");
    srsran_vec_fprint_f(stdout, bits, SRSRAN_PHICH_NBITS);
  }

  for (i = 0; i < 2; i++) {
    float corr = srsran_vec_dot_prod_fff(ack_table[i], bits, SRSRAN_PHICH_NBITS) / SRSRAN_PHICH_NBITS;
    INFO("Corr%d=%f", i, corr);
    if (corr > max_corr) {
      max_corr = corr;
      if (distance) {
        *distance = max_corr;
      }
      index = i;
    }
  }
  return index;
}

/** Encodes the ACK
 *  36.212
 */
void srsran_phich_ack_encode(uint8_t ack, uint8_t bits[SRSRAN_PHICH_NBITS])
{
  memset(bits, ack, 3 * sizeof(uint8_t));
}

int srsran_phich_decode(srsran_phich_t*         q,
                        srsran_dl_sf_cfg_t*     sf,
                        srsran_chest_dl_res_t*  channel,
                        srsran_phich_resource_t n_phich,
                        cf_t*                   sf_symbols[SRSRAN_MAX_PORTS],
                        srsran_phich_res_t*     result)
{
  /* Set pointers for layermapping & precoding */
  int   i, j;
  cf_t* x[SRSRAN_MAX_LAYERS];

  if (q == NULL || sf_symbols == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  uint32_t sf_idx = sf->tti % 10;

  if (SRSRAN_CP_ISEXT(q->cell.cp)) {
    if (n_phich.nseq >= SRSRAN_PHICH_EXT_NSEQUENCES) {
      ERROR("Invalid nseq %d", n_phich.nseq);
      return SRSRAN_ERROR_INVALID_INPUTS;
    }
  } else {
    if (n_phich.nseq >= SRSRAN_PHICH_NORM_NSEQUENCES) {
      ERROR("Invalid nseq %d", n_phich.nseq);
      return SRSRAN_ERROR_INVALID_INPUTS;
    }
  }
  if (n_phich.ngroup >= srsran_regs_phich_ngroups(q->regs)) {
    ERROR("Invalid ngroup %d", n_phich.ngroup);
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  DEBUG("Decoding PHICH Ngroup: %d, Nseq: %d", n_phich.ngroup, n_phich.nseq);

  /* number of layers equals number of ports */
  for (i = 0; i < SRSRAN_MAX_PORTS; i++) {
    x[i] = q->x[i];
  }

  cf_t* q_ce[SRSRAN_MAX_PORTS][SRSRAN_MAX_PORTS];
  cf_t* q_sf_symbols[SRSRAN_MAX_PORTS];

  /* extract symbols */
  for (int j = 0; j < q->nof_rx_antennas; j++) {
    if (SRSRAN_PHICH_MAX_NSYMB != srsran_regs_phich_get(q->regs, sf_symbols[j], q->sf_symbols[j], n_phich.ngroup)) {
      ERROR("There was an error getting the phich symbols");
      return SRSRAN_ERROR;
    }
    q_sf_symbols[j] = q->sf_symbols[j];

    /* extract channel estimates */
    for (i = 0; i < q->cell.nof_ports; i++) {
      if (SRSRAN_PHICH_MAX_NSYMB != srsran_regs_phich_get(q->regs, channel->ce[i][j], q->ce[i][j], n_phich.ngroup)) {
        ERROR("There was an error getting the phich symbols");
        return SRSRAN_ERROR;
      }
      q_ce[i][j] = q->ce[i][j];
    }
  }

  /* in control channels, only diversity is supported */
  if (q->cell.nof_ports == 1) {
    /* no need for layer demapping */
    srsran_predecoding_single_multi(
        q_sf_symbols, q_ce[0], q->d0, NULL, q->nof_rx_antennas, SRSRAN_PHICH_MAX_NSYMB, 1.0f, channel->noise_estimate);
  } else {
    srsran_predecoding_diversity_multi(
        q_sf_symbols, q_ce, x, NULL, q->nof_rx_antennas, q->cell.nof_ports, SRSRAN_PHICH_MAX_NSYMB, 1.0f);
    srsran_layerdemap_diversity(x, q->d0, q->cell.nof_ports, SRSRAN_PHICH_MAX_NSYMB / q->cell.nof_ports);
  }
  DEBUG("Recv!!: ");
  DEBUG("d0: ");
  if (SRSRAN_VERBOSE_ISDEBUG())
    srsran_vec_fprint_c(stdout, q->d0, SRSRAN_PHICH_MAX_NSYMB);

  if (SRSRAN_CP_ISEXT(q->cell.cp)) {
    if (n_phich.ngroup % 2) {
      for (i = 0; i < SRSRAN_PHICH_EXT_MSYMB / 2; i++) {
        q->d[2 * i + 0] = q->d0[4 * i + 2];
        q->d[2 * i + 1] = q->d0[4 * i + 3];
      }
    } else {
      for (i = 0; i < SRSRAN_PHICH_EXT_MSYMB / 2; i++) {
        q->d[2 * i + 0] = q->d0[4 * i];
        q->d[2 * i + 1] = q->d0[4 * i + 1];
      }
    }
  } else {
    memcpy(q->d, q->d0, SRSRAN_PHICH_MAX_NSYMB * sizeof(cf_t));
  }

  DEBUG("d: ");
  if (SRSRAN_VERBOSE_ISDEBUG())
    srsran_vec_fprint_c(stdout, q->d, SRSRAN_PHICH_EXT_MSYMB);

  srsran_scrambling_c(&q->seq[sf_idx], q->d);

  /* De-spreading */
  if (SRSRAN_CP_ISEXT(q->cell.cp)) {
    for (i = 0; i < SRSRAN_PHICH_NBITS; i++) {
      q->z[i] = 0;
      for (j = 0; j < SRSRAN_PHICH_EXT_NSF; j++) {
        q->z[i] += conjf(w_ext[n_phich.nseq][j]) * q->d[i * SRSRAN_PHICH_EXT_NSF + j] / SRSRAN_PHICH_EXT_NSF;
      }
    }
  } else {
    for (i = 0; i < SRSRAN_PHICH_NBITS; i++) {
      q->z[i] = 0;
      for (j = 0; j < SRSRAN_PHICH_NORM_NSF; j++) {
        q->z[i] += conjf(w_normal[n_phich.nseq][j]) * q->d[i * SRSRAN_PHICH_NORM_NSF + j] / SRSRAN_PHICH_NORM_NSF;
      }
    }
  }

  DEBUG("z: ");
  if (SRSRAN_VERBOSE_ISDEBUG())
    srsran_vec_fprint_c(stdout, q->z, SRSRAN_PHICH_NBITS);

  srsran_demod_soft_demodulate(SRSRAN_MOD_BPSK, q->z, q->data_rx, SRSRAN_PHICH_NBITS);

  if (result) {
    result->ack_value = srsran_phich_ack_decode(q->data_rx, &result->distance);
  }

  return SRSRAN_SUCCESS;
}

/** Encodes ACK/NACK bits, modulates and inserts into resource.
 * The parameter ack is an array of srsran_phich_ngroups() pointers to buffers of nof_sequences uint8_ts
 */
int srsran_phich_encode(srsran_phich_t*         q,
                        srsran_dl_sf_cfg_t*     sf,
                        srsran_phich_resource_t n_phich,
                        uint8_t                 ack,
                        cf_t*                   sf_symbols[SRSRAN_MAX_PORTS])
{
  int i;

  if (q == NULL || sf_symbols == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  uint32_t sf_idx = sf->tti % 10;

  if (SRSRAN_CP_ISEXT(q->cell.cp)) {
    if (n_phich.nseq >= SRSRAN_PHICH_EXT_NSEQUENCES) {
      ERROR("Invalid nseq %d", n_phich.nseq);
      return SRSRAN_ERROR_INVALID_INPUTS;
    }
  } else {
    if (n_phich.nseq >= SRSRAN_PHICH_NORM_NSEQUENCES) {
      ERROR("Invalid nseq %d", n_phich.nseq);
      return SRSRAN_ERROR_INVALID_INPUTS;
    }
  }
  if (n_phich.ngroup >= srsran_regs_phich_ngroups(q->regs)) {
    ERROR("Invalid ngroup %d", n_phich.ngroup);
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  /* Set pointers for layermapping & precoding */
  cf_t* x[SRSRAN_MAX_LAYERS];
  cf_t* symbols_precoding[SRSRAN_MAX_PORTS];

  /* number of layers equals number of ports */
  for (i = 0; i < q->cell.nof_ports; i++) {
    x[i] = q->x[i];
  }
  for (i = 0; i < SRSRAN_MAX_PORTS; i++) {
    symbols_precoding[i] = q->sf_symbols[i];
  }

  /* encode ACK/NACK bit */
  srsran_phich_ack_encode(ack, q->data);

  srsran_mod_modulate(&q->mod, q->data, q->z, SRSRAN_PHICH_NBITS);

  DEBUG("data: ");
  if (SRSRAN_VERBOSE_ISDEBUG())
    srsran_vec_fprint_c(stdout, q->z, SRSRAN_PHICH_NBITS);

  /* Spread with w */
  if (SRSRAN_CP_ISEXT(q->cell.cp)) {
    for (i = 0; i < SRSRAN_PHICH_EXT_MSYMB; i++) {
      q->d[i] = w_ext[n_phich.nseq][i % SRSRAN_PHICH_EXT_NSF] * q->z[i / SRSRAN_PHICH_EXT_NSF];
    }
  } else {
    for (i = 0; i < SRSRAN_PHICH_NORM_MSYMB; i++) {
      q->d[i] = w_normal[n_phich.nseq][i % SRSRAN_PHICH_NORM_NSF] * q->z[i / SRSRAN_PHICH_NORM_NSF];
    }
  }

  DEBUG("d: ");
  if (SRSRAN_VERBOSE_ISDEBUG())
    srsran_vec_fprint_c(stdout, q->d, SRSRAN_PHICH_EXT_MSYMB);

  srsran_scrambling_c(&q->seq[sf_idx], q->d);

  /* align to REG */
  if (SRSRAN_CP_ISEXT(q->cell.cp)) {
    if (n_phich.ngroup % 2) {
      for (i = 0; i < SRSRAN_PHICH_EXT_MSYMB / 2; i++) {
        q->d0[4 * i + 0] = 0;
        q->d0[4 * i + 1] = 0;
        q->d0[4 * i + 2] = q->d[2 * i];
        q->d0[4 * i + 3] = q->d[2 * i + 1];
      }
    } else {
      for (i = 0; i < SRSRAN_PHICH_EXT_MSYMB / 2; i++) {
        q->d0[4 * i + 0] = q->d[2 * i];
        q->d0[4 * i + 1] = q->d[2 * i + 1];
        q->d0[4 * i + 2] = 0;
        q->d0[4 * i + 3] = 0;
      }
    }
  } else {
    memcpy(q->d0, q->d, SRSRAN_PHICH_MAX_NSYMB * sizeof(cf_t));
  }

  DEBUG("d0: ");
  if (SRSRAN_VERBOSE_ISDEBUG())
    srsran_vec_fprint_c(stdout, q->d0, SRSRAN_PHICH_MAX_NSYMB);

  /* layer mapping & precoding */
  if (q->cell.nof_ports > 1) {
    srsran_layermap_diversity(q->d0, x, q->cell.nof_ports, SRSRAN_PHICH_MAX_NSYMB);
    srsran_precoding_diversity(
        x, symbols_precoding, q->cell.nof_ports, SRSRAN_PHICH_MAX_NSYMB / q->cell.nof_ports, 1.0f);
    /**TODO: According to 6.9.2, Precoding for 4 tx ports is different! */
  } else {
    memcpy(q->sf_symbols[0], q->d0, SRSRAN_PHICH_MAX_NSYMB * sizeof(cf_t));
  }

  /* mapping to resource elements */
  for (i = 0; i < q->cell.nof_ports; i++) {
    if (srsran_regs_phich_add(q->regs, q->sf_symbols[i], n_phich.ngroup, sf_symbols[i]) < 0) {
      ERROR("Error putting PCHICH resource elements");
      return SRSRAN_ERROR;
    }
  }

  return SRSRAN_SUCCESS;
}
