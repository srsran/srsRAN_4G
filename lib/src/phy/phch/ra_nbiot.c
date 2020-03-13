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

#include "srslte/phy/phch/ra_nbiot.h"
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/utils/bit.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"
#include "tbs_tables_nbiot.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

#define MAX_I_TBS_VAL 12
#define MAX_I_TBS_VAL_SIB 11
#define MAX_I_SF_VAL 7
#define EUTRA_CONTROL_REGION_SIZE 3 // TODO: Needs to be set by SIB1

/// Number of repetitions according to Table 16.4.1.3-2 in TS 36.213 13.2.0
const int n_rep_table[16] = {1, 2, 4, 8, 16, 32, 64, 128, 192, 256, 384, 512, 768, 1024, 1536, 2048};

/// Number of repetitions for NPDSCH carrying SystemInformationBlockType1-NB according to Table 16.4.1.3-3 in
/// TS 36.213 13.2.0
const int n_rep_table_sib1[16] = {4, 8, 16, 4, 8, 16, 4, 8, 16, 4, 8, 16, 0, 0, 0, 0};

/// Number of repetitions for NPUSCH according to Table 16.5.1.1-3 in TS 36.213 13.2.0
const int n_rep_table_npusch[8] = {1, 2, 4, 8, 16, 32, 64, 128};

/// Number of resource units (RU) for NPUSCH according to Table 16.5.1.1-2 in TS 36.213 13.2.0
const int n_ru_table_npusch[8] = {1, 2, 3, 4, 5, 6, 8, 10};

/// k0 value for DCI format N1 for R_max smaller than 128 according to Table 16.4.1-1 in TS 36.213 13.2.0
const int k0_table_formatN1_r_st_128[8] = {0, 4, 8, 12, 16, 32, 64, 128};

/// k0 value for DCI format N1 for R_max greater or equal than 128 according to Table 16.4.1-1 in TS 36.213 13.2.0
const int k0_table_formatN1_r_geq_128[8] = {0, 16, 32, 64, 128, 256, 512, 1024};

/// k0 value for DCI format N0 according to Table 16.5.1-1 in TS 36.213 13.2.0
const int k0_table_formatN0[4] = {8, 16, 32, 64};

/// k0 value contained in RAR grant according to Table 16.5.1-1 in TS 36.213 13.2.0, but with changes mentioned
/// in 16.3.3
const int k0_table_rar_grant[4] = {12, 16, 32, 64};

/// Starting radio frame for the first transmission of the NPDSCH carrying SystemInformationBlockType1-NB according to
/// Table 16.4.1.3-4 in TS 36.213 13.2.0
const int first_frame_sib1[3][4] = {{0, 16, 32, 48}, {0, 16, -1, -1}, {0, 1, -1, -1}};

/// Modulation and TBS index table for NPUSCH with N_sc_RU == 1 according to Table 16.5.1.2-1 in TS 36.213 13.2.0
const int i_mcs_to_i_tbs_npusch[11] = {0, 2, 1, 3, 4, 5, 6, 7, 8, 9, 10};

/// ACK/NACH resource computation TS 36.213 Section 16.4.2, for 15kHz and 3.75kHz
const int ack_nack_resource_field_to_sc[SRSLTE_NPUSCH_SC_SPACING_NITEMS][16] = {
    {0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3},
    {38, 39, 40, 41, 42, 43, 44, 45, 38, 39, 40, 41, 42, 43, 44, 45}};

const int ack_nack_resource_field_to_k0[SRSLTE_NPUSCH_SC_SPACING_NITEMS][16] = {
    {13, 13, 13, 13, 15, 15, 15, 15, 17, 17, 17, 17, 18, 18, 18, 18},
    {13, 13, 13, 13, 13, 13, 13, 13, 21, 21, 21, 21, 21, 21, 21, 21}};

