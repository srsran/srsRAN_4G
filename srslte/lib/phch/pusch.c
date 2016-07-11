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

#include "srslte/ch_estimation/refsignal_ul.h"
#include "srslte/phch/pusch.h"
#include "srslte/phch/pusch_cfg.h"
#include "srslte/phch/uci.h"
#include "srslte/common/phy_common.h"
#include "srslte/utils/bit.h"
#include "srslte/utils/debug.h"
#include "srslte/utils/vector.h"
#include "srslte/dft/dft_precoding.h"

#define MAX_PUSCH_RE(cp) (2 * SRSLTE_CP_NSYMB(cp) * 12)



const static srslte_mod_t modulations[4] =
    { SRSLTE_MOD_BPSK, SRSLTE_MOD_QPSK, SRSLTE_MOD_16QAM, SRSLTE_MOD_64QAM };
    
static int f_hop_sum(srslte_pusch_t *q, uint32_t i) {
  uint32_t sum = 0;
  for (uint32_t k=i*10+1;k<i*10+9;i++) {
    sum += (q->seq_type2_fo.c[k]<<(k-(i*10+1)));
  }
  return sum; 
}
    
static int f_hop(srslte_pusch_t *q, srslte_pusch_hopping_cfg_t *hopping, int i) {
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

static int f_m(srslte_pusch_t *q, srslte_pusch_hopping_cfg_t *hopping, uint32_t i, uint32_t current_tx_nb) {
  if (hopping->n_sb == 1) {
    if (hopping->hop_mode == SRSLTE_PUSCH_HOP_MODE_INTER_SF) {
      return current_tx_nb%2;
    } else {
      return i%2;      
    }
  } else {
    return q->seq_type2_fo.c[i*10];
  }
}

/* Computes PUSCH frequency hopping as defined in Section 8.4 of 36.213 */
void compute_freq_hopping(srslte_pusch_t *q, srslte_ra_ul_grant_t *grant, 
                               srslte_pusch_hopping_cfg_t *hopping, 
                               uint32_t sf_idx, uint32_t current_tx_nb) 
{
  
  for (uint32_t slot=0;slot<2;slot++) {    

    INFO("PUSCH Freq hopping: %d\n", grant->freq_hopping);
    uint32_t n_prb_tilde = grant->n_prb[slot]; 
    
    if (grant->freq_hopping == 1) {
      if (hopping->hop_mode == SRSLTE_PUSCH_HOP_MODE_INTER_SF) {
        n_prb_tilde = grant->n_prb[current_tx_nb%2];      
      } else {
        n_prb_tilde = grant->n_prb[slot];
      }
    }
    if (grant->freq_hopping == 2) {
      /* Freq hopping type 2 as defined in 5.3.4 of 36.211 */
      uint32_t n_vrb_tilde = grant->n_prb[0];
      if (hopping->n_sb > 1) {
        n_vrb_tilde -= (hopping->hopping_offset-1)/2+1;
      }
      int i=0;
      if (hopping->hop_mode == SRSLTE_PUSCH_HOP_MODE_INTER_SF) {
        i = sf_idx;
      } else {
        i = 2*sf_idx+slot;
      }
      uint32_t n_rb_sb = q->cell.nof_prb;
      if (hopping->n_sb > 1) {
        n_rb_sb = (n_rb_sb-hopping->hopping_offset-hopping->hopping_offset%2)/hopping->n_sb;
      }
      n_prb_tilde = (n_vrb_tilde+f_hop(q, hopping, i)*n_rb_sb+
        (n_rb_sb-1)-2*(n_vrb_tilde%n_rb_sb)*f_m(q, hopping, i, current_tx_nb))%(n_rb_sb*hopping->n_sb);
      
      INFO("n_prb_tilde: %d, n_vrb_tilde: %d, n_rb_sb: %d, n_sb: %d\n", 
           n_prb_tilde, n_vrb_tilde, n_rb_sb, hopping->n_sb);
      if (hopping->n_sb > 1) {
        n_prb_tilde += (hopping->hopping_offset-1)/2+1;
      }
      
    }
    grant->n_prb_tilde[slot] = n_prb_tilde; 
  }
}


/* Allocate/deallocate PUSCH RBs to the resource grid
 */
int pusch_cp(srslte_pusch_t *q, srslte_ra_ul_grant_t *grant, cf_t *input, cf_t *output, bool advance_input) 
{
  cf_t *in_ptr = input; 
  cf_t *out_ptr = output; 
  
  uint32_t L_ref = 3;
  if (SRSLTE_CP_ISEXT(q->cell.cp)) {
    L_ref = 2; 
  }
  for (uint32_t slot=0;slot<2;slot++) {        
    uint32_t N_srs = 0; 
    if (q->shortened && slot == 1) {
      N_srs = 1; 
    }
    INFO("%s PUSCH %d PRB to index %d at slot %d\n",advance_input?"Allocating":"Getting",grant->L_prb, grant->n_prb_tilde[slot], slot);
    for (uint32_t l=0;l<SRSLTE_CP_NSYMB(q->cell.cp)-N_srs;l++) {
      if (l != L_ref) {
        uint32_t idx = SRSLTE_RE_IDX(q->cell.nof_prb, l+slot*SRSLTE_CP_NSYMB(q->cell.cp), 
                              grant->n_prb_tilde[slot]*SRSLTE_NRE);
        if (advance_input) {
          out_ptr = &output[idx]; 
        } else {
          in_ptr = &input[idx];
        }              
        memcpy(out_ptr, in_ptr, grant->L_prb * SRSLTE_NRE * sizeof(cf_t));                       
        if (advance_input) {
          in_ptr += grant->L_prb*SRSLTE_NRE;
        } else {
          out_ptr += grant->L_prb*SRSLTE_NRE; 
        }
      }
    }        
  }
  if (advance_input) {
    return in_ptr - input;
  } else {
    return out_ptr - output; 
  }
}

int pusch_put(srslte_pusch_t *q, srslte_ra_ul_grant_t *grant, cf_t *input, cf_t *output) {
  return pusch_cp(q, grant, input, output, true);
}

int pusch_get(srslte_pusch_t *q, srslte_ra_ul_grant_t *grant, cf_t *input, cf_t *output) {
  return pusch_cp(q, grant, input, output, false);
}


/** Initializes the PDCCH transmitter and receiver */
int srslte_pusch_init(srslte_pusch_t *q, srslte_cell_t cell) {
  int ret = SRSLTE_ERROR_INVALID_INPUTS;
  int i;

 if (q                         != NULL                  &&
     srslte_cell_isvalid(&cell)) 
  {   
    
    bzero(q, sizeof(srslte_pusch_t));
    ret = SRSLTE_ERROR;
    
    q->cell = cell;
    q->max_re = q->cell.nof_prb * MAX_PUSCH_RE(q->cell.cp);

    INFO("Init PUSCH: %d ports %d PRBs, max_symbols: %d\n", q->cell.nof_ports,
        q->cell.nof_prb, q->max_re);

    for (i = 0; i < 4; i++) {
      if (srslte_modem_table_lte(&q->mod[i], modulations[i])) {
        goto clean;
      }
      srslte_modem_table_bytes(&q->mod[i]);
    }
    
    /* Precompute sequence for type2 frequency hopping */
    if (srslte_sequence_LTE_pr(&q->seq_type2_fo, 210, q->cell.id)) {
      fprintf(stderr, "Error initiating type2 frequency hopping sequence\n");
      goto clean; 
    }

    srslte_sch_init(&q->ul_sch);
    
    if (srslte_dft_precoding_init(&q->dft_precoding, cell.nof_prb)) {
      fprintf(stderr, "Error initiating DFT transform precoding\n");
      goto clean; 
    }
    
    q->rnti_is_set = false; 

    // Allocate int16 for reception (LLRs). Buffer casted to uint8_t for transmission
    q->q = srslte_vec_malloc(sizeof(int16_t) * q->max_re * srslte_mod_bits_x_symbol(SRSLTE_MOD_64QAM));
    if (!q->q) {
      goto clean;
    }

    // Allocate int16 for reception (LLRs). Buffer casted to uint8_t for transmission
    q->g = srslte_vec_malloc(sizeof(int16_t) * q->max_re * srslte_mod_bits_x_symbol(SRSLTE_MOD_64QAM));
    if (!q->g) {
      goto clean;
    }
    q->d = srslte_vec_malloc(sizeof(cf_t) * q->max_re);
    if (!q->d) {
      goto clean; 
    }

    q->ce = srslte_vec_malloc(sizeof(cf_t) * q->max_re);
    if (!q->ce) {
      goto clean;
    }
    q->z = srslte_vec_malloc(sizeof(cf_t) * q->max_re);
    if (!q->z) {
      goto clean;
    }

    ret = SRSLTE_SUCCESS;
  }
  clean: 
  if (ret == SRSLTE_ERROR) {
    srslte_pusch_free(q);
  }
  return ret;
}

void srslte_pusch_free(srslte_pusch_t *q) {
  int i;

  if (q->q) {
    free(q->q);
  }
  if (q->d) {
    free(q->d);
  }
  if (q->g) {
    free(q->g);
  }
  if (q->ce) {
    free(q->ce);
  }
  if (q->z) {
    free(q->z);
  }
  
  srslte_dft_precoding_free(&q->dft_precoding);

  for (i = 0; i < SRSLTE_NSUBFRAMES_X_FRAME; i++) {
    srslte_sequence_free(&q->seq[i]);
  }

  srslte_sequence_free(&q->seq_type2_fo);
  
  for (i = 0; i < 4; i++) {
    srslte_modem_table_free(&q->mod[i]);
  }
  srslte_sch_free(&q->ul_sch);

  bzero(q, sizeof(srslte_pusch_t));

}

/* Configures the structure srslte_pusch_cfg_t from the UL DCI allocation dci_msg. 
 * If dci_msg is NULL, the grant is assumed to be already stored in cfg->grant
 */
int srslte_pusch_cfg(srslte_pusch_t             *q, 
                     srslte_pusch_cfg_t         *cfg, 
                     srslte_ra_ul_grant_t       *grant, 
                     srslte_uci_cfg_t           *uci_cfg, 
                     srslte_pusch_hopping_cfg_t *hopping_cfg, 
                     srslte_refsignal_srs_cfg_t *srs_cfg, 
                     uint32_t tti, 
                     uint32_t rv_idx, 
                     uint32_t current_tx_nb) 
{
  if (q && cfg && grant) {
    memcpy(&cfg->grant, grant, sizeof(srslte_ra_ul_grant_t));
    
    if (srslte_cbsegm(&cfg->cb_segm, cfg->grant.mcs.tbs)) {
      fprintf(stderr, "Error computing Codeblock segmentation for TBS=%d\n", cfg->grant.mcs.tbs);
      return SRSLTE_ERROR; 
    }
    
    /* Compute PUSCH frequency hopping */
    if (hopping_cfg) {
      compute_freq_hopping(q, &cfg->grant, hopping_cfg, tti%10, current_tx_nb);
    } else {
      cfg->grant.n_prb_tilde[0] = cfg->grant.n_prb[0];
      cfg->grant.n_prb_tilde[1] = cfg->grant.n_prb[1];
    }
    if (srs_cfg) {
      q->shortened = false; 
      if (srs_cfg->configured) {
        // If UE-specific SRS is configured, PUSCH is shortened every time UE transmits SRS even if overlaping in the same RB or not
        if (srslte_refsignal_srs_send_cs(srs_cfg->subframe_config, tti%10) == 1 && 
            srslte_refsignal_srs_send_ue(srs_cfg->I_srs, tti) == 1)
        {
          q->shortened = true; 
          /* If RBs are contiguous, PUSCH is not shortened */
          uint32_t k0_srs = srslte_refsignal_srs_rb_start_cs(srs_cfg->bw_cfg, q->cell.nof_prb);
          uint32_t nrb_srs = srslte_refsignal_srs_rb_L_cs(srs_cfg->bw_cfg, q->cell.nof_prb);
          for (uint32_t ns=0;ns<2 && q->shortened;ns++) {
            if (cfg->grant.n_prb_tilde[ns] == k0_srs + nrb_srs ||         // If PUSCH is contiguous on the right-hand side of SRS
                cfg->grant.n_prb_tilde[ns] + cfg->grant.L_prb == k0_srs)  // If SRS is contiguous on the left-hand side of PUSCH
            {
              q->shortened = false; 
            }
          }
        }
        // If not coincides with UE transmission. PUSCH shall be shortened if cell-specific SRS transmission RB 
        //coincides with PUSCH allocated RB
        if (!q->shortened) {
          if (srslte_refsignal_srs_send_cs(srs_cfg->subframe_config, tti%10) == 1) {
            uint32_t k0_srs = srslte_refsignal_srs_rb_start_cs(srs_cfg->bw_cfg, q->cell.nof_prb);
            uint32_t nrb_srs = srslte_refsignal_srs_rb_L_cs(srs_cfg->bw_cfg, q->cell.nof_prb);
            for (uint32_t ns=0;ns<2 && !q->shortened;ns++) {
              if ((cfg->grant.n_prb_tilde[ns] >= k0_srs && cfg->grant.n_prb_tilde[ns] < k0_srs + nrb_srs) || 
                  (cfg->grant.n_prb_tilde[ns] + cfg->grant.L_prb >= k0_srs && 
                        cfg->grant.n_prb_tilde[ns] + cfg->grant.L_prb < k0_srs + nrb_srs) ||
                  (cfg->grant.n_prb_tilde[ns] <= k0_srs && cfg->grant.n_prb_tilde[ns] + cfg->grant.L_prb >= k0_srs + nrb_srs))
              {            
                q->shortened = true; 
              }
            }
          }
        }
      }    
    } 
    
    /* Compute final number of bits and RE */
    srslte_ra_ul_grant_to_nbits(&cfg->grant, q->cell.cp, q->shortened?1:0, &cfg->nbits);

    cfg->sf_idx = tti%10; 
    cfg->tti    = tti; 
    cfg->rv     = rv_idx; 
    cfg->cp     = q->cell.cp; 
    
    // Save UCI configuration 
    if (uci_cfg) {
      memcpy(&cfg->uci_cfg, uci_cfg, sizeof(srslte_uci_cfg_t));
    }
    
    return SRSLTE_SUCCESS;      
  } else {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }
}

/* Precalculate the PUSCH scramble sequences for a given RNTI. This function takes a while 
 * to execute, so shall be called once the final C-RNTI has been allocated for the session.
 * For the connection procedure, use srslte_pusch_encode_rnti() or srslte_pusch_decode_rnti() functions */
int srslte_pusch_set_rnti(srslte_pusch_t *q, uint16_t rnti) {
  uint32_t i;

  for (i = 0; i < SRSLTE_NSUBFRAMES_X_FRAME; i++) {
    if (srslte_sequence_pusch(&q->seq[i], rnti, 2 * i, q->cell.id,
        q->max_re * srslte_mod_bits_x_symbol(SRSLTE_MOD_64QAM))) {
      return SRSLTE_ERROR; 
    }
  }
  q->rnti_is_set = true; 
  q->rnti = rnti; 
  return SRSLTE_SUCCESS;
}


/* Initializes the memory to support pre-calculation of multiple scrambling sequences */
int srslte_pusch_init_rnti_multi(srslte_pusch_t *q, uint32_t nof_rntis)
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

int srslte_pusch_set_rnti_multi(srslte_pusch_t *q, uint32_t idx, uint16_t rnti)
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
    for (int i = 0; i < SRSLTE_NSUBFRAMES_X_FRAME; i++) {
      if (srslte_sequence_pusch(&q->seq_multi[i][idx], rnti, 2 * i, q->cell.id,
          q->max_re * srslte_mod_bits_x_symbol(SRSLTE_MOD_64QAM))) {
        return SRSLTE_ERROR; 
      }
    }
    return SRSLTE_SUCCESS;
  } else {
    return SRSLTE_ERROR_INVALID_INPUTS; 
  }
}

