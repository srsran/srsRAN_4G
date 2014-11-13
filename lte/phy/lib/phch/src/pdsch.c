/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The libLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the libLTE library.
 *
 * libLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
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

#include "prb.h"
#include "liblte/phy/phch/pdsch.h"
#include "liblte/phy/common/phy_common.h"
#include "liblte/phy/utils/bit.h"
#include "liblte/phy/utils/debug.h"
#include "liblte/phy/utils/vector.h"


#define MAX_PDSCH_RE(cp) (2 * CP_NSYMB(cp) * 12)



const lte_mod_t modulations[4] =
    { LTE_BPSK, LTE_QPSK, LTE_QAM16, LTE_QAM64 };
    
    


int pdsch_cp(pdsch_t *q, cf_t *input, cf_t *output, ra_prb_t *prb_alloc,
    uint32_t nsubframe, bool put) {
  uint32_t s, n, l, lp, lstart, lend, nof_refs;
  bool is_pbch, is_sss;
  cf_t *in_ptr = input, *out_ptr = output;
  uint32_t offset = 0;

  INFO("%s %d RE from %d PRB\n", put ? "Putting" : "Getting",
      prb_alloc->re_sf[nsubframe], prb_alloc->slot[0].nof_prb);

  if (q->cell.nof_ports == 1) {
    nof_refs = 2;
  } else {
    nof_refs = 4;
  }

  for (s = 0; s < 2; s++) {
    for (l = 0; l < CP_NSYMB(q->cell.cp); l++) {
      for (n = 0; n < prb_alloc->slot[s].nof_prb; n++) {
        if (s == 0) {
          lstart = prb_alloc->lstart;
        } else {
          lstart = 0;
        }
        lend = CP_NSYMB(q->cell.cp);
        is_pbch = is_sss = false;

        // Skip PSS/SSS signals
        if (s == 0 && (nsubframe == 0 || nsubframe == 5)) {
          if (prb_alloc->slot[s].prb_idx[n] >= q->cell.nof_prb / 2 - 3
              && prb_alloc->slot[s].prb_idx[n] <= q->cell.nof_prb / 2 + 3) {
            lend = CP_NSYMB(q->cell.cp) - 2;
            is_sss = true;
          }
        }
        // Skip PBCH
        if (s == 1 && nsubframe == 0) {
          if (prb_alloc->slot[s].prb_idx[n] >= q->cell.nof_prb / 2 - 3
              && prb_alloc->slot[s].prb_idx[n] <= q->cell.nof_prb / 2 + 3) {
            lstart = 4;
            is_pbch = true;
          }
        }
        lp = l + s * CP_NSYMB(q->cell.cp);
        if (put) {
          out_ptr = &output[(lp * q->cell.nof_prb + prb_alloc->slot[s].prb_idx[n])
              * RE_X_RB];
        } else {
          in_ptr = &input[(lp * q->cell.nof_prb + prb_alloc->slot[s].prb_idx[n])
              * RE_X_RB];
        }
        if (l >= lstart && l < lend) {
          if (SYMBOL_HAS_REF(l, q->cell.cp, q->cell.nof_ports)) {
            if (nof_refs == 2 && l != 0) {
              offset = q->cell.id % 3 + 3;
            } else {
              offset = q->cell.id % 3;
            }
            prb_cp_ref(&in_ptr, &out_ptr, offset, nof_refs, nof_refs, put);
          } else {
            prb_cp(&in_ptr, &out_ptr, 1);
          }
        }
        if ((q->cell.nof_prb % 2) && ((is_pbch && l < lstart) || (is_sss && l >= lend))) {
          if (prb_alloc->slot[s].prb_idx[n] == q->cell.nof_prb / 2 - 3) {
            if (SYMBOL_HAS_REF(l, q->cell.cp, q->cell.nof_ports)) {
              prb_cp_ref(&in_ptr, &out_ptr, offset, nof_refs, nof_refs/2, put);
            } else {
              prb_cp_half(&in_ptr, &out_ptr, 1);
            }
          } else if (prb_alloc->slot[s].prb_idx[n] == q->cell.nof_prb / 2 + 3) {
            if (put) {
              out_ptr += 6;
            } else {
              in_ptr += 6;
            }
            if (SYMBOL_HAS_REF(l, q->cell.cp, q->cell.nof_ports)) {
              prb_cp_ref(&in_ptr, &out_ptr, offset, nof_refs, nof_refs/2, put);
            } else {
              prb_cp_half(&in_ptr, &out_ptr, 1);
            }
          }
        }
      }
    }
  }

  if (put) {
    return abs((int) (input - in_ptr));
  } else {
    return abs((int) (output - out_ptr));
  }
}