/// Calculate the number of resource elements per subframe that carry data
uint32_t srslte_ra_nbiot_dl_grant_nof_re(srslte_nbiot_cell_t cell, uint32_t l_start)
{
  /// start with one full PRB
  uint32_t re = SRSLTE_CP_NORM_SF_NSYMB * SRSLTE_NRE;

  /// remove lstart number of symbols
  re -= l_start * SRSLTE_NRE;

  /// remove NRS
  switch (cell.nof_ports) {
    case 1:
    case 2:
      re -= 8 * cell.nof_ports;
      break;
    case 4:
      printf("ERROR: 4 ports are not supported for NB-IoT\n");
      break;
  }

  /// remove CRS for inband deployments
  if (cell.mode <= SRSLTE_NBIOT_MODE_INBAND_DIFFERENT_PCI) {
    switch (cell.base.nof_ports) {
      case 1:
      case 2:
        re -= 8 * cell.base.nof_ports;
        // first two symbols used for CRS
        if (l_start >= 2)
          re += (4 * cell.base.nof_ports) / 2;
        break;
      case 4:
        re -= 8 * 3;
        break;
    }
  }
  return re;
}

void srslte_ra_nbiot_dl_grant_to_nbits(srslte_ra_nbiot_dl_grant_t* grant,
                                       srslte_nbiot_cell_t         cell,
                                       uint32_t                    sf_idx,
                                       srslte_ra_nbits_t*          nbits)
{
  /// Compute number of RE
  nbits->lstart   = grant->l_start;
  nbits->nof_re   = srslte_ra_nbiot_dl_grant_nof_re(cell, grant->l_start);
  nbits->nof_symb = 2 * SRSLTE_CP_NSYMB(cell.base.cp) - nbits->lstart;
  nbits->nof_bits = nbits->nof_re * grant->Qm;
}

/// Transport block size determination 16.4.1.5 in 36.213 v13.2.0
static int nbiot_dl_dci_to_grant_mcs(srslte_ra_nbiot_dl_dci_t* dci, srslte_ra_nbiot_dl_grant_t* grant)
{
  int      tbs   = -1;
  uint32_t i_tbs = 0, i_sf = 0;

  grant->mcs[0].mod = SRSLTE_MOD_QPSK;

  // limit config values in DCI
  dci->alloc.sched_info_sib1 = SRSLTE_MIN(dci->alloc.sched_info_sib1, MAX_I_TBS_VAL_SIB);
  dci->mcs_idx               = SRSLTE_MIN(dci->mcs_idx, MAX_I_TBS_VAL);
  dci->alloc.i_sf            = SRSLTE_MIN(dci->alloc.i_sf, MAX_I_SF_VAL);

  if (dci->alloc.has_sib1) {
    i_tbs = dci->alloc.sched_info_sib1;
    tbs   = tbs_table_nbiot_sib1[i_tbs];
  } else {
    i_tbs = dci->mcs_idx;
    i_sf  = dci->alloc.i_sf;
    tbs   = tbs_table_nbiot[i_tbs][i_sf];
  }

  if (tbs <= 0) {
    INFO("Unsupported resource allocation specified: i_tbs=%d [0,12], i_sf=%d [0,7]\n", i_tbs, i_sf);
    return SRSLTE_ERROR;
  } else {
    grant->mcs[0].tbs = (uint32_t)tbs;
    return SRSLTE_SUCCESS;
  }
}

int srslte_ra_n_rep_from_dci(srslte_ra_nbiot_dl_dci_t* dci)
{
  return (dci->alloc.has_sib1 ? n_rep_table_sib1[dci->alloc.sched_info_sib1] : n_rep_table[dci->alloc.i_rep]);
}

int srslte_ra_n_rep_sib1_nb(srslte_mib_nb_t* mib)
{
  return n_rep_table_sib1[mib->sched_info_sib1];
}

