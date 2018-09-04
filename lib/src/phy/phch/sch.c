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
#include <srslte/phy/phch/sch.h>
#include "srslte/phy/phch/pdsch.h"
#include "srslte/phy/utils/bit.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"

#define SRSLTE_PDSCH_MAX_TDEC_ITERS         10

#ifdef LV_HAVE_SSE
#include <immintrin.h>
#endif /* LV_HAVE_SSE */


/* 36.213 Table 8.6.3-1: Mapping of HARQ-ACK offset values and the index signalled by higher layers */
float beta_harq_offset[16] = {2.0, 2.5, 3.125, 4.0, 5.0, 6.250, 8.0, 10.0, 
                           12.625, 15.875, 20.0, 31.0, 50.0, 80.0, 126.0, -1.0};
                                  
/* 36.213 Table 8.6.3-2: Mapping of RI offset values and the index signalled by higher layers */
float beta_ri_offset[16] = {1.25, 1.625, 2.0, 2.5, 3.125, 4.0, 5.0, 6.25, 8.0, 10.0,
                           12.625, 15.875, 20.0, -1.0, -1.0, -1.0};

/* 36.213 Table 8.6.3-3: Mapping of CQI offset values and the index signalled by higher layers */
float beta_cqi_offset[16] = {-1.0, -1.0, 1.125, 1.25, 1.375, 1.625, 1.750, 2.0, 2.25, 2.5, 2.875, 
                             3.125, 3.5, 4.0, 5.0, 6.25};


float srslte_sch_beta_cqi(uint32_t I_cqi) {
  if (I_cqi < 16) {
    return beta_cqi_offset[I_cqi];
  } else {
    return 0;
  }
}
                             
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
    ret = SRSLTE_ERROR;
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

    q->max_iterations = SRSLTE_PDSCH_MAX_TDEC_ITERS;
    
    srslte_rm_turbo_gentables();
    
    // Allocate int16 for reception (LLRs)
    q->cb_in = srslte_vec_malloc(sizeof(uint8_t) * (SRSLTE_TCOD_MAX_LEN_CB+8)/8);
    if (!q->cb_in) {
      goto clean;
    }
    
    q->parity_bits = srslte_vec_malloc(sizeof(uint8_t) * (3 * SRSLTE_TCOD_MAX_LEN_CB + 16) / 8);
    if (!q->parity_bits) {
      goto clean;
    }  
    q->temp_g_bits = srslte_vec_malloc(sizeof(uint8_t)*SRSLTE_MAX_PRB*12*12*12);
    if (!q->temp_g_bits) {
      goto clean; 
    }
    bzero(q->temp_g_bits, SRSLTE_MAX_PRB*12*12*12);
    q->ul_interleaver = srslte_vec_malloc(sizeof(uint32_t)*SRSLTE_MAX_PRB*12*12*12);
    if (!q->ul_interleaver) {
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
  srslte_rm_turbo_free_tables();

  if (q->cb_in) {
    free(q->cb_in);
  }
  if (q->parity_bits) {
    free(q->parity_bits);
  }
  if (q->temp_g_bits) {
    free(q->temp_g_bits);
  }
  if (q->ul_interleaver) {
    free(q->ul_interleaver);
  }
  srslte_tdec_free(&q->decoder);
  srslte_tcod_free(&q->encoder);
  srslte_uci_cqi_free(&q->uci_cqi);
  bzero(q, sizeof(srslte_sch_t));
}

void srslte_sch_set_max_noi(srslte_sch_t *q, uint32_t max_iterations) {
  q->max_iterations = max_iterations;
}

uint32_t srslte_sch_last_noi(srslte_sch_t *q) {
  return q->nof_iterations;
}

/* Encode a transport block according to 36.212 5.3.2
 *
 */
