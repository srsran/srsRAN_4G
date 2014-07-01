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
#include <math.h>

#include "liblte/phy/phch/dci.h"
#include "liblte/phy/phch/regs.h"
#include "liblte/phy/phch/pdcch.h"
#include "liblte/phy/common/phy_common.h"
#include "liblte/phy/utils/bit.h"
#include "liblte/phy/utils/vector.h"
#include "liblte/phy/utils/debug.h"

#define PDCCH_NOF_FORMATS               4
#define PDCCH_FORMAT_NOF_CCE(i)         (1<<i)
#define PDCCH_FORMAT_NOF_REGS(i)        ((1<<i)*9)
#define PDCCH_FORMAT_NOF_BITS(i)        ((1<<i)*72)

#define NOF_COMMON_FORMATS      2
const dci_format_t common_formats[NOF_COMMON_FORMATS] = { Format1A, Format1C };

#define NOF_UE_FORMATS          2
const dci_format_t ue_formats[NOF_UE_FORMATS] = { Format0, Format1 }; // 1A has the same payload as 0

#define MIN(a,b) ((a>b)?b:a)

void set_cfi(pdcch_t *q, uint32_t cfi);

/**
 * 36.213 9.1
 */
int gen_common_search(dci_candidate_t *c, uint32_t nof_cce, uint32_t nof_bits,
    uint16_t rnti) {
  int i, l, L, k;
  k = 0;
  for (l = 3; l > 1; l--) {
    L = (1 << l);
    for (i = 0; i < MIN(nof_cce,16) / (L); i++) {
      c[k].L = l;
      c[k].nof_bits = nof_bits;
      c[k].rnti = rnti;
      c[k].ncce = (L) * (i % (nof_cce / (L)));
      INFO("Common SS Candidate %d: RNTI: 0x%x, nCCE: %d, Nbits: %d, L: %d\n",
          k, c[k].rnti, c[k].ncce, c[k].nof_bits, c[k].L);
      k++;
    }
  }
  return k;
}

/**
 * 36.213 9.1
 */
int gen_ue_search(dci_candidate_t *c, uint32_t nof_cce, uint32_t nof_bits,
    uint16_t rnti, uint32_t subframe) {
  int i, l, L, k, m;
  unsigned int Yk;
  const int S[4] = { 6, 12, 8, 16 };
  k = 0;

  if (VERBOSE_ISDEBUG()) {
    printf("NofBits=%d, RNTI: 0x%x, SF=%d (n, L): ", nof_bits, rnti, subframe);
  }
  for (l = 3; l >= 0; l--) {
    L = (1 << l);
    for (i = 0; i < MIN(nof_cce / L, 16 / S[l]); i++) {
      c[k].L = l;
      c[k].nof_bits = nof_bits;
      c[k].rnti = rnti;
      Yk = rnti;
      for (m = 0; m < subframe; m++) {
        Yk = (39827 * Yk) % 65537;
      }
      c[k].ncce = L * ((Yk + i) % (nof_cce / L));
      if (VERBOSE_ISDEBUG()) {
        printf("(%d, %d), ", c[k].ncce, c[k].L);
      }
      if (c[k].ncce + PDCCH_FORMAT_NOF_CCE(c[k].L) > nof_cce    || 
          nof_bits > DCI_MAX_BITS) {
        fprintf(stderr, "Illegal DCI message\n");
        return LIBLTE_ERROR;
      }
      k++;
    }
  }
  if (VERBOSE_ISDEBUG()) {
    printf("\n");
  }
  return k;
}


/** 36.213 v9.3 Table 7.1-5
 * user-specific search space. Currently supported transmission Mode 1:
 * DCI Format 1A and 1 + PUSCH scheduling format 0
 */
