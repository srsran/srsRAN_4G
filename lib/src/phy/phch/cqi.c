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

#include "srslte/srslte.h"
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/phch/cqi.h"
#include "srslte/phy/utils/bit.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"

/*******************************************************
 *            PACKING FUNCTIONS                        *
 *******************************************************/
static int cqi_hl_subband_pack(srslte_cqi_cfg_t* cfg, srslte_cqi_hl_subband_t* msg, uint8_t* buff)
{
  uint8_t* body_ptr  = buff;
  uint32_t bit_count = 0;

  /* Unpack codeword 0, common for 3GPP 36.212 Tables 5.2.2.6.2-1 and 5.2.2.6.2-2 */
  srslte_bit_unpack(msg->wideband_cqi_cw0, &body_ptr, 4);
  srslte_bit_unpack(msg->subband_diff_cqi_cw0, &body_ptr, 2 * cfg->N);
  bit_count += 4 + 2 * cfg->N;

  /* Unpack codeword 1, 3GPP 36.212 Table 5.2.2.6.2-2 */
  if (cfg->rank_is_not_one) {
    srslte_bit_unpack(msg->wideband_cqi_cw1, &body_ptr, 4);
    srslte_bit_unpack(msg->subband_diff_cqi_cw1, &body_ptr, 2 * cfg->N);
    bit_count += 4 + 2 * cfg->N;
  }

  /* If PMI is present, unpack it */
  if (cfg->pmi_present) {
    if (cfg->four_antenna_ports) {
      srslte_bit_unpack(msg->pmi, &body_ptr, 4);
      bit_count += 4;
    } else {
      if (cfg->rank_is_not_one) {
        srslte_bit_unpack(msg->pmi, &body_ptr, 1);
        bit_count += 1;
      } else {
        srslte_bit_unpack(msg->pmi, &body_ptr, 2);
        bit_count += 2;
      }
    }
  }

  return bit_count;
}

static int cqi_ue_subband_pack(srslte_cqi_cfg_t* cfg, srslte_cqi_ue_subband_t* msg, uint8_t* buff)
{
  uint8_t* body_ptr = buff;
  srslte_bit_unpack(msg->wideband_cqi, &body_ptr, 4);
  srslte_bit_unpack(msg->subband_diff_cqi, &body_ptr, 2);
  srslte_bit_unpack(msg->subband_diff_cqi, &body_ptr, cfg->L);

  return 4 + 2 + cfg->L;
}

/* Pack CQI following 3GPP TS 36.212 Tables 5.2.3.3.1-1 and 5.2.3.3.1-2 */
static int cqi_format2_wideband_pack(srslte_cqi_cfg_t* cfg, srslte_cqi_format2_wideband_t* msg, uint8_t* buff)
{
  uint8_t* body_ptr = buff;

  srslte_bit_unpack(msg->wideband_cqi, &body_ptr, 4);

  if (cfg->pmi_present) {
    if (cfg->four_antenna_ports) {
      if (cfg->rank_is_not_one) {
        srslte_bit_unpack(msg->spatial_diff_cqi, &body_ptr, 3);
      }
      srslte_bit_unpack(msg->pmi, &body_ptr, 4);
    } else {
      if (cfg->rank_is_not_one) {
        srslte_bit_unpack(msg->spatial_diff_cqi, &body_ptr, 3);
        srslte_bit_unpack(msg->pmi, &body_ptr, 1);
      } else {
        srslte_bit_unpack(msg->pmi, &body_ptr, 2);
      }
    }
  }

  return (int)(body_ptr - buff);
}

static int cqi_format2_subband_pack(srslte_cqi_cfg_t* cfg, srslte_cqi_format2_subband_t* msg, uint8_t* buff)
{
  uint8_t* body_ptr = buff;
  srslte_bit_unpack(msg->subband_cqi, &body_ptr, 4);
  srslte_bit_unpack(msg->subband_label, &body_ptr, cfg->subband_label_2_bits ? 2 : 1);
  return 4 + ((cfg->subband_label_2_bits) ? 2 : 1);
}

