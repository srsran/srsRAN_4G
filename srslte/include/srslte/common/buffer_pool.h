/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
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

#ifndef BUFFER_POOL_H
#define BUFFER_POOL_H

#include <pthread.h>
#include <vector>
#include <stack>
#include <algorithm>

/*******************************************************************************
                              INCLUDES
*******************************************************************************/

#include "common/common.h"

namespace srslte {

/******************************************************************************
 * Buffer pool
 *
 * Preallocates a large number of buffer_t and provides allocate and
 * deallocate functions. Provides quick object creation and deletion as well
 * as object reuse. 
 * Singleton class of byte_buffer_t (but other pools of different type can be created)
 *****************************************************************************/

template <class buffer_t>
class buffer_pool{
public:
  
  // non-static methods
  buffer_pool(uint32_t nof_buffers = POOL_SIZE)
  {
    pthread_mutex_init(&mutex, NULL);
    for(int i=0;i<nof_buffers;i++) {
      buffer_t *b = new buffer_t;
      available.push(b);
    }
    capacity = nof_buffers; 
  }

  ~buffer_pool() { 
    // this destructor assumes all buffers have been properly deallocated 
    while(available.size()) {
      delete available.top();
      available.pop();
    }
  }
  
  
  buffer_t* allocate()
  {
    pthread_mutex_lock(&mutex);
    buffer_t* b = NULL;

    if(available.size() > 0)
    {
      b = available.top();
      used.push_back(b);
      available.pop();
      
      if (available.size() < capacity/20) {
        printf("Warning buffer pool capacity is %f %%\n", (float) available.size()/capacity);
      }
      
    } else {
      printf("Error - buffer pool is empty\n");
    }

    pthread_mutex_unlock(&mutex);
    return b;
  }
  
  bool deallocate(buffer_t *b)
  {
    bool ret = false; 
    pthread_mutex_lock(&mutex);
    typename std::vector<buffer_t*>::iterator elem = std::find(used.begin(), used.end(), b);
    if (elem != used.end()) {
      used.erase(elem); 
      available.push(b);
      ret = true; 
    } else {
      printf("Error deallocating from buffer pool: buffer not created in this pool.\n");
    }
    pthread_mutex_unlock(&mutex);
    return ret; 
  }

  
private:  
  static const int       POOL_SIZE = 2048;
  std::stack<buffer_t*>  available;
  std::vector<buffer_t*> used; 
  pthread_mutex_t        mutex;  
  uint32_t capacity;
};


class byte_buffer_pool {
public: 
  // Singleton static methods
  static byte_buffer_pool   *instance;  
  static byte_buffer_pool*   get_instance(void);
  static void                cleanup(void); 
  byte_buffer_pool() {
    pool = new buffer_pool<byte_buffer_t>;
  }
  ~byte_buffer_pool() {
    delete pool; 
  }
  byte_buffer_t* allocate() {
    return pool->allocate();
  }
  void deallocate(byte_buffer_t *b) {
    b->reset();
    pool->deallocate(b);
  }
private:
  buffer_pool<byte_buffer_t> *pool; 
};


} // namespace srsue

#endif // BUFFER_POOL_H
