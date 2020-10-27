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

#include "srslte/srslte.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>
#include <semaphore.h>

#include "prb_dl.h"
#include "srslte/phy/phch/pdsch.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"

#ifdef LV_HAVE_SSE
#include <immintrin.h>
#endif /* LV_HAVE_SSE */

#define MAX_PDSCH_RE(cp) (2 * SRSLTE_CP_NSYMB(cp) * 12)

/* 3GPP 36.213 Table 5.2-1: The cell-specific ratio rho_B / rho_A for 1, 2, or 4 cell specific antenna ports */
const static float pdsch_cfg_cell_specific_ratio_table[2][4] = {
    /* One antenna port         */ {1.0f / 1.0f, 4.0f / 5.0f, 3.0f / 5.0f, 2.0f / 5.0f},
    /* Two or more antenna port */ {5.0f / 4.0f, 1.0f / 1.0f, 3.0f / 4.0f, 1.0f / 2.0f}};

const static srslte_mod_t modulations[5] = {SRSLTE_MOD_BPSK,
                                            SRSLTE_MOD_QPSK,
                                            SRSLTE_MOD_16QAM,
                                            SRSLTE_MOD_64QAM,
                                            SRSLTE_MOD_256QAM};

typedef struct {
  /* Thread identifier: they must set before thread creation */
  pthread_t pthread;
  uint32_t  tb_idx;
  void*     pdsch_ptr;
  bool*     ack;

  /* Configuration Encoder/Decoder: they must be set before posting start semaphore */
  srslte_dl_sf_cfg_t* sf;
  srslte_pdsch_cfg_t* cfg;
  srslte_sch_t        dl_sch;

  /* Encoder/Decoder data pointers: they must be set before posting start semaphore  */
  srslte_pdsch_res_t* data;

  /* Execution status */
  int ret_status;

  /* Semaphores */
  sem_t start;
  sem_t finish;

  /* Thread flags */
  bool started;
  bool quit;
} srslte_pdsch_coworker_t;

static void* srslte_pdsch_decode_thread(void* arg);

static inline bool pdsch_cp_skip_symbol(const srslte_cell_t*        cell,
                                        const srslte_pdsch_grant_t* grant,
                                        uint32_t                    sf_idx,
                                        uint32_t                    s,
                                        uint32_t                    l,
                                        uint32_t                    n)
{
  // Skip center block signals
  if ((n >= cell->nof_prb / 2 - 3 && n < cell->nof_prb / 2 + 3 + (cell->nof_prb % 2))) {
    if (cell->frame_type == SRSLTE_FDD) {
      // FDD PSS/SSS
      if (s == 0 && (sf_idx == 0 || sf_idx == 5) && (l >= grant->nof_symb_slot[s] - 2)) {
        return true;
      }
    } else {
      // TDD SSS
      if (s == 1 && (sf_idx == 0 || sf_idx == 5) && (l >= grant->nof_symb_slot[s] - 1)) {
        return true;
      }
      // TDD PSS
      if (s == 0 && (sf_idx == 1 || sf_idx == 6) && (l == 2)) {
        return true;
      }
    }
    // PBCH same in FDD and TDD
    if (s == 1 && sf_idx == 0 && l < 4) {
      return true;
    }
  }

  return false;
}

static inline uint32_t pdsch_cp_crs_offset(const srslte_cell_t* cell, uint32_t l, bool has_crs)
{
  // No CRS, return 0
  if (!has_crs) {
    return 0;
  }

  // For 1 port cell
  if (cell->nof_ports == 1) {
    if (l == 0) {
      return cell->id % 6;
    } else {
      return (cell->id + 3) % 6;
    }
  }

  // For more 2 ports or more
  return cell->id % 3;
}

static int srslte_pdsch_cp(const srslte_pdsch_t*       q,
                           cf_t*                       input,
                           cf_t*                       output,
                           const srslte_pdsch_grant_t* grant,
                           uint32_t                    lstart_grant,
                           uint32_t                    sf_idx,
                           bool                        put)
{
  cf_t*    in_ptr   = input;
  cf_t*    out_ptr  = output;
  uint32_t nof_refs = (q->cell.nof_ports == 1) ? 2 : 4;

  // Iterate over slots
  for (uint32_t s = 0; s < SRSLTE_NOF_SLOTS_PER_SF; s++) {
    // Skip PDCCH symbols
    uint32_t lstart = (s == 0) ? lstart_grant : 0;

    // Iterate over symbols
    for (uint32_t l = lstart; l < grant->nof_symb_slot[s]; l++) {
      bool     has_crs    = SRSLTE_SYMBOL_HAS_REF(l, q->cell.cp, q->cell.nof_ports);
      uint32_t crs_offset = pdsch_cp_crs_offset(&q->cell, l, has_crs);

      // Grid symbol
      uint32_t lp = l + s * grant->nof_symb_slot[0];

      // Iterate over PRB
      for (uint32_t n = 0; n < q->cell.nof_prb; n++) {

        // If this PRB is assigned
        if (grant->prb_idx[s][n]) {
          bool skip = pdsch_cp_skip_symbol(&q->cell, grant, sf_idx, s, l, n);

          // Get grid pointer
          if (put) {
            out_ptr = &output[(lp * q->cell.nof_prb + n) * SRSLTE_NRE];
          } else {
            in_ptr = &input[(lp * q->cell.nof_prb + n) * SRSLTE_NRE];
          }

          // This is a symbol in a normal PRB with or without references
          if (!skip) {
            if (has_crs) {
              prb_cp_ref(&in_ptr, &out_ptr, crs_offset, nof_refs, nof_refs, put);
            } else {
              prb_cp(&in_ptr, &out_ptr, 1);
            }
          } else if (q->cell.nof_prb % 2 != 0) {
            // This is a symbol in a PRB with PBCH or Synch signals (SS).
            // If the number or total PRB is odd, half of the the PBCH or SS will fall into the symbol
            if (n == q->cell.nof_prb / 2 - 3) {
              // Lower sync block half RB
              if (has_crs) {
                prb_cp_ref(&in_ptr, &out_ptr, crs_offset, nof_refs, nof_refs / 2, put);
              } else {
                prb_cp_half(&in_ptr, &out_ptr, 1);
              }
            } else if (n == q->cell.nof_prb / 2 + 3) {
              // Upper sync block half RB
              // Skip half RB on the grid
              if (put) {
                out_ptr += SRSLTE_NRE / 2;
              } else {
                in_ptr += SRSLTE_NRE / 2;
              }

              if (has_crs) {
                prb_cp_ref(&in_ptr, &out_ptr, crs_offset, nof_refs, nof_refs / 2, put);
              } else {
                prb_cp_half(&in_ptr, &out_ptr, 1);
              }
            }
          }
        }
      }
    }
  }

  int r;
  if (put) {
    r = abs((int)(input - in_ptr));
  } else {
    r = abs((int)(output - out_ptr));
  }

  return r;
}