int srslte_ra_nbiot_get_sib1_tbs(srslte_mib_nb_t* mib)
{
  uint32_t i_tbs = SRSLTE_MIN(mib->sched_info_sib1, MAX_I_TBS_VAL_SIB);
  return tbs_table_nbiot_sib1[i_tbs];
}

int srslte_ra_nbiot_get_npdsch_tbs(uint32_t i_tbs, uint32_t i_sf)
{
  if (i_tbs <= 12 && i_sf <= 7) {
    return tbs_table_nbiot[i_tbs][i_sf];
  } else {
    return 0;
  }
}

int srslte_ra_nbiot_get_npusch_tbs(uint32_t i_tbs, uint32_t i_ru)
{
  if (i_tbs <= 12 && i_ru <= 7) {
    return tbs_table_npusch[i_tbs][i_ru];
  } else {
    return 0;
  }
}

uint32_t srslte_ra_n_rep_sib1_nb_idx(srslte_mib_nb_t* mib)
{
  switch (srslte_ra_n_rep_sib1_nb(mib)) {
    case 4:
      return 0;
    case 8:
      return 1;
    case 16:
      return 2;
    default:
      return 0;
  }
}

int srslte_ra_nbiot_get_starting_sib1_frame(uint32_t cell_id, srslte_mib_nb_t* mib)
{
  return first_frame_sib1[srslte_ra_n_rep_sib1_nb_idx(mib)][cell_id % (srslte_ra_n_rep_sib1_nb(mib) == 4 ? 4 : 2)];
}

int srslte_ra_nbiot_sib1_start(uint32_t n_id_ncell, srslte_mib_nb_t* mib)
{
  return ((srslte_ra_n_rep_sib1_nb(mib) == 16 && n_id_ncell % 2 == 1) ? 1 : 0);
}

/// Section 16.4.1 in 36.213 v13.3.0
int srslte_ra_k0_from_dci(srslte_ra_nbiot_dl_dci_t* dci, uint32_t r_max)
{
  if (dci->dci_is_n2) {
    return 0;
  }

  if (r_max < 128) {
    return k0_table_formatN1_r_st_128[dci->alloc.i_delay];
  } else {
    return k0_table_formatN1_r_geq_128[dci->alloc.i_delay];
  }
}

int srslte_ra_n_sf_from_dci(srslte_ra_nbiot_dl_dci_t* dci)
{
  if (dci->alloc.i_sf < 6) {
    return dci->alloc.i_sf + 1;
  } else if (dci->alloc.i_sf == 6) {
    return 8;
  } else if (dci->alloc.i_sf == 7) {
    return 10;
  }
  // Invalid DCI config, default to 1 subframe in this case
  return 1;
}

/// According to TS 36.211 Sec 10.2.6 before obtaining operationModeInfo,
/// only sf_idx 0, 4 and 9 if no NSSS is sent carry NRS
bool srslte_ra_nbiot_dl_has_ref_signal(uint32_t tti)
{
  return (tti % 10 == 0 || tti % 10 == 4 || (tti % 10 == 9 && (tti / 10 % 2 != 0)));
}

/// According to TS 36.211 Sec 10.2.6 in opMode standalone and guardband,
/// only sf_idx 0, 1, 3, 4, and 9 if no NSSS is sent carry NRS
bool srslte_ra_nbiot_dl_has_ref_signal_standalone(uint32_t tti)
{
  return (tti % 10 == 0 || tti % 10 == 1 || tti % 10 == 3 || tti % 10 == 4 || (tti % 10 == 9 && (tti / 10 % 2 != 0)));
}

/// According to TS 36.211 Sec 10.2.6 before optaining operationModeInfo
/// Only sf_idx 0, 4 and 9 if no NSSS is sent carry NRS
bool srslte_ra_nbiot_dl_has_ref_signal_inband(uint32_t tti)
{
  return srslte_ra_nbiot_dl_has_ref_signal(tti);
}

