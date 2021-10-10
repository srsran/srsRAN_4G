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

#ifndef SRSRAN_CELL_ASN1_CONFIG_H
#define SRSRAN_CELL_ASN1_CONFIG_H

#include "rrc_config_nr.h"
#include "srsran/asn1/rrc_nr.h"

namespace srsenb {

int fill_serv_cell_from_enb_cfg(const rrc_nr_cfg_t& cfg, asn1::rrc_nr::serving_cell_cfg_s& serv_cell);

}

#endif // SRSRAN_CELL_ASN1_CONFIG_H