/**
 * Puts PDSCH in slot number 1
 *
 * Returns the number of symbols written to sf_symbols
 *
 * 36.211 10.3 section 6.3.5
 */
int srslte_pdsch_put(srslte_pdsch_t*       q,
                     cf_t*                 symbols,
                     cf_t*                 sf_symbols,
                     srslte_pdsch_grant_t* grant,
                     uint32_t              lstart,
                     uint32_t              subframe)
{
  return srslte_pdsch_cp(q, symbols, sf_symbols, grant, lstart, subframe, true);
}

/**
 * Extracts PDSCH from slot number 1
 *
 * Returns the number of symbols written to PDSCH
 *
 * 36.211 10.3 section 6.3.5
 */
int srslte_pdsch_get(srslte_pdsch_t*       q,
                     cf_t*                 sf_symbols,
                     cf_t*                 symbols,
                     srslte_pdsch_grant_t* grant,
                     uint32_t              lstart,
                     uint32_t              subframe)
{
  return srslte_pdsch_cp(q, sf_symbols, symbols, grant, lstart, subframe, false);
}

/** Initializes the PDSCH transmitter and receiver */
static int pdsch_init(srslte_pdsch_t* q, uint32_t max_prb, bool is_ue, uint32_t nof_antennas)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q != NULL) {

    bzero(q, sizeof(srslte_pdsch_t));
    ret = SRSLTE_ERROR;

    q->max_re          = max_prb * MAX_PDSCH_RE(q->cell.cp);
    q->is_ue           = is_ue;
    q->nof_rx_antennas = nof_antennas;

    INFO("Init PDSCH: %d PRBs, max_symbols: %d\n", max_prb, q->max_re);

    for (int i = 0; i < SRSLTE_MOD_NITEMS; i++) {
      if (srslte_modem_table_lte(&q->mod[i], modulations[i])) {
        goto clean;
      }
      srslte_modem_table_bytes(&q->mod[i]);
    }

    if (srslte_sch_init(&q->dl_sch)) {
      ERROR("Initiating DL SCH");
      goto clean;
    }

    for (int i = 0; i < SRSLTE_MAX_CODEWORDS; i++) {
      // Allocate int16_t for reception (LLRs)
      q->e[i] = srslte_vec_i16_malloc(q->max_re * srslte_mod_bits_x_symbol(SRSLTE_MOD_256QAM));
      if (!q->e[i]) {
        goto clean;
      }

      q->d[i] = srslte_vec_cf_malloc(q->max_re);
      if (!q->d[i]) {
        goto clean;
      }

      // If it is the UE, allocate EVM buffer, for only minimum PRB
      if (is_ue) {
        q->evm_buffer[i] = srslte_evm_buffer_alloc(6);
        if (!q->evm_buffer[i]) {
          ERROR("Allocating EVM buffer\n");
          goto clean;
        }
      }
    }

    for (int i = 0; i < SRSLTE_MAX_PORTS; i++) {
      q->x[i] = srslte_vec_cf_malloc(q->max_re);
      if (!q->x[i]) {
        goto clean;
      }
      q->symbols[i] = srslte_vec_cf_malloc(q->max_re);
      if (!q->symbols[i]) {
        goto clean;
      }
      if (q->is_ue) {
        for (int j = 0; j < SRSLTE_MAX_PORTS; j++) {
          q->ce[i][j] = srslte_vec_cf_malloc(q->max_re);
          if (!q->ce[i][j]) {
            goto clean;
          }
        }
      }
    }

    q->users = calloc(sizeof(srslte_pdsch_user_t*), q->is_ue ? 1 : (1 + SRSLTE_SIRNTI));
    if (!q->users) {
      ERROR("malloc");
      goto clean;
    }

    if (srslte_sequence_init(&q->tmp_seq, q->max_re * srslte_mod_bits_x_symbol(SRSLTE_MOD_256QAM))) {
      goto clean;
    }

    for (int i = 0; i < SRSLTE_MAX_CODEWORDS; i++) {
      if (!q->csi[i]) {
        q->csi[i] = srslte_vec_f_malloc(q->max_re * 2);
        if (!q->csi[i]) {
          return SRSLTE_ERROR;
        }
      }
    }

    ret = SRSLTE_SUCCESS;
  }

clean:
  if (ret == SRSLTE_ERROR) {
    srslte_pdsch_free(q);
  }
  return ret;
}

int srslte_pdsch_init_ue(srslte_pdsch_t* q, uint32_t max_prb, uint32_t nof_antennas)
{
  return pdsch_init(q, max_prb, true, nof_antennas);
}

int srslte_pdsch_init_enb(srslte_pdsch_t* q, uint32_t max_prb)
{
  return pdsch_init(q, max_prb, false, 0);
}

static void srslte_pdsch_disable_coworker(srslte_pdsch_t* q)
{
  srslte_pdsch_coworker_t* h = (srslte_pdsch_coworker_t*)q->coworker_ptr;
  if (h) {
    /* Stop threads */
    h->quit = true;
    sem_post(&h->start);

    pthread_join(h->pthread, NULL);
    pthread_detach(h->pthread);

    srslte_sch_free(&h->dl_sch);

    free(h);

    q->coworker_ptr = NULL;
  }
}

int srslte_pdsch_enable_coworker(srslte_pdsch_t* q)
{
  int ret = SRSLTE_SUCCESS;

  if (!q->coworker_ptr) {
    srslte_pdsch_coworker_t* h = calloc(sizeof(srslte_pdsch_coworker_t), 1);

    if (!h) {
      ERROR("Allocating coworker");
      ret = SRSLTE_ERROR;
      goto clean;
    }
    q->coworker_ptr = h;

    if (srslte_sch_init(&h->dl_sch)) {
      ERROR("Initiating DL SCH");
      ret = SRSLTE_ERROR;
      goto clean;
    }

    if (sem_init(&h->start, 0, 0)) {
      ERROR("Creating semaphore");
      ret = SRSLTE_ERROR;
      goto clean;
    }
    if (sem_init(&h->finish, 0, 0)) {
      ERROR("Creating semaphore");
      ret = SRSLTE_ERROR;
      goto clean;
    }
    pthread_create(&h->pthread, NULL, srslte_pdsch_decode_thread, (void*)h);
  }

clean:
  if (ret) {
    srslte_pdsch_disable_coworker(q);
  }
  return ret;
}