/**
 * Puts PDSCH in slot number 1
 *
 * Returns the number of symbols written to sf_symbols
 *
 * 36.211 10.3 section 6.3.5
 */
int pdsch_put(pdsch_t *q, cf_t *pdsch_symbols, cf_t *sf_symbols,
    ra_prb_t *prb_alloc, uint32_t subframe) {
  return pdsch_cp(q, pdsch_symbols, sf_symbols, prb_alloc, subframe, true);
}

/**
 * Extracts PDSCH from slot number 1
 *
 * Returns the number of symbols written to PDSCH
 *
 * 36.211 10.3 section 6.3.5
 */
int pdsch_get(pdsch_t *q, cf_t *sf_symbols, cf_t *pdsch_symbols,
    ra_prb_t *prb_alloc, uint32_t subframe) {
  return pdsch_cp(q, sf_symbols, pdsch_symbols, prb_alloc, subframe, false);
}

/** Initializes the PDCCH transmitter and receiver */
int pdsch_init(pdsch_t *q, lte_cell_t cell) {
  int ret = LIBLTE_ERROR_INVALID_INPUTS;
  int i;

 if (q                         != NULL                  &&
     lte_cell_isvalid(&cell)) 
  {   
    
    bzero(q, sizeof(pdsch_t));
    ret = LIBLTE_ERROR;
    
    q->cell = cell;
    q->average_nof_iterations_n = 0; 
    q->max_symbols = q->cell.nof_prb * MAX_PDSCH_RE(q->cell.cp);

    INFO("Init PDSCH: %d ports %d PRBs, max_symbols: %d\n", q->cell.nof_ports,
        q->cell.nof_prb, q->max_symbols);

    if (precoding_init(&q->precoding, SF_LEN_RE(cell.nof_prb, cell.cp))) {
      fprintf(stderr, "Error initializing precoding\n");
    }

    for (i = 0; i < 4; i++) {
      if (modem_table_lte(&q->mod[i], modulations[i], true)) {
        goto clean;
      }
    }
    if (crc_init(&q->crc_tb, LTE_CRC24A, 24)) {
      goto clean;
    }
    if (crc_init(&q->crc_cb, LTE_CRC24B, 24)) {
      goto clean;
    }

    demod_soft_init(&q->demod);
    demod_soft_alg_set(&q->demod, APPROX);
    
    q->rnti_is_set = false; 

    if (tcod_init(&q->encoder, MAX_LONG_CB)) {
      goto clean;
    }
    if (tdec_init(&q->decoder, MAX_LONG_CB)) {
      goto clean;
    }

    // Allocate floats for reception (LLRs)
    q->cb_in = malloc(sizeof(uint8_t) * MAX_LONG_CB);
    if (!q->cb_in) {
      goto clean;
    }
    
    q->cb_out = malloc(sizeof(float) * (3 * MAX_LONG_CB + 12));
    if (!q->cb_out) {
      goto clean;
    }

    // Allocate floats for reception (LLRs)
    q->pdsch_e = malloc(sizeof(float) * q->max_symbols * q->mod[3].nbits_x_symbol);
    if (!q->pdsch_e) {
      goto clean;
    }
    
    q->pdsch_d = malloc(sizeof(cf_t) * q->max_symbols);
    if (!q->pdsch_d) {
      goto clean;
    }

    for (i = 0; i < q->cell.nof_ports; i++) {
      q->ce[i] = malloc(sizeof(cf_t) * q->max_symbols);
      if (!q->ce[i]) {
        goto clean;
      }
      q->pdsch_x[i] = malloc(sizeof(cf_t) * q->max_symbols);
      if (!q->pdsch_x[i]) {
        goto clean;
      }
      q->pdsch_symbols[i] = malloc(sizeof(cf_t) * q->max_symbols);
      if (!q->pdsch_symbols[i]) {
        goto clean;
      }
    }

    ret = LIBLTE_SUCCESS;
  }
  clean: 
  if (ret == LIBLTE_ERROR) {
    pdsch_free(q);
  }
  return ret;
}

