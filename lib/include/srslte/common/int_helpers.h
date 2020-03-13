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

#ifndef SRSLTE_INT_HELPERS_H
#define SRSLTE_INT_HELPERS_H

namespace srslte {

/******************************************************************************
 * Safe conversions between byte buffers and integer types.
 * Note: these don't perform endian conversion - use e.g. htonl/ntohl if required
 *****************************************************************************/
inline void uint8_to_uint32(const uint8_t* buf, uint32_t* i)
{
  *i = (uint32_t)buf[0] << 24 | (uint32_t)buf[1] << 16 | (uint32_t)buf[2] << 8 | (uint32_t)buf[3];
}

inline void uint32_to_uint8(uint32_t i, uint8_t* buf)
{
  buf[0] = (i >> 24) & 0xFF;
  buf[1] = (i >> 16) & 0xFF;
  buf[2] = (i >> 8) & 0xFF;
  buf[3] = i & 0xFF;
}

inline void uint8_to_uint16(uint8_t* buf, uint16_t* i)
{
  *i = (uint32_t)buf[0] << 8 | (uint32_t)buf[1];
}

inline void uint16_to_uint8(uint16_t i, uint8_t* buf)
{
  buf[0] = (i >> 8) & 0xFF;
  buf[1] = i & 0xFF;
}

inline void uint8_to_uint24(uint8_t* buf, uint32_t* i)
{
  *i = (uint32_t)buf[0] << 16 | (uint32_t)buf[1] << 8 | (uint32_t)buf[2];
}

inline void uint24_to_uint8(uint32_t i, uint8_t* buf)
{
  buf[0] = (i >> 16) & 0xFF;
  buf[1] = (i >> 8) & 0xFF;
  buf[2] = i & 0xFF;
}

} // namespace srslte
#endif // SRSLTE_INT_HELPERS_H