void srslte_pdsch_free(srslte_pdsch_t* q)
{
  srslte_pdsch_disable_coworker(q);

  for (int i = 0; i < SRSLTE_MAX_CODEWORDS; i++) {

    if (q->e[i]) {
      free(q->e[i]);
    }

    if (q->d[i]) {
      free(q->d[i]);
    }

    if (q->csi[i]) {
      free(q->csi[i]);
    }

    if (q->evm_buffer[i]) {
      srslte_evm_free(q->evm_buffer[i]);
    }
  }

  /* Free sch objects */
  srslte_sch_free(&q->dl_sch);

  for (int i = 0; i < SRSLTE_MAX_PORTS; i++) {
    if (q->x[i]) {
      free(q->x[i]);
    }
    if (q->symbols[i]) {
      free(q->symbols[i]);
    }
    if (q->is_ue) {
      for (int j = 0; j < SRSLTE_MAX_PORTS; j++) {
        if (q->ce[i][j]) {
          free(q->ce[i][j]);
        }
      }
    }
  }
  if (q->users) {
    if (q->is_ue) {
      srslte_pdsch_free_rnti(q, 0);
    } else {
      for (int u = 0; u <= SRSLTE_SIRNTI; u++) {
        if (q->users[u]) {
          srslte_pdsch_free_rnti(q, u);
        }
      }
    }
    free(q->users);
  }

  srslte_sequence_free(&q->tmp_seq);

  for (int i = 0; i < SRSLTE_MOD_NITEMS; i++) {
    srslte_modem_table_free(&q->mod[i]);
  }

  bzero(q, sizeof(srslte_pdsch_t));
}

int srslte_pdsch_set_cell(srslte_pdsch_t* q, srslte_cell_t cell)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q != NULL && srslte_cell_isvalid(&cell)) {
    q->cell   = cell;
    q->max_re = q->cell.nof_prb * MAX_PDSCH_RE(q->cell.cp);

    // Resize EVM buffer, only for UE
    if (q->is_ue) {
      for (int i = 0; i < SRSLTE_MAX_CODEWORDS; i++) {
        srslte_evm_buffer_resize(q->evm_buffer[i], cell.nof_prb);
      }
    }

    INFO("PDSCH: Cell config PCI=%d, %d ports, %d PRBs, max_symbols: %d\n",
         q->cell.id,
         q->cell.nof_ports,
         q->cell.nof_prb,
         q->max_re);

    ret = SRSLTE_SUCCESS;
  }
  return ret;
}

/* Precalculate the PDSCH scramble sequences for a given RNTI. This function takes a while
 * to execute, so shall be called once the final C-RNTI has been allocated for the session.
 */
int srslte_pdsch_set_rnti(srslte_pdsch_t* q, uint16_t rnti)
{
  uint32_t rnti_idx = q->is_ue ? 0 : rnti;

  // Decide whether re-generating the sequence
  if (!q->users[rnti_idx]) {
    // If the sequence is not allocated generate
    q->users[rnti_idx] = calloc(1, sizeof(srslte_pdsch_user_t));
    if (!q->users[rnti_idx]) {
      ERROR("Alocating PDSCH user\n");
      return SRSLTE_ERROR;
    }
  } else if (q->users[rnti_idx]->sequence_generated && q->users[rnti_idx]->cell_id == q->cell.id && !q->is_ue) {
    // The sequence was generated, cell has not changed and it is eNb, save any efforts
    return SRSLTE_SUCCESS;
  }

  // Set sequence as not generated
  q->users[rnti_idx]->sequence_generated = false;

  // For each subframe
  for (int sf_idx = 0; sf_idx < SRSLTE_NOF_SF_X_FRAME; sf_idx++) {
    // For each codeword
    for (int j = 0; j < SRSLTE_MAX_CODEWORDS; j++) {
      if (srslte_sequence_pdsch(&q->users[rnti_idx]->seq[j][sf_idx],
                                rnti,
                                j,
                                SRSLTE_NOF_SLOTS_PER_SF * sf_idx,
                                q->cell.id,
                                q->max_re * srslte_mod_bits_x_symbol(SRSLTE_MOD_256QAM))) {
        ERROR("Error initializing PDSCH scrambling sequence\n");
        srslte_pdsch_free_rnti(q, rnti);
        return SRSLTE_ERROR;
      }
    }
  }

  // Save generation states
  q->ue_rnti                             = rnti;
  q->users[rnti_idx]->cell_id            = q->cell.id;
  q->users[rnti_idx]->sequence_generated = true;

  return SRSLTE_SUCCESS;
}

void srslte_pdsch_free_rnti(srslte_pdsch_t* q, uint16_t rnti)
{
  uint32_t rnti_idx = q->is_ue ? 0 : rnti;
  if (q->users[rnti_idx]) {
    for (int i = 0; i < SRSLTE_NOF_SF_X_FRAME; i++) {
      for (int j = 0; j < SRSLTE_MAX_CODEWORDS; j++) {
        srslte_sequence_free(&q->users[rnti_idx]->seq[j][i]);
      }
    }
    free(q->users[rnti_idx]);
    q->users[rnti_idx] = NULL;
    q->ue_rnti         = 0;
  }
}
static float apply_power_allocation(srslte_pdsch_t* q, srslte_pdsch_cfg_t* cfg, cf_t* sf_symbols_m[SRSLTE_MAX_PORTS])
{

  uint32_t nof_symbols_slot = cfg->grant.nof_symb_slot[0];
  uint32_t nof_re_symbol    = SRSLTE_NRE * q->cell.nof_prb;

  /* Set power allocation according to 3GPP 36.213 clause 5.2 Downlink power allocation */
  float rho_a = srslte_convert_dB_to_amplitude(cfg->p_a) * ((q->cell.nof_ports == 1) ? 1.0f : M_SQRT2);

  uint32_t idx0                = (q->cell.nof_ports == 1) ? 0 : 1;
  float    cell_specific_ratio = pdsch_cfg_cell_specific_ratio_table[idx0][cfg->p_b];
  float    rho_b               = sqrtf(cell_specific_ratio);

  /* Apply rho_b if required according to 3GPP 36.213 Table 5.2-2 */
  if (rho_b != 0.0f && rho_b != 1.0f) {
    float scaling = 1.0f / rho_b;
    for (uint32_t i = 0; i < q->nof_rx_antennas; i++) {
      for (uint32_t j = 0; j < 2; j++) {
        cf_t* ptr;
        ptr = sf_symbols_m[i] + nof_re_symbol * (j * nof_symbols_slot + 0);
        srslte_vec_sc_prod_cfc(ptr, scaling, ptr, nof_re_symbol);
        if (q->cell.cp == SRSLTE_CP_NORM) {
          ptr = sf_symbols_m[i] + nof_re_symbol * (j * nof_symbols_slot + 4);
          srslte_vec_sc_prod_cfc(ptr, scaling, ptr, nof_re_symbol);
        } else {
          ptr = sf_symbols_m[i] + nof_re_symbol * (j * nof_symbols_slot + 3);
          srslte_vec_sc_prod_cfc(ptr, scaling, ptr, nof_re_symbol);
        }
        if (q->cell.nof_ports == 4) {
          ptr = sf_symbols_m[i] + nof_re_symbol * (j * nof_symbols_slot + 1);
          srslte_vec_sc_prod_cfc(ptr, scaling, ptr, nof_re_symbol);
        }
      }
    }
  }
  return rho_a;
}

