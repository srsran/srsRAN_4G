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
#include <complex.h>

#include "srslte/ch_estimation/refsignal_ul.h"
#include "srslte/phch/pucch.h"
#include "srslte/common/sequence.h"
#include "srslte/common/phy_common.h"
#include "srslte/scrambling/scrambling.h"
#include "srslte/utils/debug.h"
#include "srslte/utils/vector.h"

#define MAX_PUSCH_RE(cp) (2 * SRSLTE_CP_NSYMB(cp) * 12)

uint32_t pucch_symbol_format1_cpnorm[4] = {0, 1, 5, 6};
uint32_t pucch_symbol_format1_cpext[4] = {0, 1, 4, 5};
uint32_t pucch_symbol_format2_cpnorm[5] = {0, 2, 3, 4, 6};
uint32_t pucch_symbol_format2_cpext[5] = {0, 1, 2, 4, 5};

float w_n_oc[2][3][4] = {
              // Table 5.4.1-2 Orthogonal sequences w for N_sf=4 (complex argument)
                      {{0, 0, 0, 0},
                      {0,M_PI, 0, M_PI},
                      {0,M_PI, M_PI, 0}}, 
              // Table 5.4.1-3 Orthogonal sequences w for N_sf=3
                      {{0, 0, 0, 0},
                      {0,2*M_PI/3, 4*M_PI/3,0},
                      {0,4*M_PI/3,2*M_PI/3,0}}, 
  
};


/* Verify PUCCH configuration as given in Section 5.4 36.211 */
bool srslte_pucch_cfg_isvalid(srslte_pucch_cfg_t *cfg, uint32_t nof_prb) {
  if (cfg->delta_pucch_shift > 0 && cfg->delta_pucch_shift < 4 &&
      cfg->N_cs < 8 && (cfg->N_cs%cfg->delta_pucch_shift) == 0 && 
      cfg->n_rb_2 < nof_prb) {
    return true; 
  } else {
    return false;    
  }
}

// Verifies n_2_pucch as defined in 5.4
bool srslte_pucch_n2_isvalid(srslte_pucch_cfg_t *cfg, uint32_t n_pucch_2) {
  if (n_pucch_2 < cfg->n_rb_2*SRSLTE_NRE+(uint32_t) ceilf((float) cfg->N_cs/8)*(SRSLTE_NRE-cfg->N_cs-2)) {
    return true; 
  } else {
    return false; 
  }
}

void srslte_pucch_cfg_default(srslte_pucch_cfg_t *cfg) {
  cfg->delta_pucch_shift = 1; 
}

uint32_t get_N_sf(srslte_pucch_format_t format, uint32_t slot_idx, bool shortened) {
  switch (format) {
    case SRSLTE_PUCCH_FORMAT_1:
    case SRSLTE_PUCCH_FORMAT_1A:
    case SRSLTE_PUCCH_FORMAT_1B:
      if (!slot_idx) {
        return 4; 
      } else {
        return shortened?3:4; 
      }
    case SRSLTE_PUCCH_FORMAT_2:
    case SRSLTE_PUCCH_FORMAT_2A:
    case SRSLTE_PUCCH_FORMAT_2B:
      return 5; 
  }
  return 0; 
}

uint32_t srslte_pucch_nof_symbols(srslte_pucch_cfg_t *cfg, srslte_pucch_format_t format, bool shortened) {
  uint32_t len=0;
  for (uint32_t ns=0;ns<2;ns++) {
    len += SRSLTE_NRE*get_N_sf(format, ns, shortened);
  }
  return len; 
}

// Number of bits per subframe (M_bit) Table 5.4-1 36.211 
uint32_t srslte_pucch_nbits_format(srslte_pucch_format_t format) {
  switch(format) {
    case SRSLTE_PUCCH_FORMAT_1:
      return 0; 
    case SRSLTE_PUCCH_FORMAT_1A:
      return 1; 
    case SRSLTE_PUCCH_FORMAT_1B:
      return 2; 
    case SRSLTE_PUCCH_FORMAT_2:
      return 20; 
    case SRSLTE_PUCCH_FORMAT_2A:
      return 21; 
    case SRSLTE_PUCCH_FORMAT_2B:
      return 22; 
  }
  return 0; 
}

