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
#include <stdlib.h>

#include "srslte/srslte.h"

#ifndef PARAMS_H
#define PARAMS_H

namespace srslte {
namespace ue {
  class SRSLTE_API params_db
  {
  public: 
    params_db(uint32_t nof_params_) {
      nof_params = nof_params_; 
      db = (int64_t*) calloc(sizeof(int64_t), nof_params); 
    }
   ~params_db() {
      if (db) {
        free(db);
      }
    }
    void    set_param(uint32_t param_idx, int64_t value) {
      if (param_idx < nof_params) {
        db[param_idx] = value;       
      }
    }
    int64_t get_param(uint32_t param_idx) {
      if (param_idx < nof_params) {
        return db[param_idx]; 
      } else {
        return -1; 
      }
    }
    
  private:
    uint32_t nof_params; 
    int64_t *db;
  };
}
}

#endif