int pdcch_init_search_ue(pdcch_t *q, uint16_t c_rnti, uint32_t cfi) {
  int k, i, r;
  uint32_t n; 

  set_cfi(q, cfi);

  pdcch_search_t *s = &q->search_mode[SEARCH_UE];
  for (n = 0; n < NSUBFRAMES_X_FRAME; n++) {
    dci_candidate_t *c = s->candidates[n];

    if (!n) s->nof_candidates = 0;

    // Expect Formats 1, 1A, 0
    k = 0;
    for (i = 0; i < NOF_UE_FORMATS && k < MAX_CANDIDATES; i++) {
      r = gen_ue_search(&c[k], q->nof_cce,
          dci_format_sizeof(ue_formats[i], q->cell.nof_prb), c_rnti, n);
      if (r < 0) {
        fprintf(stderr, "Error generating UE-specific search space\n");
        return r;
      }
      k += r;
    }
    s->nof_candidates = k;
  }
  INFO("Initiated %d candidate(s) in the UE-specific search space for C-RNTI: 0x%x\n",
      s->nof_candidates, c_rnti);
  q->current_search_mode = SEARCH_UE;
 
  return LIBLTE_SUCCESS; 
}


int pdcch_init_common(pdcch_t *q, pdcch_search_t *s, uint16_t rnti) {
  int k, r, i;
  dci_candidate_t *c = s->candidates[0];
  s->nof_candidates = 0;
  // Format 1A and 1C L=4 and L=8, 4 and 2 candidates, only if nof_cce > 16
  k = 0;
  for (i = 0; i < NOF_COMMON_FORMATS && k < MAX_CANDIDATES; i++) {
    r = gen_common_search(&c[k], q->nof_cce,
        dci_format_sizeof(common_formats[i], q->cell.nof_prb), SIRNTI);
    if (r < 0) {
      return r;
    }
    k += r;
  }
  s->nof_candidates=k;
  INFO("Initiated %d candidate(s) in the Common search space for RNTI: 0x%x\n",
      s->nof_candidates, rnti);
  
  return LIBLTE_SUCCESS;
}

/** 36.213 v9.3 Table 7.1-1: System Information DCI messages
 * Expect DCI formats 1C and 1A in the common search space
 */
int pdcch_init_search_si(pdcch_t *q, uint32_t cfi) {
  set_cfi(q, cfi);
  int r = pdcch_init_common(q, &q->search_mode[SEARCH_SI], SIRNTI);
  if (r >= 0) {
    q->current_search_mode = SEARCH_SI;    
  }
  return r;
}

/** 36.213 v9.3 Table 7.1-3
 * Expect DCI formats 1C and 1A in the common search space
 */
int pdcch_init_search_ra(pdcch_t *q, uint16_t ra_rnti, uint32_t cfi) {
  set_cfi(q, cfi);
  int r = pdcch_init_common(q, &q->search_mode[SEARCH_RA], ra_rnti);
  if (r >= 0) {
    q->current_search_mode = SEARCH_RA;    
  }
  return r;
}

void pdcch_set_search_si(pdcch_t *q) {
  q->current_search_mode = SEARCH_SI;
}
void pdcch_set_search_ue(pdcch_t *q) {
  q->current_search_mode = SEARCH_UE;
}
void pdcch_set_search_ra(pdcch_t *q) {
  q->current_search_mode = SEARCH_RA;
}

void set_cfi(pdcch_t *q, uint32_t cfi) {
  if (cfi > 0 && cfi < 4) {
    q->nof_regs = (regs_pdcch_nregs(q->regs, cfi) / 9) * 9;
    q->nof_cce = q->nof_regs / 9;
    q->nof_symbols = 4 * q->nof_regs;
    q->nof_bits = 2 * q->nof_symbols;    
  } 
}