int srslte_cqi_value_pack(srslte_cqi_cfg_t* cfg, srslte_cqi_value_t* value, uint8_t buff[SRSLTE_CQI_MAX_BITS])
{
  switch (cfg->type) {
    case SRSLTE_CQI_TYPE_WIDEBAND:
      return cqi_format2_wideband_pack(cfg, &value->wideband, buff);
    case SRSLTE_CQI_TYPE_SUBBAND:
      return cqi_format2_subband_pack(cfg, &value->subband, buff);
    case SRSLTE_CQI_TYPE_SUBBAND_UE:
      return cqi_ue_subband_pack(cfg, &value->subband_ue, buff);
    case SRSLTE_CQI_TYPE_SUBBAND_HL:
      return cqi_hl_subband_pack(cfg, &value->subband_hl, buff);
  }
  return -1;
}

/*******************************************************
 *          UNPACKING FUNCTIONS                        *
 *******************************************************/

int cqi_hl_subband_unpack(srslte_cqi_cfg_t* cfg, uint8_t* buff, srslte_cqi_hl_subband_t* msg)
{
  uint8_t* body_ptr  = buff;
  uint32_t bit_count = 0;

  msg->wideband_cqi_cw0     = (uint8_t)srslte_bit_pack(&body_ptr, 4);
  msg->subband_diff_cqi_cw0 = srslte_bit_pack(&body_ptr, 2 * cfg->N);
  bit_count += 4 + 2 * cfg->N;

  /* Unpack codeword 1, 3GPP 36.212 Table 5.2.2.6.2-2 */
  if (cfg->rank_is_not_one) {
    msg->wideband_cqi_cw1     = (uint8_t)srslte_bit_pack(&body_ptr, 4);
    msg->subband_diff_cqi_cw1 = srslte_bit_pack(&body_ptr, 2 * cfg->N);
    bit_count += 4 + 2 * cfg->N;
  }

  /* If PMI is present, unpack it */
  if (cfg->pmi_present) {
    if (cfg->four_antenna_ports) {
      msg->pmi = (uint8_t)srslte_bit_pack(&body_ptr, 4);
      bit_count += 4;
    } else {
      if (cfg->rank_is_not_one) {
        msg->pmi = (uint8_t)srslte_bit_pack(&body_ptr, 1);
        bit_count += 1;
      } else {
        msg->pmi = (uint8_t)srslte_bit_pack(&body_ptr, 2);
        bit_count += 2;
      }
    }
  }

  return bit_count;
}

int cqi_ue_subband_unpack(srslte_cqi_cfg_t* cfg, uint8_t* buff, srslte_cqi_ue_subband_t* msg)
{
  uint8_t* body_ptr     = buff;
  msg->wideband_cqi     = srslte_bit_pack(&body_ptr, 4);
  msg->subband_diff_cqi = srslte_bit_pack(&body_ptr, 2);
  msg->subband_diff_cqi = srslte_bit_pack(&body_ptr, cfg->L);

  return 4 + 2 + cfg->L;
}

/* Unpack CQI following 3GPP TS 36.212 Tables 5.2.3.3.1-1 and 5.2.3.3.1-2 */
static int cqi_format2_wideband_unpack(srslte_cqi_cfg_t* cfg, uint8_t* buff, srslte_cqi_format2_wideband_t* msg)
{
  uint8_t* body_ptr = buff;
  msg->wideband_cqi = (uint8_t)srslte_bit_pack(&body_ptr, 4);

  if (cfg->pmi_present) {
    if (cfg->four_antenna_ports) {
      if (cfg->rank_is_not_one) {
        msg->spatial_diff_cqi = (uint8_t)srslte_bit_pack(&body_ptr, 3);
      }
      msg->pmi = (uint8_t)srslte_bit_pack(&body_ptr, 4);
    } else {
      if (cfg->rank_is_not_one) {
        msg->spatial_diff_cqi = (uint8_t)srslte_bit_pack(&body_ptr, 3);
        msg->pmi              = (uint8_t)srslte_bit_pack(&body_ptr, 1);
      } else {
        msg->pmi = (uint8_t)srslte_bit_pack(&body_ptr, 2);
      }
    }
  }
  return 4;
}

