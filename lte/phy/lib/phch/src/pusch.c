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

#include "liblte/phy/phch/pusch.h"
#include "liblte/phy/phch/uci.h"
#include "liblte/phy/common/phy_common.h"
#include "liblte/phy/utils/bit.h"
#include "liblte/phy/utils/debug.h"
#include "liblte/phy/utils/vector.h"
#include "liblte/phy/filter/dft_precoding.h"

#define MAX_PUSCH_RE(cp) (2 * CP_NSYMB(cp) * 12)



const static lte_mod_t modulations[4] =
    { LTE_BPSK, LTE_QPSK, LTE_QAM16, LTE_QAM64 };
    
static int f_hop_sum(pusch_t *q, uint32_t i) {
  uint32_t sum = 0;
  for (uint32_t k=i*10+1;k<i*10+9;i++) {
    sum += (q->seq_type2_fo.c[k]<<(k-(i*10+1)));
  }
  return sum; 
}
    
static int f_hop(pusch_t *q, pusch_hopping_cfg_t *hopping, int i) {
  if (i == -1) {
    return 0; 
  } else {
    if (hopping->n_sb == 1) {
      return 0;
    } else if (hopping->n_sb == 2) {
      return (f_hop(q, hopping, i-1) + f_hop_sum(q, i))%2;
    } else {
      return (f_hop(q, hopping, i-1) + f_hop_sum(q, i)%(hopping->n_sb-1)+1)%hopping->n_sb;   
    }    
  }
}

static int f_m(pusch_t *q, pusch_hopping_cfg_t *hopping, uint32_t i) {
  if (hopping->n_sb == 1) {
    if (hopping->hop_mode == hop_mode_inter_sf) {
      return hopping->current_tx_nb%2;
    } else {
      return i%2;      
    }
  } else {
    return q->seq_type2_fo.c[i*10];
  }
}

int pusch_cp(pusch_t *q, harq_t *harq, cf_t *input, cf_t *output, bool advance_input) 
{
  cf_t *in_ptr = input; 
  cf_t *out_ptr = output; 
  pusch_hopping_cfg_t *hopping = &q->hopping_cfg; 
  
  uint32_t L_ref = 3;
  if (CP_ISEXT(q->cell.cp)) {
    L_ref = 2; 
  }
  INFO("PUSCH Freq hopping: %d\n", harq->ul_alloc.freq_hopping);
  for (uint32_t slot=0;slot<2;slot++) {
    uint32_t n_prb_tilde = harq->ul_alloc.n_prb[slot]; 
    if (harq->ul_alloc.freq_hopping == 1) {
      if (hopping->hop_mode == hop_mode_inter_sf) {
        n_prb_tilde = harq->ul_alloc.n_prb[hopping->current_tx_nb%2];      
      } else {
        n_prb_tilde = harq->ul_alloc.n_prb[slot];
      }
    }
    if (harq->ul_alloc.freq_hopping == 2) {
      /* Freq hopping type 2 as defined in 5.3.4 of 36.211 */
      uint32_t n_vrb_tilde = harq->ul_alloc.n_prb[0];
      if (hopping->n_sb > 1) {
        n_vrb_tilde -= (hopping->hopping_offset-1)/2+1;
      }
      int i=0;
      if (hopping->hop_mode == hop_mode_inter_sf) {
        i = harq->sf_idx;
      } else {
        i = 2*harq->sf_idx+slot;
      }
      uint32_t n_rb_sb = q->cell.nof_prb;
      if (hopping->n_sb > 1) {
        n_rb_sb = (n_rb_sb-hopping->hopping_offset-hopping->hopping_offset%2)/hopping->n_sb;
      }
      n_prb_tilde = (n_vrb_tilde+f_hop(q, hopping, i)*n_rb_sb+
        (n_rb_sb-1)-2*(n_vrb_tilde%n_rb_sb)*f_m(q, hopping, i))%(n_rb_sb*hopping->n_sb);
      
      INFO("n_prb_tilde: %d, n_vrb_tilde: %d, n_rb_sb: %d, n_sb: %d\n", n_prb_tilde, n_vrb_tilde, n_rb_sb, hopping->n_sb);
      if (hopping->n_sb > 1) {
        n_prb_tilde += (hopping->hopping_offset-1)/2+1;
      }
      
    }
    harq->ul_alloc.n_prb_tilde[slot] = n_prb_tilde; 
    INFO("Allocating PUSCH %d PRB to index %d at slot %d\n",harq->ul_alloc.L_prb, n_prb_tilde,slot);
    for (uint32_t l=0;l<CP_NSYMB(q->cell.cp);l++) {
      if (l != L_ref) {
        uint32_t idx = RE_IDX(q->cell.nof_prb, l+slot*CP_NSYMB(q->cell.cp), 
                              n_prb_tilde*RE_X_RB);
        if (advance_input) {
          out_ptr = &output[idx]; 
        } else {
          in_ptr = &input[idx];
        }              
        memcpy(out_ptr, in_ptr, harq->ul_alloc.L_prb * RE_X_RB * sizeof(cf_t));                       
        if (advance_input) {
          in_ptr += harq->ul_alloc.L_prb*RE_X_RB;
        } else {
          out_ptr += harq->ul_alloc.L_prb*RE_X_RB; 
        }
      }
    }        
  }
  return RE_X_RB*harq->ul_alloc.L_prb; 
}

