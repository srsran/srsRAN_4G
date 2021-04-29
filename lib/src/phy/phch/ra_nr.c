/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#include "srsran/phy/phch/ra_nr.h"
#include "srsran/phy/ch_estimation/csi_rs.h"
#include "srsran/phy/phch/csi.h"
#include "srsran/phy/phch/pdsch_nr.h"
#include "srsran/phy/phch/ra_dl_nr.h"
#include "srsran/phy/phch/ra_ul_nr.h"
#include "srsran/phy/phch/uci_nr.h"
#include "srsran/phy/utils/debug.h"

typedef struct {
  srsran_mod_t modulation;
  double       R; // Target code Rate R x [1024]
  double       S; // Spectral efficiency
} mcs_entry_t;

#define RA_NR_MCS_SIZE_TABLE1 29
#define RA_NR_MCS_SIZE_TABLE2 28
#define RA_NR_MCS_SIZE_TABLE3 29
#define RA_NR_TBS_SIZE_TABLE 93
#define RA_NR_BETA_OFFSET_HARQACK_SIZE 32
#define RA_NR_BETA_OFFSET_CSI_SIZE 32

#define RA_NR_READ_TABLE(N)                                                                                            \
  static double srsran_ra_nr_R_from_mcs_table##N(uint32_t mcs_idx)                                                     \
  {                                                                                                                    \
    if (mcs_idx >= RA_NR_MCS_SIZE_TABLE##N) {                                                                          \
      return NAN;                                                                                                      \
    }                                                                                                                  \
                                                                                                                       \
    return ra_nr_table##N[mcs_idx].R;                                                                                  \
  }                                                                                                                    \
                                                                                                                       \
  static srsran_mod_t srsran_ra_nr_modulation_from_mcs_table##N(uint32_t mcs_idx)                                      \
  {                                                                                                                    \
    if (mcs_idx >= RA_NR_MCS_SIZE_TABLE##N) {                                                                          \
      return SRSRAN_MOD_NITEMS;                                                                                        \
    }                                                                                                                  \
                                                                                                                       \
    return ra_nr_table##N[mcs_idx].modulation;                                                                         \
  }

/**
 * TS 38.214 V15.10.0 Table 5.1.3.1-1: MCS index table 1 for PDSCH
 */
static const mcs_entry_t ra_nr_table1[RA_NR_MCS_SIZE_TABLE1] = {
    {SRSRAN_MOD_QPSK, 120, 0.2344},  {SRSRAN_MOD_QPSK, 157, 0.3066},  {SRSRAN_MOD_QPSK, 193, 0.3770},
    {SRSRAN_MOD_QPSK, 251, 0.4902},  {SRSRAN_MOD_QPSK, 308, 0.6016},  {SRSRAN_MOD_QPSK, 379, 0.7402},
    {SRSRAN_MOD_QPSK, 449, 0.8770},  {SRSRAN_MOD_QPSK, 526, 1.0273},  {SRSRAN_MOD_QPSK, 602, 1.1758},
    {SRSRAN_MOD_QPSK, 679, 1.3262},  {SRSRAN_MOD_16QAM, 340, 1.3281}, {SRSRAN_MOD_16QAM, 378, 1.4766},
    {SRSRAN_MOD_16QAM, 434, 1.6953}, {SRSRAN_MOD_16QAM, 490, 1.9141}, {SRSRAN_MOD_16QAM, 553, 2.1602},
    {SRSRAN_MOD_16QAM, 616, 2.4063}, {SRSRAN_MOD_16QAM, 658, 2.5703}, {SRSRAN_MOD_64QAM, 438, 2.5664},
    {SRSRAN_MOD_64QAM, 466, 2.7305}, {SRSRAN_MOD_64QAM, 517, 3.0293}, {SRSRAN_MOD_64QAM, 567, 3.3223},
    {SRSRAN_MOD_64QAM, 616, 3.6094}, {SRSRAN_MOD_64QAM, 666, 3.9023}, {SRSRAN_MOD_64QAM, 719, 4.2129},
    {SRSRAN_MOD_64QAM, 772, 4.5234}, {SRSRAN_MOD_64QAM, 822, 4.8164}, {SRSRAN_MOD_64QAM, 873, 5.1152},
    {SRSRAN_MOD_64QAM, 910, 5.3320}, {SRSRAN_MOD_64QAM, 948, 5.5547}};

/**
 * TS 38.214 V15.10.0 Table 5.1.3.1-2: MCS index table 2 for PDSCH
 */
static const mcs_entry_t ra_nr_table2[RA_NR_MCS_SIZE_TABLE2] = {
    {SRSRAN_MOD_QPSK, 120, 0.2344},   {SRSRAN_MOD_QPSK, 193, 0.3770},   {SRSRAN_MOD_QPSK, 308, 0.6016},
    {SRSRAN_MOD_QPSK, 449, 0.8770},   {SRSRAN_MOD_QPSK, 602, 1.1758},   {SRSRAN_MOD_16QAM, 378, 1.4766},
    {SRSRAN_MOD_16QAM, 434, 1.6953},  {SRSRAN_MOD_16QAM, 490, 1.9141},  {SRSRAN_MOD_16QAM, 553, 2.1602},
    {SRSRAN_MOD_16QAM, 616, 2.4063},  {SRSRAN_MOD_16QAM, 658, 2.5703},  {SRSRAN_MOD_64QAM, 466, 2.7305},
    {SRSRAN_MOD_64QAM, 517, 3.0293},  {SRSRAN_MOD_64QAM, 567, 3.3223},  {SRSRAN_MOD_64QAM, 616, 3.6094},
    {SRSRAN_MOD_64QAM, 666, 3.9023},  {SRSRAN_MOD_64QAM, 719, 4.2129},  {SRSRAN_MOD_64QAM, 772, 4.5234},
    {SRSRAN_MOD_64QAM, 822, 4.8164},  {SRSRAN_MOD_64QAM, 873, 5.1152},  {SRSRAN_MOD_256QAM, 682.5, 5.3320},
    {SRSRAN_MOD_256QAM, 711, 5.5547}, {SRSRAN_MOD_256QAM, 754, 5.8906}, {SRSRAN_MOD_256QAM, 797, 6.2266},
    {SRSRAN_MOD_256QAM, 841, 6.5703}, {SRSRAN_MOD_256QAM, 885, 6.9141}, {SRSRAN_MOD_256QAM, 916.5, 7.1602},
    {SRSRAN_MOD_256QAM, 948, 7.4063}};

/**
 * TS 38.214 V15.10.0 Table 5.1.3.1-3: MCS index table 3 for PDSCH
 */
