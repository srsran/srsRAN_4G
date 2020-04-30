/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
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

#include <assert.h>
#include <complex.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "prb_dl.h"
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/phch/npbch.h"
#include "srslte/phy/phch/ra_nbiot.h"
#include "srslte/phy/utils/bit.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"

#define SRSLTE_NBIOT_NPBCH_NUM_REFS_PER_SYMB 4

// Table 5.3.1.1-1 in 36.212 v13.3.0
const uint8_t srslte_npbch_crc_mask[4][16] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                              {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
                                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                              {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1}};

/** Initializes the NPBCH transmitter and receiver.
 * At the receiver, the field nof_ports in the cell structure indicates the
 * maximum number of BS transmitter ports to look for.
 */
int srslte_npbch_init(srslte_npbch_t* q)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q != NULL) {
    bzero(q, sizeof(srslte_npbch_t));

    ret = SRSLTE_ERROR;

    q->nof_symbols = SRSLTE_NPBCH_NUM_RE;

    if (srslte_modem_table_lte(&q->mod, SRSLTE_MOD_QPSK)) {
      fprintf(stderr, "Error initiating modem table.\n");
      goto clean;
    }

    int poly[3] = {0x6D, 0x4F, 0x57};
    if (srslte_viterbi_init(&q->decoder, SRSLTE_VITERBI_37, poly, SRSLTE_MIB_NB_CRC_LEN, true)) {
      fprintf(stderr, "Error initiating Viterbi.\n");
      goto clean;
    }
    if (srslte_crc_init(&q->crc, SRSLTE_LTE_CRC16, 16)) {
      fprintf(stderr, "Error initiating CRC object.\n");
      goto clean;
    }
    q->encoder.K           = 7;
    q->encoder.R           = 3;
    q->encoder.tail_biting = true;
    memcpy(q->encoder.poly, poly, 3 * sizeof(int));

    q->d = srslte_vec_cf_malloc(q->nof_symbols);
    if (!q->d) {
      fprintf(stderr, "Error allocating memory.\n");
      goto clean;
    }
    for (uint32_t i = 0; i < SRSLTE_MAX_PORTS; i++) {
      q->ce[i] = srslte_vec_cf_malloc(q->nof_symbols);
      if (!q->ce[i]) {
        fprintf(stderr, "Error allocating memory.\n");
        goto clean;
      }
      q->x[i] = srslte_vec_cf_malloc(q->nof_symbols);
      if (!q->x[i]) {
        fprintf(stderr, "Error allocating memory.\n");
        goto clean;
      }
      q->symbols[i] = srslte_vec_cf_malloc(q->nof_symbols * SRSLTE_NPBCH_NUM_FRAMES);
      if (!q->symbols[i]) {
        fprintf(stderr, "Error allocating memory.\n");
        goto clean;
      }
    }
    q->llr = srslte_vec_f_malloc(q->nof_symbols * SRSLTE_NPBCH_NUM_FRAMES * 2);
    if (!q->llr) {
      fprintf(stderr, "Error allocating memory.\n");
      goto clean;
    }

    q->temp = srslte_vec_f_malloc(q->nof_symbols * SRSLTE_NPBCH_NUM_FRAMES * 2);
    if (!q->temp) {
      fprintf(stderr, "Error allocating memory.\n");
      goto clean;
    }
    q->rm_b = srslte_vec_u8_malloc(q->nof_symbols * SRSLTE_NPBCH_NUM_FRAMES * 2);
    if (!q->rm_b) {
      fprintf(stderr, "Error allocating memory.\n");
      goto clean;
    }
    ret = SRSLTE_SUCCESS;
  }
clean:
  if (ret == SRSLTE_ERROR) {
    srslte_npbch_free(q);
  }
  return ret;
}