uint16_t srslte_pusch_get_rnti_multi(srslte_pusch_t *q, uint32_t idx)
{
  if (idx < q->nof_crnti) {
    return q->rnti_multi[idx];
  } else {
    return SRSLTE_ERROR_INVALID_INPUTS; 
  }
}

int srslte_pusch_encode_rnti(srslte_pusch_t *q, srslte_pusch_cfg_t *cfg, srslte_softbuffer_tx_t *softbuffer,
                             uint8_t *data, uint16_t rnti, 
                             cf_t *sf_symbols) 
{
  srslte_uci_data_t uci_data; 
  bzero(&uci_data, sizeof(srslte_uci_data_t));
  return srslte_pusch_uci_encode_rnti(q, cfg, softbuffer, data, uci_data, rnti, sf_symbols);
}

int srslte_pusch_encode(srslte_pusch_t *q, srslte_pusch_cfg_t *cfg, srslte_softbuffer_tx_t *softbuffer,
                        uint8_t *data, cf_t *sf_symbols) 
{
  if (q->rnti_is_set) {
    srslte_uci_data_t uci_data; 
    bzero(&uci_data, sizeof(srslte_uci_data_t));
    return srslte_pusch_uci_encode_rnti(q, cfg, softbuffer, data, uci_data, q->rnti, sf_symbols);    
  } else {
    fprintf(stderr, "Must call srslte_pusch_set_rnti() to set the encoder/decoder RNTI\n");       
    return SRSLTE_ERROR;     
  }
}