static int cqi_format2_subband_unpack(srslte_cqi_cfg_t* cfg, uint8_t* buff, srslte_cqi_format2_subband_t* msg)
{
  uint8_t* body_ptr  = buff;
  msg->subband_cqi   = srslte_bit_pack(&body_ptr, 4);
  msg->subband_label = srslte_bit_pack(&body_ptr, cfg->subband_label_2_bits ? 2 : 1);
  return 4 + ((cfg->subband_label_2_bits) ? 2 : 1);
}

int srslte_cqi_value_unpack(srslte_cqi_cfg_t* cfg, uint8_t buff[SRSLTE_CQI_MAX_BITS], srslte_cqi_value_t* value)
{
  switch (cfg->type) {
    case SRSLTE_CQI_TYPE_WIDEBAND:
      return cqi_format2_wideband_unpack(cfg, buff, &value->wideband);
    case SRSLTE_CQI_TYPE_SUBBAND:
      return cqi_format2_subband_unpack(cfg, buff, &value->subband);
    case SRSLTE_CQI_TYPE_SUBBAND_UE:
      return cqi_ue_subband_unpack(cfg, buff, &value->subband_ue);
    case SRSLTE_CQI_TYPE_SUBBAND_HL:
      return cqi_hl_subband_unpack(cfg, buff, &value->subband_hl);
  }
  return -1;
}

/*******************************************************
 *                TO STRING FUNCTIONS                  *
 *******************************************************/

static int
cqi_format2_wideband_tostring(srslte_cqi_cfg_t* cfg, srslte_cqi_format2_wideband_t* msg, char* buff, uint32_t buff_len)
{
  int n = 0;

  n += snprintf(buff + n, buff_len - n, ", cqi=%d", msg->wideband_cqi);

  if (cfg->pmi_present) {
    if (cfg->rank_is_not_one) {
      n += snprintf(buff + n, buff_len - n, ", diff_cqi=%d", msg->spatial_diff_cqi);
    }
    n += snprintf(buff + n, buff_len - n, ", pmi=%d", msg->pmi);
  }

  return n;
}

static int
cqi_format2_subband_tostring(srslte_cqi_cfg_t* cfg, srslte_cqi_format2_subband_t* msg, char* buff, uint32_t buff_len)
{
  int n = 0;

  n += snprintf(buff + n, buff_len - n, ", cqi=%d", msg->subband_cqi);
  n += snprintf(buff + n, buff_len - n, ", label=%d", msg->subband_label);

  return n;
}

static int cqi_ue_subband_tostring(srslte_cqi_cfg_t* cfg, srslte_cqi_ue_subband_t* msg, char* buff, uint32_t buff_len)
{
  int n = 0;

  n += snprintf(buff + n, buff_len - n, ", cqi=%d", msg->wideband_cqi);
  n += snprintf(buff + n, buff_len - n, ", diff_cqi=%d", msg->subband_diff_cqi);
  n += snprintf(buff + n, buff_len - n, ", L=%d", cfg->L);

  return n;
}

static int cqi_hl_subband_tostring(srslte_cqi_cfg_t* cfg, srslte_cqi_hl_subband_t* msg, char* buff, uint32_t buff_len)
{
  int n = 0;

  n += snprintf(buff + n, buff_len - n, ", cqi=%d", msg->wideband_cqi_cw0);
  n += snprintf(buff + n, buff_len - n, ", diff=%d", msg->subband_diff_cqi_cw0);

  if (cfg->rank_is_not_one) {
    n += snprintf(buff + n, buff_len - n, ", cqi1=%d", msg->wideband_cqi_cw1);
    n += snprintf(buff + n, buff_len - n, ", diff1=%d", msg->subband_diff_cqi_cw1);
  }

  if (cfg->pmi_present) {
    n += snprintf(buff + n, buff_len - n, ", pmi=%d", msg->pmi);
  }

  n += snprintf(buff + n, buff_len - n, ", N=%d", cfg->N);

  return n;
}

