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

#include <math.h>
#include <string.h>
#include <srslte/srslte.h>

#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/phch/prach.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"

float save_corr[4096];

//PRACH detection threshold is PRACH_DETECT_FACTOR*average
#define PRACH_DETECT_FACTOR 18

#define   N_SEQS        64    // Number of prach sequences available
#define   N_RB_SC       12    // Number of subcarriers per resource block
#define   DELTA_F       15000 // Normal subcarrier spacing
#define   DELTA_F_RA    1250  // PRACH subcarrier spacing
#define   DELTA_F_RA_4  7500  // PRACH subcarrier spacing for format 4
#define   PHI           7     // PRACH phi parameter
#define   PHI_4         2     // PRACH phi parameter for format 4
#define   MAX_ROOTS     838   // Max number of root sequences

#define PRACH_AMP       1.0

/******************************************************
 * Reference tables from 3GPP TS 36.211 v10.7.0
 *****************************************************/

// Table 5.7.1-1 T_cp for preamble formats
uint32_t prach_Tcp[5] = {3168, 21024, 6240, 21024, 448};

// Table 5.7.1-1 T_seq for preamble formats
uint32_t prach_Tseq[5] = {24576, 24576, 2 * 24576, 2 * 24576, 4096};

// Table 5.7.2-2 - N_cs values for unrestricted sets
uint32_t prach_Ncs_unrestricted[16] = {0, 13, 15, 18, 22, 26, 32, 38, 46, 59, 76, 93, 119, 167, 279, 419};

#define MAX_N_zc 839

// Table 5.7.2-2 - N_cs values for restricted sets
uint32_t prach_Ncs_restricted[15] = {15, 18, 22, 26, 32, 38, 46, 55, 68, 82, 100, 128, 158, 202, 237};

// Table 5.7.2-3 - N_cs values for preamble format 4
uint32_t prach_Ncs_format4[7] = {2, 4, 6, 8, 10, 12, 15};

