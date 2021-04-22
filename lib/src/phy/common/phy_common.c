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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/common/sequence.h"
#include "srsran/phy/utils/debug.h"

#ifdef FORCE_STANDARD_RATE
static bool use_standard_rates = true;
#else
static bool use_standard_rates = false;
#endif

/* Returns true if the structure pointed by cell has valid parameters
 */

bool srsran_cellid_isvalid(uint32_t cell_id)
{
  if (cell_id < 504) {
    return true;
  } else {
    return false;
  }
}

bool srsran_nofprb_isvalid(uint32_t nof_prb)
{
  if (nof_prb == 1 || (nof_prb >= 6 && nof_prb <= SRSRAN_MAX_PRB)) {
    return true;
  } else {
    return false;
  }
}

bool srsran_cell_isvalid(srsran_cell_t* cell)
{
  return srsran_cellid_isvalid(cell->id) && srsran_portid_isvalid(cell->nof_ports) &&
         srsran_nofprb_isvalid(cell->nof_prb);
}

void srsran_cell_fprint(FILE* stream, srsran_cell_t* cell, uint32_t sfn)
{
  fprintf(stream, " - Type:            %s\n", cell->frame_type == SRSRAN_FDD ? "FDD" : "TDD");
  fprintf(stream, " - PCI:             %d\n", cell->id);
  fprintf(stream, " - Nof ports:       %d\n", cell->nof_ports);
  fprintf(stream, " - CP:              %s\n", srsran_cp_string(cell->cp));
  fprintf(stream, " - PRB:             %d\n", cell->nof_prb);
  fprintf(stream, " - PHICH Length:    %s\n", cell->phich_length == SRSRAN_PHICH_EXT ? "Extended" : "Normal");
  fprintf(stream, " - PHICH Resources: ");
  switch (cell->phich_resources) {
    case SRSRAN_PHICH_R_1_6:
      fprintf(stream, "1/6");
      break;
    case SRSRAN_PHICH_R_1_2:
      fprintf(stream, "1/2");
      break;
    case SRSRAN_PHICH_R_1:
      fprintf(stream, "1");
      break;
    case SRSRAN_PHICH_R_2:
      fprintf(stream, "2");
      break;
  }
  fprintf(stream, "\n");
  fprintf(stream, " - SFN:             %d\n", sfn);
}

// Initialize the matrix below with shorter symbols to improve readability.
#define D SRSRAN_TDD_SF_D
#define U SRSRAN_TDD_SF_U
#define S SRSRAN_TDD_SF_S

// TDD uplink-downlink configurations.
// TS 36.211 v8.9.0 Table 4.2-2.
static const srsran_tdd_sf_t tdd_sf[SRSRAN_MAX_TDD_SF_CONFIGS][SRSRAN_NOF_SF_X_FRAME] = {
    {D, S, U, U, U, D, S, U, U, U},
    {D, S, U, U, D, D, S, U, U, D},
    {D, S, U, D, D, D, S, U, D, D},
    {D, S, U, U, U, D, D, D, D, D},
    {D, S, U, U, D, D, D, D, D, D},
    {D, S, U, D, D, D, D, D, D, D},
    {D, S, U, U, U, D, S, U, U, D}};

#undef D
#undef U
#undef S

srsran_tdd_sf_t srsran_sfidx_tdd_type(srsran_tdd_config_t tdd_config, uint32_t sf_idx)
{
  if (tdd_config.sf_config < SRSRAN_MAX_TDD_SF_CONFIGS && sf_idx < SRSRAN_NOF_SF_X_FRAME && tdd_config.configured) {
    return tdd_sf[tdd_config.sf_config][sf_idx];
  }

  return SRSRAN_TDD_SF_D;
}

uint32_t srsran_sfidx_tdd_nof_dw_slot(srsran_tdd_config_t tdd_config, uint32_t slot, srsran_cp_t cp)
{
  uint32_t n = srsran_sfidx_tdd_nof_dw(tdd_config);
  if (n < SRSRAN_CP_NSYMB(cp)) {
    if (slot == 1) {
      return 0;
    } else {
      return n;
    }
  } else {
    if (slot == 1) {
      return n - SRSRAN_CP_NSYMB(cp);
    } else {
      return SRSRAN_CP_NSYMB(cp);
    }
  }
}

