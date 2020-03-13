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

#include <srslte/phy/phch/pucch_proc.h>
#include <srslte/phy/utils/debug.h>

static bool pucch_proc_tx_sr(const srslte_uci_cfg_t* uci_cfg, const srslte_uci_value_t* uci_value)
{
  // Check SR transmission
  if (uci_value) {
    // If UCI value is provided, ignore scheduling request TTI condition
    return uci_value->scheduling_request;
  }

  return uci_cfg->is_scheduling_request_tti;
}

srslte_pucch_format_t srslte_pucch_proc_select_format(const srslte_cell_t*      cell,
                                                      const srslte_pucch_cfg_t* cfg,
                                                      const srslte_uci_cfg_t*   uci_cfg,
                                                      const srslte_uci_value_t* uci_value)
{
  srslte_pucch_format_t format    = SRSLTE_PUCCH_FORMAT_ERROR;
  uint32_t              total_ack = srslte_uci_cfg_total_ack(uci_cfg);

  // No CQI data
  if (!uci_cfg->cqi.data_enable && uci_cfg->cqi.ri_len == 0) {
    if (cfg->ack_nack_feedback_mode == SRSLTE_PUCCH_ACK_NACK_FEEDBACK_MODE_PUCCH3 &&
        total_ack > uci_cfg->ack[0].nof_acks) {
      format = SRSLTE_PUCCH_FORMAT_3;
    }
    // 1-bit ACK + optional SR
    else if (total_ack == 1) {
      format = SRSLTE_PUCCH_FORMAT_1A;
    }
    // 2-bit ACK + optional SR
    else if (total_ack >= 2 && total_ack <= 4) {
      format = SRSLTE_PUCCH_FORMAT_1B; // with channel selection if > 2
    }
    // If UCI value is provided, use SR signal only, otherwise SR request opportunity
    else if (pucch_proc_tx_sr(uci_cfg, uci_value)) {
      format = SRSLTE_PUCCH_FORMAT_1;
    } else {
      ERROR("Error selecting PUCCH format: Unsupported number of ACK bits %d\n", total_ack);
    }
  }
  // CQI data
  else {
    // CQI and no ack
    if (total_ack == 0) {
      format = SRSLTE_PUCCH_FORMAT_2;
    }
    // CQI + 1-bit ACK
    else if (total_ack == 1 && SRSLTE_CP_ISNORM(cell->cp)) {
      format = SRSLTE_PUCCH_FORMAT_2A;
    }
    // CQI + 2-bit ACK
    else if (total_ack == 2) {
      format = SRSLTE_PUCCH_FORMAT_2B;
    }
    // CQI + 1-bit ACK + extended cyclic prefix
    else if (total_ack == 1 && SRSLTE_CP_ISEXT(cell->cp)) {
      format = SRSLTE_PUCCH_FORMAT_2B;
    }
  }
  if (format == SRSLTE_PUCCH_FORMAT_ERROR) {
    ERROR("Returned Error while selecting PUCCH format\n");
  }

  return format;
}