// Table 5.7.2-4 - Root ZC sequence order
uint32_t prach_zc_roots[838] = {
    129, 710, 140, 699, 120, 719, 210, 629, 168, 671, 84, 755,
    105, 734, 93, 746, 70, 769, 60, 779, 2, 837, 1, 838,
    56, 783, 112, 727, 148, 691, 80, 759, 42, 797, 40, 799,
    35, 804, 73, 766, 146, 693, 31, 808, 28, 811, 30, 809,
    27, 812, 29, 810, 24, 815, 48, 791, 68, 771, 74, 765,
    178, 661, 136, 703, 86, 753, 78, 761, 43, 796, 39, 800,
    20, 819, 21, 818, 95, 744, 202, 637, 190, 649, 181, 658,
    137, 702, 125, 714, 151, 688, 217, 622, 128, 711, 142, 697,
    122, 717, 203, 636, 118, 721, 110, 729, 89, 750, 103, 736,
    61, 778, 55, 784, 15, 824, 14, 825, 12, 827, 23, 816,
    34, 805, 37, 802, 46, 793, 207, 632, 179, 660, 145, 694,
    130, 709, 223, 616, 228, 611, 227, 612, 132, 707, 133, 706,
    143, 696, 135, 704, 161, 678, 201, 638, 173, 666, 106, 733,
    83, 756, 91, 748, 66, 773, 53, 786, 10, 829, 9, 830,
    7, 832, 8, 831, 16, 823, 47, 792, 64, 775, 57, 782,
    104, 735, 101, 738, 108, 731, 208, 631, 184, 655, 197, 642,
    191, 648, 121, 718, 141, 698, 149, 690, 216, 623, 218, 621,
    152, 687, 144, 695, 134, 705, 138, 701, 199, 640, 162, 677,
    176, 663, 119, 720, 158, 681, 164, 675, 174, 665, 171, 668,
    170, 669, 87, 752, 169, 670, 88, 751, 107, 732, 81, 758,
    82, 757, 100, 739, 98, 741, 71, 768, 59, 780, 65, 774,
    50, 789, 49, 790, 26, 813, 17, 822, 13, 826, 6, 833,
    5, 834, 33, 806, 51, 788, 75, 764, 99, 740, 96, 743,
    97, 742, 166, 673, 172, 667, 175, 664, 187, 652, 163, 676,
    185, 654, 200, 639, 114, 725, 189, 650, 115, 724, 194, 645,
    195, 644, 192, 647, 182, 657, 157, 682, 156, 683, 211, 628,
    154, 685, 123, 716, 139, 700, 212, 627, 153, 686, 213, 626,
    215, 624, 150, 689, 225, 614, 224, 615, 221, 618, 220, 619,
    127, 712, 147, 692, 124, 715, 193, 646, 205, 634, 206, 633,
    116, 723, 160, 679, 186, 653, 167, 672, 79, 760, 85, 754,
    77, 762, 92, 747, 58, 781, 62, 777, 69, 770, 54, 785,
    36, 803, 32, 807, 25, 814, 18, 821, 11, 828, 4, 835,
    3, 836, 19, 820, 22, 817, 41, 798, 38, 801, 44, 795,
    52, 787, 45, 794, 63, 776, 67, 772, 72, 767, 76, 763,
    94, 745, 102, 737, 90, 749, 109, 730, 165, 674, 111, 728,
    209, 630, 204, 635, 117, 722, 188, 651, 159, 680, 198, 641,
    113, 726, 183, 656, 180, 659, 177, 662, 196, 643, 155, 684,
    214, 625, 126, 713, 131, 708, 219, 620, 222, 617, 226, 613,
    230, 609, 232, 607, 262, 577, 252, 587, 418, 421, 416, 423,
    413, 426, 411, 428, 376, 463, 395, 444, 283, 556, 285, 554,
    379, 460, 390, 449, 363, 476, 384, 455, 388, 451, 386, 453,
    361, 478, 387, 452, 360, 479, 310, 529, 354, 485, 328, 511,
    315, 524, 337, 502, 349, 490, 335, 504, 324, 515, 323, 516,
    320, 519, 334, 505, 359, 480, 295, 544, 385, 454, 292, 547,
    291, 548, 381, 458, 399, 440, 380, 459, 397, 442, 369, 470,
    377, 462, 410, 429, 407, 432, 281, 558, 414, 425, 247, 592,
    277, 562, 271, 568, 272, 567, 264, 575, 259, 580, 237, 602,
    239, 600, 244, 595, 243, 596, 275, 564, 278, 561, 250, 589,
    246, 593, 417, 422, 248, 591, 394, 445, 393, 446, 370, 469,
    365, 474, 300, 539, 299, 540, 364, 475, 362, 477, 298, 541,
    312, 527, 313, 526, 314, 525, 353, 486, 352, 487, 343, 496,
    327, 512, 350, 489, 326, 513, 319, 520, 332, 507, 333, 506,
    348, 491, 347, 492, 322, 517, 330, 509, 338, 501, 341, 498,
    340, 499, 342, 497, 301, 538, 366, 473, 401, 438, 371, 468,
    408, 431, 375, 464, 249, 590, 269, 570, 238, 601, 234, 605,
    257, 582, 273, 566, 255, 584, 254, 585, 245, 594, 251, 588,
    412, 427, 372, 467, 282, 557, 403, 436, 396, 443, 392, 447,
    391, 448, 382, 457, 389, 450, 294, 545, 297, 542, 311, 528,
    344, 495, 345, 494, 318, 521, 331, 508, 325, 514, 321, 518,
    346, 493, 339, 500, 351, 488, 306, 533, 289, 550, 400, 439,
    378, 461, 374, 465, 415, 424, 270, 569, 241, 598, 231, 608,
    260, 579, 268, 571, 276, 563, 409, 430, 398, 441, 290, 549,
    304, 535, 308, 531, 358, 481, 316, 523, 293, 546, 288, 551,
    284, 555, 368, 471, 253, 586, 256, 583, 263, 576, 242, 597,
    274, 565, 402, 437, 383, 456, 357, 482, 329, 510, 317, 522,
    307, 532, 286, 553, 287, 552, 266, 573, 261, 578, 236, 603,
    303, 536, 356, 483, 355, 484, 405, 434, 404, 435, 406, 433,
    235, 604, 267, 572, 302, 537, 309, 530, 265, 574, 233, 606,
    367, 472, 296, 543, 336, 503, 305, 534, 373, 466, 280, 559,
    279, 560, 419, 420, 240, 599, 258, 581, 229, 610};

