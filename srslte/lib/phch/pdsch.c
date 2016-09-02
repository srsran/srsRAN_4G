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

#include "prb_dl.h"
#include "srslte/phch/pdsch.h"
#include "srslte/phch/sch.h"
#include "srslte/common/phy_common.h"
#include "srslte/utils/bit.h"
#include "srslte/utils/debug.h"
#include "srslte/utils/vector.h"


#define MAX_PDSCH_RE(cp) (2 * SRSLTE_CP_NSYMB(cp) * 12)



const static srslte_mod_t modulations[4] =
    { SRSLTE_MOD_BPSK, SRSLTE_MOD_QPSK, SRSLTE_MOD_16QAM, SRSLTE_MOD_64QAM };
    
//#define DEBUG_IDX

#ifdef DEBUG_IDX    
cf_t *offset_original=NULL;
extern int indices[100000];
extern int indices_ptr; 
#endif


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

/** Initializes the PDCCH transmitter and receiver */
int srslte_pdsch_init(srslte_pdsch_t *q, srslte_cell_t cell) {
  int ret = SRSLTE_ERROR_INVALID_INPUTS;
  int i;

 if (q                         != NULL                  &&
     srslte_cell_isvalid(&cell)) 
  {   
    
    bzero(q, sizeof(srslte_pdsch_t));
    ret = SRSLTE_ERROR;
    
    q->cell = cell;
    q->max_re = q->cell.nof_prb * MAX_PDSCH_RE(q->cell.cp);
    q->nof_crnti = 0; 

    INFO("Init PDSCH: %d ports %d PRBs, max_symbols: %d\n", q->cell.nof_ports,
        q->cell.nof_prb, q->max_re);

    for (i = 0; i < 4; i++) {
      if (srslte_modem_table_lte(&q->mod[i], modulations[i])) {
        goto clean;
      }
      srslte_modem_table_bytes(&q->mod[i]);
    }
    
    srslte_sch_init(&q->dl_sch);
    
    q->rnti_is_set = false; 

    // Allocate int16_t for reception (LLRs)
    q->e = srslte_vec_malloc(sizeof(int16_t) * q->max_re * srslte_mod_bits_x_symbol(SRSLTE_MOD_64QAM));
    if (!q->e) {
      goto clean;
    }
    
    q->d = srslte_vec_malloc(sizeof(cf_t) * q->max_re);
    if (!q->d) {
      goto clean;
    }

    for (i = 0; i < q->cell.nof_ports; i++) {
      q->ce[i] = srslte_vec_malloc(sizeof(cf_t) * q->max_re);
      if (!q->ce[i]) {
        goto clean;
      }
      q->x[i] = srslte_vec_malloc(sizeof(cf_t) * q->max_re);
      if (!q->x[i]) {
        goto clean;
      }
      q->symbols[i] = srslte_vec_malloc(sizeof(cf_t) * q->max_re);
      if (!q->symbols[i]) {
        goto clean;
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

void srslte_pdsch_free(srslte_pdsch_t *q) {
  int i;

  if (q->e) {
    free(q->e);
  }
  if (q->d) {
    free(q->d);
  }
  for (i = 0; i < q->cell.nof_ports; i++) {
    if (q->ce[i]) {
      free(q->ce[i]);
    }
    if (q->x[i]) {
      free(q->x[i]);
    }
    if (q->symbols[i]) {
      free(q->symbols[i]);
    }
  }

  for (i = 0; i < SRSLTE_NSUBFRAMES_X_FRAME; i++) {
    srslte_sequence_free(&q->seq[i]);
  }
  
  for (i = 0; i < SRSLTE_NSUBFRAMES_X_FRAME; i++) {
    for (int n=0;n<q->nof_crnti;n++) {
      srslte_sequence_free(&q->seq_multi[i][n]);
    }
    if (q->seq_multi[i]) {
      free(q->seq_multi[i]);
    }
  }
  
  if (q->rnti_multi) {
    free(q->rnti_multi);
  }

  for (i = 0; i < 4; i++) {
    srslte_modem_table_free(&q->mod[i]);
  }

  srslte_sch_free(&q->dl_sch);

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


/* Precalculate the PDSCH scramble sequences for a given RNTI. This function takes a while 
 * to execute, so shall be called once the final C-RNTI has been allocated for the session.
 */
int srslte_pdsch_set_rnti(srslte_pdsch_t *q, uint16_t rnti) {
  uint32_t i;
  for (i = 0; i < SRSLTE_NSUBFRAMES_X_FRAME; i++) {
    if (srslte_sequence_pdsch(&q->seq[i], rnti, 0, 2 * i, q->cell.id,
        q->max_re * srslte_mod_bits_x_symbol(SRSLTE_MOD_64QAM))) {
      return SRSLTE_ERROR; 
    }
  }
  q->rnti_is_set = true; 
  q->rnti = rnti; 
  return SRSLTE_SUCCESS;
}

/* Initializes the memory to support pre-calculation of multiple scrambling sequences */
int srslte_pdsch_init_rnti_multi(srslte_pdsch_t *q, uint32_t nof_rntis)
{
  for (int i = 0; i < SRSLTE_NSUBFRAMES_X_FRAME; i++) {
    q->seq_multi[i] = malloc(sizeof(srslte_sequence_t)*nof_rntis);
    if (!q->seq_multi[i]) {
      perror("malloc"); 
      return SRSLTE_ERROR; 
    }
  }
  
  q->rnti_multi = srslte_vec_malloc(sizeof(uint16_t)*nof_rntis);
  if (!q->rnti_multi) {
    perror("malloc");
    return SRSLTE_ERROR; 
  }
  bzero(q->rnti_multi, sizeof(uint16_t)*nof_rntis);
  
  q->nof_crnti = nof_rntis;
  
  return SRSLTE_SUCCESS;
}

int srslte_pdsch_set_rnti_multi(srslte_pdsch_t *q, uint32_t idx, uint16_t rnti)
{
  if (idx < q->nof_crnti) {
    if (q->rnti_multi[idx]) {
      for (uint32_t i = 0; i < SRSLTE_NSUBFRAMES_X_FRAME; i++) {
        srslte_sequence_free(&q->seq_multi[i][idx]);  
      }
      q->rnti_multi[idx] = 0; 
    }
    q->rnti_multi[idx] = rnti; 
    q->rnti_is_set = true; 
    for (uint32_t i = 0; i < SRSLTE_NSUBFRAMES_X_FRAME; i++) {
      if (srslte_sequence_pdsch(&q->seq_multi[i][idx], rnti, 0, 2 * i, q->cell.id,
          q->max_re * srslte_mod_bits_x_symbol(SRSLTE_MOD_64QAM))) {
        return SRSLTE_ERROR; 
      }
    }
    return SRSLTE_SUCCESS;
  } else {
    return SRSLTE_ERROR_INVALID_INPUTS; 
  }
}

uint16_t srslte_pdsch_get_rnti_multi(srslte_pdsch_t *q, uint32_t idx)
{
  if (idx < q->nof_crnti) {
    return q->rnti_multi[idx];
  } else {
    return SRSLTE_ERROR_INVALID_INPUTS; 
  }
}

int srslte_pdsch_decode(srslte_pdsch_t *q, 
                        srslte_pdsch_cfg_t *cfg, srslte_softbuffer_rx_t *softbuffer,
                        cf_t *sf_symbols, cf_t *ce[SRSLTE_MAX_PORTS], float noise_estimate, 
                        uint8_t *data) 
{
  if (q                     != NULL &&
      sf_symbols            != NULL &&
      data                  != NULL && 
      cfg          != NULL)
  {
    if (q->rnti_is_set) {
      return srslte_pdsch_decode_rnti(q, cfg, softbuffer, sf_symbols, ce, noise_estimate, q->rnti, data);
    } else {
      fprintf(stderr, "Must call srslte_pdsch_set_rnti() before calling srslte_pdsch_decode()\n");
      return SRSLTE_ERROR; 
    }
  } else {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }
}

/** Decodes the PDSCH from the received symbols
 */
int srslte_pdsch_decode_rnti(srslte_pdsch_t *q, 
                             srslte_pdsch_cfg_t *cfg, srslte_softbuffer_rx_t *softbuffer,
                             cf_t *sf_symbols, cf_t *ce[SRSLTE_MAX_PORTS], float noise_estimate, 
                             uint16_t rnti, uint8_t *data) 
{

  /* Set pointers for layermapping & precoding */
  uint32_t i, n;
  cf_t *x[SRSLTE_MAX_LAYERS];
  
  if (q            != NULL &&
      sf_symbols   != NULL &&
      data         != NULL && 
      cfg          != NULL)
  {
    
    INFO("Decoding PDSCH SF: %d, RNTI: 0x%x, Mod %s, TBS: %d, NofSymbols: %d, NofBitsE: %d, rv_idx: %d, C_prb=%d\n",
        cfg->sf_idx, rnti, srslte_mod_string(cfg->grant.mcs.mod), cfg->grant.mcs.tbs, cfg->nbits.nof_re, 
         cfg->nbits.nof_bits, cfg->rv, cfg->grant.nof_prb);

    /* number of layers equals number of ports */
    for (i = 0; i < q->cell.nof_ports; i++) {
      x[i] = q->x[i];
    }
    memset(&x[q->cell.nof_ports], 0, sizeof(cf_t*) * (SRSLTE_MAX_LAYERS - q->cell.nof_ports));
      
    /* extract symbols */
    n = srslte_pdsch_get(q, sf_symbols, q->symbols[0], &cfg->grant, cfg->nbits.lstart, cfg->sf_idx);
    if (n != cfg->nbits.nof_re) {
      fprintf(stderr, "Error expecting %d symbols but got %d\n", cfg->nbits.nof_re, n);
      return SRSLTE_ERROR;
    }
    
    /* extract channel estimates */
    for (i = 0; i < q->cell.nof_ports; i++) {
      n = srslte_pdsch_get(q, ce[i], q->ce[i], &cfg->grant, cfg->nbits.lstart, cfg->sf_idx);
      if (n != cfg->nbits.nof_re) {
        fprintf(stderr, "Error expecting %d symbols but got %d\n", cfg->nbits.nof_re, n);
        return SRSLTE_ERROR;
      }
    }
    
    /* TODO: only diversity is supported */
    if (q->cell.nof_ports == 1) {
      /* no need for layer demapping */
      srslte_predecoding_single(q->symbols[0], q->ce[0], q->d, cfg->nbits.nof_re, noise_estimate);
    } else {
      srslte_predecoding_diversity(q->symbols[0], q->ce, x, q->cell.nof_ports,
          cfg->nbits.nof_re);
      srslte_layerdemap_diversity(x, q->d, q->cell.nof_ports,
          cfg->nbits.nof_re / q->cell.nof_ports);
    }
    
    if (SRSLTE_VERBOSE_ISDEBUG()) {
      DEBUG("SAVED FILE subframe.dat: received subframe symbols\n",0);
      srslte_vec_save_file("subframe.dat", sf_symbols, SRSLTE_SF_LEN_RE(q->cell.nof_prb, q->cell.cp)*sizeof(cf_t));
      DEBUG("SAVED FILE hest0.dat and hest1.dat: channel estimates for port 0 and port 1\n",0);
      srslte_vec_save_file("hest0.dat", ce[0], SRSLTE_SF_LEN_RE(q->cell.nof_prb, q->cell.cp)*sizeof(cf_t));
      if (q->cell.nof_ports > 1) {
        srslte_vec_save_file("hest1.dat", ce[1], SRSLTE_SF_LEN_RE(q->cell.nof_prb, q->cell.cp)*sizeof(cf_t));
      }
      DEBUG("SAVED FILE pdsch_symbols.dat: symbols after equalization\n",0);
      srslte_vec_save_file("pdsch_symbols.dat", q->d, cfg->nbits.nof_re*sizeof(cf_t));
    }
    
    /* demodulate symbols 
    * The MAX-log-MAP algorithm used in turbo decoding is unsensitive to SNR estimation, 
    * thus we don't need tot set it in the LLRs normalization
    */
    srslte_demod_soft_demodulate_s(cfg->grant.mcs.mod, q->d, q->e, cfg->nbits.nof_re);
    
    /*
    printf("WARNING REMOVE ME!\n");
    int16_t *e=q->e;
    for (int i=0;i<cfg->nbits.nof_bits;i++) {
      e[i] = e[i]>0?10:-10;
    }
    */

    /* descramble */
    if (rnti != q->rnti) {
      srslte_sequence_t seq; 
      if (srslte_sequence_pdsch(&seq, rnti, 0, 2 * cfg->sf_idx, q->cell.id, cfg->nbits.nof_bits)) {
        return SRSLTE_ERROR; 
      }
      srslte_scrambling_s_offset(&seq, q->e, 0, cfg->nbits.nof_bits);      
      srslte_sequence_free(&seq);
    } else {    
      srslte_scrambling_s_offset(&q->seq[cfg->sf_idx], q->e, 0, cfg->nbits.nof_bits);      
    }

    if (SRSLTE_VERBOSE_ISDEBUG()) {
      DEBUG("SAVED FILE llr.dat: LLR estimates after demodulation and descrambling\n",0);
      srslte_vec_save_file("llr.dat", q->e, cfg->nbits.nof_bits*sizeof(int16_t));
    }

    return srslte_dlsch_decode(&q->dl_sch, cfg, softbuffer, q->e, data);      
    
  } else {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }
}

int srslte_pdsch_encode(srslte_pdsch_t *q, 
                        srslte_pdsch_cfg_t *cfg, srslte_softbuffer_tx_t *softbuffer, 
                        uint8_t *data, cf_t *sf_symbols[SRSLTE_MAX_PORTS]) 
{
  if (q    != NULL &&
      data != NULL &&
      cfg  != NULL)
  {
    if (q->rnti_is_set) {
      return srslte_pdsch_encode_rnti(q, cfg, softbuffer, data, q->rnti, sf_symbols);
    } else {
      fprintf(stderr, "Must call srslte_pdsch_set_rnti() to set the encoder/decoder RNTI\n");       
      return SRSLTE_ERROR;
    }    
  } else {
    return SRSLTE_ERROR_INVALID_INPUTS; 
  }
}

int srslte_pdsch_encode_seq(srslte_pdsch_t *q, 
                             srslte_pdsch_cfg_t *cfg, srslte_softbuffer_tx_t *softbuffer,
                             uint8_t *data, srslte_sequence_t *seq, cf_t *sf_symbols[SRSLTE_MAX_PORTS]) 
{
  
  int i;
  /* Set pointers for layermapping & precoding */
  cf_t *x[SRSLTE_MAX_LAYERS];
  int ret = SRSLTE_ERROR_INVALID_INPUTS; 
   
   if (q             != NULL &&
       data          != NULL &&
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

    if (srslte_dlsch_encode(&q->dl_sch, cfg, softbuffer, data, q->e)) {
      fprintf(stderr, "Error encoding TB\n");
      return SRSLTE_ERROR;
    }

    srslte_scrambling_bytes(seq, (uint8_t*) q->e, cfg->nbits.nof_bits);

    srslte_mod_modulate_bytes(&q->mod[cfg->grant.mcs.mod], (uint8_t*) q->e, q->d, cfg->nbits.nof_bits);
    
    /* TODO: only diversity supported */
    if (q->cell.nof_ports > 1) {
      srslte_layermap_diversity(q->d, x, q->cell.nof_ports, cfg->nbits.nof_re);
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

int srslte_pdsch_encode_rnti_idx(srslte_pdsch_t *q, 
                                 srslte_pdsch_cfg_t *cfg, srslte_softbuffer_tx_t *softbuffer,
                                 uint8_t *data, uint32_t rnti_idx, cf_t *sf_symbols[SRSLTE_MAX_PORTS]) 
{
  if (rnti_idx < q->nof_crnti) {
    if (q->rnti_multi[rnti_idx]) {
      return srslte_pdsch_encode_seq(q, cfg, softbuffer, data, &q->seq_multi[cfg->sf_idx][rnti_idx], sf_symbols);
    } else {
      fprintf(stderr, "Error RNTI idx %d not set\n", rnti_idx);
      return SRSLTE_ERROR; 
    }
  } else {
    return SRSLTE_ERROR_INVALID_INPUTS; 
  }
}

/** Converts the PDSCH data bits to symbols mapped to the slot ready for transmission
 */
int srslte_pdsch_encode_rnti(srslte_pdsch_t *q, 
                             srslte_pdsch_cfg_t *cfg, srslte_softbuffer_tx_t *softbuffer,
                             uint8_t *data, uint16_t rnti, cf_t *sf_symbols[SRSLTE_MAX_PORTS]) 
{
  if (rnti != q->rnti) {
    srslte_sequence_t seq; 
    if (srslte_sequence_pdsch(&seq, rnti, 0, 2 * cfg->sf_idx, q->cell.id, cfg->nbits.nof_bits)) {
      return SRSLTE_ERROR; 
    }
    int r = srslte_pdsch_encode_seq(q, cfg, softbuffer, data, &seq, sf_symbols);
    srslte_sequence_free(&seq);
    return r; 
  } else {    
    return srslte_pdsch_encode_seq(q, cfg, softbuffer, data, &q->seq[cfg->sf_idx], sf_symbols);
  } 
}


float srslte_pdsch_average_noi(srslte_pdsch_t *q) 
{
  return q->dl_sch.average_nof_iterations;
}

uint32_t srslte_pdsch_last_noi(srslte_pdsch_t *q) {
  return q->dl_sch.nof_iterations;
}



  