static int encode_tb_off(srslte_sch_t *q, 
                     srslte_softbuffer_tx_t *softbuffer, srslte_cbsegm_t *cb_segm, 
                     uint32_t Qm, uint32_t rv, uint32_t nof_e_bits,  
                     uint8_t *data, uint8_t *e_bits, uint32_t w_offset) 
{
  uint32_t i;
  uint32_t cb_len=0, rp=0, wp=0, rlen=0, n_e=0;
  int ret = SRSLTE_ERROR_INVALID_INPUTS; 
  
  if (q            != NULL &&
      e_bits       != NULL &&
      cb_segm      != NULL &&
      softbuffer  != NULL)
  {
  
    if (cb_segm->F) {
      fprintf(stderr, "Error filler bits are not supported. Use standard TBS\n");
      return SRSLTE_ERROR;       
    }

    if (cb_segm->C > softbuffer->max_cb) {
      fprintf(stderr, "Error number of CB (%d) exceeds soft buffer size (%d CBs)\n", cb_segm->C, softbuffer->max_cb);
      return -1; 
    }

    uint32_t Gp = nof_e_bits / Qm;
    
    uint32_t gamma = Gp;
    if (cb_segm->C > 0) {
      gamma = Gp%cb_segm->C;
    }

    /* Reset TB CRC */
    srslte_crc_set_init(&q->crc_tb, 0);

    wp = 0;
    rp = 0;
    for (i = 0; i < cb_segm->C; i++) {

      uint32_t cblen_idx; 
      /* Get read lengths */
      if (i < cb_segm->C2) {
        cb_len = cb_segm->K2;
        cblen_idx = cb_segm->K2_idx;
      } else {
        cb_len = cb_segm->K1;
        cblen_idx = cb_segm->K1_idx;
      }
      if (cb_segm->C > 1) {
        rlen = cb_len - 24;
      } else {
        rlen = cb_len;
      }
      if (i <= cb_segm->C - gamma - 1) {
        n_e = Qm * (Gp/cb_segm->C);
      } else {
        n_e = Qm * ((uint32_t) ceilf((float) Gp/cb_segm->C));
      }

      INFO("CB#%d: cb_len: %d, rlen: %d, wp: %d, rp: %d, E: %d\n", i,
          cb_len, rlen, wp, rp, n_e);

      if (data) {
        bool last_cb = false;

        /* Copy data to another buffer, making space for the Codeblock CRC */
        if (i < cb_segm->C - 1) {
          // Copy data 
          memcpy(q->cb_in, &data[rp/8], rlen * sizeof(uint8_t)/8);
        } else {
          INFO("Last CB, appending parity: %d from %d and 24 to %d\n",
              rlen - 24, rp, rlen - 24);
          
          /* Append Transport Block parity bits to the last CB */
          memcpy(q->cb_in, &data[rp/8], (rlen - 24) * sizeof(uint8_t)/8);
          last_cb = true;
        }

        /* Turbo Encoding
         * If Codeblock CRC is required it is given the CRC instance pointer, otherwise CRC pointer shall be NULL
         */
        srslte_tcod_encode_lut(&q->encoder,
                               &q->crc_tb,
                               (cb_segm->C > 1) ? &q->crc_cb : NULL,
                               q->cb_in,
                               q->parity_bits,
                               cblen_idx,
                               last_cb);
      }
      DEBUG("RM cblen_idx=%d, n_e=%d, wp=%d, nof_e_bits=%d\n",cblen_idx, n_e, wp, nof_e_bits);
      
      /* Rate matching */
      if (srslte_rm_turbo_tx_lut(softbuffer->buffer_b[i], q->cb_in, q->parity_bits, 
        &e_bits[(wp+w_offset)/8], cblen_idx, n_e, (wp+w_offset)%8, rv))
      {
        fprintf(stderr, "Error in rate matching\n");
        return SRSLTE_ERROR;
      }
      
      /* Set read/write pointers */
      rp += rlen;
      wp += n_e;
    }
    
    INFO("END CB#%d: wp: %d, rp: %d\n", i, wp, rp);
    ret = SRSLTE_SUCCESS;      
  } 
  return ret; 
}


static int encode_tb(srslte_sch_t *q, 
                     srslte_softbuffer_tx_t *soft_buffer, srslte_cbsegm_t *cb_segm, 
                     uint32_t Qm, uint32_t rv, uint32_t nof_e_bits,  
                     uint8_t *data, uint8_t *e_bits) 
{
  return encode_tb_off(q, soft_buffer, cb_segm, Qm, rv, nof_e_bits, data, e_bits, 0);
}