/// Valid NB-IoT DL subframes are subframes that DON'T carry:
///  - NPBCH (subframe 0)
///  - NPSS (subframe 5)
///  - NSSS (subframe 9 in all even frames)
bool srslte_ra_nbiot_is_valid_dl_sf(uint32_t tti)
{
  return !(tti % 10 == 0 || tti % 10 == 5 || (tti % 10 == 9 && ((tti / 10) % 2 == 0)));
}

int srslte_ra_nbiot_dl_dci_to_grant(srslte_ra_nbiot_dl_dci_t*   dci,
                                    srslte_ra_nbiot_dl_grant_t* grant,
                                    uint32_t                    sfn,
                                    uint32_t                    sf_idx,
                                    uint32_t                    r_max,
                                    bool                        is_prescheduled,
                                    srslte_nbiot_mode_t         mode)
{
  if (!nbiot_dl_dci_to_grant_mcs(dci, grant)) {
    /// Fill rest of grant structure
    grant->mcs[0].mcs_idx    = dci->mcs_idx;
    grant->Qm                = srslte_mod_bits_x_symbol(grant->mcs[0].mod);
    grant->k0                = srslte_ra_k0_from_dci(dci, r_max);
    grant->nof_sf            = dci->alloc.has_sib1 ? 8 : srslte_ra_n_sf_from_dci(dci);
    grant->nof_rep           = srslte_ra_n_rep_from_dci(dci);
    grant->has_sib1          = dci->alloc.has_sib1;
    grant->ack_nack_resource = dci->alloc.harq_ack;

    if (mode == SRSLTE_NBIOT_MODE_INBAND_SAME_PCI || mode == SRSLTE_NBIOT_MODE_INBAND_DIFFERENT_PCI) {
      grant->l_start = dci->alloc.has_sib1 ? 3 : EUTRA_CONTROL_REGION_SIZE;
    } else {
      grant->l_start = 0;
    }

    /// compute grant duration
    int      offset = (is_prescheduled == true) ? 0 : 5;
    uint32_t rx_tti = sfn * 10 + sf_idx;
    uint32_t tx_tti = (rx_tti + offset + grant->k0) % 10240;

    /// make sure tx_tti is a valid DL sf
    while (!srslte_ra_nbiot_is_valid_dl_sf(tx_tti)) {
      tx_tti = (tx_tti + 1) % 10240;
    }
    grant->start_hfn   = 0; // not handling HFN
    grant->start_sfn   = tx_tti / 10;
    grant->start_sfidx = tx_tti % 10;
  } else {
    return SRSLTE_ERROR;
  }
  return SRSLTE_SUCCESS;
}

void srslte_nbiot_dl_dci_fprint(FILE* f, srslte_ra_nbiot_dl_dci_t* dci)
{
  fprintf(f, "NB-IoT DL DCI:\n");
  fprintf(f, " - Format flag:\t\t\t\t%d\n", dci->format);
  fprintf(f, "   + FormatN%d DCI:\t\t\t%s\n", dci->format ? 1 : 0, dci->format ? "Downlink" : "Uplink");
  fprintf(f, " - PDCCH Order:\t\t\t\t%d\n", dci->alloc.is_ra);
  fprintf(f, " - Scheduling delay:\t\t\t%d (%d subframes)\n", dci->alloc.i_delay, srslte_ra_k0_from_dci(dci, 64));
  fprintf(f, " - Resource assignment:\t\t\t%d\n", dci->alloc.i_sf);
  fprintf(f, "   + Number of subframes:\t\t%d\n", srslte_ra_n_sf_from_dci(dci));
  fprintf(f, " - Modulation and coding scheme index:\t%d\n", dci->mcs_idx);
  fprintf(f, " - Repetition number:\t\t\t%d\n", dci->alloc.i_rep);
  fprintf(f, "   + Number of repetitions:\t\t%d\n", srslte_ra_n_rep_from_dci(dci));
  fprintf(f, " - New data indicator:\t\t\t%d\n", dci->ndi);
  fprintf(f, " - HARQ-ACK resource:\t\t\t%d\n", dci->alloc.harq_ack);
  fprintf(f, " - DCI subframe repetition number:\t%d\n", dci->alloc.dci_sf_rep_num);
}

