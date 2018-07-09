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

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#include "srslte/phy/fec/rm_turbo.h"
#include "srslte/phy/utils/bit.h"
#include "srslte/phy/utils/vector.h"
#include "srslte/phy/fec/cbsegm.h"

#ifdef DEBUG_MODE
#warning FIXME: Disabling SSE/AVX turbo rate matching 
#undef LV_HAVE_SSE
#undef LV_HAVE_AVX
#endif

#ifdef LV_HAVE_SSE
#include <smmintrin.h>
int srslte_rm_turbo_rx_lut_sse(int16_t *input, int16_t *output, uint32_t in_len, uint32_t cb_idx, uint32_t rv_idx);
#endif

#ifdef LV_HAVE_AVX
#include <immintrin.h>
int srslte_rm_turbo_rx_lut_avx(int16_t *input, int16_t *output, uint32_t in_len, uint32_t cb_idx, uint32_t rv_idx);
#endif

#define NCOLS 32
#define NROWS_MAX NCOLS

static uint8_t RM_PERM_TC[NCOLS] = { 0, 16, 8, 24, 4, 20, 12, 28, 2, 18, 10, 26,
    6, 22, 14, 30, 1, 17, 9, 25, 5, 21, 13, 29, 3, 19, 11, 27, 7, 23, 15, 31 };

/* Align tables to 16-byte boundary */

static uint16_t interleaver_systematic_bits[192][6160]; // 4 tail bits
static srslte_bit_interleaver_t bit_interleavers_systematic_bits[192];
static uint16_t interleaver_parity_bits[192][2*6160];
static srslte_bit_interleaver_t bit_interleavers_parity_bits[192];
static uint16_t deinterleaver[192][4][18448];
static int k0_vec[SRSLTE_NOF_TC_CB_SIZES][4][2];
static bool rm_turbo_tables_generated = false; 


static uint16_t temp_table1[3*6176], temp_table2[3*6176];

void srslte_rm_turbo_gentable_systematic(uint16_t *table_bits, int k0_vec[4][2], uint32_t nrows, int ndummy) {

  bool last_is_null=true;
  int k_b=0, buff_idx=0;
  for (int j = 0; j < NCOLS; j++) {
    for (int i = 0; i < nrows; i++) {      
      if (i * NCOLS + RM_PERM_TC[j] >= ndummy) {
        table_bits[k_b] = i * NCOLS + RM_PERM_TC[j] - ndummy;
        k_b++;        
        last_is_null=false;
      } else {
        last_is_null=true;
      }
      for (int i=0;i<4;i++) {
        if (k0_vec[i][1] == -1) {
          if (k0_vec[i][0]%(3*nrows*NCOLS) <= buff_idx && !last_is_null) {
            k0_vec[i][1] = k_b-1;
          }
        }
      }
      buff_idx++;
    }
  }
}

void srslte_rm_turbo_gentable_parity(uint16_t *table_parity, int k0_vec[4][2], int offset, uint16_t nrows, int ndummy) {
  
  bool last_is_null=true;
  int k_b=0, buff_idx0=0;
  int K_p = nrows*NCOLS;
  int buff_idx1=0;
  for (int j = 0; j < NCOLS; j++) {
    for (int i = 0; i < nrows; i++) {      
      if (i * NCOLS + RM_PERM_TC[j] >= ndummy) {
        table_parity[k_b] = i * NCOLS + RM_PERM_TC[j] - ndummy;
        k_b++;
        last_is_null=false;
      } else {
        last_is_null=true;
      }
      for (int i=0;i<4;i++) {
        if (k0_vec[i][1] == -1) {
          if (k0_vec[i][0]%(3*K_p) <= 2*buff_idx0+K_p && !last_is_null) {
            k0_vec[i][1] = offset+k_b-1;
          }
        }
      }
      buff_idx0++;
      
      int kidx = (RM_PERM_TC[buff_idx1 / nrows] + NCOLS * (buff_idx1 % nrows) + 1) % K_p;
      if ((kidx - ndummy) >= 0) {
        table_parity[k_b] = kidx-ndummy+offset;
        k_b++;
        last_is_null=false;
      } else {
        last_is_null=true;
      }
      for (int i=0;i<4;i++) {
        if (k0_vec[i][1] == -1) {
          if (k0_vec[i][0]%(3*K_p) <= 2*buff_idx1+1+K_p && !last_is_null) {
            k0_vec[i][1] = offset+k_b-1;
          }
        }
      }
      buff_idx1++;
    }    
  }
}