bool decode_tb_cb(srslte_sch_t *q, 
                     srslte_softbuffer_rx_t *softbuffer, srslte_cbsegm_t *cb_segm, 
                     uint32_t Qm, uint32_t rv, uint32_t nof_e_bits, 
                     void *e_bits, uint8_t *data)
{

  int8_t *e_bits_b  = e_bits;
  int16_t *e_bits_s = e_bits;

  if (cb_segm->C > SRSLTE_MAX_CODEBLOCKS) {
    fprintf(stderr, "Error SRSLTE_MAX_CODEBLOCKS=%d\n", SRSLTE_MAX_CODEBLOCKS);
    return false;
  }

  q->nof_iterations = 0;

  for (int cb_idx=0;cb_idx<cb_segm->C;cb_idx++)
  {
    /* Do not process blocks with CRC Ok */
    if (softbuffer->cb_crc[cb_idx] == false) {

      uint32_t cb_len     = cb_idx<cb_segm->C1?cb_segm->K1:cb_segm->K2;
      uint32_t cb_len_idx = cb_idx<cb_segm->C1?cb_segm->K1_idx:cb_segm->K2_idx;

      uint32_t rlen       = cb_segm->C==1?cb_len:(cb_len-24);
      uint32_t Gp         = nof_e_bits / Qm;
      uint32_t gamma      = cb_segm->C>0?Gp%cb_segm->C:Gp;
      uint32_t n_e        = Qm * (Gp/cb_segm->C);

      uint32_t rp   = cb_idx*n_e;
      uint32_t n_e2 = n_e;

      if (cb_idx > cb_segm->C - gamma) {
        n_e2 = n_e+Qm;
        rp   = (cb_segm->C - gamma)*n_e + (cb_idx-(cb_segm->C - gamma))*n_e2;
      }

      if (q->llr_is_8bit) {
        if (srslte_rm_turbo_rx_lut_8bit(&e_bits_b[rp], (int8_t*) softbuffer->buffer_f[cb_idx], n_e2, cb_len_idx, rv)) {
          fprintf(stderr, "Error in rate matching\n");
          return SRSLTE_ERROR;
        }
      } else {
        if (srslte_rm_turbo_rx_lut(&e_bits_s[rp], softbuffer->buffer_f[cb_idx], n_e2, cb_len_idx, rv)) {
          fprintf(stderr, "Error in rate matching\n");
          return SRSLTE_ERROR;
        }
      }

      srslte_tdec_new_cb(&q->decoder, cb_len);

      // Run iterations and use CRC for early stopping
      bool early_stop = false;
      uint32_t cb_noi = 0;
      do {
        if (q->llr_is_8bit) {
          srslte_tdec_iteration_8bit(&q->decoder, (int8_t*) softbuffer->buffer_f[cb_idx], &data[cb_idx*rlen/8]);
        } else {
          srslte_tdec_iteration(&q->decoder, softbuffer->buffer_f[cb_idx], &data[cb_idx*rlen/8]);
        }
        q->nof_iterations++;
        cb_noi++;

        uint32_t len_crc;
        srslte_crc_t *crc_ptr;

        if (cb_segm->C > 1) {
          len_crc = cb_len;
          crc_ptr = &q->crc_cb;
        } else {
          len_crc = cb_segm->tbs+24;
          crc_ptr = &q->crc_tb;
        }

        // CRC is OK
        if (!srslte_crc_checksum_byte(crc_ptr, &data[cb_idx*rlen/8], len_crc)) {

          softbuffer->cb_crc[cb_idx] = true;
          early_stop = true;

          // CRC is error and exceeded maximum iterations for this CB.
          // Early stop the whole transport block.
        }

      } while (cb_noi < q->max_iterations && !early_stop);

      INFO("CB %d: rp=%d, n_e=%d, cb_len=%d, CRC=%s, rlen=%d, iterations=%d/%d\n",
           cb_idx, rp, n_e2, cb_len, early_stop?"OK":"KO", rlen, cb_noi, q->max_iterations);

    } else {
      // Copy decoded data from previous transmissions
      uint32_t cb_len     = cb_idx<cb_segm->C1?cb_segm->K1:cb_segm->K2;
      uint32_t rlen       = cb_segm->C==1?cb_len:(cb_len-24);
      memcpy(&data[cb_idx*rlen/8], softbuffer->data[cb_idx], rlen/8 * sizeof(uint8_t));
    }
  }

  softbuffer->tb_crc = true;
  for (int i = 0; i < cb_segm->C && softbuffer->tb_crc; i++) {
    /* If one CB failed return false */
    softbuffer->tb_crc = softbuffer->cb_crc[i];
  }
  // If TB CRC failed, save correct CB for next retransmission
  if (!softbuffer->tb_crc) {
    for (int i = 0; i < cb_segm->C; i++) {
      if (softbuffer->cb_crc[i]) {
        uint32_t cb_len     = i<cb_segm->C1?cb_segm->K1:cb_segm->K2;
        uint32_t rlen       = cb_segm->C==1?cb_len:(cb_len-24);
        memcpy(softbuffer->data[i], &data[i * rlen / 8], rlen/8 * sizeof(uint8_t));
      }
    }
  }

  q->nof_iterations /= cb_segm->C;
  return softbuffer->tb_crc;
}

/**
 * Decode a transport block according to 36.212 5.3.2
 *
 * @param[in] q
 * @param[inout] softbuffer Initialized softbuffer
 * @param[in] cb_segm Code block segmentation parameters
 * @param[in] e_bits Input transport block
 * @param[in] Qm Modulation type
 * @param[in] rv Redundancy Version. Indicates which part of FEC bits is in input buffer
 * @param[out] softbuffer Initialized output softbuffer
 * @param[out] data Decoded transport block
 * @return negative if error in parameters or CRC error in decoding
 */
static int decode_tb(srslte_sch_t *q, 
                     srslte_softbuffer_rx_t *softbuffer, srslte_cbsegm_t *cb_segm, 
                     uint32_t Qm, uint32_t rv, uint32_t nof_e_bits, 
                     int16_t *e_bits, uint8_t *data) 
{
  
  if (q            != NULL && 
      data         != NULL &&       
      softbuffer   != NULL &&
      e_bits       != NULL &&
      cb_segm      != NULL)
  {
    
    if (cb_segm->tbs == 0 || cb_segm->C == 0) {
      return SRSLTE_SUCCESS;
    }
    
    if (cb_segm->F) {
      fprintf(stderr, "Error filler bits are not supported. Use standard TBS\n");
      return SRSLTE_ERROR_INVALID_INPUTS;
    }

    if (cb_segm->C > softbuffer->max_cb) {
      fprintf(stderr, "Error number of CB (%d) exceeds soft buffer size (%d CBs)\n", cb_segm->C, softbuffer->max_cb);
      return SRSLTE_ERROR_INVALID_INPUTS;
    }
        
    bool crc_ok = true; 

    data[cb_segm->tbs/8+0] = 0; 
    data[cb_segm->tbs/8+1] = 0; 
    data[cb_segm->tbs/8+2] = 0; 
    
    // Process Codeblocks
    crc_ok = decode_tb_cb(q, softbuffer, cb_segm, Qm, rv, nof_e_bits, e_bits, data);

    if (crc_ok) {

      uint32_t par_rx = 0, par_tx = 0;
  
      // Compute transport block CRC
      par_rx = srslte_crc_checksum_byte(&q->crc_tb, data, cb_segm->tbs);

      // check parity bits
      par_tx = ((uint32_t) data[cb_segm->tbs/8+0])<<16  | 
               ((uint32_t) data[cb_segm->tbs/8+1])<<8   | 
               ((uint32_t) data[cb_segm->tbs/8+2]);

      if (par_rx == par_tx && par_rx) {
        INFO("TB decoded OK\n");
        return SRSLTE_SUCCESS;
      } else {
        INFO("Error in TB parity: par_tx=0x%x, par_rx=0x%x\n", par_tx, par_rx);
        return SRSLTE_ERROR;
      }
    } else {
      return SRSLTE_ERROR; 
    }        
  } else {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }
}

