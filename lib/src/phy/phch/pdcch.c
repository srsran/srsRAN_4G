/**
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include <complex.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/phch/dci.h"
#include "srsran/phy/phch/pdcch.h"
#include "srsran/phy/phch/regs.h"
#include "srsran/phy/utils/bit.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"

#define PDCCH_NOF_FORMATS 4
#define PDCCH_FORMAT_NOF_CCE(i) (1 << i)
#define PDCCH_FORMAT_NOF_REGS(i) ((1 << i) * 9)
#define PDCCH_FORMAT_NOF_BITS(i) ((1 << i) * 72)

#define NOF_CCE(cfi) ((cfi > 0 && cfi < 4) ? q->nof_cce[cfi - 1] : 0)
#define NOF_REGS(cfi) ((cfi > 0 && cfi < 4) ? q->nof_regs[cfi - 1] : 0)

float srsran_pdcch_coderate(uint32_t nof_bits, uint32_t l)
{
  static const int nof_bits_x_symbol = 2; // QPSK
  return (float)(nof_bits + 16) / (nof_bits_x_symbol * PDCCH_FORMAT_NOF_REGS(l));
}

/** Initializes the PDCCH transmitter and receiver */
static int pdcch_init(srsran_pdcch_t* q, uint32_t max_prb, uint32_t nof_rx_antennas, bool is_ue)
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;

  if (q != NULL) {
    ret = SRSRAN_ERROR;
    bzero(q, sizeof(srsran_pdcch_t));
    q->nof_rx_antennas = nof_rx_antennas;
    q->is_ue           = is_ue;
    /* Allocate memory for the maximum number of PDCCH bits (CFI=3) */
    q->max_bits = max_prb * 3 * 12 * 2;

    INFO("Init PDCCH: Max bits: %d", q->max_bits);

    if (srsran_modem_table_lte(&q->mod, SRSRAN_MOD_QPSK)) {
      goto clean;
    }
    if (srsran_crc_init(&q->crc, SRSRAN_LTE_CRC16, 16)) {
      goto clean;
    }

    int poly[3] = {0x6D, 0x4F, 0x57};
    if (srsran_viterbi_init(&q->decoder, SRSRAN_VITERBI_37, poly, SRSRAN_DCI_MAX_BITS + 16, true)) {
      goto clean;
    }

    q->e = srsran_vec_u8_malloc(q->max_bits);
    if (!q->e) {
      goto clean;
    }

    q->llr = srsran_vec_f_malloc(q->max_bits);
    if (!q->llr) {
      goto clean;
    }

    srsran_vec_f_zero(q->llr, q->max_bits);

    q->d = srsran_vec_cf_malloc(q->max_bits / 2);
    if (!q->d) {
      goto clean;
    }

    for (int i = 0; i < SRSRAN_MAX_PORTS; i++) {
      q->x[i] = srsran_vec_cf_malloc(q->max_bits / 2);
      if (!q->x[i]) {
        goto clean;
      }
      q->symbols[i] = srsran_vec_cf_malloc(q->max_bits / 2);
      if (!q->symbols[i]) {
        goto clean;
      }
      if (q->is_ue) {
        for (int j = 0; j < q->nof_rx_antennas; j++) {
          q->ce[i][j] = srsran_vec_cf_malloc(q->max_bits / 2);
          if (!q->ce[i][j]) {
            goto clean;
          }
        }
      }
    }

    ret = SRSRAN_SUCCESS;
  }
clean:
  if (ret == SRSRAN_ERROR) {
    srsran_pdcch_free(q);
  }
  return ret;
}

int srsran_pdcch_init_enb(srsran_pdcch_t* q, uint32_t max_prb)
{
  return pdcch_init(q, max_prb, 0, false);
}

int srsran_pdcch_init_ue(srsran_pdcch_t* q, uint32_t max_prb, uint32_t nof_rx_antennas)
{
  return pdcch_init(q, max_prb, nof_rx_antennas, true);
}