void srslte_rm_turbo_gentable_receive(uint16_t *table, uint32_t cb_len, uint32_t rv_idx) 
{
  
  int nrows = (uint32_t) (cb_len / 3 - 1) / NCOLS + 1;
  int ndummy = nrows*NCOLS - cb_len / 3;
  if (ndummy < 0) {
    ndummy = 0;
  }

  /* Undo bit collection. Account for dummy bits */
  int N_cb = 3*nrows*NCOLS;   
  int k0 = nrows*(2*(uint16_t) ceilf((float) N_cb/(float) (8*nrows))*rv_idx+2);

  int kidx; 
  int K_p = nrows * NCOLS;
  int k = 0, jp=0, j=0;  
  bool isdummy = false; 
  int d_i, d_j; 
  while (k < cb_len) {
    jp = (k0 + j) % N_cb;

    if (jp < K_p || !(jp % 2)) {
      if (jp >= K_p) {
        d_i = ((jp - K_p) / 2) / nrows;
        d_j = ((jp - K_p) / 2) % nrows;
      } else {
        d_i = jp / nrows;
        d_j = jp % nrows;        
      }      
      if (d_j * NCOLS + RM_PERM_TC[d_i] >= ndummy) {
        isdummy = false;
        if (d_j * NCOLS + RM_PERM_TC[d_i] - ndummy < 0) {
          isdummy = true;
        } 
      } else {
        isdummy = true;
      }

    } else {
      uint32_t jpp = (jp - K_p - 1) / 2;
      kidx = (RM_PERM_TC[jpp / nrows] + NCOLS * (jpp % nrows) + 1) % K_p;
      if ((kidx - ndummy) < 0) {
        isdummy = true;
      } else {
        isdummy = false;
      }      
    }
    
    if (!isdummy) {
      temp_table1[k] = jp%(3*nrows*NCOLS);
      k++;
    }
    j++;
  }

  for (int i = 0; i < cb_len / 3; i++) {
    d_i = (i + ndummy) / NCOLS;
    d_j = (i + ndummy) % NCOLS;
    for (j = 0; j < 3; j++) {
      if (j != 2) {
        kidx = K_p * j + (j + 1) * (RM_PERM_TC[d_j] * nrows + d_i);
      } else {
        k = (i + ndummy - 1) % K_p;
        if (k < 0)
          k += K_p;
        kidx = (k / NCOLS + nrows * RM_PERM_TC[k % NCOLS]) % K_p;
        kidx = 2 * kidx + K_p + 1;
      }
      temp_table2[kidx] = 3*i+j;
    }
  }
  for (int i=0;i<cb_len;i++) {
    table[i] = temp_table2[temp_table1[i]];
  }
}

