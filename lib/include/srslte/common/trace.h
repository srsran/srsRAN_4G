/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
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

/******************************************************************************
 *  File:         trace.h
 *  Description:
 *  Reference:
 *****************************************************************************/

#ifndef SRSLTE_TRACE_H
#define SRSLTE_TRACE_H

#include <stdio.h>
#include <string>
#include <sys/time.h>
#include <vector>

namespace srslte {

template <class elemType>
class trace
{
public:
  trace(uint32_t nof_elems_) : tti(nof_elems_), data(nof_elems_)
  {
    rpm       = 0;
    nof_elems = nof_elems_;
    wrapped   = false;
  };
  void push_cur_time_us(uint32_t cur_tti)
  {
    struct timeval t;
    gettimeofday(&t, NULL);
    elemType us = t.tv_sec * 1e6 + t.tv_usec;
    push(cur_tti, us);
  }
  void push(uint32_t value_tti, elemType value)
  {
    tti[rpm]  = value_tti;
    data[rpm] = value;
    rpm++;
    if (rpm >= nof_elems) {
      rpm     = 0;
      wrapped = true;
    }
  }
  bool writeToBinary(std::string filename)
  {
    FILE* f = fopen(filename.c_str(), "w");
    if (f != NULL) {
      uint32_t st = wrapped ? (rpm + 1) : 0;
      do {
        writeToBinaryValue(f, st++);
        if (st >= nof_elems) {
          st = 0;
        }
      } while (st != rpm);
      fclose(f);
      return true;
    } else {
      perror("fopen");
      return false;
    }
  }

private:
  std::vector<uint32_t> tti;
  std::vector<elemType> data;
  uint32_t              rpm;
  uint32_t              nof_elems;
  bool                  wrapped;

  void writeToBinaryValue(FILE* f, uint32_t idx)
  {
    fwrite(&tti[idx], 1, sizeof(uint32_t), f);
    fwrite(&data[idx], 1, sizeof(elemType), f);
  }
};

} // namespace srslte

#endif // SRSLTE_TRACE_H