void srslte_ra_npusch_fprint(FILE* f, srslte_ra_nbiot_ul_dci_t* dci)
{
  fprintf(f, "NB-IoT UL DCI:\n");
  fprintf(f, " - Format flag:\t\t\t\t%d\n", dci->format);
  fprintf(f, " - Subcarrier indication field:\t\t%d\n", dci->i_sc);
  fprintf(f, " - Scheduling delay:\t\t\t%d\n", dci->i_delay);
  fprintf(f, " - Resource assignment:\t\t\t%d\n", dci->i_ru);
  fprintf(f, " - Modulation and coding scheme index:\t%d\n", dci->i_mcs);
  fprintf(f, " - Redundency version:\t\t\t%d\n", dci->i_rv);
  fprintf(f, " - Repetition number:\t\t\t%d\n", dci->i_rep);
  fprintf(f, " - New data indicator:\t\t\t%d\n", dci->ndi);
  fprintf(f, " - DCI subframe repetition number:\t%d\n", dci->dci_sf_rep_num);
}

void srslte_ra_nbiot_dl_grant_fprint(FILE* f, srslte_ra_nbiot_dl_grant_t* grant)
{
  fprintf(f, "DL grant config:\n");
  fprintf(f, " - Number of subframes:\t\t\t%d\n", grant->nof_sf);
  fprintf(f, " - Number of repetitions:\t\t%d\n", grant->nof_rep);
  fprintf(f, " - Total number of subframes:\t\t%d\n", grant->nof_sf * grant->nof_rep);
  fprintf(f, " - Starting SFN:\t\t\t%d\n", grant->start_sfn);
  fprintf(f, " - Starting SF index:\t\t\t%d\n", grant->start_sfidx);
  fprintf(f, " - Modulation type:\t\t\t%s\n", srslte_mod_string(grant->mcs[0].mod));
  fprintf(f, " - Transport block size:\t\t%d\n", grant->mcs[0].tbs);
}

void srslte_ra_nbiot_ul_grant_fprint(FILE* f, srslte_ra_nbiot_ul_grant_t* grant)
{
  fprintf(f, "UL grant config:\n");
  fprintf(f, " - NPUSCH format:\t\t%s\n", srslte_npusch_format_text[grant->format]);
  fprintf(f, " - Delay:\t\t\t%d subframes\n", grant->k0);
  fprintf(f, " - Tx TTI:\t\t\t%d\n", grant->tx_tti);
  fprintf(f, " - Subcarriers:\t\t\t%d (%s)\n", grant->nof_sc, srslte_npusch_sc_spacing_text[grant->sc_spacing]);
  fprintf(f, " - Number of slots:\t\t%d\n", grant->nof_slots);
  fprintf(f, " - Number of resource units:\t%d\n", grant->nof_ru);
  fprintf(f, " - Number of repetitions:\t%d\n", grant->nof_rep);
  fprintf(f, " - Modulation type:\t\t%s\n", srslte_mod_string(grant->mcs.mod));
  fprintf(f, " - Transport block size:\t%d\n", grant->mcs.tbs);
}