/** Initializes the PDCCH transmitter and receiver */
int pdcch_init(pdcch_t *q, regs_t *regs, lte_cell_t cell) {
  int ret = LIBLTE_ERROR_INVALID_INPUTS;
  int i;

  if (q                         != NULL &&
      regs                      != NULL &&
      lte_cell_isvalid(&cell)) 
  {   
    ret = LIBLTE_ERROR;
    bzero(q, sizeof(pdcch_t));
    q->cell = cell;
    q->regs = regs;
    q->current_search_mode = SEARCH_NONE;

    /* Now allocate memory for the maximum number of REGs (CFI=3)
    */
    set_cfi(q, 3);
    q->max_bits = q->nof_bits;

    INFO("Init PDCCH: %d CCEs (%d REGs), %d bits, %d symbols, %d ports\n",
        q->nof_cce, q->nof_regs, q->nof_bits, q->nof_symbols, q->cell.nof_ports);

    if (modem_table_std(&q->mod, LTE_QPSK, true)) {
      goto clean;
    }
    if (crc_init(&q->crc, LTE_CRC16, 16)) {
      goto clean;
    }

    demod_soft_init(&q->demod);
    demod_soft_table_set(&q->demod, &q->mod);
    demod_soft_alg_set(&q->demod, APPROX);

    for (i = 0; i < NSUBFRAMES_X_FRAME; i++) {
      if (sequence_pdcch(&q->seq_pdcch[i], 2 * i, q->cell.id, q->nof_bits)) {
        goto clean;
      }
    }

    int poly[3] = { 0x6D, 0x4F, 0x57 };
    if (viterbi_init(&q->decoder, viterbi_37, poly, DCI_MAX_BITS + 16, true)) {
      goto clean;
    }

    q->pdcch_e = malloc(sizeof(char) * q->nof_bits);
    if (!q->pdcch_e) {
      goto clean;
    }

    q->pdcch_llr = malloc(sizeof(float) * q->nof_bits);
    if (!q->pdcch_llr) {
      goto clean;
    }

    q->pdcch_d = malloc(sizeof(cf_t) * q->nof_symbols);
    if (!q->pdcch_d) {
      goto clean;
    }

    for (i = 0; i < MAX_PORTS; i++) {
      q->ce[i] = malloc(sizeof(cf_t) * q->nof_symbols);
      if (!q->ce[i]) {
        goto clean;
      }
      q->pdcch_x[i] = malloc(sizeof(cf_t) * q->nof_symbols);
      if (!q->pdcch_x[i]) {
        goto clean;
      }
      q->pdcch_symbols[i] = malloc(sizeof(cf_t) * q->nof_symbols);
      if (!q->pdcch_symbols[i]) {
        goto clean;
      }
    }

    ret = LIBLTE_SUCCESS;
  }
  clean: 
  if (ret == LIBLTE_ERROR) {
    pdcch_free(q);
  }
  return ret;
}

void pdcch_free(pdcch_t *q) {
  int i;

  if (q->pdcch_e) {
    free(q->pdcch_e);
  }
  if (q->pdcch_llr) {
    free(q->pdcch_llr);
  }
  if (q->pdcch_d) {
    free(q->pdcch_d);
  }
  for (i = 0; i < MAX_PORTS; i++) {
    if (q->ce[i]) {
      free(q->ce[i]);
    }
    if (q->pdcch_x[i]) {
      free(q->pdcch_x[i]);
    }
    if (q->pdcch_symbols[i]) {
      free(q->pdcch_symbols[i]);
    }
  }

  for (i = 0; i < NSUBFRAMES_X_FRAME; i++) {
    sequence_free(&q->seq_pdcch[i]);
  }

  modem_table_free(&q->mod);
  viterbi_free(&q->decoder);
}

/** 36.212 5.3.3.2 to 5.3.3.4
 *
 * Returns XOR between parity and remainder bits
 *
 * TODO: UE transmit antenna selection CRC mask
 */
int dci_decode(pdcch_t *q, float *e, char *data, uint32_t E, uint32_t nof_bits, uint16_t *crc) {

  float tmp[3 * (DCI_MAX_BITS + 16)];
  uint16_t p_bits, crc_res;
  char *x;

  if (q         != NULL         &&
      data      != NULL         &&
      E         < q->max_bits   && 
      nof_bits  < DCI_MAX_BITS)
  {

    /* unrate matching */
    rm_conv_rx(e, E, tmp, 3 * (nof_bits + 16));

    DEBUG("Viterbi input: ", 0);
    if (VERBOSE_ISDEBUG()) {
      vec_fprint_f(stdout, tmp, 3 * (nof_bits + 16));
    }

    /* viterbi decoder */
    viterbi_decode_f(&q->decoder, tmp, data, nof_bits + 16);

    if (VERBOSE_ISDEBUG()) {
      bit_fprint(stdout, data, nof_bits + 16);
    }

    x = &data[nof_bits];
    p_bits = (uint16_t) bit_unpack(&x, 16);
    crc_res = ((uint16_t) crc_checksum(&q->crc, data, nof_bits) & 0xffff);
    DEBUG("p_bits: 0x%x, crc_res: 0x%x, tot: 0x%x\n", p_bits, crc_res,
        p_bits ^ crc_res);
    
    if (crc) {
      *crc = p_bits ^ crc_res; 
    }
    return LIBLTE_SUCCESS;
  } else {
    return LIBLTE_ERROR_INVALID_INPUTS;
  }
}