static int pucch_cs_resources(const srslte_pucch_cfg_t* cfg,
                              const srslte_uci_cfg_t*   uci_cfg,
                              uint32_t                  n_pucch_i[SRSLTE_PUCCH_MAX_ALLOC])
{
  // Check inputs
  if (!cfg || !uci_cfg || !n_pucch_i) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  // Determine up to 4 PUCCH resources n_pucch_j associated with HARQ-ACK(j)
  int k = 0;
  for (int i = 0; i < SRSLTE_PUCCH_CS_MAX_CARRIERS && k < SRSLTE_PUCCH_CS_MAX_ACK; i++) {
    if (uci_cfg->ack[i].grant_cc_idx == 0) {
      // - for a PDSCH transmission indicated by the detection of a corresponding PDCCH in subframe n − 4 on the primary
      //   cell, or for a PDCCH indicating downlink SPS release (defined in subclause 9.2) in subframe n − 4 on the
      //   primary cell, the PUCCH resource is n_pucch_i = n_cce + N_pucch_1, and for transmission mode that supports up
      //   to two transport blocks, the PUCCH resource n_pucch_i+1 = n_cce + N_pucch_1 + 1
      for (uint32_t j = 0; j < uci_cfg->ack[i].nof_acks && k < SRSLTE_PUCCH_CS_MAX_ACK; j++) {
        n_pucch_i[k++] = uci_cfg->ack[i].ncce[0] + cfg->N_pucch_1 + j;
      }
    } else if (i == 0) {
      // - for a PDSCH transmission on the primary cell where there is not a corresponding PDCCH detected in subframe
      //   n − 4 , the value of n_pucch_i is determined according to higher layer configuration and Table 9.2-2. For
      //   transmission mode that supports up to two transport blocks, the PUCCH resource n_pucch_i+1 = n_pucch_i + 1
      for (uint32_t j = 0; j < uci_cfg->ack[i].nof_acks && k < SRSLTE_PUCCH_CS_MAX_ACK; j++) {
        n_pucch_i[k++] = cfg->n1_pucch_an_cs[uci_cfg->ack[i].tpc_for_pucch % SRSLTE_PUCCH_SIZE_AN_CS][0] + j;
      }
    } else {
      // - for a PDSCH transmission indicated by the detection of a corresponding PDCCH in subframe n − 4 on the
      //   secondary cell, the value of n_pucch_i, and the value of n_pucch_i+1 for the transmission mode that supports
      //   up to two transport blocks is determined according to higher layer configuration and Table 10.1.2.2.1-2. The
      //   TPC field in the DCI format of the corresponding PDCCH shall be used to determine the PUCCH resource values
      //   from one of the four resource values configured by higher layers, with the mapping defined in Table
      //   10.1.2.2.1-2. For a UE configured for a transmission mode that supports up to two transport blocks a PUCCH
      //   resource value in Table 10.1.2.2.1-2 maps to two PUCCH resources (n_pucch_i, n_pucch_i + 1), otherwise the
      //   PUCCH resource value maps to a single PUCCH resource n_pucch_i.
      for (uint32_t j = 0; j < uci_cfg->ack[i].nof_acks && k < SRSLTE_PUCCH_CS_MAX_ACK; j++) {
        n_pucch_i[k++] =
            cfg->n1_pucch_an_cs[uci_cfg->ack[i].tpc_for_pucch % SRSLTE_PUCCH_SIZE_AN_CS][j % SRSLTE_PUCCH_NOF_AN_CS];
      }
    }
  }

  return k;
}

#define PUCCH_CS_SET_ACK(J, B0, B1, ...)                                                                               \
  do {                                                                                                                 \
    if (j == J && b[0] == B0 && b[1] == B1) {                                                                          \
      uint8_t pos[] = {__VA_ARGS__};                                                                                   \
      for (uint32_t i = 0; i < sizeof(pos) && pos[i] < SRSLTE_PUCCH_CS_MAX_ACK; i++) {                                 \
        uci_value[pos[i]] = 1;                                                                                         \
      }                                                                                                                \
      ret = SRSLTE_SUCCESS;                                                                                            \
    }                                                                                                                  \
  } while (false)

static int puccch_cs_get_ack_a2(uint32_t j, const uint8_t b[2], uint8_t uci_value[SRSLTE_UCI_MAX_ACK_BITS])
{
  int ret = SRSLTE_ERROR;

  PUCCH_CS_SET_ACK(1, 1, 1, 0, 1);
  PUCCH_CS_SET_ACK(0, 1, 1, 0);
  PUCCH_CS_SET_ACK(1, 0, 0, 1);
  PUCCH_CS_SET_ACK(1, 0, 0, SRSLTE_PUCCH_CS_MAX_ACK);

  return ret;
}

static int puccch_cs_get_ack_a3(uint32_t j, const uint8_t b[2], uint8_t uci_value[SRSLTE_UCI_MAX_ACK_BITS])
{
  int ret = SRSLTE_ERROR;

  PUCCH_CS_SET_ACK(1, 1, 1, 0, 1, 2);
  PUCCH_CS_SET_ACK(1, 1, 0, 0, 2);
  PUCCH_CS_SET_ACK(1, 0, 1, 1, 2);
  PUCCH_CS_SET_ACK(2, 1, 1, 2);
  PUCCH_CS_SET_ACK(0, 1, 1, 0, 1);
  PUCCH_CS_SET_ACK(0, 1, 0, 0);
  PUCCH_CS_SET_ACK(0, 0, 1, 1);
  PUCCH_CS_SET_ACK(1, 0, 0, SRSLTE_PUCCH_CS_MAX_ACK);

  return ret;
}