// Table 5.7.2-5 - Root ZC sequence order for preamble format 4
uint32_t prach_zc_roots_format4[138] = {
    1, 138, 2, 137, 3, 136, 4, 135, 5, 134, 6, 133,
    7, 132, 8, 131, 9, 130, 10, 129, 11, 128, 12, 127,
    13, 126, 14, 125, 15, 124, 16, 123, 17, 122, 18, 121,
    19, 120, 20, 119, 21, 118, 22, 117, 23, 116, 24, 115,
    25, 114, 26, 113, 27, 112, 28, 111, 29, 110, 30, 109,
    31, 108, 32, 107, 33, 106, 34, 105, 35, 104, 36, 103,
    37, 102, 38, 101, 39, 100, 40, 99, 41, 98, 42, 97,
    43, 96, 44, 95, 45, 94, 46, 93, 47, 92, 48, 91,
    49, 90, 50, 89, 51, 88, 52, 87, 53, 86, 54, 85,
    55, 84, 56, 83, 57, 82, 58, 81, 59, 80, 60, 79,
    61, 78, 62, 77, 63, 76, 64, 75, 65, 74, 66, 73,
    67, 72, 68, 71, 69, 70};

srslte_prach_sf_config_t prach_sf_config[16] = {
    {1, {1, 0, 0, 0, 0}},
    {1, {4, 0, 0, 0, 0}},
    {1, {7, 0, 0, 0, 0}},
    {1, {1, 0, 0, 0, 0}},
    {1, {4, 0, 0, 0, 0}},
    {1, {7, 0, 0, 0, 0}},
    {2, {1, 6, 0, 0, 0}},
    {2, {2, 7, 0, 0, 0}},
    {2, {3, 8, 0, 0, 0}},
    {3, {1, 4, 7, 0, 0}},
    {3, {2, 5, 8, 0, 0}},
    {3, {3, 6, 9, 0, 0}},
    {5, {0, 2, 4, 6, 8}},
    {5, {1, 3, 5, 7, 9}},
    {-1, {0, 0, 0, 0, 0}}, // this means all subframes
    {1, {9, 0, 0, 0, 0}}};

uint32_t srslte_prach_get_preamble_format(uint32_t config_idx) {
  return config_idx / 16;
}

srslte_prach_sfn_t srslte_prach_get_sfn(uint32_t config_idx) {
  if ((config_idx % 16) < 3 || (config_idx % 16) == 15) {
    return SRSLTE_PRACH_SFN_EVEN;
  } else {
    return SRSLTE_PRACH_SFN_ANY;
  }
}

/* Returns true if current_tti is a valid opportunity for PRACH transmission and the is an allowed subframe, 
 * or allowed_subframe == -1
 */
bool srslte_prach_tti_opportunity(srslte_prach_t *p, uint32_t current_tti, int allowed_subframe) {
  uint32_t config_idx = p->config_idx;
  return srslte_prach_tti_opportunity_config(config_idx,current_tti,allowed_subframe);
}