void srslte_rm_turbo_gentables() {
  if (!rm_turbo_tables_generated) {
    rm_turbo_tables_generated = true; 
    for (int cb_idx=0;cb_idx<SRSLTE_NOF_TC_CB_SIZES;cb_idx++) {
      int cb_len=srslte_cbsegm_cbsize(cb_idx);
      int in_len=3*cb_len+12;
      
      int nrows = (in_len / 3 - 1) / NCOLS + 1;
      int K_p = nrows * NCOLS;
      int ndummy = K_p - in_len / 3;
      if (ndummy < 0) {
        ndummy = 0;
      }

      for (int i=0;i<4;i++) {
        k0_vec[cb_idx][i][0] = nrows * (2 * (uint16_t) ceilf((float) (3*K_p) / (float) (8 * nrows)) * i + 2);
        k0_vec[cb_idx][i][1] = -1; 
      }
      srslte_rm_turbo_gentable_systematic(interleaver_systematic_bits[cb_idx], k0_vec[cb_idx], nrows, ndummy);
      srslte_bit_interleaver_init(&bit_interleavers_systematic_bits[cb_idx], interleaver_systematic_bits[cb_idx],
                                  (uint32_t) srslte_cbsegm_cbsize(cb_idx)+4);

      srslte_rm_turbo_gentable_parity(interleaver_parity_bits[cb_idx], k0_vec[cb_idx], in_len/3, nrows, ndummy);
      srslte_bit_interleaver_init(&bit_interleavers_parity_bits[cb_idx], interleaver_parity_bits[cb_idx],
                                  (uint32_t) (srslte_cbsegm_cbsize(cb_idx)+4)*2);

      for (int i=0;i<4;i++) {
        srslte_rm_turbo_gentable_receive(deinterleaver[cb_idx][i], in_len, i);  
      }      
    }
  }  
}

void srslte_rm_turbo_free_tables () {
  if (rm_turbo_tables_generated) {
    for (int i = 0; i < SRSLTE_NOF_TC_CB_SIZES; i++) {
      srslte_bit_interleaver_free(&bit_interleavers_systematic_bits[i]);
      srslte_bit_interleaver_free(&bit_interleavers_parity_bits[i]);
    }
    rm_turbo_tables_generated = false;
  }
}

/**
 * Rate matching for LTE Turbo Coder
 *
 * @param[out] w_buff Preallocated softbuffer
 * @param[in] systematic Input code block in a byte array
 * @param[in] parity Input code turbo coder parity bits in a byte array
 * @param[out] output Rate matched output array of size out_len
 * @param out_len Output buffer size to be filled with as many FEC bits as fit
 * @param w_offset Start writing to output at this bit offset
 * @param cb_idx Code block index. Used to lookup interleaver parameters
 * @param rv_idx Redundancy Version Index. Indexed offset of FEC bits to copy
 *
 * @return Error code
 */
int srslte_rm_turbo_tx_lut(uint8_t *w_buff, uint8_t *systematic, uint8_t *parity, uint8_t *output, 
                           uint32_t cb_idx, uint32_t out_len, 
                           uint32_t w_offset, uint32_t rv_idx) 
{

  
  if (rv_idx < 4 && cb_idx < SRSLTE_NOF_TC_CB_SIZES) {
    
    int in_len=3*srslte_cbsegm_cbsize(cb_idx)+12;
    
    
    /* Sub-block interleaver (5.1.4.1.1) and bit collection */
    if (rv_idx == 0) {
      
      // Systematic bits 
      //srslte_bit_interleave(systematic, w_buff, interleaver_systematic_bits[cb_idx], in_len/3);
      srslte_bit_interleaver_run(&bit_interleavers_systematic_bits[cb_idx], systematic, w_buff, 0);


      // Parity bits 
      //srslte_bit_interleave_w_offset(parity, &w_buff[in_len/24], interleaver_parity_bits[cb_idx], 2*in_len/3, 4);
      srslte_bit_interleaver_run(&bit_interleavers_parity_bits[cb_idx], parity, &w_buff[in_len/24], 4);
    }

    /* Bit selection and transmission 5.1.4.1.2 */    
    int w_len = 0; 
    int r_ptr = k0_vec[cb_idx][rv_idx][1]; 
    while (w_len < out_len) {
      int cp_len = out_len - w_len; 
      if (cp_len + r_ptr >= in_len) {
        cp_len = in_len - r_ptr;
      }
      srslte_bit_copy(output, w_len+w_offset, w_buff, r_ptr, cp_len);
      r_ptr += cp_len; 
      if (r_ptr >= in_len) {
        r_ptr -= in_len; 
      }
      w_len += cp_len; 
    }

    return 0;
  } else {
    return SRSLTE_ERROR_INVALID_INPUTS; 
  }
}