int pusch_put(pusch_t *q, harq_t *harq, cf_t *input, cf_t *output) {
  return pusch_cp(q, harq, input, output, true);
}

int pusch_get(pusch_t *q, harq_t *harq, cf_t *input, cf_t *output) {
  return pusch_cp(q, harq, input, output, false);
}


/** Initializes the PDCCH transmitter and receiver */
int pusch_init(pusch_t *q, lte_cell_t cell) {
  int ret = LIBLTE_ERROR_INVALID_INPUTS;
  int i;

 if (q                         != NULL                  &&
     lte_cell_isvalid(&cell)) 
  {   
    
    bzero(q, sizeof(pusch_t));
    ret = LIBLTE_ERROR;
    
    q->cell = cell;
    q->max_re = q->cell.nof_prb * MAX_PUSCH_RE(q->cell.cp);

    INFO("Init PUSCH: %d ports %d PRBs, max_symbols: %d\n", q->cell.nof_ports,
        q->cell.nof_prb, q->max_re);

    for (i = 0; i < 4; i++) {
      if (modem_table_lte(&q->mod[i], modulations[i], true)) {
        goto clean;
      }
    }
    
    /* Precompute sequence for type2 frequency hopping */
    if (sequence_LTE_pr(&q->seq_type2_fo, 210, q->cell.id)) {
      fprintf(stderr, "Error initiating type2 frequency hopping sequence\n");
      goto clean; 
    }

    demod_soft_init(&q->demod, q->max_re);
    demod_soft_alg_set(&q->demod, APPROX);
    
    sch_init(&q->dl_sch);
    
    if (dft_precoding_init(&q->dft_precoding, cell.nof_prb)) {
      fprintf(stderr, "Error initiating DFT transform precoding\n");
      goto clean; 
    }
    
    /* This is for equalization at receiver */
    if (precoding_init(&q->equalizer, SF_LEN_RE(cell.nof_prb, cell.cp))) {
      fprintf(stderr, "Error initializing precoding\n");
      goto clean; 
    }

    q->rnti_is_set = false; 

    // Allocate floats for reception (LLRs). Buffer casted to uint8_t for transmission
    q->pusch_q = vec_malloc(sizeof(float) * q->max_re * lte_mod_bits_x_symbol(LTE_QAM64));
    if (!q->pusch_q) {
      goto clean;
    }

    // Allocate floats for reception (LLRs). Buffer casted to uint8_t for transmission
    q->pusch_g = vec_malloc(sizeof(float) * q->max_re * lte_mod_bits_x_symbol(LTE_QAM64));
    if (!q->pusch_g) {
      goto clean;
    }
    q->pusch_d = vec_malloc(sizeof(cf_t) * q->max_re);
    if (!q->pusch_d) {
      goto clean;
    }

    q->ce = vec_malloc(sizeof(cf_t) * q->max_re);
    if (!q->ce) {
      goto clean;
    }
    q->pusch_z = vec_malloc(sizeof(cf_t) * q->max_re);
    if (!q->pusch_z) {
      goto clean;
    }

    ret = LIBLTE_SUCCESS;
  }
  clean: 
  if (ret == LIBLTE_ERROR) {
    pusch_free(q);
  }
  return ret;
}

