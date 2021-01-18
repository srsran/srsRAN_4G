/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srslte/phy/phch/pucch_nr.h"
#include "srslte/phy/common/phy_common_nr.h"
#include "srslte/phy/common/sequence.h"
#include "srslte/phy/common/zc_sequence.h"
#include "srslte/phy/modem/mod.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"
#include <complex.h>

// Implements TS 38.211 clause 6.3.2.2.1 Group and sequence hopping
static int pucch_nr_group_sequence(const srslte_carrier_nr_t*          carrier,
                                   const srslte_pucch_nr_common_cfg_t* cfg,
                                   uint32_t*                           u_,
                                   uint32_t*                           v_)
{
  uint32_t f_gh = 0;
  uint32_t f_ss = 0;
  uint32_t n_id = cfg->hopping_id_present ? cfg->hopping_id : carrier->id;

  switch (cfg->group_hopping) {

    case SRSLTE_PUCCH_NR_GROUP_HOPPING_NEITHER:
      f_ss = n_id % SRSLTE_ZC_SEQUENCE_NOF_GROUPS;
      break;
    case SRSLTE_PUCCH_NR_GROUP_HOPPING_ENABLE:
      ERROR("Group hopping is not implemented\n");
      return SRSLTE_ERROR;
    case SRSLTE_PUCCH_NR_GROUP_HOPPING_DISABLE:
      ERROR("Hopping is not implemented\n");
      return SRSLTE_ERROR;
  }

  uint32_t u = (f_gh + f_ss) % SRSLTE_ZC_SEQUENCE_NOF_GROUPS;
  uint32_t v = 0;

  if (u_) {
    *u_ = u;
  }

  if (v_) {
    *v_ = v;
  }

  return SRSLTE_SUCCESS;
}

// Implements TS 38.211 clause 6.3.2.2.2 Cyclic shift hopping
static uint32_t pucch_nr_alpha_idx(const srslte_carrier_nr_t*          carrier,
                                   const srslte_pucch_nr_common_cfg_t* cfg,
                                   const srslte_dl_slot_cfg_t*         slot,
                                   uint32_t                            l,
                                   uint32_t                            l_prime,
                                   uint32_t                            m0,
                                   uint32_t                            m_cs)
{
  // Compute number of slot
  uint32_t n_slot = slot->idx % SRSLTE_NSLOTS_PER_FRAME_NR(carrier->numerology);

  // Generate pseudo-random sequence
  uint32_t cinit = cfg->hopping_id_present ? cfg->hopping_id : carrier->id;
  uint8_t  cs[SRSLTE_NSYMB_PER_SLOT_NR * SRSLTE_NSLOTS_PER_FRAME_NR(SRSLTE_NR_MAX_NUMEROLOGY) * 8U] = {};
  srslte_sequence_apply_bit(
      cs, cs, SRSLTE_NSYMB_PER_SLOT_NR * SRSLTE_NSLOTS_PER_FRAME_NR(carrier->numerology) * 8, cinit);

  // Create n_cs parameter
  uint32_t n_cs = 0;
  for (uint32_t m = 0; m < 8; m++) {
    n_cs += cs[SRSLTE_NSYMB_PER_SLOT_NR * n_slot + (l + l_prime) * 8 + m] << m;
  }

  return (m0 + m_cs + n_cs) % SRSLTE_NRE;
}