int srslte_pusch_uci_encode(srslte_pusch_t *q, srslte_pusch_cfg_t *cfg, srslte_softbuffer_tx_t *softbuffer,
                            uint8_t *data, srslte_uci_data_t uci_data, 
                            cf_t *sf_symbols) 
{
  if (q->rnti_is_set) {
    return srslte_pusch_uci_encode_rnti(q, cfg, softbuffer, data, uci_data, q->rnti, sf_symbols);
  } else {
    fprintf(stderr, "Must call srslte_pusch_set_rnti() to set the encoder/decoder RNTI\n");       
    return SRSLTE_ERROR; 
  }
}

/** Converts the PUSCH data bits to symbols mapped to the slot ready for transmission
 */
int srslte_pusch_uci_encode_rnti(srslte_pusch_t *q, srslte_pusch_cfg_t *cfg, srslte_softbuffer_tx_t *softbuffer,
                                 uint8_t *data, srslte_uci_data_t uci_data, uint16_t rnti, 
                                 cf_t *sf_symbols) 
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS; 
   
  if (q    != NULL &&
      cfg  != NULL)
  {

    if (cfg->nbits.nof_re > q->max_re) {
      fprintf(stderr, "Error too many RE per subframe (%d). PUSCH configured for %d RE (%d PRB)\n",
          cfg->nbits.nof_re, q->max_re, q->cell.nof_prb);
      return SRSLTE_ERROR_INVALID_INPUTS;
    }

    INFO("Encoding PUSCH SF: %d, Mod %s, RNTI: %d, TBS: %d, NofRE: %d, NofSymbols=%d, NofBitsE: %d, rv_idx: %d\n",
         cfg->sf_idx, srslte_mod_string(cfg->grant.mcs.mod), rnti, 
         cfg->grant.mcs.tbs, cfg->nbits.nof_re, cfg->nbits.nof_symb, cfg->nbits.nof_bits, cfg->rv);
    
    bzero(q->q, cfg->nbits.nof_bits);
    if (srslte_ulsch_uci_encode(&q->ul_sch, cfg, softbuffer, data, uci_data, q->g, q->q)) {
      fprintf(stderr, "Error encoding TB\n");
      return SRSLTE_ERROR;
    }

    if (rnti != q->rnti || !q->rnti_is_set) {
      srslte_sequence_t seq; 
      if (srslte_sequence_pusch(&seq, rnti, 2 * cfg->sf_idx, q->cell.id, cfg->nbits.nof_bits)) {
        return SRSLTE_ERROR; 
      }
      srslte_scrambling_bytes(&seq, (uint8_t*) q->q, cfg->nbits.nof_bits);      
      srslte_sequence_free(&seq);
    } else {
      srslte_scrambling_bytes(&q->seq[cfg->sf_idx], (uint8_t*) q->q, cfg->nbits.nof_bits);            
    }
    
    // Correct UCI placeholder/repetition bits    
    uint8_t *d = q->q; 
    for (int i = 0; i < q->ul_sch.nof_ri_ack_bits; i++) {     
      if (q->ul_sch.ack_ri_bits[i].type == UCI_BIT_PLACEHOLDER) {
        d[q->ul_sch.ack_ri_bits[i].position/8] |= (1<<(7-q->ul_sch.ack_ri_bits[i].position%8)); 
      } else if (q->ul_sch.ack_ri_bits[i].type == UCI_BIT_REPETITION) {
        if (q->ul_sch.ack_ri_bits[i].position > 1) {
          uint32_t p=q->ul_sch.ack_ri_bits[i].position;
          uint8_t bit = d[(p-1)/8] & (1<<(7-(p-1)%8)); 
          if (bit) {
            d[p/8] |= 1<<(7-p%8);
          } else {
            d[p/8] &= ~(1<<(7-p%8));
          }
        } 
      }
    }
    
    // Bit mapping
    srslte_mod_modulate_bytes(&q->mod[cfg->grant.mcs.mod], (uint8_t*) q->q, q->d, cfg->nbits.nof_bits);

    // DFT precoding
    srslte_dft_precoding(&q->dft_precoding, q->d, q->z, cfg->grant.L_prb, cfg->nbits.nof_symb);
    
    // Mapping to resource elements
    pusch_put(q, &cfg->grant, q->z, sf_symbols);
    
    ret = SRSLTE_SUCCESS;
  } 
  return ret; 
}

