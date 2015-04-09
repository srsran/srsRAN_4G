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

#include <pthread.h>
#include "srslte/ue_itf/tti_sync.h"
#include "srslte/srslte.h"

#ifndef TTISYNC_CV_H
#define TTISYNC_CV_H


namespace srslte {
namespace ue {
  
  /* Implements tti_sync interface with condition variables. 
   */
  
class SRSLTE_API tti_sync_cv : public tti_sync
{
  public: 
             tti_sync_cv(uint32_t modulus);
            ~tti_sync_cv();
    void     increase();
    uint32_t wait();      
    void     set_producer_cntr(uint32_t producer_cntr);
    
  private: 
    pthread_cond_t  cond; 
    pthread_mutex_t mutex; 
}; 
}
}

#endif