int srslte_cqi_value_tostring(srslte_cqi_cfg_t* cfg, srslte_cqi_value_t* value, char* buff, uint32_t buff_len)
{
  int ret = -1;

  switch (cfg->type) {
    case SRSLTE_CQI_TYPE_WIDEBAND:
      ret = cqi_format2_wideband_tostring(cfg, &value->wideband, buff, buff_len);
      break;
    case SRSLTE_CQI_TYPE_SUBBAND:
      ret = cqi_format2_subband_tostring(cfg, &value->subband, buff, buff_len);
      break;
    case SRSLTE_CQI_TYPE_SUBBAND_UE:
      ret = cqi_ue_subband_tostring(cfg, &value->subband_ue, buff, buff_len);
      break;
    case SRSLTE_CQI_TYPE_SUBBAND_HL:
      ret = cqi_hl_subband_tostring(cfg, &value->subband_hl, buff, buff_len);
      break;
    default:
      /* Do nothing */;
  }

  return ret;
}

int srslte_cqi_size(srslte_cqi_cfg_t* cfg)
{
  int size = 0;

  if (!cfg->data_enable) {
    return cfg->ri_len;
  }

  switch (cfg->type) {
    case SRSLTE_CQI_TYPE_WIDEBAND:
      /* Compute size according to 3GPP TS 36.212 Tables 5.2.3.3.1-1 and 5.2.3.3.1-2 */
      size = 4;
      if (cfg->pmi_present) {
        if (cfg->four_antenna_ports) {
          if (cfg->rank_is_not_one) {
            size += 3; // Differential
          } else {
            size += 0; // Differential
          }
          size += 4; // PMI
        } else {
          if (cfg->rank_is_not_one) {
            size += 3; // Differential
            size += 1; // PMI
          } else {
            size += 0; // Differential
            size += 2; // PMI
          }
        }
      }
      break;
    case SRSLTE_CQI_TYPE_SUBBAND:
      size = 4 + ((cfg->subband_label_2_bits) ? 2 : 1);
      break;
    case SRSLTE_CQI_TYPE_SUBBAND_UE:
      size = 4 + 2 + cfg->L;
      break;
    case SRSLTE_CQI_TYPE_SUBBAND_HL:
      /* First codeword */
      size += 4 + 2 * cfg->N;

      /* Add Second codeword if required */
      if (cfg->rank_is_not_one && cfg->pmi_present) {
        size += 4 + 2 * cfg->N;
      }

      /* Add PMI if required*/
      if (cfg->pmi_present) {
        if (cfg->four_antenna_ports) {
          size += 4;
        } else {
          if (cfg->rank_is_not_one) {
            size += 1;
          } else {
            size += 2;
          }
        }
      }
      break;
    default:
      size = SRSLTE_ERROR;
  }
  return size;
}

static bool cqi_get_N_fdd(uint32_t I_cqi_pmi, uint32_t* N_p, uint32_t* N_offset)
{
  // Acccording to 3GPP 36.213 R10 Table 7.2.2-1A: Mapping of I CQI / PMI to N pd and N OFFSET , CQI for FDD
  if (I_cqi_pmi <= 1) {
    *N_p      = 2;
    *N_offset = I_cqi_pmi;
  } else if (I_cqi_pmi <= 6) {
    *N_p      = 5;
    *N_offset = I_cqi_pmi - 2;
  } else if (I_cqi_pmi <= 16) {
    *N_p      = 10;
    *N_offset = I_cqi_pmi - 7;
  } else if (I_cqi_pmi <= 36) {
    *N_p      = 20;
    *N_offset = I_cqi_pmi - 17;
  } else if (I_cqi_pmi <= 76) {
    *N_p      = 40;
    *N_offset = I_cqi_pmi - 37;
  } else if (I_cqi_pmi <= 156) {
    *N_p      = 80;
    *N_offset = I_cqi_pmi - 77;
  } else if (I_cqi_pmi <= 316) {
    *N_p      = 160;
    *N_offset = I_cqi_pmi - 157;
  } else if (I_cqi_pmi == 317) {
    return false;
  } else if (I_cqi_pmi <= 349) {
    *N_p      = 32;
    *N_offset = I_cqi_pmi - 318;
  } else if (I_cqi_pmi <= 413) {
    *N_p      = 64;
    *N_offset = I_cqi_pmi - 350;
  } else if (I_cqi_pmi <= 541) {
    *N_p      = 128;
    *N_offset = I_cqi_pmi - 414;
  } else if (I_cqi_pmi <= 1023) {
    return false;
  }
  return true;
}

