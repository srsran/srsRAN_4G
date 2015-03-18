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

#include "srslte/phch/pusch.h"
#include "srslte/phch/sch.h"
#include "srslte/phch/uci.h"
#include "srslte/common/phy_common.h"
#include "srslte/utils/bit.h"
#include "srslte/utils/debug.h"
#include "srslte/utils/vector.h"


int sch_init(sch_t *q) {
  int ret = SRSLTE_ERROR_INVALID_INPUTS;
  if (q) {    
    bzero(q, sizeof(sch_t));
    
    if (srslte_crc_init(&q->srslte_crc_tb, SRSLTE_LTE_CRC24A, 24)) {
      fprintf(stderr, "Error initiating CRC\n");
      goto clean;
    }
    if (srslte_crc_init(&q->srslte_crc_cb, SRSLTE_LTE_CRC24B, 24)) {
      fprintf(stderr, "Error initiating CRC\n");
      goto clean;
    }

    if (srslte_tcod_init(&q->encoder, MAX_LONG_CB)) {
      fprintf(stderr, "Error initiating Turbo Coder\n");
      goto clean;
    }
    if (srslte_tdec_init(&q->decoder, MAX_LONG_CB)) {
      fprintf(stderr, "Error initiating Turbo Decoder\n");
      goto clean;
    }

    // Allocate floats for reception (LLRs)
    q->cb_in = vec_malloc(sizeof(uint8_t) * MAX_LONG_CB);
    if (!q->cb_in) {
      goto clean;
    }
    
    q->cb_out = vec_malloc(sizeof(float) * (3 * MAX_LONG_CB + 12));
    if (!q->cb_out) {
      goto clean;
    }  
    if (uci_cqi_init(&q->uci_cqi)) {
      goto clean;
    }
    
    ret = SRSLTE_SUCCESS;
  }
clean: 
  if (ret == SRSLTE_ERROR) {
    sch_free(q);
  }
  return ret; 
}

void sch_free(sch_t *q) {
  if (q->cb_in) {
    free(q->cb_in);
  }
  if (q->cb_out) {
    free(q->cb_out);
  }
  srslte_tdec_free(&q->decoder);
  srslte_tcod_free(&q->encoder);
  uci_cqi_free(&q->uci_cqi);
  bzero(q, sizeof(sch_t));
}


float sch_average_noi(sch_t *q) {
  return q->average_nof_iterations; 
}

uint32_t sch_last_noi(sch_t *q) {
  return q->nof_iterations;
}



/* Encode a transport block according to 36.212 5.3.2
 *
 */
