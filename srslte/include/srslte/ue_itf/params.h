/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srslte/srslte.h"

#ifndef UEH
#define UEH

namespace srslte {
namespace ue {
  class SRSLTE_API params
  {
  public: 
    typedef enum {
      
      DL_FREQ = 0, 
      UL_FREQ, 

      CELLSEARCH_TIMEOUT_PSS_NFRAMES, 
      CELLSEARCH_TIMEOUT_MIB_NFRAMES, 
      CELLSEARCH_TIMEOUT_PSS_CORRELATION_THRESHOLD, // integer that will be divided by 10 

      PUSCH_BETA, 
      PUSCH_RS_GROUP_HOPPING_EN,
      PUSCH_RS_SEQUENCE_HOPPING_EN,
      PUSCH_RS_CYCLIC_SHIFT,
      PUSCH_RS_GROUP_ASSIGNMENT,
      
      PUSCH_HOPPING_N_SB,
      PUSCH_HOPPING_INTRA_SF,
      PUSCH_HOPPING_OFFSET,

      PUCCH_BETA, 
      PUCCH_DELTA_SHIFT,
      PUCCH_CYCLIC_SHIFT,
      PUCCH_N_RB_2,
      PUCCH_N_PUCCH_1_0,
      PUCCH_N_PUCCH_1_1,
      PUCCH_N_PUCCH_1_2,
      PUCCH_N_PUCCH_1_3,
      PUCCH_N_PUCCH_1,
      PUCCH_N_PUCCH_2,
      PUCCH_N_PUCCH_SR,

      UCI_I_OFFSET_ACK,
      UCI_I_OFFSET_RI,
      UCI_I_OFFSET_CQI,
      
      PRACH_CONFIG_INDEX,
      PRACH_ROOT_SEQ_IDX,
      PRACH_HIGH_SPEED_FLAG,
      PRACH_ZC_CONFIG,
      PRACH_FREQ_OFFSET,
      
      NOF_PARAMS,    
    } param_t;
  
    void    set_param(param_t param, int64_t value); 
    int64_t get_param(param_t param); 
    
  private:
    int64_t params_db[NOF_PARAMS];
  };
}
}

#endif