static const mcs_entry_t ra_nr_table3[RA_NR_MCS_SIZE_TABLE3] = {
    {SRSRAN_MOD_QPSK, 30, 0.0586},   {SRSRAN_MOD_QPSK, 40, 0.0781},   {SRSRAN_MOD_QPSK, 50, 0.0977},
    {SRSRAN_MOD_QPSK, 64, 0.1250},   {SRSRAN_MOD_QPSK, 78, 0.1523},   {SRSRAN_MOD_QPSK, 99, 0.1934},
    {SRSRAN_MOD_QPSK, 120, 0.2344},  {SRSRAN_MOD_QPSK, 157, 0.3066},  {SRSRAN_MOD_QPSK, 193, 0.3770},
    {SRSRAN_MOD_QPSK, 251, 0.4902},  {SRSRAN_MOD_QPSK, 308, 0.6016},  {SRSRAN_MOD_QPSK, 379, 0.7402},
    {SRSRAN_MOD_QPSK, 449, 0.8770},  {SRSRAN_MOD_QPSK, 526, 1.0273},  {SRSRAN_MOD_QPSK, 602, 1.1758},
    {SRSRAN_MOD_16QAM, 340, 1.3281}, {SRSRAN_MOD_16QAM, 378, 1.4766}, {SRSRAN_MOD_16QAM, 434, 1.6953},
    {SRSRAN_MOD_16QAM, 490, 1.9141}, {SRSRAN_MOD_16QAM, 553, 2.1602}, {SRSRAN_MOD_16QAM, 616, 2.4063},
    {SRSRAN_MOD_64QAM, 438, 2.5664}, {SRSRAN_MOD_64QAM, 466, 2.7305}, {SRSRAN_MOD_64QAM, 517, 3.0293},
    {SRSRAN_MOD_64QAM, 567, 3.3223}, {SRSRAN_MOD_64QAM, 616, 3.6094}, {SRSRAN_MOD_64QAM, 666, 3.9023},
    {SRSRAN_MOD_64QAM, 719, 4.2129}, {SRSRAN_MOD_64QAM, 772, 4.5234}};

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

/**
 * TS 38.213 V15.10.0 Table 9.3-1: Mapping of beta_offset values for HARQ-ACK information and the index signalled by
 * higher layers
 */
static const float ra_nr_beta_offset_ack_table[RA_NR_BETA_OFFSET_HARQACK_SIZE] = {
    1.000f,  2.000f,  2.500f,  3.125f,  4.000f,   5.000f, 6.250f, 8.000f, 10.000f, 12.625f, 15.875f,
    20.000f, 31.000f, 50.000f, 80.000f, 126.000f, NAN,    NAN,    NAN,    NAN,     NAN,     NAN,
    NAN,     NAN,     NAN,     NAN,     NAN,      NAN,    NAN,    NAN,    NAN,     NAN};

/**
 * TS 38.213 V15.10.0 Table 9.3-2: Mapping of beta_offset values for CSI and the index signalled by higher layers
 */
static const float ra_nr_beta_offset_csi_table[RA_NR_BETA_OFFSET_CSI_SIZE] = {
    1.125f, 1.250f, 1.375f, 1.625f, 1.750f,  2.000f,  2.250f,  2.500f,  2.875f, 3.125f, 3.500f,
    4.000f, 5.000f, 6.250f, 8.000f, 10.000f, 12.625f, 15.875f, 20.000f, NAN,    NAN,    NAN,
    NAN,    NAN,    NAN,    NAN,    NAN,     NAN,     NAN,     NAN,     NAN,    NAN};

typedef enum { ra_nr_table_1 = 0, ra_nr_table_2, ra_nr_table_3 } ra_nr_table_t;

static ra_nr_table_t ra_nr_select_table_pusch_noprecoding(srsran_mcs_table_t         mcs_table,
                                                          srsran_dci_format_nr_t     dci_format,
                                                          srsran_search_space_type_t search_space_type,
                                                          srsran_rnti_type_t         rnti_type)
{
  // Non-implemented parameters
  bool mcs_c_rnti = false;

  // - if mcs-Table in pusch-Config is set to 'qam256', and
  // - PUSCH is scheduled by a PDCCH with DCI format 0_1 with
  // - CRC scrambled by C-RNTI or SP-CSI-RNTI,
  if (mcs_table == srsran_mcs_table_256qam && dci_format == srsran_dci_format_nr_0_1 &&
      (rnti_type == srsran_rnti_type_c || rnti_type == srsran_rnti_type_sp_csi)) {
    return ra_nr_table_2;
  }

  // - the UE is not configured with MCS-C-RNTI,
  // - mcs-Table in pusch-Config is set to 'qam64LowSE', and the
  // - PUSCH is scheduled by a PDCCH in a UE-specific search space with
  // - CRC scrambled by C-RNTI or SP-CSI-RNTI,
  if (!mcs_c_rnti && mcs_table == srsran_mcs_table_qam64LowSE && dci_format != srsran_dci_format_nr_rar &&
      search_space_type == srsran_search_space_type_ue &&
      (rnti_type == srsran_rnti_type_c || rnti_type == srsran_rnti_type_sp_csi)) {
    return ra_nr_table_3;
  }

  // - the UE is configured with MCS-C-RNTI, and
  // - the PUSCH is scheduled by a PDCCH with
  // - CRC scrambled by MCS-C-RNTI,
  //  if (mcs_c_rnti && dci_format != srsran_dci_format_nr_rar && rnti_type == srsran_rnti_type_mcs_c) {
  //    return ra_nr_table_3;
  //  }

  // - mcs-Table in configuredGrantConfig is set to 'qam256',
  //   - if PUSCH is scheduled by a PDCCH with CRC scrambled by CS-RNTI or
  //   - if PUSCH is transmitted with configured grant
  //  if (configured_grant_table == srsran_mcs_table_256qam &&
  //      (rnti_type == srsran_rnti_type_cs || dci_format == srsran_dci_format_nr_cg)) {
  //    return ra_nr_table_2;
  //  }

  // - mcs-Table in configuredGrantConfig is set to 'qam64LowSE'
  //   - if PUSCH is scheduled by a PDCCH with CRC scrambled by CS-RNTI or
  //   - if PUSCH is transmitted with configured grant,
  //  if (configured_grant_table == srsran_mcs_table_qam64LowSE &&
  //      (rnti_type == srsran_rnti_type_cs || dci_format == srsran_dci_format_nr_cg)) {
  //    return ra_nr_table_3;
  //  }

  return ra_nr_table_1;
}

