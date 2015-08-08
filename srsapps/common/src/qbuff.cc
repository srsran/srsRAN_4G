/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
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


#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#include "srsapps/common/qbuff.h"

namespace srslte {
  

qbuff::qbuff()
{
  nof_messages=0; 
  max_msg_size=0;
  wp = 0; 
  rp = 0; 
  buffer = NULL;
  packets = NULL; 
}

qbuff::~qbuff()
{
  free(buffer);
  free(packets);
}

bool qbuff::init(uint32_t nof_messages_, uint32_t max_msg_size_)
{
  nof_messages = nof_messages_; 
  max_msg_size = max_msg_size_; 
  
  buffer  = (uint8_t*) calloc(nof_messages,max_msg_size);
  packets = (pkt_t*)   calloc(nof_messages,sizeof(pkt_t));  
  if (buffer && packets) {
    flush();
    return true; 
  } else {
    return false; 
  }
}

void qbuff::flush()
{
  wp = 0; 
  rp = 0; 
  for (int i=0;i<nof_messages;i++) {
    packets[i].valid = false; 
    packets[i].ptr   = &buffer[i*max_msg_size];
    packets[i].len   = 0; 
  }  
}

bool qbuff::isempty()
{
  return !packets[rp].valid;
}

bool qbuff::isfull()
{
  return packets[wp].valid; 
}


void* qbuff::request()
{
  if (!isfull()) {
    return packets[wp].ptr; 
  } else {
    return NULL; 
  }
}

bool qbuff::push(uint32_t len)
{
  packets[wp].len = len; 
  packets[wp].valid = true; 
  wp += (wp+1 >= nof_messages)?(1-nof_messages):1; 
  return true; 
}

void* qbuff::pop()
{
  return pop(NULL);
}

void* qbuff::pop(uint32_t* len)
{
  if (!isempty()) {
    if (len) {
      *len = packets[rp].len;
    }
    return packets[rp].ptr; 
  } else {
    return NULL; 
  }
}

void* qbuff::pop(uint32_t* len, uint32_t idx)
{
  if (idx == 0) {
    return pop(len);
  } else {
    uint32_t rpp = rp; 
    uint32_t i   = 0; 
    while(i<idx && packets[rpp].valid) {
      rpp += (rpp+1 >= nof_messages)?(1-nof_messages):1; 
      i++;
    }
    if (packets[rpp].valid) {
      if (len) {
        *len = packets[rpp].len;
      }
      return packets[rpp].ptr; 
    } else {
      return NULL; 
    }    
  }
}

void qbuff::release()
{
  packets[rp].valid = false; 
  packets[rp].len = 0; 
  rp += (rp+1 >= nof_messages)?(1-nof_messages):1; 
}

bool qbuff::send(void* buffer, uint32_t msg_size)
{
  if (msg_size <= max_msg_size) {
    void *ptr = request();
    if (ptr) {
      memcpy(ptr, buffer, msg_size);
      return push(msg_size);
    } else {
      printf("No ptr\n");
      return false; 
    }
  } else {
    return false; 
  }
}

uint32_t qbuff::pending_data()
{
  uint32_t total_len = 0; 
  for (int i=0;i<nof_messages;i++) {
    total_len += packets[i].len;
  }
  return total_len; 
}

// Move packets between queues with only 1 memcpy
void qbuff::move_to(qbuff *dst) {
  uint32_t len; 
  void *ptr_src = pop(&len);
  if (ptr_src) {
    void *ptr_dst = dst->request();
    if (ptr_dst) {
      memcpy(ptr_dst, ptr_src, len);
      dst->push(len);
      release();
    }
  }
}
    


int qbuff::recv(void* buffer, uint32_t buffer_size)
{
  uint32_t len; 
  void *ptr = pop(&len);
  if (ptr) {
    if (len <= buffer_size) {
      memcpy(buffer, ptr, len);   
      release();
      return len; 
    } else {
      return -1; 
    }
  } else {
    return 0; 
  }
}



}