void srslte_npbch_free(srslte_npbch_t* q)
{
  if (q->d) {
    free(q->d);
  }
  for (uint32_t i = 0; i < SRSLTE_MAX_PORTS; i++) {
    if (q->ce[i]) {
      free(q->ce[i]);
    }
    if (q->x[i]) {
      free(q->x[i]);
    }
    if (q->symbols[i]) {
      free(q->symbols[i]);
    }
  }
  if (q->llr) {
    free(q->llr);
  }
  if (q->temp) {
    free(q->temp);
  }
  if (q->rm_b) {
    free(q->rm_b);
  }
  for (uint32_t i = 0; i < SRSLTE_NPBCH_NUM_BLOCKS; i++) {
    srslte_sequence_free(&q->seq_r14[i]);
  }

  srslte_sequence_free(&q->seq);
  srslte_modem_table_free(&q->mod);
  srslte_viterbi_free(&q->decoder);
}

int srslte_npbch_set_cell(srslte_npbch_t* q, srslte_nbiot_cell_t cell)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q != NULL && srslte_nbiot_cell_isvalid(&cell)) {
    // set ports configuration
    if (cell.nof_ports == 0) {
      q->search_all_ports = true;
      cell.nof_ports      = SRSLTE_NBIOT_MAX_PORTS;
    } else {
      q->search_all_ports = false;
    }

    if (q->cell.n_id_ncell != cell.n_id_ncell || q->cell.base.nof_prb == 0) {
      q->cell = cell;
      if (srslte_sequence_npbch(&q->seq, q->cell.base.cp, q->cell.n_id_ncell)) {
        fprintf(stderr, "Error initiating NPBCH sequence.\n");
        return SRSLTE_ERROR;
      }

      // pre-compute the 8 rotation sequences for R14
      if (q->cell.is_r14) {
        for (uint32_t i = 0; i < SRSLTE_NPBCH_NUM_BLOCKS; i++) {
          if (srslte_sequence_npbch_r14(&q->seq_r14[i], q->cell.n_id_ncell, i)) {
            return SRSLTE_ERROR;
          }
        }
      }
    }

    ret = SRSLTE_SUCCESS;
  }
  return ret;
}

/** Packs the MIB-NB, according to TS 36.331 v13.2.0 Section 6.7.2
 */
void srslte_npbch_mib_pack(uint32_t hfn, uint32_t sfn, srslte_mib_nb_t mib, uint8_t* payload)
{
  uint8_t* msg = payload;
  bzero(msg, SRSLTE_MIB_NB_LEN);

  // System Frame Number (SFN), 4 MSB
  srslte_bit_unpack(sfn >> 6, &msg, 4);

  // Hyper Frame Number (HFB), 2 LSB
  srslte_bit_unpack(hfn, &msg, 2);

  // schedulingInfoSIB1, integer 0-15, 4 bits
  srslte_bit_unpack(mib.sched_info_sib1, &msg, 4);

  // system info value tag, integer 0-31, 5 bits
  srslte_bit_unpack(mib.sys_info_tag, &msg, 5);

  // access barring enabled, 1 bit
  srslte_bit_unpack(mib.ac_barring, &msg, 1);

  // operation mode info, 2 for mode + 5 for config, 7 bits in total
  srslte_bit_unpack(mib.mode, &msg, 2);

  // 11 spare bits
}

/** Unpacks MIB-NB from NPBCH message.
 * msg buffer must be 34 byte length at least
 */
void srslte_npbch_mib_unpack(uint8_t* msg, srslte_mib_nb_t* mib)
{
  if (mib) {
    mib->sfn             = (srslte_bit_pack(&msg, 4) << 6) & 0x3C0;
    mib->hfn             = srslte_bit_pack(&msg, 2) & 0x3;
    mib->sched_info_sib1 = srslte_bit_pack(&msg, 4) & 0x0000ffff;
    mib->sys_info_tag    = srslte_bit_pack(&msg, 5) & 0x0001ffff;
    mib->ac_barring      = srslte_bit_pack(&msg, 1) & 0x1;
    mib->mode            = srslte_bit_pack(&msg, 2) & 0x0000000B;
  }
}

