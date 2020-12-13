/**
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

#include "srslte/phy/phch/ra_nr.h"
#include "srslte/phy/phch/pdsch_nr.h"
#include "srslte/phy/utils/debug.h"

typedef struct {
  srslte_mod_t modulation;
  double       R; // Target code Rate R x [1024]
  double       S; // Spectral efficiency
} mcs_entry_t;

#define RA_NR_MCS_SIZE_TABLE1 29
#define RA_NR_MCS_SIZE_TABLE2 28
#define RA_NR_MCS_SIZE_TABLE3 29
#define RA_NR_TBS_SIZE_TABLE 93

#define RA_NR_READ_TABLE(N)                                                                                            \
  static double srslte_ra_nr_R_from_mcs_table##N(uint32_t mcs_idx)                                                     \
  {                                                                                                                    \
    if (mcs_idx >= RA_NR_MCS_SIZE_TABLE##N) {                                                                          \
      return NAN;                                                                                                      \
    }                                                                                                                  \
                                                                                                                       \
    return ra_nr_table##N[mcs_idx].R;                                                                                  \
  }                                                                                                                    \
                                                                                                                       \
  static srslte_mod_t srslte_ra_nr_modulation_from_mcs_table##N(uint32_t mcs_idx)                                      \
  {                                                                                                                    \
    if (mcs_idx >= RA_NR_MCS_SIZE_TABLE##N) {                                                                          \
      return SRSLTE_MOD_NITEMS;                                                                                        \
    }                                                                                                                  \
                                                                                                                       \
    return ra_nr_table##N[mcs_idx].modulation;                                                                         \
  }

/**
 * TS 38.214 V15.10.0 Table 5.1.3.1-1: MCS index table 1 for PDSCH
 */
static const mcs_entry_t ra_nr_table1[RA_NR_MCS_SIZE_TABLE1] = {
    {SRSLTE_MOD_QPSK, 120, 0.2344},  {SRSLTE_MOD_QPSK, 157, 0.3066},  {SRSLTE_MOD_QPSK, 193, 0.3770},
    {SRSLTE_MOD_QPSK, 251, 0.4902},  {SRSLTE_MOD_QPSK, 308, 0.6016},  {SRSLTE_MOD_QPSK, 379, 0.7402},
    {SRSLTE_MOD_QPSK, 449, 0.8770},  {SRSLTE_MOD_QPSK, 526, 1.0273},  {SRSLTE_MOD_QPSK, 602, 1.1758},
    {SRSLTE_MOD_QPSK, 679, 1.3262},  {SRSLTE_MOD_16QAM, 340, 1.3281}, {SRSLTE_MOD_16QAM, 378, 1.4766},
    {SRSLTE_MOD_16QAM, 434, 1.6953}, {SRSLTE_MOD_16QAM, 490, 1.9141}, {SRSLTE_MOD_16QAM, 553, 2.1602},
    {SRSLTE_MOD_16QAM, 616, 2.4063}, {SRSLTE_MOD_16QAM, 658, 2.5703}, {SRSLTE_MOD_64QAM, 438, 2.5664},
    {SRSLTE_MOD_64QAM, 466, 2.7305}, {SRSLTE_MOD_64QAM, 517, 3.0293}, {SRSLTE_MOD_64QAM, 567, 3.3223},
    {SRSLTE_MOD_64QAM, 616, 3.6094}, {SRSLTE_MOD_64QAM, 666, 3.9023}, {SRSLTE_MOD_64QAM, 719, 4.2129},
    {SRSLTE_MOD_64QAM, 772, 4.5234}, {SRSLTE_MOD_64QAM, 822, 4.8164}, {SRSLTE_MOD_64QAM, 873, 5.1152},
    {SRSLTE_MOD_64QAM, 910, 5.3320}, {SRSLTE_MOD_64QAM, 948, 5.5547}};

/**
 * TS 38.214 V15.10.0 Table 5.1.3.1-2: MCS index table 2 for PDSCH
 */
