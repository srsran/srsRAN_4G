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

#include <sys/time.h>

#include "srsran/phy/common/phy_common_sl.h"
#include "srsran/phy/phch/ra.h"
#include "srsran/phy/phch/ra_sl.h"
#include "srsran/phy/utils/random.h"

int srsran_sl_get_available_pool_prb(uint32_t prb_num, uint32_t prb_start, uint32_t prb_end)
{
  if ((prb_num * 2) <= (prb_end - prb_start + 1)) {
    return prb_num * 2;
  } else {
    return prb_num * 2 - 1;
  }
}

int srsran_pscch_resources(uint32_t  prb_num,
                           uint32_t  prb_start,
                           uint32_t  prb_end,
                           uint8_t*  pscch_sf_bitmap,
                           uint32_t  period_length,
                           uint32_t  n_pscch,
                           uint32_t* m_a,
                           uint32_t* l_b)
{
  uint8_t  M = srsran_sl_get_available_pool_prb(prb_num, prb_start, prb_end);
  uint32_t L = 0;
  for (uint32_t i = 0; i < period_length; i++) {
    if (pscch_sf_bitmap[i] == 1) {
      L++;
    }
  }

  if (L < 2) {
    // ERROR("Invalid PSCCH subframe bitmap");
    return SRSRAN_ERROR;
  }

  n_pscch = n_pscch % (L * M / 2);

  uint32_t a1 = n_pscch / L;                        // RB index A
  uint32_t b1 = n_pscch % L;                        // SF index A
  uint32_t a2 = a1 + M / 2;                         // RB index B
  uint32_t b2 = (n_pscch + 1 + (a1 % (L - 1))) % L; // SF index B

  m_a[0] = a1 + prb_start;         // PRB 1
  m_a[1] = a2 + (prb_end + 1 - M); // PRB 2

  uint32_t k = 0;
  for (uint32_t i = 0; i < period_length; i++) {
    if (pscch_sf_bitmap[i] == 1) {
      k++;
      if (k == (b1 + 1)) {
        l_b[0] = i;
        break;
      }
    }
  }

  k = 0;
  for (uint32_t i = 0; i < period_length; i++) {
    if (pscch_sf_bitmap[i] == 1) {
      k++;
      if (k == (b2 + 1)) {
        l_b[1] = i;
        break;
      }
    }
  }

  return SRSRAN_SUCCESS;
}

// 3GPP TS 36.213 Section 8.1.1. Uplink resource allocation type 0
uint32_t srsran_ra_sl_type0_to_riv(uint32_t nof_prb, uint32_t prb_start, uint32_t L_crb)
{
  return srsran_ra_type2_to_riv(L_crb, prb_start, nof_prb);
}

// 3GPP TS 36.213 Section 8.1.1. Uplink resource allocation type 0
void srsran_ra_sl_type0_from_riv(uint32_t riv, uint32_t nof_prb, uint32_t* L_crb, uint32_t* prb_start)
{
  srsran_ra_type2_from_riv(riv, L_crb, prb_start, nof_prb, nof_prb);
}

int srsran_ra_sl_pssch_allowed_sf(uint32_t pssch_sf_idx, uint32_t trp_idx, uint32_t duplex_mode, uint32_t tdd_config)
{
  if (duplex_mode == SRSRAN_SL_DUPLEX_MODE_FDD) {
    return srsran_sl_N_TRP_8[trp_idx][pssch_sf_idx % 8];
  }
  switch (tdd_config) {
    // N_TRP = 6
    case 3:
    case 6:
      return srsran_sl_N_TRP_6[trp_idx][pssch_sf_idx % 6];
    // N_TRP = 7
    case 0:
      return srsran_sl_N_TRP_7[trp_idx][pssch_sf_idx % 7];
    // N_TRP = 8
    case 1:
    case 2:
    case 4:
    case 5:
      return srsran_sl_N_TRP_8[trp_idx][pssch_sf_idx % 8];
    default:
      return 0;
  }
}