int pdcch_decode_candidate(pdcch_t *q, float *llr, dci_candidate_t *c,
    dci_msg_t *msg) {
  uint16_t crc_res;
  INFO("Trying Candidate: Nbits: %d, E: %3d, nCCE: %d, L: %d, RNTI: 0x%x\n",
      c->nof_bits, PDCCH_FORMAT_NOF_BITS(c->L), c->ncce, c->L, c->rnti);
   
  if (dci_decode(q, &llr[72 * c->ncce], msg->data,
      PDCCH_FORMAT_NOF_BITS(c->L), c->nof_bits, &crc_res)) {
    return LIBLTE_ERROR;
  }

  if (c->rnti == crc_res) {
    memcpy(&msg->location, c, sizeof(dci_candidate_t));
    INFO("FOUND Candidate: Nbits: %d, E: %d, nCCE: %d, L: %d, RNTI: 0x%x\n",
        c->nof_bits, PDCCH_FORMAT_NOF_BITS(c->L), c->ncce, c->L, c->rnti);
    return 1;
  }
  return LIBLTE_SUCCESS;
}

int pdcch_extract_llr(pdcch_t *q, cf_t *slot_symbols, cf_t *ce[MAX_PORTS],
    float *llr, uint32_t nsubframe, uint32_t cfi) {

  /* Set pointers for layermapping & precoding */
  int i;
  cf_t *x[MAX_LAYERS];

  if (q                 != NULL && 
      llr               != NULL && 
      slot_symbols      != NULL && 
      nsubframe         <  10   &&
      cfi               >  0    &&
      cfi               <  4)
  {
    set_cfi(q, cfi);
    
    /* number of layers equals number of ports */
    for (i = 0; i < q->cell.nof_ports; i++) {
      x[i] = q->pdcch_x[i];
    }
    memset(&x[q->cell.nof_ports], 0, sizeof(cf_t*) * (MAX_LAYERS - q->cell.nof_ports));

    /* extract symbols */
    int n = regs_pdcch_get(q->regs, slot_symbols, q->pdcch_symbols[0]);
    if (q->nof_symbols != n) {
      fprintf(stderr, "Expected %d PDCCH symbols but got %d symbols\n",
          q->nof_symbols, n);
      return LIBLTE_ERROR;
    }

    /* extract channel estimates */
    for (i = 0; i < q->cell.nof_ports; i++) {
      n = regs_pdcch_get(q->regs, ce[i], q->ce[i]);
      if (q->nof_symbols != n) {
        fprintf(stderr, "Expected %d PDCCH symbols but got %d symbols\n",
            q->nof_symbols, n);
        return LIBLTE_ERROR;
      }
    }

    /* in control channels, only diversity is supported */
    if (q->cell.nof_ports == 1) {
      /* no need for layer demapping */
      predecoding_single_zf(q->pdcch_symbols[0], q->ce[0], q->pdcch_d,
          q->nof_symbols);
    } else {
      predecoding_diversity_zf(q->pdcch_symbols[0], q->ce, x, q->cell.nof_ports,
          q->nof_symbols);
      layerdemap_diversity(x, q->pdcch_d, q->cell.nof_ports,
          q->nof_symbols / q->cell.nof_ports);
    }

    DEBUG("pdcch d symbols: ", 0);
    if (VERBOSE_ISDEBUG()) {
      vec_fprint_c(stdout, q->pdcch_d, q->nof_symbols);
    }

    /* demodulate symbols */
    demod_soft_sigma_set(&q->demod, 1.0);
    demod_soft_demodulate(&q->demod, q->pdcch_d, q->pdcch_llr, q->nof_symbols);

    DEBUG("llr: ", 0);
    if (VERBOSE_ISDEBUG()) {
      vec_fprint_f(stdout, q->pdcch_llr, q->nof_bits);
    }

    /* descramble */
    scrambling_f_offset(&q->seq_pdcch[nsubframe], llr, 0, q->nof_bits);

    return LIBLTE_SUCCESS;
  } else {
    return LIBLTE_ERROR_INVALID_INPUTS;
  }
}