/// UL RA for Msg3, i.e. RAR grant
int srslte_ra_nbiot_ul_rar_dci_to_grant(srslte_ra_nbiot_ul_dci_t*   dci,
                                        srslte_ra_nbiot_ul_grant_t* grant,
                                        uint32_t                    rx_tti)
{
  /// use DCI to fill default UL grant values
  if (srslte_ra_nbiot_ul_dci_to_grant(dci, grant, rx_tti, SRSLTE_NPUSCH_SC_SPACING_15000)) {
    fprintf(stderr, "Error while reading UL DCI for RAR grant.\n");
    return SRSLTE_ERROR;
  }

  /// now update all RAR specific fields
  grant->sc_spacing = dci->sc_spacing;
  assert(dci->i_delay <= 3);
  grant->k0     = k0_table_rar_grant[dci->i_delay];
  grant->tx_tti = (rx_tti + grant->k0 + 1) % 10240;

  /// set number of RU
  switch (dci->i_mcs) {
    case 0:
      grant->nof_ru = 4;
      break;
    case 1:
      grant->nof_ru = 3;
      break;
    case 2:
      grant->nof_ru = 1;
      break;
    default:
      fprintf(stderr, "Invalid i_mcs value in UL DCI for RAR grant.\n");
      return SRSLTE_ERROR;
  }

  /// set modulation
  grant->mcs.tbs = 88;
  if (grant->sc_spacing == SRSLTE_NPUSCH_SC_SPACING_15000 && dci->i_sc > 11) {
    grant->mcs.mod = SRSLTE_MOD_QPSK;
  } else if (dci->i_sc <= 11) {
    // TODO: Use SRSLTE_MOD_PI2_BPSK and SRSLTE_MOD_PI4_QPSK
    grant->mcs.mod = (dci->i_mcs == 0) ? SRSLTE_MOD_BPSK : SRSLTE_MOD_QPSK;
  }
  return 0;
}

/// Fill a grant for NPUSCH without UL-SCH data but for UL control information
void srslte_ra_nbiot_ul_get_uci_grant(srslte_ra_nbiot_ul_grant_t* grant,
                                      const uint8_t               resource_field,
                                      const uint32_t              tti)
{
  bzero(grant, sizeof(srslte_ra_nbiot_ul_grant_t));
  grant->format          = SRSLTE_NPUSCH_FORMAT2;
  grant->sc_spacing      = SRSLTE_NPUSCH_SC_SPACING_15000;
  grant->sc_alloc_set[0] = ack_nack_resource_field_to_sc[grant->sc_spacing][resource_field];
  grant->nof_sc          = 1;
  grant->k0              = ack_nack_resource_field_to_k0[grant->sc_spacing][resource_field];
  grant->tx_tti          = (tti + grant->k0) % 10240;
  grant->mcs.mcs_idx     = 1;
  grant->mcs.mod         = SRSLTE_MOD_BPSK;
  grant->mcs.tbs         = 16;
  grant->Qm              = 1;
  grant->nof_ru          = 1;
  grant->nof_slots       = 4;
  grant->nof_rep         = 0; // TODO: set appropiatly
  grant->rv_idx          = 0;
}