void srsran_pdcch_free(srsran_pdcch_t* q)
{
  if (q->e) {
    free(q->e);
  }
  if (q->llr) {
    free(q->llr);
  }
  if (q->d) {
    free(q->d);
  }
  for (int i = 0; i < SRSRAN_MAX_PORTS; i++) {
    if (q->x[i]) {
      free(q->x[i]);
    }
    if (q->symbols[i]) {
      free(q->symbols[i]);
    }
    if (q->is_ue) {
      for (int j = 0; j < q->nof_rx_antennas; j++) {
        if (q->ce[i][j]) {
          free(q->ce[i][j]);
        }
      }
    }
  }
  for (int i = 0; i < SRSRAN_NOF_SF_X_FRAME; i++) {
    srsran_sequence_free(&q->seq[i]);
  }

  srsran_modem_table_free(&q->mod);
  srsran_viterbi_free(&q->decoder);

  bzero(q, sizeof(srsran_pdcch_t));
}

void srsran_pdcch_set_regs(srsran_pdcch_t* q, srsran_regs_t* regs)
{
  q->regs = regs;

  for (int cfi = 0; cfi < 3; cfi++) {
    q->nof_regs[cfi] = (srsran_regs_pdcch_nregs(q->regs, cfi + 1) / 9) * 9;
    q->nof_cce[cfi]  = q->nof_regs[cfi] / 9;
  }

  /* Allocate memory for the maximum number of PDCCH bits (CFI=3) */
  q->max_bits = (NOF_REGS(3) / 9) * 72;
}

int srsran_pdcch_set_cell(srsran_pdcch_t* q, srsran_regs_t* regs, srsran_cell_t cell)
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;

  if (q != NULL && regs != NULL && srsran_cell_isvalid(&cell)) {
    srsran_pdcch_set_regs(q, regs);

    INFO("PDCCH: Cell config PCI=%d, %d ports.", q->cell.id, q->cell.nof_ports);

    if (q->cell.id != cell.id || q->cell.nof_prb == 0) {
      q->cell = cell;

      for (int i = 0; i < SRSRAN_NOF_SF_X_FRAME; i++) {
        // we need to pregenerate the sequence for the maximum number of bits, which is 8 times
        // the maximum number of REGs (for CFI=3)
        if (srsran_sequence_pdcch(&q->seq[i], 2 * i, q->cell.id, 8 * srsran_regs_pdcch_nregs(q->regs, 3))) {
          return SRSRAN_ERROR;
        }
      }
    }
    ret = SRSRAN_SUCCESS;
  }
  return ret;
}

uint32_t srsran_pdcch_ue_locations(srsran_pdcch_t*        q,
                                   srsran_dl_sf_cfg_t*    sf,
                                   srsran_dci_location_t* c,
                                   uint32_t               max_candidates,
                                   uint16_t               rnti)
{
  return srsran_pdcch_ue_locations_ncce(NOF_CCE(sf->cfi), c, max_candidates, sf->tti % 10, rnti);
}

uint32_t srsran_pdcch_ue_locations_ncce(uint32_t               nof_cce,
                                        srsran_dci_location_t* c,
                                        uint32_t               max_candidates,
                                        uint32_t               sf_idx,
                                        uint16_t               rnti)
{
  return srsran_pdcch_ue_locations_ncce_L(nof_cce, c, max_candidates, sf_idx, rnti, -1);
}

/** 36.213 v9.1.1
 * Computes up to max_candidates UE-specific candidates for DCI messages and saves them
 * in the structure pointed by c.
 * Returns the number of candidates saved in the array c.
 */
