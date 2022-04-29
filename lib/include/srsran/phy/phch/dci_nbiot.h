/**
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
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

#ifndef SRSRAN_DCI_NBIOT_H
#define SRSRAN_DCI_NBIOT_H

#include <stdint.h>

#include "srsran/config.h"
#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/phch/dci.h"
#include "srsran/phy/phch/ra_nbiot.h"

#define SRSRAN_DCI_MAX_BITS 128
#define SRSRAN_NBIOT_RAR_GRANT_LEN 15

SRSRAN_API void srsran_nbiot_dci_rar_grant_unpack(srsran_nbiot_dci_rar_grant_t* rar,
                                                  const uint8_t                 grant[SRSRAN_NBIOT_RAR_GRANT_LEN]);

SRSRAN_API int srsran_nbiot_dci_msg_to_dl_grant(const srsran_dci_msg_t*     msg,
                                                const uint16_t              msg_rnti,
                                                srsran_ra_nbiot_dl_dci_t*   dl_dci,
                                                srsran_ra_nbiot_dl_grant_t* grant,
                                                const uint32_t              sfn,
                                                const uint32_t              sf_idx,
                                                const uint32_t              r_max,
                                                const srsran_nbiot_mode_t   mode);

SRSRAN_API int srsran_nbiot_dci_msg_to_ul_grant(const srsran_dci_msg_t*          msg,
                                                srsran_ra_nbiot_ul_dci_t*        ul_dci,
                                                srsran_ra_nbiot_ul_grant_t*      grant,
                                                const uint32_t                   rx_tti,
                                                const srsran_npusch_sc_spacing_t spacing);

SRSRAN_API int
srsran_nbiot_dci_rar_to_ul_grant(srsran_nbiot_dci_rar_grant_t* rar, srsran_ra_nbiot_ul_grant_t* grant, uint32_t rx_tti);

SRSRAN_API bool srsran_nbiot_dci_location_isvalid(const srsran_dci_location_t* c);

SRSRAN_API int srsran_dci_msg_pack_npdsch(const srsran_ra_nbiot_dl_dci_t* data,
                                          const srsran_dci_format_t       format,
                                          srsran_dci_msg_t*               msg,
                                          const bool                      crc_is_crnti);

SRSRAN_API int
srsran_dci_msg_unpack_npdsch(const srsran_dci_msg_t* msg, srsran_ra_nbiot_dl_dci_t* data, const bool crc_is_crnti);

SRSRAN_API int srsran_dci_msg_unpack_npusch(const srsran_dci_msg_t* msg, srsran_ra_nbiot_ul_dci_t* data);

SRSRAN_API uint32_t srsran_dci_nbiot_format_sizeof(srsran_dci_format_t format);

#endif // SRSRAN_DCI_NBIOT_H
