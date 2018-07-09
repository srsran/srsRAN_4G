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
#include <srslte/phy/phch/uci.h>

#include "srslte/phy/phch/uci.h"
#include "srslte/phy/fec/cbsegm.h"
#include "srslte/phy/fec/convcoder.h"
#include "srslte/phy/fec/crc.h"
#include "srslte/phy/fec/rm_conv.h"
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/utils/vector.h"
#include "srslte/phy/utils/bit.h"
#include "srslte/phy/utils/debug.h"


/* Table 5.2.2.6.4-1: Basis sequence for (32, O) code */
static uint8_t M_basis_seq[32][11]={
                                    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
                                    {1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1 },
                                    {1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 1 },
                                    {1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1 },
                                    {1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1 },
                                    {1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 1 },
                                    {1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1 },
                                    {1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1 },
                                    {1, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1 },
                                    {1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1 },
                                    {1, 0, 1, 0, 0, 1, 1, 1, 0, 1, 1 },
                                    {1, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1 },
                                    {1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1 },
                                    {1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1 },
                                    {1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 1 },
                                    {1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1 },
                                    {1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 0 },
                                    {1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0 },
                                    {1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0 },
                                    {1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0 },
                                    {1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1 },
                                    {1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1 },
                                    {1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1 },
                                    {1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1 },
                                    {1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0 },
                                    {1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1 },
                                    {1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0 },
                                    {1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0 },
                                    {1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0 },
                                    {1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
                                    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
                                    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                                    };


static uint8_t M_basis_seq_pucch[20][13]={
                                  {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0},
                                  {1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0},
                                  {1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1},
                                  {1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1},
                                  {1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 1},
                                  {1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1},
                                  {1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1},
                                  {1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1},
                                  {1, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 1, 1},
                                  {1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1},
                                  {1, 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1},
                                  {1, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1},
                                  {1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1},
                                  {1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1},
                                  {1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1},
                                  {1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1},
                                  {1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1},
                                  {1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1},
                                  {1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
                                  {1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0},
                                  };

void srslte_uci_cqi_pucch_init(srslte_uci_cqi_pucch_t *q) {
  uint8_t word[16];

  uint32_t nwords = 1 << SRSLTE_UCI_MAX_CQI_LEN_PUCCH;
  q->cqi_table = srslte_vec_malloc(nwords * sizeof(int8_t *));
  q->cqi_table_s = srslte_vec_malloc(nwords * sizeof(int16_t *));

  for (uint32_t w = 0; w < nwords; w++) {
    q->cqi_table[w] = srslte_vec_malloc(SRSLTE_UCI_CQI_CODED_PUCCH_B * sizeof(int8_t));
    q->cqi_table_s[w] = srslte_vec_malloc(SRSLTE_UCI_CQI_CODED_PUCCH_B * sizeof(int16_t));
    uint8_t *ptr = word;
    srslte_bit_unpack(w, &ptr, SRSLTE_UCI_MAX_CQI_LEN_PUCCH);
    srslte_uci_encode_cqi_pucch(word, SRSLTE_UCI_MAX_CQI_LEN_PUCCH, q->cqi_table[w]);
    for (int j = 0; j < SRSLTE_UCI_CQI_CODED_PUCCH_B; j++) {
      q->cqi_table_s[w][j] = (int16_t)(2 * q->cqi_table[w][j] - 1);
    }
  }
}

void srslte_uci_cqi_pucch_free(srslte_uci_cqi_pucch_t *q) {
  uint32_t nwords      = 1 << SRSLTE_UCI_MAX_CQI_LEN_PUCCH;
  for (uint32_t w=0;w<nwords;w++) {
    if (q->cqi_table[w]) {
      free(q->cqi_table[w]);
    }
    if (q->cqi_table_s[w]) {
      free(q->cqi_table_s[w]);
    }
  }
  free(q->cqi_table);
  free(q->cqi_table_s);
}

/* Encode UCI CQI/PMI as described in 5.2.3.3 of 36.212 
 */
int srslte_uci_encode_cqi_pucch(uint8_t *cqi_data, uint32_t cqi_len, uint8_t b_bits[SRSLTE_UCI_CQI_CODED_PUCCH_B])
{
  if (cqi_len <= SRSLTE_UCI_MAX_CQI_LEN_PUCCH) {
    for (uint32_t i=0;i<SRSLTE_UCI_CQI_CODED_PUCCH_B;i++) {
      uint64_t x=0;
      for (uint32_t n=0;n<cqi_len;n++) {
        x += cqi_data[n]*M_basis_seq_pucch[i][n];
      }
      b_bits[i] = (uint8_t) (x%2);
    }
    return SRSLTE_SUCCESS;
  } else {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }
}

int srslte_uci_encode_cqi_pucch_from_table(srslte_uci_cqi_pucch_t *q, uint8_t *cqi_data, uint32_t cqi_len, uint8_t b_bits[SRSLTE_UCI_CQI_CODED_PUCCH_B])
{
  if (cqi_len <= SRSLTE_UCI_MAX_CQI_LEN_PUCCH) {
    bzero(&cqi_data[cqi_len], SRSLTE_UCI_MAX_CQI_LEN_PUCCH - cqi_len);
    uint8_t *ptr = cqi_data;
    uint32_t packed = srslte_bit_pack(&ptr, SRSLTE_UCI_MAX_CQI_LEN_PUCCH);
    memcpy(b_bits, q->cqi_table[packed], SRSLTE_UCI_CQI_CODED_PUCCH_B);

    return SRSLTE_SUCCESS;
  } else {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }
}

/* Decode UCI CQI/PMI over PUCCH 
 */
int16_t srslte_uci_decode_cqi_pucch(srslte_uci_cqi_pucch_t *q, int16_t b_bits[32], uint8_t *cqi_data, uint32_t cqi_len)
{
  if (cqi_len           < SRSLTE_UCI_MAX_CQI_LEN_PUCCH     &&
      b_bits            != NULL  &&
      cqi_data          != NULL) 
  {
    uint32_t max_w = 0;
    int32_t max_corr = INT32_MIN;
    uint32_t nwords      = 1 << SRSLTE_UCI_MAX_CQI_LEN_PUCCH;
    for (uint32_t w=0;w<nwords;w += 1<<(SRSLTE_UCI_MAX_CQI_LEN_PUCCH - cqi_len)) {
          
      // Calculate correlation with pregenerated word and select maximum
      int32_t corr = srslte_vec_dot_prod_sss(q->cqi_table_s[w], b_bits, SRSLTE_UCI_CQI_CODED_PUCCH_B);
      if (corr > max_corr) {
        max_corr = corr; 
        max_w = w; 
      }
    }
    // Convert word to bits again
    uint8_t *ptr = cqi_data; 
    srslte_bit_unpack(max_w, &ptr, SRSLTE_UCI_MAX_CQI_LEN_PUCCH);
    
    INFO("Decoded CQI: w=%d, corr=%d\n", max_w, max_corr);
    return max_corr;
  } else {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }  
}

                                  
                                  
                                  
                                  
                                  
                                  
                                  
void encode_cqi_pusch_block(srslte_uci_cqi_pusch_t *q, uint8_t *data, uint32_t nof_bits, uint8_t output[32]) {
  for (int i=0;i<32;i++) {
    output[i] = 0;
    for (int n=0;n<nof_bits;n++) {
      output[i] = (output[i] + data[n] * M_basis_seq[i][n])%2; 
    }
  }
}

void cqi_pusch_pregen(srslte_uci_cqi_pusch_t *q) {
  uint8_t word[11]; 
    
  for (int i=0;i<11;i++) {
    uint32_t nwords   = (1<<(i+1));
    q->cqi_table[i]   = srslte_vec_malloc(sizeof(uint8_t)*nwords*32);
    q->cqi_table_s[i] = srslte_vec_malloc(sizeof(int16_t)*nwords*32);
    for (uint32_t w=0;w<nwords;w++) {
      uint8_t *ptr = word; 
      srslte_bit_unpack(w, &ptr, i+1);
      encode_cqi_pusch_block(q, word, i+1, &q->cqi_table[i][32*w]);    
      for (int j=0;j<32;j++) {
        q->cqi_table_s[i][32*w+j] = 2*q->cqi_table[i][32*w+j]-1;
      }
    }
  }
}

void cqi_pusch_pregen_free(srslte_uci_cqi_pusch_t *q) {
  for (int i=0;i<11;i++) {
    if (q->cqi_table[i]) {
      free(q->cqi_table[i]);
    }
    if (q->cqi_table_s[i]) {
      free(q->cqi_table_s[i]);
    }
  }
}

int srslte_uci_cqi_init(srslte_uci_cqi_pusch_t *q) {
  if (srslte_crc_init(&q->crc, SRSLTE_LTE_CRC8, 8)) {
    return SRSLTE_ERROR;
  }
  int poly[3] = { 0x6D, 0x4F, 0x57 };
  if (srslte_viterbi_init(&q->viterbi, SRSLTE_VITERBI_37, poly, SRSLTE_UCI_MAX_CQI_LEN_PUSCH, true)) {
    return SRSLTE_ERROR;
  }
  
  cqi_pusch_pregen(q);
  
  return SRSLTE_SUCCESS;
}

void srslte_uci_cqi_free(srslte_uci_cqi_pusch_t *q) 
{
  srslte_viterbi_free(&q->viterbi);
  
  cqi_pusch_pregen_free(q);
}

static uint32_t Q_prime_cqi(srslte_pusch_cfg_t *cfg, 
                            uint32_t O, float beta, uint32_t Q_prime_ri) 
{
  
  uint32_t K = cfg->cb_segm.C1*cfg->cb_segm.K1 + cfg->cb_segm.C2*cfg->cb_segm.K2;
    
  uint32_t Q_prime = 0;
  uint32_t L = (O<11)?0:8;
  uint32_t x = 999999;
  
  if (K > 0) {
    x = (uint32_t) ceilf((float) (O+L)*cfg->grant.M_sc_init*cfg->nbits.nof_symb*beta/K);      
  }
  
  Q_prime = SRSLTE_MIN(x, cfg->grant.M_sc * cfg->nbits.nof_symb - Q_prime_ri);    

  return Q_prime; 
}

/* Encode UCI CQI/PMI for payloads equal or lower to 11 bits (Sec 5.2.2.6.4)
 */
int encode_cqi_short(srslte_uci_cqi_pusch_t *q, uint8_t *data, uint32_t nof_bits, uint8_t *q_bits, uint32_t Q)
{
  if (nof_bits          <= 11    &&
      nof_bits          > 0      && 
      q                 != NULL  &&
      data              != NULL  &&
      q_bits            != NULL) 
  {
    uint8_t *ptr = data;
    uint32_t w = srslte_bit_pack(&ptr, nof_bits);
    
    for (int i=0;i<Q;i++) {
      q_bits[i] = q->cqi_table[nof_bits-1][w*32+(i%32)];
    }
    return SRSLTE_SUCCESS;
  } else {
    return SRSLTE_ERROR_INVALID_INPUTS;     
  }
}

// For decoding the block-encoded CQI we use ML decoding
int decode_cqi_short(srslte_uci_cqi_pusch_t *q, int16_t *q_bits, uint32_t Q, uint8_t *data, uint32_t nof_bits)
{
  if (nof_bits          <= 11    &&
      nof_bits          > 0      && 
      q                 != NULL  &&
      data              != NULL  &&
      q_bits            != NULL) 
  {
    // Accumulate all copies of the 32-length sequence 
    if (Q>32) {
      int i=1; 
      for (;i<Q/32;i++) {
        srslte_vec_sum_sss(&q_bits[i*32], q_bits, q_bits, 32);
      }
      srslte_vec_sum_sss(&q_bits[i*32], q_bits, q_bits, Q%32);
    }
    
    uint32_t max_w = 0;
    int32_t max_corr = INT32_MIN;   
    for (uint32_t w=0;w<(1<<nof_bits);w++) {
          
      // Calculate correlation with pregenerated word and select maximum
      int32_t corr = srslte_vec_dot_prod_sss(&q->cqi_table_s[nof_bits-1][w*32], q_bits, SRSLTE_MIN(32, Q));
      if (corr > max_corr) {
        max_corr = corr; 
        max_w = w; 
      }
    }
    // Convert word to bits again
    uint8_t *ptr = data; 
    srslte_bit_unpack(max_w, &ptr, nof_bits);
    
    INFO("Decoded CQI: w=%d, corr=%d\n", max_w, max_corr);
    return SRSLTE_SUCCESS;
  } else {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }  
}

/* Encode UCI CQI/PMI for payloads greater than 11 bits (go through CRC, conv coder and rate match)
 */
int encode_cqi_long(srslte_uci_cqi_pusch_t *q, uint8_t *data, uint32_t nof_bits, uint8_t *q_bits, uint32_t Q)
{
  srslte_convcoder_t encoder;

  if (nof_bits + 8 < SRSLTE_UCI_MAX_CQI_LEN_PUSCH &&
      q            != NULL             &&
      data         != NULL             &&
      q_bits       != NULL) 
  {    
    int poly[3] = { 0x6D, 0x4F, 0x57 };
    encoder.K = 7;
    encoder.R = 3;
    encoder.tail_biting = true;
    memcpy(encoder.poly, poly, 3 * sizeof(int));

    memcpy(q->tmp_cqi, data, sizeof(uint8_t) * nof_bits);
    srslte_crc_attach(&q->crc, q->tmp_cqi, nof_bits);

    DEBUG("cqi_crc_tx=");
    if (SRSLTE_VERBOSE_ISDEBUG()) {
      srslte_vec_fprint_b(stdout, q->tmp_cqi, nof_bits+8);
    }
    
    srslte_convcoder_encode(&encoder, q->tmp_cqi, q->encoded_cqi, nof_bits + 8);

    DEBUG("cconv_tx=");
    if (SRSLTE_VERBOSE_ISDEBUG()) {
      srslte_vec_fprint_b(stdout, q->encoded_cqi, 3 * (nof_bits + 8));
    }

    srslte_rm_conv_tx(q->encoded_cqi, 3 * (nof_bits + 8), q_bits, Q);
    
    return SRSLTE_SUCCESS;
  } else {
    return SRSLTE_ERROR_INVALID_INPUTS; 
  }
}

int decode_cqi_long(srslte_uci_cqi_pusch_t *q, int16_t *q_bits, uint32_t Q, 
                    uint8_t *data, uint32_t nof_bits)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS; 
  if (nof_bits + 8 < SRSLTE_UCI_MAX_CQI_LEN_PUSCH &&
      q            != NULL             &&
      data         != NULL             &&
      q_bits       != NULL) 
  {    
    
    srslte_rm_conv_rx_s(q_bits, Q, q->encoded_cqi_s, 3 * (nof_bits + 8));

    DEBUG("cconv_rx=");
    if (SRSLTE_VERBOSE_ISDEBUG()) {
      srslte_vec_fprint_s(stdout, q->encoded_cqi_s, 3 * (nof_bits + 8));
    }

    srslte_viterbi_decode_s(&q->viterbi, q->encoded_cqi_s, q->tmp_cqi, nof_bits + 8);
    
    DEBUG("cqi_crc_rx=");
    if (SRSLTE_VERBOSE_ISDEBUG()) {
      srslte_vec_fprint_b(stdout, q->tmp_cqi, nof_bits+8);
    }
    
    ret = srslte_crc_checksum(&q->crc, q->tmp_cqi, nof_bits + 8);
   if (ret == 0) {
      memcpy(data, q->tmp_cqi, nof_bits*sizeof(uint8_t));
      ret = 1;
    } else {
      ret = 0; 
    }
  }
  return ret;   
}

/* Encode UCI CQI/PMI 
 */
int srslte_uci_decode_cqi_pusch(srslte_uci_cqi_pusch_t *q, srslte_pusch_cfg_t *cfg,
                                int16_t *q_bits, 
                                float beta, uint32_t Q_prime_ri, uint32_t cqi_len, 
                                uint8_t *cqi_data, bool *cqi_ack)
{
  if (beta < 0) {
    fprintf(stderr, "Error beta is reserved\n");
    return -1; 
  }
  uint32_t Q_prime = Q_prime_cqi(cfg, cqi_len, beta, Q_prime_ri);

  int ret = SRSLTE_ERROR;
  if (cqi_len <= 11) {
    ret = decode_cqi_short(q, q_bits, Q_prime*cfg->grant.Qm, cqi_data, cqi_len);
  } else {
    ret = decode_cqi_long(q, q_bits, Q_prime*cfg->grant.Qm, cqi_data, cqi_len);
    if (ret == 1) {
      if (cqi_ack) {
        *cqi_ack = true; 
      }
      ret = 0; 
    } else if (ret == 0) {
      if (cqi_ack) {
        *cqi_ack = false; 
      }
    }
  }
  if (ret) {
    return ret;
  } else {
    return (int) Q_prime;
  }

  return Q_prime;   
}

/* Encode UCI CQI/PMI as described in 5.2.2.6 of 36.212 
 */
int srslte_uci_encode_cqi_pusch(srslte_uci_cqi_pusch_t *q, srslte_pusch_cfg_t *cfg,
                                uint8_t *cqi_data, uint32_t cqi_len, 
                                float beta, uint32_t Q_prime_ri, 
                                uint8_t *q_bits)
{
  if (beta < 0) {
    fprintf(stderr, "Error beta is reserved\n");
    return -1; 
  }

  uint32_t Q_prime = Q_prime_cqi(cfg, cqi_len, beta, Q_prime_ri);
  
  int ret = SRSLTE_ERROR;
  if (cqi_len <= 11) {
    ret = encode_cqi_short(q, cqi_data, cqi_len, q_bits, Q_prime*cfg->grant.Qm);
  } else {
    ret = encode_cqi_long(q, cqi_data, cqi_len, q_bits, Q_prime*cfg->grant.Qm);
  }
  if (ret) {
    return ret;
  } else {
    return (int) Q_prime;
  }
}

static void uci_ulsch_interleave_put(srslte_uci_bit_type_t ack_coded_bits[6], uint32_t Qm, srslte_uci_bit_t *ack_bits) 
{
  for(uint32_t k=0; k<Qm; k++) {
    ack_bits[k].type = ack_coded_bits[k];
  }      
}

/* Generates UCI-ACK bits and computes position in q bits */
static int uci_ulsch_interleave_ack_gen(uint32_t ack_q_bit_idx, 
                          uint32_t Qm, uint32_t H_prime_total, uint32_t N_pusch_symbs, srslte_cp_t cp,
                          srslte_uci_bit_t *ack_bits) 
{

  const uint32_t ack_column_set_norm[4] = {2, 3, 8, 9};
  const uint32_t ack_column_set_ext[4] = {1, 2, 6, 7};

  if (H_prime_total/N_pusch_symbs >= 1+ack_q_bit_idx/4) {
    uint32_t row = H_prime_total/N_pusch_symbs-1-ack_q_bit_idx/4;
    uint32_t colidx = (3*ack_q_bit_idx)%4;
    uint32_t col = SRSLTE_CP_ISNORM(cp)?ack_column_set_norm[colidx]:ack_column_set_ext[colidx];
    for(uint32_t k=0; k<Qm; k++) {
      ack_bits[k].position = row *Qm + (H_prime_total/N_pusch_symbs)*col*Qm + k;
    }    
    return SRSLTE_SUCCESS;
  } else {
    fprintf(stderr, "Error interleaving UCI-ACK bit idx %d for H_prime_total=%d and N_pusch_symbs=%d\n",
            ack_q_bit_idx, H_prime_total, N_pusch_symbs);
    return SRSLTE_ERROR;
  }
}

/* Inserts UCI-RI bits into the correct positions in the g buffer before interleaving */
static int uci_ulsch_interleave_ri_gen(uint32_t ri_q_bit_idx, 
                          uint32_t Qm, uint32_t H_prime_total, uint32_t N_pusch_symbs, srslte_cp_t cp,
                          srslte_uci_bit_t *ri_bits) 
{
  
  static uint32_t ri_column_set_norm[4]  = {1, 4, 7, 10};
  static uint32_t ri_column_set_ext[4]  = {0, 3, 5, 8};

  if (H_prime_total/N_pusch_symbs >= 1+ri_q_bit_idx/4) {
    uint32_t row = H_prime_total/N_pusch_symbs-1-ri_q_bit_idx/4;
    uint32_t colidx = (3*ri_q_bit_idx)%4;
    uint32_t col = SRSLTE_CP_ISNORM(cp)?ri_column_set_norm[colidx]:ri_column_set_ext[colidx];

    for(uint32_t k=0; k<Qm; k++) {
      ri_bits[k].position = row *Qm + (H_prime_total/N_pusch_symbs)*col*Qm + k;
    }    
    return SRSLTE_SUCCESS;
  } else {
    fprintf(stderr, "Error interleaving UCI-RI bit idx %d for H_prime_total=%d and N_pusch_symbs=%d\n",
            ri_q_bit_idx, H_prime_total, N_pusch_symbs);
    return SRSLTE_ERROR;
  }

}

static uint32_t Q_prime_ri_ack(srslte_pusch_cfg_t *cfg, 
                               uint32_t O, uint32_t O_cqi, float beta) {
  
  if (beta < 0) {
    fprintf(stderr, "Error beta is reserved\n");
    return -1; 
  }

  uint32_t K = cfg->cb_segm.C1*cfg->cb_segm.K1 + cfg->cb_segm.C2*cfg->cb_segm.K2;
  
  // If not carrying UL-SCH, get Q_prime according to 5.2.4.1
  if (K == 0) {
    if (O_cqi <= 11) {
      K = O_cqi; 
    } else {
      K = O_cqi+8;     
    }
  }
    
  uint32_t x = (uint32_t) ceilf((float) O*cfg->grant.M_sc_init*cfg->nbits.nof_symb*beta/K);

  uint32_t Q_prime = SRSLTE_MIN(x, 4*cfg->grant.M_sc);

  return Q_prime; 
}

static uint32_t encode_ri_ack(uint8_t data[2], uint32_t data_len, srslte_uci_bit_type_t q_encoded_bits[18], uint8_t Qm)
{
  uint32_t i = 0;

  if (data_len == 1) {
    q_encoded_bits[i++] = data[0] ? UCI_BIT_1 : UCI_BIT_0;
    q_encoded_bits[i++] = UCI_BIT_REPETITION;
    while(i < Qm) {
      q_encoded_bits[i++] = UCI_BIT_PLACEHOLDER;
    }
  } else if (data_len == 2) {
    q_encoded_bits[i++] = data[0] ? UCI_BIT_1 : UCI_BIT_0;
    q_encoded_bits[i++] = data[1] ? UCI_BIT_1 : UCI_BIT_0;
    while(i<Qm) {
      q_encoded_bits[i++] = UCI_BIT_PLACEHOLDER;
    }
    q_encoded_bits[i++] = (data[0]^data[1]) ? UCI_BIT_1 : UCI_BIT_0;
    q_encoded_bits[i++] = data[0] ? UCI_BIT_1 : UCI_BIT_0;
    while(i<Qm*2) {
      q_encoded_bits[i++] = UCI_BIT_PLACEHOLDER;
    }
    q_encoded_bits[i++] = data[1] ? UCI_BIT_1 : UCI_BIT_0;
    q_encoded_bits[i++] = (data[0]^data[1]) ? UCI_BIT_1 : UCI_BIT_0;
    while(i<Qm*3) {
      q_encoded_bits[i++] = UCI_BIT_PLACEHOLDER;
    }
  }

  return i;
}


/* Decode UCI HARQ/ACK bits as described in 5.2.2.6 of 36.212 
 *  Currently only supporting 1-bit HARQ
 */
static int32_t decode_ri_ack_1bit(int16_t *q_bits, uint8_t *c_seq, srslte_uci_bit_t *pos)
{
  uint32_t p0 = pos[0].position;
  uint32_t p1 = pos[1].position;

  uint32_t q0 = c_seq[p0]?q_bits[p0]:-q_bits[p0];
  uint32_t q1 = c_seq[p0]?q_bits[p1]:-q_bits[p1];

  return -(q0+q1);
}

static void decode_ri_ack_2bits(int16_t *q_bits, uint8_t *c_seq, srslte_uci_bit_t *pos, uint32_t Qm, int32_t data[3])
{
  uint32_t p0 = pos[Qm * 0 + 0].position;
  uint32_t p1 = pos[Qm * 0 + 1].position;
  uint32_t p2 = pos[Qm * 1 + 0].position;
  uint32_t p3 = pos[Qm * 1 + 1].position;
  uint32_t p4 = pos[Qm * 2 + 0].position;
  uint32_t p5 = pos[Qm * 2 + 1].position;

  int32_t q0 = c_seq[p0] ? q_bits[p0] : -q_bits[p0];
  int32_t q1 = c_seq[p1] ? q_bits[p1] : -q_bits[p1];
  int32_t q2 = c_seq[p2] ? q_bits[p2] : -q_bits[p2];
  int32_t q3 = c_seq[p3] ? q_bits[p3] : -q_bits[p3];
  int32_t q4 = c_seq[p4] ? q_bits[p4] : -q_bits[p4];
  int32_t q5 = c_seq[p5] ? q_bits[p5] : -q_bits[p5];

  data[0] -= q0 + q3;
  data[1] -= q1 + q4;
  data[2] -= q2 + q5;
}

/* Encode UCI HARQ/ACK bits as described in 5.2.2.6 of 36.212
 *  Currently only supporting 1-bit HARQ
 */
int srslte_uci_encode_ack(srslte_pusch_cfg_t *cfg, uint8_t acks[2], uint32_t nof_acks,
                          uint32_t O_cqi, float beta, uint32_t H_prime_total,
                          srslte_uci_bit_t *ack_bits)
{
  if (beta < 0) {
    fprintf(stderr, "Error beta is reserved\n");
    return -1;
  }

  uint32_t Qprime = Q_prime_ri_ack(cfg, nof_acks, O_cqi, beta);
  srslte_uci_bit_type_t q_encoded_bits[18];

  uint32_t nof_encoded_bits = encode_ri_ack(acks, nof_acks, q_encoded_bits, cfg->grant.Qm);

  if (nof_encoded_bits > 0) {
    for (uint32_t i = 0; i < Qprime; i++) {
      uci_ulsch_interleave_ack_gen(i,
                                   cfg->grant.Qm,
                                   H_prime_total,
                                   cfg->nbits.nof_symb,
                                   cfg->cp,
                                   &ack_bits[cfg->grant.Qm * i]);
      uci_ulsch_interleave_put(&q_encoded_bits[(i * cfg->grant.Qm) % nof_encoded_bits],
                               cfg->grant.Qm,
                               &ack_bits[cfg->grant.Qm * i]);
    }
  }
  return (int) Qprime;
}

/* Encode UCI RI bits as described in 5.2.2.6 of 36.212
 *  Currently only supporting 1-bit RI
 */
int srslte_uci_encode_ri(srslte_pusch_cfg_t *cfg,
                         uint8_t ri,
                         uint32_t O_cqi, float beta, uint32_t H_prime_total,
                         srslte_uci_bit_t *ri_bits)
{
  // FIXME: It supports RI of 1 bit only
  uint8_t data[2] = {ri, 0};
  if (beta < 0) {
    fprintf(stderr, "Error beta is reserved\n");
    return -1;
  }
  uint32_t Qprime = Q_prime_ri_ack(cfg, 1, O_cqi, beta);
  srslte_uci_bit_type_t q_encoded_bits[18];

  uint32_t nof_encoded_bits = encode_ri_ack(data, 1, q_encoded_bits, cfg->grant.Qm);

  for (uint32_t i=0;i<Qprime;i++) {
    uci_ulsch_interleave_ri_gen(i, cfg->grant.Qm, H_prime_total, cfg->nbits.nof_symb, cfg->cp, &ri_bits[cfg->grant.Qm*i]);
    uci_ulsch_interleave_put(&q_encoded_bits[(i*cfg->grant.Qm)%nof_encoded_bits], cfg->grant.Qm, &ri_bits[cfg->grant.Qm*i]);
  }

  return (int) Qprime;
}

/* Encode UCI ACK/RI bits as described in 5.2.2.6 of 36.212
 *  Currently only supporting 1-bit RI
 */
int srslte_uci_encode_ack_ri(srslte_pusch_cfg_t *cfg,
                             uint8_t *data, uint32_t data_len,
                             uint32_t O_cqi, float beta, uint32_t H_prime_total,
                             srslte_uci_bit_t *bits, bool ack_ri) {
  if (beta < 0) {
    fprintf(stderr, "Error beta is reserved\n");
    return -1;
  }
  uint32_t Qprime = Q_prime_ri_ack(cfg, data_len, O_cqi, beta);
  srslte_uci_bit_type_t q_encoded_bits[18];

  uint32_t nof_encoded_bits = encode_ri_ack(data, data_len, q_encoded_bits, cfg->grant.Qm);

  if (nof_encoded_bits > 0) {
    for (uint32_t i = 0; i < Qprime; i++) {
      if (ack_ri) {
        uci_ulsch_interleave_ri_gen(i,
                                    cfg->grant.Qm,
                                    H_prime_total,
                                    cfg->nbits.nof_symb,
                                    cfg->cp,
                                    &bits[cfg->grant.Qm * i]);
      } else {
        uci_ulsch_interleave_ack_gen(i,
                                     cfg->grant.Qm,
                                     H_prime_total,
                                     cfg->nbits.nof_symb,
                                     cfg->cp,
                                     &bits[cfg->grant.Qm * i]);
      }
      uci_ulsch_interleave_put(&q_encoded_bits[(i * cfg->grant.Qm) % nof_encoded_bits],
                               cfg->grant.Qm,
                               &bits[cfg->grant.Qm * i]);
    }
  }

  return (int) Qprime;
}

/* Decode UCI ACK/RI bits as described in 5.2.2.6 of 36.212
 *  Currently only supporting 1-bit RI
 */
int srslte_uci_decode_ack_ri(srslte_pusch_cfg_t *cfg, int16_t *q_bits, uint8_t *c_seq,
                          float beta, uint32_t H_prime_total,
                          uint32_t O_cqi, srslte_uci_bit_t *ack_ri_bits, uint8_t data[2], uint32_t nof_bits, bool is_ri)
{
  int32_t sum[3] = {0, 0, 0};

  if (beta < 0) {
    fprintf(stderr, "Error beta is reserved\n");
    return -1;
  }

  uint32_t Qprime = Q_prime_ri_ack(cfg, nof_bits, O_cqi, beta);

  for (uint32_t i = 0; i < Qprime; i++) {
    if (is_ri) {
      uci_ulsch_interleave_ri_gen(i,
                                  cfg->grant.Qm,
                                  H_prime_total,
                                  cfg->nbits.nof_symb,
                                  cfg->cp,
                                  &ack_ri_bits[cfg->grant.Qm * i]);
    } else {
      uci_ulsch_interleave_ack_gen(i,
                                   cfg->grant.Qm,
                                   H_prime_total,
                                   cfg->nbits.nof_symb,
                                   cfg->cp,
                                   &ack_ri_bits[cfg->grant.Qm * i]);

    }
    if (nof_bits == 2 && (i % 3 == 0) && i > 0) {
      decode_ri_ack_2bits(q_bits, &c_seq[0], &ack_ri_bits[cfg->grant.Qm * (i - 3)], cfg->grant.Qm, sum);
    } else if (nof_bits == 1) {
      sum[0] += (int32_t) decode_ri_ack_1bit(q_bits, c_seq, &ack_ri_bits[cfg->grant.Qm * i]);
    }
  }

  data[0] = (uint8_t) (sum[0] > 0);
  if (nof_bits == 2) {
    data[1] = (uint8_t) (sum[1] > 0);
  }

  return (int) Qprime;
}

