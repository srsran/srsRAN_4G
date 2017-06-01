

#ifndef CFG_PARSER_H
#define CFG_PARSER_H

#include "enb.h"

namespace srsenb {
class cfg_parser
{
public:
  void parse_sibs(all_args_t *args, rrc_cfg_t *rrc_cfg, phy_cfg_t *phy_config_common); 
};

}

#endif // CFG_PARSER_H
