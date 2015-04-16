/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
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
#include <complex.h>

#include "srslte/phch/pucch.h"
#include "srslte/common/sequence.h"
#include "srslte/common/phy_common.h"
#include "srslte/utils/debug.h"
#include "srslte/utils/vector.h"

#define MAX_PUSCH_RE(cp) (2 * SRSLTE_CP_NSYMB(cp) * 12)

uint32_t pucch_symbol_format1_cpnorm[4] = {0, 1, 5, 6};
uint32_t pucch_symbol_format1_cpext[4] = {0, 1, 4, 5};
uint32_t pucch_symbol_format2_cpnorm[5] = {0, 2, 3, 4, 6};
uint32_t pucch_symbol_format2_cpext[5] = {0, 1, 2, 4, 5};

// Table 5.4.1-2 Orthogonal sequences w for N_sf=4
float w_n_oc[3][4] = {{1, 1, 1, 1},
                      {1,-1, 1,-1},
                      {1,-1,-1,1}};

/* Verify PUCCH configuration as given in Section 5.4 36.211 */
bool srslte_pucch_cfg_isvalid(srslte_pucch_cfg_t *cfg, uint32_t nof_prb) {
  if (cfg->beta_pucch > 0 && 
      cfg->delta_pucch_shift > 0 && cfg->delta_pucch_shift < 4 &&
      cfg->N_cs < 8 && (cfg->N_cs%cfg->delta_pucch_shift) == 0 && 
      cfg->n_rb_2 < nof_prb) {
    return true; 
  } else {
    return false;    
  }
}

void srslte_pucch_cfg_default(srslte_pucch_cfg_t *cfg) {
  cfg->beta_pucch = 1.0; 
  cfg->delta_pucch_shift = 1; 
}

uint32_t get_N_sf(srslte_pucch_format_t format) {
  switch (format) {
    case SRSLTE_PUCCH_FORMAT_1:
    case SRSLTE_PUCCH_FORMAT_1A:
    case SRSLTE_PUCCH_FORMAT_1B:
      return 4;
    case SRSLTE_PUCCH_FORMAT_2:
    case SRSLTE_PUCCH_FORMAT_2A:
    case SRSLTE_PUCCH_FORMAT_2B:
      return 5; 
  }
  return 0; 
}

uint32_t get_pucch_symbol(uint32_t m, srslte_pucch_format_t format, srslte_cp_t cp) {
  switch (format) {
    case SRSLTE_PUCCH_FORMAT_1:
    case SRSLTE_PUCCH_FORMAT_1A:
    case SRSLTE_PUCCH_FORMAT_1B:
      if (m < 5) {
        if (SRSLTE_CP_ISNORM(cp)) {
          return pucch_symbol_format1_cpnorm[m];
        } else {
          return pucch_symbol_format1_cpext[m];
        }        
      }
    break;
    case SRSLTE_PUCCH_FORMAT_2:
    case SRSLTE_PUCCH_FORMAT_2A:
    case SRSLTE_PUCCH_FORMAT_2B:
      if (m < 6) {
        if (SRSLTE_CP_ISNORM(cp)) {
          return pucch_symbol_format2_cpnorm[m];
        } else {
          return pucch_symbol_format2_cpext[m];
        }        
      }
    break;
  }
  return 0; 
}


// Compute m according to Section 5.4.3 of 36.211
uint32_t srslte_pucch_m(srslte_pucch_cfg_t *cfg, srslte_pucch_format_t format, uint32_t n_pucch, srslte_cp_t cp) {  
  uint32_t m=0; 
  switch (format) {
  case SRSLTE_PUCCH_FORMAT_1:
  case SRSLTE_PUCCH_FORMAT_1A:
  case SRSLTE_PUCCH_FORMAT_1B:
    m = cfg->n_rb_2;
    uint32_t c=SRSLTE_CP_ISNORM(cp)?3:2; 
    if (n_pucch >= c*cfg->N_cs/cfg->delta_pucch_shift) {
      m = (n_pucch-c*cfg->N_cs/cfg->delta_pucch_shift)/(c*SRSLTE_NRE/cfg->delta_pucch_shift)
          +cfg->n_rb_2+(uint32_t)ceilf((float) cfg->N_cs/8);
    }
  break;
  case SRSLTE_PUCCH_FORMAT_2:
  case SRSLTE_PUCCH_FORMAT_2A:
  case SRSLTE_PUCCH_FORMAT_2B:
    m = n_pucch/SRSLTE_NRE; 
  break;
  }
  return m; 
}