int srsran_sci_generate_trp_idx(uint32_t duplex_mode, uint32_t tdd_config, uint32_t k_TRP)
{
  int            retval = SRSRAN_ERROR;
  struct timeval tv;
  gettimeofday(&tv, NULL);
  srsran_random_t random = srsran_random_init(tv.tv_usec);

  // N_TRP = 8
  if (duplex_mode == SRSRAN_SL_DUPLEX_MODE_FDD || tdd_config == 1 || tdd_config == 2 || tdd_config == 4 ||
      tdd_config == 5) {
    switch (k_TRP) {
      case 1:
        retval = srsran_sl_N_TRP_8_k_1[srsran_random_uniform_int_dist(random, 0, sizeof(srsran_sl_N_TRP_8_k_1) - 1)];
        break;
      case 2:
        retval = srsran_sl_N_TRP_8_k_2[srsran_random_uniform_int_dist(random, 0, sizeof(srsran_sl_N_TRP_8_k_2) - 1)];
        break;
      case 4:
        retval = srsran_sl_N_TRP_8_k_4[srsran_random_uniform_int_dist(random, 0, sizeof(srsran_sl_N_TRP_8_k_4) - 1)];
        break;
      case 8:
        retval = srsran_sl_N_TRP_8_k_8[srsran_random_uniform_int_dist(random, 0, sizeof(srsran_sl_N_TRP_8_k_8) - 1)];
        break;
    }
    // N_TRP = 7
  } else if (tdd_config == 0) {
    switch (k_TRP) {
      case 1:
        retval = srsran_sl_N_TRP_7_k_1[srsran_random_uniform_int_dist(random, 0, sizeof(srsran_sl_N_TRP_7_k_1) - 1)];
        break;
      case 2:
        retval = srsran_sl_N_TRP_7_k_2[srsran_random_uniform_int_dist(random, 0, sizeof(srsran_sl_N_TRP_7_k_2) - 1)];
        break;
      case 3:
        retval = srsran_sl_N_TRP_7_k_3[srsran_random_uniform_int_dist(random, 0, sizeof(srsran_sl_N_TRP_7_k_3) - 1)];
        break;
      case 4:
        retval = srsran_sl_N_TRP_7_k_4[srsran_random_uniform_int_dist(random, 0, sizeof(srsran_sl_N_TRP_7_k_4) - 1)];
        break;
      case 5:
        retval = srsran_sl_N_TRP_7_k_5[srsran_random_uniform_int_dist(random, 0, sizeof(srsran_sl_N_TRP_7_k_5) - 1)];
        break;
      case 6:
        retval = srsran_sl_N_TRP_7_k_6[srsran_random_uniform_int_dist(random, 0, sizeof(srsran_sl_N_TRP_7_k_6) - 1)];
        break;
      case 7:
        retval = srsran_sl_N_TRP_7_k_7[srsran_random_uniform_int_dist(random, 0, sizeof(srsran_sl_N_TRP_7_k_7) - 1)];
        break;
    }
    // N_TRP = 6
  } else if (tdd_config == 3 || tdd_config == 6) {
    switch (k_TRP) {
      case 1:
        retval = srsran_sl_N_TRP_6_k_1[srsran_random_uniform_int_dist(random, 0, sizeof(srsran_sl_N_TRP_6_k_1) - 1)];
        break;
      case 2:
        retval = srsran_sl_N_TRP_6_k_2[srsran_random_uniform_int_dist(random, 0, sizeof(srsran_sl_N_TRP_6_k_2) - 1)];
        break;
      case 3:
        retval = srsran_sl_N_TRP_6_k_3[srsran_random_uniform_int_dist(random, 0, sizeof(srsran_sl_N_TRP_6_k_3) - 1)];
        break;
      case 4:
        retval = srsran_sl_N_TRP_6_k_4[srsran_random_uniform_int_dist(random, 0, sizeof(srsran_sl_N_TRP_6_k_4) - 1)];
        break;
      case 5:
        retval = srsran_sl_N_TRP_6_k_5[srsran_random_uniform_int_dist(random, 0, sizeof(srsran_sl_N_TRP_6_k_5) - 1)];
        break;
      case 6:
        retval = srsran_sl_N_TRP_6_k_6[srsran_random_uniform_int_dist(random, 0, sizeof(srsran_sl_N_TRP_6_k_6) - 1)];
        break;
    }
  } else {
    retval = SRSRAN_SUCCESS;
  }

  srsran_random_free(random);

  return retval;
}
