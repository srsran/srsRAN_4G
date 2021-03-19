/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSRAN_INT_HELPERS_H
#define SRSRAN_INT_HELPERS_H

namespace srsran {

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

} // namespace srsran
#endif // SRSRAN_INT_HELPERS_H