int srslte_dlsch_decode(srslte_sch_t *q, srslte_pdsch_cfg_t *cfg, srslte_softbuffer_rx_t *softbuffer,
                        int16_t *e_bits, uint8_t *data) {
  return srslte_dlsch_decode2(q, cfg, softbuffer, e_bits, data, 0);
}


int srslte_dlsch_decode2(srslte_sch_t *q, srslte_pdsch_cfg_t *cfg, srslte_softbuffer_rx_t *softbuffer,
                         int16_t *e_bits, uint8_t *data, int tb_idx) {
  uint32_t Nl = 1;

  if (cfg->nof_layers != SRSLTE_RA_DL_GRANT_NOF_TB(&cfg->grant)) {
    Nl = 2;
  }

  return decode_tb(q, softbuffer, &cfg->cb_segm[tb_idx],
                   cfg->grant.Qm[tb_idx] * Nl, cfg->rv[tb_idx], cfg->nbits[tb_idx].nof_bits,
                   e_bits, data);
}

/**
 * Encode transport block. Segments into code blocks, adds channel coding, and does rate matching.
 *
 * @param[in] q Initialized 
 * @param[in] cfg Encoding parameters
 * @param[inout] softbuffer Initialized softbuffer
 * @param[in] data Byte array of data. Size is implicit in cfg->cb_segm
 * @param e_bits
 * @return Error code
 */
int srslte_dlsch_encode(srslte_sch_t *q, srslte_pdsch_cfg_t *cfg, srslte_softbuffer_tx_t *softbuffer,
                        uint8_t *data, uint8_t *e_bits) 
{
  return srslte_dlsch_encode2(q, cfg, softbuffer, data, e_bits, 0);
}

int srslte_dlsch_encode2(srslte_sch_t *q, srslte_pdsch_cfg_t *cfg, srslte_softbuffer_tx_t *softbuffer,
                              uint8_t *data, uint8_t *e_bits, int tb_idx) {
  uint32_t Nl = 1;

  if (cfg->nof_layers != SRSLTE_RA_DL_GRANT_NOF_TB(&cfg->grant)) {
    Nl = 2;
  }

  return encode_tb(q, softbuffer, &cfg->cb_segm[tb_idx], cfg->grant.Qm[tb_idx]*Nl, cfg->rv[tb_idx],
                   cfg->nbits[tb_idx].nof_bits, data, e_bits);
}

/* Compute the interleaving function on-the-fly, because it depends on number of RI bits
 * Profiling show that the computation of this matrix is neglegible. 
 */
static void ulsch_interleave_gen(uint32_t H_prime_total, uint32_t N_pusch_symbs, uint32_t Qm,
                                 uint8_t *ri_present, uint32_t *interleaver_lut)
{
  uint32_t rows = H_prime_total/N_pusch_symbs;
  uint32_t cols = N_pusch_symbs;
  uint32_t idx = 0;
  for(uint32_t j=0; j<rows; j++) {        
    for(uint32_t i=0; i<cols; i++) {
      for(uint32_t k=0; k<Qm; k++) {
        if (ri_present[j*Qm + i*rows*Qm + k]) {
          interleaver_lut[j*Qm + i*rows*Qm + k] = 0; 
        } else {
          interleaver_lut[j*Qm + i*rows*Qm + k] = idx;
          idx++;                  
        }
      }
    }
  }
}

static void ulsch_interleave_qm2(const uint8_t *g_bits, uint32_t rows, uint32_t cols, uint8_t *q_bits, uint32_t ri_min_row, const uint8_t *ri_present) {
  uint32_t bit_read_idx = 0;

  for (uint32_t j = 0; j < ri_min_row; j++) {
    for (uint32_t i = 0; i < cols; i++) {
      uint32_t k = (i * rows + j) * 2;

      uint32_t read_byte_idx = bit_read_idx / 8;
      uint32_t read_bit_idx = bit_read_idx % 8;
      uint32_t write_byte_idx = k / 8;
      uint32_t write_bit_idx = k % 8;
      uint8_t w = (g_bits[read_byte_idx] >> (6 - read_bit_idx)) & (uint8_t) 0x03;
      q_bits[write_byte_idx] |= w << (6 - write_bit_idx);

      bit_read_idx += 2;
    }
  }

  for (uint32_t j = ri_min_row; j < rows; j++) {
    for (uint32_t i = 0; i < cols; i++) {
      uint32_t k = (i * rows + j) * 2;

      if (ri_present[k]) {
        /* do nothing */
      } else {
        uint32_t read_byte_idx = bit_read_idx / 8;
        uint32_t read_bit_idx = bit_read_idx % 8;
        uint32_t write_byte_idx = k / 8;
        uint32_t write_bit_idx = k % 8;
        uint8_t w = (g_bits[read_byte_idx] >> (6 - read_bit_idx)) & (uint8_t) 0x03;
        q_bits[write_byte_idx] |= w << (6 - write_bit_idx);

        bit_read_idx += 2;
      }
    }
  }
}

