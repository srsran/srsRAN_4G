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
#include "liblte/phy/phch/sch.h"
#include "liblte/phy/phch/uci.h"
#include "liblte/phy/common/phy_common.h"
#include "liblte/phy/utils/bit.h"
#include "liblte/phy/utils/debug.h"
#include "liblte/phy/utils/vector.h"


int sch_init(sch_t *q) {
  int ret = LIBLTE_ERROR_INVALID_INPUTS;
  if (q) {    
    bzero(q, sizeof(sch_t));
    
    if (crc_init(&q->crc_tb, LTE_CRC24A, 24)) {
      fprintf(stderr, "Error initiating CRC\n");
      goto clean;
    }
    if (crc_init(&q->crc_cb, LTE_CRC24B, 24)) {
      fprintf(stderr, "Error initiating CRC\n");
      goto clean;
    }

    if (tcod_init(&q->encoder, MAX_LONG_CB)) {
      fprintf(stderr, "Error initiating Turbo Coder\n");
      goto clean;
    }
    if (tdec_init(&q->decoder, MAX_LONG_CB)) {
      fprintf(stderr, "Error initiating Turbo Decoder\n");
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
    if (uci_cqi_init(&q->uci_cqi)) {
      goto clean;
    }
    
    ret = LIBLTE_SUCCESS;
  }
clean: 
  if (ret == LIBLTE_ERROR) {
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
  tdec_free(&q->decoder);
  tcod_free(&q->encoder);
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
static int encode_tb(sch_t *q, uint8_t *data, uint8_t *e_bits, uint32_t tbs, uint32_t nb_e, 
                    harq_t *harq_process, uint32_t rv_idx) 
{
  uint8_t parity[24];
  uint8_t *p_parity = parity;
  uint32_t par;
  uint32_t i;
  uint32_t cb_len, rp, wp, rlen, F, n_e;
  int ret = LIBLTE_ERROR_INVALID_INPUTS; 
  uint32_t Qm = lte_mod_bits_x_symbol(harq_process->mcs.mod);
  
  if (q             != NULL &&
      data          != NULL &&
      harq_process  != NULL)
  {
  
    uint32_t Gp = nb_e / Qm;
    uint32_t gamma = Gp%harq_process->cb_segm.C;

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
    }
    
    wp = 0;
    rp = 0;
    for (i = 0; i < harq_process->cb_segm.C; i++) {

      /* Get read lengths */
      if (i < harq_process->cb_segm.C2) {
        cb_len = harq_process->cb_segm.K2;
      } else {
        cb_len = harq_process->cb_segm.K1;
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
      if (i <= harq_process->cb_segm.C - gamma - 1) {
        n_e = Qm * (Gp/harq_process->cb_segm.C);
      } else {
        n_e = Qm * ((uint32_t) ceilf((float) Gp/harq_process->cb_segm.C));
      }

      INFO("CB#%d: cb_len: %d, rlen: %d, wp: %d, rp: %d, F: %d, E: %d\n", i,
          cb_len, rlen - F, wp, rp, F, n_e);

      if (rv_idx == 0) {

        /* Copy data to another buffer, making space for the Codeblock CRC */
        if (i < harq_process->cb_segm.C - 1) {
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
        if (harq_process->cb_segm.C > 1) {
          crc_attach(&q->crc_cb, q->cb_in, rlen);
        }
        /* Set the filler bits to <NULL> */
        for (int j = 0; j < F; j++) {
          q->cb_in[j] = TX_NULL;
        }
        if (VERBOSE_ISDEBUG()) {
          DEBUG("CB#%d: ", i);
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
  } 
  return ret; 
}

/* Decode a transport block according to 36.212 5.3.2
 *
 */
static int decode_tb(sch_t *q, float *e_bits, uint8_t *data, uint32_t tbs, uint32_t nb_e, 
                    harq_t *harq_process, uint32_t rv_idx) 
{
  uint8_t parity[24];
  uint8_t *p_parity = parity;
  uint32_t par_rx, par_tx;
  uint32_t i;
  uint32_t cb_len, rp, wp, rlen, F, n_e;
  uint32_t Qm = lte_mod_bits_x_symbol(harq_process->mcs.mod);
  
  if (q            != NULL   && 
      data         != NULL   &&       
      harq_process != NULL)
  {

    rp = 0;
    rp = 0;
    wp = 0;
    uint32_t Gp = nb_e / Qm;
    uint32_t gamma = Gp%harq_process->cb_segm.C;
    bool early_stop = true;
    for (i = 0; i < harq_process->cb_segm.C && early_stop; i++) {

      /* Get read/write lengths */
      if (i < harq_process->cb_segm.C2) {
        cb_len = harq_process->cb_segm.K2;
      } else {
        cb_len = harq_process->cb_segm.K1;
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

      if (i <= harq_process->cb_segm.C - gamma - 1) {
        n_e = Qm * (Gp/harq_process->cb_segm.C);
      } else {
        n_e = Qm * ((uint32_t) ceilf((float) Gp/harq_process->cb_segm.C));
      }

      INFO("CB#%d: cb_len: %d, rlen: %d, wp: %d, rp: %d, F: %d, E: %d\n", i,
          cb_len, rlen - F, wp, rp, F, n_e);
      
      /* Rate Unmatching */
      if (rm_turbo_rx(harq_process->pdsch_w_buff_f[i], harq_process->w_buff_size,  
                  &e_bits[rp], n_e, 
                  (float*) q->cb_out, 3 * cb_len + 12, rv_idx, F)) {
        fprintf(stderr, "Error in rate matching\n");
        return LIBLTE_ERROR;
      }

      if (VERBOSE_ISDEBUG()) {
        DEBUG("CB#%d RMOUT: ", i);
        vec_fprint_f(stdout, q->cb_out, 3*cb_len+12);
      }

      /* Turbo Decoding with CRC-based early stopping */
      q->nof_iterations = 0; 
      uint32_t len_crc; 
      uint8_t *cb_in_ptr; 
      crc_t *crc_ptr; 
      early_stop = false; 

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
          cb_in_ptr = &q->cb_in[F];
          crc_ptr = &q->crc_tb; 
        }

        tdec_decision(&q->decoder, q->cb_in, cb_len);
  
        /* Check Codeblock CRC and stop early if incorrect */
        if (!crc_checksum(crc_ptr, cb_in_ptr, len_crc)) {
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

    if (!early_stop) {
      INFO("CB %d failed. TB is erroneous.\n",i-1);
      return LIBLTE_ERROR; 
    } else {
      INFO("END CB#%d: wp: %d, rp: %d\n", i, wp, rp);

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
      
    }
  } else {
    return LIBLTE_ERROR_INVALID_INPUTS;
  }
}

int dlsch_decode(sch_t *q, float *e_bits, uint8_t *data, uint32_t tbs, uint32_t nb_e, 
                    harq_t *harq_process, uint32_t rv_idx) 
{
  return decode_tb(q, e_bits, data, tbs, nb_e, harq_process, rv_idx);
}

int dlsch_encode(sch_t *q, uint8_t *data, uint8_t *e_bits, uint32_t tbs, uint32_t nb_e, 
                    harq_t *harq_process, uint32_t rv_idx) {
  return encode_tb(q, data, e_bits, tbs, nb_e, harq_process, rv_idx);
}

int ulsch_decode(sch_t *q, float *e_bits, uint8_t *data, uint32_t tbs, uint32_t nb_e, 
                    harq_t *harq_process, uint32_t rv_idx) 
{
  return decode_tb(q, e_bits, data, tbs, nb_e, harq_process, rv_idx);
}


uint8_t ulsch_y_idx[10000];
uint8_t ulsch_y_mat[10000];

/* UL-SCH channel interleaver according to 5.5.2.8 of 36.212 */
void ulsch_interleave(uint8_t *q_bits, uint32_t nb_q, 
                      uint8_t q_bits_ack[6], uint32_t Q_prime_ack, 
                      uint8_t q_bits_ri[6], uint32_t Q_prime_ri,
                      uint32_t Q_m) 
{
  uint32_t C_mux;
  uint32_t H_prime;
  uint32_t H_prime_total;
  uint32_t R_mux;
  uint32_t R_prime_mux;
  uint32_t i;
  uint32_t j;
  uint32_t k;
  uint32_t r;
  uint32_t idx;
  uint32_t ri_column_set[4]  = {1, 4, 7, 10};
  uint32_t ack_column_set[4] = {2, 3, 8, 9};
  uint32_t C_ri;
  uint32_t C_ack;
  uint32_t N_pusch_symbs = 12; 
  
  // Step 1: Define C_mux
  C_mux = N_pusch_symbs;

  // Step 2: Define R_mux and R_prime_mux
  H_prime       = nb_q;
  H_prime_total = H_prime + Q_prime_ri;
  R_mux         = (H_prime_total*Q_m)/C_mux;
  R_prime_mux   = R_mux/Q_m;
 

  // Initialize the matricies
  printf("Cmux*R_prime=%d*%d=%d, H_prime=%d, H_prime_total=%d\n",C_mux, R_prime_mux, C_mux*R_prime_mux, H_prime, H_prime_total);
  for(i=0; i<C_mux*R_prime_mux; i++) {
    ulsch_y_idx[i] = 100;
  }
  for(i=0; i<C_mux*R_mux; i++) {
    ulsch_y_mat[i] = 0;
  }

  // Step 3: Interleave the RI control bits
  i = 0;
  j = 0;
  r = R_prime_mux-1;
  while(i < Q_prime_ri) {
    C_ri = ri_column_set[j];
    ulsch_y_idx[r*C_mux + C_ri] = 1;
    for(k=0; k<Q_m; k++) {
      ulsch_y_mat[(C_mux*r*Q_m) + C_ri*Q_m + k] = q_bits_ri[Q_m*i+k];
    }
    i++;
    r = R_prime_mux - 1 - i/4;
    j = (j + 3) % 4;
  }
  printf("H_prime: %d, C_mux: %d, R_prime: %d\n", H_prime, C_mux, R_prime_mux);
  
  // Step 4: Interleave the data bits
  i = 0;
  k = 0;
  while(k < H_prime) {
    if(ulsch_y_idx[i] == 100) {
      ulsch_y_idx[i] = 1;
      for(j=0; j<Q_m; j++) {
        ulsch_y_mat[i*Q_m + j] = q_bits[Q_m*k+j];
      }
      k++;
    }
    i++;
  }

  // Step 5: Interleave the ACK control bits
  i = 0;
  j = 0;
  r = R_prime_mux-1;
  while(i < Q_prime_ack) {
    C_ack = ack_column_set[j];
    ulsch_y_idx[r*C_mux + C_ack] = 2;
    for(k=0; k<Q_m; k++) {
      ulsch_y_mat[(C_mux*r*Q_m) + C_ack*Q_m + k] = q_bits_ack[Q_m*i+k];
    }
    i++;
    r = R_prime_mux - 1 - i/4;
    j = (j + 3) % 4;
  }

  // Step 6: Read out the bits
  idx = 0;
  printf("go for C_mux: %d, R_prime: %d, Q_m: %d\n", C_mux, R_prime_mux, Q_m);
  for(i=0; i<C_mux; i++) {
    for(j=0; j<R_prime_mux; j++) {
      for(k=0; k<Q_m; k++) {
        q_bits[idx++] = ulsch_y_mat[j*C_mux*Q_m + i*Q_m + k];          
      }
    }
  }
  
}


int ulsch_encode(sch_t *q, uint8_t *data, uint8_t *q_bits, 
                 harq_t *harq_process, uint32_t rv_idx) 
{
  uci_data_t uci_data; 
  bzero(&uci_data, sizeof(uci_data_t));
  return ulsch_uci_encode(q, data, uci_data, q_bits, NULL, NULL, harq_process, rv_idx);
}


int ulsch_uci_encode(sch_t *q, uint8_t *data, uci_data_t uci_data, uint8_t *q_bits, 
                 uint8_t *q_bits_ack, uint8_t *q_bits_ri, 
                 harq_t *harq_process, uint32_t rv_idx) 
{
  int ret; 
   
  uint32_t e_offset = 0;
  uint32_t Q_prime_cqi = 0; 
  uint32_t Q_prime_ack = 0;
  uint32_t Q_prime_ri = 0;
  uint32_t Q_m = lte_mod_bits_x_symbol(harq_process->mcs.mod);
  
  uint32_t nof_symbols = 12*harq_process->prb_alloc.slot[0].nof_prb*RE_X_RB;
  uint32_t nb_q = nof_symbols * Q_m;

 // Encode ACK
  if (uci_data.uci_ack_len > 0) {
    float beta = uci_data.beta_ack; 
    if (harq_process->mcs.tbs == 0) {
        beta /= uci_data.beta_cqi;
    }
    ret = uci_encode_ri_ack(uci_data.uci_ack, uci_data.uci_cqi_len, beta, harq_process, q_bits_ack);
    if (ret < 0) {
      return ret; 
    }
    Q_prime_ack = (uint32_t) ret; 
  }
    
  // Encode RI
  if (uci_data.uci_ri_len > 0) {
    float beta = uci_data.beta_ri; 
    if (harq_process->mcs.tbs == 0) {
        beta /= uci_data.beta_cqi;
    }
    ret = uci_encode_ri_ack(uci_data.uci_ri, uci_data.uci_cqi_len, beta, harq_process, q_bits_ri);
    if (ret < 0) {
      return ret; 
    }
    Q_prime_ri = (uint32_t) ret; 
  }

  // Encode CQI
  if (uci_data.uci_cqi_len > 0) {
    ret = uci_encode_cqi(&q->uci_cqi, uci_data.uci_cqi, uci_data.uci_cqi_len, uci_data.beta_cqi, 
                                 Q_prime_ri, harq_process, q_bits);
    if (ret < 0) {
      return ret; 
    }
    Q_prime_cqi = (uint32_t) ret; 
  }
  
  e_offset += Q_prime_cqi*Q_m;

  printf("Q_prime_ack=%d, Q_prime_cqi=%d, Q_prime_ri=%d\n",Q_prime_ack, Q_prime_cqi, Q_prime_ri);  
  // Encode UL-SCH
  if (harq_process->mcs.tbs > 0) {
    uint32_t G = nb_q/Q_m - Q_prime_ri - Q_prime_cqi;     
    ret = encode_tb(q, data, &q_bits[e_offset], harq_process->mcs.tbs, 
                G*Q_m, harq_process, rv_idx);
    if (ret) {
      return ret; 
    }    
  } 

  // Multiplexing and Interleaving 
  ulsch_interleave(q_bits, nb_q/Q_m-Q_prime_ri, 
                   q_bits_ack, Q_prime_ack,
                   q_bits_ri, Q_prime_ri, 
                   Q_m);



  return LIBLTE_SUCCESS;
}