void srslte_mib_nb_printf(FILE* stream, srslte_nbiot_cell_t cell, srslte_mib_nb_t* mib)
{
  fprintf(stream, " - N_id_ncell:           %d\n", cell.n_id_ncell);
  fprintf(stream, " - Release:              %s\n", cell.is_r14 ? "r14" : "r13");
  fprintf(stream, " - Nof ports:            %d\n", cell.nof_ports);
  fprintf(stream, " - SFN:                  %d\n", mib->sfn);
  fprintf(stream, " - HFN (2 LSB):          %d\n", mib->hfn);
  fprintf(stream, " - Sched. Info SIB1      %d\n", mib->sched_info_sib1);
  fprintf(stream, "   - First frame         %d\n", srslte_ra_nbiot_get_starting_sib1_frame(cell.n_id_ncell, mib));
  fprintf(stream, "   - #repetitions        %d\n", srslte_ra_n_rep_sib1_nb(mib));
  fprintf(stream, "   - TBS                 %d\n", srslte_ra_nbiot_get_sib1_tbs(mib));
  fprintf(stream, " - System Info Val       %d\n", mib->sys_info_tag);
  fprintf(stream, " - AC barring            %s\n", mib->ac_barring ? "Yes" : "No");
  fprintf(stream, " - Operating mode        %s\n", srslte_nbiot_mode_string(mib->mode));
}

int srslte_npbch_put_subframe(srslte_npbch_t* q,
                              uint8_t         bch_payload[SRSLTE_MIB_NB_LEN],
                              cf_t*           sf[SRSLTE_MAX_PORTS],
                              uint32_t        frame_idx)
{
  return srslte_npbch_encode(q, bch_payload, sf, frame_idx);
}

/** Converts the MIB-NB message to symbols mapped to the first subframe,
 *  The MIB-NB is split over 8 blocks, each of which is repeated 8 times, always in SF0,
 *  it therefore lasts for 640ms.
 */
int srslte_npbch_encode(srslte_npbch_t* q,
                        uint8_t         bch_payload[SRSLTE_MIB_NB_LEN],
                        cf_t*           sf[SRSLTE_MAX_PORTS],
                        uint32_t        frame_idx)
{
  int   block_idx = (frame_idx / SRSLTE_NPBCH_NUM_REP) % SRSLTE_NPBCH_NUM_BLOCKS;
  cf_t* x[SRSLTE_MAX_LAYERS];

  if (q != NULL && bch_payload != NULL && q->cell.nof_ports != 0) {
    for (int i = 0; i < q->cell.nof_ports; i++) {
      if (sf[i] == NULL) {
        return SRSLTE_ERROR_INVALID_INPUTS;
      }
    }
    // Set pointers for layermapping & precoding
    int nof_bits = 2 * q->nof_symbols;

    // number of layers equals number of ports
    for (int i = 0; i < q->cell.nof_ports; i++) {
      x[i] = q->x[i];
    }
    memset(&x[q->cell.nof_ports], 0, sizeof(cf_t*) * (SRSLTE_MAX_LAYERS - q->cell.nof_ports));

    // generate new BCH message every 64 frames
    if ((frame_idx % SRSLTE_NPBCH_NUM_FRAMES) == 0) {
      INFO("Encoding new NPBCH signal in frame %d.\n", frame_idx);

      memcpy(q->data, bch_payload, sizeof(uint8_t) * SRSLTE_MIB_NB_LEN);

      // encode and rate-match
      srslte_crc_attach(&q->crc, q->data, SRSLTE_MIB_NB_LEN);
      srslte_npbch_crc_set_mask(q->data, q->cell.nof_ports);

      srslte_convcoder_encode(&q->encoder, q->data, q->data_enc, SRSLTE_MIB_NB_CRC_LEN);

      srslte_rm_conv_tx(q->data_enc, SRSLTE_MIB_NB_ENC_LEN, q->rm_b, SRSLTE_NPBCH_NUM_BLOCKS * nof_bits);
    }

    // Scramble and modulate a new block every 8 frames
    if (frame_idx % SRSLTE_NPBCH_NUM_REP == 0) {
      INFO("Modulating MIB-NB block %d in frame %d.\n", block_idx, frame_idx);
      srslte_scrambling_b_offset(&q->seq, &q->rm_b[block_idx * nof_bits], block_idx * nof_bits, nof_bits);
      srslte_mod_modulate(&q->mod, &q->rm_b[block_idx * nof_bits], q->d, nof_bits);

      // layer mapping & precoding
      if (q->cell.nof_ports > 1) {
        srslte_layermap_diversity(q->d, x, q->cell.nof_ports, q->nof_symbols);
        srslte_precoding_diversity(x, q->symbols, q->cell.nof_ports, q->nof_symbols / q->cell.nof_ports, 1.0);
      } else {
        memcpy(q->symbols[0], q->d, q->nof_symbols * sizeof(cf_t));
      }
    }

    // Write exactly SRSLTE_NPBCH_NUM_RE (assumes symbols have been modulated before)
    for (int i = 0; i < q->cell.nof_ports; i++) {
      if (q->cell.is_r14) {
        DEBUG("Applying phase rotation on port %d in frame %d.\n", i, frame_idx);
        srslte_npbch_rotate(q, frame_idx, q->symbols[i], q->symbols[i], q->nof_symbols, false);
      }
      DEBUG("Putting MIB-NB block %d on port %d in frame %d.\n", block_idx, i, frame_idx);
      if (srslte_npbch_cp(q->symbols[i], sf[i], q->cell, true) != SRSLTE_NPBCH_NUM_RE) {
        INFO("Error while mapping NPBCH symbols.\n");
        return SRSLTE_ERROR;
      }
    }

    return SRSLTE_SUCCESS;
  } else {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }
}

