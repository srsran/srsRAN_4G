/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
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

#include "srslte/phch/pdsch.h"
#include "srslte/phch/pusch.h"
#include "srslte/phch/sch.h"
#include "srslte/phch/uci.h"
#include "srslte/common/phy_common.h"
#include "srslte/utils/bit.h"
#include "srslte/utils/debug.h"
#include "srslte/utils/vector.h"

/* 36.213 Table 8.6.3-1: Mapping of HARQ-ACK offset values and the index signalled by higher layers */
float beta_harq_offset[16] = {2.0, 2.5, 3.125, 4.0, 5.0, 6.250, 8.0, 10.0, 
                           12.625, 15.875, 20.0, 31.0, 50.0, 80.0, 126.0, -1.0};
                                  
/* 36.213 Table 8.6.3-2: Mapping of RI offset values and the index signalled by higher layers */
float beta_ri_offset[16] = {1.25, 1.625, 2.0, 2.5, 3.125, 4.0, 5.0, 6.25, 8.0, 10.0,
                           12.625, 15.875, 20.0, -1.0, -1.0, -1.0};

/* 36.213 Table 8.6.3-3: Mapping of CQI offset values and the index signalled by higher layers */
float beta_cqi_offset[16] = {-1.0, -1.0, 1.125, 1.25, 1.375, 1.625, 1.750, 2.0, 2.25, 2.5, 2.875, 
                             3.125, 3.5, 4.0, 5.0, 6.25};


uint32_t srslte_sch_find_Ioffset_ack(float beta) {
  for (int i=0;i<16;i++) {
    if (beta_harq_offset[i] >= beta) {
      return i; 
    }
  }
  return 0;
}
                             
uint32_t srslte_sch_find_Ioffset_ri(float beta) {
  for (int i=0;i<16;i++) {
    if (beta_ri_offset[i] >= beta) {
      return i; 
    }
  }
  return 0;
}
                             
uint32_t srslte_sch_find_Ioffset_cqi(float beta) {
  for (int i=0;i<16;i++) {
    if (beta_cqi_offset[i] >= beta) {
      return i; 
    }
  }
  return 0;
}
                             
int srslte_sch_init(srslte_sch_t *q) {
  int ret = SRSLTE_ERROR_INVALID_INPUTS;
  if (q) {    
    bzero(q, sizeof(srslte_sch_t));
    
    if (srslte_crc_init(&q->crc_tb, SRSLTE_LTE_CRC24A, 24)) {
      fprintf(stderr, "Error initiating CRC\n");
      goto clean;
    }
    if (srslte_crc_init(&q->crc_cb, SRSLTE_LTE_CRC24B, 24)) {
      fprintf(stderr, "Error initiating CRC\n");
      goto clean;
    }

    if (srslte_tcod_init(&q->encoder, SRSLTE_TCOD_MAX_LEN_CB)) {
      fprintf(stderr, "Error initiating Turbo Coder\n");
      goto clean;
    }
    if (srslte_tdec_init(&q->decoder, SRSLTE_TCOD_MAX_LEN_CB)) {
      fprintf(stderr, "Error initiating Turbo Decoder\n");
      goto clean;
    }

    // Allocate floats for reception (LLRs)
    q->cb_in = srslte_vec_malloc(sizeof(uint8_t) * SRSLTE_TCOD_MAX_LEN_CB);
    if (!q->cb_in) {
      goto clean;
    }
    
    q->cb_out = srslte_vec_malloc(sizeof(float) * (3 * SRSLTE_TCOD_MAX_LEN_CB + 12));
    if (!q->cb_out) {
      goto clean;
    }  
    if (srslte_uci_cqi_init(&q->uci_cqi)) {
      goto clean;
    }
    
    ret = SRSLTE_SUCCESS;
  }
clean: 
  if (ret == SRSLTE_ERROR) {
    srslte_sch_free(q);
  }
  return ret; 
}

void srslte_sch_free(srslte_sch_t *q) {
  if (q->cb_in) {
    free(q->cb_in);
  }
  if (q->cb_out) {
    free(q->cb_out);
  }
  srslte_tdec_free(&q->decoder);
  srslte_tcod_free(&q->encoder);
  srslte_uci_cqi_free(&q->uci_cqi);
  bzero(q, sizeof(srslte_sch_t));
}


float srslte_sch_average_noi(srslte_sch_t *q) {
  return q->average_nof_iterations; 
}

uint32_t srslte_sch_last_noi(srslte_sch_t *q) {
  return q->nof_iterations;
}



/* Encode a transport block according to 36.212 5.3.2
 *
 */