static srslte_sequence_t*
get_user_sequence(srslte_pdsch_t* q, uint16_t rnti, uint32_t codeword_idx, uint32_t sf_idx, uint32_t len)
{
  uint32_t rnti_idx = q->is_ue ? 0 : rnti;

  // The scrambling sequence is pregenerated for all RNTIs in the eNodeB but only for C-RNTI in the UE
  if (q->users[rnti_idx] && q->users[rnti_idx]->sequence_generated && q->users[rnti_idx]->cell_id == q->cell.id &&
      (!q->is_ue || q->ue_rnti == rnti)) {
    return &q->users[rnti_idx]->seq[codeword_idx][sf_idx];
  } else {
    srslte_sequence_pdsch(&q->tmp_seq, rnti, codeword_idx, 2 * sf_idx, q->cell.id, len);
    return &q->tmp_seq;
  }
}

static void csi_correction(srslte_pdsch_t* q, srslte_pdsch_cfg_t* cfg, uint32_t codeword_idx, uint32_t tb_idx, void* e)
{

  uint32_t qm = 0;
  switch (cfg->grant.tb[tb_idx].mod) {

    case SRSLTE_MOD_BPSK:
      qm = 1;
      break;
    case SRSLTE_MOD_QPSK:
      qm = 2;
      break;
    case SRSLTE_MOD_16QAM:
      qm = 4;
      break;
    case SRSLTE_MOD_64QAM:
      qm = 6;
      break;
    case SRSLTE_MOD_256QAM:
      qm = 8;
      break;
    default:
      ERROR("No modulation");
  }

  const uint32_t csi_max_idx = srslte_vec_max_fi(q->csi[codeword_idx], cfg->grant.tb[tb_idx].nof_bits / qm);
  float          csi_max     = 1.0f;
  if (csi_max_idx < cfg->grant.tb[tb_idx].nof_bits / qm) {
    csi_max = q->csi[codeword_idx][csi_max_idx];
  }
  int8_t*  e_b   = e;
  int16_t* e_s   = e;
  float*   csi_v = q->csi[codeword_idx];
  if (q->llr_is_8bit) {
    for (int i = 0; i < cfg->grant.tb[tb_idx].nof_bits / qm; i++) {
      const float csi = *(csi_v++) / csi_max;
      for (int k = 0; k < qm; k++) {
        *e_b = (int8_t)((float)*e_b * csi);
        e_b++;
      }
    }
  } else {
    int i = 0;

#ifdef LV_HAVE_SSE
    __m128 _csi_scale = _mm_set1_ps(INT16_MAX / csi_max);
    __m64* _e         = (__m64*)e;

    switch (cfg->grant.tb[tb_idx].mod) {
      case SRSLTE_MOD_QPSK:
        for (; i < cfg->grant.tb[tb_idx].nof_bits - 3; i += 4) {
          __m128 _csi1 = _mm_set1_ps(*(csi_v++));
          __m128 _csi2 = _mm_set1_ps(*(csi_v++));
          _csi1        = _mm_blend_ps(_csi1, _csi2, 3);

          _csi1 = _mm_mul_ps(_csi1, _csi_scale);

          _e[0] = _mm_mulhi_pi16(_e[0], _mm_cvtps_pi16(_csi1));
          _e += 1;
        }
        break;
      case SRSLTE_MOD_16QAM:
        for (; i < cfg->grant.tb[tb_idx].nof_bits - 3; i += 4) {
          __m128 _csi = _mm_set1_ps(*(csi_v++));

          _csi = _mm_mul_ps(_csi, _csi_scale);

          _e[0] = _mm_mulhi_pi16(_e[0], _mm_cvtps_pi16(_csi));
          _e += 1;
        }
        break;
      case SRSLTE_MOD_64QAM:
        for (; i < cfg->grant.tb[tb_idx].nof_bits - 11; i += 12) {
          __m128 _csi1 = _mm_set1_ps(*(csi_v++));
          __m128 _csi3 = _mm_set1_ps(*(csi_v++));

          _csi1        = _mm_mul_ps(_csi1, _csi_scale);
          _csi3        = _mm_mul_ps(_csi3, _csi_scale);
          __m128 _csi2 = _mm_blend_ps(_csi1, _csi3, 3);

          _e[0] = _mm_mulhi_pi16(_e[0], _mm_cvtps_pi16(_csi1));
          _e[1] = _mm_mulhi_pi16(_e[1], _mm_cvtps_pi16(_csi2));
          _e[2] = _mm_mulhi_pi16(_e[2], _mm_cvtps_pi16(_csi3));
          _e += 3;
        }
        break;
      case SRSLTE_MOD_BPSK:
        break;
      case SRSLTE_MOD_256QAM:
        for (; i < cfg->grant.tb[tb_idx].nof_bits - 7; i += 8) {
          __m128 _csi = _mm_set1_ps(*(csi_v++));

          _csi = _mm_mul_ps(_csi, _csi_scale);

          _e[0] = _mm_mulhi_pi16(_e[0], _mm_cvtps_pi16(_csi));
          _e[1] = _mm_mulhi_pi16(_e[1], _mm_cvtps_pi16(_csi));
          _e += 2;
        }
        break;
      case SRSLTE_MOD_NITEMS:
      default:; // Do nothing
    }

    i /= qm;
#endif /* LV_HAVE_SSE */

    for (; i < cfg->grant.tb[tb_idx].nof_bits / qm; i++) {
      const float csi = q->csi[codeword_idx][i] / csi_max;
      for (int k = 0; k < qm; k++) {
        e_s[qm * i + k] = (int16_t)((float)e_s[qm * i + k] * csi);
      }
    }
  }
}