/* Generates n_cs_cell according to Sec 5.4 of 36.211 */
int srslte_pucch_n_cs_cell(srslte_cell_t cell, uint32_t n_cs_cell[SRSLTE_NSLOTS_X_FRAME][SRSLTE_CP_NORM_NSYMB]) 
{
  srslte_sequence_t seq; 
  bzero(&seq, sizeof(srslte_sequence_t));

  srslte_sequence_LTE_pr(&seq, 8*SRSLTE_CP_NSYMB(cell.cp)*SRSLTE_NSLOTS_X_FRAME, cell.id);

  for (uint32_t ns=0;ns<SRSLTE_NSLOTS_X_FRAME;ns++) {
    for (uint32_t l=0;l<SRSLTE_CP_NSYMB(cell.cp);l++) {
      n_cs_cell[ns][l] = 0; 
      for (uint32_t i=0;i<8;i++) {
        n_cs_cell[ns][l] += seq.c[8*SRSLTE_CP_NSYMB(cell.cp)*ns+8*l+i]<<i;
      }
    }
  }
  srslte_sequence_free(&seq);
  return SRSLTE_SUCCESS;
}


/* Calculates alpha according to 5.5.2.2.2 (is_dmrs=true) or 5.4.1 (is_dmrs=false) of 36.211 */
float srslte_pucch_alpha(uint32_t n_cs_cell[SRSLTE_NSLOTS_X_FRAME][SRSLTE_CP_NORM_NSYMB], 
                                   srslte_pucch_cfg_t *cfg,
                                   uint32_t n_pucch, 
                                   srslte_cp_t cp, bool is_dmrs,
                                   uint32_t ns, uint32_t l, 
                                   uint32_t *n_oc_ptr, uint32_t *n_prime_ns) 
{
  uint32_t c = SRSLTE_CP_ISNORM(cp)?3:2;
  uint32_t N_prime = (n_pucch < c*cfg->N_cs/cfg->delta_pucch_shift)?cfg->N_cs:SRSLTE_NRE;

  uint32_t n_prime = n_pucch;
  if (n_pucch >= c*cfg->N_cs/cfg->delta_pucch_shift) {
    n_prime = (n_pucch-c*cfg->N_cs/cfg->delta_pucch_shift)%(c*SRSLTE_NRE/cfg->delta_pucch_shift);
  }
  if (ns%2) {
    if (n_pucch >= c*cfg->N_cs/cfg->delta_pucch_shift) {
      n_prime = (c*(n_prime+1))%(c*SRSLTE_NRE/cfg->delta_pucch_shift+1)-1;
    } else {
      uint32_t d=SRSLTE_CP_ISNORM(cp)?2:0;
      uint32_t h=(n_prime+d)%(c*N_prime/cfg->delta_pucch_shift);
      n_prime = (h/c)+(h%c)*N_prime/cfg->delta_pucch_shift;
    }
  }
  
  if (n_prime_ns) {
    *n_prime_ns = n_prime; 
  }
  
  uint32_t n_oc_div = (!is_dmrs && SRSLTE_CP_ISEXT(cp))?2:1;

  uint32_t n_oc = n_prime*cfg->delta_pucch_shift/N_prime;
  if (!is_dmrs && SRSLTE_CP_ISEXT(cp)) {
    n_oc *= 2; 
  }
  if (n_oc_ptr) {
    *n_oc_ptr = n_oc; 
  }
  uint32_t n_cs = 0; 
  if (SRSLTE_CP_ISNORM(cp)) {
    n_cs = (n_cs_cell[ns][l]+(n_prime*cfg->delta_pucch_shift+(n_oc%cfg->delta_pucch_shift))%N_prime)%12;
  } else {
    n_cs = (n_cs_cell[ns][l]+(n_prime*cfg->delta_pucch_shift+n_oc/n_oc_div)%N_prime)%12;    
  }
  
  return 2 * M_PI * (n_cs) / 12;
}
/* Map PUCCH symbols to physical resources according to 5.4.3 in 36.211 */
static int pucch_put(srslte_pucch_t *q, srslte_pucch_format_t format, uint32_t n_pucch, cf_t *output) {
  int ret = SRSLTE_ERROR_INVALID_INPUTS; 
  if (q && output) {
    ret = SRSLTE_ERROR; 
    uint32_t nsymbols = SRSLTE_CP_ISNORM(q->cell.cp)?SRSLTE_CP_NORM_NSYMB:SRSLTE_CP_EXT_NSYMB;

    // Determine m 
    uint32_t m = srslte_pucch_m(&q->pucch_cfg, format, n_pucch, q->cell.cp); 
    
    uint32_t N_sf = get_N_sf(format);
    for (uint32_t ns=0;ns<2;ns++) {
      // Determine n_prb 
      uint32_t n_prb = m/2; 
      if ((m+ns)%2) {
        n_prb = q->cell.nof_prb-1-m/2; 
      }

      if (n_prb < q->cell.nof_prb) {
        for (uint32_t i=0;i<N_sf;i++) {
          uint32_t l = get_pucch_symbol(i, format, q->cell.cp);
          memcpy(&output[SRSLTE_RE_IDX(q->cell.nof_prb, l+ns*nsymbols, n_prb*SRSLTE_NRE)], 
                &q->z[i*SRSLTE_NRE+ns*N_sf*SRSLTE_NRE], 
                SRSLTE_NRE*sizeof(cf_t));
        }        
      } else {
        return SRSLTE_ERROR; 
      }
    }
    
    ret = SRSLTE_SUCCESS; 
  }
  return ret;   
}


