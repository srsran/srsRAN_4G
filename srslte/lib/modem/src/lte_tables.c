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


#include <stdbool.h>
#include <complex.h>
#include <stdint.h>
#include <math.h>

#include "srslte/modem/modem_table.h"
#include "lte_tables.h"

/**
 * Set the BPSK modulation table */
void set_BPSKtable(cf_t* table)
{
  // LTE-BPSK constellation:
  //    Q
  //    |  0
  //---------> I
  // 1  |
  table[0] = BPSK_LEVEL + BPSK_LEVEL*_Complex_I;
  table[1] = -BPSK_LEVEL -BPSK_LEVEL*_Complex_I;

}

/**
 * Set the QPSK modulation table */
void set_QPSKtable(cf_t* table)
{

  // LTE-QPSK constellation:
  //     Q
  // 10  |  00
  //-----------> I
  // 11  |  01
  table[0] = QPSK_LEVEL + QPSK_LEVEL*_Complex_I;
  table[1] = QPSK_LEVEL - QPSK_LEVEL*_Complex_I;
  table[2] = -QPSK_LEVEL + QPSK_LEVEL*_Complex_I;
  table[3] = -QPSK_LEVEL - QPSK_LEVEL*_Complex_I;
}

/**
 * Set the 16QAM modulation table */
void set_16QAMtable(cf_t* table)
{
  // LTE-16QAM constellation:
  //                Q
  //  1011  1001  |   0001  0011
  //  1010  1000  |   0000  0010
  //---------------------------------> I
  //  1110    1100  |  0100  0110
  //  1111    1101  |  0101  0111
  table[0] = QAM16_LEVEL_1 + QAM16_LEVEL_1*_Complex_I;
  table[1] = QAM16_LEVEL_1 + QAM16_LEVEL_2*_Complex_I;
  table[2] = QAM16_LEVEL_2 + QAM16_LEVEL_1*_Complex_I;
  table[3] = QAM16_LEVEL_2 + QAM16_LEVEL_2*_Complex_I;
  table[4] = QAM16_LEVEL_1 - QAM16_LEVEL_1*_Complex_I;
  table[5] = QAM16_LEVEL_1 - QAM16_LEVEL_2*_Complex_I;
  table[6] = QAM16_LEVEL_2 - QAM16_LEVEL_1*_Complex_I;
  table[7] = QAM16_LEVEL_2 - QAM16_LEVEL_2*_Complex_I;
  table[8] = -QAM16_LEVEL_1 + QAM16_LEVEL_1*_Complex_I;
  table[9] = -QAM16_LEVEL_1 + QAM16_LEVEL_2*_Complex_I;
  table[10] = -QAM16_LEVEL_2 + QAM16_LEVEL_1*_Complex_I;
  table[11] = -QAM16_LEVEL_2 + QAM16_LEVEL_2*_Complex_I;
  table[12] = -QAM16_LEVEL_1 - QAM16_LEVEL_1*_Complex_I;
  table[13] = -QAM16_LEVEL_1 - QAM16_LEVEL_2*_Complex_I;
  table[14] = -QAM16_LEVEL_2 - QAM16_LEVEL_1*_Complex_I;
  table[15] = -QAM16_LEVEL_2 - QAM16_LEVEL_2*_Complex_I;
}

/**
 * Set the 64QAM modulation table */