uint32_t srsran_pdcch_ue_locations_ncce_L(uint32_t               nof_cce,
                                          srsran_dci_location_t* c,
                                          uint32_t               max_candidates,
                                          uint32_t               sf_idx,
                                          uint16_t               rnti,
                                          int                    Ls)
{
  int       l; // this must be int because of the for(;;--) loop
  uint32_t  i, k, L, m;
  uint32_t  Yk, ncce;
  const int nof_candidates[4] = {6, 6, 2, 2};

  // Compute Yk for this subframe
  Yk = rnti;
  for (m = 0; m < sf_idx + 1; m++) {
    Yk = (39827 * Yk) % 65537;
  }

  k = 0;
  // All aggregation levels from 1 to 8
  for (l = 0; l <= 3; l++) {
    L = (1 << l);
    if (Ls < 0 || Ls == L) {
      // For all candidates as given in table 9.1.1-1
      for (i = 0; i < nof_candidates[l]; i++) {
        if (nof_cce >= L) {
          ncce = L * ((Yk + i) % (nof_cce / L));

          // Check candidate fits in array
          bool valid = (k < max_candidates);

          // Check candidate fits in CCE region
          valid &= (ncce + L <= nof_cce);

          // Check candidate is not repeated in the list
          for (uint32_t j = 0; j < k && valid; j++) {
            valid &= (c[j].L != l || c[j].ncce != ncce);
          }

          // Append candidate
          if (valid) {
            c[k].L    = l;
            c[k].ncce = ncce;

            DEBUG("UE-specific SS Candidate %d: nCCE: %d, L: %d", k, c[k].ncce, c[k].L);

            k++;
          }
        }
      }
    }
  }

  DEBUG("Initiated %d candidate(s) in the UE-specific search space for C-RNTI: 0x%x, sf_idx=%d, nof_cce=%d",
        k,
        rnti,
        sf_idx,
        nof_cce);

  return k;
}

/**
 * 36.213 9.1.1
 * Computes up to max_candidates candidates in the common search space
 * for DCI messages and saves them in the structure pointed by c.
 * Returns the number of candidates saved in the array c.
 */
uint32_t
srsran_pdcch_common_locations(srsran_pdcch_t* q, srsran_dci_location_t* c, uint32_t max_candidates, uint32_t cfi)
{
  return srsran_pdcch_common_locations_ncce(NOF_CCE(cfi), c, max_candidates);
}

uint32_t srsran_pdcch_common_locations_ncce(uint32_t nof_cce, srsran_dci_location_t* c, uint32_t max_candidates)
{
  uint32_t i, l, L, k;

  k = 0;
  for (l = 2; l <= 3; l++) {
    L = (1 << l);
    for (i = 0; i < SRSRAN_MIN(nof_cce, 16) / (L); i++) {
      // Simplified expression, derived from:
      // L * ((Yk + m) % (N_cce / L)) + i
      uint32_t ncce = L * i;
      if (k < max_candidates && ncce + L <= nof_cce) {
        c[k].L    = l;
        c[k].ncce = ncce;
        DEBUG("Common SS Candidate %d: nCCE: %d/%d, L: %d", k, c[k].ncce, nof_cce, c[k].L);
        k++;
      }
    }
  }

  INFO("Initiated %d candidate(s) in the Common search space", k);

  return k;
}

/** 36.212 5.3.3.2 to 5.3.3.4
 *
 * Returns XOR between parity and remainder bits
 *
 * TODO: UE transmit antenna selection CRC mask
 */
int srsran_pdcch_dci_decode(srsran_pdcch_t* q, float* e, uint8_t* data, uint32_t E, uint32_t nof_bits, uint16_t* crc)
{
  uint16_t p_bits, crc_res;
  uint8_t* x;

  if (q != NULL) {
    if (data != NULL && E <= q->max_bits && nof_bits <= SRSRAN_DCI_MAX_BITS) {
      srsran_vec_f_zero(q->rm_f, 3 * (SRSRAN_DCI_MAX_BITS + 16));

      uint32_t coded_len = 3 * (nof_bits + 16);

      /* unrate matching */
      srsran_rm_conv_rx(e, E, q->rm_f, coded_len);

      /* viterbi decoder */
      srsran_viterbi_decode_f(&q->decoder, q->rm_f, data, nof_bits + 16);

      x       = &data[nof_bits];
      p_bits  = (uint16_t)srsran_bit_pack(&x, 16);
      crc_res = ((uint16_t)srsran_crc_checksum(&q->crc, data, nof_bits) & 0xffff);

      if (crc) {
        *crc = p_bits ^ crc_res;
      }

      return SRSRAN_SUCCESS;
    } else {
      ERROR("Invalid parameters: E: %d, max_bits: %d, nof_bits: %d", E, q->max_bits, nof_bits);
      return SRSRAN_ERROR_INVALID_INPUTS;
    }
  } else {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }
}

/** Tries to decode a DCI message from the LLRs stored in the srsran_pdcch_t structure by the function
 * srsran_pdcch_extract_llr(). This function can be called multiple times.
 * The location to search for is obtained from msg.
 * The decoded message is stored in msg and the CRC remainder in msg->rnti
 *
 */