/**
 * Undoes rate matching for LTE Turbo Coder. Expands rate matched buffer to full size buffer.
 *
 * @param[in] input Input buffer of size in_len
 * @param[out] output Output buffer of size 3*srslte_cbsegm_cbsize(cb_idx)+12
 * @param[in] cb_idx Code block table index
 * @param[in] rv_idx Redundancy Version from DCI control message
 * @return Error code
 */
int srslte_rm_turbo_rx_lut(int16_t *input, int16_t *output, uint32_t in_len, uint32_t cb_idx, uint32_t rv_idx) 
{ 
  
#ifdef LV_HAVE_AVX
  return srslte_rm_turbo_rx_lut_avx(input, output, in_len, cb_idx, rv_idx);
#else 
  #ifdef LV_HAVE_SSE
    return srslte_rm_turbo_rx_lut_sse(input, output, in_len, cb_idx, rv_idx);
  #else
    if (rv_idx < 4 && cb_idx < SRSLTE_NOF_TC_CB_SIZES) {
      uint32_t out_len = 3*srslte_cbsegm_cbsize(cb_idx)+12;
      uint16_t *deinter = deinterleaver[cb_idx][rv_idx];
      for (int i=0;i<in_len;i++) {        
        output[deinter[i%out_len]] += input[i];
      }
      return 0;    
    } else {
      printf("Invalid inputs rv_idx=%d, cb_idx=%d\n", rv_idx, cb_idx);
      return SRSLTE_ERROR_INVALID_INPUTS; 
    }
  #endif
#endif
}

#ifdef LV_HAVE_SSE

int srslte_rm_turbo_rx_lut_sse(int16_t *input, int16_t *output, uint32_t in_len, uint32_t cb_idx, uint32_t rv_idx) 
{
  if (rv_idx < 4 && cb_idx < SRSLTE_NOF_TC_CB_SIZES) {
    uint32_t out_len = 3*srslte_cbsegm_cbsize(cb_idx)+12;
    uint16_t *deinter = deinterleaver[cb_idx][rv_idx];
    
    const __m128i* xPtr   = (const __m128i*) input;
    const __m128i* lutPtr = (const __m128i*) deinter;
    __m128i xVal, lutVal;
    
    /* Simplify load if we do not need to wrap (ie high rates) */
    if (in_len <= out_len) {
      for (int i=0;i<in_len/8;i++) {
        xVal   = _mm_loadu_si128(xPtr);
        lutVal = _mm_loadu_si128(lutPtr);
      
        for (int j=0;j<8;j++) {
          int16_t x  = (int16_t)  _mm_extract_epi16(xVal,   j); 
          uint16_t l = (uint16_t) _mm_extract_epi16(lutVal, j);
          output[l] += x;
        }
        xPtr ++;
        lutPtr ++;
      }
      for (int i=8*(in_len/8);i<in_len;i++) {      
        output[deinter[i%out_len]] += input[i];
      }
    } else {
      int intCnt = 8;
      int inputCnt = 0;
      int nwrapps = 0; 
      while(inputCnt < in_len - 8) {
        xVal   = _mm_loadu_si128(xPtr);
        lutVal = _mm_loadu_si128(lutPtr);
      
        for (int j=0;j<8;j++) {
          int16_t x  = (int16_t)  _mm_extract_epi16(xVal,   j); 
          uint16_t l = (uint16_t) _mm_extract_epi16(lutVal, j);
          output[l] += x;
        }
        xPtr++;
        lutPtr++;
        intCnt   += 8;
        inputCnt += 8;
        if (intCnt >= out_len && inputCnt < in_len - 8) {
          /* Copy last elements */
          for (int j=(nwrapps+1)*out_len-4;j<(nwrapps+1)*out_len;j++) {      
            output[deinter[j%out_len]] += input[j];
            inputCnt++;
          }
          /* And wrap pointers */
          nwrapps++;
          intCnt = 8; 
          xPtr   = (const __m128i*) &input[nwrapps*out_len];
          lutPtr = (const __m128i*) deinter;
        }
      }      
      for (int i=inputCnt;i<in_len;i++) {      
        output[deinter[i%out_len]] += input[i];
      }
    }

    
    return 0;    
  } else {
    printf("Invalid inputs rv_idx=%d, cb_idx=%d\n", rv_idx, cb_idx);
    return SRSLTE_ERROR_INVALID_INPUTS; 
  }
}