static void pdsch_decode_debug(srslte_pdsch_t*     q,
                               srslte_pdsch_cfg_t* cfg,
                               cf_t*               sf_symbols[SRSLTE_MAX_PORTS],
                               cf_t*               ce[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS])
{
  if (SRSLTE_VERBOSE_ISDEBUG()) {
    char filename[FILENAME_MAX];
    for (int j = 0; j < q->nof_rx_antennas; j++) {
      if (snprintf(filename, FILENAME_MAX, "subframe_p%d.dat", j) < 0) {
        ERROR("Generating file name");
        break;
      }
      DEBUG("SAVED FILE %s: received subframe symbols\n", filename);
      srslte_vec_save_file(filename, sf_symbols[j], SRSLTE_NOF_RE(q->cell) * sizeof(cf_t));

      for (int i = 0; i < q->cell.nof_ports; i++) {
        if (snprintf(filename, FILENAME_MAX, "hest_%d%d.dat", i, j) < 0) {
          ERROR("Generating file name");
          break;
        }
        DEBUG("SAVED FILE %s: channel estimates for Tx %d and Rx %d\n", filename, j, i);
        srslte_vec_save_file(filename, ce[i][j], SRSLTE_NOF_RE(q->cell) * sizeof(cf_t));
      }
    }
    for (int i = 0; i < cfg->grant.nof_layers; i++) {
      if (snprintf(filename, FILENAME_MAX, "pdsch_symbols_%d.dat", i) < 0) {
        ERROR("Generating file name");
        break;
      }
      DEBUG("SAVED FILE %s: symbols after equalization\n", filename);
      srslte_vec_save_file(filename, q->d[i], cfg->grant.nof_re * sizeof(cf_t));

      if (snprintf(filename, FILENAME_MAX, "llr_%d.dat", i) < 0) {
        ERROR("Generating file name");
        break;
      }
      DEBUG("SAVED FILE %s: LLR estimates after demodulation and descrambling\n", filename);
      srslte_vec_save_file(filename, q->e[i], cfg->grant.tb[0].nof_bits * sizeof(int16_t));
    }
  }
}

static int srslte_pdsch_codeword_decode(srslte_pdsch_t*     q,
                                        srslte_dl_sf_cfg_t* sf,
                                        srslte_pdsch_cfg_t* cfg,
                                        srslte_sch_t*       dl_sch,
                                        srslte_pdsch_res_t* data,
                                        uint32_t            tb_idx,
                                        bool*               ack)
{
  srslte_ra_tb_t*         mcs          = &cfg->grant.tb[tb_idx];
  uint32_t                rv           = mcs->rv;
  uint32_t                codeword_idx = mcs->cw_idx;
  uint32_t                nof_layers   = cfg->grant.nof_layers;
  srslte_softbuffer_rx_t* softbuffer   = cfg->softbuffers.rx[tb_idx];

  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (softbuffer && data && ack && cfg->grant.tb[tb_idx].nof_bits && cfg->grant.nof_re) {
    INFO("Decoding PDSCH SF: %d (CW%d -> TB%d), Mod %s, NofBits: %d, NofSymbols: %d, NofBitsE: %d, rv_idx: %d\n",
         sf->tti % 10,
         codeword_idx,
         tb_idx,
         srslte_mod_string(mcs->mod),
         mcs->tbs,
         cfg->grant.nof_re,
         cfg->grant.tb[tb_idx].nof_bits,
         rv);

    /* demodulate symbols
     * The MAX-log-MAP algorithm used in turbo decoding is unsensitive to SNR estimation,
     * thus we don't need tot set it in the LLRs normalization
     */
    if (q->llr_is_8bit) {
      srslte_demod_soft_demodulate_b(mcs->mod, q->d[codeword_idx], q->e[codeword_idx], cfg->grant.nof_re);
    } else {
      srslte_demod_soft_demodulate_s(mcs->mod, q->d[codeword_idx], q->e[codeword_idx], cfg->grant.nof_re);
    }
    if (cfg->meas_evm_en && q->evm_buffer[codeword_idx]) {
      if (q->llr_is_8bit) {
        data[tb_idx].evm = srslte_evm_run_b(q->evm_buffer[codeword_idx],
                                            &q->mod[mcs->mod],
                                            q->d[codeword_idx],
                                            q->e[codeword_idx],
                                            cfg->grant.tb[tb_idx].nof_bits);
      } else {
        data[tb_idx].evm = srslte_evm_run_s(q->evm_buffer[codeword_idx],
                                            &q->mod[mcs->mod],
                                            q->d[codeword_idx],
                                            q->e[codeword_idx],
                                            cfg->grant.tb[tb_idx].nof_bits);
      }
    } else {
      data[tb_idx].evm = NAN;
    }

    /* Select scrambling sequence */
    srslte_sequence_t* seq =
        get_user_sequence(q, cfg->rnti, codeword_idx, sf->tti % 10, cfg->grant.tb[tb_idx].nof_bits);
    if (!seq) {
      ERROR("Error getting user sequence for rnti=0x%x\n", cfg->rnti);
      return -1;
    }

    /* Bit scrambling */
    if (q->llr_is_8bit) {
      srslte_scrambling_sb_offset(seq, q->e[codeword_idx], 0, cfg->grant.tb[tb_idx].nof_bits);
    } else {
      srslte_scrambling_s_offset(seq, q->e[codeword_idx], 0, cfg->grant.tb[tb_idx].nof_bits);
    }

    if (cfg->csi_enable) {
      csi_correction(q, cfg, codeword_idx, tb_idx, q->e[codeword_idx]);
    }

    /* Return  */
    ret = srslte_dlsch_decode2(dl_sch, cfg, q->e[codeword_idx], data[tb_idx].payload, tb_idx, nof_layers);

    if (ret == SRSLTE_SUCCESS) {
      *ack = true;
    } else if (ret == SRSLTE_ERROR) {
      *ack = false;
      ret  = SRSLTE_SUCCESS;
    } else if (ret == SRSLTE_ERROR_INVALID_INPUTS) {
      *ack = false;
      ret  = SRSLTE_ERROR;
    }
  } else {
    ERROR("Invalid parameters in TB%d &softbuffer=%p &data=%p &ack=%p, nbits=%d, nof_re=%d\n",
          codeword_idx,
          softbuffer,
          (void*)data,
          ack,
          cfg->grant.tb[tb_idx].nof_bits,
          cfg->grant.nof_re);
  }

  return ret;
}