static bool cqi_get_N_tdd(uint32_t I_cqi_pmi, uint32_t* N_p, uint32_t* N_offset)
{
  // Acccording to 3GPP 36.213 R10 Table 7.2.2-1A: Mapping of I CQI / PMI to N pd and N OFFSET , CQI for TDD
  if (I_cqi_pmi == 0) {
    *N_p      = 1;
    *N_offset = I_cqi_pmi;
  } else if (I_cqi_pmi <= 5) {
    *N_p      = 5;
    *N_offset = I_cqi_pmi - 1;
  } else if (I_cqi_pmi <= 15) {
    *N_p      = 10;
    *N_offset = I_cqi_pmi - 5;
  } else if (I_cqi_pmi <= 35) {
    *N_p      = 20;
    *N_offset = I_cqi_pmi - 16;
  } else if (I_cqi_pmi <= 75) {
    *N_p      = 40;
    *N_offset = I_cqi_pmi - 36;
  } else if (I_cqi_pmi <= 155) {
    *N_p      = 80;
    *N_offset = I_cqi_pmi - 76;
  } else if (I_cqi_pmi <= 315) {
    *N_p      = 160;
    *N_offset = I_cqi_pmi - 156;
  } else if (I_cqi_pmi == 1023) {
    return false;
  }
  return true;
}

static bool cqi_send(uint32_t I_cqi_pmi, uint32_t tti, bool is_fdd)
{

  uint32_t N_p      = 0;
  uint32_t N_offset = 0;

  if (is_fdd) {
    if (!cqi_get_N_fdd(I_cqi_pmi, &N_p, &N_offset)) {
      return false;
    }
  } else {
    if (!cqi_get_N_tdd(I_cqi_pmi, &N_p, &N_offset)) {
      return false;
    }
  }

  if (N_p) {
    if ((tti - N_offset) % N_p == 0) {
      return true;
    }
  }
  return false;
}

static bool ri_send(uint32_t I_cqi_pmi, uint32_t I_ri, uint32_t tti, bool is_fdd)
{

  uint32_t M_ri        = 0;
  int      N_offset_ri = 0;
  uint32_t N_p         = 0;
  uint32_t N_offset_p  = 0;

  if (is_fdd) {
    if (!cqi_get_N_fdd(I_cqi_pmi, &N_p, &N_offset_p)) {
      return false;
    }
  } else {
    if (!cqi_get_N_tdd(I_cqi_pmi, &N_p, &N_offset_p)) {
      return false;
    }
  }

  // According to 3GPP 36.213 R10 Table 7.2.2-1B Mapping I_RI to M_RI and N_OFFSET_RI
  if (I_ri <= 160) {
    M_ri        = 1;
    N_offset_ri = -I_ri;
  } else if (I_ri <= 321) {
    M_ri        = 2;
    N_offset_ri = -(I_ri - 161);
  } else if (I_ri <= 482) {
    M_ri        = 4;
    N_offset_ri = -(I_ri - 322);
  } else if (I_ri <= 643) {
    M_ri        = 8;
    N_offset_ri = -(I_ri - 483);
  } else if (I_ri <= 804) {
    M_ri        = 16;
    N_offset_ri = -(I_ri - 644);
  } else if (I_ri <= 965) {
    M_ri        = 32;
    N_offset_ri = -(I_ri - 805);
  } else {
    return false;
  }

  if (M_ri && N_p) {
    if ((tti - N_offset_p - N_offset_ri) % (N_p * M_ri) == 0) {
      return true;
    }
  }
  return false;
}