void pdsch_free(pdsch_t *q) {
  int i;

  if (q->cb_in) {
    free(q->cb_in);
  }
  if (q->cb_out) {
    free(q->cb_out);
  }
  if (q->pdsch_e) {
    free(q->pdsch_e);
  }
  if (q->pdsch_d) {
    free(q->pdsch_d);
  }
  for (i = 0; i < q->cell.nof_ports; i++) {
    if (q->ce[i]) {
      free(q->ce[i]);
    }
    if (q->pdsch_x[i]) {
      free(q->pdsch_x[i]);
    }
    if (q->pdsch_symbols[i]) {
      free(q->pdsch_symbols[i]);
    }
  }

  for (i = 0; i < NSUBFRAMES_X_FRAME; i++) {
    sequence_free(&q->seq_pdsch[i]);
  }

  for (i = 0; i < 4; i++) {
    modem_table_free(&q->mod[i]);
  }
  tdec_free(&q->decoder);
  tcod_free(&q->encoder);
  precoding_free(&q->precoding);

  bzero(q, sizeof(pdsch_t));

}

int pdsch_set_rnti(pdsch_t *q, uint16_t rnti) {
  uint32_t i;
  for (i = 0; i < NSUBFRAMES_X_FRAME; i++) {
    if (sequence_pdsch(&q->seq_pdsch[i], rnti, 0, 2 * i, q->cell.id,
        q->max_symbols * q->mod[3].nbits_x_symbol)) {
      return LIBLTE_ERROR; 
    }
  }
  q->rnti_is_set = true; 
  q->rnti = rnti; 
  return LIBLTE_SUCCESS;
}
/* Calculate Codeblock Segmentation as in Section 5.1.2 of 36.212 */
static int codeblock_segmentation(struct cb_segm *s, uint32_t tbs) {
  uint32_t Bp, B, idx1;
  int ret; 
  
  B = tbs + 24;

  /* Calculate CB sizes */
  if (B < 6114) {
    s->C = 1;
    Bp = B;
  } else {
    s->C = (uint32_t) ceilf((float) B / (6114 - 24));
    Bp = B + 24 * s->C;
  }
  ret = lte_find_cb_index(Bp / s->C);
  if (ret != LIBLTE_ERROR) {
    idx1 = (uint32_t) ret;
    ret = lte_cb_size(idx1);
    if (ret != LIBLTE_ERROR) {
      s->K1 = (uint32_t) ret;
      ret = lte_cb_size(idx1 - 1);
      if (ret != LIBLTE_ERROR) {
        if (s->C == 1) {
          s->K2 = 0;
          s->C2 = 0;
          s->C1 = 1;
        } else {
          s->K2 = (uint32_t) ret;
          s->C2 = (s->C * s->K1 - Bp) / (s->K1 - s->K2);
          s->C1 = s->C - s->C2;
        }
        s->F = s->C1 * s->K1 + s->C2 * s->K2 - Bp;
        INFO("CB Segmentation: TBS: %d, C=%d, C+=%d K+=%d, C-=%d, K-=%d, F=%d, Bp=%d\n",
            tbs, s->C, s->C1, s->K1, s->C2, s->K2, s->F, Bp); 
      }
    }
  }  
  return ret;
}

