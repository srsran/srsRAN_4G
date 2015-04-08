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

#include <stdio.h>
#include <stdlib.h>
#include "srslte/ue_itf/queue.h"

namespace srslte {
namespace ue {
  queue::queue(uint32_t nof_elements_, uint32_t element_size)
  {
    nof_elements = nof_elements_; 
    buffer_of_elements = (queue::element**) malloc(sizeof(queue::element*) * nof_elements);
    for (int i=0;i<nof_elements;i++) {
      buffer_of_elements[i] = (queue::element*) malloc(element_size); 
    }
  }

  queue::~queue()
  {
    printf("destroying %d elements\n", nof_elements);
    for (int i=0;i<nof_elements;i++) {
      if (buffer_of_elements[i]) {
        free(buffer_of_elements[i]);
      }
    }
    if (buffer_of_elements) {
      free(buffer_of_elements);      
    }
  }  
  
  queue::element* queue::get(uint32_t idx)
  {
    return (queue::element*) buffer_of_elements[idx%nof_elements];
  }
  
} // namespace ue
} // namespace srslte