#endif


#ifdef LV_HAVE_AVX

#define SAVE_OUTPUT(j) x  = (int16_t) _mm256_extract_epi16(xVal,   j);\
                       l = (uint16_t) _mm256_extract_epi16(lutVal, j);\
                       output[l] += x;


int srslte_rm_turbo_rx_lut_avx(int16_t *input, int16_t *output, uint32_t in_len, uint32_t cb_idx, uint32_t rv_idx) 
{
  if (rv_idx < 4 && cb_idx < SRSLTE_NOF_TC_CB_SIZES) {
    uint32_t out_len = 3*srslte_cbsegm_cbsize(cb_idx)+12;
    uint16_t *deinter = deinterleaver[cb_idx][rv_idx];
    
    const __m256i* xPtr   = (const __m256i*) input;
    const __m256i* lutPtr = (const __m256i*) deinter;
    __m256i xVal, lutVal;
    
    int16_t x; 
    uint16_t l;
    
    /* Simplify load if we do not need to wrap (ie high rates) */
    if (in_len <= out_len) {
      for (int i=0;i<in_len/16;i++) {
        xVal   = _mm256_loadu_si256(xPtr);
        lutVal = _mm256_loadu_si256(lutPtr);
        SAVE_OUTPUT(0);
        SAVE_OUTPUT(1);
        SAVE_OUTPUT(2);
        SAVE_OUTPUT(3);
        SAVE_OUTPUT(4);
        SAVE_OUTPUT(5);
        SAVE_OUTPUT(6);
        SAVE_OUTPUT(7);
        
        SAVE_OUTPUT(8);
        SAVE_OUTPUT(9);
        SAVE_OUTPUT(10);
        SAVE_OUTPUT(11);
        SAVE_OUTPUT(12);
        SAVE_OUTPUT(13);
        SAVE_OUTPUT(14);
        SAVE_OUTPUT(15);
        
        xPtr ++;
        lutPtr ++;
      }
      for (int i=16*(in_len/16);i<in_len;i++) {      
        output[deinter[i%out_len]] += input[i];
      }
    } else {
      int intCnt = 16;
      int inputCnt = 0;
      int nwrapps = 0; 
      while(inputCnt < in_len - 16) {
        xVal   = _mm256_loadu_si256(xPtr);
        lutVal = _mm256_loadu_si256(lutPtr);
      
        SAVE_OUTPUT(0);
        SAVE_OUTPUT(1);
        SAVE_OUTPUT(2);
        SAVE_OUTPUT(3);
        SAVE_OUTPUT(4);
        SAVE_OUTPUT(5);
        SAVE_OUTPUT(6);
        SAVE_OUTPUT(7);
        
        SAVE_OUTPUT(8);
        SAVE_OUTPUT(9);
        SAVE_OUTPUT(10);
        SAVE_OUTPUT(11);
        SAVE_OUTPUT(12);
        SAVE_OUTPUT(13);
        SAVE_OUTPUT(14);
        SAVE_OUTPUT(15);
        xPtr++;
        lutPtr++;
        intCnt   += 16;
        inputCnt += 16;
        if (intCnt >= out_len && inputCnt < in_len - 16) {
          /* Copy last elements */
          if ((out_len%16) == 12) {
            for (int j=(nwrapps+1)*out_len-12;j<(nwrapps+1)*out_len;j++) {      
              output[deinter[j%out_len]] += input[j];
              inputCnt++;
            }
          } else {
            for (int j=(nwrapps+1)*out_len-4;j<(nwrapps+1)*out_len;j++) {      
              output[deinter[j%out_len]] += input[j];
              inputCnt++;
            }
          }
          /* And wrap pointers */
          nwrapps++;
          intCnt = 16; 
          xPtr   = (const __m256i*) &input[nwrapps*out_len];
          lutPtr = (const __m256i*) deinter;
        }
      }      
      for (int i=inputCnt;i<in_len;i++) {      
        output[deinter[i%out_len]] += input[i];
      }
    }    
    return 0;    
  } else {
    printf("Invalid inputs rv_idx=%d, cb_idx=%d\n", rv_idx, cb_idx);
    return SRSLTE_ERROR_INVALID_INPUTS; 
  }
}

