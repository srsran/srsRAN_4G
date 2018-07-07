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
#include "srslte/phy/phch/pdsch.h"
#include "srslte/phy/utils/bit.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"

#define SRSLTE_PDSCH_MAX_TDEC_ITERS         4

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
  uint8_t parity[3] = {0, 0, 0};
  uint32_t par;
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

    if (data) {

      /* Compute transport block CRC */
      par = srslte_crc_checksum_byte(&q->crc_tb, data, cb_segm->tbs);

      /* parity bits will be appended later */
      parity[0] = (par&(0xff<<16))>>16;
      parity[1] = (par&(0xff<<8))>>8;
      parity[2] = par&0xff;
    }
    
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

        /* Copy data to another buffer, making space for the Codeblock CRC */
        if (i < cb_segm->C - 1) {
          // Copy data 
          memcpy(q->cb_in, &data[rp/8], rlen * sizeof(uint8_t)/8);
        } else {
          INFO("Last CB, appending parity: %d from %d and 24 to %d\n",
              rlen - 24, rp, rlen - 24);
          
          /* Append Transport Block parity bits to the last CB */
          memcpy(q->cb_in, &data[rp/8], (rlen - 24) * sizeof(uint8_t)/8);
          memcpy(&q->cb_in[(rlen - 24)/8], parity, 3 * sizeof(uint8_t));
        }        
        
        /* Attach Codeblock CRC */
        if (cb_segm->C > 1) {
          srslte_crc_attach_byte(&q->crc_cb, q->cb_in, rlen);
        }

        /* Turbo Encoding */
        srslte_tcod_encode_lut(&q->encoder, q->cb_in, q->parity_bits, cblen_idx);        
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
                     int16_t *e_bits, uint8_t *data,
                     uint32_t cb_size_group) 
{

  bool cb_map[SRSLTE_MAX_CODEBLOCKS];
    
  uint32_t cb_idx[SRSLTE_TDEC_MAX_NPAR];
  int16_t *decoder_input[SRSLTE_TDEC_MAX_NPAR];
  
  uint32_t nof_cb     = cb_size_group?cb_segm->C2:cb_segm->C1;
  uint32_t first_cb   = cb_size_group?cb_segm->C1:0;
  uint32_t cb_len     = cb_size_group?cb_segm->K2:cb_segm->K1;
  uint32_t cb_len_idx = cb_size_group?cb_segm->K2_idx:cb_segm->K1_idx;

  uint32_t rlen       = cb_segm->C==1?cb_len:(cb_len-24);
  uint32_t Gp         = nof_e_bits / Qm;
  uint32_t gamma      = cb_segm->C>0?Gp%cb_segm->C:Gp;
  uint32_t n_e        = Qm * (Gp/cb_segm->C);
  
  if (nof_cb > SRSLTE_MAX_CODEBLOCKS) {
    fprintf(stderr, "Error SRSLTE_MAX_CODEBLOCKS=%d\n", SRSLTE_MAX_CODEBLOCKS);
    return false; 
  }
  
  for (int i=0;i<srslte_tdec_get_nof_parallel(&q->decoder);i++) {
    cb_idx[i]        = i+first_cb; 
    decoder_input[i] = NULL;
  }
  
  uint32_t remaining_cb = 0;
  for (int i=0;i<nof_cb;i++) {
    /* Do not process blocks with CRC Ok */
    cb_map[i] = softbuffer->cb_crc[i];
    if (softbuffer->cb_crc[i] == false) {
      remaining_cb ++;
    }
  }
    
  srslte_tdec_reset(&q->decoder, cb_len);
  
  q->nof_iterations = 0;

  while(remaining_cb>0) {
        
    // Unratematch the codeblocks left to decode 
    for (int i=0;i<srslte_tdec_get_nof_parallel(&q->decoder);i++) {
      
      if (!decoder_input[i] && remaining_cb > 0) {        
        // Find an unprocessed CB 
        cb_idx[i]=first_cb;
        while(cb_idx[i]<first_cb+nof_cb-1 && cb_map[cb_idx[i]]) {
          cb_idx[i]++;
        }
        if (cb_map[cb_idx[i]] == false) {
          cb_map[cb_idx[i]] = true; 
          
          uint32_t rp   = cb_idx[i]*n_e;  
          uint32_t n_e2 = n_e;
          
          if (cb_idx[i] > cb_segm->C - gamma) {
            n_e2 = n_e+Qm;
            rp   = (cb_segm->C - gamma)*n_e + (cb_idx[i]-(cb_segm->C - gamma))*n_e2;
          }

          INFO("CB %d: rp=%d, n_e=%d, i=%d\n", cb_idx[i], rp, n_e2, i);
          if (srslte_rm_turbo_rx_lut(&e_bits[rp], softbuffer->buffer_f[cb_idx[i]], n_e2, cb_len_idx, rv)) {
            fprintf(stderr, "Error in rate matching\n");
            return SRSLTE_ERROR;
          }

          decoder_input[i] = softbuffer->buffer_f[cb_idx[i]];
        }
      }
    }
        
    // Run 1 iteration for the codeblocks in queue
    srslte_tdec_iteration_par(&q->decoder, decoder_input, cb_len);

    // Decide output bits and compute CRC 
    for (int i=0;i<srslte_tdec_get_nof_parallel(&q->decoder);i++) {
      if (decoder_input[i]) {        
        srslte_tdec_decision_byte_par_cb(&q->decoder, q->cb_in, i, cb_len);

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
        if (!srslte_crc_checksum_byte(crc_ptr, q->cb_in, len_crc)) {

          memcpy(softbuffer->data[cb_idx[i]], q->cb_in, rlen/8 * sizeof(uint8_t));
          softbuffer->cb_crc[cb_idx[i]] = true;

          q->nof_iterations += srslte_tdec_get_nof_iterations_cb(&q->decoder, i);

          // Reset number of iterations for that CB in the decoder 
          srslte_tdec_reset_cb(&q->decoder, i);
          remaining_cb--;        
          decoder_input[i] = NULL; 
          cb_idx[i] = 0; 

        // CRC is error and exceeded maximum iterations for this CB.
        // Early stop the whole transport block.
        } else if (srslte_tdec_get_nof_iterations_cb(&q->decoder, i) >= q->max_iterations) {
          INFO("CB %d: Error. CB is erroneous. remaining_cb=%d, i=%d, first_cb=%d, nof_cb=%d\n", 
                cb_idx[i], remaining_cb, i, first_cb, nof_cb);

          q->nof_iterations += q->max_iterations;
          srslte_tdec_reset_cb(&q->decoder, i);
          remaining_cb--;
          decoder_input[i] = NULL;
          cb_idx[i] = 0;
        }
      }
    }    
  }

  softbuffer->tb_crc = true;
  for (int i = 0; i < nof_cb && softbuffer->tb_crc; i++) {
    /* If one CB failed return false */
    softbuffer->tb_crc = softbuffer->cb_crc[i];
  }
  if (softbuffer->tb_crc) {
    for (int i = 0; i < nof_cb; i++) {
      memcpy(&data[i * rlen / 8], softbuffer->data[i], rlen/8 * sizeof(uint8_t));
    }
  }

  q->nof_iterations /= nof_cb;
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
    
    uint32_t nof_cb_groups = cb_segm->C2>0?2:1; 
    
    data[cb_segm->tbs/8+0] = 0; 
    data[cb_segm->tbs/8+1] = 0; 
    data[cb_segm->tbs/8+2] = 0; 
    
    // Process Codeblocks in groups of equal CB size to parallelize according to SRSLTE_TDEC_MAX_NPAR
    for (uint32_t i=0;i<nof_cb_groups && crc_ok;i++) {
      crc_ok = decode_tb_cb(q, softbuffer, cb_segm, Qm, rv, nof_e_bits, e_bits, data, i);            
    }
    
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

/* UL-SCH channel interleaver according to 5.2.2.8 of 36.212 */
void ulsch_interleave(uint8_t *g_bits, uint32_t Qm, uint32_t H_prime_total, 
                      uint32_t N_pusch_symbs, uint8_t *q_bits, srslte_uci_bit_t *ri_bits, uint32_t nof_ri_bits, 
                      uint8_t *ri_present, uint32_t *inteleaver_lut)
{
  
  // Prepare ri_bits for fast search using temp_buffer
  if (nof_ri_bits > 0) {
    for (uint32_t i=0;i<nof_ri_bits;i++) {
      ri_present[ri_bits[i].position] = 1;       
    }
  }
  
  // Genearate interleaver table and interleave bits
  ulsch_interleave_gen(H_prime_total, N_pusch_symbs, Qm, ri_present, inteleaver_lut); 
  srslte_bit_interleave_i(g_bits, q_bits, inteleaver_lut, H_prime_total*Qm);
  
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