// Number of DwPTS / GP / UpPTS symbols per subframe.
// TS 36.211 v13.13.0 Table 4.2-2.
static const uint32_t tdd_nof_sf_symbols[SRSRAN_MAX_TDD_SS_CONFIGS][3] = {{3, 10, 1},
                                                                          {9, 4, 1},
                                                                          {10, 3, 1},
                                                                          {11, 2, 1},
                                                                          {12, 1, 1},
                                                                          {3, 9, 2},
                                                                          {9, 3, 2},
                                                                          {10, 2, 2},
                                                                          {11, 1, 1},
                                                                          {6, 6, 2}};

uint32_t srsran_sfidx_tdd_nof_dw(srsran_tdd_config_t tdd_config)
{
  if (tdd_config.ss_config < SRSRAN_MAX_TDD_SS_CONFIGS) {
    return tdd_nof_sf_symbols[tdd_config.ss_config][0];
  }

  return 0;
}

uint32_t srsran_sfidx_tdd_nof_gp(srsran_tdd_config_t tdd_config)
{
  if (tdd_config.ss_config < SRSRAN_MAX_TDD_SS_CONFIGS) {
    return tdd_nof_sf_symbols[tdd_config.ss_config][1];
  }

  return 0;
}

uint32_t srsran_sfidx_tdd_nof_up(srsran_tdd_config_t tdd_config)
{
  if (tdd_config.ss_config < SRSRAN_MAX_TDD_SS_CONFIGS) {
    return tdd_nof_sf_symbols[tdd_config.ss_config][2];
  }

  return 0;
}

uint32_t srsran_tdd_nof_harq(srsran_tdd_config_t tdd_config)
{
  static const uint32_t tdd_nof_harq[7] = {7, 4, 2, 3, 2, 1, 6};
  return tdd_nof_harq[tdd_config.sf_config];
}

bool srsran_sfidx_isvalid(uint32_t sf_idx)
{
  if (sf_idx <= SRSRAN_NOF_SF_X_FRAME) {
    return true;
  } else {
    return false;
  }
}

bool srsran_portid_isvalid(uint32_t port_id)
{
  if (port_id <= SRSRAN_MAX_PORTS) {
    return true;
  } else {
    return false;
  }
}

bool srsran_N_id_2_isvalid(uint32_t N_id_2)
{
  if (N_id_2 < 3) {
    return true;
  } else {
    return false;
  }
}

bool srsran_N_id_1_isvalid(uint32_t N_id_1)
{
  if (N_id_1 < 168) {
    return true;
  } else {
    return false;
  }
}

srsran_mod_t srsran_str2mod(const char* str)
{
  char mod_str[7] = {};

  // Convert letters to upper case
  for (uint32_t i = 0; str[i] != '\0' && i < 6; i++) {
    char c = str[i];
    if (c >= 'a' && c <= 'z') {
      c &= (~' ');
    }
    mod_str[i] = c;
  }

  if (!strcmp(mod_str, "QPSK")) {
    return SRSRAN_MOD_QPSK;
  } else if (!strcmp(mod_str, "16QAM")) {
    return SRSRAN_MOD_16QAM;
  } else if (!strcmp(mod_str, "64QAM")) {
    return SRSRAN_MOD_64QAM;
  } else if (!strcmp(mod_str, "256QAM")) {
    return SRSRAN_MOD_256QAM;
  } else {
    return (srsran_mod_t)SRSRAN_ERROR_INVALID_INPUTS;
  }
};

char* srsran_mod_string(srsran_mod_t mod)
{
  switch (mod) {
    case SRSRAN_MOD_BPSK:
      return "BPSK";
    case SRSRAN_MOD_QPSK:
      return "QPSK";
    case SRSRAN_MOD_16QAM:
      return "16QAM";
    case SRSRAN_MOD_64QAM:
      return "64QAM";
    case SRSRAN_MOD_256QAM:
      return "256QAM";
    default:
      return "N/A";
  }
}