int pdsch_harq_init(pdsch_harq_t *p, pdsch_t *pdsch) {
  int ret = LIBLTE_ERROR_INVALID_INPUTS;
  
  if (p != NULL) {    
    uint32_t i;
    bzero(p, sizeof(pdsch_harq_t));
    
    p->cell = pdsch->cell;
    ret = ra_tbs_from_idx(26, p->cell.nof_prb);
    if (ret != LIBLTE_ERROR) {
      p->max_cb =  (uint32_t) ret / (6114 - 24) + 1; 
      
      p->pdsch_w_buff_f = malloc(sizeof(float*) * p->max_cb);
      if (!p->pdsch_w_buff_f) {
        perror("malloc");
        return LIBLTE_ERROR;
      }
      
      p->pdsch_w_buff_c = malloc(sizeof(uint8_t*) * p->max_cb);
      if (!p->pdsch_w_buff_c) {
        perror("malloc");
        return LIBLTE_ERROR;
      }
      
      // We add 50 % larger buffer to the maximum expected bits per subframe
      // FIXME: Use HARQ buffer limitation based on UE category
      p->w_buff_size = p->cell.nof_prb * MAX_PDSCH_RE(p->cell.cp) * 6 * 2 / p->max_cb;
      for (i=0;i<p->max_cb;i++) {
        p->pdsch_w_buff_f[i] = malloc(sizeof(float) * p->w_buff_size);
        if (!p->pdsch_w_buff_f[i]) {
          perror("malloc");
          return LIBLTE_ERROR;
        }
        p->pdsch_w_buff_c[i] = malloc(sizeof(uint8_t) * p->w_buff_size);
        if (!p->pdsch_w_buff_c[i]) {
          perror("malloc");
          return LIBLTE_ERROR;
        }
      }      
      ret = LIBLTE_SUCCESS;
    }
  }
  return ret;
}

void pdsch_harq_free(pdsch_harq_t *p) {
  if (p) {
    uint32_t i;
    if (p->pdsch_w_buff_f) {
      for (i=0;i<p->max_cb;i++) {
        if (p->pdsch_w_buff_f[i]) {
          free(p->pdsch_w_buff_f[i]);
        }
      }
      free(p->pdsch_w_buff_f);
    }
    if (p->pdsch_w_buff_c) {
      for (i=0;i<p->max_cb;i++) {
        if (p->pdsch_w_buff_c[i]) {
          free(p->pdsch_w_buff_c[i]);
        }
      }
      free(p->pdsch_w_buff_c);
    }
    bzero(p, sizeof(pdsch_harq_t));
  }
}

int pdsch_harq_setup(pdsch_harq_t *p, ra_mcs_t mcs, ra_prb_t *prb_alloc) {
  int ret = LIBLTE_ERROR_INVALID_INPUTS;
  
  if (p                 != NULL         &&
      mcs.tbs            > 0)
  {
    uint32_t nof_bits, nof_bits_e, nof_symbols;
    
    p->mcs = mcs;
    memcpy(&p->prb_alloc, prb_alloc, sizeof(ra_prb_t));
    
    codeblock_segmentation(&p->cb_segm, mcs.tbs);
    
    nof_bits = mcs.tbs;
    nof_symbols = prb_alloc->re_sf[1]; // Any subframe except 0 and 5 has maximum RE
    nof_bits_e = nof_symbols * lte_mod_bits_x_symbol(mcs.mod);
    
    if (nof_bits > nof_bits_e) {
      fprintf(stderr, "Invalid code rate %.2f\n", (float) nof_bits / nof_bits_e);
      return LIBLTE_ERROR;
    }

    if (nof_symbols > p->cell.nof_prb * MAX_PDSCH_RE(p->cell.cp)) {
      fprintf(stderr,
          "Error too many RE per subframe (%d). PDSCH configured for %d RE (%d PRB)\n",
          nof_symbols, p->cell.nof_prb * MAX_PDSCH_RE(p->cell.cp), p->cell.nof_prb);
      return LIBLTE_ERROR;
    }
    
    if (p->cb_segm.C > p->max_cb) {
      fprintf(stderr, "Codeblock segmentation returned more CBs (%d) than allocated (%d)\n", 
        p->cb_segm.C, p->max_cb);
      return LIBLTE_ERROR;
    }       
    ret = LIBLTE_SUCCESS;    
  }
  return ret;
}


float pdsch_average_noi(pdsch_t *q) {
  return q->average_nof_iterations; 
}

uint32_t pdsch_last_noi(pdsch_t *q) {
  return q->nof_iterations;
}


/* Decode a transport block according to 36.212 5.3.2
 *
 */