static void ulsch_interleave_qm4(uint8_t *g_bits, uint32_t rows, uint32_t cols, uint8_t *q_bits, uint32_t ri_min_row, const uint8_t *ri_present) {
  uint32_t bit_read_idx = 0;
  
  for (uint32_t j = 0; j < ri_min_row; j++) {
    int32_t i = 0;

#ifndef LV_HAVE_SSE
    __m128i _counter = _mm_slli_epi32(_mm_add_epi32(_mm_mullo_epi32(_counter0,_rows),_mm_set1_epi32(j)), 2);
    uint8_t *_g_bits = &g_bits[bit_read_idx/8];

    /* First bits are aligned to byte */
    if (0 == (bit_read_idx & 0x3)) {
      for (; i < (cols - 3); i += 4) {

        uint8_t w1 = *(_g_bits++);
        uint8_t w2 = *(_g_bits++);

        __m128i _write_byte_idx = _mm_srli_epi32(_counter, 3);
        __m128i _write_bit_idx = _mm_and_si128(_counter, _7);
        __m128i _write_shift = _mm_sub_epi32(_4, _write_bit_idx);

        q_bits[_mm_extract_epi32(_write_byte_idx, 0)] |= (w1 >> 0x4) << _mm_extract_epi32(_write_shift, 0);
        q_bits[_mm_extract_epi32(_write_byte_idx, 1)] |= (w1 & 0xf) << _mm_extract_epi32(_write_shift, 1);
        q_bits[_mm_extract_epi32(_write_byte_idx, 2)] |= (w2 >> 0x4) << _mm_extract_epi32(_write_shift, 2);
        q_bits[_mm_extract_epi32(_write_byte_idx, 3)] |= (w2 & 0xf) << _mm_extract_epi32(_write_shift, 3);
        _counter = _mm_add_epi32(_counter, _inc);
      }
    } else {
      for (; i < (cols - 3); i += 4) {
        __m128i _write_byte_idx = _mm_srli_epi32(_counter, 3);
        __m128i _write_bit_idx = _mm_and_si128(_counter, _7);
        __m128i _write_shift = _mm_sub_epi32(_4, _write_bit_idx);

        uint8_t w1 = *(_g_bits);
        uint8_t w2 = *(_g_bits++);
        uint8_t w3 = *(_g_bits++);
        q_bits[_mm_extract_epi32(_write_byte_idx, 0)] |= (w1 & 0xf) << _mm_extract_epi32(_write_shift, 0);
        q_bits[_mm_extract_epi32(_write_byte_idx, 1)] |= (w2 >> 0x4) << _mm_extract_epi32(_write_shift, 1);
        q_bits[_mm_extract_epi32(_write_byte_idx, 2)] |= (w2 & 0xf) << _mm_extract_epi32(_write_shift, 2);
        q_bits[_mm_extract_epi32(_write_byte_idx, 3)] |= (w3 >> 0x4) << _mm_extract_epi32(_write_shift, 3);

        _counter = _mm_add_epi32(_counter, _inc);
      }
    }
    bit_read_idx += i * 4;
#endif /* LV_HAVE_SSE */

    /* Spare bits */
    for (; i < cols; i++) {
      uint32_t k = (i * rows + j) * 4;

      uint32_t read_byte_idx = bit_read_idx / 8;
      uint32_t read_bit_idx = bit_read_idx % 8;
      uint32_t write_byte_idx = k / 8;
      uint32_t write_bit_idx = k % 8;
      uint8_t w = (g_bits[read_byte_idx] >> (4 - read_bit_idx)) & (uint8_t) 0x0f;
      q_bits[write_byte_idx] |= w << (4 - write_bit_idx);

      bit_read_idx += 4;
    }
  }

  /* Do rows containing RI */
  for (uint32_t j = ri_min_row; j < rows; j++) {
    for (uint32_t i = 0; i < cols; i++) {
      uint32_t k = (i * rows + j) * 4;

      if (ri_present[k]) {
        /* do nothing */
      } else {
        uint32_t read_byte_idx = bit_read_idx / 8;
        uint32_t read_bit_idx = bit_read_idx % 8;
        uint32_t write_byte_idx = k / 8;
        uint32_t write_bit_idx = k % 8;
        uint8_t w = (g_bits[read_byte_idx] >> (4 - read_bit_idx)) & (uint8_t) 0x0f;
        q_bits[write_byte_idx] |= w << (4 - write_bit_idx);

        bit_read_idx += 4;
      }
    }

  }
}