uint32_t srsran_mod_bits_x_symbol(srsran_mod_t mod)
{
  switch (mod) {
    case SRSRAN_MOD_BPSK:
      return 1;
    case SRSRAN_MOD_QPSK:
      return 2;
    case SRSRAN_MOD_16QAM:
      return 4;
    case SRSRAN_MOD_64QAM:
      return 6;
    case SRSRAN_MOD_256QAM:
      return 8;
    default:
      return 0;
  }
}

char* srsran_cp_string(srsran_cp_t cp)
{
  if (cp == SRSRAN_CP_NORM) {
    return "Normal  ";
  } else {
    return "Extended";
  }
}

/* Returns the new time advance N_ta_new as specified in Section 4.2.3 of 36.213 */
uint32_t srsran_N_ta_new(uint32_t N_ta_old, uint32_t ta)
{
  ta &= 63;
  int n_ta_new = N_ta_old + ((float)ta - 31) * 16;
  if (n_ta_new < 0) {
    return 0;
  } else {
    if (n_ta_new < 20512) {
      return (uint32_t)n_ta_new;
    } else {
      return 20512;
    }
  }
}

float srsran_coderate(uint32_t tbs, uint32_t nof_re)
{
  return (float)(24 + tbs) / nof_re;
}

/* Returns the new time advance as indicated by the random access response
 * as specified in Section 4.2.3 of 36.213 */
uint32_t srsran_N_ta_new_rar(uint32_t ta)
{
  if (ta > 1282) {
    ta = 1282;
  }
  return ta * 16;
}

void srsran_use_standard_symbol_size(bool enabled)
{
  use_standard_rates = enabled;
}

bool srsran_symbol_size_is_standard()
{
  return use_standard_rates;
}

int srsran_sampling_freq_hz(uint32_t nof_prb)
{
  int n = srsran_symbol_sz(nof_prb);
  if (n == -1) {
    return SRSRAN_ERROR;
  } else {
    return 15000 * n;
  }
}

int srsran_symbol_sz_power2(uint32_t nof_prb)
{
  if (nof_prb <= 6) {
    return 128;
  } else if (nof_prb <= 15) {
    return 256;
  } else if (nof_prb <= 25) {
    return 512;
  } else if (nof_prb <= 50) {
    return 1024;
  } else if (nof_prb <= 75) {
    return 1536;
  } else if (nof_prb <= 110) {
    return 2048;
  } else {
    return -1;
  }
}

int srsran_symbol_sz(uint32_t nof_prb)
{
  if (nof_prb <= 0) {
    return SRSRAN_ERROR;
  }
  if (!use_standard_rates) {
    if (nof_prb <= 6) {
      return 128;
    } else if (nof_prb <= 15) {
      return 256;
    } else if (nof_prb <= 25) {
      return 384;
    } else if (nof_prb <= 50) {
      return 768;
    } else if (nof_prb <= 75) {
      return 1024;
    } else if (nof_prb <= 110) {
      return 1536;
    } else {
      return SRSRAN_ERROR;
    }
  } else {
    return srsran_symbol_sz_power2(nof_prb);
  }
}

int srsran_nof_prb(uint32_t symbol_sz)
{
  if (!use_standard_rates) {
    switch (symbol_sz) {
      case 128:
        return 6;
      case 256:
        return 15;
      case 384:
        return 25;
      case 768:
        return 50;
      case 1024:
        return 75;
      case 1536:
        return 100;
    }
  } else {
    switch (symbol_sz) {
      case 128:
        return 6;
      case 256:
        return 15;
      case 512:
        return 25;
      case 1024:
        return 50;
      case 1536:
        return 75;
      case 2048:
        return 100;
    }
  }
  return SRSRAN_ERROR;
}

bool srsran_symbol_sz_isvalid(uint32_t symbol_sz)
{
  if (!use_standard_rates) {
    if (symbol_sz == 128 || symbol_sz == 256 || symbol_sz == 384 || symbol_sz == 768 || symbol_sz == 1024 ||
        symbol_sz == 1536) {
      return true;
    } else {
      return false;
    }
  } else {
    if (symbol_sz == 128 || symbol_sz == 256 || symbol_sz == 512 || symbol_sz == 1024 || symbol_sz == 1536 ||
        symbol_sz == 2048) {
      return true;
    } else {
      return false;
    }
  }
}