static int puccch_cs_get_ack_a4(uint32_t j, const uint8_t b[2], uint8_t uci_value[SRSLTE_UCI_MAX_ACK_BITS])
{
  int ret = SRSLTE_ERROR;

  PUCCH_CS_SET_ACK(1, 1, 1, 0, 1, 2, 3);
  PUCCH_CS_SET_ACK(2, 0, 1, 0, 2, 3);
  PUCCH_CS_SET_ACK(1, 0, 1, 1, 2, 3);
  PUCCH_CS_SET_ACK(3, 1, 1, 2, 3);
  PUCCH_CS_SET_ACK(1, 1, 0, 0, 1, 2);
  PUCCH_CS_SET_ACK(2, 0, 0, 0, 2);
  PUCCH_CS_SET_ACK(1, 0, 0, 1, 2);
  PUCCH_CS_SET_ACK(3, 1, 0, 2);
  PUCCH_CS_SET_ACK(2, 1, 1, 0, 1, 3);
  PUCCH_CS_SET_ACK(2, 1, 0, 0, 3);
  PUCCH_CS_SET_ACK(3, 0, 1, 1, 3);
  PUCCH_CS_SET_ACK(3, 0, 0, 3);
  PUCCH_CS_SET_ACK(0, 1, 1, 0, 1);
  PUCCH_CS_SET_ACK(0, 1, 0, 0);
  PUCCH_CS_SET_ACK(0, 0, 1, 1);
  PUCCH_CS_SET_ACK(0, 0, 0, SRSLTE_PUCCH_CS_MAX_ACK);

  return ret;
}

int srslte_pucch_cs_get_ack(const srslte_pucch_cfg_t* cfg,
                            const srslte_uci_cfg_t*   uci_cfg,
                            uint32_t                  j,
                            const uint8_t             b[2],
                            srslte_uci_value_t*       uci_value)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (cfg && uci_cfg && uci_value) {
    // Set bits to 0 by default
    memset(uci_value->ack.ack_value, 0, SRSLTE_UCI_MAX_ACK_BITS);
    uci_value->ack.valid = true;

    uint32_t nof_ack = srslte_uci_cfg_total_ack(uci_cfg);
    switch (nof_ack) {
      case 2:
        // A = 2
        ret = puccch_cs_get_ack_a2(j, b, uci_value->ack.ack_value);
        break;
      case 3:
        // A = 3
        ret = puccch_cs_get_ack_a3(j, b, uci_value->ack.ack_value);
        break;
      case 4:
        // A = 4
        ret = puccch_cs_get_ack_a4(j, b, uci_value->ack.ack_value);
        break;
      default:
        // Unhandled case
        ERROR("Unexpected number of ACK (%d)\n", nof_ack);
        ret = SRSLTE_ERROR;
    }
  }

  return ret;
}

static int pucch_f3_resources(const srslte_pucch_cfg_t* cfg, const srslte_uci_cfg_t* uci_cfg, uint32_t* n_pucch_3)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (cfg && uci_cfg && n_pucch_3) {
    n_pucch_3[0] = cfg->n3_pucch_an_list[uci_cfg->ack[0].tpc_for_pucch % SRSLTE_PUCCH_SIZE_AN_CS];
    ret          = 1;
  }

  return ret;
}

static uint32_t get_Np(uint32_t p, uint32_t nof_prb)
{
  if (p == 0) {
    return 0;
  } else {
    return nof_prb * (SRSLTE_NRE * p - 4) / 36;
  }
}