static const mcs_entry_t ra_nr_table2[RA_NR_MCS_SIZE_TABLE2] = {
    {SRSLTE_MOD_QPSK, 120, 0.2344},   {SRSLTE_MOD_QPSK, 193, 0.3770},   {SRSLTE_MOD_QPSK, 308, 0.6016},
    {SRSLTE_MOD_QPSK, 449, 0.8770},   {SRSLTE_MOD_QPSK, 602, 1.1758},   {SRSLTE_MOD_16QAM, 378, 1.4766},
    {SRSLTE_MOD_16QAM, 434, 1.6953},  {SRSLTE_MOD_16QAM, 490, 1.9141},  {SRSLTE_MOD_16QAM, 553, 2.1602},
    {SRSLTE_MOD_16QAM, 616, 2.4063},  {SRSLTE_MOD_16QAM, 658, 2.5703},  {SRSLTE_MOD_64QAM, 466, 2.7305},
    {SRSLTE_MOD_64QAM, 517, 3.0293},  {SRSLTE_MOD_64QAM, 567, 3.3223},  {SRSLTE_MOD_64QAM, 616, 3.6094},
    {SRSLTE_MOD_64QAM, 666, 3.9023},  {SRSLTE_MOD_64QAM, 719, 4.2129},  {SRSLTE_MOD_64QAM, 772, 4.5234},
    {SRSLTE_MOD_64QAM, 822, 4.8164},  {SRSLTE_MOD_64QAM, 873, 5.1152},  {SRSLTE_MOD_256QAM, 682.5, 5.3320},
    {SRSLTE_MOD_256QAM, 711, 5.5547}, {SRSLTE_MOD_256QAM, 754, 5.8906}, {SRSLTE_MOD_256QAM, 797, 6.2266},
    {SRSLTE_MOD_256QAM, 841, 6.5703}, {SRSLTE_MOD_256QAM, 885, 6.9141}, {SRSLTE_MOD_256QAM, 916.5, 7.1602},
    {SRSLTE_MOD_256QAM, 948, 7.4063}};

/**
 * TS 38.214 V15.10.0 Table 5.1.3.1-3: MCS index table 3 for PDSCH
 */
static const mcs_entry_t ra_nr_table3[RA_NR_MCS_SIZE_TABLE3] = {
    {SRSLTE_MOD_QPSK, 30, 0.0586},   {SRSLTE_MOD_QPSK, 40, 0.0781},   {SRSLTE_MOD_QPSK, 50, 0.0977},
    {SRSLTE_MOD_QPSK, 64, 0.1250},   {SRSLTE_MOD_QPSK, 78, 0.1523},   {SRSLTE_MOD_QPSK, 99, 0.1934},
    {SRSLTE_MOD_QPSK, 120, 0.2344},  {SRSLTE_MOD_QPSK, 157, 0.3066},  {SRSLTE_MOD_QPSK, 193, 0.3770},
    {SRSLTE_MOD_QPSK, 251, 0.4902},  {SRSLTE_MOD_QPSK, 308, 0.6016},  {SRSLTE_MOD_QPSK, 379, 0.7402},
    {SRSLTE_MOD_QPSK, 449, 0.8770},  {SRSLTE_MOD_QPSK, 526, 1.0273},  {SRSLTE_MOD_QPSK, 602, 1.1758},
    {SRSLTE_MOD_16QAM, 340, 1.3281}, {SRSLTE_MOD_16QAM, 378, 1.4766}, {SRSLTE_MOD_16QAM, 434, 1.6953},
    {SRSLTE_MOD_16QAM, 490, 1.9141}, {SRSLTE_MOD_16QAM, 553, 2.1602}, {SRSLTE_MOD_16QAM, 616, 2.4063},
    {SRSLTE_MOD_64QAM, 438, 2.5664}, {SRSLTE_MOD_64QAM, 466, 2.7305}, {SRSLTE_MOD_64QAM, 517, 3.0293},
    {SRSLTE_MOD_64QAM, 567, 3.3223}, {SRSLTE_MOD_64QAM, 616, 3.6094}, {SRSLTE_MOD_64QAM, 666, 3.9023},
    {SRSLTE_MOD_64QAM, 719, 4.2129}, {SRSLTE_MOD_64QAM, 772, 4.5234}};

/**
 * Generate MCS table access functions
 */
RA_NR_READ_TABLE(1)
RA_NR_READ_TABLE(2)
RA_NR_READ_TABLE(3)

/**
 * TS 38.214 V15.10.0 Table 5.1.3.2-1: TBS for N info ≤ 3824
 */