static int encode_tb(sch_t *q, harq_t *harq, uint8_t *data, uint8_t *e_bits, uint32_t nof_e_bits) 
{
  uint8_t parity[24];
  uint8_t *p_parity = parity;
  uint32_t par;
  uint32_t i;
  uint32_t cb_len, rp, wp, rlen, F, n_e;
  int ret = SRSLTE_ERROR_INVALID_INPUTS; 
  uint32_t Qm = srslte_mod_bits_x_symbol(harq->mcs.mod);
  
  if (q             != NULL &&
      data          != NULL &&
      harq          != NULL)
  {
  
    uint32_t Gp = nof_e_bits / Qm;
    
    uint32_t gamma = Gp;
    if (harq->cb_segm.C > 0) {
      gamma = Gp%harq->cb_segm.C;
    }
    
    if (harq->rv == 0) {
      /* Compute transport block CRC */
      par = srslte_crc_checksum(&q->srslte_crc_tb, data, harq->mcs.tbs);

      /* parity bits will be appended later */
      bit_pack(par, &p_parity, 24);

      if (VERBOSE_ISDEBUG()) {
        DEBUG("DATA: ", 0);
        vec_fprint_b(stdout, data, harq->mcs.tbs);
        DEBUG("PARITY: ", 0);
        vec_fprint_b(stdout, parity, 24);
      }      
    }
    
    wp = 0;
    rp = 0;
    for (i = 0; i < harq->cb_segm.C; i++) {

      /* Get read lengths */
      if (i < harq->cb_segm.C2) {
        cb_len = harq->cb_segm.K2;
      } else {
        cb_len = harq->cb_segm.K1;
      }
      if (harq->cb_segm.C > 1) {
        rlen = cb_len - 24;
      } else {
        rlen = cb_len;
      }
      if (i == 0) {
        F = harq->cb_segm.F;
      } else {
        F = 0;
      }
      if (i <= harq->cb_segm.C - gamma - 1) {
        n_e = Qm * (Gp/harq->cb_segm.C);
      } else {
        n_e = Qm * ((uint32_t) ceilf((float) Gp/harq->cb_segm.C));
      }

      INFO("CB#%d: cb_len: %d, rlen: %d, wp: %d, rp: %d, F: %d, E: %d\n", i,
          cb_len, rlen - F, wp, rp, F, n_e);

      if (harq->rv == 0) {

        /* Copy data to another buffer, making space for the Codeblock CRC */
        if (i < harq->cb_segm.C - 1) {
          // Copy data 
          memcpy(&q->cb_in[F], &data[rp], (rlen - F) * sizeof(uint8_t));
        } else {
          INFO("Last CB, appending parity: %d from %d and 24 to %d\n",
              rlen - F - 24, rp, rlen - 24);
          /* Append Transport Block parity bits to the last CB */
          memcpy(&q->cb_in[F], &data[rp], (rlen - 24 - F) * sizeof(uint8_t));
          memcpy(&q->cb_in[rlen - 24], parity, 24 * sizeof(uint8_t));
        }        
        /* Filler bits are treated like zeros for the CB CRC calculation */
        for (int j = 0; j < F; j++) {
          q->cb_in[j] = 0;
        }
        /* Attach Codeblock CRC */
        if (harq->cb_segm.C > 1) {
          srslte_crc_attach(&q->srslte_crc_cb, q->cb_in, rlen);
        }
        /* Set the filler bits to <NULL> */
        for (int j = 0; j < F; j++) {
          q->cb_in[j] = SRSLTE_TX_NULL;
        }
        if (VERBOSE_ISDEBUG()) {
          DEBUG("CB#%d: ", i);
          vec_fprint_b(stdout, q->cb_in, cb_len);
        }

        /* Turbo Encoding */
        srslte_tcod_encode(&q->encoder, q->cb_in, (uint8_t*) q->cb_out, cb_len);
      }
      
      /* Rate matching */
      if (srslte_rm_turbo_tx(harq->pdsch_w_buff_c[i], harq->w_buff_size, 
                  (uint8_t*) q->cb_out, 3 * cb_len + 12,
                  &e_bits[wp], n_e, harq->rv))
      {
        fprintf(stderr, "Error in rate matching\n");
        return SRSLTE_ERROR;
      }

      /* Set read/write pointers */
      rp += (rlen - F);
      wp += n_e;
    }
    INFO("END CB#%d: wp: %d, rp: %d\n", i, wp, rp);
    
    ret = SRSLTE_SUCCESS;      
  } 
  return ret; 
}

/* Decode a transport block according to 36.212 5.3.2
 *
 */