/// Transport block size determination 16.5.1.2 in 36.213 v13.2.0
int srslte_ra_nbiot_ul_dci_to_grant(srslte_ra_nbiot_ul_dci_t*   dci,
                                    srslte_ra_nbiot_ul_grant_t* grant,
                                    uint32_t                    rx_tti,
                                    srslte_npusch_sc_spacing_t  spacing)
{
  bzero(grant, sizeof(srslte_ra_nbiot_ul_grant_t));
  grant->format     = SRSLTE_NPUSCH_FORMAT1;
  grant->sc_spacing = SRSLTE_NPUSCH_SC_SPACING_15000;

  int      tbs   = -1;
  uint32_t i_tbs = 0;

  /// calculate actual values
  if (dci->i_sc <= 11) {
    /// the value of i_sc
    grant->sc_alloc_set[0] = dci->i_sc;
    grant->nof_sc          = 1;
  } else {
    printf("UL i_sc > 11 not implemented yet!\n");
    return SRSLTE_ERROR;
  }
  grant->nof_ru  = n_ru_table_npusch[dci->i_ru];
  grant->nof_rep = n_rep_table_npusch[dci->i_rep];
  if (grant->nof_rep != 1) {
    printf("NPUSCH repetitions are currently not supported!\n");
    return SRSLTE_ERROR;
  }

  /// Compute number of slots according to Table Table 10.1.2.3-1 in 36.211
  switch (grant->format) {
    case SRSLTE_NPUSCH_FORMAT1:
      if (grant->nof_sc == 1) {
        grant->nof_slots = 16;
      } else if (grant->nof_sc == 3) {
        grant->nof_slots = 8;
      } else if (grant->nof_sc == 6) {
        grant->nof_slots = 4;
      } else if (grant->nof_sc == 12) {
        grant->nof_slots = 2;
      } else {
        DEBUG("Unsupported value for N_sc_RU=%d\n", grant->nof_sc);
      }
      break;
    case SRSLTE_NPUSCH_FORMAT2:
      grant->nof_slots = 4;
      break;
    default:
      fprintf(stderr, "Invalid NPUSCH format.\n");
      return SRSLTE_ERROR;
  }
  grant->nof_slots *= grant->nof_ru;

  /// set TBS and Qm (according to table Table 10.1.3.2-1)
  if (grant->nof_sc == 1) {
    assert(dci->i_mcs < 11);
    grant->Qm = (dci->i_mcs <= 1) ? 1 : 2;
    i_tbs     = i_mcs_to_i_tbs_npusch[dci->i_mcs];
  } else if (grant->nof_sc > 1) {
    assert(dci->i_mcs <= 12);
    grant->Qm = 2;
    i_tbs     = dci->i_mcs;
  }
  tbs = tbs_table_npusch[i_tbs][dci->i_ru];

  /// Redundency version according to TS 36 212 Section 6.3.2
  /// And TS 36 213 Sec. 16.5.1.2
  /// TODO: implement NPUSCH repetitions
  /// int L = (grant->nof_sc == 1) ? 1 : min(4, grant->nof_rep/2);
  /// int B = L * grant->nof_ru * grant->nof_slots;
  int j         = 0;
  grant->rv_idx = 2 * ((dci->i_rv + j) % 2);

  /// set Tx TTI
  assert(dci->i_delay <= 3);
  grant->k0     = k0_table_formatN0[dci->i_delay];
  grant->tx_tti = (rx_tti + grant->k0 + 1) % 10240;

  /// set fixed values
  grant->sc_spacing  = spacing;
  grant->mcs.mcs_idx = dci->i_mcs;
  grant->mcs.mod     = (grant->Qm == 1) ? SRSLTE_MOD_BPSK : SRSLTE_MOD_QPSK;

  if (tbs < 0) {
    return SRSLTE_ERROR;
  } else {
    grant->mcs.tbs = (uint32_t)tbs;
    return SRSLTE_SUCCESS;
  }
}

void srslte_ra_nbiot_ul_grant_to_nbits(srslte_ra_nbiot_ul_grant_t* grant, srslte_ra_nbits_t* nbits)
{
  /// set DMRS symbols according to TS 36.211 v13.3 Table 10.1.4.2-1
  int num_dmrs_syms = (grant->format == SRSLTE_NPUSCH_FORMAT1) ? 1 : 3;

  /// computer number of RUs
  nbits->nof_symb = 7;
  nbits->nof_re   = (nbits->nof_symb - num_dmrs_syms) * grant->nof_slots *
                  grant->nof_sc; // Here, a RE is a Resource Unit (RU) which is N_symb_UL*N_slots_UL*N_sc_RU, one symbol
                                 // per slot is used for DMRS
  nbits->nof_bits = nbits->nof_re * grant->Qm;
}

float srslte_ra_nbiot_get_delta_f(srslte_npusch_sc_spacing_t spacing)
{
  return ((spacing == SRSLTE_NPUSCH_SC_SPACING_15000) ? 15000 : 3750);
}