static int encode_tb(srslte_sch_t *q, 
                     srslte_softbuffer_tx_t *soft_buffer, srslte_cbsegm_t *cb_segm, 
                     uint32_t Qm, uint32_t rv, uint32_t nof_e_bits,  
                     uint8_t *data, uint8_t *e_bits) 
{
  uint8_t parity[24];
  uint8_t *p_parity = parity;
  uint32_t par;
  uint32_t i;
  uint32_t cb_len, rp, wp, rlen, F, n_e;
  int ret = SRSLTE_ERROR_INVALID_INPUTS; 
  
  if (q            != NULL &&
      e_bits       != NULL &&
      cb_segm      != NULL &&
      soft_buffer  != NULL)
  {
  
    uint32_t Gp = nof_e_bits / Qm;
    
    uint32_t gamma = Gp;
    if (cb_segm->C > 0) {
      gamma = Gp%cb_segm->C;
    }
    
    if (data) {
      /* Compute transport block CRC */
      par = srslte_crc_checksum(&q->crc_tb, data, cb_segm->tbs);

      /* parity bits will be appended later */
      srslte_bit_pack(par, &p_parity, 24);

      if (SRSLTE_VERBOSE_ISDEBUG()) {
        DEBUG("DATA: ", 0);
        srslte_vec_fprint_b(stdout, data, cb_segm->tbs);
        DEBUG("PARITY: ", 0);
        srslte_vec_fprint_b(stdout, parity, 24);
      }      
    }
    
    wp = 0;
    rp = 0;
    for (i = 0; i < cb_segm->C; i++) {

      /* Get read lengths */
      if (i < cb_segm->C2) {
        cb_len = cb_segm->K2;
      } else {
        cb_len = cb_segm->K1;
      }
      if (cb_segm->C > 1) {
        rlen = cb_len - 24;
      } else {
        rlen = cb_len;
      }
      if (i == 0) {
        F = cb_segm->F;
      } else {
        F = 0;
      }
      if (i <= cb_segm->C - gamma - 1) {
        n_e = Qm * (Gp/cb_segm->C);
      } else {
        n_e = Qm * ((uint32_t) ceilf((float) Gp/cb_segm->C));
      }

      INFO("CB#%d: cb_len: %d, rlen: %d, wp: %d, rp: %d, F: %d, E: %d\n", i,
          cb_len, rlen - F, wp, rp, F, n_e);

      if (data) {

        /* Copy data to another buffer, making space for the Codeblock CRC */
        if (i < cb_segm->C - 1) {
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
        if (cb_segm->C > 1) {
          srslte_crc_attach(&q->crc_cb, q->cb_in, rlen);
        }
        /* Set the filler bits to <NULL> */
        for (int j = 0; j < F; j++) {
          q->cb_in[j] = SRSLTE_TX_NULL;
        }
        if (SRSLTE_VERBOSE_ISDEBUG()) {
          DEBUG("CB#%d: ", i);
          srslte_vec_fprint_b(stdout, q->cb_in, cb_len);
        }

        /* Turbo Encoding */
        srslte_tcod_encode(&q->encoder, q->cb_in, (uint8_t*) q->cb_out, cb_len);
      }
      
      /* Rate matching */
      if (srslte_rm_turbo_tx(soft_buffer->buffer_b[i], soft_buffer->buff_size, 
                  (uint8_t*) q->cb_out, 3 * cb_len + 12,
                  &e_bits[wp], n_e, rv))
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
static int decode_tb(srslte_sch_t *q, 
                     srslte_softbuffer_rx_t *softbuffer, srslte_cbsegm_t *cb_segm, 
                     uint32_t Qm, uint32_t rv, uint32_t nof_e_bits, 
                     float *e_bits, uint8_t *data) 
{
  uint8_t parity[24];
  uint8_t *p_parity = parity;
  uint32_t par_rx, par_tx;
  uint32_t i;
  uint32_t cb_len, rp, wp, rlen, F, n_e;
  
  if (q            != NULL && 
      data         != NULL &&       
      softbuffer   != NULL &&
      e_bits       != NULL &&
      cb_segm      != NULL)
  {

    if (cb_segm->tbs == 0 || cb_segm->C == 0) {
      return SRSLTE_SUCCESS;
    }
    
    rp = 0;
    rp = 0;
    wp = 0;
    uint32_t Gp = nof_e_bits / Qm;
    uint32_t gamma=Gp;

    if (cb_segm->C>0) {
      gamma = Gp%cb_segm->C;
    }
    
    bool early_stop = true;
    for (i = 0; i < cb_segm->C && early_stop; i++) {

      /* Get read/write lengths */
      if (i < cb_segm->C2) {
        cb_len = cb_segm->K2;
      } else {
        cb_len = cb_segm->K1;
      }
      if (cb_segm->C == 1) {
        rlen = cb_len;
      } else {
        rlen = cb_len - 24;
      }
      if (i == 0) {
        F = cb_segm->F;
      } else {
        F = 0;
      }

      if (i <= cb_segm->C - gamma - 1) {
        n_e = Qm * (Gp/cb_segm->C);
      } else {
        n_e = Qm * ((uint32_t) ceilf((float) Gp/cb_segm->C));
      }

      INFO("CB#%d: cb_len: %d, rlen: %d, wp: %d, rp: %d, F: %d, E: %d\n", i,
          cb_len, rlen - F, wp, rp, F, n_e);
      
      /* Rate Unmatching */
      if (srslte_rm_turbo_rx(softbuffer->buffer_f[i], softbuffer->buff_size,  
                  &e_bits[rp], n_e, 
                  (float*) q->cb_out, 3 * cb_len + 12, rv, F)) {
        fprintf(stderr, "Error in rate matching\n");
        return SRSLTE_ERROR;
      }

      if (SRSLTE_VERBOSE_ISDEBUG()) {
        DEBUG("CB#%d RMOUT: ", i);
        srslte_vec_fprint_f(stdout, q->cb_out, 3*cb_len+12);
      }

      /* Turbo Decoding with CRC-based early stopping */
      q->nof_iterations = 0; 
      uint32_t len_crc; 
      uint8_t *cb_in_ptr; 
      srslte_crc_t *crc_ptr; 
      early_stop = false; 

      srslte_tdec_reset(&q->decoder, cb_len);
            
      do {
        srslte_tdec_iteration(&q->decoder, (float*) q->cb_out, cb_len); 
        q->nof_iterations++;
        
        if (cb_segm->C > 1) {
          len_crc = cb_len; 
          cb_in_ptr = q->cb_in; 
          crc_ptr = &q->crc_cb; 
        } else {
          len_crc = cb_segm->tbs+24; 
          cb_in_ptr = &q->cb_in[F];
          crc_ptr = &q->crc_tb; 
        }

        srslte_tdec_decision(&q->decoder, q->cb_in, cb_len);
  
        /* Check Codeblock CRC and stop early if incorrect */
        if (!srslte_crc_checksum(crc_ptr, cb_in_ptr, len_crc)) {
          early_stop = true;           
        }
        
      } while (q->nof_iterations < SRSLTE_PDSCH_MAX_TDEC_ITERS && !early_stop);
      q->average_nof_iterations = SRSLTE_VEC_EMA((float) q->nof_iterations, q->average_nof_iterations, 0.2);

      if (SRSLTE_VERBOSE_ISDEBUG()) {
        DEBUG("CB#%d IN: ", i);
        srslte_vec_fprint_b(stdout, q->cb_in, cb_len);
      }
            
      // If CB CRC is not correct, early_stop will be false and wont continue with rest of CBs

      /* Copy data to another buffer, removing the Codeblock CRC */
      if (i < cb_segm->C - 1) {
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
      par_rx = srslte_crc_checksum(&q->crc_tb, data, cb_segm->tbs);

      // check parity bits
      par_tx = srslte_bit_unpack(&p_parity, 24);

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

int srslte_dlsch_decode(srslte_sch_t *q, srslte_pdsch_cfg_t *cfg, srslte_softbuffer_rx_t *softbuffer, 
                        float *e_bits, uint8_t *data) 
{
  return decode_tb(q,                    
                   softbuffer, &cfg->cb_segm, 
                   cfg->grant.Qm, cfg->rv, cfg->grant.nof_bits, 
                   e_bits, data);
}

int srslte_dlsch_encode(srslte_sch_t *q, srslte_pdsch_cfg_t *cfg, srslte_softbuffer_tx_t *softbuffer,
                        uint8_t *data, uint8_t *e_bits) 
{
  return encode_tb(q, 
                   softbuffer, &cfg->cb_segm, 
                   cfg->grant.Qm, cfg->rv, cfg->grant.nof_bits, 
                   data, e_bits);
}

int srslte_ulsch_decode(srslte_sch_t *q, srslte_pusch_cfg_t *cfg, srslte_softbuffer_rx_t *softbuffer,
                        float *e_bits, uint8_t *data) 
{
  return decode_tb(q,                    
                   softbuffer, &cfg->cb_segm, 
                   cfg->grant.Qm, cfg->rv, cfg->grant.nof_bits, 
                   e_bits, data);
}


/* UL-SCH channel interleaver according to 5.5.2.8 of 36.212 */
void ulsch_interleave(uint8_t *g_bits, uint32_t Qm, uint32_t H_prime_total, uint32_t N_pusch_symbs, uint8_t *q_bits) 
{
  
  uint32_t rows = H_prime_total/N_pusch_symbs;
  uint32_t cols = N_pusch_symbs;
  
  uint32_t idx = 0;
  for(uint32_t j=0; j<rows; j++) {        
    for(uint32_t i=0; i<cols; i++) {
      for(uint32_t k=0; k<Qm; k++) {
        if (q_bits[j*Qm + i*rows*Qm + k] >= 10) {
          q_bits[j*Qm + i*rows*Qm + k] -= 10;
        } else {
          q_bits[j*Qm + i*rows*Qm + k] = g_bits[idx];                                
          idx++;                  
        }
      }
    }
  }
  
}

int srslte_ulsch_encode(srslte_sch_t *q, srslte_pusch_cfg_t *cfg, srslte_softbuffer_tx_t *softbuffer,
                        uint8_t *data, uint8_t *g_bits, uint8_t *q_bits) 
{
  srslte_uci_data_t uci_data; 
  bzero(&uci_data, sizeof(srslte_uci_data_t));
  return srslte_ulsch_uci_encode(q, cfg, softbuffer, data, uci_data, g_bits, q_bits);
}

int srslte_ulsch_uci_encode(srslte_sch_t *q, 
                            srslte_pusch_cfg_t *cfg, srslte_softbuffer_tx_t *softbuffer,
                            uint8_t *data, srslte_uci_data_t uci_data, 
                            uint8_t *g_bits, uint8_t *q_bits) 
{
  int ret; 
   
  uint32_t e_offset = 0;
  uint32_t Q_prime_cqi = 0; 
  uint32_t Q_prime_ack = 0;
  uint32_t Q_prime_ri = 0;

  uint32_t nb_q = cfg->grant.nof_bits; 
  uint32_t Qm = cfg->grant.Qm; 
  
  bzero(q_bits, sizeof(uint8_t) * nb_q);

  // Encode RI
  if (uci_data.uci_ri_len > 0) {
    float beta = beta_ri_offset[uci_data.I_offset_ri]; 
    if (cfg->cb_segm.tbs == 0) {
        beta /= beta_cqi_offset[uci_data.I_offset_cqi];
    }
    ret = srslte_uci_encode_ri(cfg, uci_data.uci_ri, uci_data.uci_cqi_len, beta, nb_q/Qm, q_bits);
    if (ret < 0) {
      return ret; 
    }
    Q_prime_ri = (uint32_t) ret; 
  }

  // Encode CQI
  if (uci_data.uci_cqi_len > 0) {
    ret = srslte_uci_encode_cqi_pusch(&q->uci_cqi, cfg, 
                                      uci_data.uci_cqi, uci_data.uci_cqi_len, 
                                      beta_cqi_offset[uci_data.I_offset_cqi], 
                                      Q_prime_ri, g_bits);
    if (ret < 0) {
      return ret; 
    }
    Q_prime_cqi = (uint32_t) ret; 
  }
  
  e_offset += Q_prime_cqi*Qm;
  
  // Encode UL-SCH
  if (cfg->cb_segm.tbs > 0) {
    uint32_t G = nb_q/Qm - Q_prime_ri - Q_prime_cqi;     
    ret = encode_tb(q, softbuffer, &cfg->cb_segm, 
                    Qm, cfg->rv, G*Qm, 
                    data, &g_bits[e_offset]);
    if (ret) {
      return ret; 
    }    
  } 
    
  // Interleave UL-SCH (and RI and CQI)
  ulsch_interleave(g_bits, Qm, nb_q/Qm, cfg->grant.nof_symb, q_bits);
  
   // Encode (and interleave) ACK
  if (uci_data.uci_ack_len > 0) {
    float beta = beta_harq_offset[uci_data.I_offset_ack]; 
    if (cfg->cb_segm.tbs == 0) {
        beta /= beta_cqi_offset[uci_data.I_offset_cqi];
    }
    ret = srslte_uci_encode_ack(cfg, uci_data.uci_ack, uci_data.uci_cqi_len, beta, nb_q/Qm, q_bits);
    if (ret < 0) {
      return ret; 
    }
    Q_prime_ack = (uint32_t) ret; 
  }
    
  INFO("Q_prime_ack=%d, Q_prime_cqi=%d, Q_prime_ri=%d\n",Q_prime_ack, Q_prime_cqi, Q_prime_ri);  

  return SRSLTE_SUCCESS;
}