static void ulsch_interleave_qm6(const uint8_t *g_bits,
                                 uint32_t rows,
                                 uint32_t cols,
                                 uint8_t *q_bits,
                                 uint32_t ri_min_row,
                                 const uint8_t *ri_present) {
  uint32_t bit_read_idx = 0;

  for (uint32_t j = 0; j < ri_min_row; j++) {
    for (uint32_t i = 0; i < cols; i++) {
      uint32_t k = (i * rows + j) * 6;

      uint32_t read_byte_idx = bit_read_idx / 8;
      uint32_t read_bit_idx = bit_read_idx % 8;
      uint32_t write_byte_idx = k / 8;
      uint32_t write_bit_idx = k % 8;
      uint8_t w;

      switch (read_bit_idx) {
        case 0:
          w = g_bits[read_byte_idx] >> 2;
          break;
        case 2:
          w = g_bits[read_byte_idx] & (uint8_t) 0x3f;
          break;
        case 4:
          w = ((g_bits[read_byte_idx] << 2) | (g_bits[read_byte_idx + 1] >> 6)) & (uint8_t) 0x3f;
          break;
        case 6:
          w = ((g_bits[read_byte_idx] << 4) | (g_bits[read_byte_idx + 1] >> 4)) & (uint8_t) 0x3f;
          break;
        default:
          w = 0;
      }

      switch (write_bit_idx) {
        case 0:
          q_bits[write_byte_idx] |= w << 2;
          break;
        case 2:
          q_bits[write_byte_idx] |= w;
          break;
        case 4:
          q_bits[write_byte_idx] |= w >> 2;
          q_bits[write_byte_idx + 1] |= w << 6;
          break;
        case 6:
          q_bits[write_byte_idx] |= w >> 4;
          q_bits[write_byte_idx + 1] |= w << 4;
          break;
        default:
          /* Do nothing */;
      }

      bit_read_idx += 6;
    }
  }

  for (uint32_t j = ri_min_row; j < rows; j++) {
    for (uint32_t i = 0; i < cols; i++) {
      uint32_t k = (i * rows + j) * 6;

      if (ri_present[k]) {
        /* do nothing */
      } else {
        uint32_t read_byte_idx = bit_read_idx / 8;
        uint32_t read_bit_idx = bit_read_idx % 8;
        uint32_t write_byte_idx = k / 8;
        uint32_t write_bit_idx = k % 8;
        uint8_t w;

        switch (read_bit_idx) {
          case 0:
            w = g_bits[read_byte_idx] >> 2;
            break;
          case 2:
            w = g_bits[read_byte_idx] & (uint8_t) 0x3f;
            break;
          case 4:
            w = ((g_bits[read_byte_idx] << 2) | (g_bits[read_byte_idx + 1] >> 6)) & (uint8_t) 0x3f;
            break;
          case 6:
            w = ((g_bits[read_byte_idx] << 4) | (g_bits[read_byte_idx + 1] >> 4)) & (uint8_t) 0x3f;
            break;
          default:
            w = 0;
        }

        switch (write_bit_idx) {
          case 0:
            q_bits[write_byte_idx] |= w << 2;
            break;
          case 2:
            q_bits[write_byte_idx] |= w;
            break;
          case 4:
            q_bits[write_byte_idx] |= w >> 2;
            q_bits[write_byte_idx + 1] |= w << 6;
            break;
          case 6:
            q_bits[write_byte_idx] |= w >> 4;
            q_bits[write_byte_idx + 1] |= w << 4;
            break;
          default:
            /* Do nothing */;
        }

        bit_read_idx += 6;
      }
    }
  }
}

/* UL-SCH channel interleaver according to 5.2.2.8 of 36.212 */
void ulsch_interleave(uint8_t *g_bits, uint32_t Qm, uint32_t H_prime_total, 
                      uint32_t N_pusch_symbs, uint8_t *q_bits, srslte_uci_bit_t *ri_bits, uint32_t nof_ri_bits, 
                      uint8_t *ri_present, uint32_t *inteleaver_lut)
{

  const uint32_t nof_bits = H_prime_total * Qm;
  uint32_t rows = H_prime_total / N_pusch_symbs;
  uint32_t cols = N_pusch_symbs;
  uint32_t ri_min_row = rows;

  // Prepare ri_bits for fast search using temp_buffer
  if (nof_ri_bits > 0) {
    for (uint32_t i=0;i<nof_ri_bits;i++) {
      uint32_t ri_row = (ri_bits[i].position / Qm) % rows;

      if (ri_row < ri_min_row) {
        ri_min_row = ri_row;
      }

      ri_present[ri_bits[i].position] = 1;
    }
  }

#if 1
  bzero(q_bits, nof_bits / 8);
  switch (Qm) {
    case 2:
      ulsch_interleave_qm2(g_bits, rows, cols, q_bits, ri_min_row, ri_present);
      break;
    case 4:
      ulsch_interleave_qm4(g_bits, rows, cols, q_bits, ri_min_row, ri_present);
      break;
    case 6:
      ulsch_interleave_qm6(g_bits, rows, cols, q_bits, ri_min_row, ri_present);
      break;
    default:
      /* This line should never be reached */
      fprintf(stderr, "Wrong Qm (%d)\n", Qm);
  }
#else
  // Genearate interleaver table and interleave bits
  ulsch_interleave_gen(H_prime_total, N_pusch_symbs, Qm, ri_present, inteleaver_lut); 
  srslte_bit_interleave_i(g_bits, q_bits, inteleaver_lut, H_prime_total*Qm);
#endif

  // Reset temp_buffer because will be reused next time
  if (nof_ri_bits > 0) {
    for (uint32_t i=0;i<nof_ri_bits;i++) {
      ri_present[ri_bits[i].position] = 0;       
    }
  }
}