bool srslte_prach_tti_opportunity_config(uint32_t config_idx, uint32_t current_tti, int allowed_subframe)
{
  // Get SFN and sf_idx from the PRACH configuration index
  srslte_prach_sfn_t prach_sfn = srslte_prach_get_sfn(config_idx);

  // This is the only option which provides always an opportunity for PRACH transmission.
  if (config_idx == 14) {
    return true;
  }

  if ((prach_sfn == SRSLTE_PRACH_SFN_EVEN && ((current_tti / 10) % 2) == 0) ||
      prach_sfn == SRSLTE_PRACH_SFN_ANY) {
    srslte_prach_sf_config_t sf_config;
    srslte_prach_sf_config(config_idx, &sf_config);
    for (int i = 0; i < sf_config.nof_sf; i++) {
      if (((current_tti % 10) == sf_config.sf[i] && allowed_subframe == -1) ||
          ((current_tti % 10) == sf_config.sf[i] && (current_tti % 10) == allowed_subframe)) {
        return true;
      }
    }
  }
  return false;

}

void srslte_prach_sf_config(uint32_t config_idx, srslte_prach_sf_config_t *sf_config) {
  memcpy(sf_config, &prach_sf_config[config_idx % 16], sizeof(srslte_prach_sf_config_t));
}

// For debug use only
void print(void *d, uint32_t size, uint32_t len, char *file_str) {
  FILE *f;
  f = fopen(file_str, "wb");
  fwrite(d, size, len, f);
  fclose(f);
}

int srslte_prach_gen_seqs(srslte_prach_t *p) {
  uint32_t u = 0;
  uint32_t v = 1;
  int v_max = 0;
  uint32_t p_ = 0;
  uint32_t d_u = 0;
  uint32_t d_start = 0;
  uint32_t N_shift = 0;
  int N_neg_shift = 0;
  uint32_t N_group = 0;
  uint32_t C_v = 0;
  cf_t root[839];

  // Generate our 64 preamble sequences
  for (int i = 0; i < N_SEQS; i++) {

    if (v > v_max) {
      // Get a new root sequence
      if (4 == p->f) {
        u = prach_zc_roots_format4[(p->rsi + p->N_roots) % 138];
      } else {
        u = prach_zc_roots[(p->rsi + p->N_roots) % 838];
      }

      for (int j = 0; j < p->N_zc; j++) {
        double phase = -M_PI * u * j * (j + 1) / p->N_zc;
        root[j] = cexp(phase * I);
      }
      p->root_seqs_idx[p->N_roots++] = i;

      // Determine v_max
      if (p->hs) {
        // High-speed cell
        for (p_ = 1; p_ <= p->N_zc; p_++) {
          if (((p_ * u) % p->N_zc) == 1)
            break;
        }
        if (p_ < p->N_zc / 2) {
          d_u = p_;
        } else {
          d_u = p->N_zc - p_;
        }
        if (d_u >= p->N_cs && d_u < p->N_zc / 3) {
          N_shift = d_u / p->N_cs;
          d_start = 2 * d_u + N_shift * p->N_cs;
          N_group = p->N_zc / d_start;
          if (p->N_zc > 2 * d_u + N_group * d_start) {
            N_neg_shift = (p->N_zc - 2 * d_u - N_group * d_start) / p->N_cs;
          } else {
            N_neg_shift = 0;
          }
        } else if (p->N_zc / 3 <= d_u && d_u <= (p->N_zc - p->N_cs) / 2) {
          N_shift = (p->N_zc - 2 * d_u) / p->N_cs;
          d_start = p->N_zc - 2 * d_u + N_shift * p->N_cs;
          N_group = d_u / d_start;
          if (d_u > N_group * d_start) {
            N_neg_shift = (d_u - N_group * d_start) / p->N_cs;
          } else {
            N_neg_shift = 0;
          }
          if (N_neg_shift > N_shift)
            N_neg_shift = N_shift;
        } else {
          N_shift = 0;
        }
        v_max = N_shift * N_group + N_neg_shift - 1;
        if (v_max < 0) {
          v_max = 0;
        }
      } else {
        // Normal cell
        if (0 == p->N_cs) {
          v_max = 0;
        } else {
          v_max = (p->N_zc / p->N_cs) - 1;
        }
      }

      v = 0;
    }

    // Shift root and add to set
    if (p->hs) {
      if (N_shift == 0) {
        C_v = 0;
      } else {
        C_v = d_start * floor(v / N_shift) + (v % N_shift) * p->N_cs;
      }
    } else {
      C_v = v * p->N_cs;
    }
    for (int j = 0; j < p->N_zc; j++) {
      p->seqs[i][j] = root[(j + C_v) % p->N_zc];
    }

    v++;
  }
  return 0;
}