int srslte_npbch_rotate(srslte_npbch_t* q,
                        uint32_t        nf,
                        cf_t*           input_signal,
                        cf_t*           output_signal,
                        int             num_samples,
                        bool            back)
{
  // Generate frame specific scrambling sequence for symbol rotation
  DEBUG("%sotating NPBCH in SFN=%d\n", back ? "De-R" : "R", nf);

  for (int i = 0; i < num_samples; i++) {
    int c_2i   = q->seq_r14[nf % 8].c[2 * i];
    int c_2ip1 = q->seq_r14[nf % 8].c[2 * i + 1];

#if 1
    cf_t phi_f = 0;
    if (c_2i == 0 && c_2ip1 == 0)
      phi_f = 1;
    else if (c_2i == 0 && c_2ip1 == 1)
      phi_f = -1;
    else if (c_2i == 1 && c_2ip1 == 0)
      phi_f = _Complex_I;
    else if (c_2i == 1 && c_2ip1 == 1)
      phi_f = -_Complex_I;
#else
    cf_t phi_f = (c_2i == 0) ? 1 : _Complex_I;
    if (c_2ip1 == 0)
      phi_f *= -1;
#endif

    output_signal[i] = back ? input_signal[i] / phi_f : input_signal[i] * phi_f;

    if (SRSLTE_VERBOSE_ISDEBUG()) {
      printf("i=%d c_2i=%d, c_2i+1=%d -> phi_f=", i, c_2i, c_2ip1);
      srslte_vec_fprint_c(stdout, &phi_f, 1);
      printf("input:\n");
      srslte_vec_fprint_c(stdout, &input_signal[i], 1);
      printf("output:\n");
      srslte_vec_fprint_c(stdout, &output_signal[i], 1);
      printf("\n\n");
    }
  }

  return SRSLTE_SUCCESS;
}

/* Decodes the NPBCH channel
 *
 * The NPBCH spans over 640 ms. This function is called every 10 ms. It tries to decode the MIB
 * given the symbols of a subframe (1 ms). Successive calls will use more subframes
 * to help the decoding process.
 *
 * Returns 1 if successfully decoded MIB-NB, 0 if not and -1 on error
 */