static const uint32_t ra_nr_tbs_table[RA_NR_TBS_SIZE_TABLE] = {
    24,   32,   40,   48,   56,   64,   72,   80,   88,   96,   104,  112,  120,  128,  136,  144,  152,  160,  168,
    176,  184,  192,  208,  224,  240,  256,  272,  288,  304,  320,  336,  352,  368,  384,  408,  432,  456,  480,
    504,  528,  552,  576,  608,  640,  672,  704,  736,  768,  808,  848,  888,  928,  984,  1032, 1064, 1128, 1160,
    1192, 1224, 1256, 1288, 1320, 1352, 1416, 1480, 1544, 1608, 1672, 1736, 1800, 1864, 1928, 2024, 2088, 2152, 2216,
    2280, 2408, 2472, 2536, 2600, 2664, 2728, 2792, 2856, 2976, 3104, 3240, 3368, 3496, 3624, 3752, 3824};

typedef enum { ra_nr_table_1 = 0, ra_nr_table_2, ra_nr_table_3 } ra_nr_table_t;

static ra_nr_table_t ra_nr_select_table(srslte_mcs_table_t         mcs_table,
                                        srslte_dci_format_nr_t     dci_format,
                                        srslte_search_space_type_t search_space_type,
                                        uint16_t                   rnti)
{
  // Non-implemented parameters
  bool               sps_config_mcs_table_present = false;
  srslte_mcs_table_t sps_config_mcs_table         = srslte_mcs_table_64qam;
  bool               is_cs_rnti                   = false;
  bool               is_pdcch_sps                 = false;

  // - the higher layer parameter mcs-Table given by PDSCH-Config is set to 'qam256', and
  // - the PDSCH is scheduled by a PDCCH with DCI format 1_1 with
  // - CRC scrambled by C-RNTI
  if (mcs_table == srslte_mcs_table_256qam && dci_format == srslte_dci_format_nr_1_1 && SRSLTE_RNTI_ISUSER(rnti)) {
    return ra_nr_table_1;
  }

  // the UE is not configured with MCS-C-RNTI,
  // the higher layer parameter mcs-Table given by PDSCH-Config is set to 'qam64LowSE', and
  // the PDSCH is scheduled by a PDCCH in a UE-specific search space with
  // CRC scrambled by C - RNTI
  if (mcs_table == srslte_mcs_table_qam64LowSE && search_space_type == srslte_search_space_type_ue &&
      SRSLTE_RNTI_ISUSER(rnti)) {
    return ra_nr_table_3;
  }

  // - the UE is not configured with the higher layer parameter mcs-Table given by SPS-Config,
  // - the higher layer parameter mcs-Table given by PDSCH-Config is set to 'qam256',
  //   - if the PDSCH is scheduled by a PDCCH with DCI format 1_1 with CRC scrambled by CS-RNTI or
  //   - if the PDSCH is scheduled without corresponding PDCCH transmission using SPS-Config,
  if (!sps_config_mcs_table_present && mcs_table == srslte_mcs_table_256qam &&
      ((dci_format == srslte_dci_format_nr_1_1 && is_cs_rnti) || (!is_pdcch_sps))) {
    return ra_nr_table_2;
  }

  // - the UE is configured with the higher layer parameter mcs-Table given by SPS-Config set to 'qam64LowSE'
  //   - if the PDSCH is scheduled by a PDCCH with CRC scrambled by CS-RNTI or
  //   - if the PDSCH is scheduled without corresponding PDCCH transmission using SPS-Config,
  if (sps_config_mcs_table_present && sps_config_mcs_table == srslte_mcs_table_qam64LowSE &&
      (is_cs_rnti || is_pdcch_sps)) {
    return ra_nr_table_3;
  }

  // else
  return ra_nr_table_1;
}

double srslte_ra_nr_R_from_mcs(srslte_mcs_table_t         mcs_table,
                               srslte_dci_format_nr_t     dci_format,
                               srslte_search_space_type_t search_space_type,
                               uint16_t                   rnti,
                               uint32_t                   mcs_idx)
{
  ra_nr_table_t table = ra_nr_select_table(mcs_table, dci_format, search_space_type, rnti);

  switch (table) {
    case ra_nr_table_1:
      return srslte_ra_nr_R_from_mcs_table1(mcs_idx) / 1024.0;
    case ra_nr_table_2:
      return srslte_ra_nr_R_from_mcs_table2(mcs_idx) / 1024.0;
    case ra_nr_table_3:
      return srslte_ra_nr_R_from_mcs_table3(mcs_idx) / 1024.0;
    default:
      ERROR("Invalid table %d\n", table);
  }

  return NAN;
}