int pdcch_decode_current_mode(pdcch_t *q, float *llr, dci_t *dci, uint32_t subframe) {
  int k, i;
  int ret; 
  
  if (q->current_search_mode == SEARCH_UE) {
    k = subframe;
  } else {
    k = 0;
  }

  for (i = 0;
      i < q->search_mode[q->current_search_mode].nof_candidates
          && dci->nof_dcis < dci->max_dcis; i++) {
    ret = pdcch_decode_candidate(q, q->pdcch_llr,
        &q->search_mode[q->current_search_mode].candidates[k][i],
        &dci->msg[dci->nof_dcis]);
    if (ret == 1) {
      dci->nof_dcis++;
    } else if (ret == -1) {
      return LIBLTE_ERROR;
    }
  }
  return dci->nof_dcis;
}

int pdcch_decode_si(pdcch_t *q, float *llr, dci_t *dci) {
  pdcch_set_search_si(q);
  return pdcch_decode_current_mode(q, llr, dci, 0);
}
int pdcch_decode_ra(pdcch_t *q, float *llr, dci_t *dci) {
  pdcch_set_search_ra(q);
  return pdcch_decode_current_mode(q, llr, dci, 0);
}
int pdcch_decode_ue(pdcch_t *q, float *llr, dci_t *dci, uint32_t nsubframe) {
  pdcch_set_search_ue(q);
  return pdcch_decode_current_mode(q, llr, dci, nsubframe);
}

/* Decodes PDCCH channels
 *
 * dci->nof_dcis is the size of the dci->msg buffer (ie max number of messages)
 *
 * Returns number of messages stored in dci
 */
int pdcch_decode(pdcch_t *q, cf_t *slot_symbols, cf_t *ce[MAX_PORTS],
    dci_t *dci, uint32_t subframe, uint32_t cfi) {
  
  if (q                 != NULL && 
      dci               != NULL && 
      slot_symbols      != NULL && 
      subframe         <  10   &&
      cfi               >  0    &&
      cfi               <  4)
  {
    if (pdcch_extract_llr(q, slot_symbols, ce, q->pdcch_llr, subframe, cfi)) {
      return LIBLTE_ERROR;
    }

    if (q->current_search_mode != SEARCH_NONE) {
      return pdcch_decode_current_mode(q, q->pdcch_llr, dci, subframe);
    }

    return LIBLTE_SUCCESS;    
  } else {
    return LIBLTE_ERROR_INVALID_INPUTS;
  }
}

void crc_set_mask_rnti(char *crc, uint16_t rnti) {
  int i;
  char mask[16];
  char *r = mask;

  INFO("Mask CRC with RNTI 0x%x\n", rnti);

  bit_pack(rnti, &r, 16);
  for (i = 0; i < 16; i++) {
    crc[i] = (crc[i] + mask[i]) % 2;
  }
}

/** 36.212 5.3.3.2 to 5.3.3.4
 * TODO: UE transmit antenna selection CRC mask
 */