int pdsch_decode_tb(pdsch_t *q, uint8_t *data, uint32_t tbs, uint32_t nb_e, 
                    pdsch_harq_t *harq_process, uint32_t rv_idx) 
{
  uint8_t parity[24];
  uint8_t *p_parity = parity;
  uint32_t par_rx, par_tx;
  uint32_t i;
  uint32_t cb_len, rp, wp, rlen, F, n_e;
  float *e_bits = q->pdsch_e;
  
  if (q         != NULL   && 
      data      != NULL   &&       
      nb_e      < q->max_symbols * q->mod[3].nbits_x_symbol)
  {

    rp = 0;
    rp = 0;
    wp = 0;
    for (i = 0; i < harq_process->cb_segm.C; i++) {

      /* Get read/write lengths */
      if (i < harq_process->cb_segm.C - harq_process->cb_segm.C2) {
        cb_len = harq_process->cb_segm.K1;
      } else {
        cb_len = harq_process->cb_segm.K2;
      }
      if (harq_process->cb_segm.C == 1) {
        rlen = cb_len;
      } else {
        rlen = cb_len - 24;
      }
      if (i == 0) {
        F = harq_process->cb_segm.F;
      } else {
        F = 0;
      }

      if (i < harq_process->cb_segm.C - 1) {
        n_e = nb_e / harq_process->cb_segm.C;
      } else {
        n_e = nb_e - rp;
      }

      DEBUG("CB#%d: cb_len: %d, rlen: %d, wp: %d, rp: %d, F: %d, E: %d\n", i,
          cb_len, rlen - F, wp, rp, F, n_e);

      
      /* Rate Unmatching */
      if (rm_turbo_rx(harq_process->pdsch_w_buff_f[i], harq_process->w_buff_size,  
                  &e_bits[rp], n_e, 
                  (float*) q->cb_out, 3 * cb_len + 12, rv_idx)) {
        fprintf(stderr, "Error in rate matching\n");
        return LIBLTE_ERROR;
      }

      /* Turbo Decoding with CRC-based early stopping */
      q->nof_iterations = 0; 
      bool early_stop = false;
      uint32_t len_crc; 
      uint8_t *cb_in_ptr; 
      crc_t *crc_ptr; 
      tdec_reset(&q->decoder, cb_len);
            
      do {
        
        tdec_iteration(&q->decoder, (float*) q->cb_out, cb_len); 
        q->nof_iterations++;
        
        if (harq_process->cb_segm.C > 1) {
          len_crc = cb_len; 
          cb_in_ptr = q->cb_in; 
          crc_ptr = &q->crc_cb; 
        } else {
          len_crc = tbs+24; 
          bzero(q->cb_in, F*sizeof(uint8_t));
          cb_in_ptr = &q->cb_in[F];
          crc_ptr = &q->crc_tb; 
        }

        tdec_decision(&q->decoder, q->cb_in, cb_len);
  
        /* Check Codeblock CRC and stop early if incorrect */
        if (!crc_checksum(crc_ptr, cb_in_ptr, len_crc)) {
          early_stop = true;           
        }
        
      } while (q->nof_iterations < TDEC_MAX_ITERATIONS && !early_stop);
            
      q->average_nof_iterations = VEC_CMA((float) q->nof_iterations, 
                                             q->average_nof_iterations, 
                                             q->average_nof_iterations_n);
      q->average_nof_iterations_n++;

      /* Copy data to another buffer, removing the Codeblock CRC */
      if (i < harq_process->cb_segm.C - 1) {
        memcpy(&data[wp], &q->cb_in[F], (rlen - F) * sizeof(uint8_t));
      } else {
        DEBUG("Last CB, appending parity: %d to %d from %d and 24 from %d\n",
            rlen - F - 24, wp, F, rlen - 24);
        
        /* Append Transport Block parity bits to the last CB */
        memcpy(&data[wp], &q->cb_in[F], (rlen - F - 24) * sizeof(uint8_t));
        memcpy(parity, &q->cb_in[rlen - 24], 24 * sizeof(uint8_t));
      }

      /* Set read/write pointers */
      wp += (rlen - F);
      rp += n_e;
    }

    DEBUG("END CB#%d: wp: %d, rp: %d\n", i, wp, rp);

    // Compute transport block CRC
    par_rx = crc_checksum(&q->crc_tb, data, tbs);

    // check parity bits
    par_tx = bit_unpack(&p_parity, 24);

    if (!par_rx) {
      INFO("\n\tCAUTION!! Received all-zero transport block\n\n", 0);
    }

    if (par_rx == par_tx) {
      INFO("TB decoded OK\n",i);
      return LIBLTE_SUCCESS;
    } else {
      INFO("Error in TB parity\n",i);
      return LIBLTE_ERROR;
    }
  } else {
    return LIBLTE_ERROR_INVALID_INPUTS;
  }
}

