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

/**
 *
 * @file dci_nbiot.h
 *
 * @brief Downlink control information (DCI) for NB-IoT.
 *
 * Packing/Unpacking functions to convert between bit streams
 * and packed DCI UL/DL grants defined in ra_nbiot.h
 *
 * Reference: 3GPP TS 36.212 version 13.2.0 Release 13 Sec. 6.4.3
 *
 */

#ifndef SRSLTE_DCI_NBIOT_H
#define SRSLTE_DCI_NBIOT_H

#include <stdint.h>

#include "srslte/config.h"
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/phch/dci.h"
#include "srslte/phy/phch/ra_nbiot.h"

#define SRSLTE_DCI_MAX_BITS 128
#define SRSLTE_NBIOT_RAR_GRANT_LEN 15

SRSLTE_API void srslte_nbiot_dci_rar_grant_unpack(srslte_nbiot_dci_rar_grant_t* rar,
                                                  const uint8_t                 grant[SRSLTE_NBIOT_RAR_GRANT_LEN]);

SRSLTE_API int srslte_nbiot_dci_msg_to_dl_grant(const srslte_dci_msg_t*     msg,
                                                const uint16_t              msg_rnti,
                                                srslte_ra_nbiot_dl_dci_t*   dl_dci,
                                                srslte_ra_nbiot_dl_grant_t* grant,
                                                const uint32_t              sfn,
                                                const uint32_t              sf_idx,
                                                const uint32_t              r_max,
                                                const srslte_nbiot_mode_t   mode);

SRSLTE_API int srslte_nbiot_dci_msg_to_ul_grant(const srslte_dci_msg_t*          msg,
                                                srslte_ra_nbiot_ul_dci_t*        ul_dci,
                                                srslte_ra_nbiot_ul_grant_t*      grant,
                                                const uint32_t                   rx_tti,
                                                const srslte_npusch_sc_spacing_t spacing);

SRSLTE_API int
srslte_nbiot_dci_rar_to_ul_grant(srslte_nbiot_dci_rar_grant_t* rar, srslte_ra_nbiot_ul_grant_t* grant, uint32_t rx_tti);

SRSLTE_API bool srslte_nbiot_dci_location_isvalid(const srslte_dci_location_t* c);

SRSLTE_API int srslte_dci_msg_pack_npdsch(const srslte_ra_nbiot_dl_dci_t* data,
                                          const srslte_dci_format_t       format,
                                          srslte_dci_msg_t*               msg,
                                          const bool                      crc_is_crnti);

SRSLTE_API int
srslte_dci_msg_unpack_npdsch(const srslte_dci_msg_t* msg, srslte_ra_nbiot_dl_dci_t* data, const bool crc_is_crnti);

SRSLTE_API int srslte_dci_msg_unpack_npusch(const srslte_dci_msg_t* msg, srslte_ra_nbiot_ul_dci_t* data);

SRSLTE_API uint32_t srslte_dci_nbiot_format_sizeof(srslte_dci_format_t format);

#endif // SRSLTE_DCI_NBIOT_H