int srslte_npbch_decode_nf(srslte_npbch_t* q,
                           cf_t*           sf_symbols,
                           cf_t*           ce[SRSLTE_MAX_PORTS],
                           float           noise_estimate,
                           uint8_t         bch_payload[SRSLTE_MIB_NB_LEN],
                           uint32_t*       nof_tx_ports,
                           int*            sfn_offset,
                           int             nf)
{
  cf_t* x[SRSLTE_MAX_LAYERS] = {NULL};

  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q != NULL && sf_symbols != NULL && q->cell.nof_ports != 0) {
    for (int i = 0; i < q->cell.nof_ports; i++) {
      if (ce[i] == NULL) {
        return ret;
      }
    }

    ret = SRSLTE_ERROR;

    // Set pointers for layermapping & precoding
    int nof_bits = 2 * q->nof_symbols;

    // number of layers equals number of ports
    for (int i = 0; i < SRSLTE_MAX_PORTS; i++) {
      x[i] = q->x[i];
    }

    // extract symbols
    int nof_ext_syms = srslte_npbch_cp(sf_symbols, q->symbols[0], q->cell, false);
    if (q->nof_symbols != nof_ext_syms) {
      fprintf(stderr, "There was an error getting the PBCH symbols\n");
      return ret;
    }

    if (q->cell.is_r14) {
      // de-rotate symbols
      srslte_npbch_rotate(q, nf, q->symbols[0], q->symbols[0], q->nof_symbols, true);
    }

    // extract channel estimates
    for (int i = 0; i < q->cell.nof_ports; i++) {
      if (q->nof_symbols != srslte_npbch_cp(ce[i], q->ce[i], q->cell, false)) {
        fprintf(stderr, "There was an error getting the PBCH symbols\n");
        return ret;
      }
    }

    q->frame_idx++;
    ret = 0;

    // Try decoding for 1 to cell.nof_ports antennas
    uint32_t nant = q->cell.nof_ports;
    if (q->search_all_ports) {
      nant = 1;
    }
    do {
      if (nant != 3) {
        DEBUG("Trying %d TX antennas with %d frames\n", nant, q->frame_idx);

        // in control channels, only diversity is supported
        if (nant == 1) {
          // no need for layer demapping
          srslte_predecoding_single(q->symbols[0], q->ce[0], q->d, NULL, q->nof_symbols, 1.0, noise_estimate);
        } else {
          srslte_predecoding_diversity(q->symbols[0], q->ce, x, nant, q->nof_symbols, 1.0);
          srslte_layerdemap_diversity(x, q->d, nant, q->nof_symbols / nant);
        }

        // demodulate symbols
        srslte_demod_soft_demodulate(SRSLTE_MOD_QPSK, q->d, &q->llr[nof_bits * (q->frame_idx - 1)], q->nof_symbols);

        // only one subframe
        DEBUG("Trying to decode NPBCH ..\n");

        // TODO: simplified decoding only using first MIB block
        ret = srslte_npbch_decode_frame(q, 0, nf, 1, nof_bits, nant);
        if (ret == SRSLTE_SUCCESS) {
          if (sfn_offset) {
            *sfn_offset = (int)0;
          }
          if (nof_tx_ports) {
            *nof_tx_ports = nant;
          }
          if (bch_payload) {
            memcpy(bch_payload, q->data, sizeof(uint8_t) * SRSLTE_MIB_NB_LEN);
          }
          INFO("Successfully decoded NPBCH sfn_offset=%d\n", q->frame_idx - 1);
          q->frame_idx = 0;
          return ret;
        }
      }
      nant++;
    } while (nant <= q->cell.nof_ports);

    q->frame_idx = 0;
  }
  return ret;
}

int srslte_npbch_decode(srslte_npbch_t* q,
                        cf_t*           sf_symbols,
                        cf_t*           ce[SRSLTE_MAX_PORTS],
                        float           noise_estimate,
                        uint8_t         bch_payload[SRSLTE_MIB_NB_LEN],
                        uint32_t*       nof_tx_ports,
                        int*            sfn_offset)
{
  return srslte_npbch_decode_nf(q, sf_symbols, ce, noise_estimate, bch_payload, nof_tx_ports, sfn_offset, 0);
}

void srslte_npbch_decode_reset(srslte_npbch_t* q)
{
  q->frame_idx = 0;
}