static int decode_tb(sch_t *q, harq_t *harq, float *e_bits, uint8_t *data, uint32_t nof_e_bits) 
{
  uint8_t parity[24];
  uint8_t *p_parity = parity;
  uint32_t par_rx, par_tx;
  uint32_t i;
  uint32_t cb_len, rp, wp, rlen, F, n_e;
  uint32_t Qm = srslte_mod_bits_x_symbol(harq->mcs.mod);
  
  if (q            != NULL   && 
      data         != NULL   &&       
      harq         != NULL)
  {

    if (harq->mcs.tbs == 0 || harq->cb_segm.C == 0) {
      return SRSLTE_SUCCESS;
    }
    
    rp = 0;
    rp = 0;
    wp = 0;
    uint32_t Gp = nof_e_bits / Qm;
    uint32_t gamma=Gp;

    if (harq->cb_segm.C>0) {
      gamma = Gp%harq->cb_segm.C;
    }
    
    bool early_stop = true;
    for (i = 0; i < harq->cb_segm.C && early_stop; i++) {

      /* Get read/write lengths */
      if (i < harq->cb_segm.C2) {
        cb_len = harq->cb_segm.K2;
      } else {
        cb_len = harq->cb_segm.K1;
      }
      if (harq->cb_segm.C == 1) {
        rlen = cb_len;
      } else {
        rlen = cb_len - 24;
      }
      if (i == 0) {
        F = harq->cb_segm.F;
      } else {
        F = 0;
      }

      if (i <= harq->cb_segm.C - gamma - 1) {
        n_e = Qm * (Gp/harq->cb_segm.C);
      } else {
        n_e = Qm * ((uint32_t) ceilf((float) Gp/harq->cb_segm.C));
      }

      INFO("CB#%d: cb_len: %d, rlen: %d, wp: %d, rp: %d, F: %d, E: %d\n", i,
          cb_len, rlen - F, wp, rp, F, n_e);
      
      /* Rate Unmatching */
      if (srslte_rm_turbo_rx(harq->pdsch_w_buff_f[i], harq->w_buff_size,  
                  &e_bits[rp], n_e, 
                  (float*) q->cb_out, 3 * cb_len + 12, harq->rv, F)) {
        fprintf(stderr, "Error in rate matching\n");
        return SRSLTE_ERROR;
      }

      if (VERBOSE_ISDEBUG()) {
        DEBUG("CB#%d RMOUT: ", i);
        vec_fprint_f(stdout, q->cb_out, 3*cb_len+12);
      }

      /* Turbo Decoding with CRC-based early stopping */
      q->nof_iterations = 0; 
      uint32_t len_crc; 
      uint8_t *cb_in_ptr; 
      srslte_crc_t *srslte_crc_ptr; 
      early_stop = false; 

      srslte_tdec_reset(&q->decoder, cb_len);
            
      do {
        srslte_tdec_iteration(&q->decoder, (float*) q->cb_out, cb_len); 
        q->nof_iterations++;
        
        if (harq->cb_segm.C > 1) {
          len_crc = cb_len; 
          cb_in_ptr = q->cb_in; 
          srslte_crc_ptr = &q->srslte_crc_cb; 
        } else {
          len_crc = harq->mcs.tbs+24; 
          cb_in_ptr = &q->cb_in[F];
          srslte_crc_ptr = &q->srslte_crc_tb; 
        }

        srslte_tdec_decision(&q->decoder, q->cb_in, cb_len);
  
        /* Check Codeblock CRC and stop early if incorrect */
        if (!srslte_crc_checksum(srslte_crc_ptr, cb_in_ptr, len_crc)) {
          early_stop = true;           
        }
        
      } while (q->nof_iterations < TDEC_MAX_ITERATIONS && !early_stop);
      q->average_nof_iterations = VEC_EMA((float) q->nof_iterations, q->average_nof_iterations, 0.2);

      if (VERBOSE_ISDEBUG()) {
        DEBUG("CB#%d IN: ", i);
        vec_fprint_b(stdout, q->cb_in, cb_len);
      }
            
      // If CB CRC is not correct, early_stop will be false and wont continue with rest of CBs

      /* Copy data to another buffer, removing the Codeblock CRC */
      if (i < harq->cb_segm.C - 1) {
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

    if (!early_stop) {
      INFO("CB %d failed. TB is erroneous.\n",i-1);
      return SRSLTE_ERROR; 
    } else {
      INFO("END CB#%d: wp: %d, rp: %d\n", i, wp, rp);

      // Compute transport block CRC
      par_rx = srslte_crc_checksum(&q->srslte_crc_tb, data, harq->mcs.tbs);

      // check parity bits
      par_tx = bit_unpack(&p_parity, 24);

      if (!par_rx) {
        INFO("\n\tCAUTION!! Received all-zero transport block\n\n", 0);
      }

      if (par_rx == par_tx) {
        INFO("TB decoded OK\n",i);
        return SRSLTE_SUCCESS;
      } else {
        INFO("Error in TB parity\n",i);
        return SRSLTE_ERROR;
      }
      
    }
  } else {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }
}

int dlsch_decode(sch_t *q, harq_t *harq, float *e_bits, uint8_t *data) 
{
  return decode_tb(q, harq, e_bits, data, harq->nof_bits);
}

int dlsch_encode(sch_t *q, harq_t *harq, uint8_t *data, uint8_t *e_bits) {
  return encode_tb(q, harq, data, e_bits, harq->nof_bits);
}

int ulsch_decode(sch_t *q, harq_t *harq, float *e_bits, uint8_t *data) 
{
  return decode_tb(q, harq, e_bits, data, harq->nof_bits);
}


/* UL-SCH channel interleaver according to 5.5.2.8 of 36.212 */
void ulsch_interleave(uint8_t *g_bits, uint32_t Q_m, uint32_t H_prime_total, uint32_t N_pusch_symbs, uint8_t *q_bits) 
{
  
  uint32_t rows = H_prime_total/N_pusch_symbs;
  uint32_t cols = N_pusch_symbs;
  
  uint32_t idx = 0;
  for(uint32_t j=0; j<rows; j++) {        
    for(uint32_t i=0; i<cols; i++) {
      for(uint32_t k=0; k<Q_m; k++) {
        if (q_bits[j*Q_m + i*rows*Q_m + k] >= 10) {
          q_bits[j*Q_m + i*rows*Q_m + k] -= 10;
        } else {
          q_bits[j*Q_m + i*rows*Q_m + k] = g_bits[idx];                                
          idx++;                  
        }
      }
    }
  }
  
}

int ulsch_encode(sch_t *q, harq_t *harq, uint8_t *data, uint8_t *g_bits, uint8_t *q_bits) 
{
  uci_data_t uci_data; 
  bzero(&uci_data, sizeof(uci_data_t));
  return ulsch_uci_encode(q, harq, data, uci_data, g_bits, q_bits);
}


int ulsch_uci_encode(sch_t *q, harq_t *harq, uint8_t *data, uci_data_t uci_data, uint8_t *g_bits, uint8_t *q_bits) 
{
  int ret; 
   
  uint32_t e_offset = 0;
  uint32_t Q_prime_cqi = 0; 
  uint32_t Q_prime_ack = 0;
  uint32_t Q_prime_ri = 0;
  uint32_t Q_m = srslte_mod_bits_x_symbol(harq->mcs.mod);
  
  uint32_t nof_symbols = 12*harq->ul_alloc.L_prb*SRSLTE_NRE;
  uint32_t nb_q = nof_symbols * Q_m;

  bzero(q_bits, sizeof(uint8_t) * nb_q);

  // Encode RI
  if (uci_data.uci_ri_len > 0) {
    float beta = uci_data.beta_ri; 
    if (harq->mcs.tbs == 0) {
        beta /= uci_data.beta_cqi;
    }
    ret = uci_encode_ri(uci_data.uci_ri, uci_data.uci_cqi_len, beta, harq, nb_q/Q_m, q_bits);
    if (ret < 0) {
      return ret; 
    }
    Q_prime_ri = (uint32_t) ret; 
  }

  // Encode CQI
  if (uci_data.uci_cqi_len > 0) {

    ret = uci_encode_cqi_pusch(&q->uci_cqi, uci_data.uci_cqi, uci_data.uci_cqi_len, uci_data.beta_cqi, 
                                 Q_prime_ri, harq, g_bits);
    if (ret < 0) {
      return ret; 
    }
    Q_prime_cqi = (uint32_t) ret; 
  }
  
  e_offset += Q_prime_cqi*Q_m;
  
  // Encode UL-SCH
  if (harq->mcs.tbs > 0) {
    uint32_t G = nb_q/Q_m - Q_prime_ri - Q_prime_cqi;     
    ret = encode_tb(q, harq, data, &g_bits[e_offset], G*Q_m);
    if (ret) {
      return ret; 
    }    
  } 
    
  // Interleave UL-SCH (and RI and CQI)
  ulsch_interleave(g_bits, Q_m, nb_q/Q_m, harq->nof_symb, q_bits);
  
   // Encode (and interleave) ACK
  if (uci_data.uci_ack_len > 0) {
    float beta = uci_data.beta_ack; 
    if (harq->mcs.tbs == 0) {
        beta /= uci_data.beta_cqi;
    }
    ret = uci_encode_ack(uci_data.uci_ack, uci_data.uci_cqi_len, beta, harq, nb_q/Q_m, q_bits);
    if (ret < 0) {
      return ret; 
    }
    Q_prime_ack = (uint32_t) ret; 
  }
    
  INFO("Q_prime_ack=%d, Q_prime_cqi=%d, Q_prime_ri=%d\n",Q_prime_ack, Q_prime_cqi, Q_prime_ri);  



  return SRSLTE_SUCCESS;
}