static int srslte_pucch_nr_format0_resource_valid(const srslte_pucch_nr_resource_format0_t* resource)
{
  if (resource == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  if (resource->nof_symbols != 1 && resource->nof_symbols != 2) {
    ERROR("Invalid number of symbols (%d)\n", resource->nof_symbols);
    return SRSLTE_ERROR;
  }

  if (resource->initial_cyclic_shift > 11) {
    ERROR("Invalid initial cyclic shift (%d)\n", resource->initial_cyclic_shift);
    return SRSLTE_ERROR;
  }

  if (resource->start_symbol_idx > 13) {
    ERROR("Invalid initial start symbol idx (%d)\n", resource->start_symbol_idx);
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

static int srslte_pucch_nr_format1_resource_valid(const srslte_pucch_nr_resource_format1_t* resource)
{
  if (resource == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  if (resource->nof_symbols < 4 || resource->nof_symbols > 14) {
    ERROR("Invalid number of symbols (%d)\n", resource->nof_symbols);
    return SRSLTE_ERROR;
  }

  if (resource->initial_cyclic_shift > 11) {
    ERROR("Invalid initial cyclic shift (%d)\n", resource->initial_cyclic_shift);
    return SRSLTE_ERROR;
  }

  if (resource->start_symbol_idx > 10) {
    ERROR("Invalid initial start symbol idx (%d)\n", resource->start_symbol_idx);
    return SRSLTE_ERROR;
  }

  if (resource->time_domain_occ > 6) {
    ERROR("Invalid time domain occ (%d)\n", resource->time_domain_occ);
    return SRSLTE_ERROR;
  }

  if (resource->intra_slot_hopping) {
    ERROR("Intra-slot hopping is not implemented\n");
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

// TS 38.211 Table 6.3.2.4.1-2: Orthogonal sequences for PUCCH format 1
static uint32_t
    pucch_nr_format1_rho[SRSLTE_PUCCH_NR_FORMAT1_N_MAX][SRSLTE_PUCCH_NR_FORMAT1_N_MAX][SRSLTE_PUCCH_NR_FORMAT1_N_MAX] =
        {{{0}, {0, 0}, {0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0}},
         {{}, {0, 1}, {0, 1, 2}, {0, 2, 0, 2}, {0, 1, 2, 3, 4}, {0, 1, 2, 3, 4, 5}, {0, 1, 2, 3, 4, 5, 6}},
         {{}, {}, {0, 2, 1}, {0, 0, 2, 2}, {0, 2, 4, 1, 3}, {0, 2, 4, 0, 2, 4}, {0, 2, 4, 6, 1, 3, 5}},
         {{}, {}, {}, {0, 2, 2, 0}, {0, 3, 1, 4, 2}, {0, 3, 0, 3, 0, 3}, {0, 3, 6, 2, 5, 1, 4}},
         {{}, {}, {}, {}, {0, 4, 3, 2, 1}, {0, 4, 2, 0, 4, 2}, {0, 4, 1, 5, 2, 6, 3}},
         {{}, {}, {}, {}, {}, {0, 5, 4, 3, 2, 1}, {0, 5, 3, 1, 6, 4, 2}},
         {{}, {}, {}, {}, {}, {}, {0, 6, 5, 4, 3, 2, 1}}};

#define SRSLTE_PUCCH_NR_FORMAT1_MAX_NOF_BITS 2

int srslte_pucch_nr_init(srslte_pucch_nr_t* q)
{
  if (q == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  // Initialise ZC sequences for 1PRB
  float alphas_1prb[SRSLTE_NRE] = {};
  for (uint32_t i = 0; i < SRSLTE_NRE; i++) {
    alphas_1prb[i] = 2.0f * (float)M_PI * (float)i / (float)SRSLTE_NRE;
  }
  srslte_zc_sequence_lut_init_nr(&q->r_uv_1prb, 1, 0, alphas_1prb, SRSLTE_NRE);

  // Initialise BPSK modulation table
  if (srslte_modem_table_lte(&q->bpsk, SRSLTE_MOD_BPSK) < SRSLTE_SUCCESS) {
    ERROR("Initiating modem table\n");
    return SRSLTE_ERROR;
  }

  // Initialise QPSK modulation table
  if (srslte_modem_table_lte(&q->qpsk, SRSLTE_MOD_QPSK) < SRSLTE_SUCCESS) {
    ERROR("Initiating modem table\n");
    return SRSLTE_ERROR;
  }

  for (uint32_t n_pucch = 1; n_pucch <= SRSLTE_PUCCH_NR_FORMAT1_N_MAX; n_pucch++) {
    for (uint32_t i = 0; i < SRSLTE_PUCCH_NR_FORMAT1_N_MAX; i++) {
      for (uint32_t m = 0; m < SRSLTE_PUCCH_NR_FORMAT1_N_MAX; m++) {
        uint32_t rho                        = pucch_nr_format1_rho[i][n_pucch - 1][m];
        q->format1_w_i_m[i][n_pucch - 1][m] = cexpf(I * 2.0f * (float)M_PI * (float)rho / n_pucch);
      }
    }
  }

  return SRSLTE_SUCCESS;
}

void srslte_pucch_nr_free(srslte_pucch_nr_t* q)
{
  if (q == NULL) {
    return;
  }

  srslte_zc_sequence_lut_free(&q->r_uv_1prb);

  srslte_modem_table_free(&q->bpsk);
  srslte_modem_table_free(&q->qpsk);

  SRSLTE_MEM_ZERO(q, srslte_pucch_nr_t, 1);
}

int srslte_pucch_nr_format0_put(const srslte_pucch_nr_t*            q,
                                const srslte_carrier_nr_t*          carrier,
                                const srslte_pucch_nr_common_cfg_t* cfg,
                                const srslte_dl_slot_cfg_t*         slot,
                                srslte_pucch_nr_resource_format0_t* resource,
                                uint32_t                            m_cs,
                                cf_t*                               slot_symbols)
{
  if (carrier == NULL || cfg == NULL || slot == NULL || resource == NULL || slot_symbols == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  if (srslte_pucch_nr_format0_resource_valid(resource) < SRSLTE_SUCCESS) {
    ERROR("Invalid PUCCH format 0 resource\n");
    return SRSLTE_SUCCESS;
  }

  uint32_t u = 0;
  uint32_t v = 0;
  if (pucch_nr_group_sequence(carrier, cfg, &u, &v) < SRSLTE_SUCCESS) {
    ERROR("Error getting group sequence\n");
    return SRSLTE_ERROR;
  }

  uint32_t l_prime = resource->start_symbol_idx;
  for (uint32_t l = 0; l < resource->nof_symbols; l++) {
    // Get Alpha index
    uint32_t alpha_idx = pucch_nr_alpha_idx(carrier, cfg, slot, l, l_prime, resource->initial_cyclic_shift, m_cs);

    // get r_uv sequence from LUT object
    const cf_t* r_uv = srslte_zc_sequence_lut_get(&q->r_uv_1prb, u, v, alpha_idx);
    if (r_uv == NULL) {
      ERROR("Getting r_uv sequence\n");
      return SRSLTE_ERROR;
    }

    // Get start of the sequence in resource grid
    cf_t* slot_symbols_ptr = &slot_symbols[(carrier->nof_prb * (l + l_prime) + resource->starting_prb) * SRSLTE_NRE];

    // Copy sequence in grid
    srslte_vec_cf_copy(slot_symbols_ptr, r_uv, SRSLTE_NRE);
  }

  return SRSLTE_SUCCESS;
}

int srslte_pucch_nr_format0_measure(const srslte_pucch_nr_t*            q,
                                    const srslte_carrier_nr_t*          carrier,
                                    const srslte_pucch_nr_common_cfg_t* cfg,
                                    const srslte_dl_slot_cfg_t*         slot,
                                    srslte_pucch_nr_resource_format0_t* resource,
                                    uint32_t                            m_cs,
                                    const cf_t*                         slot_symbols,
                                    srslte_pucch_nr_measure_t*          measure)
{
  if (carrier == NULL || cfg == NULL || slot == NULL || resource == NULL || slot_symbols == NULL || measure == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  if (srslte_pucch_nr_format0_resource_valid(resource) < SRSLTE_SUCCESS) {
    ERROR("Invalid PUCCH format 0 resource\n");
    return SRSLTE_SUCCESS;
  }

  uint32_t u = 0;
  uint32_t v = 0;
  if (pucch_nr_group_sequence(carrier, cfg, &u, &v) < SRSLTE_SUCCESS) {
    ERROR("Error getting group sequence\n");
    return SRSLTE_ERROR;
  }

  uint32_t l_prime = resource->start_symbol_idx;
  float    epre    = 0.0f;
  float    rsrp    = 0.0f;
  for (uint32_t l = 0; l < resource->nof_symbols; l++) {
    // Get Alpha index
    uint32_t alpha_idx = pucch_nr_alpha_idx(carrier, cfg, slot, l, l_prime, resource->initial_cyclic_shift, m_cs);

    // get r_uv sequence from LUT object
    const cf_t* r_uv = srslte_zc_sequence_lut_get(&q->r_uv_1prb, u, v, alpha_idx);
    if (r_uv == NULL) {
      ERROR("Getting r_uv sequence\n");
      return SRSLTE_ERROR;
    }

    // Get start of the sequence in resource grid
    const cf_t* slot_symbols_ptr =
        &slot_symbols[(carrier->nof_prb * (l + l_prime) + resource->starting_prb) * SRSLTE_NRE];

    // Measure EPRE and average
    epre += srslte_vec_avg_power_cf(slot_symbols_ptr, SRSLTE_NRE) / resource->nof_symbols;

    // Do correlation
    cf_t corr = srslte_vec_dot_prod_conj_ccc(r_uv, slot_symbols_ptr, SRSLTE_NRE);
    corr /= SRSLTE_NRE;

    // Measure RSRP and average
    rsrp += (__real__ corr * __real__ corr + __imag__ corr * __imag__ corr) / resource->nof_symbols;
  }

  // Save measurement
  measure->rsrp      = rsrp;
  measure->rsrp_dBfs = srslte_convert_power_to_dB(rsrp);
  measure->epre      = epre;
  measure->epre_dBfs = srslte_convert_power_to_dB(epre);
  if (isnormal(epre)) {
    measure->norm_corr = rsrp / epre;
  } else {
    measure->norm_corr = 0.0f;
  }

  return SRSLTE_SUCCESS;
}

// Implements TS 38.211 table 6.3.2.4.1-1 Number of PUCCH symbols and the corresponding N_PUC...
uint32_t pucch_nr_format1_n_pucch(const srslte_pucch_nr_resource_format1_t* resource, uint32_t m_prime)
{
  if (resource->intra_slot_hopping) {
    if (m_prime == 0) {
      return resource->nof_symbols / 4;
    }

    return resource->nof_symbols / 2 - resource->nof_symbols / 4;
  }

  if (m_prime == 1) {
    return 0;
  }

  return resource->nof_symbols / 2;
}

static cf_t pucch_nr_format1_w(const srslte_pucch_nr_t* q, uint32_t n_pucch, uint32_t i, uint32_t m)
{
  if (n_pucch < 1 || n_pucch > SRSLTE_PUCCH_NR_FORMAT1_N_MAX) {
    ERROR("Invalid n_pucch\n");
    return NAN;
  }
  if (i >= SRSLTE_PUCCH_NR_FORMAT1_N_MAX) {
    ERROR("Invalid i\n");
    return NAN;
  }
  if (m >= SRSLTE_PUCCH_NR_FORMAT1_N_MAX) {
    ERROR("Invalid m\n");
    return NAN;
  }

  // Get value
  return q->format1_w_i_m[i][n_pucch - 1][m];
}

int srslte_pucch_nr_put_format1(srslte_pucch_nr_t*                        q,
                                const srslte_carrier_nr_t*                carrier,
                                const srslte_pucch_nr_common_cfg_t*       cfg,
                                const srslte_dl_slot_cfg_t*               slot,
                                const srslte_pucch_nr_resource_format1_t* resource,
                                uint8_t                                   b[SRSLTE_PUCCH_NR_FORMAT1_MAX_NOF_BITS],
                                uint32_t                                  nof_bits,
                                cf_t*                                     slot_symbols)
{
  uint32_t m_cs = 0;

  if (carrier == NULL || cfg == NULL || slot == NULL || resource == NULL || b == NULL || slot_symbols == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  if (srslte_pucch_nr_format1_resource_valid(resource) < SRSLTE_SUCCESS) {
    ERROR("Invalid PUCCH format 1 resource\n");
    return SRSLTE_SUCCESS;
  }

  if (nof_bits > 2) {
    ERROR("Invalid number of bits (%d)\n", nof_bits);
    return SRSLTE_ERROR;
  }

  // Modulate d
  cf_t d = 0;
  if (nof_bits == 1) {
    srslte_mod_modulate(&q->bpsk, b, &d, nof_bits);
  } else {
    srslte_mod_modulate(&q->qpsk, b, &d, nof_bits);
  }

  // Get group sequence
  uint32_t u = 0;
  uint32_t v = 0;
  if (pucch_nr_group_sequence(carrier, cfg, &u, &v) < SRSLTE_SUCCESS) {
    ERROR("Error getting group sequence\n");
    return SRSLTE_ERROR;
  }

  // Calculate number of symbols carrying PUCCH (No DMRS)
  uint32_t n_pucch = pucch_nr_format1_n_pucch(resource, 0);

  uint32_t l_prime = resource->start_symbol_idx;
  for (uint32_t l = 1, m = 0; l < resource->nof_symbols; l += 2, m++) {
    // Get start of the sequence in resource grid
    cf_t* slot_symbols_ptr = &slot_symbols[(carrier->nof_prb * (l + l_prime) + resource->starting_prb) * SRSLTE_NRE];

    // Get Alpha index
    uint32_t alpha_idx = pucch_nr_alpha_idx(carrier, cfg, slot, l, l_prime, resource->initial_cyclic_shift, m_cs);

    // get r_uv sequence from LUT object
    const cf_t* r_uv = srslte_zc_sequence_lut_get(&q->r_uv_1prb, u, v, alpha_idx);
    if (r_uv == NULL) {
      ERROR("Getting r_uv sequence\n");
      return SRSLTE_ERROR;
    }

    // Compute y = d(0) * r_uv
    cf_t y[SRSLTE_NRE];
    srslte_vec_sc_prod_ccc(r_uv, d, y, SRSLTE_NRE);

    // Get w_i_m
    cf_t w_i_m = pucch_nr_format1_w(q, n_pucch, resource->time_domain_occ, m);

    // Compute z(n) = w(i) * y(n)
    cf_t z[SRSLTE_NRE];
    srslte_vec_sc_prod_ccc(y, w_i_m, z, SRSLTE_NRE);

    // Put z in the grid
    srslte_vec_cf_copy(slot_symbols_ptr, z, SRSLTE_NRE);
  }

  return SRSLTE_SUCCESS;
}