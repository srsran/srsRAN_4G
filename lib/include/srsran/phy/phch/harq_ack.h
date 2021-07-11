/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#ifndef SRSRAN_HARQ_ACK_H
#define SRSRAN_HARQ_ACK_H

#include "srsran/phy/phch/dci_nr.h"
#include "srsran/phy/phch/harq_ack_cfg.h"
#include "srsran/phy/phch/uci_cfg_nr.h"

SRSRAN_API int srsran_harq_ack_resource(const srsran_harq_ack_cfg_hl_t* cfg,
                                        const srsran_dci_dl_nr_t*       dci_dl,
                                        srsran_harq_ack_resource_t*     pdsch_ack_resource);

SRSRAN_API int srsran_harq_ack_gen_uci_cfg(const srsran_harq_ack_cfg_hl_t* cfg,
                                           const srsran_pdsch_ack_nr_t*    ack_info,
                                           srsran_uci_cfg_nr_t*            uci_cfg);

SRSRAN_API int srsran_harq_ack_pack(const srsran_harq_ack_cfg_hl_t* cfg,
                                    const srsran_pdsch_ack_nr_t*    ack_info,
                                    srsran_uci_data_nr_t*           uci_data);

SRSRAN_API int srsran_harq_ack_unpack(const srsran_harq_ack_cfg_hl_t* cfg,
                                      const srsran_uci_data_nr_t*     uci_data,
                                      srsran_pdsch_ack_nr_t*          ack_info);

SRSRAN_API int srsran_harq_ack_insert_m(srsran_pdsch_ack_nr_t* ack_info, const srsran_harq_ack_m_t* m);

SRSRAN_API uint32_t srsran_harq_ack_info(const srsran_pdsch_ack_nr_t* ack_info, char* str, uint32_t str_len);

#endif // SRSRAN_HARQ_ACK_H
