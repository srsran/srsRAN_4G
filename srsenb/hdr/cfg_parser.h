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

#ifndef SRSENB_CFG_PARSER_H
#define SRSENB_CFG_PARSER_H

#include "enb.h"

namespace srsenb {
class cfg_parser
{
public:
  void parse_sibs(all_args_t* args, rrc_cfg_t* rrc_cfg, phy_cfg_t* phy_config_common);
};

} // namespace srsenb

#endif // CFG_PARSER_H