int srslte_pusch_decode(srslte_pusch_t *q, 
                        srslte_pusch_cfg_t *cfg, srslte_softbuffer_rx_t *softbuffer, 
                        cf_t *sf_symbols, 
                        cf_t *ce, float noise_estimate, 
                        uint8_t *data) 
{
  srslte_uci_data_t uci_data; 
  bzero(&uci_data, sizeof(srslte_uci_data_t));
  return srslte_pusch_uci_decode(q, cfg, softbuffer, sf_symbols, ce, noise_estimate, data, &uci_data);
}

int srslte_pusch_uci_decode_seq(srslte_pusch_t *q, 
                                srslte_pusch_cfg_t *cfg, srslte_softbuffer_rx_t *softbuffer, 
                                srslte_sequence_t *seq, 
                                cf_t *sf_symbols, 
                                cf_t *ce, float noise_estimate, 
                                uint8_t *data, srslte_uci_data_t *uci_data) 
{

  uint32_t n;
  
  if (q           != NULL &&
      sf_symbols  != NULL &&
      data        != NULL &&
      cfg         != NULL)
  {
    
    if (q->rnti_is_set) {
      INFO("Decoding PUSCH SF: %d, Mod %s, NofBits: %d, NofRE: %d, NofSymbols=%d, NofBitsE: %d, rv_idx: %d\n",
          cfg->sf_idx, srslte_mod_string(cfg->grant.mcs.mod), cfg->grant.mcs.tbs, 
           cfg->nbits.nof_re, cfg->nbits.nof_symb, cfg->nbits.nof_bits, cfg->rv);

      /* extract symbols */
      n = pusch_get(q, &cfg->grant, sf_symbols, q->d);
      if (n != cfg->nbits.nof_re) {
        fprintf(stderr, "Error expecting %d symbols but got %d\n", cfg->nbits.nof_re, n);
        return SRSLTE_ERROR;
      }
      
      /* extract channel estimates */
      n = pusch_get(q, &cfg->grant, ce, q->ce);
      if (n != cfg->nbits.nof_re) {
        fprintf(stderr, "Error expecting %d symbols but got %d\n", cfg->nbits.nof_re, n);
        return SRSLTE_ERROR;
      }

      // Equalization
      srslte_predecoding_single(q->d, q->ce, q->z, cfg->nbits.nof_re, noise_estimate);
      
      // DFT predecoding
      srslte_dft_predecoding(&q->dft_precoding, q->z, q->d, cfg->grant.L_prb, cfg->nbits.nof_symb);
      
      // Soft demodulation
      srslte_demod_soft_demodulate_s(cfg->grant.mcs.mod, q->d, q->q, cfg->nbits.nof_re);

      // Decode RI/HARQ bits before descrambling 
      if (srslte_ulsch_uci_decode_ri_ack(&q->ul_sch, cfg, softbuffer, q->q, seq->c, uci_data)) {
        fprintf(stderr, "Error decoding RI/HARQ bits\n");
        return SRSLTE_ERROR; 
      }
      
      // Descrambling
      srslte_scrambling_s_offset(seq, q->q, 0, cfg->nbits.nof_bits);

      return srslte_ulsch_uci_decode(&q->ul_sch, cfg, softbuffer, q->q, q->g, data, uci_data);      
    } else {
      fprintf(stderr, "Must call srslte_pusch_set_rnti() before calling srslte_pusch_decode()\n");
      return SRSLTE_ERROR; 
    }    
  } else {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }
}