/** Decodes the PDSCH from the received symbols
 */
int pdsch_decode(pdsch_t *q, cf_t *sf_symbols, cf_t *ce[MAX_PORTS], float noise_estimate, uint8_t *data, uint32_t subframe, 
                 pdsch_harq_t *harq_process, uint32_t rv_idx) 
{

  /* Set pointers for layermapping & precoding */
  uint32_t i, n;
  cf_t *x[MAX_LAYERS];
  uint32_t nof_symbols, nof_bits, nof_bits_e;
  
  if (q                     != NULL &&
      sf_symbols            != NULL &&
      data                  != NULL &&
      subframe              <  10   &&
      harq_process          != NULL && 
      harq_process->mcs.mod > 0)
  {
    
    nof_bits = harq_process->mcs.tbs;
    nof_symbols = harq_process->prb_alloc.re_sf[subframe];
    nof_bits_e = nof_symbols * q->mod[harq_process->mcs.mod - 1].nbits_x_symbol;


    INFO("Decoding PDSCH SF: %d, Mod %d, NofBits: %d, NofSymbols: %d, NofBitsE: %d, rv_idx: %d\n",
        subframe, harq_process->mcs.mod, nof_bits, nof_symbols, nof_bits_e, rv_idx);

    /* number of layers equals number of ports */
    for (i = 0; i < q->cell.nof_ports; i++) {
      x[i] = q->pdsch_x[i];
    }
    memset(&x[q->cell.nof_ports], 0, sizeof(cf_t*) * (MAX_LAYERS - q->cell.nof_ports));
      
    /* extract symbols */
    n = pdsch_get(q, sf_symbols, q->pdsch_symbols[0], &harq_process->prb_alloc, subframe);
    if (n != nof_symbols) {
      fprintf(stderr, "Error expecting %d symbols but got %d\n", nof_symbols, n);
      return LIBLTE_ERROR;
    }

    /* extract channel estimates */
    for (i = 0; i < q->cell.nof_ports; i++) {
      n = pdsch_get(q, ce[i], q->ce[i], &harq_process->prb_alloc, subframe);
      if (n != nof_symbols) {
        fprintf(stderr, "Error expecting %d symbols but got %d\n", nof_symbols, n);
        return LIBLTE_ERROR;
      }
    }
      
    /* TODO: only diversity is supported */
    if (q->cell.nof_ports == 1) {
      /* no need for layer demapping */
      predecoding_single_mmse(&q->precoding, q->pdsch_symbols[0], q->ce[0], q->pdsch_d,
          nof_symbols, noise_estimate);
    } else {
      predecoding_diversity_zf(&q->precoding, q->pdsch_symbols[0], q->ce, x, q->cell.nof_ports,
          nof_symbols);
      layerdemap_diversity(x, q->pdsch_d, q->cell.nof_ports,
          nof_symbols / q->cell.nof_ports);
    }
    
    /* demodulate symbols 
     * The MAX-log-MAP algorithm used in turbo decoding is unsensitive to SNR estimation, 
     * thus we don't need tot set it in the LLRs normalization
     */
    demod_soft_sigma_set(&q->demod, 2.0 / q->mod[harq_process->mcs.mod - 1].nbits_x_symbol);
    demod_soft_table_set(&q->demod, &q->mod[harq_process->mcs.mod - 1]);
    demod_soft_demodulate(&q->demod, q->pdsch_d, q->pdsch_e, nof_symbols);
 
    /*
    for (int j=0;j<nof_symbols;j++) {
      if (isnan(crealf(q->pdsch_d[j])) || isnan(cimagf(q->pdsch_d[j]))) {
        printf("\nerror in d[%d]=%f+%f symbols:%f+%f ce0:%f+%f ce1:%f+%f\n",j,
               crealf(q->pdsch_d[j]), cimagf(q->pdsch_d[j]), 
               crealf(q->pdsch_symbols[0][j]), cimagf(q->pdsch_symbols[0][j]), 
               crealf(q->ce[0][j]), cimagf(q->ce[0][j]), 
               crealf(q->ce[1][j]), cimagf(q->ce[1][j])
              );
      }
    }
    */

    /* descramble */
    scrambling_f_offset(&q->seq_pdsch[subframe], q->pdsch_e, 0, nof_bits_e);
    
    return pdsch_decode_tb(q, data, nof_bits, nof_bits_e, harq_process, rv_idx);
  } else {
    return LIBLTE_ERROR_INVALID_INPUTS;
  }
}