int srsran_pdcch_decode_msg(srsran_pdcch_t* q, srsran_dl_sf_cfg_t* sf, srsran_dci_cfg_t* dci_cfg, srsran_dci_msg_t* msg)
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;
  if (q != NULL && msg != NULL && srsran_dci_location_isvalid(&msg->location)) {
    if (msg->location.ncce * 72 + PDCCH_FORMAT_NOF_BITS(msg->location.L) > NOF_CCE(sf->cfi) * 72) {
      ERROR("Invalid location: nCCE: %d, L: %d, NofCCE: %d", msg->location.ncce, msg->location.L, NOF_CCE(sf->cfi));
    } else {
      ret = SRSRAN_SUCCESS;

      uint32_t nof_bits = srsran_dci_format_sizeof(&q->cell, sf, dci_cfg, msg->format);
      uint32_t e_bits   = PDCCH_FORMAT_NOF_BITS(msg->location.L);

      // Compute absolute mean of the LLRs
      double mean = 0;
      for (int i = 0; i < e_bits; i++) {
        mean += fabsf(q->llr[msg->location.ncce * 72 + i]);
      }
      mean /= e_bits;

      if (mean > 0.3f) {
        ret = srsran_pdcch_dci_decode(q, &q->llr[msg->location.ncce * 72], msg->payload, e_bits, nof_bits, &msg->rnti);
        if (ret == SRSRAN_SUCCESS) {
          msg->nof_bits = nof_bits;
          // Check format differentiation
          if (msg->format == SRSRAN_DCI_FORMAT0 || msg->format == SRSRAN_DCI_FORMAT1A) {
            msg->format = (msg->payload[dci_cfg->cif_enabled ? 3 : 0] == 0) ? SRSRAN_DCI_FORMAT0 : SRSRAN_DCI_FORMAT1A;
          }
        } else {
          ERROR("Error calling pdcch_dci_decode");
        }
        INFO("Decoded DCI: nCCE=%d, L=%d, format=%s, msg_len=%d, mean=%f, crc_rem=0x%x",
             msg->location.ncce,
             msg->location.L,
             srsran_dci_format_string(msg->format),
             nof_bits,
             mean,
             msg->rnti);
      } else {
        INFO("Skipping DCI:  nCCE=%d, L=%d, msg_len=%d, mean=%f", msg->location.ncce, msg->location.L, nof_bits, mean);
      }
    }
  } else if (msg != NULL) {
    ERROR("Invalid parameters, location=%d,%d", msg->location.ncce, msg->location.L);
  }
  return ret;
}

float srsran_pdcch_msg_corr(srsran_pdcch_t* q, srsran_dci_msg_t* msg)
{
  if (q == NULL || msg == NULL) {
    return 0.0f;
  }

  uint32_t E       = PDCCH_FORMAT_NOF_BITS(msg->location.L);
  uint32_t nof_llr = E / 2;

  // Encode same decoded message and compute correlation
  srsran_pdcch_dci_encode(q, msg->payload, q->e, msg->nof_bits, E, msg->rnti);

  // Modulate
  srsran_mod_modulate(&q->mod, q->e, q->d, E);

  // Correlate
  cf_t corr = srsran_vec_dot_prod_conj_ccc((cf_t*)&q->llr[msg->location.ncce * 72], q->d, nof_llr);

  return cabsf(corr / nof_llr) * (float)M_SQRT1_2;
}

/** Performs PDCCH receiver processing to extract LLR for all control region. LLR bits are stored in srsran_pdcch_t
 * object. DCI can be decoded from given locations in successive calls to srsran_pdcch_decode_msg()
 */