/** Decodes the PUSCH from the received symbols
 */
int srslte_pusch_uci_decode(srslte_pusch_t *q, 
                            srslte_pusch_cfg_t *cfg, srslte_softbuffer_rx_t *softbuffer, 
                            cf_t *sf_symbols, 
                            cf_t *ce, float noise_estimate, 
                            uint8_t *data, srslte_uci_data_t *uci_data) 
{
  return srslte_pusch_uci_decode_seq(q, cfg, softbuffer, &q->seq[cfg->sf_idx], sf_symbols, ce, noise_estimate, data, uci_data);
}

/** Decodes the PUSCH from the received symbols for a given RNTI index
 */
int srslte_pusch_uci_decode_rnti_idx(srslte_pusch_t *q, 
                                     srslte_pusch_cfg_t *cfg, srslte_softbuffer_rx_t *softbuffer, 
                                     cf_t *sf_symbols, 
                                     cf_t *ce, float noise_estimate, 
                                     uint32_t rnti_idx, 
                                     uint8_t *data, srslte_uci_data_t *uci_data) 
{
  if (rnti_idx < q->nof_crnti) {
    if (q->rnti_multi[rnti_idx]) {
      return srslte_pusch_uci_decode_seq(q, cfg, softbuffer, &q->seq_multi[cfg->sf_idx][rnti_idx], sf_symbols, ce, noise_estimate, data, uci_data);
    } else {
      fprintf(stderr, "Error RNTI idx %d not set\n", rnti_idx);
      return SRSLTE_ERROR; 
    }
  } else {
    return SRSLTE_ERROR_INVALID_INPUTS; 
  }
}

uint32_t srslte_pusch_last_noi(srslte_pusch_t *q) {
  return q->ul_sch.nof_iterations;
}


  