/** Initializes the PDCCH transmitter and receiver */
int srslte_pucch_init(srslte_pucch_t *q, srslte_cell_t cell) {
  int ret = SRSLTE_ERROR_INVALID_INPUTS;
  if (q != NULL && srslte_cell_isvalid(&cell)) {
    ret = SRSLTE_ERROR;
    bzero(q, sizeof(srslte_pucch_t));
    
    q->cell = cell; 
    srslte_pucch_cfg_default(&q->pucch_cfg);
    
    // Precompute group hopping values u. 
    if (srslte_group_hopping_f_gh(q->f_gh, q->cell.id)) {
      return SRSLTE_ERROR;
    }
    
    if (srslte_pucch_n_cs_cell(q->cell, q->n_cs_cell)) {
      return SRSLTE_ERROR;
    }

    ret = SRSLTE_SUCCESS;
  }
  return ret;
}

void srslte_pucch_free(srslte_pucch_t *q) {
 bzero(q, sizeof(srslte_pucch_t));
}

bool srslte_pucch_set_cfg(srslte_pucch_t *q, srslte_pucch_cfg_t *cfg)
{
  if (srslte_pucch_cfg_isvalid(cfg, q->cell.nof_prb)) {
    memcpy(&q->pucch_cfg, cfg, sizeof(srslte_pucch_cfg_t));      
    return true; 
  } else {
    return false; 
  }
}

static cf_t uci_encode_format1() {
  return 1.0;
}

static cf_t uci_encode_format1a(uint8_t bit) {
  return bit?-1.0:1.0;
}

static cf_t uci_encode_format1b(uint8_t bits[2]) {
  if (bits[0] == 0) {
    if (bits[1] == 0) {
      return 1;  
    } else {
      return -I; 
    }
  } else {
    if (bits[1] == 0) {
      return I;  
    } else {
      return -1.0; 
    }    
  }
}