static ra_nr_table_t ra_nr_select_table_pdsch(srsran_mcs_table_t         mcs_table,
                                              srsran_dci_format_nr_t     dci_format,
                                              srsran_search_space_type_t search_space_type,
                                              srsran_rnti_type_t         rnti_type)
{
  // Non-implemented parameters
  bool sps_config_mcs_table_present = false;
  bool is_pdcch_sps                 = false;

  // - the higher layer parameter mcs-Table given by PDSCH-Config is set to 'qam256', and
  // - the PDSCH is scheduled by a PDCCH with DCI format 1_1 with
  // - CRC scrambled by C-RNTI
  if (mcs_table == srsran_mcs_table_256qam && dci_format == srsran_dci_format_nr_1_1 &&
      rnti_type == srsran_rnti_type_c) {
    return ra_nr_table_1;
  }

  // the UE is not configured with MCS-C-RNTI,
  // the higher layer parameter mcs-Table given by PDSCH-Config is set to 'qam64LowSE', and
  // the PDSCH is scheduled by a PDCCH in a UE-specific search space with
  // CRC scrambled by C - RNTI
  if (mcs_table == srsran_mcs_table_qam64LowSE && search_space_type == srsran_search_space_type_ue &&
      rnti_type == srsran_rnti_type_c) {
    return ra_nr_table_3;
  }

  // - the UE is not configured with the higher layer parameter mcs-Table given by SPS-Config,
  // - the higher layer parameter mcs-Table given by PDSCH-Config is set to 'qam256',
  //   - if the PDSCH is scheduled by a PDCCH with DCI format 1_1 with CRC scrambled by CS-RNTI or
  //   - if the PDSCH is scheduled without corresponding PDCCH transmission using SPS-Config,
  if (!sps_config_mcs_table_present && mcs_table == srsran_mcs_table_256qam &&
      ((dci_format == srsran_dci_format_nr_1_1 && rnti_type == srsran_rnti_type_c) || (!is_pdcch_sps))) {
    return ra_nr_table_2;
  }

  // - the UE is configured with the higher layer parameter mcs-Table given by SPS-Config set to 'qam64LowSE'
  //   - if the PDSCH is scheduled by a PDCCH with CRC scrambled by CS-RNTI or
  //   - if the PDSCH is scheduled without corresponding PDCCH transmission using SPS-Config,
  //  if (sps_config_mcs_table_present && sps_config_mcs_table == srsran_mcs_table_qam64LowSE &&
  //      (rnti_type == srsran_rnti_type_cs || is_pdcch_sps)) {
  //    return ra_nr_table_3;
  //  }

  // else
  return ra_nr_table_1;
}

static ra_nr_table_t ra_nr_select_table(srsran_mcs_table_t         mcs_table,
                                        srsran_dci_format_nr_t     dci_format,
                                        srsran_search_space_type_t search_space_type,
                                        srsran_rnti_type_t         rnti_type)
{
  // Check if it is a PUSCH transmission
  if (dci_format == srsran_dci_format_nr_0_0 || dci_format == srsran_dci_format_nr_0_1 ||
      dci_format == srsran_dci_format_nr_rar || dci_format == srsran_dci_format_nr_cg) {
    return ra_nr_select_table_pusch_noprecoding(mcs_table, dci_format, search_space_type, rnti_type);
  }

  return ra_nr_select_table_pdsch(mcs_table, dci_format, search_space_type, rnti_type);
}