void pusch_free(pusch_t *q) {
  int i;

  if (q->pusch_q) {
    free(q->pusch_q);
  }
  if (q->pusch_d) {
    free(q->pusch_d);
  }
  if (q->pusch_g) {
    free(q->pusch_g);
  }
  if (q->ce) {
    free(q->ce);
  }
  if (q->pusch_z) {
    free(q->pusch_z);
  }
  
  dft_precoding_free(&q->dft_precoding);

  precoding_free(&q->equalizer);
  
  for (i = 0; i < NSUBFRAMES_X_FRAME; i++) {
    sequence_free(&q->seq_pusch[i]);
  }

  for (i = 0; i < 4; i++) {
    modem_table_free(&q->mod[i]);
  }
  demod_soft_free(&q->demod);
  sch_free(&q->dl_sch);

  bzero(q, sizeof(pusch_t));

}

void pusch_set_hopping_cfg(pusch_t *q, pusch_hopping_cfg_t *cfg)
{
  memcpy(&q->hopping_cfg, cfg, sizeof(pusch_hopping_cfg_t));
}

/* Precalculate the PUSCH scramble sequences for a given RNTI. This function takes a while 
 * to execute, so shall be called once the final C-RNTI has been allocated for the session.
 * For the connection procedure, use pusch_encode_rnti() or pusch_decode_rnti() functions */
int pusch_set_rnti(pusch_t *q, uint16_t rnti) {
  uint32_t i;

  for (i = 0; i < NSUBFRAMES_X_FRAME; i++) {
    if (sequence_pusch(&q->seq_pusch[i], rnti, 2 * i, q->cell.id,
        q->max_re * lte_mod_bits_x_symbol(LTE_QAM64))) {
      return LIBLTE_ERROR; 
    }
  }
  q->rnti_is_set = true; 
  q->rnti = rnti; 
  return LIBLTE_SUCCESS;
}

/** Decodes the PUSCH from the received symbols
 */
int pusch_decode(pusch_t *q, harq_t *harq, cf_t *sf_symbols, cf_t *ce, float noise_estimate, uint8_t *data) 
{

  uint32_t n;
  
  if (q                     != NULL &&
      sf_symbols            != NULL &&
      data                  != NULL &&
       harq          != NULL)
  {
    
    if (q->rnti_is_set) {
      INFO("Decoding PUSCH SF: %d, Mod %s, NofBits: %d, NofSymbols: %d, NofBitsE: %d, rv_idx: %d\n",
          harq->sf_idx, lte_mod_string(harq->mcs.mod), harq->mcs.tbs, harq->nof_re, harq->nof_bits, harq->rv);

      /* extract symbols */
      n = pusch_get(q, harq, sf_symbols, q->pusch_d);
      if (n != harq->nof_re) {
        fprintf(stderr, "Error expecting %d symbols but got %d\n", harq->nof_re, n);
        return LIBLTE_ERROR;
      }
      
      /* extract channel estimates */
      n = pusch_get(q, harq, ce, q->ce);
      if (n != harq->nof_re) {
        fprintf(stderr, "Error expecting %d symbols but got %d\n", harq->nof_re, n);
        return LIBLTE_ERROR;
      }
      
      predecoding_single(&q->equalizer, q->pusch_d, q->ce, q->pusch_z,
            harq->nof_re, noise_estimate);

      dft_predecoding(&q->dft_precoding, q->pusch_z, q->pusch_d, 
                      harq->ul_alloc.L_prb, harq->nof_symb);
      
      /* demodulate symbols 
      * The MAX-log-MAP algorithm used in turbo decoding is unsensitive to SNR estimation, 
      * thus we don't need tot set it in the LLRs normalization
      */
      demod_soft_sigma_set(&q->demod, sqrt(0.5));
      demod_soft_table_set(&q->demod, &q->mod[harq->mcs.mod]);
      demod_soft_demodulate(&q->demod, q->pusch_d, q->pusch_q, harq->nof_re);

      /* descramble */
      scrambling_f_offset(&q->seq_pusch[harq->sf_idx], q->pusch_q, 0, harq->nof_bits);

      return ulsch_decode(&q->dl_sch, harq, q->pusch_q, data);      
    } else {
      fprintf(stderr, "Must call pusch_set_rnti() before calling pusch_decode()\n");
      return LIBLTE_ERROR; 
    }    
  } else {
    return LIBLTE_ERROR_INVALID_INPUTS;
  }
}

