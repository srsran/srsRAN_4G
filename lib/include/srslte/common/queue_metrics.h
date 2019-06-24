/**
 *
 * \section COPYRIGHT
 *
 * Copyright (c) 2019 - Adjacent Link LLC, Bridgewater, New Jersey
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
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
 * \section COPYRIGHT
 *
 * Copyright 2015 The srsUE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

/******************************************************************************
 *  File:         qmetrics.h
 *  Description:  Generic queue metrics
 *               
 *              
 *****************************************************************************/


#ifndef QUEUE_METRICS_H
#define QUEUE_METRICS_H

#include <sstream>

namespace srslte {

  struct queue_metrics_t {
   int    capacity;
   size_t currsize;
   size_t highwater;
   size_t num_cleared;
   size_t num_push;
   size_t num_push_fail;
   size_t num_pop;
   size_t num_pop_fail;

   queue_metrics_t() { 
     reset();
   }

   void reset() {
    capacity = 0;
    currsize = 0;
    highwater = 0;
    num_cleared = 0;
    num_push = 0;
    num_push_fail = 0;
    num_pop = 0;
    num_pop_fail = 0;
   }

   std::string toString() const
    {
      std::stringstream ss;

      ss << "cs=" 
         << currsize
         << ", hw="
         << highwater
         << ", cap="
         << capacity
         << ", nc="
         << num_cleared
         << ", npu="
         << num_push
         << ", npuf="
         << num_push_fail
         << ", npo="
         << num_pop
         << ", npof="
         << num_pop_fail;

       return ss.str();
    }
 };
}

#endif // QUEUE_METRICS_H