#endif





















/* Turbo Code Rate Matching.
 * 3GPP TS 36.212 v10.1.0 section 5.1.4.1
 *
 * If rv_idx==0, the circular buffer w_buff is filled with all redundancy versions and 
 * the corresponding version of length out_len is saved in the output buffer.  
 * Otherwise, the corresponding version is directly obtained from w_buff and saved into output. 
 * 
 * Note that calling this function with rv_idx!=0 without having called it first with rv_idx=0
 * will produce unwanted results. 
 * 
 * TODO: Soft buffer size limitation according to UE category
 */
int srslte_rm_turbo_tx(uint8_t *w_buff, uint32_t w_buff_len, uint8_t *input, uint32_t in_len, uint8_t *output,
    uint32_t out_len, uint32_t rv_idx) {

  int ndummy, kidx; 
  int nrows, K_p;

  int i, j, k, s, N_cb, k0;
  
  if (in_len < 3) {
    fprintf(stderr, "Error minimum input length for rate matching is 3\n");
    return -1;
  }

  nrows = (uint16_t) (in_len / 3 - 1) / NCOLS + 1;
  K_p = nrows * NCOLS;
  if (3 * K_p > w_buff_len) {
    fprintf(stderr,
        "Input too large. Max input length including dummy bits is %d (3x%dx32, in_len %d, Kp=%d)\n",
        w_buff_len, nrows, in_len, K_p);
    return -1;
  }

  ndummy = K_p - in_len / 3;
  if (ndummy < 0) {
    ndummy = 0;
  }

  if (rv_idx == 0) {
    /* Sub-block interleaver (5.1.4.1.1) and bit collection */
    k = 0;
    for (s = 0; s < 2; s++) {
      for (j = 0; j < NCOLS; j++) {
        for (i = 0; i < nrows; i++) {
          if (s == 0) {
            kidx = k % K_p;
          } else {
            kidx = K_p + 2 * (k % K_p);
          }
          if (i * NCOLS + RM_PERM_TC[j] < ndummy) {
            w_buff[kidx] = SRSLTE_TX_NULL;
          } else {
            w_buff[kidx] = input[(i * NCOLS + RM_PERM_TC[j] - ndummy) * 3 + s];
          }
          k++;
        }
      }
    }

    // d_k^(2) goes through special permutation
    for (k = 0; k < K_p; k++) {
      kidx = (RM_PERM_TC[k / nrows] + NCOLS * (k % nrows) + 1) % K_p;
      if ((kidx - ndummy) < 0) {
        w_buff[K_p + 2 * k + 1] = SRSLTE_TX_NULL;
      } else {
        w_buff[K_p + 2 * k + 1] = input[3 * (kidx - ndummy) + 2];
      }
    }
  }
  
  /* Bit selection and transmission 5.1.4.1.2 */
  N_cb = 3 * K_p;       // TODO: Soft buffer size limitation

  k0 = nrows
      * (2 * (uint16_t) ceilf((float) N_cb / (float) (8 * nrows)) * rv_idx + 2);
  k = 0;
  j = 0;
  
  while (k < out_len) {
    if (w_buff[(k0 + j) % N_cb] != SRSLTE_TX_NULL) {
      output[k] = w_buff[(k0 + j) % N_cb];
      k++;      
    }
    j++;
  }
  return 0;
}

