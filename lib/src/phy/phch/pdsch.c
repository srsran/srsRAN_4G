/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>

#ifndef SRSLTE_SINGLE_THREAD

#include <pthread.h>
#include <semaphore.h>

#endif /* SRSLTE_SINGLE_THREAD */

#include "prb_dl.h"
#include "srslte/phy/phch/pdsch.h"
#include "srslte/phy/phch/sch.h"
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/utils/bit.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"


#define MAX_PDSCH_RE(cp) (2 * SRSLTE_CP_NSYMB(cp) * 12)



const static srslte_mod_t modulations[4] =
    { SRSLTE_MOD_BPSK, SRSLTE_MOD_QPSK, SRSLTE_MOD_16QAM, SRSLTE_MOD_64QAM };
    
//#define DEBUG_IDX

#ifdef DEBUG_IDX    
cf_t *offset_original=NULL;
extern int indices[100000];
extern int indices_ptr; 
#endif

#ifndef SRSLTE_SINGLE_THREAD

static void *srslte_pdsch_encode_thread (void *arg);
static void *srslte_pdsch_decode_thread (void *arg);

#endif /* SRSLTE_SINGLE_THREAD */

float srslte_pdsch_coderate(uint32_t tbs, uint32_t nof_re) 
{
  return (float) (tbs + 24)/(nof_re);
}