static uint32_t n_pucch_i_tdd(uint32_t ncce, uint32_t N_pucch_1, uint32_t nof_prb, uint32_t M, uint32_t m)
{
  uint32_t Np = 0, Np_1 = 0;
  for (uint32_t p = 0; p < 4; p++) {
    Np   = get_Np(p, nof_prb);
    Np_1 = get_Np(p + 1, nof_prb);
    if (ncce >= Np && ncce < Np_1) {
      uint32_t npucch = (M - m - 1) * Np + m * Np_1 + ncce + N_pucch_1;
      return npucch;
    }
  }
  ERROR("Could not find Np value for ncce=%d\n", ncce);
  return 0;
}

static int pucch_tdd_resources(const srslte_cell_t*      cell,
                               const srslte_pucch_cfg_t* cfg,
                               const srslte_uci_cfg_t*   uci_cfg,
                               uint32_t                  n_pucch_tdd[SRSLTE_PUCCH_MAX_ALLOC])
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  for (uint32_t i = 0; i < uci_cfg->ack[0].tdd_ack_M; i++) {
    n_pucch_tdd[i] =
        n_pucch_i_tdd(uci_cfg->ack[0].ncce[i], cfg->N_pucch_1, cell->nof_prb, uci_cfg->ack[0].tdd_ack_M, i);
  }

  return ret;
}