static void* srslte_pdsch_decode_thread(void* arg)
{
  srslte_pdsch_coworker_t* q = (srslte_pdsch_coworker_t*)arg;

  INFO("[PDSCH Coworker] waiting for data\n");

  sem_wait(&q->start);
  while (!q->quit) {
    q->ret_status = srslte_pdsch_codeword_decode(q->pdsch_ptr, q->sf, q->cfg, &q->dl_sch, q->data, q->tb_idx, q->ack);

    /* Post finish semaphore */
    sem_post(&q->finish);

    /* Wait for next loop */
    sem_wait(&q->start);
  }
  sem_post(&q->finish);

  pthread_exit(NULL);
  return q;
}

/** Decodes the PDSCH from the received symbols
 */
int srslte_pdsch_decode(srslte_pdsch_t*        q,
                        srslte_dl_sf_cfg_t*    sf,
                        srslte_pdsch_cfg_t*    cfg,
                        srslte_chest_dl_res_t* channel,
                        cf_t*                  sf_symbols[SRSLTE_MAX_PORTS],
                        srslte_pdsch_res_t     data[SRSLTE_MAX_CODEWORDS])
{

  /* Set pointers for layermapping & precoding */
  uint32_t i;
  cf_t**   x;

  if (q != NULL && sf_symbols != NULL && data != NULL && cfg != NULL) {

    struct timeval t[3];
    if (cfg->meas_time_en) {
      gettimeofday(&t[1], NULL);
    }

    uint32_t nof_tb = cfg->grant.nof_tb;

    float pdsch_scaling = 1.0f;
    if (cfg->power_scale) {
      float rho_a = apply_power_allocation(q, cfg, sf_symbols);
      if (rho_a != 0.0f && isnormal(rho_a)) {
        pdsch_scaling = rho_a;
      }
    }

    if (cfg->max_nof_iterations) {
      srslte_sch_set_max_noi(&q->dl_sch, cfg->max_nof_iterations);
    }

    float noise_estimate = cfg->decoder_type == SRSLTE_MIMO_DECODER_ZF ? 0 : channel->noise_estimate;

    INFO("Decoding PDSCH SF: %d, RNTI: 0x%x, NofSymbols: %d, C_prb=%d, mod=%s, nof_layers=%d, nof_tb=%d\n",
         sf->tti % 10,
         cfg->rnti,
         cfg->grant.nof_re,
         cfg->grant.nof_prb,
         srslte_mod_string(cfg->grant.tb[0].mod),
         cfg->grant.nof_layers,
         nof_tb);

    // Extract Symbols and Channel Estimates
    uint32_t lstart = SRSLTE_NOF_CTRL_SYMBOLS(q->cell, sf->cfi);
    for (int j = 0; j < q->nof_rx_antennas; j++) {
      int n = srslte_pdsch_get(q, sf_symbols[j], q->symbols[j], &cfg->grant, lstart, sf->tti % 10);
      if (n != cfg->grant.nof_re) {
        ERROR("Error expecting %d symbols but got %d\n", cfg->grant.nof_re, n);
        return SRSLTE_ERROR;
      }

      for (i = 0; i < q->cell.nof_ports; i++) {
        n = srslte_pdsch_get(q, channel->ce[i][j], q->ce[i][j], &cfg->grant, lstart, sf->tti % 10);
        if (n != cfg->grant.nof_re) {
          ERROR("Error expecting %d symbols but got %d\n", cfg->grant.nof_re, n);
          return SRSLTE_ERROR;
        }
      }
    }

    if (cfg->grant.nof_layers == 0 || cfg->grant.nof_layers > SRSLTE_MAX_LAYERS) {
      ERROR("PDSCH Number of layers (%d) is out-of-bounds\n", cfg->grant.nof_layers);
      return SRSLTE_ERROR_OUT_OF_BOUNDS;
    }

    // Prepare layers
    int nof_symbols[SRSLTE_MAX_CODEWORDS];
    nof_symbols[0] = cfg->grant.nof_re * nof_tb / cfg->grant.nof_layers;
    nof_symbols[1] = cfg->grant.nof_re * nof_tb / cfg->grant.nof_layers;

    if (cfg->grant.nof_layers == nof_tb) {
      /* Skip layer demap */
      x = q->d;
    } else {
      /* number of layers equals number of ports */
      x = q->x;
    }

    // Pre-decoder
    uint32_t codebook_idx = nof_tb == 1 ? cfg->grant.pmi : (cfg->grant.pmi + 1);
    if (srslte_predecoding_type(q->symbols,
                                q->ce,
                                x,
                                q->csi,
                                q->nof_rx_antennas,
                                q->cell.nof_ports,
                                cfg->grant.nof_layers,
                                codebook_idx,
                                cfg->grant.nof_re,
                                cfg->grant.tx_scheme,
                                pdsch_scaling,
                                noise_estimate) < 0) {
      ERROR("Error predecoding\n");
      return SRSLTE_ERROR;
    }

    // Layer demapping only if necessary
    if (cfg->grant.nof_layers != nof_tb) {
      srslte_layerdemap_type(x, q->d, cfg->grant.nof_layers, nof_tb, nof_symbols[0], nof_symbols, cfg->grant.tx_scheme);
    }

    /* Codeword decoding: Implementation of 3GPP 36.212 Table 5.3.3.1.5-1 and Table 5.3.3.1.5-2 */
    for (uint32_t tb_idx = 0; tb_idx < SRSLTE_MAX_TB; tb_idx++) {

      /* Decode only if transport block is enabled and the default ACK is not true */
      if (cfg->grant.tb[tb_idx].enabled) {
        if (!data[tb_idx].crc) {
          int ret = SRSLTE_SUCCESS;
          if (cfg->grant.nof_tb > 1 && tb_idx == 0 && q->coworker_ptr) {
            srslte_pdsch_coworker_t* h = (srslte_pdsch_coworker_t*)q->coworker_ptr;

            h->pdsch_ptr             = q;
            h->cfg                   = cfg;
            h->sf                    = sf;
            h->data                  = &data[tb_idx];
            h->tb_idx                = tb_idx;
            h->ack                   = &data[tb_idx].crc;
            h->dl_sch.max_iterations = q->dl_sch.max_iterations;
            h->started               = true;
            sem_post(&h->start);

          } else {
            ret = srslte_pdsch_codeword_decode(q, sf, cfg, &q->dl_sch, data, tb_idx, &data[tb_idx].crc);

            data[tb_idx].avg_iterations_block = srslte_sch_last_noi(&q->dl_sch);
          }

          /* Check if there has been any execution error */
          if (ret) {
            /* Do Nothing */
          }
        }
      }
    }

    if (q->coworker_ptr) {
      srslte_pdsch_coworker_t* h = (srslte_pdsch_coworker_t*)q->coworker_ptr;
      if (h->started) {
        int err = sem_wait(&h->finish);
        if (err) {
          printf("SCH coworker: %s (nof_tb=%d)\n", strerror(errno), cfg->grant.nof_tb);
        }
        if (h->ret_status) {
          ERROR("PDSCH Coworker Decoder: Error decoding");
        }
        data[h->tb_idx].avg_iterations_block = srslte_sch_last_noi(&q->dl_sch);
        h->started                           = false;
      }
    }

    pdsch_decode_debug(q, cfg, sf_symbols, channel->ce);

    if (cfg->meas_time_en) {
      gettimeofday(&t[2], NULL);
      get_time_interval(t);
      cfg->meas_time_value = t[0].tv_usec;
    }

    return SRSLTE_SUCCESS;
  } else {
    ERROR("Invalid inputs\n");
    return SRSLTE_ERROR_INVALID_INPUTS;
  }
}