void set_64QAMtable(cf_t* table)
{
  // LTE-64QAM constellation:
  // see [3GPP TS 36.211 version 10.5.0 Release 10, Section 7.1.4]
  table[0] = QAM64_LEVEL_2 + QAM64_LEVEL_2*_Complex_I;
  table[1] = QAM64_LEVEL_2 + QAM64_LEVEL_1*_Complex_I;
  table[2] = QAM64_LEVEL_1 + QAM64_LEVEL_2*_Complex_I;
  table[3] = QAM64_LEVEL_1 + QAM64_LEVEL_1*_Complex_I;
  table[4] = QAM64_LEVEL_2 + QAM64_LEVEL_3*_Complex_I;
  table[5] = QAM64_LEVEL_2 + QAM64_LEVEL_4*_Complex_I;
  table[6] = QAM64_LEVEL_1 + QAM64_LEVEL_3*_Complex_I;
  table[7] = QAM64_LEVEL_1 + QAM64_LEVEL_4*_Complex_I;
  table[8] = QAM64_LEVEL_3 + QAM64_LEVEL_2*_Complex_I;
  table[9] = QAM64_LEVEL_3 + QAM64_LEVEL_1*_Complex_I;
  table[10] = QAM64_LEVEL_4 + QAM64_LEVEL_2*_Complex_I;
  table[11] = QAM64_LEVEL_4 + QAM64_LEVEL_1*_Complex_I;
  table[12] = QAM64_LEVEL_3 + QAM64_LEVEL_3*_Complex_I;
  table[13] = QAM64_LEVEL_3 + QAM64_LEVEL_4*_Complex_I;
  table[14] = QAM64_LEVEL_4 + QAM64_LEVEL_3*_Complex_I;
  table[15] = QAM64_LEVEL_4 + QAM64_LEVEL_4*_Complex_I;
  table[16] = QAM64_LEVEL_2 - QAM64_LEVEL_2*_Complex_I;
  table[17] = QAM64_LEVEL_2 - QAM64_LEVEL_1*_Complex_I;
  table[18] = QAM64_LEVEL_1 - QAM64_LEVEL_2*_Complex_I;
  table[19] = QAM64_LEVEL_1 - QAM64_LEVEL_1*_Complex_I;
  table[20] = QAM64_LEVEL_2 - QAM64_LEVEL_3*_Complex_I;
  table[21] = QAM64_LEVEL_2 - QAM64_LEVEL_4*_Complex_I;
  table[22] = QAM64_LEVEL_1 - QAM64_LEVEL_3*_Complex_I;
  table[23] = QAM64_LEVEL_1 - QAM64_LEVEL_4*_Complex_I;
  table[24] = QAM64_LEVEL_3 - QAM64_LEVEL_2*_Complex_I;
  table[25] = QAM64_LEVEL_3 - QAM64_LEVEL_1*_Complex_I;
  table[26] = QAM64_LEVEL_4 - QAM64_LEVEL_2*_Complex_I;
  table[27] = QAM64_LEVEL_4 - QAM64_LEVEL_1*_Complex_I;
  table[28] = QAM64_LEVEL_3 - QAM64_LEVEL_3*_Complex_I;
  table[29] = QAM64_LEVEL_3 - QAM64_LEVEL_4*_Complex_I;
  table[30] = QAM64_LEVEL_4 - QAM64_LEVEL_3*_Complex_I;
  table[31] = QAM64_LEVEL_4 - QAM64_LEVEL_4*_Complex_I;
  table[32] = -QAM64_LEVEL_2 + QAM64_LEVEL_2*_Complex_I;
  table[33] = -QAM64_LEVEL_2 + QAM64_LEVEL_1*_Complex_I;
  table[34] = -QAM64_LEVEL_1 + QAM64_LEVEL_2*_Complex_I;
  table[35] = -QAM64_LEVEL_1 + QAM64_LEVEL_1*_Complex_I;
  table[36] = -QAM64_LEVEL_2 + QAM64_LEVEL_3*_Complex_I;
  table[37] = -QAM64_LEVEL_2 + QAM64_LEVEL_4*_Complex_I;
  table[38] = -QAM64_LEVEL_1 + QAM64_LEVEL_3*_Complex_I;
  table[39] = -QAM64_LEVEL_1 + QAM64_LEVEL_4*_Complex_I;
  table[40] = -QAM64_LEVEL_3 + QAM64_LEVEL_2*_Complex_I;
  table[41] = -QAM64_LEVEL_3 + QAM64_LEVEL_1*_Complex_I;
  table[42] = -QAM64_LEVEL_4 + QAM64_LEVEL_2*_Complex_I;
  table[43] = -QAM64_LEVEL_4 + QAM64_LEVEL_1*_Complex_I;
  table[44] = -QAM64_LEVEL_3 + QAM64_LEVEL_3*_Complex_I;
  table[45] = -QAM64_LEVEL_3 + QAM64_LEVEL_4*_Complex_I;
  table[46] = -QAM64_LEVEL_4 + QAM64_LEVEL_3*_Complex_I;
  table[47] = -QAM64_LEVEL_4 + QAM64_LEVEL_4*_Complex_I;
  table[48] = -QAM64_LEVEL_2 - QAM64_LEVEL_2*_Complex_I;
  table[49] = -QAM64_LEVEL_2 - QAM64_LEVEL_1*_Complex_I;
  table[50] = -QAM64_LEVEL_1 - QAM64_LEVEL_2*_Complex_I;
  table[51] = -QAM64_LEVEL_1 - QAM64_LEVEL_1*_Complex_I;
  table[52] = -QAM64_LEVEL_2 - QAM64_LEVEL_3*_Complex_I;
  table[53] = -QAM64_LEVEL_2 - QAM64_LEVEL_4*_Complex_I;
  table[54] = -QAM64_LEVEL_1 - QAM64_LEVEL_3*_Complex_I;
  table[55] = -QAM64_LEVEL_1 - QAM64_LEVEL_4*_Complex_I;
  table[56] = -QAM64_LEVEL_3 - QAM64_LEVEL_2*_Complex_I;
  table[57] = -QAM64_LEVEL_3 - QAM64_LEVEL_1*_Complex_I;
  table[58] = -QAM64_LEVEL_4 - QAM64_LEVEL_2*_Complex_I;
  table[59] = -QAM64_LEVEL_4 - QAM64_LEVEL_1*_Complex_I;
  table[60] = -QAM64_LEVEL_3 - QAM64_LEVEL_3*_Complex_I;
  table[61] = -QAM64_LEVEL_3 - QAM64_LEVEL_4*_Complex_I;
  table[62] = -QAM64_LEVEL_4 - QAM64_LEVEL_3*_Complex_I;
  table[63] = -QAM64_LEVEL_4 - QAM64_LEVEL_4*_Complex_I;
}