int srslte_pucch_proc_get_resources(const srslte_cell_t*      cell,
                                    const srslte_pucch_cfg_t* cfg,
                                    const srslte_uci_cfg_t*   uci_cfg,
                                    const srslte_uci_value_t* uci_value,
                                    uint32_t*                 n_pucch_i)
{
  if (!cfg || !cell || !uci_cfg || !n_pucch_i) {
    ERROR("pucch_resource_selection(): Invalid parameters\n");
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  uint32_t total_nof_ack = srslte_uci_cfg_total_ack(uci_cfg);

  // Available scheduling request and PUCCH format is not PUCCH3
  if (pucch_proc_tx_sr(uci_cfg, uci_value) && cfg->format != SRSLTE_PUCCH_FORMAT_3) {
    n_pucch_i[0] = cfg->n_pucch_sr;
    return 1;
  }

  // PUCCH formats 1, 1A and 1B (normal anb channel selection modes)
  if (cfg->format < SRSLTE_PUCCH_FORMAT_2) {
    if (cfg->sps_enabled) {
      n_pucch_i[0] = cfg->n_pucch_1[uci_cfg->ack[0].tpc_for_pucch % 4];
      return 1;
    }

    if (cell->frame_type == SRSLTE_TDD) {
      return pucch_tdd_resources(cell, cfg, uci_cfg, n_pucch_i);
    }

    if (cfg->ack_nack_feedback_mode == SRSLTE_PUCCH_ACK_NACK_FEEDBACK_MODE_CS) {
      return pucch_cs_resources(cfg, uci_cfg, n_pucch_i);
    }

    if (cfg->ack_nack_feedback_mode == SRSLTE_PUCCH_ACK_NACK_FEEDBACK_MODE_NORMAL ||
        (cfg->ack_nack_feedback_mode == SRSLTE_PUCCH_ACK_NACK_FEEDBACK_MODE_PUCCH3 &&
         total_nof_ack == uci_cfg->ack[0].nof_acks)) {
      // If normal or feedback mode PUCCH3 with only data in PCell
      n_pucch_i[0] = uci_cfg->ack[0].ncce[0] + cfg->N_pucch_1;
      return 1;
    }

    // Otherwise an error shall be prompt
    ERROR("Unhandled PUCCH format mode %s\n", srslte_ack_nack_feedback_mode_string(cfg->ack_nack_feedback_mode));
    return SRSLTE_ERROR;
  }

  // PUCCH format 3
  if (cfg->format == SRSLTE_PUCCH_FORMAT_3) {
    return pucch_f3_resources(cfg, uci_cfg, n_pucch_i);
  }

  // PUCCH format 2
  n_pucch_i[0] = cfg->n_pucch_2;
  return 1;
}

// Selection of n_pucch for PUCCH Format 1a and 1b with channel selection for 1 and 2 CC
static uint32_t get_npucch_cs(const srslte_pucch_cfg_t* cfg,
                              const srslte_uci_cfg_t*   uci_cfg,
                              const uint32_t            n_pucch_i[SRSLTE_PUCCH_MAX_ALLOC],
                              uint8_t                   b[SRSLTE_UCI_MAX_ACK_BITS])
{
  uint32_t n_pucch = 0;

  // Do resource selection and bit mapping according to tables 10.1.2.2.1-3, 10.1.2.2.1-4 and 10.1.2.2.1-5
  uint32_t nof_ack = srslte_uci_cfg_total_ack(uci_cfg);
  switch (nof_ack) {
    case 1:
      // 1-bit is Format1A always
      n_pucch = n_pucch_i[0];
      break;
    case 2:
      if (b[1] != 1) {
        /* n_pucch1_0 */
        n_pucch = n_pucch_i[0];
      } else {
        /* n_pucch1_1 */
        n_pucch = n_pucch_i[1];
      }
      if (b[0] == 1) {
        b[0] = 1;
        b[1] = 1;
      } else {
        b[0] = 0;
        b[1] = 0;
      }
      break;
    case 3:
      if (b[0] != 1 && b[1] != 1) {
        /* n_pucch1_2 */
        n_pucch = n_pucch_i[2];
      } else if (b[2] == 1) {
        /* n_pucch1_1 */
        n_pucch = n_pucch_i[1];
      } else {
        /* n_pucch1_0 */
        n_pucch = n_pucch_i[0];
      }
      if (b[0] != 1 && b[1] != 1 && b[2] != 1) {
        b[0] = 0;
        b[1] = 0;
      } else if (b[0] != 1 && b[1] == 1) {
        b[0] = 0;
        b[1] = 1;
      } else if (b[0] == 1 && b[1] != 1) {
        b[0] = 1;
        b[1] = 0;
      } else {
        b[0] = 1;
        b[1] = 1;
      }
      break;
    case 4:
      if (b[2] != 1 && b[3] != 1) {
        /* n_pucch1_0 */
        n_pucch = n_pucch_i[0];
      } else if (b[1] == 1 && b[2] == 1) {
        /* n_pucch1_1 */
        n_pucch = n_pucch_i[1];
      } else if (b[0] == 1) {
        /* n_pucch1_2 */
        n_pucch = n_pucch_i[2];
      } else {
        /* n_pucch1_3 */
        n_pucch = n_pucch_i[3];
      }
      if (b[2] != 1 && b[3] != 1) {
        /* n_pucch1_0 */
        b[0] = (uint8_t)(b[0] != 1 ? 0 : 1);
        b[1] = (uint8_t)(b[1] != 1 ? 0 : 1);
      } else if (b[1] == 1 && b[2] == 1) {
        /* n_pucch1_1 */
        b[0] = (uint8_t)(b[0] != 1 ? 0 : 1);
        b[1] = (uint8_t)(b[3] != 1 ? 0 : 1);
      } else if (b[0] == 1) {
        /* n_pucch1_2 */
        b[0] = (uint8_t)((b[3] != 1 ? 0 : 1) & (b[2] != 1 ? 1 : 0));
        b[1] = (uint8_t)((b[3] != 1 ? 0 : 1) & ((b[1] != 1 ? 0 : 1) ^ (b[2] != 1 ? 0 : 1)));
      } else {
        /* n_pucch1_3 */
        b[0] = (uint8_t)(b[2] != 1 ? 0 : 1);
        b[1] = (uint8_t)(((b[3] == 1) && ((b[1] == 1) != (b[2] == 1))) ? 1 : 0);
      }
      break;
    default:
      ERROR("Too many (%d) ACK for this CS mode\n", srslte_uci_cfg_total_ack(uci_cfg));
  }

  return n_pucch;
}

static void set_b01(uint8_t* b, uint8_t x)
{
  switch (x) {
    case 0:
      b[0] = 0;
      b[1] = 0;
      break;
    case 1:
      b[0] = 0;
      b[1] = 1;
      break;
    case 2:
      b[0] = 1;
      b[1] = 0;
      break;
    case 3:
      b[0] = 1;
      b[1] = 1;
      break;
    default:
      ERROR("Unhandled case (%d)\n", x);
  }
}

#define is_ack(h) (h == 1)
#define is_nack(h) (h == 0)
#define is_nackdtx(h) ((h & 1) == 0)
#define is_dtx(h) (h == 2)

// n_pucch and b0b1 selection for TDD, tables 10.1-2, 10.1-3 and 10.1-4
static uint32_t
get_npucch_tdd(const uint32_t n_pucch[4], const srslte_uci_cfg_t* uci_cfg, uint8_t b[SRSLTE_UCI_MAX_ACK_BITS])
{
  switch (uci_cfg->ack[0].nof_acks) {
    case 1:
      return n_pucch[0];
    case 2:
      if (is_ack(b[0]) && is_ack(b[1])) {
        set_b01(b, 3);
        return n_pucch[1];
      } else if (is_ack(b[0]) && is_nackdtx(b[1])) {
        set_b01(b, 1);
        return n_pucch[0];
      } else if (is_nackdtx(b[0]) && is_ack(b[1])) {
        set_b01(b, 0);
        return n_pucch[1];
      } else if (is_nackdtx(b[0]) && is_nack(b[1])) {
        set_b01(b, 2);
        return n_pucch[1];
      } else if (is_nack(b[0]) && is_dtx(b[1])) {
        set_b01(b, 2);
        return n_pucch[0];
      }
      break;
    case 3:
      if (is_ack(b[0]) && is_ack(b[1]) && is_ack(b[2])) {
        set_b01(b, 3);
        return n_pucch[2];
      } else if (is_ack(b[0]) && is_ack(b[1]) && is_nackdtx(b[2])) {
        set_b01(b, 3);
        return n_pucch[1];
      } else if (is_ack(b[0]) && is_nackdtx(b[1]) && is_ack(b[2])) {
        set_b01(b, 3);
        return n_pucch[0];
      } else if (is_ack(b[0]) && is_nackdtx(b[1]) && is_nackdtx(b[2])) {
        set_b01(b, 1);
        return n_pucch[0];
      } else if (is_nackdtx(b[0]) && is_ack(b[1]) && is_ack(b[2])) {
        set_b01(b, 2);
        return n_pucch[2];
      } else if (is_nackdtx(b[0]) && is_ack(b[1]) && is_nackdtx(b[2])) {
        set_b01(b, 0);
        return n_pucch[1];
      } else if (is_nackdtx(b[0]) && is_nackdtx(b[1]) && is_ack(b[2])) {
        set_b01(b, 0);
        return n_pucch[2];
      } else if (is_dtx(b[0]) && is_dtx(b[1]) && is_nack(b[2])) {
        set_b01(b, 1);
        return n_pucch[2];
      } else if (is_dtx(b[0]) && is_nack(b[1]) && is_nackdtx(b[2])) {
        set_b01(b, 2);
        return n_pucch[1];
      } else if (is_nack(b[0]) && is_nackdtx(b[1]) && is_nackdtx(b[2])) {
        set_b01(b, 2);
        return n_pucch[0];
      }
      break;
    case 4:
      if (is_ack(b[0]) && is_ack(b[1]) && is_ack(b[2]) && is_ack(b[3])) {
        set_b01(b, 3);
        return n_pucch[1];
      } else if (is_ack(b[0]) && is_ack(b[1]) && is_ack(b[2]) && is_nackdtx(b[3])) {
        set_b01(b, 2);
        return n_pucch[1];
      } else if (is_nackdtx(b[0]) && is_nackdtx(b[1]) && is_nack(b[2]) && is_dtx(b[3])) {
        set_b01(b, 3);
        return n_pucch[2];
      } else if (is_ack(b[0]) && is_ack(b[1]) && is_nackdtx(b[2]) && is_ack(b[3])) {
        set_b01(b, 2);
        return n_pucch[1];
      } else if (is_nack(b[0]) && is_dtx(b[1]) && is_dtx(b[2]) && is_dtx(b[3])) {
        set_b01(b, 2);
        return n_pucch[0];
      } else if (is_ack(b[0]) && is_ack(b[1]) && is_nackdtx(b[2]) && is_nackdtx(b[3])) {
        set_b01(b, 2);
        return n_pucch[1];
      } else if (is_ack(b[0]) && is_nackdtx(b[1]) && is_ack(b[2]) && is_ack(b[3])) {
        set_b01(b, 1);
        return n_pucch[3];
      } else if (is_nackdtx(b[0]) && is_nackdtx(b[1]) && is_nackdtx(b[2]) && is_nack(b[3])) {
        set_b01(b, 3);
        return n_pucch[3];
      } else if (is_ack(b[0]) && is_nackdtx(b[1]) && is_ack(b[2]) && is_nack(b[3])) {
        set_b01(b, 2);
        return n_pucch[1];
      } else if (is_ack(b[0]) && is_nackdtx(b[1]) && is_nackdtx(b[2]) && is_ack(b[3])) {
        set_b01(b, 1);
        return n_pucch[0];
      } else if (is_ack(b[0]) && is_nackdtx(b[1]) && is_nackdtx(b[2]) && is_nackdtx(b[3])) {
        set_b01(b, 3);
        return n_pucch[0];
      } else if (is_nackdtx(b[0]) && is_ack(b[1]) && is_ack(b[2]) && is_ack(b[3])) {
        set_b01(b, 1);
        return n_pucch[3];
      } else if (is_nackdtx(b[0]) && is_nack(b[1]) && is_dtx(b[2]) && is_dtx(b[3])) {
        set_b01(b, 0);
        return n_pucch[1];
      } else if (is_nackdtx(b[0]) && is_ack(b[1]) && is_ack(b[2]) && is_nackdtx(b[3])) {
        set_b01(b, 2);
        return n_pucch[2];
      } else if (is_nackdtx(b[0]) && is_ack(b[1]) && is_nackdtx(b[2]) && is_ack(b[3])) {
        set_b01(b, 2);
        return n_pucch[3];
      } else if (is_nackdtx(b[0]) && is_ack(b[1]) && is_nackdtx(b[2]) && is_nackdtx(b[3])) {
        set_b01(b, 1);
        return n_pucch[1];
      } else if (is_nackdtx(b[0]) && is_nackdtx(b[1]) && is_ack(b[2]) && is_ack(b[3])) {
        set_b01(b, 1);
        return n_pucch[3];
      } else if (is_nackdtx(b[0]) && is_nackdtx(b[1]) && is_ack(b[2]) && is_nackdtx(b[3])) {
        set_b01(b, 0);
        return n_pucch[2];
      } else if (is_nackdtx(b[0]) && is_nackdtx(b[1]) && is_nackdtx(b[2]) && is_ack(b[3])) {
        set_b01(b, 0);
        return n_pucch[3];
      }
  }
  return 0;
}

uint32_t srslte_pucch_proc_get_npucch(const srslte_cell_t*      cell,
                                      const srslte_pucch_cfg_t* cfg,
                                      const srslte_uci_cfg_t*   uci_cfg,
                                      const srslte_uci_value_t* uci_value,
                                      uint8_t                   b[SRSLTE_UCI_MAX_ACK_BITS])
{
  uint32_t n_pucch_i[SRSLTE_PUCCH_MAX_ALLOC] = {};
  int      nof_resources = srslte_pucch_proc_get_resources(cell, cfg, uci_cfg, uci_value, n_pucch_i);

  // Copy original bits in b
  memcpy(b, uci_value->ack.ack_value, SRSLTE_UCI_MAX_ACK_BITS);

  // If error occurred, return 0
  if (nof_resources < 1) {
    return 0;
  }

  // Return immediately if only one possible resource
  if (nof_resources == 1) {
    return n_pucch_i[0];
  }

  // Select TDD resource
  if (cell->frame_type == SRSLTE_TDD) {
    return get_npucch_tdd(n_pucch_i, uci_cfg, b);
  }

  // Select Channel Selection resource
  if (cfg->ack_nack_feedback_mode == SRSLTE_PUCCH_ACK_NACK_FEEDBACK_MODE_CS) {
    return get_npucch_cs(cfg, uci_cfg, n_pucch_i, b);
  }

  return 0;
}
