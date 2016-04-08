/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
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




#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "srslte/common/phy_common.h"
#include "srslte/common/sequence.h"

#ifdef FORCE_STANDARD_RATE
static bool use_standard_rates = true; 
#else 
static bool use_standard_rates = false; 
#endif

/* Returns true if the structure pointed by cell has valid parameters
 */

bool srslte_cellid_isvalid(uint32_t cell_id) {
  if (cell_id < 504) {
    return true;
  } else {
    return false;
  }
}

bool srslte_nofprb_isvalid(uint32_t nof_prb) {
  if (nof_prb >= 6 && nof_prb <= SRSLTE_MAX_PRB) {
    return true;
  } else {
    return false;
  }
}

bool srslte_cell_isvalid(srslte_cell_t *cell) {
  return srslte_cellid_isvalid(cell->id)           && 
         srslte_portid_isvalid(cell->nof_ports)    &&
         srslte_nofprb_isvalid(cell->nof_prb);
}

void srslte_cell_fprint(FILE *stream, srslte_cell_t *cell, uint32_t sfn) {
  fprintf(stream, " - Cell ID:         %d\n", cell->id);
  fprintf(stream, " - Nof ports:       %d\n", cell->nof_ports);
  fprintf(stream, " - CP:              %s\n", srslte_cp_string(cell->cp));
  fprintf(stream, " - PRB:             %d\n", cell->nof_prb);
  fprintf(stream, " - PHICH Length:    %s\n",
         cell->phich_length == SRSLTE_PHICH_EXT ? "Extended" : "Normal");
  fprintf(stream, " - PHICH Resources: ");
  switch (cell->phich_resources) {
  case SRSLTE_PHICH_R_1_6:
    fprintf(stream, "1/6");
    break;
  case SRSLTE_PHICH_R_1_2:
    fprintf(stream, "1/2");
    break;
  case SRSLTE_PHICH_R_1:
    fprintf(stream, "1");
    break;
  case SRSLTE_PHICH_R_2:
    fprintf(stream, "2");
    break;
  }
  fprintf(stream, "\n");
  fprintf(stream, " - SFN:             %d\n", sfn);

}

bool srslte_sfidx_isvalid(uint32_t sf_idx) {
  if (sf_idx <= SRSLTE_NSUBFRAMES_X_FRAME) {
    return true; 
  } else {
    return false; 
  }
}

bool srslte_portid_isvalid(uint32_t port_id) {
  if (port_id <= SRSLTE_MAX_PORTS) {
    return true; 
  } else {
    return false; 
  }
}

bool srslte_N_id_2_isvalid(uint32_t N_id_2) {
  if (N_id_2 < 3) {
    return true;
  } else {
    return false;
  }
}

bool srslte_N_id_1_isvalid(uint32_t N_id_1) {
  if (N_id_1 < 169) {
    return true;
  } else {
    return false;
  }
}

char *srslte_mod_string(srslte_mod_t mod) {
  switch (mod) {
  case SRSLTE_MOD_BPSK:
    return "BPSK";
  case SRSLTE_MOD_QPSK:
    return "QPSK";
  case SRSLTE_MOD_16QAM:
    return "16QAM";
  case SRSLTE_MOD_64QAM:
    return "64QAM";
  default:
    return "N/A";
  } 
}

uint32_t srslte_mod_bits_x_symbol(srslte_mod_t mod) {
  switch (mod) {
  case SRSLTE_MOD_BPSK:
    return 1;
  case SRSLTE_MOD_QPSK:
    return 2;
  case SRSLTE_MOD_16QAM:
    return 4;
  case SRSLTE_MOD_64QAM:
    return 6;
  default:
    return 0;
  }   
}

char *srslte_cp_string(srslte_cp_t cp) {
  if (cp == SRSLTE_CP_NORM) {
    return "Normal  ";
  } else {
    return "Extended";
  }
}

/* Returns the new time advance N_ta_new as specified in Section 4.2.3 of 36.213 */
uint32_t srslte_N_ta_new(uint32_t N_ta_old, uint32_t ta) {
  
  ta &= 63;   
  int n_ta_new = N_ta_old + ((float) ta - 31) * 16;
  if (n_ta_new < 0) {
    return 0; 
  } else {
    if (n_ta_new < 20512) {
      return (uint32_t) n_ta_new;
    } else {
      return 20512; 
    }
  }
}

