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

#include <sys/time.h>

#include "srslte/phy/common/phy_common_sl.h"
#include "srslte/phy/phch/ra.h"
#include "srslte/phy/phch/ra_sl.h"
#include "srslte/phy/utils/random.h"

int srslte_sl_get_available_pool_prb(uint32_t prb_num, uint32_t prb_start, uint32_t prb_end)
{
  if ((prb_num * 2) <= (prb_end - prb_start + 1)) {
    return prb_num * 2;
  } else {
    return prb_num * 2 - 1;
  }
}

int srslte_pscch_resources(uint32_t  prb_num,
                           uint32_t  prb_start,
                           uint32_t  prb_end,
                           uint8_t*  pscch_sf_bitmap,
                           uint32_t  period_length,
                           uint32_t  n_pscch,
                           uint32_t* m_a,
                           uint32_t* l_b)
{
  uint8_t  M = srslte_sl_get_available_pool_prb(prb_num, prb_start, prb_end);
  uint32_t L = 0;
  for (uint32_t i = 0; i < period_length; i++) {
    if (pscch_sf_bitmap[i] == 1) {
      L++;
    }
  }

  if (L == 0) {
    // ERROR("Invalid PSCCH subframe bitmap");
    return SRSLTE_ERROR;
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

  return SRSLTE_SUCCESS;
}

// 3GPP TS 36.213 Section 8.1.1. Uplink resource allocation type 0
uint32_t srslte_ra_sl_type0_to_riv(uint32_t nof_prb, uint32_t prb_start, uint32_t L_crb)
{
  return srslte_ra_type2_to_riv(L_crb, prb_start, nof_prb);
}

// 3GPP TS 36.213 Section 8.1.1. Uplink resource allocation type 0
void srslte_ra_sl_type0_from_riv(uint32_t riv, uint32_t nof_prb, uint32_t* L_crb, uint32_t* prb_start)
{
  srslte_ra_type2_from_riv(riv, L_crb, prb_start, nof_prb, nof_prb);
}

int srslte_ra_sl_pssch_allowed_sf(uint32_t pssch_sf_idx, uint32_t trp_idx, uint32_t duplex_mode, uint32_t tdd_config)
{
  if (duplex_mode == SRSLTE_SL_DUPLEX_MODE_FDD) {
    return srslte_sl_N_TRP_8[trp_idx][pssch_sf_idx % 8];
  }
  switch (tdd_config) {
    // N_TRP = 6
    case 3:
    case 6:
      return srslte_sl_N_TRP_6[trp_idx][pssch_sf_idx % 6];
    // N_TRP = 7
    case 0:
      return srslte_sl_N_TRP_7[trp_idx][pssch_sf_idx % 7];
    // N_TRP = 8
    case 1:
    case 2:
    case 4:
    case 5:
      return srslte_sl_N_TRP_8[trp_idx][pssch_sf_idx % 8];
    default:
      return 0;
  }
}

int srslte_sci_generate_trp_idx(uint32_t duplex_mode, uint32_t tdd_config, uint32_t k_TRP)
{
  int retval = SRSLTE_ERROR;
  struct timeval tv;
  gettimeofday(&tv, NULL);
  srslte_random_t random = srslte_random_init(tv.tv_usec);

  // N_TRP = 8
  if (duplex_mode == SRSLTE_SL_DUPLEX_MODE_FDD || tdd_config == 1 || tdd_config == 2 || tdd_config == 4 ||
      tdd_config == 5) {
    switch (k_TRP) {
      case 1:
        retval = srslte_sl_N_TRP_8_k_1[srslte_random_uniform_int_dist(random, 0, sizeof(srslte_sl_N_TRP_8_k_1) - 1)];
        break;
      case 2:
        retval = srslte_sl_N_TRP_8_k_2[srslte_random_uniform_int_dist(random, 0, sizeof(srslte_sl_N_TRP_8_k_2) - 1)];
        break;
      case 4:
        retval = srslte_sl_N_TRP_8_k_4[srslte_random_uniform_int_dist(random, 0, sizeof(srslte_sl_N_TRP_8_k_4) - 1)];
        break;
      case 8:
        retval = srslte_sl_N_TRP_8_k_8[srslte_random_uniform_int_dist(random, 0, sizeof(srslte_sl_N_TRP_8_k_8) - 1)];
        break;
    }
    // N_TRP = 7
  } else if (tdd_config == 0) {
    switch (k_TRP) {
      case 1:
        retval = srslte_sl_N_TRP_7_k_1[srslte_random_uniform_int_dist(random, 0, sizeof(srslte_sl_N_TRP_7_k_1) - 1)];
        break;
      case 2:
        retval = srslte_sl_N_TRP_7_k_2[srslte_random_uniform_int_dist(random, 0, sizeof(srslte_sl_N_TRP_7_k_2) - 1)];
        break;
      case 3:
        retval = srslte_sl_N_TRP_7_k_3[srslte_random_uniform_int_dist(random, 0, sizeof(srslte_sl_N_TRP_7_k_3) - 1)];
        break;
      case 4:
        retval = srslte_sl_N_TRP_7_k_4[srslte_random_uniform_int_dist(random, 0, sizeof(srslte_sl_N_TRP_7_k_4) - 1)];
        break;
      case 5:
        retval = srslte_sl_N_TRP_7_k_5[srslte_random_uniform_int_dist(random, 0, sizeof(srslte_sl_N_TRP_7_k_5) - 1)];
        break;
      case 6:
        retval = srslte_sl_N_TRP_7_k_6[srslte_random_uniform_int_dist(random, 0, sizeof(srslte_sl_N_TRP_7_k_6) - 1)];
        break;
      case 7:
        retval = srslte_sl_N_TRP_7_k_7[srslte_random_uniform_int_dist(random, 0, sizeof(srslte_sl_N_TRP_7_k_7) - 1)];
        break;
    }
    // N_TRP = 6
  } else if (tdd_config == 3 || tdd_config == 6) {
    switch (k_TRP) {
      case 1:
        retval = srslte_sl_N_TRP_6_k_1[srslte_random_uniform_int_dist(random, 0, sizeof(srslte_sl_N_TRP_6_k_1) - 1)];
        break;
      case 2:
        retval = srslte_sl_N_TRP_6_k_2[srslte_random_uniform_int_dist(random, 0, sizeof(srslte_sl_N_TRP_6_k_2) - 1)];
        break;
      case 3:
        retval = srslte_sl_N_TRP_6_k_3[srslte_random_uniform_int_dist(random, 0, sizeof(srslte_sl_N_TRP_6_k_3) - 1)];
        break;
      case 4:
        retval = srslte_sl_N_TRP_6_k_4[srslte_random_uniform_int_dist(random, 0, sizeof(srslte_sl_N_TRP_6_k_4) - 1)];
        break;
      case 5:
        retval = srslte_sl_N_TRP_6_k_5[srslte_random_uniform_int_dist(random, 0, sizeof(srslte_sl_N_TRP_6_k_5) - 1)];
        break;
      case 6:
        retval = srslte_sl_N_TRP_6_k_6[srslte_random_uniform_int_dist(random, 0, sizeof(srslte_sl_N_TRP_6_k_6) - 1)];
        break;
    }
  }
  else {
    retval = SRSLTE_SUCCESS;
  }

  srslte_random_free(random);

  return retval;
}