int srslte_pdsch_cp(srslte_pdsch_t *q, cf_t *input, cf_t *output, srslte_ra_dl_grant_t *grant, uint32_t lstart_grant, uint32_t nsubframe, bool put) 
{
  uint32_t s, n, l, lp, lstart, lend, nof_refs;
  bool is_pbch, is_sss;
  cf_t *in_ptr = input, *out_ptr = output;
  uint32_t offset = 0;

#ifdef DEBUG_IDX    
  indices_ptr = 0; 
  if (put) {
    offset_original = output; 
  } else {
    offset_original = input;     
  }
#endif
  
  if (q->cell.nof_ports == 1) {
    nof_refs = 2;
  } else {
    nof_refs = 4;
  }

  for (s = 0; s < 2; s++) {
    for (l = 0; l < SRSLTE_CP_NSYMB(q->cell.cp); l++) {
      for (n = 0; n < q->cell.nof_prb; n++) {

        // If this PRB is assigned
        if (grant->prb_idx[s][n]) {
          if (s == 0) {
            lstart = lstart_grant;
          } else {
            lstart = 0;
          }
          lend = SRSLTE_CP_NSYMB(q->cell.cp);
          is_pbch = is_sss = false;

          // Skip PSS/SSS signals
          if (s == 0 && (nsubframe == 0 || nsubframe == 5)) {
            if (n >= q->cell.nof_prb / 2 - 3
                && n < q->cell.nof_prb / 2 + 3 + (q->cell.nof_prb%2)) {
              lend = SRSLTE_CP_NSYMB(q->cell.cp) - 2;
              is_sss = true;
            }
          }
          // Skip PBCH
          if (s == 1 && nsubframe == 0) {
            if (n >= q->cell.nof_prb / 2 - 3
                && n < q->cell.nof_prb / 2 + 3 + (q->cell.nof_prb%2)) {
              lstart = 4;
              is_pbch = true;
            }
          }
          lp = l + s * SRSLTE_CP_NSYMB(q->cell.cp);
          if (put) {
            out_ptr = &output[(lp * q->cell.nof_prb + n)
                * SRSLTE_NRE];
          } else {
            in_ptr = &input[(lp * q->cell.nof_prb + n)
                * SRSLTE_NRE];
          }
          // This is a symbol in a normal PRB with or without references
          if (l >= lstart && l < lend) {
            if (SRSLTE_SYMBOL_HAS_REF(l, q->cell.cp, q->cell.nof_ports)) {
              if (nof_refs == 2) {
                if (l == 0) {
                  offset = q->cell.id % 6;
                } else {
                  offset = (q->cell.id + 3) % 6;                  
                }
              } else {
                offset = q->cell.id % 3;
              }
              prb_cp_ref(&in_ptr, &out_ptr, offset, nof_refs, nof_refs, put);
            } else {
              prb_cp(&in_ptr, &out_ptr, 1);
            }
          }
          // This is a symbol in a PRB with PBCH or Synch signals (SS). 
          // If the number or total PRB is odd, half of the the PBCH or SS will fall into the symbol
          if ((q->cell.nof_prb % 2) && ((is_pbch && l < lstart) || (is_sss && l >= lend))) {
            if (n == q->cell.nof_prb / 2 - 3) {
              if (SRSLTE_SYMBOL_HAS_REF(l, q->cell.cp, q->cell.nof_ports)) {
                prb_cp_ref(&in_ptr, &out_ptr, offset, nof_refs, nof_refs/2, put);
              } else {
                prb_cp_half(&in_ptr, &out_ptr, 1);
              }
            } else if (n == q->cell.nof_prb / 2 + 3) {
              if (put) {
                out_ptr += 6;
              } else {
                in_ptr += 6;
              }
              if (SRSLTE_SYMBOL_HAS_REF(l, q->cell.cp, q->cell.nof_ports)) {
                prb_cp_ref(&in_ptr, &out_ptr, offset, nof_refs, nof_refs/2, put);
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
    r = abs((int) (input - in_ptr));
  } else {
    r = abs((int) (output - out_ptr));
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
int srslte_pdsch_put(srslte_pdsch_t *q, cf_t *symbols, cf_t *sf_symbols,
    srslte_ra_dl_grant_t *grant, uint32_t lstart, uint32_t subframe) 
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
int srslte_pdsch_get(srslte_pdsch_t *q, cf_t *sf_symbols, cf_t *symbols,
    srslte_ra_dl_grant_t *grant, uint32_t lstart, uint32_t subframe) 
{
  return srslte_pdsch_cp(q, sf_symbols, symbols, grant, lstart, subframe, false);
}

/** Initializes the PDCCH transmitter or receiver */
int srslte_pdsch_init_multi(srslte_pdsch_t *q, srslte_cell_t cell, uint32_t nof_antennas, bool is_receiver)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;
  int i;

 if (q != NULL                  &&
     srslte_cell_isvalid(&cell) && 
     nof_antennas <= SRSLTE_MAX_PORTS) 
  {   
    
    bzero(q, sizeof(srslte_pdsch_t));
    ret = SRSLTE_ERROR;
    
    q->cell = cell;
    q->max_re = q->cell.nof_prb * MAX_PDSCH_RE(q->cell.cp);
    if (is_receiver) {
      q->nof_rx_antennas = nof_antennas;
    }

    INFO("Init PDSCH: %d ports %d PRBs, max_symbols: %d\n", q->cell.nof_ports,
        q->cell.nof_prb, q->max_re);

    for (i = 0; i < 4; i++) {
      if (srslte_modem_table_lte(&q->mod[i], modulations[i])) {
        goto clean;
      }
      srslte_modem_table_bytes(&q->mod[i]);
    }

    for (i = 0; i < SRSLTE_MAX_CODEWORDS; i++) {
      if (srslte_sch_init(&q->dl_sch[i])) {
        ERROR("Initiating DL SCH");
        goto clean;
      }

      // Allocate int16_t for reception (LLRs)
      q->e[i] = srslte_vec_malloc(sizeof(int16_t) * q->max_re * srslte_mod_bits_x_symbol(SRSLTE_MOD_64QAM));
      if (!q->e[i]) {
        goto clean;
      }

      q->d[i] = srslte_vec_malloc(sizeof(cf_t) * q->max_re);
      if (!q->d[i]) {
        goto clean;
      }

#ifndef SRSLTE_SINGLE_THREAD
      if (sem_init(&q->thread_args[i].start, 0, 0)) {
        ERROR("Creating semaphore");
        goto clean;
      }
      if (sem_init(&q->thread_args[i].finish, 0, 0)) {
        ERROR("Creating semaphore");
        goto clean;
      }
      q->thread_args[i].codeword_idx = (uint32_t) i;
      q->thread_args[i].pdsch_ptr = q;
      pthread_create(&q->threads[i], NULL, (is_receiver) ? srslte_pdsch_decode_thread : srslte_pdsch_encode_thread,
                     (void *) &q->thread_args[i]);
#endif /* SRSLTE_SINGLE_THREAD */
    }

    /* Layer mapped symbols memory allocation */
    for (i = 0; i < q->cell.nof_ports; i++) {
      q->x[i] = srslte_vec_malloc(sizeof(cf_t) * q->max_re);
      if (!q->x[i]) {
        goto clean;
      }
    }

    /* If it is the receiver side, allocate estimated channel */
    if (is_receiver) {
      for (i = 0; i < q->cell.nof_ports; i++) {
        for (int j = 0; j < q->nof_rx_antennas; j++) {
          q->ce[i][j] = srslte_vec_malloc(sizeof(cf_t) * q->max_re);
          if (!q->ce[i][j]) {
            goto clean;
          }
        }
      }
    }
    for (int j=0;j<SRSLTE_MAX(q->nof_rx_antennas, q->cell.nof_ports);j++) {
      q->symbols[j] = srslte_vec_malloc(sizeof(cf_t) * q->max_re);
      if (!q->symbols[j]) {
        goto clean;
      }              
    }
    
    /* Allocate User memory (all zeros) */
    q->users = calloc(sizeof(srslte_pdsch_user_t*), 1+SRSLTE_SIRNTI);
    if (!q->users) {
      perror("malloc");
      goto clean;
    }
 }

  ret = SRSLTE_SUCCESS;

  clean:
  if (ret == SRSLTE_ERROR) {
    srslte_pdsch_free(q);
  }
  return ret;
}

int srslte_pdsch_init(srslte_pdsch_t *q, srslte_cell_t cell) {
  return srslte_pdsch_init_multi(q, cell, 1, true);
}

int srslte_pdsch_init_tx_multi(srslte_pdsch_t *q, srslte_cell_t cell) {
  return srslte_pdsch_init_multi(q, cell, 0, false);
}

int srslte_pdsch_init_rx_multi(srslte_pdsch_t *q, srslte_cell_t cell, uint32_t nof_antennas) {
  return srslte_pdsch_init_multi(q, cell, nof_antennas, true);
}

void srslte_pdsch_free(srslte_pdsch_t *q) {
  int i;

  for (i = 0; i < SRSLTE_MAX_CODEWORDS; i++) {

#ifndef SRSLTE_SINGLE_THREAD
    /* Stop threads */
    q->thread_args[i].quit = true;
    sem_post(&q->thread_args[i].start);
    pthread_join(q->threads[i], NULL);
    pthread_detach(q->threads[i]);

#endif /* SRSLTE_SINGLE_THREAD */

    if (q->e[i]) {
      free(q->e[i]);
    }

    if (q->d[i]) {
      free(q->d[i]);
    }

    /* Free sch objects */
    srslte_sch_free(&q->dl_sch[i]);


  }

  for (i = 0; i < q->cell.nof_ports; i++) {
    if (q->x[i]) {
      free(q->x[i]);
    }
    for (int j=0;j<q->nof_rx_antennas;j++) {
      if (q->ce[i][j]) {
        free(q->ce[i][j]);
      }
    }
  }
  for (int j=0;j<SRSLTE_MAX_PORTS;j++) {
    if (q->symbols[j]) {
      free(q->symbols[j]);
    }          
  }
  if (q->users) {
    for (uint16_t u=0;u<SRSLTE_SIRNTI;u++) {
      if (q->users[u]) {
        srslte_pdsch_free_rnti(q, u);
      }
    }      
    free(q->users);
  }
  for (i = 0; i < 4; i++) {
    srslte_modem_table_free(&q->mod[i]);
  }

  bzero(q, sizeof(srslte_pdsch_t));

}

/* Configures the structure srslte_pdsch_cfg_t from the DL DCI allocation dci_msg. 
 * If dci_msg is NULL, the grant is assumed to be already stored in cfg->grant
 */
int srslte_pdsch_cfg(srslte_pdsch_cfg_t *cfg, srslte_cell_t cell, srslte_ra_dl_grant_t *grant, uint32_t cfi, uint32_t sf_idx, uint32_t rvidx) 
{
  if (cfg) {
    if (grant) {
      memcpy(&cfg->grant, grant, sizeof(srslte_ra_dl_grant_t));
    }
    if (srslte_cbsegm(&cfg->cb_segm, cfg->grant.mcs.tbs)) {
      fprintf(stderr, "Error computing Codeblock segmentation for TBS=%d\n", cfg->grant.mcs.tbs);
      return SRSLTE_ERROR; 
    }
    srslte_ra_dl_grant_to_nbits(&cfg->grant, cfi, cell, sf_idx, &cfg->nbits);
    cfg->sf_idx = sf_idx; 
    cfg->rv = rvidx;  

    return SRSLTE_SUCCESS;   
  } else {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }
}

/* Configures the structure srslte_pdsch_cfg_t from the DL DCI allocation dci_msg.
 * If dci_msg is NULL, the grant is assumed to be already stored in cfg->grant
 */
int srslte_pdsch_cfg_multi(srslte_pdsch_cfg_t *cfg, srslte_cell_t cell, srslte_ra_dl_grant_t *grant, uint32_t cfi,
                           uint32_t sf_idx, uint32_t rvidx, uint32_t rvidx2, srslte_mimo_type_t mimo_type, uint32_t pmi)
{
  if (cfg) {
    if (grant) {
      memcpy(&cfg->grant, grant, sizeof(srslte_ra_dl_grant_t));
    }
    if (srslte_cbsegm(&cfg->cb_segm, (uint32_t) cfg->grant.mcs.tbs)) {
      fprintf(stderr, "Error computing Codeblock (1) segmentation for TBS=%d\n", cfg->grant.mcs.tbs);
      return SRSLTE_ERROR;
    }

    if (srslte_cbsegm(&cfg->cb_segm2, (uint32_t) cfg->grant.mcs2.tbs)) {
      fprintf(stderr, "Error computing Codeblock (2) segmentation for TBS=%d\n", cfg->grant.mcs2.tbs);
      return SRSLTE_ERROR;
    }

    srslte_ra_dl_grant_to_nbits_multi(&cfg->grant, cfi, cell, sf_idx, &cfg->nbits, &cfg->nbits2);
    cfg->sf_idx = sf_idx;
    cfg->rv = rvidx;
    cfg->rv2 = rvidx2;
    cfg->mimo_type = mimo_type;

    /* Check and configure PDSCH transmission modes */
    switch(mimo_type) {
      case SRSLTE_MIMO_TYPE_SINGLE_ANTENNA:
        if (grant->nof_tb != 1) {
          ERROR("Number of transport blocks is not supported for single transmission mode.");
          return SRSLTE_ERROR;
        }
        cfg->nof_layers = 1;
        break;
      case SRSLTE_MIMO_TYPE_TX_DIVERSITY:
        if (grant->nof_tb != 1) {
          ERROR("Number of transport blocks is not supported for transmit diversity mode.");
          return SRSLTE_ERROR;
        }
        cfg->nof_layers = 2;
        break;
      case SRSLTE_MIMO_TYPE_SPATIAL_MULTIPLEX:
        if (grant->nof_tb == 1) {
          cfg->codebook_idx = pmi;
          cfg->nof_layers = 1;
        } else {
          cfg->codebook_idx = pmi + 1;
          cfg->nof_layers = 2;
        }
        INFO("PDSCH configured for Spatial Multiplex; nof_codewords=%d; nof_layers=%d; codebook_idx=%d;\n", grant->nof_tb, cfg->nof_layers, cfg->codebook_idx);
        break;
      case SRSLTE_MIMO_TYPE_CDD:
        if (grant->nof_tb != 2) {
          ERROR("Number of transport blocks is not supported for CDD transmission mode.");
          return SRSLTE_ERROR;
        }
        cfg->nof_layers = 2;
        break;
    }

    return SRSLTE_SUCCESS;
  } else {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }
}

/* Precalculate the PDSCH scramble sequences for a given RNTI. This function takes a while 
 * to execute, so shall be called once the final C-RNTI has been allocated for the session.
 */
int srslte_pdsch_set_rnti(srslte_pdsch_t *q, uint16_t rnti) {
  uint32_t i, j;
  if (!q->users[rnti]) {
    q->users[rnti] = calloc(1, sizeof(srslte_pdsch_user_t));
    if (q->users[rnti]) {
      for (i = 0; i < SRSLTE_NSUBFRAMES_X_FRAME; i++) {
        for (j = 0; j < SRSLTE_MAX_CODEWORDS; j++) {
          if (srslte_sequence_pdsch(&q->users[rnti]->seq[j][i], rnti, j, 2 * i, q->cell.id,
                                    q->max_re * srslte_mod_bits_x_symbol(SRSLTE_MOD_64QAM))) {
            ERROR("Generating scrambling sequence");
            return SRSLTE_ERROR;
          }
        }
      }
      q->users[rnti]->sequence_generated = true;
    }
  }
  return SRSLTE_SUCCESS;
}

static inline int srslte_pdsch_codeword_encode(srslte_pdsch_t *pdsch, srslte_pdsch_cfg_t *cfg,
                                               srslte_softbuffer_tx_t *softbuffer, uint16_t rnti, uint8_t *data,
                                               uint32_t codeword_idx) {
  srslte_sch_t *dl_sch = &pdsch->dl_sch[codeword_idx];
  srslte_ra_nbits_t *nbits = (codeword_idx == 0) ? &cfg->nbits : &cfg->nbits2;
  srslte_ra_mcs_t *mcs = (codeword_idx == 0) ? &cfg->grant.mcs : &cfg->grant.mcs2;

  if (nbits->nof_bits) {
    INFO("Encoding PDSCH SF: %d (TB %d), Mod %s, NofBits: %d, NofSymbols: %d, NofBitsE: %d, rv_idx: %d\n",
         cfg->sf_idx, codeword_idx, srslte_mod_string(mcs->mod), mcs->tbs,
         nbits->nof_re, nbits->nof_bits, (codeword_idx == 0) ? cfg->rv : cfg->rv2);

    /* Channel coding */
    if (srslte_dlsch_encode2(dl_sch, cfg, softbuffer, data, pdsch->e[codeword_idx], codeword_idx)) {
      ERROR("Error encoding TB %d", codeword_idx);
      return SRSLTE_ERROR;
    }

    /* Bit scrambling */
    if (!pdsch->users[rnti]) {
      srslte_sequence_t seq;

      if (srslte_sequence_pdsch(&seq, rnti, codeword_idx, 2 * cfg->sf_idx, pdsch->cell.id, nbits->nof_bits)) {
        ERROR("Initialising scrambling sequence");
        return SRSLTE_ERROR;
      }
      srslte_scrambling_bytes(&seq, (uint8_t *) pdsch->e[codeword_idx], nbits->nof_bits);
      srslte_sequence_free(&seq);

    } else {
      srslte_scrambling_bytes(&pdsch->users[rnti]->seq[codeword_idx][cfg->sf_idx],
                              (uint8_t *) pdsch->e[codeword_idx],
                              nbits->nof_bits);
    }

    /* Bit mapping */
    srslte_mod_modulate_bytes(&pdsch->mod[mcs->mod],
                              (uint8_t *) pdsch->e[codeword_idx],
                              pdsch->d[codeword_idx], nbits->nof_bits);
  }

  return SRSLTE_SUCCESS;
}


#ifndef SRSLTE_SINGLE_THREAD

static void *srslte_pdsch_encode_thread(void *arg) {
  srslte_pdsch_thread_args_t *q = (srslte_pdsch_thread_args_t *) arg;
  uint32_t codeword_idx = q->codeword_idx;

  INFO("[PDSCH Encoder CW %d] waiting for data\n", codeword_idx);

  sem_wait(&q->start);
  while (!q->quit) {
    q->ret_status = srslte_pdsch_codeword_encode(q->pdsch_ptr, q->cfg, q->softbuffer, q->rnti, q->data, codeword_idx);

    /* Post finish semaphore */
    sem_post(&q->finish);

    /* Wait for next loop */
    sem_wait(&q->start);
  }

  pthread_exit(NULL);
  return q;
}

#endif /* SRSLTE_SINGLE_THREAD */

static inline int srslte_pdsch_codeword_decode(srslte_pdsch_t *pdsch, srslte_pdsch_cfg_t *cfg,
                                               srslte_softbuffer_rx_t *softbuffer, uint16_t rnti, uint8_t *data,
                                               uint32_t codeword_idx) {
  srslte_sch_t *dl_sch = &pdsch->dl_sch[codeword_idx];
  srslte_ra_nbits_t *nbits = (codeword_idx == 0) ? &cfg->nbits : &cfg->nbits2;
  srslte_ra_mcs_t *mcs = (codeword_idx == 0) ? &cfg->grant.mcs : &cfg->grant.mcs2;

  if (nbits->nof_bits) {
    INFO("Decoding PDSCH SF: %d (TB %d), Mod %s, NofBits: %d, NofSymbols: %d, NofBitsE: %d, rv_idx: %d\n",
         cfg->sf_idx, codeword_idx, srslte_mod_string(mcs->mod), mcs->tbs,
         nbits->nof_re, nbits->nof_bits, (codeword_idx == 0) ? cfg->rv : cfg->rv2);

    srslte_demod_soft_demodulate_s(mcs->mod, pdsch->d[codeword_idx], pdsch->e[codeword_idx], cfg->nbits.nof_re);

    if (pdsch->users[rnti] && pdsch->users[rnti]->sequence_generated) {
      srslte_scrambling_s_offset(&pdsch->users[rnti]->seq[codeword_idx][cfg->sf_idx], pdsch->e[codeword_idx],
                                 0, nbits->nof_bits);
    } else {
      srslte_sequence_t seq;
      if (srslte_sequence_pdsch(&seq, rnti, codeword_idx, 2 * cfg->sf_idx, pdsch->cell.id, nbits->nof_bits)) {
        ERROR("Initialising scrambling sequence");
        return SRSLTE_ERROR;
      }
      srslte_scrambling_s_offset(&seq, pdsch->e[codeword_idx], codeword_idx, nbits->nof_bits);
      srslte_sequence_free(&seq);
    }

    return srslte_dlsch_decode2(dl_sch, cfg, softbuffer, pdsch->e[codeword_idx], data, codeword_idx);
  }

  return SRSLTE_SUCCESS;
}

#ifndef SRSLTE_SINGLE_THREAD

static void *srslte_pdsch_decode_thread(void *arg) {
  srslte_pdsch_thread_args_t *q = (srslte_pdsch_thread_args_t *) arg;
  uint32_t codeword_idx = q->codeword_idx;

  INFO("[PDSCH Encoder CW %d] waiting for data\n", codeword_idx);

  sem_wait(&q->start);
  while (!q->quit) {
    q->ret_status = srslte_pdsch_codeword_decode(q->pdsch_ptr, q->cfg, q->softbuffer, q->rnti, q->data, codeword_idx);

    /* Post finish semaphore */
    sem_post(&q->finish);

    /* Wait for next loop */
    sem_wait(&q->start);
  }

  pthread_exit(NULL);
  return q;
}

#endif /* SRSLTE_SINGLE_THREAD */


void srslte_pdsch_free_rnti(srslte_pdsch_t* q, uint16_t rnti)
{
  if (q->users[rnti]) {
    for (int i = 0; i < SRSLTE_NSUBFRAMES_X_FRAME; i++) {
      for (int j = 0; j < SRSLTE_MAX_CODEWORDS; j++) {
        srslte_sequence_free(&q->users[rnti]->seq[j][i]);
      }
    }
    free(q->users[rnti]);
    q->users[rnti] = NULL;
  }
}

int srslte_pdsch_decode(srslte_pdsch_t *q,
                        srslte_pdsch_cfg_t *cfg, srslte_softbuffer_rx_t *softbuffer,
                        cf_t *sf_symbols, cf_t *ce[SRSLTE_MAX_PORTS], float noise_estimate,
                        uint16_t rnti, uint8_t *data)
{
  cf_t *_sf_symbols[SRSLTE_MAX_PORTS];
  cf_t *_ce[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS];

  _sf_symbols[0] = sf_symbols;
  for (int i=0;i<q->cell.nof_ports;i++) {
    _ce[i][0] = ce[i];
  }
  return srslte_pdsch_decode_multi(q, cfg, softbuffer, _sf_symbols, _ce, noise_estimate, rnti, &data);
}

/** Decodes the PDSCH from the received symbols
 */
int srslte_pdsch_decode_multi(srslte_pdsch_t *q,
                              srslte_pdsch_cfg_t *cfg, srslte_softbuffer_rx_t softbuffers[SRSLTE_MAX_CODEWORDS],
                              cf_t *sf_symbols[SRSLTE_MAX_PORTS], cf_t *ce[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS], float noise_estimate,
                              uint16_t rnti, uint8_t *data[SRSLTE_MAX_CODEWORDS])
{

  /* Set pointers for layermapping & precoding */
  uint32_t i;
  cf_t *x[SRSLTE_MAX_LAYERS];
  int ret = 0;

  if (q            != NULL &&
      sf_symbols   != NULL &&
      data         != NULL &&
      cfg          != NULL)
  {

    INFO("Decoding PDSCH SF: %d, RNTI: 0x%x, Mod %s, TBS: %d, NofSymbols: %d, NofBitsE: %d, rv_idx: [%d %d], C_prb=%d\n",
        cfg->sf_idx, rnti, srslte_mod_string(cfg->grant.mcs.mod), cfg->grant.mcs.tbs, cfg->nbits.nof_re,
         cfg->nbits.nof_bits, cfg->rv, cfg->rv2, cfg->grant.nof_prb);

    for (int j=0;j<q->nof_rx_antennas;j++) {
      /* extract symbols */
      int n = srslte_pdsch_get(q, sf_symbols[j], q->symbols[j], &cfg->grant, cfg->nbits.lstart, cfg->sf_idx);
      if (n != cfg->nbits.nof_re) {
        fprintf(stderr, "Error expecting %d symbols but got %d\n", cfg->nbits.nof_re, n);
        return SRSLTE_ERROR;
      }

      /* extract channel estimates */
      for (i = 0; i < q->cell.nof_ports; i++) {
        n = srslte_pdsch_get(q, ce[i][j], q->ce[i][j], &cfg->grant, cfg->nbits.lstart, cfg->sf_idx);
        if (n != cfg->nbits.nof_re) {
          fprintf(stderr, "Error expecting %d symbols but got %d\n", cfg->nbits.nof_re, n);
          return SRSLTE_ERROR;
        }
      }
    }

    INFO("PDSCH Layer demapper and predecode: mimo_type=%d, nof_layers=%d, nof_tb=%d\n", cfg->mimo_type,
         cfg->nof_layers, cfg->grant.nof_tb);
    if (q->cell.nof_ports == 1) {
      /* no need for layer demapping */
      srslte_predecoding_single_multi(q->symbols, q->ce[0], q->d[0], q->nof_rx_antennas, cfg->nbits.nof_re, noise_estimate);
    } else {
      int nof_symbols [SRSLTE_MAX_CODEWORDS];
      nof_symbols[0] = cfg->nbits.nof_re * cfg->grant.nof_tb / cfg->nof_layers;
      nof_symbols[1] = cfg->nbits2.nof_re * cfg->grant.nof_tb / cfg->nof_layers;

      if (cfg->nof_layers == cfg->grant.nof_tb) {
        for (i = 0; i < cfg->nof_layers; i++) {
          x[i] = q->d[i];
        }

        srslte_predecoding_type_multi(q->symbols, q->ce, x, q->nof_rx_antennas, q->cell.nof_ports, cfg->nof_layers,
                                      cfg->codebook_idx, cfg->nbits.nof_re, cfg->mimo_type, noise_estimate);

      } else {
        /* number of layers equals number of ports */
        for (i = 0; i < cfg->nof_layers; i++) {
          x[i] = q->x[i];
        }
        memset(&x[cfg->nof_layers], 0, sizeof(cf_t*) * (SRSLTE_MAX_LAYERS - cfg->nof_layers));

        srslte_predecoding_type_multi(q->symbols, q->ce, x, q->nof_rx_antennas, q->cell.nof_ports, cfg->nof_layers,
                                      cfg->codebook_idx, cfg->nbits.nof_re, cfg->mimo_type, noise_estimate);

        srslte_layerdemap_type(x, q->d, cfg->nof_layers, cfg->grant.nof_tb,
                               nof_symbols[0], nof_symbols, cfg->mimo_type);
      }
    }

    if (SRSLTE_VERBOSE_ISDEBUG()) {
      char filename[FILENAME_MAX];
      for (int j = 0; j < q->nof_rx_antennas; j++) {
        if (snprintf(filename, FILENAME_MAX, "subframe_p%d.dat", j) < 0) {
          ERROR("Generating file name");
          break;
        }
        DEBUG("SAVED FILE %s: received subframe symbols\n", filename);
        srslte_vec_save_file(filename, sf_symbols, SRSLTE_SF_LEN_RE(q->cell.nof_prb, q->cell.cp)*sizeof(cf_t));

        for (i = 0; i < q->cell.nof_ports; i++) {
          if (snprintf(filename, FILENAME_MAX, "hest_%d%d.dat", i, j) < 0) {
            ERROR("Generating file name");
            break;
          }
          DEBUG("SAVED FILE %s: channel estimates for Tx %d and Rx %d\n", filename, j, i);
          srslte_vec_save_file(filename, ce[i][j], SRSLTE_SF_LEN_RE(q->cell.nof_prb, q->cell.cp)*sizeof(cf_t));
        }
      }
      DEBUG("SAVED FILE pdsch_symbols.dat: symbols after equalization\n",0);
      srslte_vec_save_file("pdsch_symbols.dat", q->d, cfg->nbits.nof_re*sizeof(cf_t));
    }

#ifndef SRSLTE_SINGLE_THREAD

    for (i = 0; i < cfg->grant.nof_tb; i++) {
      srslte_pdsch_thread_args_t *thread_args = &q->thread_args[i];
      thread_args->cfg = cfg;
      thread_args->softbuffer = &softbuffers[i];
      thread_args->data = data[i];
      thread_args->rnti = rnti;
      sem_post(&thread_args->start);
    }

    for (i = 0; i < cfg->grant.nof_tb; i++) {
      srslte_pdsch_thread_args_t *thread_args = &q->thread_args[i];
      sem_wait(&thread_args->finish);
      ret |= thread_args->ret_status;
    }

#else
    /* demodulate symbols
    * The MAX-log-MAP algorithm used in turbo decoding is unsensitive to SNR estimation,
    * thus we don't need tot set it in the LLRs normalization
    */
    for (uint32_t tb = 0; tb < cfg->grant.nof_tb; tb ++) {
        ret |= srslte_pdsch_codeword_decode(q, cfg, &softbuffers[tb], rnti, data[tb], tb);
    }
#endif /* SRSLTE_SINGLE_THREAD */

    if (SRSLTE_VERBOSE_ISDEBUG()) {
      DEBUG("SAVED FILE llr.dat: LLR estimates after demodulation and descrambling\n",0);
      srslte_vec_save_file("llr.dat", q->e, cfg->nbits.nof_bits*sizeof(int16_t));
    }

    return ret;

  } else {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }
}

int srslte_pdsch_ri_pmi_select(srslte_pdsch_t *q,
                               srslte_pdsch_cfg_t *cfg,
                               cf_t *ce[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS], float noise_estimate, uint32_t nof_ce,
                               uint32_t *ri, uint32_t *pmi,
                               float *current_sinr) {
  uint32_t best_pmi_1l;
  uint32_t best_pmi_2l;
  float sinr_1l[SRSLTE_MAX_CODEBOOKS];
  float sinr_2l[SRSLTE_MAX_CODEBOOKS];
  float best_sinr_1l = 0.0;
  float best_sinr_2l = 0.0;
  int n1, n2;

  if (q->cell.nof_ports == 2 && q->nof_rx_antennas == 2) {
    n1 = srslte_precoding_pmi_select(ce, nof_ce, noise_estimate, 1, &best_pmi_1l, sinr_1l);
    if (n1 < 0) {
      ERROR("PMI Select for 1 layer");
      return SRSLTE_ERROR;
    }

    n2 = srslte_precoding_pmi_select(ce, nof_ce, noise_estimate, 2, &best_pmi_2l, sinr_2l);
    if (n2 < 0) {
      ERROR("PMI Select for 2 layer");
      return SRSLTE_ERROR;
    }

    for (int i = 0; i < n1; i++) {
      if (sinr_1l[i] > best_sinr_1l) {
        best_sinr_1l = sinr_1l[i];
      }
    }

    for (int i = 0; i < n2; i++) {
      if (sinr_2l[i] > best_sinr_2l) {
        best_sinr_2l = sinr_2l[i];
      }
    }

    /* Set RI */
    if (ri != NULL) {
      *ri = (best_sinr_1l > best_sinr_2l) ? 1 : 2;
    }

    /* Set PMI */
    if (pmi != NULL) {
      *pmi = (best_sinr_1l > best_sinr_2l) ? best_pmi_1l : best_pmi_2l;
    }

    /* Set current SINR */
    if (current_sinr != NULL && cfg->mimo_type == SRSLTE_MIMO_TYPE_SPATIAL_MULTIPLEX) {
      if (cfg->nof_layers == 1) {
        *current_sinr = sinr_1l[cfg->codebook_idx];
      } else if (cfg->nof_layers == 2) {
        *current_sinr = sinr_2l[cfg->codebook_idx - 1];
      } else {
        ERROR("Not implemented number of layers");
        return SRSLTE_ERROR;
      }
    }

    /* Print Trace */
    if (ri != NULL && pmi != NULL && current_sinr != NULL) {
      INFO("PDSCH Select RI=%d; PMI=%d; Current SINR=%.1fdB (nof_layers=%d, codebook_idx=%d)\n", *ri, *pmi,
           10*log10(*current_sinr), cfg->nof_layers, cfg->codebook_idx);
    }
  } else {
    ERROR("Not implemented configuration");
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  return SRSLTE_SUCCESS;
}

int srslte_pdsch_encode(srslte_pdsch_t *q,
                        srslte_pdsch_cfg_t *cfg, srslte_softbuffer_tx_t *softbuffer,
                        uint8_t *data, uint16_t rnti, cf_t *sf_symbols[SRSLTE_MAX_PORTS])
{

  int i;
  /* Set pointers for layermapping & precoding */
  cf_t *x[SRSLTE_MAX_LAYERS];
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

   if (q             != NULL &&
       cfg  != NULL)
  {

    for (i=0;i<q->cell.nof_ports;i++) {
      if (sf_symbols[i] == NULL) {
        return SRSLTE_ERROR_INVALID_INPUTS;
      }
    }

    if (cfg->grant.mcs.tbs == 0) {
      return SRSLTE_ERROR_INVALID_INPUTS;
    }

    if (cfg->nbits.nof_re > q->max_re) {
      fprintf(stderr,
          "Error too many RE per subframe (%d). PDSCH configured for %d RE (%d PRB)\n",
          cfg->nbits.nof_re, q->max_re, q->cell.nof_prb);
      return SRSLTE_ERROR_INVALID_INPUTS;
    }

    INFO("Encoding PDSCH SF: %d, Mod %s, NofBits: %d, NofSymbols: %d, NofBitsE: %d, rv_idx: %d\n",
        cfg->sf_idx, srslte_mod_string(cfg->grant.mcs.mod), cfg->grant.mcs.tbs,
         cfg->nbits.nof_re, cfg->nbits.nof_bits, cfg->rv);

    /* number of layers equals number of ports */
    for (i = 0; i < q->cell.nof_ports; i++) {
      x[i] = q->x[i];
    }
    memset(&x[q->cell.nof_ports], 0, sizeof(cf_t*) * (SRSLTE_MAX_LAYERS - q->cell.nof_ports));

    if (srslte_dlsch_encode(&q->dl_sch[0], cfg, softbuffer, data, q->e[0])) {
      fprintf(stderr, "Error encoding TB\n");
      return SRSLTE_ERROR;
    }

    /* scramble */
    if (q->users[rnti] && q->users[rnti]->sequence_generated) {
      srslte_scrambling_bytes(&q->users[rnti]->seq[0][cfg->sf_idx], (uint8_t*) q->e[0], cfg->nbits.nof_bits);
    } else {
      srslte_sequence_t seq;
      if (srslte_sequence_pdsch(&seq, rnti, 0, 2 * cfg->sf_idx, q->cell.id, cfg->nbits.nof_bits)) {
        return SRSLTE_ERROR;
      }
      srslte_scrambling_bytes(&seq, (uint8_t*) q->e[0], cfg->nbits.nof_bits);
      srslte_sequence_free(&seq);
    }

    srslte_mod_modulate_bytes(&q->mod[cfg->grant.mcs.mod], (uint8_t*) q->e[0], q->d[0], cfg->nbits.nof_bits);

    /* TODO: only diversity supported */
    if (q->cell.nof_ports > 1) {
      srslte_layermap_diversity(q->d[0], x, q->cell.nof_ports, cfg->nbits.nof_re);
      srslte_precoding_diversity(x, q->symbols, q->cell.nof_ports,
          cfg->nbits.nof_re / q->cell.nof_ports);
    } else {
      memcpy(q->symbols[0], q->d, cfg->nbits.nof_re * sizeof(cf_t));
    }

    /* mapping to resource elements */
    for (i = 0; i < q->cell.nof_ports; i++) {
      srslte_pdsch_put(q, q->symbols[i], sf_symbols[i], &cfg->grant, cfg->nbits.lstart, cfg->sf_idx);
    }

    ret = SRSLTE_SUCCESS;
  }
  return ret;
}

int srslte_pdsch_encode_multi(srslte_pdsch_t *q,
                              srslte_pdsch_cfg_t *cfg, srslte_softbuffer_tx_t softbuffers[SRSLTE_MAX_CODEWORDS],
                              uint8_t *data[SRSLTE_MAX_CODEWORDS], uint16_t rnti, cf_t *sf_symbols[SRSLTE_MAX_PORTS]) {

  int i;
  /* Set pointers for layermapping & precoding */
  cf_t *x[SRSLTE_MAX_LAYERS];
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q != NULL &&
      cfg != NULL) {

    for (i = 0; i < q->cell.nof_ports; i++) {
      if (sf_symbols[i] == NULL) {
        return SRSLTE_ERROR_INVALID_INPUTS;
      }
    }

    /* If both transport block sizes are zero return error */
    if (cfg->grant.mcs.tbs == 0 && cfg->grant.mcs2.tbs == 0) {
      return SRSLTE_ERROR_INVALID_INPUTS;
    }

    if (cfg->nbits.nof_re > q->max_re) {
      fprintf(stderr,
              "Error too many RE per subframe (%d). PDSCH configured for %d RE (%d PRB)\n",
              cfg->nbits.nof_re, q->max_re, q->cell.nof_prb);
      return SRSLTE_ERROR_INVALID_INPUTS;
    }

#ifndef SRSLTE_SINGLE_THREAD

    for (int tb = 0; tb < cfg->grant.nof_tb; tb++) {
      srslte_pdsch_thread_args_t *thread_args = &q->thread_args[tb];
      thread_args->cfg = cfg;
      thread_args->softbuffer = &softbuffers[tb];
      thread_args->data = data[tb];
      thread_args->rnti = rnti;
      sem_post(&thread_args->start);
    }

    for (int tb = 0; tb < cfg->grant.nof_tb; tb++) {
      srslte_pdsch_thread_args_t *thread_args = &q->thread_args[tb];
      sem_wait(&thread_args->finish);
      ret |= thread_args->ret_status;
    }

#else
    for (uint32_t tb = 0; tb < cfg->grant.nof_tb; tb ++) {
      ret |= srslte_pdsch_codeword_encode(q, cfg, &softbuffers[tb], rnti, data[tb], tb);
    }
#endif /* SRSLTE_SINGLE_THREAD */

    if (q->cell.nof_ports > 1) {
      int nof_symbols;
      /* If number of layers is equal to transport blocks (codewords) skip layer mapping */
      if (cfg->nof_layers == cfg->grant.nof_tb) {
        for (i = 0; i < cfg->nof_layers; i++) {
          x[i] = q->d[i];
        }
        nof_symbols = cfg->nbits.nof_re;
      } else {
        /* Initialise layer map pointers */
        for (i = 0; i < cfg->nof_layers; i++) {
          x[i] = q->x[i];
        }
        memset(&x[cfg->nof_layers], 0, sizeof(cf_t *) * (SRSLTE_MAX_LAYERS - cfg->nof_layers));

        nof_symbols = srslte_layermap_type(q->d, x, cfg->grant.nof_tb, cfg->nof_layers,
                                           (int[SRSLTE_MAX_CODEWORDS]) {cfg->nbits.nof_re, cfg->nbits2.nof_re}, cfg->mimo_type);
      }

      /* Precode */
      srslte_precoding_type(x, q->symbols, cfg->nof_layers, q->cell.nof_ports, cfg->codebook_idx,
                            nof_symbols, cfg->mimo_type);
    } else {
      memcpy(q->symbols[0], q->d[0], cfg->nbits.nof_re * sizeof(cf_t));
    }

    /* mapping to resource elements */

    for (i = 0; i < q->cell.nof_ports; i++) {
      srslte_pdsch_put(q, q->symbols[i], sf_symbols[i], &cfg->grant, cfg->nbits.lstart, cfg->sf_idx);
    }

    ret = SRSLTE_SUCCESS;
  }
  return ret;
}

void srslte_pdsch_set_max_noi(srslte_pdsch_t *q, int max_iter) {
  for (int cw = 0; cw < SRSLTE_MAX_CODEWORDS; cw++) {
    srslte_sch_set_max_noi(&q->dl_sch[cw], max_iter);
  }
}

float srslte_pdsch_average_noi(srslte_pdsch_t *q) {
  return q->dl_sch[0].average_nof_iterations;
}

uint32_t srslte_pdsch_last_noi(srslte_pdsch_t *q) {
  return q->dl_sch[0].nof_iterations;
}



  
