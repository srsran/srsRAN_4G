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

/******************************************************************************
 *  File:         dci.h
 *
 *  Description:  Downlink control information (DCI). 
 *                Packing/Unpacking functions to convert between bit streams 
 *                and packed DCI UL/DL grants defined in ra.h
 *
 *  Reference:    3GPP TS 36.212 version 10.0.0 Release 10 Sec. 5.3.3
 *****************************************************************************/

#ifndef SRSLTE_DCI_H
#define SRSLTE_DCI_H

#include <stdint.h>

#include "srslte/config.h"
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/phch/ra.h"

#define SRSLTE_DCI_MAX_BITS  128

#define SRSLTE_RAR_GRANT_LEN 20

SRSLTE_API extern int harq_pid_len; 

typedef enum {
  SRSLTE_DCI_FORMAT0 = 0, 
  SRSLTE_DCI_FORMAT1, 
  SRSLTE_DCI_FORMAT1A, 
  SRSLTE_DCI_FORMAT1C, 
  SRSLTE_DCI_FORMAT1B,
  SRSLTE_DCI_FORMAT1D, 
  SRSLTE_DCI_FORMAT2, 
  SRSLTE_DCI_FORMAT2A, 
  SRSLTE_DCI_FORMAT2B, 
  //SRSLTE_DCI_FORMAT3, 
  //SRSLTE_DCI_FORMAT3A, 
  SRSLTE_DCI_NOF_FORMATS
} srslte_dci_format_t;

// Each type is for a different interface to packing/unpacking functions
typedef struct SRSLTE_API {
  enum {
    SRSLTE_DCI_MSG_TYPE_PUSCH_SCHED, 
    SRSLTE_DCI_MSG_TYPE_PDSCH_SCHED, 
    SRSLTE_DCI_MSG_TYPE_MCCH_CHANGE, 
    SRSLTE_DCI_MSG_TYPE_TPC_COMMAND, 
    SRSLTE_DCI_MSG_TYPE_RA_PROC_PDCCH
  } type;
  srslte_dci_format_t format;
}srslte_dci_msg_type_t;

typedef enum {
  SRSLTE_DCI_SPEC_COMMON_ = 0, 
  SRSLTE_DCI_SPEC_UE = 1
} dci_spec_t;

typedef struct SRSLTE_API {
  uint32_t L;    // Aggregation level
  uint32_t ncce; // Position of first CCE of the dci
} srslte_dci_location_t;

typedef struct SRSLTE_API {
  uint8_t data[SRSLTE_DCI_MAX_BITS];
  uint32_t nof_bits;
  srslte_dci_format_t format; 
} srslte_dci_msg_t;

typedef struct SRSLTE_API {
  uint32_t rba;
  uint32_t trunc_mcs;
  uint32_t tpc_pusch;
  bool ul_delay;
  bool cqi_request; 
  bool hopping_flag; 
} srslte_dci_rar_grant_t;

/* Converts a received PDSCH DL scheduling DCI message 
 * to ra structures ready to be passed to the harq setup function
 */
SRSLTE_API int srslte_dci_msg_to_dl_grant(srslte_dci_msg_t *msg, 
                                          uint16_t msg_rnti,
                                          uint32_t nof_prb, 
                                          uint32_t nof_ports, 
                                          srslte_ra_dl_dci_t *dl_dci, 
                                          srslte_ra_dl_grant_t *grant);

SRSLTE_API int srslte_dci_msg_to_ul_grant(srslte_dci_msg_t *msg, 
                                          uint32_t nof_prb,
                                          uint32_t n_rb_ho, 
                                          srslte_ra_ul_dci_t *ul_dci, 
                                          srslte_ra_ul_grant_t *grant, 
                                          uint32_t harq_pid);

SRSLTE_API int srslte_dci_rar_to_ul_grant(srslte_dci_rar_grant_t *rar,
                                          uint32_t nof_prb, 
                                          uint32_t n_rb_ho, 
                                          srslte_ra_ul_dci_t *ul_dci,
                                          srslte_ra_ul_grant_t *grant); 

SRSLTE_API void srslte_dci_rar_grant_unpack(srslte_dci_rar_grant_t *rar, 
                                            uint8_t grant[SRSLTE_RAR_GRANT_LEN]);

SRSLTE_API void srslte_dci_rar_grant_pack(srslte_dci_rar_grant_t *rar, 
                                          uint8_t grant[SRSLTE_RAR_GRANT_LEN]);

SRSLTE_API void srslte_dci_rar_grant_fprint(FILE *stream, 
                                            srslte_dci_rar_grant_t *rar);

SRSLTE_API srslte_dci_format_t srslte_dci_format_from_string(char *str);

SRSLTE_API char* srslte_dci_format_string(srslte_dci_format_t format);

SRSLTE_API char* srslte_dci_format_string_short(srslte_dci_format_t format);

SRSLTE_API int srslte_dci_location_set(srslte_dci_location_t *c,
                                       uint32_t L, 
                                       uint32_t nCCE);

SRSLTE_API bool srslte_dci_location_isvalid(srslte_dci_location_t *c);

SRSLTE_API int srslte_dci_msg_get_type(srslte_dci_msg_t *msg, 
                                       srslte_dci_msg_type_t *type, 
                                       uint32_t nof_prb, 
                                       uint16_t msg_rnti);

SRSLTE_API void srslte_dci_msg_type_fprint(FILE *f, 
                                           srslte_dci_msg_type_t type);

// For srslte_dci_msg_type_t = SRSLTE_DCI_MSG_TYPE_PUSCH_SCHED
SRSLTE_API int srslte_dci_msg_pack_pusch(srslte_ra_ul_dci_t *data, 
                                         srslte_dci_msg_t *msg, 
                                         uint32_t nof_prb);

SRSLTE_API int srslte_dci_msg_unpack_pusch(srslte_dci_msg_t *msg, 
                                           srslte_ra_ul_dci_t *data, 
                                           uint32_t nof_prb);

// For srslte_dci_msg_type_t = SRSLTE_DCI_MSG_TYPE_PDSCH_SCHED
SRSLTE_API int srslte_dci_msg_pack_pdsch(srslte_ra_dl_dci_t *data, 
                                         srslte_dci_format_t format,
                                         srslte_dci_msg_t *msg, 
                                         uint32_t nof_prb, 
                                         uint32_t nof_ports,
                                         bool crc_is_crnti);

SRSLTE_API int srslte_dci_msg_unpack_pdsch(srslte_dci_msg_t *msg, 
                                           srslte_ra_dl_dci_t *data, 
                                           uint32_t nof_prb, 
                                           uint32_t nof_ports, 
                                           bool crc_is_crnti);

SRSLTE_API uint32_t srslte_dci_format_sizeof(srslte_dci_format_t format, 
                                             uint32_t nof_prb, 
                                             uint32_t nof_ports);

SRSLTE_API uint32_t srslte_dci_dl_info(char *info_str,
                                    uint32_t str_len,
                                    srslte_ra_dl_dci_t *dci_msg,
                                    srslte_dci_format_t format);

SRSLTE_API uint32_t srslte_dci_ul_info(char *info_str,
                                       uint32_t len,
                                       srslte_ra_ul_dci_t *dci_msg);

// This is for backwards compatibility only for tm1 formats
SRSLTE_API uint32_t srslte_dci_format_sizeof_lut(srslte_dci_format_t format, 
                                                 uint32_t nof_prb);

#endif // DCI_