bool srslte_cqi_periodic_ri_send(const srslte_cqi_report_cfg_t* cfg, uint32_t tti, srslte_frame_type_t frame_type)
{
  return cfg->periodic_configured && cfg->ri_idx_present &&
         ri_send(cfg->pmi_idx, cfg->ri_idx, tti, frame_type == SRSLTE_FDD);
}

bool srslte_cqi_periodic_send(const srslte_cqi_report_cfg_t* cfg, uint32_t tti, srslte_frame_type_t frame_type)
{
  return cfg->periodic_configured && cqi_send(cfg->pmi_idx, tti, frame_type == SRSLTE_FDD);
}

// CQI-to-Spectral Efficiency:  36.213 Table 7.2.3-1
static float cqi_to_coderate[16] = {0,
                                    0.1523,
                                    0.2344,
                                    0.3770,
                                    0.6016,
                                    0.8770,
                                    1.1758,
                                    1.4766,
                                    1.9141,
                                    2.4063,
                                    2.7305,
                                    3.3223,
                                    3.9023,
                                    4.5234,
                                    5.1152,
                                    5.5547};

// CQI-to-Spectral Efficiency:  36.213 Table 7.2.3-2
static float cqi_to_coderate_table2[16] = {0,
                                           0.1523,
                                           0.3770,
                                           0.8770,
                                           1.4766,
                                           1.9141,
                                           2.4063,
                                           2.7305,
                                           3.3223,
                                           3.9023,
                                           4.5234,
                                           5.1152,
                                           5.5547,
                                           6.2266,
                                           6.9141,
                                           7.4063};

float srslte_cqi_to_coderate(uint32_t cqi, bool use_alt_table)
{
  if (cqi < 16) {
    return use_alt_table ? cqi_to_coderate_table2[cqi] : cqi_to_coderate[cqi];
  } else {
    return 0;
  }
}

/* SNR-to-CQI conversion, got from "Downlink SNR to CQI Mapping for Different Multiple Antenna Techniques in LTE"
 * Table III.
 */
// From paper
static float cqi_to_snr_table[15] = {1.95, 4, 6, 8, 10, 11.95, 14.05, 16, 17.9, 20.9, 22.5, 24.75, 25.5, 27.30, 29};

// From experimental measurements @ 5 MHz
// static float cqi_to_snr_table[15] = { 1, 1.75, 3, 4, 5, 6, 7.5, 9, 11.5, 13.0, 15.0, 18, 20, 22.5, 26.5};

uint8_t srslte_cqi_from_snr(float snr)
{
  for (int cqi = 14; cqi >= 0; cqi--) {
    if (snr >= cqi_to_snr_table[cqi]) {
      return (uint8_t)cqi + 1;
    }
  }
  return 0;
}

/* Returns the subband size for higher layer-configured subband feedback,
 * i.e., the number of RBs per subband as a function of the cell bandwidth
 * (Table 7.2.1-3 in TS 36.213)
 */
static int cqi_hl_get_subband_size(int nof_prb)
{
  if (nof_prb < 7) {
    return 0;
  } else if (nof_prb <= 26) {
    return 4;
  } else if (nof_prb <= 63) {
    return 6;
  } else if (nof_prb <= 110) {
    return 8;
  } else {
    return -1;
  }
}

/* Returns the number of subbands to be reported in CQI measurements as
 * defined in clause 7.2 in TS 36.213, i.e., the N parameter
 */
int srslte_cqi_hl_get_no_subbands(int nof_prb)
{
  int hl_size = cqi_hl_get_subband_size(nof_prb);
  if (hl_size > 0) {
    return (int)ceil((float)nof_prb / hl_size);
  } else {
    return 0;
  }
}

void srslte_cqi_to_str(const uint8_t* cqi_value, int cqi_len, char* str, int str_len)
{
  int i = 0;

  for (i = 0; i < cqi_len && i < (str_len - 5); i++) {
    str[i] = (cqi_value[i] == 0) ? (char)'0' : (char)'1';
  }

  if (i == (str_len - 5)) {
    str[i++] = '.';
    str[i++] = '.';
    str[i++] = '.';
    str[i++] = (cqi_value[cqi_len - 1] == 0) ? (char)'0' : (char)'1';
  }
  str[i] = '\0';
}