// Returns maximum number of CCE assuming CFI 3 and PHICH 1/6
uint32_t srsran_max_cce(uint32_t nof_prb)
{
  switch (nof_prb) {
    case 6:
      return 6;
    case 15:
      return 12;
    case 25:
      return 21;
    case 50:
      return 43;
    case 75:
      return 65;
    default:
      return 87;
  }
}

uint32_t srsran_voffset(uint32_t symbol_id, uint32_t cell_id, uint32_t nof_ports)
{
  if (nof_ports == 1 && symbol_id == 0) {
    return (cell_id + 3) % 6;
  } else {
    return cell_id % 6;
  }
}

/** Computes sequence-group pattern f_gh according to 5.5.1.3 of 36.211 */
int srsran_group_hopping_f_gh(uint32_t f_gh[SRSRAN_NSLOTS_X_FRAME], uint32_t cell_id)
{
  srsran_sequence_t seq;
  bzero(&seq, sizeof(srsran_sequence_t));

  if (srsran_sequence_LTE_pr(&seq, 160, cell_id / 30)) {
    return SRSRAN_ERROR;
  }

  for (uint32_t ns = 0; ns < SRSRAN_NSLOTS_X_FRAME; ns++) {
    f_gh[ns] = 0;
    for (int i = 0; i < 8; i++) {
      f_gh[ns] += (((uint32_t)seq.c[8 * ns + i]) << i);
    }
  }

  srsran_sequence_free(&seq);
  return SRSRAN_SUCCESS;
}

/* Returns the number of available RE per PRB */
uint32_t srsran_re_x_prb(uint32_t ns, uint32_t symbol, uint32_t nof_ports, uint32_t nof_symbols)
{
  if (symbol == 0) {
    if (((ns % 2) == 0) || (ns == 1)) {
      return SRSRAN_NRE - 4;
    } else {
      if (nof_ports == 1) {
        return SRSRAN_NRE - 2;
      } else {
        return SRSRAN_NRE - 4;
      }
    }
  } else if (symbol == 1) {
    if (ns == 1) {
      return SRSRAN_NRE - 4;
    } else if (nof_ports == 4) {
      return SRSRAN_NRE - 4;
    } else {
      return SRSRAN_NRE;
    }
  } else if (symbol == nof_symbols - 3) {
    if (nof_ports == 1) {
      return SRSRAN_NRE - 2;
    } else {
      return SRSRAN_NRE - 4;
    }
  } else {
    return SRSRAN_NRE;
  }
}

struct lte_band {
  uint8_t                     band;
  double                      fd_low_mhz;
  uint32_t                    dl_earfcn_offset;
  uint32_t                    ul_earfcn_offset;
  double                      duplex_mhz;
  enum band_geographical_area area;
};