/* Encode a transport block according to 36.212 5.3.2
 *
 */
int pdsch_encode_tb(pdsch_t *q, uint8_t *data, uint32_t tbs, uint32_t nb_e, 
                    pdsch_harq_t *harq_process, uint32_t rv_idx) 
{
  uint8_t parity[24];
  uint8_t *p_parity = parity;
  uint32_t par;
  uint32_t i;
  uint32_t cb_len, rp, wp, rlen, F, n_e;
  uint8_t *e_bits = q->pdsch_e;
  int ret = LIBLTE_ERROR_INVALID_INPUTS; 
  
  if (q             != NULL &&
      data          != NULL &&
      nb_e          <  q->max_symbols * q->mod[3].nbits_x_symbol)
  {
  
    if (q->rnti_is_set) {
      if (rv_idx == 0) {
        /* Compute transport block CRC */
        par = crc_checksum(&q->crc_tb, data, tbs);

        /* parity bits will be appended later */
        bit_pack(par, &p_parity, 24);

        if (VERBOSE_ISDEBUG()) {
          DEBUG("DATA: ", 0);
          vec_fprint_b(stdout, data, tbs);
          DEBUG("PARITY: ", 0);
          vec_fprint_b(stdout, parity, 24);
        }

        /* Add filler bits to the new data buffer */
        for (i = 0; i < harq_process->cb_segm.F; i++) {
          q->cb_in[i] = LTE_NULL_BIT;
        }
      }
      
      wp = 0;
      rp = 0;
      for (i = 0; i < harq_process->cb_segm.C; i++) {

        /* Get read lengths */
        if (i < harq_process->cb_segm.C - harq_process->cb_segm.C2) {
          cb_len = harq_process->cb_segm.K1;
        } else {
          cb_len = harq_process->cb_segm.K2;
        }
        if (harq_process->cb_segm.C > 1) {
          rlen = cb_len - 24;
        } else {
          rlen = cb_len;
        }
        if (i == 0) {
          F = harq_process->cb_segm.F;
        } else {
          F = 0;
        }

        if (i < harq_process->cb_segm.C - 1) {
          n_e = nb_e / harq_process->cb_segm.C;
        } else {
          n_e = nb_e - wp;
        }

        INFO("CB#%d: cb_len: %d, rlen: %d, wp: %d, rp: %d, F: %d, E: %d\n", i,
            cb_len, rlen - F, wp, rp, F, n_e);

        if (rv_idx == 0) {
          /* Copy data to another buffer, making space for the Codeblock CRC */
          if (i < harq_process->cb_segm.C - 1) {
            memcpy(&q->cb_in[F], &data[rp], (rlen - F) * sizeof(uint8_t));
          } else {
            INFO("Last CB, appending parity: %d from %d and 24 to %d\n",
                rlen - F - 24, rp, rlen - 24);
            /* Append Transport Block parity bits to the last CB */
            memcpy(&q->cb_in[F], &data[rp], (rlen - F - 24) * sizeof(uint8_t));
            memcpy(&q->cb_in[rlen - 24], parity, 24 * sizeof(uint8_t));
          }        
          if (harq_process->cb_segm.C > 1) {
            /* Attach Codeblock CRC */
            crc_attach(&q->crc_cb, q->cb_in, rlen);
          }
          if (VERBOSE_ISDEBUG()) {
            DEBUG("CB#%d Len=%d: ", i, cb_len);
            vec_fprint_b(stdout, q->cb_in, cb_len);
          }
          /* Turbo Encoding */
          tcod_encode(&q->encoder, q->cb_in, (uint8_t*) q->cb_out, cb_len);
        }
        
        /* Rate matching */
        if (rm_turbo_tx(harq_process->pdsch_w_buff_c[i], harq_process->w_buff_size, 
                    (uint8_t*) q->cb_out, 3 * cb_len + 12,
                    &e_bits[wp], n_e, rv_idx))
        {
          fprintf(stderr, "Error in rate matching\n");
          return LIBLTE_ERROR;
        }

        /* Set read/write pointers */
        rp += (rlen - F);
        wp += n_e;
      }

      INFO("END CB#%d: wp: %d, rp: %d\n", i, wp, rp);
      
      ret = LIBLTE_SUCCESS;      
    } else {
      fprintf(stderr, "Must call pdsch_set_rnti() to set the encoder/decoder RNTI\n");  
    }
  } 
  return ret; 
}

