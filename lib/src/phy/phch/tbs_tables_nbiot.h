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

#ifndef SRSLTE_TBS_TABLES_NBIOT_H
#define SRSLTE_TBS_TABLES_NBIOT_H

// Transport Block Size from 3GPP TS 36.213 v13.2.0 table 16.4.1.5.1-1
const int tbs_table_nbiot[13][8] = {{16, 32, 56, 88, 120, 152, 208, 256},
                                    {24, 56, 88, 144, 176, 208, 256, 344},
                                    {32, 72, 144, 176, 208, 256, 328, 424},
                                    {40, 104, 176, 208, 256, 328, 440, 568},
                                    {56, 120, 208, 256, 328, 408, 552, 680},
                                    {72, 144, 224, 328, 424, 504, 680, 0},
                                    {88, 176, 256, 392, 504, 600, 0, 0},
                                    {104, 224, 328, 472, 584, 680, 0, 0},
                                    {120, 256, 392, 536, 680, 0, 0, 0},
                                    {136, 296, 456, 616, 0, 0, 0, 0},
                                    {144, 328, 504, 680, 0, 0, 0, 0},
                                    {176, 376, 584, 0, 0, 0, 0, 0},
                                    {208, 440, 680, 0, 0, 0, 0, 0}};

// Transport Block Size for NPDSCH carrying SystemInformationBlockType1-NB
// from 3GPP TS 36.213 v13.2.0 table 16.4.1.5.2-1
const int tbs_table_nbiot_sib1[16] = {208, 208, 208, 328, 328, 328, 440, 440, 440, 680, 680, 680, 0, 0, 0, 0};

// Transport Block Size for NPUSCH
// from 3GPP TS 36.213 v13.2.0 table 16.5.1.2-2
const int tbs_table_npusch[13][8] = {{16, 32, 56, 88, 120, 152, 208, 256},
                                     {24, 56, 88, 144, 176, 208, 256, 344},
                                     {32, 72, 144, 176, 208, 256, 328, 424},
                                     {40, 104, 176, 208, 256, 328, 440, 568},
                                     {56, 120, 208, 256, 328, 408, 552, 680},
                                     {72, 144, 224, 328, 424, 504, 680, 872},
                                     {88, 176, 256, 392, 504, 600, 808, 1000},
                                     {104, 224, 328, 472, 584, 712, 1000, 0},
                                     {120, 256, 392, 536, 680, 808, 0, 0},
                                     {136, 296, 456, 616, 776, 936, 0, 0},
                                     {144, 328, 504, 680, 872, 1000, 0, 0},
                                     {176, 376, 584, 776, 1000, 0, 0, 0},
                                     {208, 440, 680, 1000, 0, 0, 0, 0}};

#endif // SRSLTE_TBS_TABLES_NBIOT_H