uint32_t get_pucch_symbol(uint32_t m, srslte_pucch_format_t format, srslte_cp_t cp) {
  switch (format) {
    case SRSLTE_PUCCH_FORMAT_1:
    case SRSLTE_PUCCH_FORMAT_1A:
    case SRSLTE_PUCCH_FORMAT_1B:
      if (m < 4) {
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
      if (m < 5) {
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


/* Calculates alpha for format 1/a/b according to 5.5.2.2.2 (is_dmrs=true) or 5.4.1 (is_dmrs=false) of 36.211 */
float srslte_pucch_alpha_format1(uint32_t n_cs_cell[SRSLTE_NSLOTS_X_FRAME][SRSLTE_CP_NORM_NSYMB], 
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
    n_cs = (n_cs_cell[ns][l]+(n_prime*cfg->delta_pucch_shift+(n_oc%cfg->delta_pucch_shift))%N_prime)%SRSLTE_NRE;
  } else {
    n_cs = (n_cs_cell[ns][l]+(n_prime*cfg->delta_pucch_shift+n_oc/n_oc_div)%N_prime)%SRSLTE_NRE;    
  }
  
  return 2 * M_PI * (n_cs) / SRSLTE_NRE;
}

/* Calculates alpha for format 2/a/b according to 5.4.2 of 36.211 */
float srslte_pucch_alpha_format2(uint32_t n_cs_cell[SRSLTE_NSLOTS_X_FRAME][SRSLTE_CP_NORM_NSYMB], 
                                   srslte_pucch_cfg_t *cfg,
                                   uint32_t n_pucch, 
                                   uint32_t ns, uint32_t l) 
{
  uint32_t n_prime = n_pucch%SRSLTE_NRE; 
  if (n_pucch >= SRSLTE_NRE*cfg->n_rb_2) {
    n_prime = (n_pucch + cfg->N_cs + 1)%SRSLTE_NRE;
  }
  if (ns%2) {
    n_prime = (SRSLTE_NRE*(n_prime+1))%(SRSLTE_NRE+1)-1;
    if (n_pucch >= SRSLTE_NRE*cfg->n_rb_2) {
      int x = (SRSLTE_NRE-2-(int) n_pucch)%SRSLTE_NRE;
      if (x >= 0) {
        n_prime = (uint32_t) x; 
      } else {
        n_prime = SRSLTE_NRE+x;
      }
    }
  }
  uint32_t n_cs = (n_cs_cell[ns][l]+n_prime)%SRSLTE_NRE;
  float alpha = 2 * M_PI * (n_cs) / SRSLTE_NRE;
  DEBUG("n_pucch: %d, ns: %d, l: %d, n_prime: %d, n_cs: %d, alpha=%f\n", n_pucch, ns, l, n_prime, n_cs, alpha); 
  return alpha; 
}

/* Map PUCCH symbols to physical resources according to 5.4.3 in 36.211 */
static int pucch_put(srslte_pucch_t *q, srslte_pucch_format_t format, uint32_t n_pucch, cf_t *output) {
  int ret = SRSLTE_ERROR_INVALID_INPUTS; 
  if (q && output) {
    ret = SRSLTE_ERROR; 
    uint32_t nsymbols = SRSLTE_CP_ISNORM(q->cell.cp)?SRSLTE_CP_NORM_NSYMB:SRSLTE_CP_EXT_NSYMB;

    // Determine m 
    uint32_t m = srslte_pucch_m(&q->pucch_cfg, format, n_pucch, q->cell.cp); 
    
    uint32_t N_sf_0 = get_N_sf(format, 0, q->shortened);
    for (uint32_t ns=0;ns<2;ns++) {
      uint32_t N_sf = get_N_sf(format, ns%2, q->shortened);
      // Determine n_prb 
      uint32_t n_prb = m/2; 
      if ((m+ns)%2) {
        n_prb = q->cell.nof_prb-1-m/2; 
      }

      if (n_prb < q->cell.nof_prb) {
        for (uint32_t i=0;i<N_sf;i++) {
          uint32_t l = get_pucch_symbol(i, format, q->cell.cp);
          memcpy(&output[SRSLTE_RE_IDX(q->cell.nof_prb, l+ns*nsymbols, n_prb*SRSLTE_NRE)], 
                &q->z[i*SRSLTE_NRE+ns*N_sf_0*SRSLTE_NRE], 
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
    q->rnti_is_set = false; 
    
    srslte_pucch_cfg_default(&q->pucch_cfg);
    
    if (srslte_modem_table_lte(&q->mod, SRSLTE_MOD_QPSK)) {
      return SRSLTE_ERROR;
    }

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
  if (q->rnti_is_set) {
    for (uint32_t sf_idx=0;sf_idx<SRSLTE_NSUBFRAMES_X_FRAME;sf_idx++) {
      srslte_sequence_free(&q->seq_f2[sf_idx]);
    }
  }
  srslte_modem_table_free(&q->mod);
  bzero(q, sizeof(srslte_pucch_t));
}

int srslte_pucch_set_crnti(srslte_pucch_t *q, uint16_t c_rnti) {
  for (uint32_t sf_idx=0;sf_idx<SRSLTE_NSUBFRAMES_X_FRAME;sf_idx++) {
    // Precompute scrambling sequence for pucch format 2    
    if (srslte_sequence_pucch(&q->seq_f2[sf_idx], c_rnti, 2*sf_idx, q->cell.id)) {
      fprintf(stderr, "Error computing PUCCH Format 2 scrambling sequence\n");
      return SRSLTE_ERROR; 
    }        
  }
  q->rnti_is_set = true; 
  return SRSLTE_SUCCESS; 
}

bool srslte_pucch_set_cfg(srslte_pucch_t *q, srslte_pucch_cfg_t *cfg, bool group_hopping_en)
{
  q->group_hopping_en = group_hopping_en; 
  if (cfg) {
    if (srslte_pucch_cfg_isvalid(cfg, q->cell.nof_prb)) {
      memcpy(&q->pucch_cfg, cfg, sizeof(srslte_pucch_cfg_t));      
      return true; 
    } else {
      return false; 
    }    
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

/* Modulates bit 20 and 21 for Formats 2a and 2b as in Table 5.4.2-1 in 36.211 */
int srslte_pucch_format2ab_mod_bits(srslte_pucch_format_t format, uint8_t bits[2], cf_t *d_10) {
  if (d_10) {
    if (format == SRSLTE_PUCCH_FORMAT_2A) {
      *d_10 = bits[0]?-1.0:1.0; 
      return SRSLTE_SUCCESS;
    } else if (format == SRSLTE_PUCCH_FORMAT_2B) {
      if (bits[0] == 0) {
        if (bits[1] == 0) {
          *d_10 = 1.0; 
        } else {
          *d_10 = -I; 
        }
      } else {
        if (bits[1] == 0) {
          *d_10 = I; 
        } else {
          *d_10 = -1.0; 
        }        
      }
      return SRSLTE_SUCCESS;
    } else {
      return SRSLTE_ERROR; 
    }    
  } else {
    return SRSLTE_ERROR; 
  }
}

/* Encode PUCCH bits according to Table 5.4.1-1 in Section 5.4.1 of 36.211 */
static int uci_mod_bits(srslte_pucch_t *q, srslte_pucch_format_t format, uint8_t bits[SRSLTE_PUCCH_MAX_BITS], uint32_t sf_idx)
{  
  uint8_t tmp[2];
  
  switch(format) {
    case SRSLTE_PUCCH_FORMAT_1:
      q->d[0] = uci_encode_format1();
      break;
    case SRSLTE_PUCCH_FORMAT_1A:
      q->d[0] = uci_encode_format1a(bits[0]);
      break;
    case SRSLTE_PUCCH_FORMAT_1B:
      tmp[0] = bits[0];
      tmp[1] = bits[1];
      q->d[0] = uci_encode_format1b(tmp);
      break;
    case SRSLTE_PUCCH_FORMAT_2:
    case SRSLTE_PUCCH_FORMAT_2A:
    case SRSLTE_PUCCH_FORMAT_2B:
      memcpy(q->bits_scram, bits, SRSLTE_PUCCH_MAX_BITS*sizeof(uint8_t));
      srslte_scrambling_b(&q->seq_f2[sf_idx], q->bits_scram);
      srslte_mod_modulate(&q->mod, q->bits_scram, q->d, SRSLTE_PUCCH_MAX_BITS);
      break;
    default:
      fprintf(stderr, "PUCCH format 2 not supported\n");
      return SRSLTE_ERROR;
  }
  return SRSLTE_SUCCESS;
}

// Declare this here, since we can not include refsignal_ul.h
void srslte_refsignal_r_uv_arg_1prb(float *arg, uint32_t u);

/* Encode, modulate and resource mapping of PUCCH bits according to Section 5.4.1 of 36.211 */
int srslte_pucch_encode(srslte_pucch_t* q, srslte_pucch_format_t format, 
                        uint32_t n_pucch, uint32_t sf_idx, uint8_t bits[SRSLTE_PUCCH_MAX_BITS], cf_t *sf_symbols) 
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;
  if (q          != NULL && 
      sf_symbols != NULL)
  {
    ret = SRSLTE_ERROR; 
    
    // Shortened PUCCH happen in every cell-specific SRS subframes for Format 1/1a/1b
    if (q->pucch_cfg.srs_configured && format < SRSLTE_PUCCH_FORMAT_2) {
      q->shortened = false; 
      // If CQI is not transmitted, PUCCH will be normal unless ACK/NACK and SRS simultaneous transmission is enabled 
      if (q->pucch_cfg.srs_simul_ack) {
        // If simultaneous ACK and SRS is enabled, PUCCH is shortened in cell-specific SRS subframes
        if (srslte_refsignal_srs_send_cs(q->pucch_cfg.srs_cs_subf_cfg, sf_idx) == 1) {
          q->shortened = true; 
        }
      }
    }
    
    if (format >= SRSLTE_PUCCH_FORMAT_2 && !q->rnti_is_set) {
      fprintf(stderr, "Error encoding PUCCH: C-RNTI must be set before encoding PUCCH Format 2/2a/2b\n");
      return SRSLTE_ERROR; 
    }
    if (uci_mod_bits(q, format, bits, sf_idx)) {
      fprintf(stderr, "Error encoding PUCCH bits\n");
      return SRSLTE_ERROR; 
    }
    uint32_t N_sf_0 = get_N_sf(format, 0, q->shortened);
    for (uint32_t ns=2*sf_idx;ns<2*(sf_idx+1);ns++) {
      uint32_t N_sf = get_N_sf(format, ns%2, q->shortened);
      DEBUG("ns=%d, N_sf=%d\n", ns, N_sf);
      // Get group hopping number u 
      uint32_t f_gh=0; 
      if (q->group_hopping_en) {
        f_gh = q->f_gh[ns];
      }
      uint32_t u = (f_gh + (q->cell.id%30))%30;

      srslte_refsignal_r_uv_arg_1prb(q->tmp_arg, u); 
      uint32_t N_sf_widx = N_sf==3?1:0;
      for (uint32_t m=0;m<N_sf;m++) {
        uint32_t l = get_pucch_symbol(m, format, q->cell.cp);
        float alpha=0; 
        if (format >= SRSLTE_PUCCH_FORMAT_2) {
          alpha = srslte_pucch_alpha_format2(q->n_cs_cell, &q->pucch_cfg, n_pucch, ns, l);                 
          for (uint32_t n=0;n<SRSLTE_PUCCH_N_SEQ;n++) {
            q->z[(ns%2)*N_sf*SRSLTE_PUCCH_N_SEQ+m*SRSLTE_PUCCH_N_SEQ+n] = q->d[(ns%2)*N_sf+m]*cexpf(I*(q->tmp_arg[n]+alpha*n));
          }
        } else {
          uint32_t n_prime_ns=0;
          uint32_t n_oc=0;        
          alpha = srslte_pucch_alpha_format1(q->n_cs_cell, &q->pucch_cfg, n_pucch, q->cell.cp, true, ns, l, &n_oc, &n_prime_ns);          
          float S_ns = 0; 
          if (n_prime_ns%2) {
            S_ns = M_PI/2;
          }
          DEBUG("PUCCH d_0: %.1f+%.1fi, alpha: %.1f, n_oc: %d, n_prime_ns: %d, n_rb_2=%d\n", 
                __real__ q->d[0], __imag__ q->d[0], alpha, n_oc, n_prime_ns, q->pucch_cfg.n_rb_2);
          for (uint32_t n=0;n<SRSLTE_PUCCH_N_SEQ;n++) {
            q->z[(ns%2)*N_sf_0*SRSLTE_PUCCH_N_SEQ+m*SRSLTE_PUCCH_N_SEQ+n] = 
              q->d[0]*cexpf(I*(w_n_oc[N_sf_widx][n_oc%3][m]+q->tmp_arg[n]+alpha*n+S_ns));
          }        
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

  