/* UL-SCH channel deinterleaver according to 5.2.2.8 of 36.212 */
void ulsch_deinterleave(int16_t *q_bits, uint32_t Qm, uint32_t H_prime_total, 
                        uint32_t N_pusch_symbs, int16_t *g_bits, srslte_uci_bit_t *ri_bits, uint32_t nof_ri_bits, 
                        uint8_t *ri_present, uint32_t *inteleaver_lut)
{     
  // Prepare ri_bits for fast search using temp_buffer
  if (nof_ri_bits > 0) {
    for (uint32_t i=0;i<nof_ri_bits;i++) {
      ri_present[ri_bits[i].position] = 1;       
    }
  }

  // Generate interleaver table and interleave samples 
  ulsch_interleave_gen(H_prime_total, N_pusch_symbs, Qm, ri_present, inteleaver_lut);
  srslte_vec_lut_sis(q_bits, inteleaver_lut, g_bits, H_prime_total*Qm);

  // Reset temp_buffer because will be reused next time
  if (nof_ri_bits > 0) {
    for (uint32_t i=0;i<nof_ri_bits;i++) {
      ri_present[ri_bits[i].position] = 0;       
    }
  }
}

int srslte_ulsch_decode(srslte_sch_t *q, srslte_pusch_cfg_t *cfg, srslte_softbuffer_rx_t *softbuffer,
                            int16_t *q_bits, int16_t *g_bits, uint8_t *data) 
{
  srslte_uci_data_t uci_data; 
  bzero(&uci_data, sizeof(srslte_uci_data_t));
  return srslte_ulsch_uci_decode(q, cfg, softbuffer, q_bits, g_bits, data, &uci_data);
}

/* This is done before scrambling */
int srslte_ulsch_uci_decode_ri_ack(srslte_sch_t *q, srslte_pusch_cfg_t *cfg, srslte_softbuffer_rx_t *softbuffer,
                                   int16_t *q_bits, uint8_t *c_seq, srslte_uci_data_t *uci_data) 
{
  int ret = 0; 

  uint32_t Q_prime_ri = 0;
  uint32_t Q_prime_ack = 0;
  
  uint32_t nb_q = cfg->nbits.nof_bits; 
  uint32_t Qm = cfg->grant.Qm; 

  cfg->last_O_cqi = uci_data->uci_cqi_len;
  
  // Deinterleave and decode HARQ bits
  if (uci_data->uci_ack_len > 0) {
    uint8_t acks[2] = {0, 0};
    float beta = beta_harq_offset[cfg->uci_cfg.I_offset_ack]; 
    if (cfg->cb_segm.tbs == 0) {
        beta /= beta_cqi_offset[cfg->uci_cfg.I_offset_cqi];
    }
    ret = srslte_uci_decode_ack_ri(cfg, q_bits, c_seq, beta, nb_q/Qm, uci_data->uci_cqi_len, q->ack_ri_bits, acks, uci_data->uci_ack_len, false);
    if (ret < 0) {
      return ret; 
    }
    uci_data->uci_ack = acks[0];
    uci_data->uci_ack_2 = acks[1];
    Q_prime_ack = (uint32_t) ret; 

    // Set zeros to HARQ bits
    for (uint32_t i = 0; i < Q_prime_ack * Qm; i++) {
      q_bits[q->ack_ri_bits[i].position] = 0;
    }
  }
        
  // Deinterleave and decode RI bits
  if (uci_data->uci_ri_len > 0) {
    float beta = beta_ri_offset[cfg->uci_cfg.I_offset_ri]; 
    if (cfg->cb_segm.tbs == 0) {
        beta /= beta_cqi_offset[cfg->uci_cfg.I_offset_cqi];
    }
    ret = srslte_uci_decode_ack_ri(cfg, q_bits, c_seq, beta, nb_q/Qm, uci_data->uci_cqi_len, q->ack_ri_bits, &uci_data->uci_ri, uci_data->uci_ri_len, true);
    if (ret < 0) {
      return ret; 
    }
    Q_prime_ri = (uint32_t) ret;     
  }
  
  q->nof_ri_ack_bits = Q_prime_ri; 
  
  return SRSLTE_SUCCESS;
}