struct lte_band lte_bands[SRSRAN_NOF_LTE_BANDS] = {
    {1, 2110, 0, 18000, 190, SRSRAN_BAND_GEO_AREA_ALL},
    {2, 1930, 600, 18600, 80, SRSRAN_BAND_GEO_AREA_NAR},
    {3, 1805, 1200, 19200, 95, SRSRAN_BAND_GEO_AREA_ALL},
    {4, 2110, 1950, 19950, 400, SRSRAN_BAND_GEO_AREA_NAR},
    {5, 869, 2400, 20400, 45, SRSRAN_BAND_GEO_AREA_NAR},
    {6, 875, 2650, 20650, 45, SRSRAN_BAND_GEO_AREA_APAC},
    {7, 2620, 2750, 20750, 120, SRSRAN_BAND_GEO_AREA_EMEA},
    {8, 925, 3450, 21450, 45, SRSRAN_BAND_GEO_AREA_ALL},
    {9, 1844.9, 3800, 21800, 95, SRSRAN_BAND_GEO_AREA_APAC},
    {10, 2110, 4150, 22150, 400, SRSRAN_BAND_GEO_AREA_NAR},
    {11, 1475.9, 4750, 22750, 48, SRSRAN_BAND_GEO_AREA_JAPAN},
    {12, 729, 5010, 23010, 30, SRSRAN_BAND_GEO_AREA_NAR},
    {13, 746, 5180, 23180, -31, SRSRAN_BAND_GEO_AREA_NAR},
    {14, 758, 5280, 23280, -30, SRSRAN_BAND_GEO_AREA_NAR},
    {17, 734, 5730, 23730, 30, SRSRAN_BAND_GEO_AREA_NAR},
    {18, 860, 5850, 23850, 45, SRSRAN_BAND_GEO_AREA_JAPAN},
    {19, 875, 6000, 24000, 45, SRSRAN_BAND_GEO_AREA_JAPAN},
    {20, 791, 6150, 24150, -41, SRSRAN_BAND_GEO_AREA_EMEA},
    {21, 1495.9, 6450, 24450, 48, SRSRAN_BAND_GEO_AREA_JAPAN},
    {22, 3500, 6600, 24600, 100, SRSRAN_BAND_GEO_AREA_NA},
    {23, 2180, 7500, 25500, 180, SRSRAN_BAND_GEO_AREA_NAR},
    {24, 1525, 7700, 25700, -101.5, SRSRAN_BAND_GEO_AREA_NAR},
    {25, 1930, 8040, 26040, 80, SRSRAN_BAND_GEO_AREA_NAR},
    {26, 859, 8690, 26690, 45, SRSRAN_BAND_GEO_AREA_NAR},
    {27, 852, 9040, 27040, 45, SRSRAN_BAND_GEO_AREA_NAR},
    {28, 758, 9210, 27210, 55, SRSRAN_BAND_GEO_AREA_APAC},
    {29, 717, 9660, 0, 0, SRSRAN_BAND_GEO_AREA_NAR},
    {30, 2350, 9770, 27660, 45, SRSRAN_BAND_GEO_AREA_NAR},
    {31, 462.5, 9870, 27760, 10, SRSRAN_BAND_GEO_AREA_CALA},
    {32, 1452, 9920, 0, 0, SRSRAN_BAND_GEO_AREA_EMEA},
    {33, 1900, 36000, 0, 0, SRSRAN_BAND_GEO_AREA_EMEA},
    {34, 2010, 36200, 0, 0, SRSRAN_BAND_GEO_AREA_EMEA},
    {35, 1850, 36350, 0, 0, SRSRAN_BAND_GEO_AREA_NAR},
    {36, 1930, 36950, 0, 0, SRSRAN_BAND_GEO_AREA_NAR},
    {37, 1910, 37550, 0, 0, SRSRAN_BAND_GEO_AREA_NAR},
    {38, 2570, 37750, 0, 0, SRSRAN_BAND_GEO_AREA_EMEA},
    {39, 1880, 38250, 0, 0, SRSRAN_BAND_GEO_AREA_APAC},
    {40, 2300, 38650, 0, 0, SRSRAN_BAND_GEO_AREA_APAC},
    {41, 2496, 39650, 0, 0, SRSRAN_BAND_GEO_AREA_ALL},
    {42, 3400, 41590, 0, 0, SRSRAN_BAND_GEO_AREA_ALL},
    {43, 3600, 43590, 0, 0, SRSRAN_BAND_GEO_AREA_ALL},
    {44, 703, 45590, 0, 0, SRSRAN_BAND_GEO_AREA_APAC},
    {45, 1447, 46590, 0, 0, SRSRAN_BAND_GEO_AREA_APAC},
    {46, 5150, 46790, 0, 0, SRSRAN_BAND_GEO_AREA_ALL},
    {47, 5855, 54540, 0, 0, SRSRAN_BAND_GEO_AREA_ALL},
    {48, 3550, 55240, 0, 0, SRSRAN_BAND_GEO_AREA_ALL},
    {49, 3550, 56740, 0, 0, SRSRAN_BAND_GEO_AREA_ALL},
    {50, 1432, 58240, 0, 0, SRSRAN_BAND_GEO_AREA_ALL},
    {51, 1427, 59090, 0, 0, SRSRAN_BAND_GEO_AREA_ALL},
    {52, 3300, 59140, 0, 0, SRSRAN_BAND_GEO_AREA_ALL},
    {64, 0, 60140, 27810, 0, SRSRAN_BAND_GEO_AREA_ALL}, // <- Placeholder band, not defined by 3GPP
    {65, 2110, 65536, 131072, 190, SRSRAN_BAND_GEO_AREA_ALL},
    {66, 2110, 66436, 131972, 400, SRSRAN_BAND_GEO_AREA_NAR},
    {67, 738, 67336, 0, 0, SRSRAN_BAND_GEO_AREA_EMEA},
    {68, 753, 67536, 132672, 55, SRSRAN_BAND_GEO_AREA_EMEA},
    {69, 2570, 67836, 0, 0, SRSRAN_BAND_GEO_AREA_EMEA},
    {70, 1995, 68336, 132972, 300, SRSRAN_BAND_GEO_AREA_NAR},
    {71, 0, 68586, 133122, 0, SRSRAN_BAND_GEO_AREA_NAR} // dummy band to bound band 70 earfcn
};