int srslte_prach_init_cfg(srslte_prach_t *p, srslte_prach_cfg_t *cfg, uint32_t nof_prb) {
  if (srslte_prach_init(p, srslte_symbol_sz(nof_prb))) {
    return -1;
  }
  return srslte_prach_set_cell(p,
                               srslte_symbol_sz(nof_prb),
                               cfg->config_idx,
                               cfg->root_seq_idx,
                               cfg->hs_flag,
                               cfg->zero_corr_zone);
}

int srslte_prach_init(srslte_prach_t *p, uint32_t max_N_ifft_ul) {
  int ret = SRSLTE_ERROR;
  if (p != NULL &&
      max_N_ifft_ul < 2049) {
    bzero(p, sizeof(srslte_prach_t));

    p->max_N_ifft_ul = max_N_ifft_ul;

    // Set up containers
    p->prach_bins = srslte_vec_malloc(sizeof(cf_t) * MAX_N_zc);
    p->corr_spec = srslte_vec_malloc(sizeof(cf_t) * MAX_N_zc);
    p->corr = srslte_vec_malloc(sizeof(float) * MAX_N_zc);

    // Set up ZC FFTS
    if (srslte_dft_plan(&p->zc_fft, MAX_N_zc, SRSLTE_DFT_FORWARD, SRSLTE_DFT_COMPLEX)) {
      return SRSLTE_ERROR;
    }
    srslte_dft_plan_set_mirror(&p->zc_fft, false);
    srslte_dft_plan_set_norm(&p->zc_fft, true);

    if (srslte_dft_plan(&p->zc_ifft, MAX_N_zc, SRSLTE_DFT_BACKWARD, SRSLTE_DFT_COMPLEX)) {
      return SRSLTE_ERROR;
    }
    srslte_dft_plan_set_mirror(&p->zc_ifft, false);
    srslte_dft_plan_set_norm(&p->zc_ifft, false);

    uint32_t fft_size_alloc = max_N_ifft_ul * DELTA_F / DELTA_F_RA;

    p->ifft_in = (cf_t *) srslte_vec_malloc(fft_size_alloc * sizeof(cf_t));
    p->ifft_out = (cf_t *) srslte_vec_malloc(fft_size_alloc * sizeof(cf_t));
    if (srslte_dft_plan(&p->ifft, fft_size_alloc, SRSLTE_DFT_BACKWARD, SRSLTE_DFT_COMPLEX)) {
      fprintf(stderr, "Error creating DFT plan\n");
      return -1;
    }
    srslte_dft_plan_set_mirror(&p->ifft, true);
    srslte_dft_plan_set_norm(&p->ifft, true);

    if (srslte_dft_plan(&p->fft, fft_size_alloc, SRSLTE_DFT_FORWARD, SRSLTE_DFT_COMPLEX)) {
      fprintf(stderr, "Error creating DFT plan\n");
      return -1;
    }

    p->signal_fft = srslte_vec_malloc(sizeof(cf_t) * fft_size_alloc);
    if (!p->signal_fft) {
      fprintf(stderr, "Error allocating memory\n");
      return -1;
    }

    srslte_dft_plan_set_mirror(&p->fft, true);
    srslte_dft_plan_set_norm(&p->fft, false);

    ret = SRSLTE_SUCCESS;
  } else {
    fprintf(stderr, "Invalid parameters\n");
  }

  return ret;
}