int srslte_ulsch_uci_decode(srslte_sch_t *q, srslte_pusch_cfg_t *cfg, srslte_softbuffer_rx_t *softbuffer,
                            int16_t *q_bits, int16_t *g_bits, uint8_t *data, srslte_uci_data_t *uci_data) 
{
  int ret = 0; 
  
  uint32_t Q_prime_ri = q->nof_ri_ack_bits; 
  uint32_t Q_prime_cqi = 0; 
  uint32_t e_offset = 0;

  uint32_t nb_q = cfg->nbits.nof_bits; 
  uint32_t Qm = cfg->grant.Qm; 

  // Deinterleave data and CQI in ULSCH 
  ulsch_deinterleave(q_bits, Qm, nb_q/Qm, cfg->nbits.nof_symb, g_bits, q->ack_ri_bits, Q_prime_ri*Qm, 
                     q->temp_g_bits, q->ul_interleaver);
  
  // Decode CQI (multiplexed at the front of ULSCH)
  if (uci_data->uci_cqi_len > 0) {
    ret = srslte_uci_decode_cqi_pusch(&q->uci_cqi, cfg, g_bits, 
                                      beta_cqi_offset[cfg->uci_cfg.I_offset_cqi], 
                                      Q_prime_ri, uci_data->uci_cqi_len,
                                      uci_data->uci_cqi, &uci_data->cqi_ack);
    if (ret < 0) {
      return ret; 
    }
    Q_prime_cqi = (uint32_t) ret; 
  }
  
  e_offset += Q_prime_cqi*Qm;
  
  // Decode ULSCH
  if (cfg->cb_segm.tbs > 0) {
    uint32_t G = nb_q/Qm - Q_prime_ri - Q_prime_cqi;     
    ret = decode_tb(q, softbuffer, &cfg->cb_segm, 
                   Qm, cfg->rv, G*Qm, 
                   &g_bits[e_offset], data);
    if (ret) {
      return ret; 
    }
  }
  return SRSLTE_SUCCESS; 
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

  uint32_t nb_q = cfg->nbits.nof_bits; 
  uint32_t Qm = cfg->grant.Qm; 

  if (Qm == 0) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  if (uci_data.uci_ri_len > 0) {
    float beta = beta_ri_offset[cfg->uci_cfg.I_offset_ri]; 
    if (cfg->cb_segm.tbs == 0) {
      beta /= beta_cqi_offset[cfg->uci_cfg.I_offset_cqi];
    }
    uint8_t ri[2] = {uci_data.uci_ri, 0};
    ret = srslte_uci_encode_ack_ri(cfg, ri, uci_data.uci_ri_len, uci_data.uci_cqi_len, beta, nb_q/Qm, q->ack_ri_bits, true);
    if (ret < 0) {
      return ret; 
    }
    Q_prime_ri = (uint32_t) ret; 
  }
  
  // Encode CQI
  cfg->last_O_cqi = uci_data.uci_cqi_len;
  if (uci_data.uci_cqi_len > 0) {
    ret = srslte_uci_encode_cqi_pusch(&q->uci_cqi, cfg, 
                                      uci_data.uci_cqi, uci_data.uci_cqi_len, 
                                      beta_cqi_offset[cfg->uci_cfg.I_offset_cqi], 
                                      Q_prime_ri, q->temp_g_bits);
    if (ret < 0) {
      return ret; 
    }
    Q_prime_cqi = (uint32_t) ret; 
    srslte_bit_pack_vector(q->temp_g_bits, g_bits, Q_prime_cqi*Qm);
    // Reset the buffer because will be reused in ulsch_interleave
    bzero(q->temp_g_bits, Q_prime_cqi*Qm);
  }
  
  e_offset += Q_prime_cqi*Qm;

  // Encode UL-SCH
  if (cfg->cb_segm.tbs > 0) {
    uint32_t G = nb_q/Qm - Q_prime_ri - Q_prime_cqi;     
    ret = encode_tb_off(q, softbuffer, &cfg->cb_segm, 
                    Qm, cfg->rv, G*Qm, 
                    data, &g_bits[e_offset/8], e_offset%8);
    if (ret) {
      return ret; 
    }    
  } 
  
  // Interleave UL-SCH (and RI and CQI)
  ulsch_interleave(g_bits, Qm, nb_q/Qm, cfg->nbits.nof_symb, q_bits, q->ack_ri_bits, Q_prime_ri*Qm, 
                   q->temp_g_bits, q->ul_interleaver);
  
  // Encode (and interleave) ACK
  if (uci_data.uci_ack_len > 0) {
    uint8_t acks [2] = {uci_data.uci_ack, uci_data.uci_ack_2};
    float beta = beta_harq_offset[cfg->uci_cfg.I_offset_ack]; 
    if (cfg->cb_segm.tbs == 0) {
        beta /= beta_cqi_offset[cfg->uci_cfg.I_offset_cqi];
    }
    ret = srslte_uci_encode_ack_ri(cfg, acks, uci_data.uci_ack_len, uci_data.uci_cqi_len,
                                beta, nb_q / Qm, &q->ack_ri_bits[Q_prime_ri * Qm], false);
    if (ret < 0) {
      return ret; 
    }
    Q_prime_ack = (uint32_t) ret; 
  }
  
  q->nof_ri_ack_bits = (Q_prime_ack+Q_prime_ri)*Qm;
  
  for (uint32_t i=0;i<q->nof_ri_ack_bits;i++) {
    uint32_t p = q->ack_ri_bits[i].position;
    if (p < nb_q) {
      if (q->ack_ri_bits[i].type == UCI_BIT_1) {
        q_bits[p/8] |= (1<<(7-p%8));
      } else {
        q_bits[p/8] &= ~(1<<(7-p%8));
      }
    } else {
      fprintf(stderr, "Invalid RI/ACK bit position %d. Max bits=%d\n", p, nb_q);
    }    
  }
    
  
  INFO("Q_prime_ack=%d, Q_prime_cqi=%d, Q_prime_ri=%d\n",Q_prime_ack, Q_prime_cqi, Q_prime_ri);  

  return SRSLTE_SUCCESS;
}