int srsran_str2mimotype(char* mimo_type_str, srsran_tx_scheme_t* type)
{
  int i = 0;

  /* Low case */
  while (mimo_type_str[i] |= ' ', mimo_type_str[++i])
    ;

  srsran_tx_scheme_t t = SRSRAN_TXSCHEME_PORT0;
  do {
    if (!strcmp(mimo_type_str, srsran_mimotype2str(t))) {
      *type = t;
      return SRSRAN_SUCCESS;
    }
    t++;
  } while (t <= SRSRAN_TXSCHEME_CDD);

  return SRSRAN_ERROR;
}

char* srsran_mimotype2str(srsran_tx_scheme_t mimo_type)
{
  switch (mimo_type) {
    case SRSRAN_TXSCHEME_PORT0:
      return "p0";
    case SRSRAN_TXSCHEME_DIVERSITY:
      return "div";
    case SRSRAN_TXSCHEME_SPATIALMUX:
      return "mux";
    case SRSRAN_TXSCHEME_CDD:
      return "cdd";
    default:
      return "N/A";
  }
}

static double get_fd(struct lte_band* band, uint32_t dl_earfcn)
{
  if (dl_earfcn >= band->dl_earfcn_offset) {
    return band->fd_low_mhz + 0.1 * (dl_earfcn - band->dl_earfcn_offset);
  } else {
    return 0.0;
  }
}

static double get_fu(struct lte_band* band, uint32_t ul_earfcn)
{
  if (ul_earfcn >= band->ul_earfcn_offset) {
    return band->fd_low_mhz - band->duplex_mhz + 0.1 * (ul_earfcn - band->ul_earfcn_offset);
  } else {
    return 0.0;
  }
}

bool srsran_band_is_tdd(uint32_t band)
{
  uint32_t i = 0;
  while (i < SRSRAN_NOF_LTE_BANDS && lte_bands[i].band != band) {
    i++;
  }
  if (i == SRSRAN_NOF_LTE_BANDS) {
    ERROR("Invalid Band %d", band);
    return false;
  }
  return lte_bands[i].ul_earfcn_offset == 0;
}

uint8_t srsran_band_get_band(uint32_t dl_earfcn)
{
  uint32_t i = SRSRAN_NOF_LTE_BANDS - 1;
  if (dl_earfcn > lte_bands[i].dl_earfcn_offset) {
    ERROR("Invalid DL_EARFCN=%d", dl_earfcn);
  }
  i--;
  while (i > 0 && lte_bands[i].dl_earfcn_offset > dl_earfcn) {
    i--;
  }
  return lte_bands[i].band;
}

double srsran_band_fd(uint32_t dl_earfcn)
{
  uint32_t i = SRSRAN_NOF_LTE_BANDS - 1;
  if (dl_earfcn > lte_bands[i].dl_earfcn_offset) {
    ERROR("Invalid DL_EARFCN=%d", dl_earfcn);
  }
  i--;
  while (i > 0 && lte_bands[i].dl_earfcn_offset > dl_earfcn) {
    i--;
  }
  return get_fd(&lte_bands[i], dl_earfcn);
}