int pusch_encode_rnti(pusch_t *q, harq_t *harq_process, uint8_t *data, uint16_t rnti, cf_t *sf_symbols) 
{
  uci_data_t uci_data; 
  bzero(&uci_data, sizeof(uci_data_t));
  return pusch_uci_encode_rnti(q, harq_process, data, uci_data, rnti, sf_symbols);
}

int pusch_encode(pusch_t *q, harq_t *harq_process, uint8_t *data, cf_t *sf_symbols) 
{
  if (q->rnti_is_set) {
    uci_data_t uci_data; 
    bzero(&uci_data, sizeof(uci_data_t));
    return pusch_uci_encode_rnti(q, harq_process, data, uci_data, q->rnti, sf_symbols);    
  } else {
    fprintf(stderr, "Must call pusch_set_rnti() to set the encoder/decoder RNTI\n");       
    return LIBLTE_ERROR;     
  }
}

int pusch_uci_encode(pusch_t *q, harq_t *harq, uint8_t *data, uci_data_t uci_data, cf_t *sf_symbols) 
{
  if (q->rnti_is_set) {
    return pusch_uci_encode_rnti(q, harq, data, uci_data, q->rnti, sf_symbols);
  } else {
    fprintf(stderr, "Must call pusch_set_rnti() to set the encoder/decoder RNTI\n");       
    return LIBLTE_ERROR; 
  }
}

/** Converts the PUSCH data bits to symbols mapped to the slot ready for transmission
 */
int pusch_uci_encode_rnti(pusch_t *q, harq_t *harq, uint8_t *data, uci_data_t uci_data, uint16_t rnti, cf_t *sf_symbols) 
{
  int ret = LIBLTE_ERROR_INVALID_INPUTS; 
   
  if (q    != NULL &&
      data != NULL &&
      harq != NULL)
  {
    if (harq->mcs.tbs > harq->nof_bits) {
      fprintf(stderr, "Invalid code rate %.2f\n", (float) harq->mcs.tbs / harq->nof_bits);
      return LIBLTE_ERROR_INVALID_INPUTS;
    }

    if (harq->nof_re > q->max_re) {
      fprintf(stderr, "Error too many RE per subframe (%d). PUSCH configured for %d RE (%d PRB)\n",
          harq->nof_re, q->max_re, q->cell.nof_prb);
      return LIBLTE_ERROR_INVALID_INPUTS;
    }

    INFO("Encoding PUSCH SF: %d, Mod %s, TBS: %d, NofSymbols: %d, NofBitsE: %d, rv_idx: %d\n",
        harq->sf_idx, lte_mod_string(harq->mcs.mod), harq->mcs.tbs, harq->nof_re, harq->nof_bits, harq->rv);
    
    bzero(q->pusch_q, harq->nof_bits);
    if (ulsch_uci_encode(&q->dl_sch, harq, data, uci_data, q->pusch_g, q->pusch_q)) {
      fprintf(stderr, "Error encoding TB\n");
      return LIBLTE_ERROR;
    }
    
    if (rnti != q->rnti) {
      sequence_t seq; 
      if (sequence_pusch(&seq, rnti, 2 * harq->sf_idx, q->cell.id, harq->nof_bits)) {
        return LIBLTE_ERROR; 
      }
      scrambling_b_offset_pusch(&seq, (uint8_t*) q->pusch_q, 0, harq->nof_bits);      
      sequence_free(&seq);
    } else {
      scrambling_b_offset_pusch(&q->seq_pusch[harq->sf_idx], (uint8_t*) q->pusch_q, 0, harq->nof_bits);            
    }
    
    mod_modulate(&q->mod[harq->mcs.mod], (uint8_t*) q->pusch_q, q->pusch_d, harq->nof_bits);
    
    dft_precoding(&q->dft_precoding, q->pusch_d, q->pusch_z, 
                  harq->ul_alloc.L_prb, harq->nof_symb);
    
    /* mapping to resource elements */      
    pusch_put(q, harq, q->pusch_z, sf_symbols);
    
    ret = LIBLTE_SUCCESS;
  } 
  return ret; 
}
  