int srsran_pdcch_extract_llr(srsran_pdcch_t*        q,
                             srsran_dl_sf_cfg_t*    sf,
                             srsran_chest_dl_res_t* channel,
                             cf_t*                  sf_symbols[SRSRAN_MAX_PORTS])
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;

  /* Set pointers for layermapping & precoding */
  uint32_t i, nof_symbols;
  cf_t*    x[SRSRAN_MAX_LAYERS];

  if (q != NULL && sf->cfi > 0 && sf->cfi < 4) {
    uint32_t e_bits = 72 * NOF_CCE(sf->cfi);
    nof_symbols     = e_bits / 2;
    ret             = SRSRAN_ERROR;
    srsran_vec_f_zero(q->llr, q->max_bits);

    DEBUG("Extracting LLRs: E: %d, SF: %d, CFI: %d", e_bits, sf->tti % 10, sf->cfi);

    /* number of layers equals number of ports */
    for (i = 0; i < q->cell.nof_ports; i++) {
      x[i] = q->x[i];
    }
    memset(&x[q->cell.nof_ports], 0, sizeof(cf_t*) * (SRSRAN_MAX_LAYERS - q->cell.nof_ports));

    /* extract symbols */
    for (int j = 0; j < q->nof_rx_antennas; j++) {
      int n = srsran_regs_pdcch_get(q->regs, sf->cfi, sf_symbols[j], q->symbols[j]);
      if (nof_symbols != n) {
        ERROR("Expected %d PDCCH symbols but got %d symbols", nof_symbols, n);
        return ret;
      }

      /* extract channel estimates */
      for (i = 0; i < q->cell.nof_ports; i++) {
        n = srsran_regs_pdcch_get(q->regs, sf->cfi, channel->ce[i][j], q->ce[i][j]);
        if (nof_symbols != n) {
          ERROR("Expected %d PDCCH symbols but got %d symbols", nof_symbols, n);
          return ret;
        }
      }
    }

    /* in control channels, only diversity is supported */
    if (q->cell.nof_ports == 1) {
      /* no need for layer demapping */
      srsran_predecoding_single_multi(
          q->symbols, q->ce[0], q->d, NULL, q->nof_rx_antennas, nof_symbols, 1.0f, channel->noise_estimate / 2);
    } else {
      srsran_predecoding_diversity_multi(
          q->symbols, q->ce, x, NULL, q->nof_rx_antennas, q->cell.nof_ports, nof_symbols, 1.0f);
      srsran_layerdemap_diversity(x, q->d, q->cell.nof_ports, nof_symbols / q->cell.nof_ports);
    }

    /* demodulate symbols */
    srsran_demod_soft_demodulate(SRSRAN_MOD_QPSK, q->d, q->llr, nof_symbols);

    /* descramble */
    srsran_scrambling_f_offset(&q->seq[sf->tti % 10], q->llr, 0, e_bits);

    ret = SRSRAN_SUCCESS;
  }
  return ret;
}

static void crc_set_mask_rnti(uint8_t* crc, uint16_t rnti)
{
  uint32_t i;
  uint8_t  mask[16];
  uint8_t* r = mask;

  DEBUG("Mask CRC with RNTI 0x%x", rnti);

  srsran_bit_unpack(rnti, &r, 16);
  for (i = 0; i < 16; i++) {
    crc[i] = (crc[i] + mask[i]) % 2;
  }
}

void srsran_pdcch_dci_encode_conv(srsran_pdcch_t* q,
                                  uint8_t*        data,
                                  uint32_t        nof_bits,
                                  uint8_t*        coded_data,
                                  uint16_t        rnti)
{
  srsran_convcoder_t encoder;
  int                poly[3] = {0x6D, 0x4F, 0x57};
  encoder.K                  = 7;
  encoder.R                  = 3;
  encoder.tail_biting        = true;
  memcpy(encoder.poly, poly, 3 * sizeof(int));

  srsran_crc_attach(&q->crc, data, nof_bits);
  crc_set_mask_rnti(&data[nof_bits], rnti);

  srsran_convcoder_encode(&encoder, data, coded_data, nof_bits + 16);
}

/** 36.212 5.3.3.2 to 5.3.3.4
 * TODO: UE transmit antenna selection CRC mask
 */
int srsran_pdcch_dci_encode(srsran_pdcch_t* q, uint8_t* data, uint8_t* e, uint32_t nof_bits, uint32_t E, uint16_t rnti)
{
  uint8_t tmp[3 * (SRSRAN_DCI_MAX_BITS + 16)];

  if (q != NULL && data != NULL && e != NULL && nof_bits < SRSRAN_DCI_MAX_BITS && E < q->max_bits) {
    srsran_pdcch_dci_encode_conv(q, data, nof_bits, tmp, rnti);

    DEBUG("CConv output: ");
    if (SRSRAN_VERBOSE_ISDEBUG()) {
      srsran_vec_fprint_b(stdout, tmp, 3 * (nof_bits + 16));
    }

    srsran_rm_conv_tx(tmp, 3 * (nof_bits + 16), e, E);

    return SRSRAN_SUCCESS;
  } else {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }
}

