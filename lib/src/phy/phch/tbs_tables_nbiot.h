/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSRAN_TBS_TABLES_NBIOT_H
#define SRSRAN_TBS_TABLES_NBIOT_H

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

#endif // SRSRAN_TBS_TABLES_NBIOT_H