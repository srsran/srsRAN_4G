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

#include "liblte/phy/phch/uci.h"
#include "liblte/phy/phch/harq.h"
#include "liblte/phy/fec/convcoder.h"
#include "liblte/phy/fec/crc.h"
#include "liblte/phy/fec/rm_conv.h"
#include "liblte/phy/common/phy_common.h"
#include "liblte/phy/utils/vector.h"
#include "liblte/phy/utils/debug.h"

/* Table 5.2.2.6.4-1: Basis sequence for (32, O) code */
static uint8_t M_basis_seq_pusch[32][11]={
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
                                    
int uci_cqi_init(uci_cqi_pusch_t *q) {
  if (crc_init(&q->crc, LTE_CRC8, 8)) {
    return LIBLTE_ERROR;
  }
  return LIBLTE_SUCCESS;
}

void uci_cqi_free(uci_cqi_pusch_t *q) {
  
}

static uint32_t Q_prime_cqi(uint32_t O, float beta, uint32_t Q_prime_ri, harq_t *harq_process) {
  uint32_t M_sc = harq_process->prb_alloc.slot[0].nof_prb * RE_X_RB;
  
  uint32_t K = harq_process->cb_segm.C1*harq_process->cb_segm.K1 + 
    harq_process->cb_segm.C2*harq_process->cb_segm.K2;
    
  uint32_t Q_prime = 0;
  if (K > 0) {
    uint32_t M_sc_init = harq_process->nof_prb_pusch_init * RE_X_RB;
    uint32_t L = (O<11)?0:8;
    uint32_t x = (uint32_t) ceilf((float) (O+L)*M_sc_init*harq_process->N_symb_ul*beta/K);

    Q_prime = MIN(x, M_sc * harq_process->N_symb_ul - Q_prime_ri);    
  } else {
    Q_prime = 12*harq_process->prb_alloc.slot[0].nof_prb*RE_X_RB - Q_prime_ri;
  }

  return Q_prime; 
}

/* Encode UCI CQI/PMI for payloads equal or lower to 11 bits (Sec 5.2.2.6.4)
 */
int encode_cqi_short(uci_cqi_pusch_t *q, uint8_t *data, uint32_t nof_bits, uint8_t *q_bits, uint32_t Q)
{
  if (nof_bits          < MAX_CQI_LEN_PUSCH &&
      q                 != NULL             &&
      data              != NULL             &&
      q_bits            != NULL) 
  {
    for (int i=0;i<32;i++) {
      q->encoded_cqi[i] = 0;
      for (int n=0;n<nof_bits;n++) {
        q->encoded_cqi[i] += (data[n] * M_basis_seq_pusch[i][n]); 
      }
    }
    
    for (int i=0;i<Q;i++) {
      q_bits[i] = q->encoded_cqi[i%32]%2;
    }
    return LIBLTE_SUCCESS;
  } else {
    return LIBLTE_ERROR_INVALID_INPUTS;     
  }
}

/* Encode UCI CQI/PMI for payloads greater than 11 bits (go through CRC, conv coder and rate match)
 */
int encode_cqi_long(uci_cqi_pusch_t *q, uint8_t *data, uint32_t nof_bits, uint8_t *q_bits, uint32_t Q)
{
  convcoder_t encoder;

  if (nof_bits + 8 < MAX_CQI_LEN_PUSCH &&
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
    crc_attach(&q->crc, q->tmp_cqi, nof_bits);

    convcoder_encode(&encoder, q->tmp_cqi, q->encoded_cqi, nof_bits + 8);

    DEBUG("CConv output: ", 0);
    
    if (VERBOSE_ISDEBUG()) {
      vec_fprint_b(stdout, q->encoded_cqi, 3 * (nof_bits + 8));
    }

    rm_conv_tx(q->encoded_cqi, 3 * (nof_bits + 8), q_bits, Q);
    
    return LIBLTE_SUCCESS;
  } else {
    return LIBLTE_ERROR_INVALID_INPUTS; 
  }
}

/* Encode UCI CQI/PMI as described in 5.2.3.3 of 36.212 
 */
int uci_encode_cqi_pucch(uint8_t *cqi_data, uint32_t cqi_len, uint8_t b_bits[CQI_CODED_PUCCH_B])
{
  if (cqi_len <= MAX_CQI_LEN_PUCCH) {
    for (uint32_t i=0;i<CQI_CODED_PUCCH_B;i++) {
      uint64_t x=0;
      for (uint32_t n=0;n<cqi_len;n++) {
        x += cqi_data[n]*M_basis_seq_pucch[n][i];
      }
      b_bits[i] = (uint8_t) (x%2);
    }
    return LIBLTE_SUCCESS;
  } else {
    return LIBLTE_ERROR_INVALID_INPUTS;
  }
}

/* Encode UCI CQI/PMI as described in 5.2.2.6 of 36.212 
 */
int uci_encode_cqi_pusch(uci_cqi_pusch_t *q, uint8_t *cqi_data, uint32_t cqi_len, float beta, uint32_t Q_prime_ri, 
                   harq_t *harq_process, uint8_t *q_bits)
{
  
  uint32_t Q_prime = Q_prime_cqi(cqi_len, beta, Q_prime_ri, harq_process);
  uint32_t Q_m = lte_mod_bits_x_symbol(harq_process->mcs.mod);
  
  int ret = LIBLTE_ERROR;
  if (cqi_len <= 11) {
    ret = encode_cqi_short(q, cqi_data, cqi_len, q_bits, Q_prime*Q_m);
  } else {
    ret = encode_cqi_long(q, cqi_data, cqi_len, q_bits, Q_prime*Q_m);
  }
  if (ret) {
    return ret;
  } else {
    return (int) Q_prime;
  }
}

/* Inserts UCI-ACK bits into the correct positions in the g buffer before interleaving */
static int uci_ulsch_interleave_ack(uint8_t ack_coded_bits[6], uint32_t ack_q_bit_idx, 
                          uint32_t Q_m, uint32_t H_prime_total, uint32_t N_pusch_symbs, lte_cp_t cp,
                          uint8_t *q_bits) {

  const uint32_t ack_column_set_norm[4] = {2, 3, 8, 9};
  const uint32_t ack_column_set_ext[4] = {1, 2, 6, 7};
  
  if (H_prime_total/N_pusch_symbs >= 1+ack_q_bit_idx/4) {
    uint32_t row = H_prime_total/N_pusch_symbs-1-ack_q_bit_idx/4;
    uint32_t colidx = (3*ack_q_bit_idx)%4;
    uint32_t col = CP_ISNORM(cp)?ack_column_set_norm[colidx]:ack_column_set_ext[colidx];
    for(uint32_t k=0; k<Q_m; k++) {
      q_bits[row *Q_m + 
             (H_prime_total/N_pusch_symbs)*col*Q_m + k] = ack_coded_bits[k];
    }    
    return LIBLTE_SUCCESS;
  } else {
    fprintf(stderr, "Error interleaving UCI-ACK bit idx %d for H_prime_total=%d and N_pusch_symbs=%d\n",
            ack_q_bit_idx, H_prime_total, N_pusch_symbs);
    return LIBLTE_ERROR;
  }
}

/* Inserts UCI-RI bits into the correct positions in the g buffer before interleaving */
static int uci_ulsch_interleave_ri(uint8_t ri_coded_bits[6], uint32_t ri_q_bit_idx, 
                          uint32_t Q_m, uint32_t H_prime_total, uint32_t N_pusch_symbs, lte_cp_t cp,
                          uint8_t *q_bits) {
  
  static uint32_t ri_column_set_norm[4]  = {1, 4, 7, 10};
  static uint32_t ri_column_set_ext[4]  = {0, 3, 5, 8};

  if (H_prime_total/N_pusch_symbs >= 1+ri_q_bit_idx/4) {
    uint32_t row = H_prime_total/N_pusch_symbs-1-ri_q_bit_idx/4;
    uint32_t colidx = (3*ri_q_bit_idx)%4;
    uint32_t col = CP_ISNORM(cp)?ri_column_set_norm[colidx]:ri_column_set_ext[colidx];
    printf("r=%d-%d\n",H_prime_total/N_pusch_symbs,1+ri_q_bit_idx/4);
    for(uint32_t k=0; k<Q_m; k++) {
      q_bits[row *Q_m + (H_prime_total/N_pusch_symbs)*col*Q_m + k] = 10+ri_coded_bits[k];
    }    
    return LIBLTE_SUCCESS;
  } else {
    fprintf(stderr, "Error interleaving UCI-RI bit idx %d for H_prime_total=%d and N_pusch_symbs=%d\n",
            ri_q_bit_idx, H_prime_total, N_pusch_symbs);
    return LIBLTE_ERROR;
  }

}

static uint32_t Q_prime_ri_ack(uint32_t O, uint32_t O_cqi, float beta, harq_t *harq_process) {
  uint32_t M_sc = harq_process->prb_alloc.slot[0].nof_prb * RE_X_RB;
  
  uint32_t K = harq_process->cb_segm.C1*harq_process->cb_segm.K1 + 
    harq_process->cb_segm.C2*harq_process->cb_segm.K2;
  
  // If not carrying UL-SCH, get Q_prime according to 5.2.4.1
  if (K == 0) {
    if (O_cqi <= 11) {
      K = O_cqi; 
    } else {
      K = O_cqi+8;     
    }
  }
    
  uint32_t M_sc_init = harq_process->nof_prb_pusch_init * RE_X_RB;
    
  uint32_t x = (uint32_t) ceilf((float) O*M_sc_init*harq_process->N_symb_ul*beta/K);

  uint32_t Q_prime = MIN(x, 4*M_sc);

  return Q_prime; 
}

static void encode_ri_ack(uint8_t data, uint8_t q_encoded_bits[6], uint8_t Q_m) {
  q_encoded_bits[0] = data;
  q_encoded_bits[1] = 2; 
  for (uint32_t i=2;i<Q_m;i++) {
    q_encoded_bits[i] = 3;
  }
}

/* Encode UCI HARQ/ACK bits as described in 5.2.2.6 of 36.212 
 *  Currently only supporting 1-bit HARQ
 */
int uci_encode_ack(uint8_t data, uint32_t O_cqi, float beta, harq_t *harq_process, uint32_t H_prime_total, uint8_t *q_bits)
{
  uint32_t Q_m = lte_mod_bits_x_symbol(harq_process->mcs.mod);  
  uint32_t Qprime = Q_prime_ri_ack(1, O_cqi, beta, harq_process);
  uint8_t q_encoded_bits[6];

  encode_ri_ack(data, q_encoded_bits, Q_m);
  
  for (uint32_t i=0;i<Qprime;i++) {
    uci_ulsch_interleave_ack(q_encoded_bits, i, Q_m, H_prime_total, harq_process->N_symb_ul, harq_process->cell.cp, q_bits);
  }
  
  return (int) Qprime;
}


/* Encode UCI RI bits as described in 5.2.2.6 of 36.212 
 *  Currently only supporting 1-bit RI
 */
int uci_encode_ri(uint8_t data, uint32_t O_cqi, float beta, harq_t *harq_process, uint32_t H_prime_total, uint8_t *q_bits)
{
  uint32_t Q_m = lte_mod_bits_x_symbol(harq_process->mcs.mod);  
  uint32_t Qprime = Q_prime_ri_ack(1, O_cqi, beta, harq_process);
  uint8_t q_encoded_bits[6];

  encode_ri_ack(data, q_encoded_bits, Q_m);
  
  for (uint32_t i=0;i<Qprime;i++) {
    uci_ulsch_interleave_ri(q_encoded_bits, i, Q_m, H_prime_total, harq_process->N_symb_ul, harq_process->cell.cp, q_bits);
  }
  
  return (int) Qprime;
}


