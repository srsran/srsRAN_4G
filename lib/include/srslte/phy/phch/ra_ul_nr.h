/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSLTE_RA_UL_NR_H
#define SRSLTE_RA_UL_NR_H

#include "srslte/config.h"
#include "srslte/phy/phch/pucch_cfg_nr.h"
#include "uci_cfg_nr.h"

/**
 * @brief Calculates the minimum number of PRB required for transmitting NR-PUCCH Format 2, 3 or 4
 * @remark Based in TS 38.213 9.2.5.1 UE procedure for multiplexing HARQ-ACK or CSI and SR in a PUCCH
 * @return The number of PRB if the provided configuration is valid, SRSLTE_ERROR code otherwise
 */
SRSLTE_API int srslte_ra_ul_nr_pucch_format_2_3_min_prb(const srslte_pucch_nr_resource_t* resource,
                                                        const srslte_uci_cfg_nr_t*        uci_cfg);

#endif // SRSLTE_RA_UL_NR_H