/** Encodes ONE DCI message and allocates the encoded bits to the srsran_dci_location_t indicated by
 * the parameter location. The CRC is scrambled with the RNTI parameter.
 * This function can be called multiple times and encoded DCI messages will be allocated to the
 * sf_symbols buffer ready for transmission.
 * If the same location is provided in multiple messages, the encoded bits will be overwritten.
 *
 * @TODO: Use a bitmask and CFI to ensure message locations are valid and old messages are not overwritten.
 */
int srsran_pdcch_encode(srsran_pdcch_t*     q,
                        srsran_dl_sf_cfg_t* sf,
                        srsran_dci_msg_t*   msg,
                        cf_t*               sf_symbols[SRSRAN_MAX_PORTS])
{
  int      ret = SRSRAN_ERROR_INVALID_INPUTS;
  uint32_t i;
  cf_t*    x[SRSRAN_MAX_LAYERS];
  uint32_t nof_symbols;

  if (q != NULL && sf_symbols != NULL && sf->cfi > 0 && sf->cfi < 4 && srsran_dci_location_isvalid(&msg->location)) {
    uint32_t e_bits = PDCCH_FORMAT_NOF_BITS(msg->location.L);
    nof_symbols     = e_bits / 2;
    ret             = SRSRAN_ERROR;

    if (msg->location.ncce + PDCCH_FORMAT_NOF_CCE(msg->location.L) <= NOF_CCE(sf->cfi) &&
        msg->nof_bits < SRSRAN_DCI_MAX_BITS - 16) {
      DEBUG("Encoding DCI: Nbits: %d, E: %d, nCCE: %d, L: %d, RNTI: 0x%x",
            msg->nof_bits,
            e_bits,
            msg->location.ncce,
            msg->location.L,
            msg->rnti);

      srsran_pdcch_dci_encode(q, msg->payload, q->e, msg->nof_bits, e_bits, msg->rnti);

      /* number of layers equals number of ports */
      for (i = 0; i < q->cell.nof_ports; i++) {
        x[i] = q->x[i];
      }
      memset(&x[q->cell.nof_ports], 0, sizeof(cf_t*) * (SRSRAN_MAX_LAYERS - q->cell.nof_ports));

      srsran_scrambling_b_offset(&q->seq[sf->tti % 10], q->e, 72 * msg->location.ncce, e_bits);

      DEBUG("Scrambling output: ");
      if (SRSRAN_VERBOSE_ISDEBUG()) {
        srsran_vec_fprint_b(stdout, q->e, e_bits);
      }

      srsran_mod_modulate(&q->mod, q->e, q->d, e_bits);

      /* layer mapping & precoding */
      if (q->cell.nof_ports > 1) {
        srsran_layermap_diversity(q->d, x, q->cell.nof_ports, nof_symbols);
        srsran_precoding_diversity(x, q->symbols, q->cell.nof_ports, nof_symbols / q->cell.nof_ports, 1.0f);
      } else {
        memcpy(q->symbols[0], q->d, nof_symbols * sizeof(cf_t));
      }

      /* mapping to resource elements */
      for (i = 0; i < q->cell.nof_ports; i++) {
        srsran_regs_pdcch_put_offset(q->regs,
                                     sf->cfi,
                                     q->symbols[i],
                                     sf_symbols[i],
                                     msg->location.ncce * 9,
                                     PDCCH_FORMAT_NOF_REGS(msg->location.L));
      }

      ret = SRSRAN_SUCCESS;

    } else {
      ERROR("Illegal DCI message nCCE: %d, L: %d, nof_cce: %d, nof_bits=%d",
            msg->location.ncce,
            msg->location.L,
            NOF_CCE(sf->cfi),
            msg->nof_bits);
    }
  } else {
    ERROR("Invalid parameters: cfi=%d, L=%d, nCCE=%d", sf->cfi, msg->location.L, msg->location.ncce);
  }
  return ret;
}
