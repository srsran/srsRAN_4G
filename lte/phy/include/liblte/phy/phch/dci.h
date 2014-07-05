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

#ifndef DCI_
#define DCI_

#include <stdint.h>

#include "liblte/config.h"
#include "liblte/phy/common/phy_common.h"
#include "liblte/phy/phch/ra.h"

typedef _Complex float cf_t;

/**
 * DCI message generation according to the formats, as specified in
 * 36.212 Section 5.3.3.1
 *
 */


#define DCI_MAX_BITS  57

typedef enum {
  Format0, Format1, Format1A, Format1C
} dci_format_t;

// Each type is for a different interface to packing/unpacking functions
typedef struct LIBLTE_API {
  enum {
    PUSCH_SCHED, PDSCH_SCHED, MCCH_CHANGE, TPC_COMMAND, RA_PROC_PDCCH
  } type;
  dci_format_t format;
}dci_msg_type_t;

typedef enum {
  DCI_COMMON = 0, DCI_UE = 1
} dci_spec_t;

typedef struct LIBLTE_API {
  uint32_t L;    // Aggregation level
  uint32_t ncce; // Position of first CCE of the dci
} dci_location_t;

typedef struct LIBLTE_API {
  char data[DCI_MAX_BITS];
  uint32_t nof_bits;
} dci_msg_t;


LIBLTE_API char* dci_format_string(dci_format_t format);

LIBLTE_API int dci_location_set(dci_location_t *c, 
                                uint32_t L, 
                                uint32_t nCCE);

LIBLTE_API int dci_msg_get_type(dci_msg_t *msg, 
                                dci_msg_type_t *type, 
                                uint32_t nof_prb, 
                                uint16_t msg_rnti, 
                                uint16_t crnti);

LIBLTE_API void dci_msg_type_fprint(FILE *f, 
                                    dci_msg_type_t type);

// For dci_msg_type_t = PUSCH_SCHED
LIBLTE_API int dci_msg_pack_pusch(ra_pusch_t *data, 
                                  dci_msg_t *msg, 
                                  uint32_t nof_prb);

LIBLTE_API int dci_msg_unpack_pusch(dci_msg_t *msg, 
                                    ra_pusch_t *data, 
                                    uint32_t nof_prb);

// For dci_msg_type_t = PDSCH_SCHED
LIBLTE_API int dci_msg_pack_pdsch(ra_pdsch_t *data, 
                                  dci_msg_t *msg, 
                                  dci_format_t format, 
                                  uint32_t nof_prb, 
                                  bool crc_is_crnti);

LIBLTE_API int dci_msg_unpack_pdsch(dci_msg_t *msg, 
                                    ra_pdsch_t *data, 
                                    uint32_t nof_prb, 
                                    bool crc_is_crnti);

LIBLTE_API uint32_t dci_format_sizeof(dci_format_t format, 
                                 uint32_t nof_prb);

#endif // DCI_