/* Encode PUCCH bits according to Table 5.4.1-1 in Section 5.4.1 of 36.211 */
static int uci_mod_bits(srslte_pucch_t *q, srslte_pucch_format_t format, uint8_t bits[SRSLTE_PUCCH_MAX_BITS], cf_t *d_0)
{  
  if (d_0) {
    uint8_t tmp[2];
    switch(format) {
      case SRSLTE_PUCCH_FORMAT_1:
        *d_0 = uci_encode_format1();
        break;
      case SRSLTE_PUCCH_FORMAT_1A:
        *d_0 = uci_encode_format1a(bits[0]);
        break;
      case SRSLTE_PUCCH_FORMAT_1B:
        tmp[0] = bits[0];
        tmp[1] = bits[1];
        *d_0 = uci_encode_format1b(tmp);
        break;
      default:
        fprintf(stderr, "PUCCH format 2 not supported\n");
        return SRSLTE_ERROR;
    }      
  }
  return SRSLTE_SUCCESS;
}

// Declare this here, since we can not include refsignal_ul.h
void srslte_refsignal_r_uv_arg_1prb(float *arg, uint32_t u);

/* Encode, modulate and resource mapping of PUCCH bits according to Section 5.4.1 of 36.211 */
int srslte_pucch_encode(srslte_pucch_t* q, srslte_pucch_format_t format, uint32_t n_pucch, uint32_t sf_idx, uint8_t bits[SRSLTE_PUCCH_MAX_BITS], cf_t *sf_symbols) 
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;
  if (q          != NULL && 
      sf_symbols != NULL)
  {
    ret = SRSLTE_ERROR; 
    cf_t d_0 = 0; 
    if (uci_mod_bits(q, format, bits, &d_0)) {
      fprintf(stderr, "Error encoding PUCCH bits\n");
      return SRSLTE_ERROR; 
    }
    uint32_t N_sf=get_N_sf(format); 
    for (uint32_t ns=2*sf_idx;ns<2*(sf_idx+1);ns++) {
      // Get group hopping number u 
      uint32_t f_gh=0; 
      if (q->pucch_cfg.group_hopping_en) {
        f_gh = q->f_gh[ns];
      }
      uint32_t u = (f_gh + (q->cell.id%30))%30;
      
      srslte_refsignal_r_uv_arg_1prb(q->tmp_arg, u); 

      for (uint32_t m=0;m<N_sf;m++) {
        uint32_t l = get_pucch_symbol(m, format, q->cell.cp);
        uint32_t n_prime_ns;
        uint32_t n_oc;        
        float alpha = srslte_pucch_alpha(q->n_cs_cell, &q->pucch_cfg, n_pucch, q->cell.cp, true, ns, l, &n_oc, &n_prime_ns);
        float S_ns = 0; 

        if (n_prime_ns%2) {
          S_ns = M_PI/2;
        }
        DEBUG("PUCCH d_0: %.1f+%.1fi, alpha: %.1f, n_oc: %d, n_prime_ns: %d\n", __real__ d_0, __imag__ d_0, alpha, n_oc, n_prime_ns);
        for (uint32_t n=0;n<SRSLTE_PUCCH_N_SEQ;n++) {
          q->z[(ns%2)*N_sf*SRSLTE_PUCCH_N_SEQ+m*SRSLTE_PUCCH_N_SEQ+n] = q->pucch_cfg.beta_pucch*d_0*w_n_oc[n_oc%3][m]*cexpf(I*(q->tmp_arg[n]+alpha*n+S_ns));
        }        
      }      
    }    
    
    if (pucch_put(q, format, n_pucch, sf_symbols)) {
      fprintf(stderr, "Error putting PUCCH symbols\n");
      return SRSLTE_ERROR; 
    }
    ret = SRSLTE_SUCCESS; 
  }

  return ret;     
}

  