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



static uint32_t dci_sz_table[101][4] = {
  {15, 13, 15, 5},
  {15, 17, 15, 5},
  {17, 15, 17, 5},
  {18, 17, 18, 5},
  {19, 17, 19, 7},
  {19, 18, 19, 7},
  {21, 19, 21, 8},
  {21, 22, 21, 8},
  {21, 22, 21, 9},
  {21, 22, 21, 9},
  {21, 23, 21, 9},
  {22, 21, 22, 9},
  {22, 21, 22, 9},
  {22, 21, 22, 9},
  {22, 21, 22, 10},
  {22, 23, 22, 10},
  {23, 22, 23, 11},
  {23, 25, 23, 11},
  {23, 25, 23, 11},
  {23, 25, 23, 11},
  {23, 25, 23, 11},
  {23, 25, 23, 11},
  {23, 25, 23, 11},
  {25, 27, 25, 12},
  {25, 27, 25, 12},
  {25, 27, 25, 12},
  {25, 27, 25, 12},
  {25, 23, 25, 11},
  {25, 27, 25, 11},
  {25, 27, 25, 12},
  {25, 27, 25, 12},
  {25, 27, 25, 12},
  {25, 27, 25, 12},
  {25, 27, 25, 12},
  {25, 27, 25, 13},
  {25, 27, 25, 13},
  {25, 27, 25, 13},
  {25, 27, 25, 13},
  {25, 27, 25, 13},
  {25, 27, 25, 13},
  {25, 28, 25, 13},
  {25, 28, 25, 13},
  {25, 28, 25, 13},
  {25, 29, 25, 13},
  {25, 29, 25, 13},
  {27, 29, 27, 13},
  {27, 30, 27, 13},
  {27, 30, 27, 13},
  {27, 30, 27, 13},
  {27, 31, 27, 13},
  {27, 31, 27, 13},
  {27, 31, 27, 13},
  {27, 33, 27, 13},
  {27, 33, 27, 13},
  {27, 33, 27, 13},
  {27, 33, 27, 13},
  {27, 33, 27, 13},
  {27, 33, 27, 13},
  {27, 34, 27, 13},
  {27, 34, 27, 13},
  {27, 34, 27, 13},
  {27, 35, 27, 13},
  {27, 35, 27, 13},
  {27, 35, 27, 13},
  {27, 30, 27, 14},
  {27, 31, 27, 14},
  {27, 31, 27, 14},
  {27, 31, 27, 14},
  {27, 31, 27, 14},
  {27, 33, 27, 14},
  {27, 33, 27, 14},
  {27, 33, 27, 14},
  {27, 33, 27, 14},
  {27, 33, 27, 14},
  {27, 33, 27, 14},
  {27, 33, 27, 14},
  {27, 33, 27, 14},
  {27, 34, 27, 14},
  {27, 34, 27, 14},
  {27, 34, 27, 14},
  {27, 34, 27, 14},
  {27, 35, 27, 14},
  {27, 35, 27, 14},
  {27, 35, 27, 14},
  {27, 35, 27, 14},
  {27, 36, 27, 14},
  {27, 36, 27, 14},
  {27, 36, 27, 14},
  {27, 36, 27, 14},
  {27, 37, 27, 14},
  {27, 37, 27, 14},
  {28, 37, 28, 14},
  {28, 37, 28, 14},
  {28, 38, 28, 14},
  {28, 38, 28, 15},
  {28, 38, 28, 15},
  {28, 38, 28, 15},
  {28, 39, 28, 15},
  {28, 39, 28, 15},
  {28, 39, 28, 15},
  {28, 39, 28, 15}
};