int srslte_prach_set_cell(srslte_prach_t *p,
                          uint32_t N_ifft_ul,
                          uint32_t config_idx,
                          uint32_t root_seq_index,
                          bool high_speed_flag,
                          uint32_t zero_corr_zone_config) {
  int ret = SRSLTE_ERROR;
  if (p != NULL &&
      N_ifft_ul < 2049 &&
      config_idx < 64 &&
      root_seq_index < MAX_ROOTS) {
    if (N_ifft_ul > p->max_N_ifft_ul) {
      fprintf(stderr, "PRACH: Error in set_cell(): N_ifft_ul must be lower or equal max_N_ifft_ul in init()\n");
      return -1;
    }

    uint32_t preamble_format = srslte_prach_get_preamble_format(config_idx);
    p->config_idx = config_idx;
    p->f = preamble_format;
    p->rsi = root_seq_index;
    p->hs = high_speed_flag;
    p->zczc = zero_corr_zone_config;
    p->detect_factor = PRACH_DETECT_FACTOR;


    // Determine N_zc and N_cs
    if (4 == preamble_format) {
      if (p->zczc < 7) {
        p->N_zc = 139;
        p->N_cs = prach_Ncs_format4[p->zczc];
      } else {
        fprintf(stderr, "Invalid zeroCorrelationZoneConfig=%d for format4\n", p->zczc);
        return SRSLTE_ERROR;
      }
    } else {
      p->N_zc = MAX_N_zc;
      if (p->hs) {
        if (p->zczc < 15) {
          p->N_cs = prach_Ncs_restricted[p->zczc];
        } else {
          fprintf(stderr, "Invalid zeroCorrelationZoneConfig=%d for restricted set\n", p->zczc);
          return SRSLTE_ERROR;
        }
      } else {
        if (p->zczc < 16) {
          p->N_cs = prach_Ncs_unrestricted[p->zczc];
        } else {
          fprintf(stderr, "Invalid zeroCorrelationZoneConfig=%d\n", p->zczc);
          return SRSLTE_ERROR;
        }
      }
    }

    // Set up ZC FFTS
    if (p->N_zc != MAX_N_zc) {
      if (srslte_dft_replan(&p->zc_fft, p->N_zc)) {
        return SRSLTE_ERROR;
      }
      if (srslte_dft_replan(&p->zc_ifft, p->N_zc)) {
        return SRSLTE_ERROR;
      }
    }

    // Generate our 64 sequences
    p->N_roots = 0;
    srslte_prach_gen_seqs(p);

    // Generate sequence FFTs
    for (int i = 0; i < N_SEQS; i++) {
      srslte_dft_run(&p->zc_fft, p->seqs[i], p->dft_seqs[i]);
    }

    // Create our FFT objects and buffers
    p->N_ifft_ul = N_ifft_ul;
    if (4 == preamble_format) {
      p->N_ifft_prach = p->N_ifft_ul * DELTA_F / DELTA_F_RA_4;
    } else {
      p->N_ifft_prach = p->N_ifft_ul * DELTA_F / DELTA_F_RA;
    }

    /* The deadzone specifies the number of samples at the end of the correlation window
     * that will be considered as belonging to the next preamble
     */
    p->deadzone = 0;
    /*
    if(p->N_cs != 0) {
      float samp_rate=15000*p->N_ifft_ul;
      p->deadzone = (uint32_t) ceil((float) samp_rate/((float) p->N_zc*subcarrier_spacing));
    }*/

    if (srslte_dft_replan(&p->ifft, p->N_ifft_prach)) {
      fprintf(stderr, "Error creating DFT plan\n");
      return -1;
    }
    if (srslte_dft_replan(&p->fft, p->N_ifft_prach)) {
      fprintf(stderr, "Error creating DFT plan\n");
      return -1;
    }

    p->N_seq = prach_Tseq[p->f] * p->N_ifft_ul / 2048;
    p->N_cp = prach_Tcp[p->f] * p->N_ifft_ul / 2048;
    p->T_seq = prach_Tseq[p->f] * SRSLTE_LTE_TS;
    p->T_tot = (prach_Tseq[p->f] + prach_Tcp[p->f]) * SRSLTE_LTE_TS;

    ret = SRSLTE_SUCCESS;
  } else {
    fprintf(stderr, "Invalid parameters\n");
  }

  return ret;
}

