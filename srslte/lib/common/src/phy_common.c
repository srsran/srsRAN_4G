/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The libLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the libLTE library.
 *
 * libLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */




#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "srslte/common/phy_common.h"

const uint32_t tc_cb_sizes[SRSLTE_NOF_TC_CB_SIZES] = { 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 120,
    128, 136, 144, 152, 160, 168, 176, 184, 192, 200, 208, 216, 224, 232,
    240, 248, 256, 264, 272, 280, 288, 296, 304, 312, 320, 328, 336, 344,
    352, 360, 368, 376, 384, 392, 400, 408, 416, 424, 432, 440, 448, 456,
    464, 472, 480, 488, 496, 504, 512, 528, 544, 560, 576, 592, 608, 624,
    640, 656, 672, 688, 704, 720, 736, 752, 768, 784, 800, 816, 832, 848,
    864, 880, 896, 912, 928, 944, 960, 976, 992, 1008, 1024, 1056, 1088,
    1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472,
    1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856,
    1888, 1920, 1952, 1984, 2016, 2048, 2112, 2176, 2240, 2304, 2368, 2432,
    2496, 2560, 2624, 2688, 2752, 2816, 2880, 2944, 3008, 3072, 3136, 3200,
    3264, 3328, 3392, 3456, 3520, 3584, 3648, 3712, 3776, 3840, 3904, 3968,
    4032, 4096, 4160, 4224, 4288, 4352, 4416, 4480, 4544, 4608, 4672, 4736,
    4800, 4864, 4928, 4992, 5056, 5120, 5184, 5248, 5312, 5376, 5440, 5504,
    5568, 5632, 5696, 5760, 5824, 5888, 5952, 6016, 6080, 6144 };

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

void srslte_cell_fprint(FILE *stream, srslte_cell_t *cell) {
  fprintf(stream, "PCI: %d, CP: %s, PRB: %d, Ports: %d\n", cell->id, srslte_cp_string(cell->cp), cell->nof_prb, cell->nof_ports);
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


/*
 * Returns Turbo coder interleaver size for Table 5.1.3-3 (36.212) index
 */
int srslte_cb_size(uint32_t index) {
  if (index < SRSLTE_NOF_TC_CB_SIZES) {
    return (int) tc_cb_sizes[index];
  } else {
    return SRSLTE_ERROR;
  }
}

bool srslte_cb_size_isvalid(uint32_t size) {
  for (int i=0;i<SRSLTE_NOF_TC_CB_SIZES;i++) {
    if (tc_cb_sizes[i] == size) {
      return true; 
    }
  }
  return false; 
}

char *srslte_mod_string(srslte_mod_t mod) {
  switch (mod) {
  case LTE_BPSK:
    return "BPSK";
  case LTE_QPSK:
    return "QPSK";
  case LTE_QAM16:
    return "QAM16";
  case LTE_QAM64:
    return "QAM64";
  default:
    return "N/A";
  } 
}

uint32_t srslte_mod_bits_x_symbol(srslte_mod_t mod) {
  switch (mod) {
  case LTE_BPSK:
    return 1;
  case LTE_QPSK:
    return 2;
  case LTE_QAM16:
    return 4;
  case LTE_QAM64:
    return 6;
  default:
    return 0;
  }   
}

char *srslte_cp_string(srslte_cp_t cp) {
  if (cp == SRSLTE_SRSLTE_CP_NORM) {
    return "Normal";
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
uint32_t srssrslte_N_ta_new_rar(uint32_t ta) {
  if (ta > 1282) {
    ta = 1282; 
  }
  return ta*16; 
}

/*
 * Finds index of minimum K>=long_cb in Table 5.1.3-3 of 36.212
 */
int srslte_find_cb_index(uint32_t long_cb) {
  int j = 0;
  while (j < SRSLTE_NOF_TC_CB_SIZES && tc_cb_sizes[j] < long_cb) {
    j++;
  }

  if (j == SRSLTE_NOF_TC_CB_SIZES) {
    return SRSLTE_ERROR;
  } else {
    return j;
  }
}

int srslte_sampling_freq_hz(uint32_t nof_prb) {
    int n = srslte_symbol_sz(nof_prb); 
    if (n == -1) {
      return SRSLTE_ERROR;
    } else {
      return 15000 * n;
    }
}

int srslte_symbol_sz(uint32_t nof_prb) {
  if (nof_prb<=0) {
    return SRSLTE_ERROR;
  }
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
  }
  return SRSLTE_ERROR;
}

bool srssrslte_symbol_sz_isvalid(uint32_t symbol_sz) {
  if (symbol_sz == 128  || 
      symbol_sz == 256  ||
      symbol_sz == 512  ||
      symbol_sz == 1024 ||
      symbol_sz == 2048) {
    return true;
  } else {
    return false; 
  }
}

uint32_t srslte_voffset(uint32_t symbol_id, uint32_t cell_id, uint32_t nof_ports) {
  if (nof_ports == 1 && symbol_id==0) {
    return (cell_id+3) % 6;
  } else {
    return cell_id % 6;
  }
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
    {1, 2110, 0, 599, ALL},
    {2, 1930, 600, 1199, NAR},
    {3, 1805, 1200, 1949, ALL},
    {4, 2110, 1950, 2399, NAR},
    {5, 869, 2400, 2649, NAR},
    {6, 875, 2650, 2749, APAC},
    {7, 2620, 2750, 3449, EMEA},
    {8, 925, 3450, 3799, ALL},
    {9, 1844.9, 3800, 4149, APAC},
    {10, 2110, 4150, 4749, NAR},
    {11, 1475.9, 4750, 4949, JAPAN},
    {12, 729, 5010, 5179, NAR},
    {13, 746, 5180, 5279, NAR},
    {14, 758, 5280, 5379, NAR},
    {17, 734, 5730, 5849, NAR},
    {18, 860, 5850, 5999, JAPAN},
    {19, 875, 6000, 6149, JAPAN},
    {20, 791, 6150, 6449, EMEA},
    {21, 1495.9, 6450, 6599, JAPAN},
    {22, 3500, 6600, 7399, NA},
    {23, 2180, 7500, 7699, NAR},
    {24, 1525, 7700, 8039, NAR},
    {25, 1930, 8040, 8689, NAR},
    {26, 859, 8690, 9039, NAR},
    {27, 852, 9040, 9209, NAR},
    {28, 758, 9210, 9659, APAC},
    {29, 717, 9660, 9769, NAR},
    {30, 2350, 9770, 9869, NAR},
    {31, 462.5, 9870, 9919, CALA}
};
#define EOF_BAND 9919

int srslte_str2mimotype(char *mimo_type_str, srslte_mimo_type_t *type) {
  if (!strcmp(mimo_type_str, "single")) {
    *type = SINGLE_ANTENNA;
  } else if (!strcmp(mimo_type_str, "diversity")) {
    *type = TX_DIVERSITY;
  } else if (!strcmp(mimo_type_str, "multiplex")) {
    *type = SPATIAL_MULTIPLEX;
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