/* Returns the new time advance as indicated by the random access response 
 * as specified in Section 4.2.3 of 36.213 */
uint32_t srslte_N_ta_new_rar(uint32_t ta) {
  if (ta > 1282) {
    ta = 1282; 
  }
  return ta*16; 
}


void srslte_use_standard_symbol_size(bool enabled) {
  use_standard_rates = enabled;
}

int srslte_sampling_freq_hz(uint32_t nof_prb) {
    int n = srslte_symbol_sz(nof_prb); 
    if (n == -1) {
      return SRSLTE_ERROR;
    } else {
      return 15000 * n;
    }
}

int srslte_symbol_sz_power2(uint32_t nof_prb) {
  if (nof_prb<=6) {
    return 128;
  } else if (nof_prb<=15) {
    return 256;
  } else if (nof_prb<=25) {
    return 512;
  } else if (nof_prb<=50) {
    return 1024;
  } else if (nof_prb<=75) {
    return 1536;
  } else if (nof_prb<=100) {
    return 2048;
  } else {
    return -1;
  }
}

int srslte_symbol_sz(uint32_t nof_prb) {
  if (nof_prb<=0) {
    return SRSLTE_ERROR;
  }
  if (!use_standard_rates) {
    if (nof_prb<=6) {
      return 128;
    } else if (nof_prb<=15) {
      return 256;
    } else if (nof_prb<=25) {
      return 384;
    } else if (nof_prb<=50) {
      return 768;
    } else if (nof_prb<=75) {
      return 1024;
    } else if (nof_prb<=100) {
      return 1536;
    } else {
      return SRSLTE_ERROR;
    }
  } else {
    return srslte_symbol_sz_power2(nof_prb);
  }
}