int srslte_prach_gen(srslte_prach_t *p,
                     uint32_t seq_index,
                     uint32_t freq_offset,
                     cf_t *signal) {
  int ret = SRSLTE_ERROR;
  if (p != NULL &&
      seq_index < N_SEQS &&
      signal != NULL) {
    // Calculate parameters
    uint32_t N_rb_ul = srslte_nof_prb(p->N_ifft_ul);
    uint32_t k_0 = freq_offset * N_RB_SC - N_rb_ul * N_RB_SC / 2 + p->N_ifft_ul / 2;
    uint32_t K = DELTA_F / DELTA_F_RA;
    uint32_t begin = PHI + (K * k_0) + (K / 2);

    if (6 + freq_offset > N_rb_ul) {
      fprintf(stderr, "Error no space for PRACH: frequency offset=%d, N_rb_ul=%d\n", freq_offset, N_rb_ul);
      return ret;
    }

    DEBUG("N_zc: %d, N_cp: %d, N_seq: %d, N_ifft_prach=%d begin: %d\n",
          p->N_zc, p->N_cp, p->N_seq, p->N_ifft_prach, begin);

    // Map dft-precoded sequence to ifft bins
    memset(p->ifft_in, 0, begin * sizeof(cf_t));
    memcpy(&p->ifft_in[begin], p->dft_seqs[seq_index], p->N_zc * sizeof(cf_t));
    memset(&p->ifft_in[begin + p->N_zc], 0, (p->N_ifft_prach - begin - p->N_zc) * sizeof(cf_t));

    srslte_dft_run(&p->ifft, p->ifft_in, p->ifft_out);

    // Copy CP into buffer
    memcpy(signal, &p->ifft_out[p->N_ifft_prach - p->N_cp], p->N_cp * sizeof(cf_t));

    // Copy preamble sequence into buffer
    for (int i = 0; i < p->N_seq; i++) {
      signal[p->N_cp + i] = p->ifft_out[i % p->N_ifft_prach];
    }

    ret = SRSLTE_SUCCESS;
  }

  return ret;
}

void srslte_prach_set_detect_factor(srslte_prach_t *p, float ratio) {
  p->detect_factor = ratio;
}

int srslte_prach_detect(srslte_prach_t *p,
                        uint32_t freq_offset,
                        cf_t *signal,
                        uint32_t sig_len,
                        uint32_t *indices,
                        uint32_t *n_indices) {
  return srslte_prach_detect_offset(p, freq_offset, signal, sig_len, indices, NULL, NULL, n_indices);
}