/* Undoes Turbo Code Rate Matching.
 * 3GPP TS 36.212 v10.1.0 section 5.1.4.1
 * 
 * Soft-combines the data available in w_buff 
 */
int srslte_rm_turbo_rx(float *w_buff, uint32_t w_buff_len, float *input, uint32_t in_len, float *output,
    uint32_t out_len, uint32_t rv_idx, uint32_t nof_filler_bits) {

  int nrows, ndummy, K_p, k0, N_cb, jp, kidx;
  int i, j, k;
  int d_i, d_j;
  bool isdummy;

  
  
  nrows = (uint16_t) (out_len / 3 - 1) / NCOLS + 1;
  K_p = nrows * NCOLS;
  if (3 * K_p > w_buff_len) {
    fprintf(stderr,
        "Output too large. Max output length including dummy bits is %d (3x%dx32, in_len %d)\n",
        w_buff_len, nrows, out_len);
    return -1;
  }
  
  if (out_len < 3) {
    fprintf(stderr, "Error minimum input length for rate matching is 3\n");
    return -1;
  }


  ndummy = K_p - out_len / 3;
  if (ndummy < 0) {
    ndummy = 0;
  }

  /* Undo bit collection. Account for dummy bits */
  N_cb = 3 * K_p;       // TODO: Soft buffer size limitation
  k0 = nrows
      * (2 * (uint16_t) ceilf((float) N_cb / (float) (8 * nrows)) * rv_idx + 2);
      
  k = 0;
  j = 0;
  while (k < in_len) {
    jp = (k0 + j) % N_cb;

    if (jp < K_p || !(jp % 2)) {
      if (jp >= K_p) {
        d_i = ((jp - K_p) / 2) / nrows;
        d_j = ((jp - K_p) / 2) % nrows;
      } else {
        d_i = jp / nrows;
        d_j = jp % nrows;        
      }      
      if (d_j * NCOLS + RM_PERM_TC[d_i] >= ndummy) {
        isdummy = false;
        if (d_j * NCOLS + RM_PERM_TC[d_i] - ndummy < nof_filler_bits) {
          isdummy = true;
        } 
      } else {
        isdummy = true;
      }

    } else {
      uint16_t jpp = (jp - K_p - 1) / 2;
      kidx = (RM_PERM_TC[jpp / nrows] + NCOLS * (jpp % nrows) + 1) % K_p;
      if ((kidx - ndummy) < 0) {
        isdummy = true;
      } else {
        isdummy = false;
      }      
    }

    if (!isdummy) {
      if (w_buff[jp] == SRSLTE_RX_NULL) {
        w_buff[jp] = input[k];
      } else if (input[k] != SRSLTE_RX_NULL) {
        w_buff[jp] += input[k]; /* soft combine LLRs */
      }
      k++;
    } 
    j++;
  }
  
  //printf("wbuff:\n");
  //srslte_vec_fprint_f(stdout, w_buff, out_len);

  /* interleaving and bit selection */
  for (i = 0; i < out_len / 3; i++) {
    d_i = (i + ndummy) / NCOLS;
    d_j = (i + ndummy) % NCOLS;
    for (j = 0; j < 3; j++) {
      if (j != 2) {
        kidx = K_p * j + (j + 1) * (RM_PERM_TC[d_j] * nrows + d_i);
      } else {
        k = (i + ndummy - 1) % K_p;
        if (k < 0)
          k += K_p;
        kidx = (k / NCOLS + nrows * RM_PERM_TC[k % NCOLS]) % K_p;
        kidx = 2 * kidx + K_p + 1;
      }
      if (w_buff[kidx] != SRSLTE_RX_NULL) {
        output[i * 3 + j] = w_buff[kidx];
      } else {
        output[i * 3 + j] = 0;
      }
    }
  }

  return 0;
}