double srsran_band_fu(uint32_t ul_earfcn)
{
  uint32_t i = SRSRAN_NOF_LTE_BANDS - 1;
  if (ul_earfcn > lte_bands[i].ul_earfcn_offset) {
    ERROR("Invalid UL_EARFCN=%d", ul_earfcn);
  }
  i--;
  while (i > 0 && (lte_bands[i].ul_earfcn_offset > ul_earfcn || lte_bands[i].ul_earfcn_offset == 0)) {
    i--;
  }
  return get_fu(&lte_bands[i], ul_earfcn);
}

uint32_t srsran_band_ul_earfcn(uint32_t dl_earfcn)
{
  uint32_t i = SRSRAN_NOF_LTE_BANDS - 1;
  if (dl_earfcn > lte_bands[i].dl_earfcn_offset) {
    ERROR("Invalid DL_EARFCN=%d", dl_earfcn);
  }
  i--;
  while (i > 0 && lte_bands[i].dl_earfcn_offset > dl_earfcn) {
    i--;
  }
  return lte_bands[i].ul_earfcn_offset + (dl_earfcn - lte_bands[i].dl_earfcn_offset);
}

int srsran_band_get_fd_band_all(uint32_t band, srsran_earfcn_t* earfcn, uint32_t max_elems)
{
  return srsran_band_get_fd_band(band, earfcn, -1, -1, max_elems);
}

int srsran_band_get_fd_band(uint32_t         band,
                            srsran_earfcn_t* earfcn,
                            int              start_earfcn,
                            int              end_earfcn,
                            uint32_t         max_elems)
{
  uint32_t i, j;
  uint32_t nof_earfcn;
  i = 0;
  while (i < SRSRAN_NOF_LTE_BANDS && lte_bands[i].band != band) {
    i++;
  }
  if (i >= SRSRAN_NOF_LTE_BANDS - 1) {
    ERROR("Error: Invalid band %d", band);
    return SRSRAN_ERROR;
  }
  if (end_earfcn == -1) {
    end_earfcn = lte_bands[i + 1].dl_earfcn_offset - 1;
  } else {
    if (end_earfcn > lte_bands[i + 1].dl_earfcn_offset - 1) {
      ERROR("Error: Invalid end earfcn %d. Max is %d", end_earfcn, lte_bands[i + 1].dl_earfcn_offset - 1);
      return SRSRAN_ERROR;
    }
  }
  if (start_earfcn == -1) {
    start_earfcn = lte_bands[i].dl_earfcn_offset;
  } else {
    if (start_earfcn < lte_bands[i].dl_earfcn_offset) {
      ERROR("Error: Invalid start earfcn %d. Min is %d", start_earfcn, lte_bands[i].dl_earfcn_offset);
      return SRSRAN_ERROR;
    }
  }
  nof_earfcn = end_earfcn - start_earfcn;

  if (nof_earfcn > max_elems) {
    nof_earfcn = max_elems;
  }
  for (j = 0; j < nof_earfcn; j++) {
    earfcn[j].id = j + start_earfcn;
    earfcn[j].fd = get_fd(&lte_bands[i], earfcn[j].id);
  }
  return (int)j;
}

int srsran_band_get_fd_region(enum band_geographical_area region, srsran_earfcn_t* earfcn, uint32_t max_elems)
{
  uint32_t i;
  int      n;
  int      nof_fd = 0;
  for (i = 0; i < SRSRAN_NOF_LTE_BANDS && max_elems > 0; i++) {
    if (lte_bands[i].area == region) {
      n = srsran_band_get_fd_band(i, &earfcn[nof_fd], -1, -1, max_elems);
      if (n != -1) {
        nof_fd += n;
        max_elems -= n;
      } else {
        return SRSRAN_ERROR;
      }
    }
  }
  return nof_fd;
}

/* Returns the interval tti1-tti2 mod 10240 */
uint32_t srsran_tti_interval(uint32_t tti1, uint32_t tti2)
{
  if (tti1 >= tti2) {
    return tti1 - tti2;
  } else {
    return 10240 - tti2 + tti1;
  }
}