static int ra_nr_dmrs_power_offset(srsran_sch_grant_nr_t* grant)
{
  if (grant == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Defined by TS 38.214 V15.10.0:
  // - Table 4.1-1: The ratio of PDSCH EPRE to DM-RS EPRE
  // - Table 6.2.2-1: The ratio of PUSCH EPRE to DM-RS EPRE
  float ratio_dB[3] = {0, -3, -4.77};

  if (grant->nof_dmrs_cdm_groups_without_data < 1 || grant->nof_dmrs_cdm_groups_without_data > 3) {
    ERROR("Invalid number of DMRS CDM groups without data (%d)", grant->nof_dmrs_cdm_groups_without_data);
    return SRSRAN_ERROR;
  }

  grant->beta_dmrs = srsran_convert_dB_to_amplitude(-ratio_dB[grant->nof_dmrs_cdm_groups_without_data - 1]);

  return SRSRAN_SUCCESS;
}

double srsran_ra_nr_R_from_mcs(srsran_mcs_table_t         mcs_table,
                               srsran_dci_format_nr_t     dci_format,
                               srsran_search_space_type_t search_space_type,
                               srsran_rnti_type_t         rnti_type,
                               uint32_t                   mcs_idx)
{
  ra_nr_table_t table = ra_nr_select_table(mcs_table, dci_format, search_space_type, rnti_type);

  switch (table) {
    case ra_nr_table_1:
      return srsran_ra_nr_R_from_mcs_table1(mcs_idx) / 1024.0;
    case ra_nr_table_2:
      return srsran_ra_nr_R_from_mcs_table2(mcs_idx) / 1024.0;
    case ra_nr_table_3:
      return srsran_ra_nr_R_from_mcs_table3(mcs_idx) / 1024.0;
    default:
      ERROR("Invalid table %d", table);
  }

  return NAN;
}

srsran_mod_t srsran_ra_nr_mod_from_mcs(srsran_mcs_table_t         mcs_table,
                                       srsran_dci_format_nr_t     dci_format,
                                       srsran_search_space_type_t search_space_type,
                                       srsran_rnti_type_t         rnti_type,
                                       uint32_t                   mcs_idx)
{
  ra_nr_table_t table = ra_nr_select_table(mcs_table, dci_format, search_space_type, rnti_type);

  switch (table) {
    case ra_nr_table_1:
      return srsran_ra_nr_modulation_from_mcs_table1(mcs_idx);
    case ra_nr_table_2:
      return srsran_ra_nr_modulation_from_mcs_table2(mcs_idx);
    case ra_nr_table_3:
      return srsran_ra_nr_modulation_from_mcs_table3(mcs_idx);
    default:
      ERROR("Invalid table %d", table);
  }

  return SRSRAN_MOD_NITEMS;
}

int srsran_ra_dl_nr_slot_nof_re(const srsran_sch_cfg_nr_t* pdsch_cfg, const srsran_sch_grant_nr_t* grant)
{
  // the number of symbols of the PDSCH allocation within the slot
  int n_sh_symb = grant->L;

  // the number of REs for DM-RS per PRB in the scheduled duration
  int n_prb_dmrs = srsran_dmrs_sch_get_N_prb(&pdsch_cfg->dmrs, grant);
  if (n_prb_dmrs < SRSRAN_SUCCESS) {
    ERROR("Invalid number of DMRS RE");
    return SRSRAN_ERROR;
  }

  // the overhead configured by higher layer parameter xOverhead in PDSCH-ServingCellConfig
  uint32_t n_prb_oh = 0;
  switch (pdsch_cfg->sch_cfg.xoverhead) {
    case srsran_xoverhead_0:
      n_prb_oh = 0;
      break;
    case srsran_xoverhead_6:
      n_prb_oh = 6;
      break;
    case srsran_xoverhead_12:
      n_prb_oh = 12;
      break;
    case srsran_xoverhead_18:
      n_prb_oh = 18;
      break;
  }

  // Compute total number of n_re used for PDSCH
  uint32_t n_re_prime = SRSRAN_NRE * n_sh_symb - n_prb_dmrs - n_prb_oh;

  uint32_t n_prb = 0;
  for (uint32_t i = 0; i < SRSRAN_MAX_PRB_NR; i++) {
    n_prb += (uint32_t)grant->prb_idx[i];
  }

  // Return the number of resource elements for PDSCH
  return SRSRAN_MIN(SRSRAN_MAX_NRE_NR, n_re_prime) * n_prb;
}

#define POW2(N) (1U << (N))

static uint32_t ra_nr_tbs_from_n_info3(uint32_t n_info)
{
  // quantized intermediate number of information bits
  uint32_t n            = (uint32_t)SRSRAN_MAX(3.0, floor(log2(n_info)) - 6.0);
  uint32_t n_info_prime = SRSRAN_MAX(ra_nr_tbs_table[0], POW2(n) * SRSRAN_FLOOR(n_info, POW2(n)));

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
  uint32_t n_info_prime = SRSRAN_MAX(3840, POW2(n) * SRSRAN_ROUND(n_info - 24.0, POW2(n)));

  if (R <= 0.25) {
    uint32_t C = SRSRAN_CEIL(n_info_prime + 24U, 3816U);
    return 8U * C * SRSRAN_CEIL(n_info_prime + 24U, 8U * C) - 24U;
  }

  if (n_info_prime > 8424) {
    uint32_t C = SRSRAN_CEIL(n_info_prime + 24U, 8424U);
    return 8U * C * SRSRAN_CEIL(n_info_prime + 24U, 8U * C) - 24U;
  }

  return 8U * SRSRAN_CEIL(n_info_prime + 24U, 8U) - 24U;
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

uint32_t srsran_ra_nr_tbs(uint32_t N_re, double S, double R, uint32_t Qm, uint32_t nof_layers)
{
  if (!isnormal(S)) {
    S = 1.0;
  }

  if (nof_layers == 0) {
    ERROR("Incorrect number of layers (%d). Setting to 1.", nof_layers);
    nof_layers = 1;
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

static int ra_nr_assert_csi_rs_dmrs_collision(const srsran_sch_cfg_nr_t* pdsch_cfg)
{
  // Generate DMRS pattern
  srsran_re_pattern_t dmrs_re_pattern = {};
  if (srsran_dmrs_sch_rvd_re_pattern(&pdsch_cfg->dmrs, &pdsch_cfg->grant, &dmrs_re_pattern) < SRSRAN_SUCCESS) {
    ERROR("Error computing DMRS pattern");
    return SRSRAN_ERROR;
  }

  // Check for collision
  if (srsran_re_pattern_check_collision(&pdsch_cfg->rvd_re, &dmrs_re_pattern) < SRSRAN_SUCCESS) {
    // Create reserved info string
    char str_rvd[512] = {};
    srsran_re_pattern_list_info(&pdsch_cfg->rvd_re, str_rvd, (uint32_t)sizeof(str_rvd));

    // Create DMRS info string
    char str_dmrs[512] = {};
    srsran_re_pattern_info(&dmrs_re_pattern, str_dmrs, (uint32_t)sizeof(str_dmrs));

    ERROR("Error. The UE is not expected to receive CSI-RS (%s) and DM-RS (%s) on the same resource elements.",
          str_rvd,
          str_dmrs);
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

int srsran_ra_nr_fill_tb(const srsran_sch_cfg_nr_t*   pdsch_cfg,
                         const srsran_sch_grant_nr_t* grant,
                         uint32_t                     mcs_idx,
                         srsran_sch_tb_t*             tb)
{
  // Get target Rate
  double R = srsran_ra_nr_R_from_mcs(
      pdsch_cfg->sch_cfg.mcs_table, grant->dci_format, grant->dci_search_space, grant->rnti_type, mcs_idx);
  if (!isnormal(R)) {
    return SRSRAN_ERROR;
  }

  // Get modulation
  srsran_mod_t m = srsran_ra_nr_mod_from_mcs(
      pdsch_cfg->sch_cfg.mcs_table, grant->dci_format, grant->dci_search_space, grant->rnti_type, mcs_idx);
  if (m >= SRSRAN_MOD_NITEMS) {
    return SRSRAN_ERROR;
  }

  // Get modulation order
  uint32_t Qm = srsran_mod_bits_x_symbol(m);
  if (Qm == 0) {
    return SRSRAN_ERROR;
  }

  // For the PDSCH assigned by a
  // - PDCCH with DCI format 1_0 with
  // - CRC scrambled by P-RNTI, or RA-RNTI,
  double S = 1.0;
  if ((SRSRAN_RNTI_ISRAR(grant->rnti) || SRSRAN_RNTI_ISPA(grant->rnti)) &&
      grant->dci_format == srsran_dci_format_nr_1_0) {
    // where the scaling factor S is determined based on the TB scaling
    //  field in the DCI as in Table 5.1.3.2-2.
    S = ra_nr_get_scaling(grant->tb_scaling_field);

    // If the scaling is invalid return error
    if (!isnormal(S)) {
      return SRSRAN_ERROR;
    }
  }

  // 1) The UE shall first determine the number of REs (N RE ) within the slot.
  int N_re = srsran_ra_dl_nr_slot_nof_re(pdsch_cfg, grant);
  if (N_re <= SRSRAN_SUCCESS) {
    ERROR("Invalid number of RE (%d)", N_re);
    return SRSRAN_ERROR;
  }

  // Calculate number of layers accordingly, assumes first codeword only
  uint32_t nof_cw         = grant->nof_layers < 5 ? 1 : 2;
  uint32_t nof_layers_cw1 = grant->nof_layers / nof_cw;
  tb->N_L                 = nof_layers_cw1;

  // Check DMRS and CSI-RS collision according to TS 38.211 7.4.1.5.3 Mapping to physical resources
  // If there was a collision, the number of RE in the grant would be wrong
  if (ra_nr_assert_csi_rs_dmrs_collision(pdsch_cfg) < SRSRAN_SUCCESS) {
    ERROR("Error: CSI-RS and DMRS collision detected");
    return SRSRAN_ERROR;
  }

  // Calculate reserved RE
  uint32_t N_re_rvd = srsran_re_pattern_list_count(&pdsch_cfg->rvd_re, grant->S, grant->S + grant->L, grant->prb_idx);

  // Steps 2,3,4
  tb->tbs      = (int)srsran_ra_nr_tbs(N_re, S, R, Qm, tb->N_L);
  tb->R        = R;
  tb->mod      = m;
  tb->nof_re   = (N_re - N_re_rvd) * grant->nof_layers;
  tb->nof_bits = tb->nof_re * Qm;
  tb->enabled  = true;

  return SRSRAN_SUCCESS;
}

static int ra_dl_dmrs(const srsran_sch_hl_cfg_nr_t* hl_cfg, srsran_sch_grant_nr_t* grant, srsran_sch_cfg_nr_t* cfg)
{
  const bool dedicated_dmrs_present =
      (grant->mapping == srsran_sch_mapping_type_A) ? hl_cfg->dmrs_typeA.present : hl_cfg->dmrs_typeB.present;

  if (grant->dci_format == srsran_dci_format_nr_1_0 || !dedicated_dmrs_present) {
    if (grant->mapping == srsran_sch_mapping_type_A) {
      // Absent default values are defined is TS 38.331 - DMRS-DownlinkConfig
      cfg->dmrs.additional_pos         = srsran_dmrs_sch_add_pos_2;
      cfg->dmrs.type                   = srsran_dmrs_sch_type_1;
      cfg->dmrs.length                 = srsran_dmrs_sch_len_1;
      cfg->dmrs.scrambling_id0_present = false;
      cfg->dmrs.scrambling_id1_present = false;
    } else {
      ERROR("Unsupported configuration");
      return SRSRAN_ERROR;
    }
  } else {
    if (grant->mapping == srsran_sch_mapping_type_A) {
      cfg->dmrs.additional_pos         = hl_cfg->dmrs_typeA.additional_pos;
      cfg->dmrs.type                   = hl_cfg->dmrs_typeA.type;
      cfg->dmrs.length                 = hl_cfg->dmrs_typeA.length;
      cfg->dmrs.scrambling_id0_present = false;
      cfg->dmrs.scrambling_id1_present = false;
    } else {
      cfg->dmrs.additional_pos         = hl_cfg->dmrs_typeB.additional_pos;
      cfg->dmrs.type                   = hl_cfg->dmrs_typeB.type;
      cfg->dmrs.length                 = hl_cfg->dmrs_typeB.length;
      cfg->dmrs.scrambling_id0_present = false;
      cfg->dmrs.scrambling_id1_present = false;
    }
  }

  // Set number of DMRS CDM groups without data
  if (grant->dci_format == srsran_dci_format_nr_1_0) {
    if (srsran_ra_dl_nr_nof_dmrs_cdm_groups_without_data_format_1_0(&cfg->dmrs, grant) < SRSRAN_SUCCESS) {
      ERROR("Error loading number of DMRS CDM groups");
      return SRSRAN_ERROR;
    }
  } else {
    ERROR("Invalid case");
    return SRSRAN_ERROR;
  }

  // Set DMRS power offset Table 6.2.2-1: The ratio of PUSCH EPRE to DM-RS EPRE
  if (ra_nr_dmrs_power_offset(grant) < SRSRAN_SUCCESS) {
    ERROR("Error setting DMRS power offset");
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

static int ra_dl_resource_mapping(const srsran_carrier_nr_t*    carrier,
                                  const srsran_slot_cfg_t*      slot,
                                  const srsran_sch_hl_cfg_nr_t* pdsch_hl_cfg,
                                  srsran_sch_cfg_nr_t*          pdsch_cfg)
{
  // SS/PBCH block transmission resources not available for PDSCH
  // ... Not implemented

  // 5.1.4.1 PDSCH resource mapping with RB symbol level granularity
  // rateMatchPatternToAddModList ... Not implemented

  // 5.1.4.2 PDSCH resource mapping with RE level granularity
  // RateMatchingPatternLTE-CRS ... Not implemented

  // Append periodic ZP-CSI-RS
  for (uint32_t i = 0; i < pdsch_hl_cfg->p_zp_csi_rs_set.count; i++) {
    // Select resource
    const srsran_csi_rs_zp_resource_t* resource = &pdsch_hl_cfg->p_zp_csi_rs_set.data[i];

    // Check if the periodic ZP-CSI is transmitted
    if (srsran_csi_rs_send(&resource->periodicity, slot)) {
      INFO("Tx/Rx ZP-CSI-RS @slot=%d\n", slot->idx);
      if (srsran_csi_rs_append_resource_to_pattern(carrier, &resource->resource_mapping, &pdsch_cfg->rvd_re)) {
        ERROR("Error appending ZP-CSI-RS as RE pattern");
        return SRSRAN_ERROR;
      }
    }
  }

  // Append semi-persistent ZP-CSI-RS here
  // ... not implemented

  // Append aperiodic ZP-CSI-RS here
  // ... not implemented

  // Append periodic NZP-CSI-RS according to TS 38.211 clause 7.3.1.5 Mapping to virtual resource blocks
  // Only aplicable if CRC is scrambled by C-RNTI, MCS-C-RNTI, CS-RNTI, or PDSCH with SPS
  bool nzp_rvd_valid = pdsch_cfg->grant.rnti_type == srsran_rnti_type_c ||
                       pdsch_cfg->grant.rnti_type == srsran_rnti_type_mcs_c ||
                       pdsch_cfg->grant.rnti_type == srsran_rnti_type_cs;
  for (uint32_t set_id = 0; set_id < SRSRAN_PHCH_CFG_MAX_NOF_CSI_RS_SETS && nzp_rvd_valid; set_id++) {
    for (uint32_t res_id = 0; res_id < pdsch_hl_cfg->nzp_csi_rs_sets[set_id].count; res_id++) {
      // Select resource
      const srsran_csi_rs_nzp_resource_t* resource = &pdsch_hl_cfg->nzp_csi_rs_sets[set_id].data[res_id];

      // Check if the periodic ZP-CSI is transmitted
      if (srsran_csi_rs_send(&resource->periodicity, slot)) {
        INFO("Tx/Rx NZP-CSI-RS set_id=%d; res=%d; @slot=%d\n", set_id, res_id, slot->idx);
        if (srsran_csi_rs_append_resource_to_pattern(carrier, &resource->resource_mapping, &pdsch_cfg->rvd_re)) {
          ERROR("Error appending ZP-CSI-RS as RE pattern");
          return SRSRAN_ERROR;
        }
      }
    }
  }

  return SRSRAN_SUCCESS;
}

int srsran_ra_dl_dci_to_grant_nr(const srsran_carrier_nr_t*    carrier,
                                 const srsran_slot_cfg_t*      slot,
                                 const srsran_sch_hl_cfg_nr_t* pdsch_hl_cfg,
                                 const srsran_dci_dl_nr_t*     dci_dl,
                                 srsran_sch_cfg_nr_t*          pdsch_cfg,
                                 srsran_sch_grant_nr_t*        pdsch_grant)
{
  // 5.2.1.1 Resource allocation in time domain
  if (srsran_ra_dl_nr_time(pdsch_hl_cfg,
                           dci_dl->ctx.rnti_type,
                           dci_dl->ctx.ss_type,
                           dci_dl->ctx.coreset_id,
                           dci_dl->time_domain_assigment,
                           pdsch_grant) < SRSRAN_SUCCESS) {
    ERROR("Error computing time domain resource allocation");
    return SRSRAN_ERROR;
  }

  // 5.1.2.2 Resource allocation in frequency domain
  if (srsran_ra_dl_nr_freq(carrier, pdsch_hl_cfg, dci_dl, pdsch_grant) < SRSRAN_SUCCESS) {
    ERROR("Error computing frequency domain resource allocation");
    return SRSRAN_ERROR;
  }

  // 5.1.2.3 Physical resource block (PRB) bundling
  // ...

  pdsch_grant->nof_layers = 1;
  pdsch_grant->dci_format = dci_dl->ctx.format;
  pdsch_grant->rnti       = dci_dl->ctx.rnti;
  pdsch_grant->rnti_type  = dci_dl->ctx.rnti_type;
  pdsch_grant->tb[0].rv   = dci_dl->rv;
  pdsch_grant->tb[0].mcs  = dci_dl->mcs;
  pdsch_grant->tb[0].ndi  = dci_dl->ndi;

  // 5.1.4 PDSCH resource mapping
  if (ra_dl_resource_mapping(carrier, slot, pdsch_hl_cfg, pdsch_cfg) < SRSRAN_SUCCESS) {
    ERROR("Error in resource mapping");
    return SRSRAN_ERROR;
  }

  // 5.1.6.2 DM-RS reception procedure
  if (ra_dl_dmrs(pdsch_hl_cfg, pdsch_grant, pdsch_cfg) < SRSRAN_SUCCESS) {
    ERROR("Error selecting DMRS configuration");
    return SRSRAN_ERROR;
  }

  // 5.1.3 Modulation order, target code rate, redundancy version and transport block size determination
  if (srsran_ra_nr_fill_tb(pdsch_cfg, pdsch_grant, dci_dl->mcs, &pdsch_grant->tb[0]) < SRSRAN_SUCCESS) {
    ERROR("Error filing tb");
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

static int
ra_ul_dmrs(const srsran_sch_hl_cfg_nr_t* pusch_hl_cfg, srsran_sch_grant_nr_t* pusch_grant, srsran_sch_cfg_nr_t* cfg)
{
  const bool dedicated_dmrs_present = (pusch_grant->mapping == srsran_sch_mapping_type_A)
                                          ? pusch_hl_cfg->dmrs_typeA.present
                                          : pusch_hl_cfg->dmrs_typeB.present;

  if (pusch_grant->dci_format == srsran_dci_format_nr_0_0 || pusch_grant->dci_format == srsran_dci_format_nr_rar ||
      !dedicated_dmrs_present) {
    if (pusch_grant->mapping == srsran_sch_mapping_type_A) {
      // Absent default values are defined is TS 38.331 - DMRS-DownlinkConfig
      cfg->dmrs.additional_pos         = srsran_dmrs_sch_add_pos_2;
      cfg->dmrs.type                   = srsran_dmrs_sch_type_1;
      cfg->dmrs.length                 = srsran_dmrs_sch_len_1;
      cfg->dmrs.scrambling_id0_present = false;
      cfg->dmrs.scrambling_id1_present = false;
    } else {
      ERROR("Unsupported configuration");
      return SRSRAN_ERROR;
    }
  } else {
    if (pusch_grant->mapping == srsran_sch_mapping_type_A) {
      cfg->dmrs.additional_pos         = pusch_hl_cfg->dmrs_typeA.additional_pos;
      cfg->dmrs.type                   = pusch_hl_cfg->dmrs_typeA.type;
      cfg->dmrs.length                 = pusch_hl_cfg->dmrs_typeA.length;
      cfg->dmrs.scrambling_id0_present = false;
      cfg->dmrs.scrambling_id1_present = false;
    } else {
      cfg->dmrs.additional_pos         = pusch_hl_cfg->dmrs_typeB.additional_pos;
      cfg->dmrs.type                   = pusch_hl_cfg->dmrs_typeB.type;
      cfg->dmrs.length                 = pusch_hl_cfg->dmrs_typeB.length;
      cfg->dmrs.scrambling_id0_present = false;
      cfg->dmrs.scrambling_id1_present = false;
    }
  }

  // Set number of DMRS CDM groups without data
  if (pusch_grant->dci_format == srsran_dci_format_nr_0_0 || pusch_grant->dci_format == srsran_dci_format_nr_rar) {
    if (srsran_ra_ul_nr_nof_dmrs_cdm_groups_without_data_format_0_0(cfg, pusch_grant) < SRSRAN_SUCCESS) {
      ERROR("Error loading number of DMRS CDM groups");
      return SRSRAN_ERROR;
    }
  } else {
    ERROR("DCI format not implemented %s", srsran_dci_format_nr_string(pusch_grant->dci_format));
    return SRSRAN_ERROR;
  }

  // Set DMRS power offset Table 6.2.2-1: The ratio of PUSCH EPRE to DM-RS EPRE
  if (ra_nr_dmrs_power_offset(pusch_grant) < SRSRAN_SUCCESS) {
    ERROR("Error setting DMRS power offset");
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

int srsran_ra_ul_dci_to_grant_nr(const srsran_carrier_nr_t*    carrier,
                                 const srsran_sch_hl_cfg_nr_t* pusch_hl_cfg,
                                 const srsran_dci_ul_nr_t*     dci_ul,
                                 srsran_sch_cfg_nr_t*          pusch_cfg,
                                 srsran_sch_grant_nr_t*        pusch_grant)
{
  // 5.2.1.1 Resource allocation in time domain
  if (srsran_ra_ul_nr_time(pusch_hl_cfg,
                           dci_ul->ctx.rnti_type,
                           dci_ul->ctx.ss_type,
                           dci_ul->ctx.coreset_id,
                           dci_ul->time_domain_assigment,
                           pusch_grant) < SRSRAN_SUCCESS) {
    ERROR("Error computing time domain resource allocation");
    return SRSRAN_ERROR;
  }

  // 5.1.2.2 Resource allocation in frequency domain
  if (srsran_ra_ul_nr_freq(carrier, pusch_hl_cfg, dci_ul, pusch_grant) < SRSRAN_SUCCESS) {
    ERROR("Error computing frequency domain resource allocation");
    return SRSRAN_ERROR;
  }

  // 5.1.2.3 Physical resource block (PRB) bundling
  // ...

  pusch_grant->nof_layers = 1;
  pusch_grant->dci_format = dci_ul->ctx.format;
  pusch_grant->rnti       = dci_ul->ctx.rnti;
  pusch_grant->rnti_type  = dci_ul->ctx.rnti_type;
  pusch_grant->tb[0].rv   = dci_ul->rv;
  pusch_grant->tb[0].mcs  = dci_ul->mcs;
  pusch_grant->tb[0].ndi  = dci_ul->ndi;

  // 5.1.6.2 DM-RS reception procedure
  if (ra_ul_dmrs(pusch_hl_cfg, pusch_grant, pusch_cfg) < SRSRAN_SUCCESS) {
    ERROR("Error selecting DMRS configuration");
    return SRSRAN_ERROR;
  }

  // 5.1.3 Modulation order, target code rate, redundancy version and transport block size determination
  if (srsran_ra_nr_fill_tb(pusch_cfg, pusch_grant, dci_ul->mcs, &pusch_grant->tb[0]) < SRSRAN_SUCCESS) {
    ERROR("Error filing tb");
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

/*
 * Implements clauses related to HARQ-ACK beta offset selection from the section `9.3 UCI reporting in physical uplink
 * shared channel`
 */
static float ra_ul_beta_offset_ack_semistatic(const srsran_beta_offsets_t* beta_offsets,
                                              const srsran_uci_cfg_nr_t*   uci_cfg)
{
  if (isnormal(beta_offsets->fix_ack)) {
    return beta_offsets->fix_ack;
  }

  // Select Beta Offset index from the number of HARQ-ACK bits
  uint32_t beta_offset_index = beta_offsets->ack_index1;
  if (uci_cfg->o_ack > 11) {
    beta_offset_index = beta_offsets->ack_index3;
  } else if (uci_cfg->o_ack > 2) {
    beta_offset_index = beta_offsets->ack_index2;
  }

  // Protect table boundary
  if (beta_offset_index >= RA_NR_BETA_OFFSET_HARQACK_SIZE) {
    ERROR("Beta offset index for HARQ-ACK (%d) for O_ack=%d exceeds table size (%d)",
          beta_offset_index,
          uci_cfg->o_ack,
          RA_NR_BETA_OFFSET_HARQACK_SIZE);
    return NAN;
  }

  // Select beta offset from Table 9.3-1
  return ra_nr_beta_offset_ack_table[beta_offset_index];
}

/*
 * Implements clauses related to HARQ-ACK beta offset selection from the section `9.3 UCI reporting in physical uplink
 * shared channel`
 */
static float ra_ul_beta_offset_csi_semistatic(const srsran_beta_offsets_t* beta_offsets,
                                              const srsran_uci_cfg_nr_t*   uci_cfg,
                                              bool                         part2)
{
  float fix_beta_offset = part2 ? beta_offsets->fix_csi2 : beta_offsets->fix_csi1;
  if (isnormal(fix_beta_offset)) {
    return fix_beta_offset;
  }

  int O_csi1 = srsran_csi_part1_nof_bits(uci_cfg->csi, uci_cfg->nof_csi);
  int O_csi2 = srsran_csi_part2_nof_bits(uci_cfg->csi, uci_cfg->nof_csi);
  if (O_csi1 < SRSRAN_SUCCESS || O_csi2 < SRSRAN_SUCCESS) {
    ERROR("Invalid O_csi1 (%d) or O_csi2(%d)", O_csi1, O_csi2);
    return NAN;
  }

  // Calculate number of CSI bits; CSI part 2 is not supported.
  uint32_t O_csi = (uint32_t)(part2 ? O_csi2 : O_csi1);

  // Select Beta Offset index from the number of HARQ-ACK bits
  uint32_t beta_offset_index = part2 ? beta_offsets->csi2_index1 : beta_offsets->csi1_index1;
  if (O_csi > 11) {
    beta_offset_index = part2 ? beta_offsets->csi2_index2 : beta_offsets->csi1_index2;
  }

  // Protect table boundary
  if (beta_offset_index >= RA_NR_BETA_OFFSET_CSI_SIZE) {
    ERROR("Beta offset index for CSI (%d) for O_csi=%d exceeds table size (%d)",
          beta_offset_index,
          O_csi,
          RA_NR_BETA_OFFSET_CSI_SIZE);
    return NAN;
  }

  // Select beta offset from Table 9.3-1
  return ra_nr_beta_offset_csi_table[beta_offset_index];
}

int srsran_ra_ul_set_grant_uci_nr(const srsran_carrier_nr_t*    carrier,
                                  const srsran_sch_hl_cfg_nr_t* pusch_hl_cfg,
                                  const srsran_uci_cfg_nr_t*    uci_cfg,
                                  srsran_sch_cfg_nr_t*          pusch_cfg)
{
  if (pusch_cfg->grant.nof_prb == 0) {
    ERROR("Invalid number of PRB (%d)", pusch_cfg->grant.nof_prb);
    return SRSRAN_ERROR;
  }

  // Initially, copy all fields
  pusch_cfg->uci = *uci_cfg;

  // Reset UCI PUSCH configuration
  SRSRAN_MEM_ZERO(&pusch_cfg->uci.pusch, srsran_uci_nr_pusch_cfg_t, 1);

  // Get DMRS symbol indexes
  uint32_t nof_dmrs_l                          = 0;
  uint32_t dmrs_l[SRSRAN_DMRS_SCH_MAX_SYMBOLS] = {};
  int      n = srsran_dmrs_sch_get_symbols_idx(&pusch_cfg->dmrs, &pusch_cfg->grant, dmrs_l);
  if (n < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }
  nof_dmrs_l = (uint32_t)n;

  // Find OFDM symbol index of the first OFDM symbol after the first set of consecutive OFDM symbol(s) carrying DMRS
  // Starts at first OFDM symbol carrying DMRS
  for (uint32_t l = dmrs_l[0], dmrs_l_idx = 0; l < pusch_cfg->grant.S + pusch_cfg->grant.L; l++) {
    // Check if it is not carrying DMRS...
    if (l != dmrs_l[dmrs_l_idx]) {
      // Set value and stop iterating
      pusch_cfg->uci.pusch.l0 = l;
      break;
    }

    // Move to the next DMRS OFDM symbol index
    if (dmrs_l_idx < nof_dmrs_l) {
      dmrs_l_idx++;
    }
  }

  // Find OFDM symbol index of the first OFDM symbol that does not carry DMRS
  // Starts at first OFDM symbol of the PUSCH transmission
  for (uint32_t l = pusch_cfg->grant.S, dmrs_l_idx = 0; l < pusch_cfg->grant.S + pusch_cfg->grant.L; l++) {
    // Check if it is not carrying DMRS...
    if (l != dmrs_l[dmrs_l_idx]) {
      pusch_cfg->uci.pusch.l1 = l;
      break;
    }

    // Move to the next DMRS OFDM symbol index
    if (dmrs_l_idx < nof_dmrs_l) {
      dmrs_l_idx++;
    }
  }

  // Number of DMRS per PRB
  uint32_t n_sc_dmrs = SRSRAN_DMRS_SCH_SC(pusch_cfg->grant.nof_dmrs_cdm_groups_without_data, pusch_cfg->dmrs.type);

  // Set UCI RE number of candidates per OFDM symbol according to TS 38.312 6.3.2.4.2.1
  for (uint32_t l = 0, dmrs_l_idx = 0; l < SRSRAN_NSYMB_PER_SLOT_NR; l++) {
    // Skip if OFDM symbol is outside of the PUSCH transmission
    if (l < pusch_cfg->grant.S || l >= (pusch_cfg->grant.S + pusch_cfg->grant.L)) {
      pusch_cfg->uci.pusch.M_pusch_sc[l] = 0;
      pusch_cfg->uci.pusch.M_uci_sc[l]   = 0;
      continue;
    }

    // OFDM symbol carries DMRS
    if (l == dmrs_l[dmrs_l_idx]) {
      // Calculate PUSCH RE candidates
      pusch_cfg->uci.pusch.M_pusch_sc[l] = pusch_cfg->grant.nof_prb * (SRSRAN_NRE - n_sc_dmrs);

      // The Number of RE candidates for UCI are 0
      pusch_cfg->uci.pusch.M_uci_sc[l] = 0;

      // Advance DMRS symbol index
      dmrs_l_idx++;

      // Skip to next symbol
      continue;
    }

    // Number of RE for Phase Tracking Reference Signals (PT-RS)
    uint32_t M_ptrs_sc = 0; // Not implemented yet

    // Number of RE given by the grant
    pusch_cfg->uci.pusch.M_pusch_sc[l] = pusch_cfg->grant.nof_prb * SRSRAN_NRE;

    // Calculate the number of UCI candidates
    pusch_cfg->uci.pusch.M_uci_sc[l] = pusch_cfg->uci.pusch.M_pusch_sc[l] - M_ptrs_sc;
  }

  // Generate SCH Transport block information
  srsran_sch_nr_tb_info_t sch_tb_info = {};
  if (srsran_sch_nr_fill_tb_info(carrier, &pusch_cfg->sch_cfg, &pusch_cfg->grant.tb[0], &sch_tb_info) <
      SRSRAN_SUCCESS) {
    ERROR("Generating TB info");
    return SRSRAN_ERROR;
  }

  // Calculate the sum of codeblock sizes
  for (uint32_t i = 0; i < sch_tb_info.C; i++) {
    // Accumulate codeblock size if mask is enabled
    pusch_cfg->uci.pusch.K_sum += (sch_tb_info.mask[i]) ? sch_tb_info.Kr : 0;
  }

  // Set other PUSCH parameters
  pusch_cfg->uci.pusch.modulation = pusch_cfg->grant.tb[0].mod;
  pusch_cfg->uci.pusch.nof_layers = pusch_cfg->grant.nof_layers;
  pusch_cfg->uci.pusch.R          = (float)pusch_cfg->grant.tb[0].R;
  pusch_cfg->uci.pusch.nof_re     = pusch_cfg->grant.tb[0].nof_re;

  // Select beta offsets
  pusch_cfg->uci.pusch.beta_harq_ack_offset = ra_ul_beta_offset_ack_semistatic(&pusch_hl_cfg->beta_offsets, uci_cfg);
  if (!isnormal(pusch_cfg->uci.pusch.beta_harq_ack_offset)) {
    return SRSRAN_ERROR;
  }

  pusch_cfg->uci.pusch.beta_csi1_offset = ra_ul_beta_offset_csi_semistatic(&pusch_hl_cfg->beta_offsets, uci_cfg, false);
  if (!isnormal(pusch_cfg->uci.pusch.beta_csi1_offset)) {
    return SRSRAN_ERROR;
  }

  pusch_cfg->uci.pusch.beta_csi2_offset = ra_ul_beta_offset_csi_semistatic(&pusch_hl_cfg->beta_offsets, uci_cfg, true);
  if (!isnormal(pusch_cfg->uci.pusch.beta_csi2_offset)) {
    return SRSRAN_ERROR;
  }

  pusch_cfg->uci.pusch.alpha = pusch_hl_cfg->scaling;
  if (!isnormal(pusch_cfg->uci.pusch.alpha)) {
    ERROR("Invalid Scaling (%f)", pusch_cfg->uci.pusch.alpha);
    return SRSRAN_ERROR;
  }

  // Calculate number of UCI encoded bits
  int Gack = 0;
  if (pusch_cfg->uci.o_ack > 2) {
    Gack = srsran_uci_nr_pusch_ack_nof_bits(&pusch_cfg->uci.pusch, pusch_cfg->uci.o_ack);
    if (Gack < SRSRAN_SUCCESS) {
      ERROR("Error calculating Qdack");
      return SRSRAN_ERROR;
    }
  }
  int Gcsi1 = srsran_uci_nr_pusch_csi1_nof_bits(&pusch_cfg->uci);
  if (Gcsi1 < SRSRAN_SUCCESS) {
    ERROR("Error calculating Qdack");
    return SRSRAN_ERROR;
  }
  int Gcsi2 = 0; // NOT supported

  // Update Number of TB encoded bits
  for (uint32_t i = 0; i < SRSRAN_MAX_TB; i++) {
    pusch_cfg->grant.tb[i].nof_bits =
        pusch_cfg->grant.tb[i].nof_re * srsran_mod_bits_x_symbol(pusch_cfg->grant.tb[i].mod) - Gack - Gcsi1 - Gcsi2;
  }

  return SRSRAN_SUCCESS;
}