int srslte_npbch_decode_frame(srslte_npbch_t* q,
                              uint32_t        src,
                              uint32_t        dst,
                              uint32_t        n,
                              uint32_t        nof_bits,
                              uint32_t        nof_ports)
{
  srslte_vec_f_copy(&q->temp[dst * nof_bits], &q->llr[src * nof_bits], n * nof_bits);

  // descramble
  srslte_scrambling_f_offset(&q->seq, &q->temp[dst * nof_bits], dst * nof_bits, n * nof_bits);

  for (int j = 0; j < dst * nof_bits; j++) {
    q->temp[j] = SRSLTE_RX_NULL;
  }
  for (int j = (dst + n) * nof_bits; j < SRSLTE_NPBCH_NUM_BLOCKS * nof_bits; j++) {
    q->temp[j] = SRSLTE_RX_NULL;
  }

  // unrate matching
  srslte_rm_conv_rx(q->temp, SRSLTE_NPBCH_NUM_BLOCKS * nof_bits, q->rm_f, SRSLTE_MIB_NB_ENC_LEN);

  // Normalize LLR
  srslte_vec_sc_prod_fff(q->rm_f, 1.0 / ((float)2 * n), q->rm_f, SRSLTE_MIB_NB_ENC_LEN);

  // decode
  srslte_viterbi_decode_f(&q->decoder, q->rm_f, q->data, SRSLTE_MIB_NB_CRC_LEN);

  if (srslte_npbch_crc_check(q, q->data, nof_ports) == 0) {
    return SRSLTE_SUCCESS;
  } else {
    return SRSLTE_ERROR;
  }
}

void srslte_npbch_crc_set_mask(uint8_t* data, int nof_ports)
{
  int i;
  for (i = 0; i < 16; i++) {
    data[SRSLTE_MIB_NB_LEN + i] = (data[SRSLTE_MIB_NB_LEN + i] + srslte_npbch_crc_mask[nof_ports - 1][i]) % 2;
  }
}

/* Checks CRC after applying the mask for the given number of ports.
 *
 * The bits buffer size must be at least 50 bytes.
 *
 * Returns 0 if the data is correct, -1 otherwise
 */
uint32_t srslte_npbch_crc_check(srslte_npbch_t* q, uint8_t* bits, uint32_t nof_ports)
{
  uint8_t data[SRSLTE_MIB_NB_CRC_LEN];
  memcpy(data, bits, SRSLTE_MIB_NB_CRC_LEN * sizeof(uint8_t));
  srslte_npbch_crc_set_mask(data, nof_ports);
  int ret = srslte_crc_checksum(&q->crc, data, SRSLTE_MIB_NB_CRC_LEN);
  if (ret == 0) {
    uint32_t chkzeros = 0;
    for (int i = 0; i < SRSLTE_MIB_NB_LEN; i++) {
      chkzeros += data[i];
    }
    if (chkzeros) {
      return 0;
    } else {
      return SRSLTE_ERROR;
    }
  } else {
    return ret;
  }
}

/* NPBCH starts at the fourth symbol of the sub-frame,
 * We need to assume two antenna ports for NRS and
 * four ports for CRS
 * Returns the number of symbols written/read
 */
int srslte_npbch_cp(cf_t* input, cf_t* output, srslte_nbiot_cell_t cell, bool put)
{
  cf_t *   in_ptr = input, *out_ptr = output;
  uint32_t offset = 0; // the number of REs left out before start of the REF signal RE
  uint32_t delta  = 3 * cell.base.nof_prb * SRSLTE_NRE + cell.nbiot_prb * SRSLTE_NRE;

  if (put) {
    out_ptr += delta;
  } else {
    in_ptr += delta;
  }

  for (uint32_t l = 3; l < SRSLTE_CP_NORM_SF_NSYMB; l++) {
    delta = 0;
    if (l == 3 || l == 9 || l == 10) {
      // copy entire symbol
      prb_cp(&in_ptr, &out_ptr, 1);
    } else {
      // skip LTE CRS and NRS and always assume 4 reference symbols per OFDM symbol
      offset = cell.n_id_ncell % 3;
      prb_cp_ref(
          &in_ptr, &out_ptr, offset, SRSLTE_NBIOT_NPBCH_NUM_REFS_PER_SYMB, SRSLTE_NBIOT_NPBCH_NUM_REFS_PER_SYMB, put);
      delta = (cell.n_id_ncell % 3 == 2 ? 1 : 0);
    }

    if (put) {
      out_ptr += delta;
    } else {
      in_ptr += delta;
    }
  }

  int r;
  if (put) {
    r = abs((int)(input - in_ptr));
  } else {
    r = abs((int)(output - out_ptr));
  }

  return r;
}
