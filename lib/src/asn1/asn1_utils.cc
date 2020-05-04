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

#include "srslte/asn1/asn1_utils.h"
#include "srslte/common/logmap.h"
#include <cmath>
#include <stdio.h>

namespace asn1 {

/************************
        logging
************************/

void vlog_print(srslte::LOG_LEVEL_ENUM log_level, const char* format, va_list args)
{
  char* args_msg = nullptr;
  if (vasprintf(&args_msg, format, args) > 0) {
    switch (log_level) {
      case srslte::LOG_LEVEL_ERROR:
        srslte::logmap::get("ASN1")->error("%s", args_msg);
        break;
      case srslte::LOG_LEVEL_WARNING:
        srslte::logmap::get("ASN1")->warning("%s", args_msg);
        break;
      case srslte::LOG_LEVEL_INFO:
        srslte::logmap::get("ASN1")->info("%s", args_msg);
        break;
      case srslte::LOG_LEVEL_DEBUG:
        srslte::logmap::get("ASN1")->debug("%s", args_msg);
      default:
        break;
    }
  }
  if (args_msg) {
    free(args_msg);
  }
}

void log_error(const char* format, ...)
{
  va_list args;
  va_start(args, format);
  vlog_print(srslte::LOG_LEVEL_ERROR, format, args);
  va_end(args);
}
void log_warning(const char* format, ...)
{
  va_list args;
  va_start(args, format);
  vlog_print(srslte::LOG_LEVEL_WARNING, format, args);
  va_end(args);
}
void log_info(const char* format, ...)
{
  va_list args;
  va_start(args, format);
  vlog_print(srslte::LOG_LEVEL_INFO, format, args);
  va_end(args);
}
void log_debug(const char* format, ...)
{
  va_list args;
  va_start(args, format);
  vlog_print(srslte::LOG_LEVEL_DEBUG, format, args);
  va_end(args);
}

/************************
     error handling
************************/

void log_error_code(SRSASN_CODE code, const char* filename, int line)
{
  switch (code) {
    case SRSASN_ERROR_ENCODE_FAIL:
      log_error("[%s][%d] Encoding failure.\n", filename, line);
      break;
    case SRSASN_ERROR_DECODE_FAIL:
      log_error("[%s][%d] Decoding failure.\n", filename, line);
      break;
    default:
      log_warning("[%s][%d] SRSASN_CODE=%d not recognized.\n", filename, line, (int)code);
  }
}

/*********************
       bit_ref
*********************/

template <typename Ptr>
int bit_ref_impl<Ptr>::distance(const bit_ref_impl<Ptr>& other) const
{
  return ((int)offset - (int)other.offset) + 8 * ((int)(ptr - other.ptr));
}
template <typename Ptr>
int bit_ref_impl<Ptr>::distance(const uint8_t* ref_ptr) const
{
  return (int)offset + 8 * ((int)(ptr - ref_ptr));
}
template <typename Ptr>
int bit_ref_impl<Ptr>::distance() const
{
  return (int)offset + 8 * ((int)(ptr - start_ptr));
}
template <typename Ptr>
int bit_ref_impl<Ptr>::distance_bytes(uint8_t* ref_ptr) const
{
  return ((int)(ptr - ref_ptr)) + ((offset) ? 1 : 0);
}
template <typename Ptr>
int bit_ref_impl<Ptr>::distance_bytes() const
{
  return ((int)(ptr - start_ptr)) + ((offset) ? 1 : 0);
}

SRSASN_CODE bit_ref::pack(uint32_t val, uint32_t n_bits)
{
  if (n_bits >= 32) {
    log_error("This method only supports packing up to 32 bits\n");
    return SRSASN_ERROR_ENCODE_FAIL;
  }
  uint32_t mask;
  while (n_bits > 0) {
    if (ptr >= max_ptr) {
      log_error("Buffer size limit was achieved\n");
      return SRSASN_ERROR_ENCODE_FAIL;
    }
    mask             = ((1u << n_bits) - 1u);
    val              = val & mask;
    uint8_t keepmask = ((uint8_t)-1) - (uint8_t)((1u << (8u - offset)) - 1u);
    if ((uint32_t)(8 - offset) > n_bits) {
      auto bit = static_cast<uint8_t>(val << (8u - offset - n_bits));
      *ptr     = ((*ptr) & keepmask) + bit;
      offset += n_bits;
      n_bits = 0;
    } else {
      auto bit = static_cast<uint8_t>(val >> (n_bits - 8u + offset));
      *ptr     = (*ptr & keepmask) + bit;
      n_bits -= (8 - offset);
      offset = 0;
      ptr++;
    }
  }
  return SRSASN_SUCCESS;
}

template <typename T, typename Ptr>
SRSASN_CODE unpack_bits(T& val, Ptr& ptr, uint8_t& offset, const uint8_t* max_ptr, uint32_t n_bits)
{
  if (n_bits > sizeof(T) * 8) {
    log_error("This method only supports unpacking up to %d bits\n", (int)sizeof(T) * 8);
    return SRSASN_ERROR_DECODE_FAIL;
  }
  val = 0;
  while (n_bits > 0) {
    if (ptr >= max_ptr) {
      log_error("Buffer size limit was achieved\n");
      return SRSASN_ERROR_DECODE_FAIL;
    }
    if ((uint32_t)(8 - offset) > n_bits) {
      uint8_t mask = (uint8_t)(1u << (8u - offset)) - (uint8_t)(1u << (8u - offset - n_bits));
      val += ((uint32_t)((*ptr) & mask)) >> (8u - offset - n_bits);
      offset += n_bits;
      n_bits = 0;
    } else {
      auto mask = static_cast<uint8_t>((1u << (8u - offset)) - 1u);
      val += ((uint32_t)((*ptr) & mask)) << (n_bits - 8 + offset);
      n_bits -= 8 - offset;
      offset = 0;
      ptr++;
    }
  }
  return SRSASN_SUCCESS;
}

template SRSASN_CODE
                     unpack_bits<bool, uint8_t*>(bool& val, uint8_t*& ptr, uint8_t& offset, const uint8_t* max_ptr, uint32_t n_bits);
template SRSASN_CODE unpack_bits<bool, const uint8_t*>(bool&           val,
                                                       const uint8_t*& ptr,
                                                       uint8_t&        offset,
                                                       const uint8_t*  max_ptr,
                                                       uint32_t        n_bits);
template SRSASN_CODE
                     unpack_bits<uint8_t, uint8_t*>(uint8_t& val, uint8_t*& ptr, uint8_t& offset, const uint8_t* max_ptr, uint32_t n_bits);
template SRSASN_CODE unpack_bits<uint8_t, const uint8_t*>(uint8_t&        val,
                                                          const uint8_t*& ptr,
                                                          uint8_t&        offset,
                                                          const uint8_t*  max_ptr,
                                                          uint32_t        n_bits);
template SRSASN_CODE
                     unpack_bits<uint16_t, uint8_t*>(uint16_t& val, uint8_t*& ptr, uint8_t& offset, const uint8_t* max_ptr, uint32_t n_bits);
template SRSASN_CODE unpack_bits<uint16_t, const uint8_t*>(uint16_t&       val,
                                                           const uint8_t*& ptr,
                                                           uint8_t&        offset,
                                                           const uint8_t*  max_ptr,
                                                           uint32_t        n_bits);
template SRSASN_CODE
                     unpack_bits<uint32_t, uint8_t*>(uint32_t& val, uint8_t*& ptr, uint8_t& offset, const uint8_t* max_ptr, uint32_t n_bits);
template SRSASN_CODE unpack_bits<uint32_t, const uint8_t*>(uint32_t&       val,
                                                           const uint8_t*& ptr,
                                                           uint8_t&        offset,
                                                           const uint8_t*  max_ptr,
                                                           uint32_t        n_bits);
template SRSASN_CODE
                     unpack_bits<uint64_t, uint8_t*>(uint64_t& val, uint8_t*& ptr, uint8_t& offset, const uint8_t* max_ptr, uint32_t n_bits);
template SRSASN_CODE unpack_bits<uint64_t, const uint8_t*>(uint64_t&       val,
                                                           const uint8_t*& ptr,
                                                           uint8_t&        offset,
                                                           const uint8_t*  max_ptr,
                                                           uint32_t        n_bits);

template <typename Ptr>
SRSASN_CODE bit_ref_impl<Ptr>::unpack_bytes(uint8_t* buf, uint32_t n_bytes)
{
  if (n_bytes == 0) {
    return SRSASN_SUCCESS;
  }
  if (ptr + n_bytes >= max_ptr) {
    log_error("Buffer size limit was achieved\n");
    return SRSASN_ERROR_DECODE_FAIL;
  }
  if (offset == 0) {
    // Aligned case
    memcpy(buf, ptr, n_bytes);
    ptr += n_bytes;
  } else {
    for (uint32_t i = 0; i < n_bytes; ++i) {
      HANDLE_CODE(unpack(buf[i], 8));
    }
  }
  return SRSASN_SUCCESS;
}

template <typename Ptr>
SRSASN_CODE bit_ref_impl<Ptr>::align_bytes()
{
  if (offset == 0)
    return SRSASN_SUCCESS;
  if (ptr >= max_ptr) {
    log_error("Buffer size limit was achieved\n");
    return SRSASN_ERROR_DECODE_FAIL;
  }
  offset = 0;
  ptr++;
  return SRSASN_SUCCESS;
}

template <typename Ptr>
SRSASN_CODE bit_ref_impl<Ptr>::advance_bits(uint32_t n_bits)
{
  uint32_t extra_bits     = (offset + n_bits) % 8;
  uint32_t bytes_required = ceilf((offset + n_bits) / 8.0f);
  uint32_t bytes_offset   = floorf((offset + n_bits) / 8.0f);

  if (ptr + bytes_required >= max_ptr) {
    log_error("Buffer size limit was achieved\n");
    return SRSASN_ERROR_DECODE_FAIL;
  }
  ptr += bytes_offset;
  offset = extra_bits;
  return SRSASN_SUCCESS;
}

template <typename Ptr>
void bit_ref_impl<Ptr>::set(Ptr start_ptr_, uint32_t max_size_)
{
  ptr       = start_ptr_;
  offset    = 0;
  start_ptr = start_ptr_;
  max_ptr   = max_size_ + start_ptr_;
}

template class asn1::bit_ref_impl<uint8_t*>;
template class asn1::bit_ref_impl<const uint8_t*>;

SRSASN_CODE bit_ref::pack_bytes(const uint8_t* buf, uint32_t n_bytes)
{
  if (n_bytes == 0) {
    return SRSASN_SUCCESS;
  }
  if (ptr + n_bytes >= max_ptr) {
    log_error("Buffer size limit was achieved\n");
    return SRSASN_ERROR_ENCODE_FAIL;
  }
  if (offset == 0) {
    // Aligned case
    memcpy(ptr, buf, n_bytes);
    ptr += n_bytes;
  } else {
    for (uint32_t i = 0; i < n_bytes; ++i) {
      pack(buf[i], 8);
    }
  }
  return SRSASN_SUCCESS;
}

SRSASN_CODE bit_ref::align_bytes_zero()
{
  if (offset == 0)
    return SRSASN_SUCCESS;
  if (ptr >= max_ptr) {
    log_error("Buffer size limit was achieved\n");
    return SRSASN_ERROR_ENCODE_FAIL;
  }
  auto mask = static_cast<uint8_t>(256u - (1u << (8u - offset)));
  *ptr &= mask;
  offset = 0;
  ptr++;
  return SRSASN_SUCCESS;
}

/*********************
     ext packing
*********************/

SRSASN_CODE pack_unsupported_ext_flag(bit_ref& bref, bool ext)
{
  HANDLE_CODE(bref.pack(ext, 1));
  if (ext) {
    log_error("ASN extensions not currently supported\n");
    return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

SRSASN_CODE unpack_unsupported_ext_flag(bool& ext, bit_ref& bref)
{
  SRSASN_CODE ret = bref.unpack(ext, 1);
  if (ext) {
    log_error("ASN extensions not currently supported\n");
    return SRSASN_ERROR_DECODE_FAIL;
  }
  return ret;
}

/************************
      enum packing
************************/

// Pack without extension
SRSASN_CODE pack_enum(bit_ref& bref, uint32_t enum_val, uint32_t nbits)
{
  return bref.pack(enum_val, nbits);
}

// Pack with extension
SRSASN_CODE pack_enum(bit_ref& bref, uint32_t enum_val, uint32_t nbits, uint32_t nof_noext)
{
  bool ext = enum_val >= nof_noext;
  HANDLE_CODE(bref.pack((bool)ext, 1));
  SRSASN_CODE ret;
  if (not ext) {
    ret = bref.pack(enum_val, nbits);
  } else {
    ret = pack_norm_small_non_neg_whole_number(bref, enum_val - nof_noext);
  }
  return ret;
}

SRSASN_CODE pack_enum(bit_ref& bref, uint32_t e, uint32_t nof_types, uint32_t nof_exts, bool has_ext)
{
  if (e >= nof_types) {
    log_error(
        "The provided enum is not within the range of possible values (%u>=%u)\n", (unsigned)e, (unsigned)nof_types);
    return SRSASN_ERROR_ENCODE_FAIL;
  }
  SRSASN_CODE ret;
  if (has_ext) {
    uint32_t nof_bits = (uint32_t)ceilf(log2f(nof_types - nof_exts));
    ret               = pack_enum(bref, e, nof_bits, nof_types - nof_exts);
  } else {
    uint32_t nof_bits = (uint32_t)ceilf(log2f(nof_types));
    ret               = pack_enum(bref, e, nof_bits);
  }
  return ret;
}

ValOrError unpack_enum(uint32_t nof_types, uint32_t nof_exts, bool has_ext, cbit_ref& bref)
{
  ValOrError ret;
  if (has_ext) {
    uint32_t nof_bits = (uint32_t)ceilf(log2f(nof_types - nof_exts));
    bool     ext;
    ret.code = bref.unpack(ext, 1);
    if (ret.code != SRSASN_SUCCESS) {
      return ret;
    }
    if (not ext) {
      ret.code = bref.unpack(ret.val, nof_bits);
    } else {
      ret.code = unpack_norm_small_non_neg_whole_number(ret.val, bref);
      ret.val += nof_types - nof_exts;
    }
  } else {
    uint32_t nof_bits = (uint32_t)ceilf(log2f(nof_types));
    ret.code          = bref.unpack(ret.val, nof_bits);
  }
  if (ret.val >= nof_types) {
    log_error("The provided enum is not within the range of possible values (%u>=%u)\n",
              (unsigned)ret.val,
              (unsigned)nof_types);
    ret.code = SRSASN_ERROR_DECODE_FAIL;
  }
  return ret;
}

/************************
     PER encoding
************************/

/**
 * X.691 - Section 10.5
 * Encoder function for a constrained whole number
 * @tparam IntType type of integer (e.g. uint8_t, uint32_t, etc.)
 * @param bref bit_ref where to encode integer
 * @param n integer current value
 * @param lb lower bound
 * @param ub upper bound
 * @return success or failure
 */
template <class IntType>
SRSASN_CODE pack_constrained_whole_number(bit_ref& bref, IntType n, IntType lb, IntType ub, bool aligned)
{
  if (ub < lb or n < lb or n > ub) {
    log_error("The condition lb <= n <= ub (%ld <= %ld <= %ld) was not met\n", (long)lb, (long)n, (long)ub);
    return SRSASN_ERROR_ENCODE_FAIL;
  }
  uint64_t ra = (uint64_t)(ub - lb) + 1; // NOTE: Can overflow if IntType is kept
  if (ra == 1) {
    return SRSASN_SUCCESS;
  }
  uint32_t n_bits   = (uint32_t)ceilf(log2f((float)ra)); // bit-field size
  IntType  toencode = n - lb;
  if (not aligned) {
    // UNALIGNED variant
    bref.pack(toencode, n_bits);
  } else {
    // ALIGNED variant
    SRSASN_CODE ret;
    if (ra < 256) {
      ret = bref.pack(toencode, n_bits);
    } else if (ra <= ASN_64K) {
      // both one and two octets cases
      uint32_t n_octets = ceil_frac(n_bits, 8u);
      HANDLE_CODE(bref.align_bytes_zero());
      HANDLE_CODE(bref.pack(0, n_octets * 8 - n_bits));
      HANDLE_CODE(bref.pack(toencode, n_bits));
      ret = bref.align_bytes_zero();
    } else {
      // TODO: Check if this is correct
      uint32_t n_bits_len = (uint32_t)ceilf(log2f(ceil_frac(n_bits, 8u)));
      n_bits              = (uint32_t)floorf(log2f(toencode) + 1);
      uint32_t n_octets   = (uint32_t)((n_bits + 7) / 8);
      HANDLE_CODE(bref.pack(n_octets - 1, n_bits_len));
      HANDLE_CODE(bref.align_bytes_zero());
      HANDLE_CODE(bref.pack(0, (n_octets * 8) - n_bits));
      ret = bref.pack(toencode, n_bits);
    }
    return ret;
  }
  return SRSASN_SUCCESS;
}
template SRSASN_CODE pack_constrained_whole_number<int8_t>(bit_ref& bref, int8_t n, int8_t lb, int8_t ub, bool aligned);
template SRSASN_CODE
pack_constrained_whole_number<int16_t>(bit_ref& bref, int16_t n, int16_t lb, int16_t ub, bool aligned);
template SRSASN_CODE
pack_constrained_whole_number<int32_t>(bit_ref& bref, int32_t n, int32_t lb, int32_t ub, bool aligned);
template SRSASN_CODE
pack_constrained_whole_number<int64_t>(bit_ref& bref, int64_t n, int64_t lb, int64_t ub, bool aligned);
template SRSASN_CODE
pack_constrained_whole_number<uint8_t>(bit_ref& bref, uint8_t n, uint8_t lb, uint8_t ub, bool aligned);
template SRSASN_CODE
pack_constrained_whole_number<uint16_t>(bit_ref& bref, uint16_t n, uint16_t lb, uint16_t ub, bool aligned);
template SRSASN_CODE
pack_constrained_whole_number<uint32_t>(bit_ref& bref, uint32_t n, uint32_t lb, uint32_t ub, bool aligned);
template SRSASN_CODE
pack_constrained_whole_number<uint64_t>(bit_ref& bref, uint64_t n, uint64_t lb, uint64_t ub, bool aligned);

/**
 * X.691 - Section 10.5
 * Decoder function for a constrained whole number
 * @tparam IntType type of integer (e.g. uint8_t, uint32_t, etc.)
 * @param n integer with decoded value
 * @param bref bit_ref where to encode integer
 * @param lb lower bound
 * @param ub upper bound
 * @return success or failure
 */
template <class IntType>
SRSASN_CODE unpack_constrained_whole_number(IntType& n, cbit_ref& bref, IntType lb, IntType ub, bool aligned)
{
  if (ub < lb) {
    log_error("The condition lb <= ub (%ld <= %ld) was not met\n", (long)lb, (long)ub);
    return SRSASN_ERROR_DECODE_FAIL;
  }
  uint64_t ra = (uint64_t)(ub - lb) + 1; // NOTE: Can overflow if IntType is kept.
  if (ra == 1) {
    n = lb;
    return SRSASN_SUCCESS;
  }
  uint32_t n_bits = (uint32_t)ceilf(log2f((float)ra));
  if (not aligned) {
    // UNALIGNED variant
    HANDLE_CODE(bref.unpack(n, n_bits));
    n += lb;
    if (n > ub) {
      log_error("The condition lb <= n <= ub (%ld <= %ld <= %ld) was not met\n", (long)lb, (long)n, (long)ub);
      return SRSASN_ERROR_DECODE_FAIL;
    }
  } else {
    // ALIGNED variant
    if (ra < 256) {
      HANDLE_CODE(bref.unpack(n, n_bits));
    } else if (ra <= ASN_64K) {
      uint32_t n_octets = ceil_frac(n_bits, 8u);
      HANDLE_CODE(bref.align_bytes());
      HANDLE_CODE(bref.unpack(n, n_octets * 8));
      HANDLE_CODE(bref.align_bytes());
    } else {
      // TODO: Check if this is correct
      uint32_t n_bits_len = (uint32_t)ceilf(log2f(ceil_frac(n_bits, 8u)));
      uint32_t n_octets;
      HANDLE_CODE(bref.unpack(n_octets, n_bits_len));
      n_octets += 1;
      HANDLE_CODE(bref.align_bytes());
      HANDLE_CODE(bref.unpack(n, n_octets * 8));
    }
    n += lb;
  }
  return SRSASN_SUCCESS;
}
template SRSASN_CODE
unpack_constrained_whole_number<int8_t>(int8_t& n, cbit_ref& bref, int8_t lb, int8_t ub, bool aligned);
template SRSASN_CODE
unpack_constrained_whole_number<int16_t>(int16_t& n, cbit_ref& bref, int16_t lb, int16_t ub, bool aligned);
template SRSASN_CODE
unpack_constrained_whole_number<int32_t>(int32_t& n, cbit_ref& bref, int32_t lb, int32_t ub, bool aligned);
template SRSASN_CODE
unpack_constrained_whole_number<int64_t>(int64_t& n, cbit_ref& bref, int64_t lb, int64_t ub, bool aligned);
template SRSASN_CODE
unpack_constrained_whole_number<uint8_t>(uint8_t& n, cbit_ref& bref, uint8_t lb, uint8_t ub, bool aligned);
template SRSASN_CODE
unpack_constrained_whole_number<uint16_t>(uint16_t& n, cbit_ref& bref, uint16_t lb, uint16_t ub, bool aligned);
template SRSASN_CODE
unpack_constrained_whole_number<uint32_t>(uint32_t& n, cbit_ref& bref, uint32_t lb, uint32_t ub, bool aligned);
template SRSASN_CODE
unpack_constrained_whole_number<uint64_t>(uint64_t& n, cbit_ref& bref, uint64_t lb, uint64_t ub, bool aligned);

/**
 * X.691 - Section 10.6
 * Encoder function for a normally small non-negative whole number
 * @tparam IntType type of integer (e.g. uint8_t, uint32_t, etc.)
 * @param bref bit_ref where to encode integer
 * @param n integer current value
 * @return success or failure
 */
template <typename UintType>
SRSASN_CODE pack_norm_small_non_neg_whole_number(bit_ref& bref, UintType n)
{
  if (n <= 63) {
    HANDLE_CODE(bref.pack(n, 7)); // [1 bit: 0 | 6 bit: n]
  } else {
    HANDLE_CODE(bref.pack(1, 1));
    log_error("Long small integers not supported\n");
    return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
template <typename UintType>
SRSASN_CODE unpack_norm_small_non_neg_whole_number(UintType& n, cbit_ref& bref)
{
  bool        ext;
  SRSASN_CODE ret = bref.unpack(ext, 1);
  HANDLE_CODE(ret);
  if (not ext) {
    ret = bref.unpack(n, 6);
  } else {
    log_error("Long small integers not supported\n");
    return SRSASN_ERROR_DECODE_FAIL;
  }
  return ret;
}
template SRSASN_CODE pack_norm_small_non_neg_whole_number<uint8_t>(bit_ref& bref, uint8_t n);
template SRSASN_CODE pack_norm_small_non_neg_whole_number<uint16_t>(bit_ref& bref, uint16_t n);
template SRSASN_CODE pack_norm_small_non_neg_whole_number<uint32_t>(bit_ref& bref, uint32_t n);
template SRSASN_CODE pack_norm_small_non_neg_whole_number<uint64_t>(bit_ref& bref, uint64_t n);
template SRSASN_CODE unpack_norm_small_non_neg_whole_number<uint8_t>(uint8_t& n, cbit_ref& bref);
template SRSASN_CODE unpack_norm_small_non_neg_whole_number<uint16_t>(uint16_t& n, cbit_ref& bref);
template SRSASN_CODE unpack_norm_small_non_neg_whole_number<uint32_t>(uint32_t& n, cbit_ref& bref);
template SRSASN_CODE unpack_norm_small_non_neg_whole_number<uint64_t>(uint64_t& n, cbit_ref& bref);

template <typename IntType>
IntType unconstrained_whole_number_length(IntType n)
{
  return (IntType)ceilf((log2f(n) + 1) / 8.0f);
}

/**
 * X.691 - Section 10.8
 * Encoder function for an unconstrained whole number
 * @tparam IntType type of integer (e.g. int8_t, uint32_t, etc.)
 * @param bref bit_ref where to encode integer
 * @param n integer current value
 * @return success or failure
 */
template <typename IntType>
SRSASN_CODE pack_unconstrained_whole_number(bit_ref& bref, IntType n, bool aligned)
{
  // TODO: Test
  uint32_t len = unconstrained_whole_number_length(n);
  if (aligned) {
    HANDLE_CODE(bref.align_bytes_zero());
  }
  HANDLE_CODE(bref.pack(n, len * 8));

  return SRSASN_SUCCESS;
}
template <typename IntType>
SRSASN_CODE unpack_unconstrained_whole_number(IntType& n, cbit_ref& bref, bool aligned)
{
  // TODO: Test
  uint32_t len;
  HANDLE_CODE(unpack_length(len, bref, aligned));
  if (aligned) {
    HANDLE_CODE(bref.align_bytes());
  }
  HANDLE_CODE(bref.unpack(n, len * 8));

  return SRSASN_SUCCESS;
}
template SRSASN_CODE pack_unconstrained_whole_number<int8_t>(bit_ref& bref, int8_t n, bool aligned);
template SRSASN_CODE pack_unconstrained_whole_number<int16_t>(bit_ref& bref, int16_t n, bool aligned);
template SRSASN_CODE pack_unconstrained_whole_number<int32_t>(bit_ref& bref, int32_t n, bool aligned);
template SRSASN_CODE pack_unconstrained_whole_number<int64_t>(bit_ref& bref, int64_t n, bool aligned);
template SRSASN_CODE unpack_unconstrained_whole_number<int8_t>(int8_t& n, cbit_ref& bref, bool aligned);
template SRSASN_CODE unpack_unconstrained_whole_number<int16_t>(int16_t& n, cbit_ref& bref, bool aligned);
template SRSASN_CODE unpack_unconstrained_whole_number<int32_t>(int32_t& n, cbit_ref& bref, bool aligned);
template SRSASN_CODE unpack_unconstrained_whole_number<int64_t>(int64_t& n, cbit_ref& bref, bool aligned);
template SRSASN_CODE pack_unconstrained_whole_number<uint8_t>(bit_ref& bref, uint8_t n, bool aligned);
template SRSASN_CODE pack_unconstrained_whole_number<uint16_t>(bit_ref& bref, uint16_t n, bool aligned);
template SRSASN_CODE pack_unconstrained_whole_number<uint32_t>(bit_ref& bref, uint32_t n, bool aligned);
template SRSASN_CODE pack_unconstrained_whole_number<uint64_t>(bit_ref& bref, uint64_t n, bool aligned);
template SRSASN_CODE unpack_unconstrained_whole_number<uint8_t>(uint8_t& n, cbit_ref& bref, bool aligned);
template SRSASN_CODE unpack_unconstrained_whole_number<uint16_t>(uint16_t& n, cbit_ref& bref, bool aligned);
template SRSASN_CODE unpack_unconstrained_whole_number<uint32_t>(uint32_t& n, cbit_ref& bref, bool aligned);
template SRSASN_CODE unpack_unconstrained_whole_number<uint64_t>(uint64_t& n, cbit_ref& bref, bool aligned);

/*********************
   varlength_packing
*********************/

template <typename IntType>
SRSASN_CODE pack_length(bit_ref& bref, IntType n, IntType lb, IntType ub, bool aligned)
{
  return pack_constrained_whole_number(bref, n, lb, ub, aligned);
}
template SRSASN_CODE pack_length<uint8_t>(bit_ref& bref, uint8_t n, uint8_t lb, uint8_t ub, bool aligned);
template SRSASN_CODE pack_length<uint16_t>(bit_ref& bref, uint16_t n, uint16_t lb, uint16_t ub, bool aligned);
template SRSASN_CODE pack_length<uint32_t>(bit_ref& bref, uint32_t n, uint32_t lb, uint32_t ub, bool aligned);
template SRSASN_CODE pack_length<uint64_t>(bit_ref& bref, uint64_t n, uint64_t lb, uint64_t ub, bool aligned);
template SRSASN_CODE pack_length<int8_t>(bit_ref& bref, int8_t n, int8_t lb, int8_t ub, bool aligned);
template SRSASN_CODE pack_length<int16_t>(bit_ref& bref, int16_t n, int16_t lb, int16_t ub, bool aligned);
template SRSASN_CODE pack_length<int32_t>(bit_ref& bref, int32_t n, int32_t lb, int32_t ub, bool aligned);
template SRSASN_CODE pack_length<int64_t>(bit_ref& bref, int64_t n, int64_t lb, int64_t ub, bool aligned);

template <typename IntType>
SRSASN_CODE unpack_length(IntType& n, cbit_ref& bref, IntType lb, IntType ub, bool aligned)
{
  return unpack_constrained_whole_number(n, bref, lb, ub, aligned);
}
template SRSASN_CODE unpack_length<uint8_t>(uint8_t& n, cbit_ref& bref, uint8_t lb, uint8_t ub, bool aligned);
template SRSASN_CODE unpack_length<uint16_t>(uint16_t& n, cbit_ref& bref, uint16_t lb, uint16_t ub, bool aligned);
template SRSASN_CODE unpack_length<uint32_t>(uint32_t& n, cbit_ref& bref, uint32_t lb, uint32_t ub, bool aligned);
template SRSASN_CODE unpack_length<uint64_t>(uint64_t& n, cbit_ref& bref, uint64_t lb, uint64_t ub, bool aligned);
template SRSASN_CODE unpack_length<int8_t>(int8_t& n, cbit_ref& bref, int8_t lb, int8_t ub, bool aligned);
template SRSASN_CODE unpack_length<int16_t>(int16_t& n, cbit_ref& bref, int16_t lb, int16_t ub, bool aligned);
template SRSASN_CODE unpack_length<int32_t>(int32_t& n, cbit_ref& bref, int32_t lb, int32_t ub, bool aligned);
template SRSASN_CODE unpack_length<int64_t>(int64_t& n, cbit_ref& bref, int64_t lb, int64_t ub, bool aligned);

/**
 * X.691 - Section 10.9
 * Pack unconstrained length determinant
 * @param bref
 * @param val "n" length value to pack
 * @param aligned
 * @return
 */
SRSASN_CODE pack_length(bit_ref& bref, uint32_t val, bool aligned)
{
  if (not aligned) {
    if (val < 128) {
      HANDLE_CODE(bref.pack(val, 8)); // first bit is 0
    } else if (val < ASN_16K) {
      HANDLE_CODE(bref.pack(0b10, 2));
      HANDLE_CODE(bref.pack(val, 14));
    } else {
      log_error("Not handling sizes longer than 16383 octets\n");
      return SRSASN_ERROR_ENCODE_FAIL;
    }
  } else {
    if (val > ASN_64K) {
      // TODO: Error message
      return SRSASN_ERROR_ENCODE_FAIL;
    }
    HANDLE_CODE(bref.align_bytes());
    if (val < 128) {
      return bref.pack(val, 8); // first bit is 0
    } else if (val < ASN_16K) {
      HANDLE_CODE(bref.pack(0b10, 2));
      return bref.pack(val, 14);
    } else {
      HANDLE_CODE(bref.pack(0b11, 2));
      uint32_t m = val / ASN_16K;
      HANDLE_CODE(bref.pack(m, 6));
      val = val - m * ASN_16K;
      return bref.pack(val, 16); // TODO: TODO
    }
  }
  return SRSASN_SUCCESS;
}

SRSASN_CODE unpack_length(uint32_t& val, cbit_ref& bref, bool aligned)
{
  bool ext;
  if (not aligned) {
    SRSASN_CODE ret = bref.unpack(ext, 1);
    HANDLE_CODE(ret);
    if (not ext) {
      ret = bref.unpack(val, 7);
    } else {
      ret = bref.unpack(ext, 1);
      HANDLE_CODE(ret);
      if (not ext) {
        ret = bref.unpack(val, 14);
      } else {
        log_error("Not handling octet strings longer than 16383 octets\n");
        val = 0;
        return SRSASN_ERROR_DECODE_FAIL;
      }
    }
    return ret;
  } else {
    HANDLE_CODE(bref.align_bytes());
    HANDLE_CODE(bref.unpack(val, 8));
    if (val < 128) {
      // "n" < 128
      // first bit was set to zero
      return SRSASN_SUCCESS;
    }
    val -= 128;
    if (val < 64) {
      // 128 <= "n" < 16K
      // second bit is set to zero as well
      uint32_t val_octet_2;
      HANDLE_CODE(bref.unpack(val_octet_2, 8));
      val = (val << 8u) + val_octet_2;
      return SRSASN_SUCCESS;
    }
    // TODO: Error message
    return SRSASN_ERROR_ENCODE_FAIL;
  }
}

/************************
        Integer
************************/

template <typename IntType>
SRSASN_CODE pack_integer(bit_ref& bref, IntType n, IntType lb, IntType ub, bool has_ext, bool aligned)
{
  // pack ext marker
  bool within_bounds = n >= lb and n <= ub;
  if (has_ext) {
    HANDLE_CODE(bref.pack(not within_bounds, 1));
  } else if (not within_bounds) {
    log_error("The condition lb <= n <= ub (%ld <= %ld <= %ld) was not met\n", (long)lb, (long)n, (long)ub);
    return SRSASN_ERROR_ENCODE_FAIL;
  }
  bool lower_bounded = lb != std::numeric_limits<IntType>::min() or std::is_unsigned<IntType>::value;
  bool upper_bounded = ub != std::numeric_limits<IntType>::max();

  if (within_bounds and lower_bounded and upper_bounded) {
    // constrained case
    if (lb == ub) {
      return SRSASN_SUCCESS;
    }
    // TODO: Check if we are in the indefinite length case, and pack length prefix if needed
    //    if(indefinite_length) {
    //      HANDLE_CODE(pack_length(bref, n, 1, ceilf(log2f()), aligned));
    //    }
    HANDLE_CODE(pack_constrained_whole_number(bref, n, (IntType)lb, (IntType)ub, aligned));
  } else {
    if (not within_bounds or (not lower_bounded and not upper_bounded)) {
      HANDLE_CODE(pack_length(bref, unconstrained_whole_number_length(n), aligned));
      HANDLE_CODE(pack_unconstrained_whole_number(bref, n, aligned));
    } else {
      // pack as semi-constrained
      // TODO
    }
  }
  return SRSASN_SUCCESS;
}
template SRSASN_CODE
pack_integer<uint8_t>(bit_ref& bref, uint8_t n, uint8_t lb, uint8_t ub, bool has_ext, bool aligned);
template SRSASN_CODE
pack_integer<uint16_t>(bit_ref& bref, uint16_t n, uint16_t lb, uint16_t ub, bool has_ext, bool aligned);
template SRSASN_CODE
pack_integer<uint32_t>(bit_ref& bref, uint32_t n, uint32_t lb, uint32_t ub, bool has_ext, bool aligned);
template SRSASN_CODE
                     pack_integer<uint64_t>(bit_ref& bref, uint64_t n, uint64_t lb, uint64_t ub, bool has_ext, bool aligned);
template SRSASN_CODE pack_integer<int8_t>(bit_ref& bref, int8_t n, int8_t lb, int8_t ub, bool has_ext, bool aligned);
template SRSASN_CODE
pack_integer<int16_t>(bit_ref& bref, int16_t n, int16_t lb, int16_t ub, bool has_ext, bool aligned);
template SRSASN_CODE
pack_integer<int32_t>(bit_ref& bref, int32_t n, int32_t lb, int32_t ub, bool has_ext, bool aligned);
template SRSASN_CODE
pack_integer<int64_t>(bit_ref& bref, int64_t n, int64_t lb, int64_t ub, bool has_ext, bool aligned);

// unconstrained specialization case
template <typename IntType>
SRSASN_CODE pack_unconstrained_integer(bit_ref& bref, IntType n, bool has_ext, bool aligned)
{
  return pack_integer(
      bref, n, std::numeric_limits<IntType>::min(), std::numeric_limits<IntType>::max(), has_ext, aligned);
}
template SRSASN_CODE pack_unconstrained_integer<int8_t>(bit_ref& bref, int8_t n, bool has_ext, bool aligned);
template SRSASN_CODE pack_unconstrained_integer<int16_t>(bit_ref& bref, int16_t n, bool has_ext, bool aligned);
template SRSASN_CODE pack_unconstrained_integer<int32_t>(bit_ref& bref, int32_t n, bool has_ext, bool aligned);
template SRSASN_CODE pack_unconstrained_integer<int64_t>(bit_ref& bref, int64_t n, bool has_ext, bool aligned);

template <typename IntType>
SRSASN_CODE unpack_integer(IntType& n, cbit_ref& bref, IntType lb, IntType ub, bool has_ext, bool aligned)
{
  bool within_bounds = true;
  if (has_ext) {
    HANDLE_CODE(bref.unpack(within_bounds, 1));
    within_bounds = not within_bounds;
  }
  bool lower_bounded = lb != std::numeric_limits<IntType>::min() or std::is_unsigned<IntType>::value;
  bool upper_bounded = ub != std::numeric_limits<IntType>::max();

  if (within_bounds and lower_bounded and upper_bounded) {
    // constrained case
    if (lb == ub) {
      return SRSASN_SUCCESS;
    }
    // TODO: Check if we are in the indefinite length case, and pack length prefix if needed
    HANDLE_CODE(unpack_constrained_whole_number(n, bref, (IntType)lb, (IntType)ub, aligned));
  } else {
    if (not within_bounds or (not lower_bounded and not upper_bounded)) {
      uint32_t len;
      HANDLE_CODE(unpack_length(len, bref, aligned));
      HANDLE_CODE(unpack_unconstrained_whole_number(n, bref, aligned)); // TODO
    } else {
      // pack as semi-constrained
      // TODO
    }
  }
  return SRSASN_SUCCESS;
}
template SRSASN_CODE
unpack_integer<uint8_t>(uint8_t& n, cbit_ref& bref, uint8_t lb, uint8_t ub, bool has_ext, bool aligned);
template SRSASN_CODE
unpack_integer<uint16_t>(uint16_t& n, cbit_ref& bref, uint16_t lb, uint16_t ub, bool has_ext, bool aligned);
template SRSASN_CODE
unpack_integer<uint32_t>(uint32_t& n, cbit_ref& bref, uint32_t lb, uint32_t ub, bool has_ext, bool aligned);
template SRSASN_CODE
unpack_integer<uint64_t>(uint64_t& n, cbit_ref& bref, uint64_t lb, uint64_t ub, bool has_ext, bool aligned);
template SRSASN_CODE
unpack_integer<int8_t>(int8_t& n, cbit_ref& bref, int8_t lb, int8_t ub, bool has_ext, bool aligned);
template SRSASN_CODE
unpack_integer<int16_t>(int16_t& n, cbit_ref& bref, int16_t lb, int16_t ub, bool has_ext, bool aligned);
template SRSASN_CODE
unpack_integer<int32_t>(int32_t& n, cbit_ref& bref, int32_t lb, int32_t ub, bool has_ext, bool aligned);
template SRSASN_CODE
unpack_integer<int64_t>(int64_t& n, cbit_ref& bref, int64_t lb, int64_t ub, bool has_ext, bool aligned);

// unconstrained specialization case
template <typename IntType>
SRSASN_CODE unpack_unconstrained_integer(IntType& n, cbit_ref& bref, bool has_ext, bool aligned)
{
  return unpack_integer(
      n, bref, std::numeric_limits<IntType>::min(), std::numeric_limits<IntType>::max(), has_ext, aligned);
}
template SRSASN_CODE unpack_unconstrained_integer<int8_t>(int8_t& n, cbit_ref& bref, bool has_ext, bool aligned);
template SRSASN_CODE unpack_unconstrained_integer<int16_t>(int16_t& n, cbit_ref& bref, bool has_ext, bool aligned);
template SRSASN_CODE unpack_unconstrained_integer<int32_t>(int32_t& n, cbit_ref& bref, bool has_ext, bool aligned);
template SRSASN_CODE unpack_unconstrained_integer<int64_t>(int64_t& n, cbit_ref& bref, bool has_ext, bool aligned);

// standalone packer
template <class IntType>
integer_packer<IntType>::integer_packer(IntType lb_, IntType ub_, bool has_ext_, bool aligned_) :
  lb(lb_),
  ub(ub_),
  has_ext(has_ext_),
  aligned(aligned_)
{}

template <class IntType>
SRSASN_CODE integer_packer<IntType>::pack(bit_ref& bref, IntType n)
{
  return pack_integer(bref, n, lb, ub, has_ext, aligned);
}
template <class IntType>
SRSASN_CODE integer_packer<IntType>::unpack(IntType& n, cbit_ref& bref)
{
  return unpack_integer(n, bref, lb, ub, has_ext, aligned);
}
template struct integer_packer<int8_t>;
template struct integer_packer<int16_t>;
template struct integer_packer<int32_t>;
template struct integer_packer<int64_t>;
template struct integer_packer<uint8_t>;
template struct integer_packer<uint16_t>;
template struct integer_packer<uint32_t>;
template struct integer_packer<uint64_t>;

/************************
    common octstring
************************/

uint64_t octstring_to_number(const uint8_t* ptr, uint32_t nbytes)
{
  if (nbytes > 8) {
    log_error("octstring of size=%d does not fit in an uint64_t\n", nbytes);
    return 0;
  }
  uint64_t val = 0;
  for (uint32_t i = 0; i < nbytes; ++i) {
    val += ((uint64_t)ptr[nbytes - 1 - i]) << (uint64_t)(i * 8);
  }
  return val;
}

void number_to_octstring(uint8_t* ptr, uint64_t number, uint32_t nbytes)
{
  if (nbytes > 8) {
    log_error("octstring of size=%d does not fit in an uint64_t\n", nbytes);
    return;
  }
  for (uint32_t i = 0; i < nbytes; ++i) {
    ptr[nbytes - 1 - i] = (number >> (uint64_t)(i * 8u)) & 0xFFu;
  }
}

void to_hex(char* cstr, uint8_t val)
{
  sprintf(cstr, "%02x", val);
}

// helper functions
std::string octstring_to_string(const uint8_t* ptr, uint32_t N)
{
  std::string s;
  s.resize(N * 2);
  char cstr[3];
  for (uint32_t i = 0; i < N; i++) {
    to_hex(cstr, ptr[i]);
    s.replace(i * 2, 2, cstr);
  }
  return s;
}

void string_to_octstring(uint8_t* ptr, const std::string& str)
{
  if (str.size() % 2 != 0) {
    log_warning("The provided hex string size=%zd is not a multiple of 2\n.", str.size());
  }
  char cstr[] = "\0\0\0";
  for (uint32_t i = 0; i < str.size(); i += 2) {
    memcpy(&cstr[0], &str[i], 2);
    ptr[i / 2] = strtoul(cstr, nullptr, 16);
  }
}

/************************
     dyn_octstring
************************/

template <bool Al>
SRSASN_CODE unbounded_octstring<Al>::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_length(bref, size(), aligned));
  for (uint32_t i = 0; i < size(); ++i) {
    HANDLE_CODE(bref.pack(octets_[i], 8));
  }
  return SRSASN_SUCCESS;
}

template <bool Al>
SRSASN_CODE unbounded_octstring<Al>::unpack(cbit_ref& bref)
{
  uint32_t len;
  HANDLE_CODE(unpack_length(len, bref, aligned));
  resize(len);
  for (uint32_t i = 0; i < size(); ++i) {
    HANDLE_CODE(bref.unpack(octets_[i], 8));
  }
  return SRSASN_SUCCESS;
}

template <bool Al>
std::string unbounded_octstring<Al>::to_string() const
{
  return octstring_to_string(&octets_[0], size());
}

template <bool Al>
unbounded_octstring<Al>& unbounded_octstring<Al>::from_string(const std::string& hexstr)
{
  resize(hexstr.size() / 2);
  string_to_octstring(&octets_[0], hexstr);
  return *this;
}

template class unbounded_octstring<true>;
template class unbounded_octstring<false>;

/*********************
     bitstring
*********************/

namespace bitstring_utils {

/**
 * Pack ASN1 bitstring length prefix. Accommodates for cases: fixed/unbounded/bounded, aligned/unaligned, with/out ext
 */
SRSASN_CODE pack_length_prefix(bit_ref& bref,
                               uint32_t len,
                               uint32_t lb         = 0,
                               uint32_t ub         = std::numeric_limits<uint32_t>::max(),
                               bool     has_ext    = false,
                               bool     is_aligned = false)
{
  if (has_ext and ub == std::numeric_limits<uint32_t>::max()) {
    log_error("has extension marker but it is an unbounded prefix size\n");
    return SRSASN_ERROR_ENCODE_FAIL;
  }
  bool within_bounds = len >= lb and len <= ub;
  if (not within_bounds and not has_ext) {
    log_error("bitstring length=%d is not within bounds [%d, %d]\n", len, lb, ub);
    return SRSASN_ERROR_ENCODE_FAIL;
  }

  // encode ext bit
  if (has_ext) {
    HANDLE_CODE(bref.pack(not within_bounds, 1));
  }

  // do not encode prefix if fixed size
  if (lb == ub and within_bounds) {
    return SRSASN_SUCCESS;
  }

  // pack as unbounded if unbounded bitstring or ext is active
  if (ub == std::numeric_limits<uint32_t>::max() or not within_bounds) {
    return pack_length(bref, len);
  }

  // pack as bounded bitstring
  uint32_t len_bits = (uint32_t)ceilf(log2(ub - lb));
  HANDLE_CODE(bref.pack(len - lb, len_bits));
  if (is_aligned) {
    HANDLE_CODE(bref.align_bytes_zero());
  }
  return SRSASN_SUCCESS;
}

SRSASN_CODE pack_bitfield(bit_ref& bref, const uint8_t* buf, uint32_t nbits, uint32_t lb, uint32_t ub, bool is_aligned)
{
  if (nbits == 0) {
    log_error("Invalid bitstring size=%d\n", nbits);
    return SRSASN_ERROR_ENCODE_FAIL;
  }
  if (is_aligned and (lb != ub or ub > 16)) {
    bref.align_bytes();
  }
  uint32_t n_octs = ceil_frac(nbits, 8u);
  uint32_t offset = ((nbits - 1) % 8) + 1;
  HANDLE_CODE(bref.pack(buf[n_octs - 1], offset));
  for (uint32_t i = 1; i < n_octs; ++i) {
    HANDLE_CODE(bref.pack(buf[n_octs - 1 - i], 8));
  }
  return SRSASN_SUCCESS;
}

/**
 * Pack ASN1 bitstring. Accommodates for cases: fixed/unbounded/bounded, aligned/unaligned, with/out ext
 */
SRSASN_CODE
pack(bit_ref& bref, const uint8_t* data, uint32_t len, uint32_t lb, uint32_t ub, bool has_ext, bool is_aligned)
{
  HANDLE_CODE(bitstring_utils::pack_length_prefix(bref, len, lb, ub, has_ext, is_aligned));
  return pack_bitfield(bref, data, len, lb, ub, is_aligned);
}

// Unpack prefix, excluding ext bit

/**
 * Unpack ASN1 bitstring length prefix. Accommodates for cases: fixed/unbounded/bounded, aligned/unaligned, with/out ext
 */
SRSASN_CODE unpack_length_prefix(uint32_t& len, cbit_ref& bref, uint32_t lb, uint32_t ub, bool has_ext, bool is_aligned)
{
  bool ext = false;
  if (has_ext) {
    HANDLE_CODE(bref.unpack(ext, 1));
  }

  // unbounded or ext is set
  if (ub == std::numeric_limits<uint32_t>::max() or ext) {
    return unpack_length(len, bref, is_aligned);
  }

  // fixed length with no ext
  if (lb == ub) {
    len = lb;
    return SRSASN_SUCCESS;
  }

  // constrained, simple case
  HANDLE_CODE(unpack_length(len, bref, lb, ub, is_aligned));
  //  if (is_aligned) {
  //    HANDLE_CODE(bref.align_bytes_zero());
  //  }
  //  uint32_t len_bits = ceilf(log2f(ub - lb));
  //  HANDLE_CODE(bref.unpack(len, len_bits));
  //  len += lb;
  return SRSASN_SUCCESS;
}

// for both fixed, constrained and unconstrained scenarios
SRSASN_CODE unpack_bitfield(uint8_t* buf, cbit_ref& bref, uint32_t n, uint32_t lb, uint32_t ub, bool is_aligned)
{
  if (n > ASN_64K) {
    log_error("bitstrings longer than 64K not supported\n");
    return SRSASN_ERROR_DECODE_FAIL;
  }
  if (n == 0) {
    // empty bit string
    return SRSASN_SUCCESS;
  }
  if (is_aligned and (lb != ub or ub > 16)) {
    bref.align_bytes();
  }
  // unpack exactly "n" bits
  uint32_t n_octs = ceil_frac(n, 8u);
  uint32_t offset = ((n - 1) % 8) + 1;
  HANDLE_CODE(bref.unpack(buf[n_octs - 1], offset));
  for (uint32_t i = 1; i < n_octs; ++i) {
    HANDLE_CODE(bref.unpack(buf[n_octs - 1 - i], 8));
  }
  return SRSASN_SUCCESS;
}

void from_number(uint8_t* ptr, uint64_t number, uint32_t nbits)
{
  if (nbits > 64u) {
    log_error("bitstring of size=%d does not fit in an uint64_t\n", nbits);
    return;
  }
  uint32_t nof_bytes = ceil_frac(nbits, 8u);
  for (uint32_t i = 0; i < nof_bytes; ++i) {
    ptr[i] = (number >> (i * 8u)) & 0xFFu;
  }
  uint32_t offset = nbits % 8; // clean up any extra set bit
  if (offset > 0) {
    ptr[nof_bytes - 1] &= (uint8_t)((1u << offset) - 1u);
  }
}

std::string to_string(const uint8_t* ptr, uint32_t nbits)
{
  std::string str(nbits, '\0');
  for (uint32_t i = 0; i < nbits; ++i) {
    str[i] = bitstring_utils::get(ptr, nbits - 1 - i) ? '1' : '0';
  }
  return str;
}

uint64_t to_number(const uint8_t* ptr, uint32_t nbits)
{
  if (nbits > 64u) {
    log_error("bitstring of size=%d does not fit in an uint64_t\n", nbits);
    return 0;
  }
  uint64_t val       = 0;
  uint32_t nof_bytes = ceil_frac(nbits, 8u);
  for (uint32_t i = 0; i < nof_bytes; ++i) {
    val += ptr[i] << (i * 8u);
  }
  return val;
}

} // namespace bitstring_utils

/*********************
     choice utils
*********************/
void log_invalid_access_choice_id(uint32_t val, uint32_t choice_id)
{
  log_error("The access choide id is invalid (%zd!=%zd)\n", (size_t)val, (size_t)choice_id);
}
void log_invalid_choice_id(uint32_t val, const char* choice_type)
{
  log_error("Invalid choice id=%zd for choice type %s\n", (size_t)val, choice_type);
}

/*********************
   printable string
*********************/

namespace asn_string_utils {

size_t get_nof_bits_per_char(size_t lb, size_t ub, bool aligned)
{
  size_t N = ub - lb + 1;
  auto   b = (size_t)ceilf(log2(N)); // B
  if (aligned) {
    b = (size_t)pow(2, ceilf(log2(b))); // B2
  }
  return b;
}

bool is_octet_aligned(size_t bits_per_char, size_t alb, size_t aub, bool aligned)
{
  size_t max_nof_bits = bits_per_char * aub;
  return aligned and (max_nof_bits > 16 or (alb != aub and max_nof_bits == 16));
}

constexpr bool is_length_encoded(size_t alb, size_t aub, bool aligned)
{
  return alb != aub or aub >= ASN_64K;
}

SRSASN_CODE
pack(bit_ref& bref, const std::string& s, size_t lb, size_t ub, size_t alb, size_t aub, bool ext, bool aligned)
{
  bool within_limits = s.size() >= alb and s.size() <= aub;
  if (not within_limits) {
    // TODO: print error
    // NOTE: This should be valid for exts
    log_error("The PrintableString size=%zd is not within the limits [%zd, %zd]\n", s.size(), alb, aub);
    return SRSASN_ERROR_ENCODE_FAIL;
  }
  size_t b              = asn_string_utils::get_nof_bits_per_char(lb, ub, aligned);
  bool   octet_aligned  = asn_string_utils::is_octet_aligned(b, alb, aub, aligned);
  bool   length_encoded = asn_string_utils::is_length_encoded(alb, aub, aligned);
  if (octet_aligned) {
    bref.align_bytes_zero();
  }
  if (ext) {
    HANDLE_CODE(bref.pack(0, 1));
  }
  if (length_encoded) {
    pack_length(bref, s.size(), alb, aub, octet_aligned);
  }
  if (octet_aligned) {
    bref.align_bytes_zero();
  }
  for (char c : s) {
    HANDLE_CODE(bref.pack(c, b));
  }
  return SRSASN_SUCCESS;
}

SRSASN_CODE unpack(std::string& s, cbit_ref& bref, size_t lb, size_t ub, size_t alb, size_t aub, bool ext, bool aligned)
{
  size_t b              = asn_string_utils::get_nof_bits_per_char(lb, ub, aligned);
  bool   octet_aligned  = asn_string_utils::is_octet_aligned(b, alb, aub, aligned);
  bool   length_encoded = asn_string_utils::is_length_encoded(alb, aub, aligned);
  size_t max_nof_bits   = b * aub;
  if (octet_aligned) {
    bref.align_bytes();
  }
  if (ext) {
    bool is_ext;
    HANDLE_CODE(bref.unpack(is_ext, 1));
    if (is_ext) {
      log_error("Extension of PrintableString not supported\n");
      return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (length_encoded) {
    size_t n;
    unpack_length(n, bref, alb, aub, octet_aligned);
    s.resize(n);
  }
  if (octet_aligned) {
    bref.align_bytes();
  }
  for (char& c : s) {
    HANDLE_CODE(bref.unpack(c, b));
  }
  return SRSASN_SUCCESS;
}

} // namespace asn_string_utils

/*********************
      ext group
*********************/

bool& ext_groups_packer_guard::operator[](uint32_t idx)
{
  if (idx >= groups.size()) {
    uint32_t prev_size = groups.size();
    groups.resize(idx + 1);
    std::fill(&groups[prev_size], &groups[groups.size()], false);
  }
  return groups[idx];
}

SRSASN_CODE ext_groups_packer_guard::pack(asn1::bit_ref& bref) const
{
  // pack number of groups
  int32_t i = groups.size() - 1;
  for (; i >= 0; --i) {
    if (groups[i]) {
      break;
    }
  }
  uint32_t nof_groups = (uint32_t)i + 1u;
  HANDLE_CODE(pack_norm_small_non_neg_whole_number(bref, nof_groups - 1));

  // pack each group presence flag
  for (uint32_t j = 0; j < nof_groups; ++j) {
    HANDLE_CODE(bref.pack(groups[j], 1));
  }
  return SRSASN_SUCCESS;
}

ext_groups_unpacker_guard::ext_groups_unpacker_guard(uint32_t nof_supported_groups_) :
  nof_supported_groups(nof_supported_groups_)
{
  resize(nof_supported_groups);
}

bool& ext_groups_unpacker_guard::operator[](uint32_t idx)
{
  if (idx >= groups.size()) {
    // only resizes for unknown extensions
    resize(idx + 1);
  }
  return groups[idx];
}

void ext_groups_unpacker_guard::resize(uint32_t new_size)
{
  // always grows
  uint32_t prev_size = groups.size();
  groups.resize(std::max(new_size, nof_supported_groups));
  std::fill(&groups[prev_size], &groups[groups.size()], false);
}

ext_groups_unpacker_guard::~ext_groups_unpacker_guard()
{
  // consume all the unknown extensions
  for (uint32_t i = nof_supported_groups; i < nof_unpacked_groups; ++i) {
    if (groups[i]) {
      varlength_field_unpack_guard scope(*bref_tracker);
    }
  }
}

SRSASN_CODE ext_groups_unpacker_guard::unpack(cbit_ref& bref)
{
  bref_tracker = &bref;
  // unpack nof of ext groups
  HANDLE_CODE(unpack_norm_small_non_neg_whole_number(nof_unpacked_groups, bref));
  nof_unpacked_groups += 1;
  resize(nof_unpacked_groups);

  // unpack each group presence flag
  for (uint32_t i = 0; i < nof_unpacked_groups; ++i) {
    HANDLE_CODE(bref.unpack(groups[i], 1));
  }
  return SRSASN_SUCCESS;
}

/*********************
     Open Field
*********************/

varlength_field_pack_guard::varlength_field_pack_guard(bit_ref& bref, bool align_)
{
  brefstart    = bref;
  bref         = bit_ref(&buffer[0], sizeof(buffer));
  bref_tracker = &bref;
  align        = align_;
}

varlength_field_pack_guard::~varlength_field_pack_guard()
{
  // fill the spare bits
  const bit_ref bref0    = bit_ref(&buffer[0], sizeof(buffer));
  uint32_t      leftover = 7 - ((bref_tracker->distance(bref0) - (uint32_t)1) % (uint32_t)8);
  bref_tracker->pack(0, leftover);

  // check how many bytes were written in total
  uint32_t nof_bytes = bref_tracker->distance(bref0) / (uint32_t)8;
  if (nof_bytes > sizeof(buffer)) {
    log_error("The packed variable sized field is too long for the reserved buffer (%zd > %zd)\n",
              (size_t)nof_bytes,
              sizeof(buffer));
  }

  // go back in time to pack length
  pack_length(brefstart, nof_bytes, align);

  // pack encoded bytes
  for (uint32_t i = 0; i < nof_bytes; ++i) {
    brefstart.pack(buffer[i], 8);
  }
  *bref_tracker = brefstart;
}

varlength_field_unpack_guard::varlength_field_unpack_guard(cbit_ref& bref, bool align)
{
  unpack_length(len, bref, align);
  bref0        = bref;
  bref_tracker = &bref;
}

varlength_field_unpack_guard::~varlength_field_unpack_guard()
{
  uint32_t pad;
  bref_tracker->unpack(pad, len * 8 - bref_tracker->distance(bref0));
}

/*******************
    JsonWriter
*******************/

json_writer::json_writer() : ident(""), sep(NONE) {}

void json_writer::write_fieldname(const std::string& fieldname)
{
  if (sep == COMMA) {
    ss << ",\n" << ident;
  } else if (sep == NEWLINE) {
    ss << "\n" << ident;
  }
  if (not fieldname.empty()) {
    ss << "\"" << fieldname << "\": ";
  }
  sep = NONE;
}

void json_writer::write_str(const std::string& fieldname, const std::string& value)
{
  write_fieldname(fieldname);
  ss << "\"" << value << "\"";
  sep = COMMA;
}
void json_writer::write_str(const std::string& value)
{
  write_str("", value);
}

void json_writer::write_int(const std::string& fieldname, int64_t value)
{
  write_fieldname(fieldname);
  ss << value;
  sep = COMMA;
}
void json_writer::write_int(int64_t value)
{
  write_int("", value);
}

void json_writer::write_bool(const std::string& fieldname, bool value)
{
  write_fieldname(fieldname);
  ss << (value ? "true" : "false");
  sep = COMMA;
}
void json_writer::write_bool(bool value)
{
  write_bool("", value);
}

void json_writer::write_null(const std::string& fieldname)
{
  write_fieldname(fieldname);
  ss << "null";
  sep = COMMA;
}
void json_writer::write_null()
{
  write_null("");
}

void json_writer::start_obj(const std::string& fieldname)
{
  write_fieldname(fieldname);
  ss << "{";
  ident += "  ";
  sep = NEWLINE;
}
void json_writer::end_obj()
{
  ident.erase(ident.size() - 2, 2);
  ss << "\n" << ident << "}";
  sep = COMMA;
}
void json_writer::start_array(const std::string& fieldname)
{
  write_fieldname(fieldname);
  ss << "[";
  ident += "  ";
  sep = NEWLINE;
}
void json_writer::end_array()
{
  ident.erase(ident.size() - 2, 2);
  ss << "\n" << ident << "]";
  sep = COMMA;
}

std::string json_writer::to_string() const
{
  return ss.str();
}

} // namespace asn1