srslte_mod_t srslte_ra_nr_mod_from_mcs(srslte_mcs_table_t         mcs_table,
                                       srslte_dci_format_nr_t     dci_format,
                                       srslte_search_space_type_t search_space_type,
                                       uint16_t                   rnti,
                                       uint32_t                   mcs_idx)
{
  ra_nr_table_t table = ra_nr_select_table(mcs_table, dci_format, search_space_type, rnti);

  switch (table) {
    case ra_nr_table_1:
      return srslte_ra_nr_modulation_from_mcs_table1(mcs_idx);
    case ra_nr_table_2:
      return srslte_ra_nr_modulation_from_mcs_table2(mcs_idx);
    case ra_nr_table_3:
      return srslte_ra_nr_modulation_from_mcs_table3(mcs_idx);
    default:
      ERROR("Invalid table %d\n", table);
  }

  return SRSLTE_MOD_NITEMS;
}

int srslte_ra_dl_nr_slot_nof_re(const srslte_pdsch_cfg_nr_t* pdsch_cfg, const srslte_pdsch_grant_nr_t* grant)
{
  // the number of symbols of the PDSCH allocation within the slot
  int n_sh_symb = grant->L;

  // the number of REs for DM-RS per PRB in the scheduled duration
  int n_prb_dmrs = srslte_dmrs_pdsch_get_N_prb(pdsch_cfg, grant);
  if (n_prb_dmrs < SRSLTE_SUCCESS) {
    ERROR("Invalid number of DMRS RE\n");
    return SRSLTE_ERROR;
  }

  // the overhead configured by higher layer parameter xOverhead in PDSCH-ServingCellConfig
  uint32_t n_prb_oh = 0;
  switch (pdsch_cfg->sch_cfg.xoverhead) {
    case srslte_xoverhead_0:
      n_prb_oh = 0;
      break;
    case srslte_xoverhead_6:
      n_prb_oh = 6;
      break;
    case srslte_xoverhead_12:
      n_prb_oh = 12;
      break;
    case srslte_xoverhead_18:
      n_prb_oh = 18;
      break;
  }

  // Compute total number of n_re used for PDSCH
  uint32_t n_re_prime = SRSLTE_NRE * n_sh_symb - n_prb_dmrs - n_prb_oh;

  uint32_t n_prb = 0;
  for (uint32_t i = 0; i < SRSLTE_MAX_PRB_NR; i++) {
    n_prb += (uint32_t)grant->prb_idx[i];
  }

  // Return the number of resource elements for PDSCH
  return SRSLTE_MIN(SRSLTE_MAX_NRE_NR, n_re_prime) * n_prb;
}

#define CEIL(NUM, DEN) (((NUM) + ((DEN)-1)) / (DEN))
#define FLOOR(NUM, DEN) ((NUM) / (DEN))
#define ROUND(NUM, DEN) ((uint32_t)round((NUM) / (DEN)))
#define POW2(N) (1U << (N))

static uint32_t ra_nr_tbs_from_n_info3(uint32_t n_info)
{
  // quantized intermediate number of information bits
  uint32_t n            = (uint32_t)SRSLTE_MAX(3.0, floor(log2(n_info)) - 6.0);
  uint32_t n_info_prime = SRSLTE_MAX(ra_nr_tbs_table[0], POW2(n) * FLOOR(n_info, POW2(n)));

  // use Table 5.1.3.2-1 find the closest TBS that is not less than n_info_prime
  for (uint32_t i = 0; i < RA_NR_TBS_SIZE_TABLE; i++) {
    if (n_info_prime <= ra_nr_tbs_table[i]) {
      return ra_nr_tbs_table[i];
    }
  }

  return ra_nr_tbs_table[RA_NR_TBS_SIZE_TABLE - 1];
}

static uint32_t ra_nr_tbs_from_n_info4(uint32_t n_info, double R)
{
  // quantized intermediate number of information bits
  uint32_t n            = (uint32_t)(floor(log2(n_info - 24.0)) - 5.0);
  uint32_t n_info_prime = SRSLTE_MAX(3840, POW2(n) * ROUND(n_info - 24.0, POW2(n)));

  if (R <= 0.25) {
    uint32_t C = CEIL(n_info_prime + 24U, 3816U);
    return 8U * C * CEIL(n_info_prime + 24U, 8U * C) - 24U;
  }

  if (n_info_prime > 8424) {
    uint32_t C = CEIL(n_info_prime + 24U, 8424U);
    return 8U * C * CEIL(n_info_prime + 24U, 8U * C) - 24U;
  }

  return 8U * CEIL(n_info_prime + 24U, 8U) - 24U;
}