int srslte_prach_detect_offset(srslte_prach_t *p,
                               uint32_t freq_offset,
                               cf_t *signal,
                               uint32_t sig_len,
                               uint32_t *indices,
                               float *t_offsets,
                               float *peak_to_avg,
                               uint32_t *n_indices) {
  int ret = SRSLTE_ERROR;
  if (p != NULL &&
      signal != NULL &&
      sig_len > 0 &&
      indices != NULL) {

    if (sig_len < p->N_ifft_prach) {
      fprintf(stderr, "srslte_prach_detect: Signal length is %d and should be %d\n", sig_len, p->N_ifft_prach);
      return SRSLTE_ERROR_INVALID_INPUTS;
    }

    // FFT incoming signal
    srslte_dft_run(&p->fft, signal, p->signal_fft);

    *n_indices = 0;

    // Extract bins of interest
    uint32_t N_rb_ul = srslte_nof_prb(p->N_ifft_ul);
    uint32_t k_0 = freq_offset * N_RB_SC - N_rb_ul * N_RB_SC / 2 + p->N_ifft_ul / 2;
    uint32_t K = DELTA_F / DELTA_F_RA;
    uint32_t begin = PHI + (K * k_0) + (K / 2);

    memcpy(p->prach_bins, &p->signal_fft[begin], p->N_zc * sizeof(cf_t));

    for (int i = 0; i < p->N_roots; i++) {
      cf_t *root_spec = p->dft_seqs[p->root_seqs_idx[i]];

      srslte_vec_prod_conj_ccc(p->prach_bins, root_spec, p->corr_spec, p->N_zc);

      srslte_dft_run(&p->zc_ifft, p->corr_spec, p->corr_spec);

      srslte_vec_abs_square_cf(p->corr_spec, p->corr, p->N_zc);

      float corr_ave = srslte_vec_acc_ff(p->corr, p->N_zc) / p->N_zc;

      uint32_t winsize = 0;
      if (p->N_cs != 0) {
        winsize = p->N_cs;
      } else {
        winsize = p->N_zc;
      }
      uint32_t n_wins = p->N_zc / winsize;

      float max_peak = 0;
      for (int j = 0; j < n_wins; j++) {
        uint32_t start = (p->N_zc - (j * p->N_cs)) % p->N_zc;
        uint32_t end = start + winsize;
        if (end > p->deadzone) {
          end -= p->deadzone;
        }
        start += p->deadzone;
        p->peak_values[j] = 0;
        for (int k = start; k < end; k++) {
          if (p->corr[k] > p->peak_values[j]) {
            p->peak_values[j] = p->corr[k];
            p->peak_offsets[j] = k - start;
            if (p->peak_values[j] > max_peak) {
              max_peak = p->peak_values[j];
            }
          }
        }
      }
      if (max_peak > p->detect_factor * corr_ave) {
        for (int j = 0; j < n_wins; j++) {
          if (p->peak_values[j] > p->detect_factor * corr_ave) {
            //printf("saving prach correlation\n");
            //memcpy(save_corr, p->corr, p->N_zc*sizeof(float));
            if (indices) {
              indices[*n_indices] = (i * n_wins) + j;
            }
            if (peak_to_avg) {
              peak_to_avg[*n_indices] = p->peak_values[j] / corr_ave;
            }
            if (t_offsets) {
              t_offsets[*n_indices] = (float) p->peak_offsets[j] * p->T_seq / p->N_zc;
            }
            (*n_indices)++;
          }
        }
      }
    }

    ret = SRSLTE_SUCCESS;
  }
  return ret;
}

int srslte_prach_free(srslte_prach_t *p) {
  free(p->prach_bins);
  free(p->corr_spec);
  free(p->corr);
  srslte_dft_plan_free(&p->ifft);
  free(p->ifft_in);
  free(p->ifft_out);
  srslte_dft_plan_free(&p->fft);
  srslte_dft_plan_free(&p->zc_fft);
  srslte_dft_plan_free(&p->zc_ifft);

  if (p->signal_fft) {
    free(p->signal_fft);
  }

  bzero(p, sizeof(srslte_prach_t));

  return 0;
}

int srslte_prach_print_seqs(srslte_prach_t *p) {
  for (int i = 0; i < N_SEQS; i++) {
    FILE *f;
    char str[32];
    sprintf(str, "prach_seq_%d.bin", i);
    f = fopen(str, "wb");
    fwrite(p->seqs[i], sizeof(cf_t), p->N_zc, f);
    fclose(f);
  }
  for (int i = 0; i < N_SEQS; i++) {
    FILE *f;
    char str[32];
    sprintf(str, "prach_dft_seq_%d.bin", i);
    f = fopen(str, "wb");
    fwrite(p->dft_seqs[i], sizeof(cf_t), p->N_zc, f);
    fclose(f);
  }
  for (int i = 0; i < p->N_roots; i++) {
    FILE *f;
    char str[32];
    sprintf(str, "prach_root_seq_%d.bin", i);
    f = fopen(str, "wb");
    fwrite(p->seqs[p->root_seqs_idx[i]], sizeof(cf_t), p->N_zc, f);
    fclose(f);
  }
  return 0;
}
