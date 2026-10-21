// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

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