/**
 * @brief Implements TS 38.214 V15.10.0 Table 5.1.3.2-2 Scaling factor of N_info for P-RNTI and RA-RNTI
 * @param tb_scaling_field provided by the grant
 * @return It returns the value if the field is in range, otherwise it returns NAN
 */
static double ra_nr_get_scaling(uint32_t tb_scaling_field)
{
  static const double tb_scaling[4] = {1.0, 0.5, 0.25, NAN};

  if (tb_scaling_field < 4) {
    return tb_scaling[tb_scaling_field];
  }

  return NAN;
}

uint32_t srslte_ra_nr_tbs(uint32_t N_re, double S, double R, uint32_t Qm, uint32_t nof_layers)
{
  if (!isnormal(S)) {
    S = 1.0;
  }

  // 2) Intermediate number of information bits (N info ) is obtained by N inf o = N RE · R · Q m · υ .
  uint32_t n_info = (uint32_t)(N_re * S * R * Qm * nof_layers);

  // 3) When n_info ≤ 3824
  if (n_info <= 3824) {
    return ra_nr_tbs_from_n_info3(n_info);
  }
  // 4) When n_info > 3824
  return ra_nr_tbs_from_n_info4(n_info, R);
}

int srslte_ra_nr_fill_tb(const srslte_pdsch_cfg_nr_t*   pdsch_cfg,
                         const srslte_pdsch_grant_nr_t* grant,
                         uint32_t                       mcs_idx,
                         srslte_sch_tb_t*               tb)
{
  uint32_t cw_idx = 0;

  // Get target Rate
  double R = srslte_ra_nr_R_from_mcs(
      pdsch_cfg->sch_cfg.mcs_table, grant->dci_format, grant->dci_search_space, grant->rnti, mcs_idx);
  if (!isnormal(R)) {
    return SRSLTE_ERROR;
  }

  // Get modulation
  srslte_mod_t m = srslte_ra_nr_mod_from_mcs(
      pdsch_cfg->sch_cfg.mcs_table, grant->dci_format, grant->dci_search_space, grant->rnti, mcs_idx);
  if (m >= SRSLTE_MOD_NITEMS) {
    return SRSLTE_ERROR;
  }

  // Get modulation order
  uint32_t Qm = srslte_mod_bits_x_symbol(m);
  if (Qm == 0) {
    return SRSLTE_ERROR;
  }

  // For the PDSCH assigned by a
  // - PDCCH with DCI format 1_0 with
  // - CRC scrambled by P-RNTI, or RA-RNTI,
  double S = 1.0;
  if ((SRSLTE_RNTI_ISRAR(grant->rnti) || SRSLTE_RNTI_ISPA(grant->rnti)) &&
      grant->dci_format == srslte_dci_format_nr_1_0) {
    // where the scaling factor S is determined based on the TB scaling
    //  field in the DCI as in Table 5.1.3.2-2.
    S = ra_nr_get_scaling(grant->tb_scaling_field);

    // If the scaling is invalid return error
    if (!isnormal(S)) {
      return SRSLTE_ERROR;
    }
  }

  // 1) The UE shall first determine the number of REs (N RE ) within the slot.
  int N_re = srslte_ra_dl_nr_slot_nof_re(pdsch_cfg, grant);
  if (N_re <= SRSLTE_SUCCESS) {
    ERROR("Invalid number of RE\n");
    return SRSLTE_ERROR;
  }

  // Calculate number of layers accordingly
  uint32_t nof_cw         = grant->nof_layers < 5 ? 1 : 2;
  uint32_t nof_layers_cw1 = grant->nof_layers / nof_cw;
  uint32_t nof_layers_cw2 = grant->nof_layers - nof_layers_cw1;
  tb->N_L                 = (cw_idx == 0) ? nof_layers_cw1 : nof_layers_cw2;

  // Steps 2,3,4
  tb->tbs      = (int)srslte_ra_nr_tbs(N_re, S, R, Qm, tb->N_L);
  tb->R        = R;
  tb->mod      = m;
  tb->nof_re   = N_re * grant->nof_layers;
  tb->nof_bits = tb->nof_re * Qm;
  tb->enabled  = true;

  return SRSLTE_SUCCESS;
}