int dci_encode(pdcch_t *q, char *data, char *e, uint32_t nof_bits, uint32_t E,
    uint16_t rnti) {
  convcoder_t encoder;
  char tmp[3 * (DCI_MAX_BITS + 16)];
  
  if (q                 != NULL        && 
      data              != NULL        && 
      e                 != NULL        && 
      nof_bits          < DCI_MAX_BITS &&
      E                 < q->max_bits)
  {

    int poly[3] = { 0x6D, 0x4F, 0x57 };
    encoder.K = 7;
    encoder.R = 3;
    encoder.tail_biting = true;
    memcpy(encoder.poly, poly, 3 * sizeof(int));

    crc_attach(&q->crc, data, nof_bits);
    crc_set_mask_rnti(&data[nof_bits], rnti);

    convcoder_encode(&encoder, data, tmp, nof_bits + 16);

    DEBUG("CConv output: ", 0);
    if (VERBOSE_ISDEBUG()) {
      vec_fprint_b(stdout, tmp, 3 * (nof_bits + 16));
    }

    rm_conv_tx(tmp, 3 * (nof_bits + 16), e, E);
    
    return LIBLTE_SUCCESS;
  } else {
    return LIBLTE_ERROR_INVALID_INPUTS;
  }
}

/** Converts the set of DCI messages to symbols mapped to the slot ready for transmission
 */
int pdcch_encode(pdcch_t *q, dci_t *dci, cf_t *slot_symbols[MAX_PORTS],
    uint32_t nsubframe, uint32_t cfi) {
  int i;
  /* Set pointers for layermapping & precoding */
  cf_t *x[MAX_LAYERS];
  
  if (q                 != NULL && 
      dci               != NULL && 
      slot_symbols      != NULL && 
      nsubframe         <  10   &&
      cfi               >  0    &&
      cfi               <  4)
  {
    set_cfi(q, cfi);

    /* number of layers equals number of ports */
    for (i = 0; i < q->cell.nof_ports; i++) {
      x[i] = q->pdcch_x[i];
    }
    memset(&x[q->cell.nof_ports], 0, sizeof(cf_t*) * (MAX_LAYERS - q->cell.nof_ports));

    /* should add <NIL> elements? Or maybe random bits to facilitate power estimation */
    bzero(q->pdcch_e, q->nof_bits);

    /* Encode DCIs */
    for (i = 0; i < dci->nof_dcis; i++) {
      /* do some checks */
      if (dci->msg[i].location.ncce + PDCCH_FORMAT_NOF_CCE(dci->msg[i].location.L)
          > q->nof_cce || dci->msg[i].location.L > 3
          || dci->msg[i].location.nof_bits > DCI_MAX_BITS) {
        fprintf(stderr, "Illegal DCI message nCCE: %d, L: %d, nof_cce: %d\n", 
                dci->msg[i].location.ncce, dci->msg[i].location.L, q->nof_cce);
        return LIBLTE_ERROR;
      }
      INFO("Encoding DCI %d: Nbits: %d, E: %d, nCCE: %d, L: %d, RNTI: 0x%x\n", i,
          dci->msg[i].location.nof_bits,
          PDCCH_FORMAT_NOF_BITS(dci->msg[i].location.L),
          dci->msg[i].location.ncce, dci->msg[i].location.L,
          dci->msg[i].location.rnti);

      dci_encode(q, dci->msg[i].data, &q->pdcch_e[72 * dci->msg[i].location.ncce],
          dci->msg[i].location.nof_bits,
          PDCCH_FORMAT_NOF_BITS(dci->msg[i].location.L),
          dci->msg[i].location.rnti);
    }

    scrambling_b_offset(&q->seq_pdcch[nsubframe], q->pdcch_e, 0, q->nof_bits);

    mod_modulate(&q->mod, q->pdcch_e, q->pdcch_d, q->nof_bits);

    /* layer mapping & precoding */
    if (q->cell.nof_ports > 1) {
      layermap_diversity(q->pdcch_d, x, q->cell.nof_ports, q->nof_symbols);
      precoding_diversity(x, q->pdcch_symbols, q->cell.nof_ports,
          q->nof_symbols / q->cell.nof_ports);
    } else {
      memcpy(q->pdcch_symbols[0], q->pdcch_d, q->nof_symbols * sizeof(cf_t));
    }

    /* mapping to resource elements */
    for (i = 0; i < q->cell.nof_ports; i++) {
      regs_pdcch_put(q->regs, q->pdcch_symbols[i], slot_symbols[i]);
    }
    return LIBLTE_SUCCESS;
  } else {
    return LIBLTE_ERROR_INVALID_INPUTS;
  }
}

