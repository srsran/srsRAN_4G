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
#include "queue.h"

srslte::queue::queue(uint32_t nof_elements, uint32_t element_size)
{
  buffer_of_elements = (queue::element**) malloc(sizeof(queue::element*) * nof_elements);
  for (int i=0;i<nof_elements;i++) {
    buffer_of_elements[i] = (queue::element*) malloc(element_size); 
  }
}

srslte::queue::~queue()
{
  for (int i=0;i<nof_elements;i++) {
    free(buffer_of_elements[i]);
  }
  free(buffer_of_elements);
}  
 
srslte::queue::element* srslte::queue::get(uint32_t idx)
{
 return (queue::element*) buffer_of_elements[idx%nof_elements];
}