uint32_t srsran_print_check(char* s, size_t max_len, uint32_t cur_len, const char* format, ...)
{
  if (cur_len < max_len - 1) {
    va_list args;
    va_start(args, format);
    size_t n   = max_len - cur_len;
    int    ret = vsnprintf(&s[cur_len], n, format, args);
    va_end(args);

    if (ret >= 0 && ret < n) {
      // Notice that only when this returned value is non-negative and less than n, the string has been completely
      // written.
      cur_len += ret;
    } else {
      // Formatting error detected
      ERROR("Formatting error when printing string");
      exit(-1);
    }
  } else {
    ERROR("Buffer full when printing string");
    exit(-1);
  }
  return cur_len;
}

bool srsran_nbiot_prb_isvalid(srsran_nbiot_cell_t* cell)
{
  if (cell->nbiot_prb <= cell->base.nof_prb) {
    return true;
  }
  return false;
}

bool srsran_nbiot_cell_isvalid(srsran_nbiot_cell_t* cell)
{
  return (srsran_cell_isvalid(&cell->base) && srsran_nbiot_portid_isvalid(cell->nof_ports) &&
          srsran_nbiot_prb_isvalid(cell) && srsran_cellid_isvalid(cell->n_id_ncell));
}

bool srsran_nbiot_portid_isvalid(uint32_t port_id)
{
  if (port_id <= SRSRAN_NBIOT_MAX_PORTS) {
    return true;
  } else {
    return false;
  }
}

char* srsran_nbiot_mode_string(srsran_nbiot_mode_t mode)
{
  switch (mode) {
    case SRSRAN_NBIOT_MODE_INBAND_SAME_PCI:
      return "Inband (Same PCI)";
    case SRSRAN_NBIOT_MODE_INBAND_DIFFERENT_PCI:
      return "Inband (Different PCI)";
    case SRSRAN_NBIOT_MODE_GUARDBAND:
      return "Guardband";
    case SRSRAN_NBIOT_MODE_STANDALONE:
      return "Standalone";
    default:
      return "N/A";
  }
}

const char* srsran_ack_nack_feedback_mode_string(srsran_ack_nack_feedback_mode_t ack_nack_feedback_mode)
{
  switch (ack_nack_feedback_mode) {
    case SRSRAN_PUCCH_ACK_NACK_FEEDBACK_MODE_NORMAL:
      return "normal";
    case SRSRAN_PUCCH_ACK_NACK_FEEDBACK_MODE_CS:
      return "cs";
    case SRSRAN_PUCCH_ACK_NACK_FEEDBACK_MODE_PUCCH3:
      return "pucch3";
    case SRSRAN_PUCCH_ACK_NACK_FEEDBACK_MODE_ERROR:
    default:
      return "error";
  }
}

srsran_ack_nack_feedback_mode_t srsran_string_ack_nack_feedback_mode(const char* str)
{
#define MAX_STR_LEN (8)
  int  i                 = 0;
  char str2[MAX_STR_LEN] = {};

  // Copy string in local buffer
  strncpy(str2, str, MAX_STR_LEN - 1);
  str2[MAX_STR_LEN - 1] = '\0';

  // Convert to lower case
  while (str2[i] |= ' ', str2[++i])
    ;

  // Format 1b with channel selection
  if (strcmp(str2, "cs") == 0) {
    return SRSRAN_PUCCH_ACK_NACK_FEEDBACK_MODE_CS;
  }

  // Detect PUCCH3
  if (strcmp(str2, "pucch3") == 0) {
    return SRSRAN_PUCCH_ACK_NACK_FEEDBACK_MODE_PUCCH3;
  }

  // Otherwise Normal
  return SRSRAN_PUCCH_ACK_NACK_FEEDBACK_MODE_NORMAL;
}

uint32_t srsran_ri_nof_bits(const srsran_cell_t* cell)
{
  uint32_t ret = 0;

  if (cell->nof_ports == 2) {
    ret = 1;
  } else if (cell->nof_ports > 2) {
    ret = 2;
  }

  return ret;
}