/** Converts the PDSCH data bits to symbols mapped to the slot ready for transmission
 */
int pdsch_encode(pdsch_t *q, uint8_t *data, cf_t *sf_symbols[MAX_PORTS], uint32_t subframe, 
                 pdsch_harq_t *harq_process, uint32_t rv_idx) 
{
  int i;
  uint32_t nof_symbols, nof_bits, nof_bits_e;
  /* Set pointers for layermapping & precoding */
  cf_t *x[MAX_LAYERS];
   int ret = LIBLTE_ERROR_INVALID_INPUTS; 
   
   if (q             != NULL &&
       data          != NULL &&
       subframe      <  10   &&
       harq_process  != NULL)
  {

    if (q->rnti_is_set) {
      for (i=0;i<q->cell.nof_ports;i++) {
        if (sf_symbols[i] == NULL) {
          return LIBLTE_ERROR_INVALID_INPUTS;
        }
      }
      
      nof_bits = harq_process->mcs.tbs;
      nof_symbols = harq_process->prb_alloc.re_sf[subframe];
      nof_bits_e = nof_symbols * q->mod[harq_process->mcs.mod - 1].nbits_x_symbol;

      if (harq_process->mcs.tbs == 0) {
        return LIBLTE_ERROR_INVALID_INPUTS;      
      }
      
      if (nof_bits > nof_bits_e) {
        fprintf(stderr, "Invalid code rate %.2f\n", (float) nof_bits / nof_bits_e);
        return LIBLTE_ERROR_INVALID_INPUTS;
      }

      if (nof_symbols > q->max_symbols) {
        fprintf(stderr,
            "Error too many RE per subframe (%d). PDSCH configured for %d RE (%d PRB)\n",
            nof_symbols, q->max_symbols, q->cell.nof_prb);
        return LIBLTE_ERROR_INVALID_INPUTS;
      }

      INFO("Encoding PDSCH SF: %d, Mod %d, NofBits: %d, NofSymbols: %d, NofBitsE: %d, rv_idx: %d\n",
          subframe, harq_process->mcs.mod, nof_bits, nof_symbols, nof_bits_e, rv_idx);

      /* number of layers equals number of ports */
      for (i = 0; i < q->cell.nof_ports; i++) {
        x[i] = q->pdsch_x[i];
      }
      memset(&x[q->cell.nof_ports], 0, sizeof(cf_t*) * (MAX_LAYERS - q->cell.nof_ports));

      if (pdsch_encode_tb(q, data, nof_bits, nof_bits_e, harq_process, rv_idx)) {
        fprintf(stderr, "Error encoding TB\n");
        return LIBLTE_ERROR;
      }
      
      scrambling_b_offset(&q->seq_pdsch[subframe], (uint8_t*) q->pdsch_e, 0, nof_bits_e);

      mod_modulate(&q->mod[harq_process->mcs.mod - 1], (uint8_t*) q->pdsch_e, q->pdsch_d, nof_bits_e);

      /* TODO: only diversity supported */
      if (q->cell.nof_ports > 1) {
        layermap_diversity(q->pdsch_d, x, q->cell.nof_ports, nof_symbols);
        precoding_diversity(&q->precoding, x, q->pdsch_symbols, q->cell.nof_ports,
            nof_symbols / q->cell.nof_ports);
      } else {
        memcpy(q->pdsch_symbols[0], q->pdsch_d, nof_symbols * sizeof(cf_t));
      }

      /* mapping to resource elements */
      for (i = 0; i < q->cell.nof_ports; i++) {
        pdsch_put(q, q->pdsch_symbols[i], sf_symbols[i], &harq_process->prb_alloc, subframe);
      }
      ret = LIBLTE_SUCCESS;
    } else {
     fprintf(stderr, "Must call pdsch_set_rnti() to set the encoder/decoder RNTI\n");       
    }
  } 
  return ret; 
}
  