int srslte_nof_prb(uint32_t symbol_sz)
{
  if (!use_standard_rates) {
    switch(symbol_sz) {
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
    switch(symbol_sz) {
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
  return SRSLTE_ERROR;
}

bool srslte_symbol_sz_isvalid(uint32_t symbol_sz) {
  if (!use_standard_rates) {
    if (symbol_sz == 128  || 
        symbol_sz == 256  ||
        symbol_sz == 384  ||
        symbol_sz == 768  ||
        symbol_sz == 1024 ||
        symbol_sz == 1536) {
      return true;
    } else {
      return false; 
    }
  } else {
    if (symbol_sz == 128  || 
        symbol_sz == 256  ||
        symbol_sz == 512  ||
        symbol_sz == 1024 ||
        symbol_sz == 1536 ||
        symbol_sz == 2048) {
      return true;
    } else {
      return false; 
    }
  }  
}

uint32_t srslte_voffset(uint32_t symbol_id, uint32_t cell_id, uint32_t nof_ports) {
  if (nof_ports == 1 && symbol_id==0) {
    return (cell_id+3) % 6;
  } else {
    return cell_id % 6;
  }
}


/** Computes sequence-group pattern f_gh according to 5.5.1.3 of 36.211 */
int srslte_group_hopping_f_gh(uint32_t f_gh[SRSLTE_NSLOTS_X_FRAME], uint32_t cell_id) {
  srslte_sequence_t seq; 
  bzero(&seq, sizeof(srslte_sequence_t));
  
  if (srslte_sequence_LTE_pr(&seq, 160, cell_id / 30)) {
    return SRSLTE_ERROR;
  }
  
  for (uint32_t ns=0;ns<SRSLTE_NSLOTS_X_FRAME;ns++) {
    f_gh[ns] = 0;
    for (int i = 0; i < 8; i++) {
      f_gh[ns] += (((uint32_t) seq.c[8 * ns + i]) << i);
    }
  }

  srslte_sequence_free(&seq);
  return SRSLTE_SUCCESS;
}

/* Returns the number of available RE per PRB */
uint32_t srslte_re_x_prb(uint32_t ns, uint32_t symbol, uint32_t nof_ports, uint32_t nof_symbols) {
  if (symbol == 0) {
    if (((ns % 2) == 0) || (ns == 1)) {
      return SRSLTE_NRE - 4;
    } else {
      if (nof_ports == 1) {
        return SRSLTE_NRE - 2;
      } else {
        return SRSLTE_NRE - 4;
      }
    }
  } else if (symbol == 1) {
    if (ns == 1) {
      return SRSLTE_NRE - 4;
    } else if (nof_ports == 4) {
      return SRSLTE_NRE - 4;
    } else {
      return SRSLTE_NRE;
    }
  } else if (symbol == nof_symbols - 3) {
    if (nof_ports == 1) {
      return SRSLTE_NRE - 2;
    } else {
      return SRSLTE_NRE - 4;
    }
  } else {
    return SRSLTE_NRE;
  }
}


struct lte_band {
  uint32_t band;
  float fd_low_mhz;
  uint32_t earfcn_offset;
  uint32_t earfcn_max;
  enum band_geographical_area area;
};

struct lte_band lte_bands[SRSLTE_NOF_LTE_BANDS] = {
    {1, 2110, 0, 599, SRSLTE_BAND_GEO_AREA_ALL},
    {2, 1930, 600, 1199, SRSLTE_BAND_GEO_AREA_NAR},
    {3, 1805, 1200, 1949, SRSLTE_BAND_GEO_AREA_ALL},
    {4, 2110, 1950, 2399, SRSLTE_BAND_GEO_AREA_NAR},
    {5, 869, 2400, 2649, SRSLTE_BAND_GEO_AREA_NAR},
    {6, 875, 2650, 2749, SRSLTE_BAND_GEO_AREA_APAC},
    {7, 2620, 2750, 3449, SRSLTE_BAND_GEO_AREA_EMEA},
    {8, 925, 3450, 3799, SRSLTE_BAND_GEO_AREA_ALL},
    {9, 1844.9, 3800, 4149, SRSLTE_BAND_GEO_AREA_APAC},
    {10, 2110, 4150, 4749, SRSLTE_BAND_GEO_AREA_NAR},
    {11, 1475.9, 4750, 4949, SRSLTE_BAND_GEO_AREA_JAPAN},
    {12, 729, 5010, 5179, SRSLTE_BAND_GEO_AREA_NAR},
    {13, 746, 5180, 5279, SRSLTE_BAND_GEO_AREA_NAR},
    {14, 758, 5280, 5379, SRSLTE_BAND_GEO_AREA_NAR},
    {17, 734, 5730, 5849, SRSLTE_BAND_GEO_AREA_NAR},
    {18, 860, 5850, 5999, SRSLTE_BAND_GEO_AREA_JAPAN},
    {19, 875, 6000, 6149, SRSLTE_BAND_GEO_AREA_JAPAN},
    {20, 791, 6150, 6449, SRSLTE_BAND_GEO_AREA_EMEA},
    {21, 1495.9, 6450, 6599, SRSLTE_BAND_GEO_AREA_JAPAN},
    {22, 3500, 6600, 7399, SRSLTE_BAND_GEO_AREA_NA},
    {23, 2180, 7500, 7699, SRSLTE_BAND_GEO_AREA_NAR},
    {24, 1525, 7700, 8039, SRSLTE_BAND_GEO_AREA_NAR},
    {25, 1930, 8040, 8689, SRSLTE_BAND_GEO_AREA_NAR},
    {26, 859, 8690, 9039, SRSLTE_BAND_GEO_AREA_NAR},
    {27, 852, 9040, 9209, SRSLTE_BAND_GEO_AREA_NAR},
    {28, 758, 9210, 9659, SRSLTE_BAND_GEO_AREA_APAC},
    {29, 717, 9660, 9769, SRSLTE_BAND_GEO_AREA_NAR},
    {30, 2350, 9770, 9869, SRSLTE_BAND_GEO_AREA_NAR},
    {31, 462.5, 9870, 9919, SRSLTE_BAND_GEO_AREA_CALA}
};
#define EOF_BAND 9919

int srslte_str2mimotype(char *mimo_type_str, srslte_mimo_type_t *type) {
  if (!strcmp(mimo_type_str, "single")) {
    *type = SRSLTE_MIMO_TYPE_SINGLE_ANTENNA;
  } else if (!strcmp(mimo_type_str, "diversity")) {
    *type = SRSLTE_MIMO_TYPE_TX_DIVERSITY;
  } else if (!strcmp(mimo_type_str, "multiplex")) {
    *type = SRSLTE_MIMO_TYPE_SPATIAL_MULTIPLEX;
  } else {
    return SRSLTE_ERROR;
  }
  return SRSLTE_SUCCESS;
}

float get_fd(struct lte_band *band, uint32_t earfcn) {
  if (earfcn >= band->earfcn_offset) {
    return band->fd_low_mhz + 0.1*(earfcn - band->earfcn_offset);    
  } else {
    return 0.0;
  }
}

float srslte_band_fd(uint32_t earfcn) {
  uint32_t i;
  i=0;
  while(i < SRSLTE_NOF_LTE_BANDS && lte_bands[i].earfcn_offset<earfcn) {
    i++;
  }
  if (i == SRSLTE_NOF_LTE_BANDS) {
    fprintf(stderr, "Error: EARFCN %d not found\n", earfcn);
    return -1.0;
  }
  return get_fd(&lte_bands[i], earfcn);
}

int srslte_band_get_fd_band_all(uint32_t band, srslte_earfcn_t *earfcn, uint32_t max_elems) {
  return srslte_band_get_fd_band(band, earfcn, -1, -1, max_elems);
}

int srslte_band_get_fd_band(uint32_t band, srslte_earfcn_t *earfcn, int start_earfcn, int end_earfcn, uint32_t max_elems) {
  uint32_t i, j;
  uint32_t nof_earfcn;
  i=0;
  while(i < SRSLTE_NOF_LTE_BANDS && lte_bands[i].band != band) {
    i++;
  }
  if (i == SRSLTE_NOF_LTE_BANDS) {
    fprintf(stderr, "Error: Invalid band %d\n", band);
    return SRSLTE_ERROR;
  }
  if (end_earfcn == -1) {
    end_earfcn = lte_bands[i].earfcn_max;
  } else {
    if (end_earfcn > lte_bands[i].earfcn_max) {
      fprintf(stderr, "Error: Invalid end earfcn %d. Max is %d\n", end_earfcn, lte_bands[i].earfcn_max);
      return SRSLTE_ERROR;
    }
  }
  if (start_earfcn == -1) {
    start_earfcn = lte_bands[i].earfcn_offset;
  } else {
    if (start_earfcn < lte_bands[i].earfcn_offset) {
      fprintf(stderr, "Error: Invalid start earfcn %d. Min is %d\n", start_earfcn, lte_bands[i].earfcn_offset);
      return SRSLTE_ERROR;
    }
  }
  nof_earfcn = end_earfcn - start_earfcn;

  if (nof_earfcn > max_elems) {
    nof_earfcn = max_elems;
  }
  for (j=0;j<nof_earfcn;j++) {
    earfcn[j].id = j + start_earfcn;
    earfcn[j].fd = get_fd(&lte_bands[i], earfcn[j].id);
  }
  return (int) j;
}

int srslte_band_get_fd_region(enum band_geographical_area region, srslte_earfcn_t *earfcn, uint32_t max_elems) {
  uint32_t i;
  int n;
  int nof_fd = 0;
  for (i=0;i<SRSLTE_NOF_LTE_BANDS && max_elems > 0;i++) {
    if (lte_bands[i].area == region) {
      n = srslte_band_get_fd_band(i, &earfcn[nof_fd], -1, -1, max_elems);
      if (n != -1) {
        nof_fd += n;
        max_elems -= n;
      } else {
        return SRSLTE_ERROR;
      }
    }
  }
  return nof_fd;
}


/* Returns the interval tti1-tti2 mod 10240 */
uint32_t srslte_tti_interval(uint32_t tti1, uint32_t tti2) {
  if (tti1 > tti2) {
    return tti1-tti2; 
  } else {
    return 10240-tti2+tti1;
  }
}