static int srslte_pdsch_codeword_encode(srslte_pdsch_t*         q,
                                        srslte_dl_sf_cfg_t*     sf,
                                        srslte_pdsch_cfg_t*     cfg,
                                        srslte_softbuffer_tx_t* softbuffer,
                                        uint8_t*                data,
                                        uint32_t                tb_idx,
                                        uint32_t                nof_layers)
{
  srslte_ra_tb_t* mcs = &cfg->grant.tb[tb_idx];
  uint32_t        rv  = cfg->grant.tb[tb_idx].rv;

  uint32_t codeword_idx = cfg->grant.tb[tb_idx].cw_idx;

  if (!softbuffer) {
    ERROR("Error encoding (TB%d -> CW%d), softbuffer=NULL", tb_idx, codeword_idx);
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  if (cfg->grant.tb[tb_idx].enabled) {
    if (cfg->rnti != SRSLTE_SIRNTI) {
      INFO("Encoding PDSCH SF: %d (TB%d -> CW%d), Mod %s, NofBits: %d, NofSymbols: %d, NofBitsE: %d, rv_idx: %d\n",
           sf->tti % 10,
           tb_idx,
           codeword_idx,
           srslte_mod_string(mcs->mod),
           mcs->tbs,
           cfg->grant.nof_re,
           cfg->grant.tb[tb_idx].nof_bits,
           rv);
    }

    /* Channel coding */
    if (srslte_dlsch_encode2(&q->dl_sch, cfg, data, q->e[codeword_idx], tb_idx, nof_layers)) {
      ERROR("Error encoding (TB%d -> CW%d)", tb_idx, codeword_idx);
      return SRSLTE_ERROR;
    }

    /* Select scrambling sequence */
    srslte_sequence_t* seq =
        get_user_sequence(q, cfg->rnti, codeword_idx, sf->tti % 10, cfg->grant.tb[tb_idx].nof_bits);
    if (!seq) {
      ERROR("Error getting user sequence for rnti=0x%x\n", cfg->rnti);
      return -1;
    }

    /* Bit scrambling */
    srslte_scrambling_bytes(seq, (uint8_t*)q->e[codeword_idx], cfg->grant.tb[tb_idx].nof_bits);

    /* Bit mapping */
    srslte_mod_modulate_bytes(
        &q->mod[mcs->mod], (uint8_t*)q->e[codeword_idx], q->d[codeword_idx], cfg->grant.tb[tb_idx].nof_bits);

  } else {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  return SRSLTE_SUCCESS;
}

int srslte_pdsch_encode(srslte_pdsch_t*     q,
                        srslte_dl_sf_cfg_t* sf,
                        srslte_pdsch_cfg_t* cfg,
                        uint8_t*            data[SRSLTE_MAX_CODEWORDS],
                        cf_t*               sf_symbols[SRSLTE_MAX_PORTS])
{

  int i;
  /* Set pointers for layermapping & precoding */
  cf_t* x[SRSLTE_MAX_LAYERS];
  int   ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q != NULL && cfg != NULL) {
    struct timeval t[3];
    if (cfg->meas_time_en) {
      gettimeofday(&t[1], NULL);
    }

    uint32_t nof_tb = cfg->grant.nof_tb;

    for (i = 0; i < q->cell.nof_ports; i++) {
      if (sf_symbols[i] == NULL) {
        ERROR("Error NULL pointer in sf_symbols[%d]\n", i);
        return SRSLTE_ERROR_INVALID_INPUTS;
      }
    }

    /* If both transport block size is zero return error */
    if (!nof_tb) {
      ERROR("Error number of TB is zero\n");
      return SRSLTE_ERROR_INVALID_INPUTS;
    }

    if (cfg->grant.nof_re > q->max_re) {
      ERROR("Error too many RE per subframe (%d). PDSCH configured for %d RE (%d PRB)\n",
            cfg->grant.nof_re,
            q->max_re,
            q->cell.nof_prb);
      return SRSLTE_ERROR_INVALID_INPUTS;
    }

    float rho_a = apply_power_allocation(q, cfg, sf_symbols);

    /* Implementation of 3GPP 36.212 Table 5.3.3.1.5-1 and Table 5.3.3.1.5-2 */
    for (uint32_t tb_idx = 0; tb_idx < SRSLTE_MAX_TB; tb_idx++) {
      if (cfg->grant.tb[tb_idx].enabled) {
        ret |= srslte_pdsch_codeword_encode(
            q, sf, cfg, cfg->softbuffers.tx[tb_idx], data[tb_idx], tb_idx, cfg->grant.nof_layers);
      }
    }

    /* Set scaling configured by Power Allocation */
    float scaling = 1.0f;
    if (rho_a != 0.0f) {
      scaling = rho_a;
    }

    if (cfg->rnti != SRSLTE_SIRNTI) {
      INFO("Encoding PDSCH SF: %d rho_a=%f, nof_ports=%d, nof_layers=%d, nof_tb=%d, pmi=%d, tx_scheme=%s\n",
           sf->tti % 10,
           rho_a,
           q->cell.nof_ports,
           cfg->grant.nof_layers,
           nof_tb,
           cfg->grant.pmi,
           srslte_mimotype2str(cfg->grant.tx_scheme));
    }

    // Layer mapping & precode if necessary
    if (q->cell.nof_ports > 1) {
      int nof_symbols;
      /* If number of layers is equal to transport blocks (codewords) skip layer mapping */
      if (cfg->grant.nof_layers == nof_tb) {
        for (i = 0; i < cfg->grant.nof_layers; i++) {
          x[i] = q->d[i];
        }
        nof_symbols = cfg->grant.nof_re;
      } else {
        /* Initialise layer map pointers */
        for (i = 0; i < cfg->grant.nof_layers; i++) {
          x[i] = q->x[i];
        }
        memset(&x[cfg->grant.nof_layers], 0, sizeof(cf_t*) * (SRSLTE_MAX_LAYERS - cfg->grant.nof_layers));

        nof_symbols = srslte_layermap_type(q->d,
                                           x,
                                           nof_tb,
                                           cfg->grant.nof_layers,
                                           (int[SRSLTE_MAX_CODEWORDS]){cfg->grant.nof_re, cfg->grant.nof_re},
                                           cfg->grant.tx_scheme);
      }

      /* Precode */
      uint32_t codebook_idx = nof_tb == 1 ? cfg->grant.pmi : (cfg->grant.pmi + 1);
      srslte_precoding_type(x,
                            q->symbols,
                            cfg->grant.nof_layers,
                            q->cell.nof_ports,
                            codebook_idx,
                            nof_symbols,
                            scaling,
                            cfg->grant.tx_scheme);
    } else {
      if (scaling == 1.0f) {
        memcpy(q->symbols[0], q->d[0], cfg->grant.nof_re * sizeof(cf_t));
      } else {
        srslte_vec_sc_prod_cfc(q->d[0], scaling, q->symbols[0], cfg->grant.nof_re);
      }
    }

    /* mapping to resource elements */
    uint32_t lstart = SRSLTE_NOF_CTRL_SYMBOLS(q->cell, sf->cfi);
    for (i = 0; i < q->cell.nof_ports; i++) {
      srslte_pdsch_put(q, q->symbols[i], sf_symbols[i], &cfg->grant, lstart, sf->tti % 10);
    }

    if (cfg->meas_time_en) {
      gettimeofday(&t[2], NULL);
      get_time_interval(t);
      cfg->meas_time_value = t[0].tv_usec;
    }

    ret = SRSLTE_SUCCESS;
  }
  return ret;
}

int srslte_pdsch_select_pmi(srslte_pdsch_t*        q,
                            srslte_chest_dl_res_t* channel,
                            uint32_t               nof_layers,
                            uint32_t*              best_pmi,
                            float                  sinr[SRSLTE_MAX_CODEBOOKS])
{
  uint32_t nof_ce = SRSLTE_NOF_RE(q->cell);
  uint32_t pmi    = 0;

  if (srslte_precoding_pmi_select(channel->ce, nof_ce, channel->noise_estimate, nof_layers, &pmi, sinr) < 0) {
    ERROR("PMI Select for %d layers", nof_layers);
    return SRSLTE_ERROR;
  }

  if (best_pmi) {
    *best_pmi = pmi;
  }

  return SRSLTE_SUCCESS;
}

int srslte_pdsch_compute_cn(srslte_pdsch_t* q, srslte_chest_dl_res_t* channel, float* cn)
{
  return srslte_precoding_cn(channel->ce, q->cell.nof_ports, q->nof_rx_antennas, SRSLTE_NOF_RE(q->cell), cn);
}

uint32_t srslte_pdsch_grant_rx_info(srslte_pdsch_grant_t* grant,
                                    srslte_pdsch_res_t    res[SRSLTE_MAX_CODEWORDS],
                                    char*                 str,
                                    uint32_t              str_len)
{

  uint32_t len = srslte_ra_dl_info(grant, str, str_len);

  len = srslte_print_check(str, str_len, len, ", crc={", 0);
  for (uint32_t i = 0; i < SRSLTE_MAX_CODEWORDS; i++) {
    if (grant->tb[i].enabled) {
      len = srslte_print_check(str, str_len, len, "%s", res[i].crc ? "OK" : "KO");
      if (i < SRSLTE_MAX_CODEWORDS - 1) {
        if (grant->tb[i + 1].enabled) {
          len = srslte_print_check(str, str_len, len, "/", 0);
        }
      }
    }
  }
  len = srslte_print_check(str, str_len, len, "}", 0);

  // Average iterations between nof TB and divide by 2 to get full decoder iterations
  len = srslte_print_check(
      str, str_len, len, ", it=%.1f", (res[0].avg_iterations_block + res[1].avg_iterations_block) / grant->nof_tb / 2);

  return len;
}

uint32_t
srslte_pdsch_rx_info(srslte_pdsch_cfg_t* cfg, srslte_pdsch_res_t res[SRSLTE_MAX_CODEWORDS], char* str, uint32_t str_len)
{

  uint32_t len = srslte_print_check(str, str_len, 0, "rnti=0x%x", cfg->rnti);
  len += srslte_pdsch_grant_rx_info(&cfg->grant, res, &str[len], str_len - len);

  if (cfg->meas_evm_en) {
    len = srslte_print_check(str, str_len, len, ", evm={", 0);
    for (uint32_t i = 0; i < SRSLTE_MAX_CODEWORDS; i++) {
      if (cfg->grant.tb[i].enabled && !isnan(res[i].evm)) {
        len = srslte_print_check(str, str_len, len, "%.2f", res[i].evm);
        if (i < SRSLTE_MAX_CODEWORDS - 1) {
          if (cfg->grant.tb[i + 1].enabled) {
            len = srslte_print_check(str, str_len, len, "/", 0);
          }
        }
      }
    }
    len = srslte_print_check(str, str_len, len, "}", 0);
  }

  if (cfg->meas_time_en) {
    len = srslte_print_check(str, str_len, len, ", t=%d us\n", cfg->meas_time_value);
  }

  return len;
}

uint32_t srslte_pdsch_tx_info(srslte_pdsch_cfg_t* cfg, char* str, uint32_t str_len)
{

  uint32_t len = srslte_print_check(str, str_len, 0, "rnti=0x%x", cfg->rnti);
  len += srslte_ra_dl_info(&cfg->grant, &str[len], str_len);

  if (cfg->meas_time_en) {
    len = srslte_print_check(str, str_len, len, ", t=%d us", cfg->meas_time_value);
  }
  return len;
}
