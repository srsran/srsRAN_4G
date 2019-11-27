/*
 * Copyright 2013-2019 Software Radio Systems Limited
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
#include <cmath>
#include <stdio.h>

namespace asn1 {

/************************
        logging
************************/

void vlog_print(log_handler_t handler, void* ctx, srsasn_logger_level_t log_level, const char* format, va_list args)
{
  if (handler) {
    char* args_msg = NULL;
    if (vasprintf(&args_msg, format, args) > 0) {
      handler(log_level, ctx, args_msg);
    }
    if (args_msg) {
      free(args_msg);
    }
  } else {
    vprintf(format, args);
  }
}

static log_handler_t log_handler;
static void*         callback_ctx = NULL;

void srsasn_log_register_handler(void* ctx, log_handler_t handler)
{
  log_handler  = handler;
  callback_ctx = ctx;
}

void srsasn_log_print(srsasn_logger_level_t log_level, const char* format, ...)
{
  va_list args;
  va_start(args, format);
  vlog_print(log_handler, callback_ctx, log_level, format, args);
  va_end(args);
}

/************************
     error handling
************************/

void log_error_code(SRSASN_CODE code, const char* filename, int line)
{
  switch (code) {
    case SRSASN_ERROR_ENCODE_FAIL:
      srsasn_log_print(LOG_LEVEL_ERROR, "[%s][%d] Encoding failure.\n", filename, line);
      break;
    case SRSASN_ERROR_DECODE_FAIL:
      srsasn_log_print(LOG_LEVEL_ERROR, "[%s][%d] Decoding failure.\n", filename, line);
      break;
    default:
      srsasn_log_print(LOG_LEVEL_WARN, "[%s][%d] SRSASN_CODE=%d not recognized.\n", filename, line, (uint32_t)code);
  }
}

/*********************
       bit_ref
*********************/

int bit_ref::distance(const bit_ref& other) const
{
  return ((int)offset - (int)other.offset) + 8 * ((int)(ptr - other.ptr));
}
int bit_ref::distance(uint8_t* ref_ptr) const
{
  return (int)offset + 8 * ((int)(ptr - ref_ptr));
}
int bit_ref::distance() const
{
  return (int)offset + 8 * ((int)(ptr - start_ptr));
}
int bit_ref::distance_bytes(uint8_t* ref_ptr) const
{
  return ((int)(ptr - ref_ptr)) + ((offset) ? 1 : 0);
}
int bit_ref::distance_bytes() const
{
  return ((int)(ptr - start_ptr)) + ((offset) ? 1 : 0);
}

SRSASN_CODE bit_ref::pack(uint32_t val, uint32_t n_bits)
{
  if (n_bits >= 32) {
    srsasn_log_print(LOG_LEVEL_ERROR, "This method only supports packing up to 32 bits\n");
    return SRSASN_ERROR_ENCODE_FAIL;
  }
  uint32_t mask;
  while (n_bits > 0) {
    if (ptr >= max_ptr) {
      srsasn_log_print(LOG_LEVEL_ERROR, "Buffer size limit was achieved\n");
      return SRSASN_ERROR_ENCODE_FAIL;
    }
    mask             = ((1u << n_bits) - 1u);
    val              = val & mask;
    uint8_t keepmask = ((uint8_t)-1) - (uint8_t)((1 << (8 - offset)) - 1);
    if ((uint32_t)(8 - offset) > n_bits) {
      uint8_t bit = (uint8_t)(val << (8u - offset - n_bits));
      *ptr        = ((*ptr) & keepmask) + bit;
      offset += n_bits;
      n_bits = 0;
    } else {
      uint8_t bit = (uint8_t)(val >> (n_bits - 8u + offset));
      *ptr        = (*ptr & keepmask) + bit;
      n_bits -= (8 - offset);
      offset = 0;
      ptr++;
    }
  }
  return SRSASN_SUCCESS;
}

SRSASN_CODE bit_ref::pack_bytes(const uint8_t* buf, uint32_t n_bytes)
{
  if (n_bytes == 0) {
    return SRSASN_SUCCESS;
  }
  if (ptr + n_bytes >= max_ptr) {
    srsasn_log_print(LOG_LEVEL_ERROR, "Buffer size limit was achieved\n");
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

ValOrError unpack_bits(uint8_t*& ptr, uint8_t& offset, uint8_t* max_ptr, uint32_t n_bits)
{
  if (n_bits > 32) {
    srsasn_log_print(LOG_LEVEL_ERROR, "This method only supports unpacking up to 32 bits\n");
    return {0, SRSASN_ERROR_DECODE_FAIL};
  }
  uint32_t val = 0;
  while (n_bits > 0) {
    if (ptr >= max_ptr) {
      srsasn_log_print(LOG_LEVEL_ERROR, "Buffer size limit was achieved\n");
      return ValOrError(val, SRSASN_ERROR_DECODE_FAIL);
    }
    if ((uint32_t)(8 - offset) > n_bits) {
      uint8_t mask = (uint8_t)(1u << (8u - offset)) - (uint8_t)(1u << (8u - offset - n_bits));
      val += ((uint32_t)((*ptr) & mask)) >> ((uint8_t)8 - offset - n_bits);
      offset += n_bits;
      n_bits = 0;
    } else {
      uint8_t mask = (uint8_t)((1u << (8u - offset)) - 1);
      val += ((uint32_t)((*ptr) & mask)) << (n_bits - 8 + offset);
      n_bits -= 8 - offset;
      offset = 0;
      ptr++;
    }
  }
  return ValOrError(val, SRSASN_SUCCESS);
}

SRSASN_CODE bit_ref::unpack_bytes(uint8_t* buf, uint32_t n_bytes)
{
  if (n_bytes == 0) {
    return SRSASN_SUCCESS;
  }
  if (ptr + n_bytes >= max_ptr) {
    srsasn_log_print(LOG_LEVEL_ERROR, "Buffer size limit was achieved\n");
    return SRSASN_ERROR_DECODE_FAIL;
  }
  if (offset == 0) {
    // Aligned case
    memcpy(buf, ptr, n_bytes);
    ptr += n_bytes;
  } else {
    for (uint32_t i = 0; i < n_bytes; ++i) {
      unpack(buf[i], 8);
    }
  }
  return SRSASN_SUCCESS;
}

SRSASN_CODE bit_ref::align_bytes()
{
  if (offset == 0)
    return SRSASN_SUCCESS;
  if (ptr >= max_ptr) {
    srsasn_log_print(LOG_LEVEL_ERROR, "Buffer size limit was achieved\n");
    return SRSASN_ERROR_DECODE_FAIL;
  }
  offset = 0;
  ptr++;
  return SRSASN_SUCCESS;
}

SRSASN_CODE bit_ref::align_bytes_zero()
{
  if (offset == 0)
    return SRSASN_SUCCESS;
  if (ptr >= max_ptr) {
    srsasn_log_print(LOG_LEVEL_ERROR, "Buffer size limit was achieved\n");
    return SRSASN_ERROR_ENCODE_FAIL;
  }
  uint8_t mask = (uint8_t)(256u - (1u << (8u - offset)));
  *ptr &= mask;
  offset = 0;
  ptr++;
  return SRSASN_SUCCESS;
}

SRSASN_CODE bit_ref::advance_bits(uint32_t n_bits)
{
  uint32_t extra_bits     = (offset + n_bits) % 8;
  uint32_t bytes_required = ceilf((offset + n_bits) / 8.0f);
  uint32_t bytes_offset   = floorf((offset + n_bits) / 8.0f);

  if (ptr + bytes_required >= max_ptr) {
    srsasn_log_print(LOG_LEVEL_ERROR, "Buffer size limit was achieved\n");
    return SRSASN_ERROR_DECODE_FAIL;
  }
  ptr += bytes_offset;
  offset = extra_bits;
  return SRSASN_SUCCESS;
}

void bit_ref::set(uint8_t* start_ptr_, uint32_t max_size_)
{
  ptr       = start_ptr_;
  offset    = 0;
  start_ptr = start_ptr_;
  max_ptr   = max_size_ + start_ptr_;
}

/*********************
     ext packing
*********************/

SRSASN_CODE pack_unsupported_ext_flag(bit_ref& bref, bool ext)
{
  HANDLE_CODE(bref.pack(ext, 1));
  if (ext) {
    srsasn_log_print(LOG_LEVEL_ERROR, "asn1 error: ASN extensions not currently supported\n");
    return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

SRSASN_CODE unpack_unsupported_ext_flag(bool& ext, bit_ref& bref)
{
  SRSASN_CODE ret = bref.unpack(ext, 1);
  if (ext) {
    srsasn_log_print(LOG_LEVEL_ERROR, "asn1 error: ASN extensions not currently supported\n");
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
    ret = pack_norm_small_integer(bref, enum_val - nof_noext);
  }
  return ret;
}

SRSASN_CODE pack_enum(bit_ref& bref, uint32_t e, uint32_t nof_types, uint32_t nof_exts, bool has_ext)
{
  if (e >= nof_types) {
    srsasn_log_print(
        LOG_LEVEL_ERROR, "The provided enum is not within the range of possible values (%d>=%d)\n", e, nof_types);
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

ValOrError unpack_enum(uint32_t nof_types, uint32_t nof_exts, bool has_ext, bit_ref& bref)
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
      ret.code = unpack_norm_small_integer(ret.val, bref);
      ret.val += nof_types - nof_exts;
    }
  } else {
    uint32_t nof_bits = (uint32_t)ceilf(log2f(nof_types));
    ret.code          = bref.unpack(ret.val, nof_bits);
  }
  if (ret.val >= nof_types) {
    srsasn_log_print(
        LOG_LEVEL_ERROR, "The provided enum is not within the range of possible values (%d>=%d)\n", ret.val, nof_types);
    ret.code = SRSASN_ERROR_DECODE_FAIL;
  }
  return ret;
}

/************************
       pack int
************************/

/**
 * Encoder function for a constrained whole unaligned number
 * @tparam IntType type of integer (e.g. uint8_t, uint32_t, etc.)
 * @param bref bit_ref where to encode integer
 * @param n integer current value
 * @param lb
 * @param ub
 * @return success or failure
 */
template <class IntType>
SRSASN_CODE pack_unalign_integer(bit_ref& bref, IntType n, IntType lb, IntType ub)
{
  if (ub < lb or n < lb or n > ub) {
    srsasn_log_print(LOG_LEVEL_ERROR, "The condition lb <= n <= ub (%d <= %d <= %d) was not met\n", lb, n, ub);
    return SRSASN_ERROR_ENCODE_FAIL;
  }
  uint64_t ra = ub - lb + 1; // NOTE: Can overflow if IntType is kept
  if (ra != 1) {
    uint32_t n_bits   = (uint32_t)ceilf(log2f((float)ra));
    IntType  toencode = n - lb;
    bref.pack(toencode, n_bits);
  }
  return SRSASN_SUCCESS;
}
/**
 * Encoder function for a constrained whole unaligned number
 * @tparam IntType type of integer (e.g. uint8_t, uint32_t, etc.)
 * @param bref bit_ref where to encode integer
 * @param n integer current value
 * @param lb
 * @param ub
 * @return success or failure
 */
template <class IntType>
SRSASN_CODE unpack_unalign_integer(IntType& n, bit_ref& bref, IntType lb, IntType ub)
{
  if (ub < lb) {
    srsasn_log_print(LOG_LEVEL_ERROR, "The condition lb <= ub (%d <= %d) was not met\n", lb, ub);
    return SRSASN_ERROR_DECODE_FAIL;
  }
  uint64_t ra = ub - lb + 1; // NOTE: Can overflow if IntType is kept.
  if (ra == 1) {
    n = lb;
    return SRSASN_SUCCESS;
  }
  uint32_t    n_bits = (uint32_t)ceilf(log2f(ra));
  SRSASN_CODE ret    = bref.unpack(n, n_bits);
  n += lb;
  if (n < lb or n > ub) {
    srsasn_log_print(LOG_LEVEL_ERROR, "The condition lb <= n <= ub (%d <= %d <= %d) was not met\n", lb, n, ub);
    return SRSASN_ERROR_DECODE_FAIL;
  }
  return ret;
}
template SRSASN_CODE pack_unalign_integer<int8_t>(bit_ref& bref, int8_t n, int8_t lb, int8_t ub);
template SRSASN_CODE pack_unalign_integer<int16_t>(bit_ref& bref, int16_t n, int16_t lb, int16_t ub);
template SRSASN_CODE pack_unalign_integer<int32_t>(bit_ref& bref, int32_t n, int32_t lb, int32_t ub);
template SRSASN_CODE pack_unalign_integer<int64_t>(bit_ref& bref, int64_t n, int64_t lb, int64_t ub);
template SRSASN_CODE pack_unalign_integer<uint8_t>(bit_ref& bref, uint8_t n, uint8_t lb, uint8_t ub);
template SRSASN_CODE pack_unalign_integer<uint16_t>(bit_ref& bref, uint16_t n, uint16_t lb, uint16_t ub);
template SRSASN_CODE pack_unalign_integer<uint32_t>(bit_ref& bref, uint32_t n, uint32_t lb, uint32_t ub);
template SRSASN_CODE pack_unalign_integer<uint64_t>(bit_ref& bref, uint64_t n, uint64_t lb, uint64_t ub);
template SRSASN_CODE unpack_unalign_integer<int8_t>(int8_t& n, bit_ref& bref, int8_t lb, int8_t ub);
template SRSASN_CODE unpack_unalign_integer<int16_t>(int16_t& n, bit_ref& bref, int16_t lb, int16_t ub);
template SRSASN_CODE unpack_unalign_integer<int32_t>(int32_t& n, bit_ref& bref, int32_t lb, int32_t ub);
template SRSASN_CODE unpack_unalign_integer<int64_t>(int64_t& n, bit_ref& bref, int64_t lb, int64_t ub);
template SRSASN_CODE unpack_unalign_integer<uint8_t>(uint8_t& n, bit_ref& bref, uint8_t lb, uint8_t ub);
template SRSASN_CODE unpack_unalign_integer<uint16_t>(uint16_t& n, bit_ref& bref, uint16_t lb, uint16_t ub);
template SRSASN_CODE unpack_unalign_integer<uint32_t>(uint32_t& n, bit_ref& bref, uint32_t lb, uint32_t ub);
template SRSASN_CODE unpack_unalign_integer<uint64_t>(uint64_t& n, bit_ref& bref, uint64_t lb, uint64_t ub);

template <class IntType>
SRSASN_CODE UnalignedIntegerPacker<IntType>::pack(bit_ref& bref, IntType n) const
{
  return pack_unalign_integer(bref, n, lb, ub);
}
template <class IntType>
SRSASN_CODE UnalignedIntegerPacker<IntType>::unpack(IntType& n, bit_ref& bref) const
{
  return unpack_unalign_integer(n, bref, lb, ub);
}
template struct UnalignedIntegerPacker<int8_t>;
template struct UnalignedIntegerPacker<int16_t>;
template struct UnalignedIntegerPacker<int32_t>;
template struct UnalignedIntegerPacker<int64_t>;
template struct UnalignedIntegerPacker<uint8_t>;
template struct UnalignedIntegerPacker<uint16_t>;
template struct UnalignedIntegerPacker<uint32_t>;
template struct UnalignedIntegerPacker<uint64_t>;

/**
 * Encoder of a constrained whole aligned number
 * @tparam IntType type of integer (e.g. uint8_t, uint32_t, etc.)
 * @param bref bit_ref where to encode integer
 * @param n value of integer to pack
 * @param lb
 * @param ub
 * @return
 */
template <class IntType>
SRSASN_CODE pack_align_integer(bit_ref& bref, IntType n, IntType lb, IntType ub)
{
  // FIXME: Bound checker.
  IntType ra = ub - lb + 1;
  if (ra == 1) {
    return SRSASN_SUCCESS;
  }
  uint32_t    n_bits   = (uint32_t)ceilf(log2f(ra)); // bit-field size
  IntType     toencode = n - lb;
  SRSASN_CODE ret;
  if (ra < 256) {
    ret = bref.pack(toencode, n_bits);
  } else if (ra <= 65536) {
    uint32_t n_octets = (n_bits + 7) / 8;
    HANDLE_CODE(bref.align_bytes_zero());
    HANDLE_CODE(bref.pack(0, n_octets * 8 - n_bits));
    HANDLE_CODE(bref.pack(toencode, n_bits));
    ret = bref.align_bytes_zero();
  } else {
    // TODO: Check if this is correct
    uint32_t n_bits_len = (uint32_t)ceilf(log2f((n_bits + 7) / 8));
    n_bits              = (uint32_t)floorf(log2f(toencode) + 1);
    uint32_t n_octets   = (uint32_t)((n_bits + 7) / 8);
    HANDLE_CODE(bref.pack(n_octets - 1, n_bits_len));
    HANDLE_CODE(bref.align_bytes_zero());
    HANDLE_CODE(bref.pack(0, (n_octets * 8) - n_bits));
    ret = bref.pack(toencode, n_bits);
  }
  return ret;
}
template SRSASN_CODE pack_align_integer<uint8_t>(bit_ref& bref, uint8_t n, uint8_t lb, uint8_t ub);
template SRSASN_CODE pack_align_integer<uint16_t>(bit_ref& bref, uint16_t n, uint16_t lb, uint16_t ub);
template SRSASN_CODE pack_align_integer<uint32_t>(bit_ref& bref, uint32_t n, uint32_t lb, uint32_t ub);
template SRSASN_CODE pack_align_integer<uint64_t>(bit_ref& bref, uint64_t n, uint64_t lb, uint64_t ub);
template SRSASN_CODE pack_align_integer<int8_t>(bit_ref& bref, int8_t n, int8_t lb, int8_t ub);
template SRSASN_CODE pack_align_integer<int16_t>(bit_ref& bref, int16_t n, int16_t lb, int16_t ub);
template SRSASN_CODE pack_align_integer<int32_t>(bit_ref& bref, int32_t n, int32_t lb, int32_t ub);
template SRSASN_CODE pack_align_integer<int64_t>(bit_ref& bref, int64_t n, int64_t lb, int64_t ub);

template <typename IntType>
SRSASN_CODE unpack_align_integer(IntType& n, bit_ref& bref, IntType lb, IntType ub)
{
  uint32_t ra = ub - lb + 1;
  if (ra == 1) {
    n = lb;
    return SRSASN_SUCCESS;
  }
  SRSASN_CODE ret    = SRSASN_SUCCESS;
  uint32_t    n_bits = (uint32_t)ceil(log2(ra));
  if (ra < 256) {
    ret = bref.unpack(n, n_bits);
  } else if (ra < 65536) {
    uint32_t n_octets = (n_bits + 7) / 8;
    HANDLE_CODE(bref.align_bytes());
    HANDLE_CODE(bref.unpack(n, n_octets * 8));
    HANDLE_CODE(bref.align_bytes());
  } else {
    uint32_t n_bits_len = (uint32_t)ceilf(log2f((n_bits + 7) / 8));
    uint32_t n_octets;
    HANDLE_CODE(bref.unpack(n_octets, n_bits_len));
    n_octets += 1;
    HANDLE_CODE(bref.align_bytes());
    HANDLE_CODE(bref.unpack(n, n_octets * 8));
    n += lb;
  }
  n += lb;
  return ret;
}
template SRSASN_CODE unpack_align_integer<uint8_t>(uint8_t& intval, bit_ref& bref, uint8_t lb, uint8_t ub);
template SRSASN_CODE unpack_align_integer<uint16_t>(uint16_t& intval, bit_ref& bref, uint16_t lb, uint16_t ub);
template SRSASN_CODE unpack_align_integer<uint32_t>(uint32_t& intval, bit_ref& bref, uint32_t lb, uint32_t ub);
template SRSASN_CODE unpack_align_integer<uint64_t>(uint64_t& intval, bit_ref& bref, uint64_t lb, uint64_t ub);
template SRSASN_CODE unpack_align_integer<int8_t>(int8_t& intval, bit_ref& bref, int8_t lb, int8_t ub);
template SRSASN_CODE unpack_align_integer<int16_t>(int16_t& intval, bit_ref& bref, int16_t lb, int16_t ub);
template SRSASN_CODE unpack_align_integer<int32_t>(int32_t& intval, bit_ref& bref, int32_t lb, int32_t ub);
template SRSASN_CODE unpack_align_integer<int64_t>(int64_t& intval, bit_ref& bref, int64_t lb, int64_t ub);

template <class IntType>
AlignedIntegerPacker<IntType>::AlignedIntegerPacker(IntType lb_, IntType ub_) : lb(lb_), ub(ub_)
{
}
template <class IntType>
SRSASN_CODE AlignedIntegerPacker<IntType>::pack(bit_ref& bref, IntType n)
{
  return pack_align_integer(bref, n, lb, ub);
}
template <class IntType>
SRSASN_CODE AlignedIntegerPacker<IntType>::unpack(IntType& n, bit_ref& bref)
{
  return unpack_align_integer(n, bref, lb, ub);
}
template struct AlignedIntegerPacker<int8_t>;
template struct AlignedIntegerPacker<int16_t>;
template struct AlignedIntegerPacker<int32_t>;
template struct AlignedIntegerPacker<int64_t>;
template struct AlignedIntegerPacker<uint8_t>;
template struct AlignedIntegerPacker<uint16_t>;
template struct AlignedIntegerPacker<uint32_t>;
template struct AlignedIntegerPacker<uint64_t>;

template <typename UintType>
SRSASN_CODE pack_norm_small_integer(bit_ref& bref, UintType n)
{
  if (n <= 63) {
    HANDLE_CODE(bref.pack(0, 1));
    HANDLE_CODE(bref.pack(n, 6));
  } else {
    HANDLE_CODE(bref.pack(1, 1));
    srsasn_log_print(LOG_LEVEL_ERROR, "Long small integers not supported\n");
    return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
template <typename UintType>
SRSASN_CODE unpack_norm_small_integer(UintType& n, bit_ref& bref)
{
  bool        ext;
  SRSASN_CODE ret = bref.unpack(ext, 1);
  HANDLE_CODE(ret);
  if (not ext) {
    ret = bref.unpack(n, 6);
  } else {
    srsasn_log_print(LOG_LEVEL_ERROR, "Long small integers not supported\n");
    return SRSASN_ERROR_DECODE_FAIL;
  }
  return ret;
}
template SRSASN_CODE pack_norm_small_integer<uint8_t>(bit_ref& bref, uint8_t n);
template SRSASN_CODE pack_norm_small_integer<uint16_t>(bit_ref& bref, uint16_t n);
template SRSASN_CODE pack_norm_small_integer<uint32_t>(bit_ref& bref, uint32_t n);
template SRSASN_CODE pack_norm_small_integer<uint64_t>(bit_ref& bref, uint64_t n);
template SRSASN_CODE unpack_norm_small_integer<uint8_t>(uint8_t& n, bit_ref& bref);
template SRSASN_CODE unpack_norm_small_integer<uint16_t>(uint16_t& n, bit_ref& bref);
template SRSASN_CODE unpack_norm_small_integer<uint32_t>(uint32_t& n, bit_ref& bref);
template SRSASN_CODE unpack_norm_small_integer<uint64_t>(uint64_t& n, bit_ref& bref);

/*********************
   varlength_packing
*********************/

template <typename IntType>
SRSASN_CODE pack_length(bit_ref& bref, IntType n, IntType lb, IntType ub)
{
  // TODO: FIXME if range higher than 64K
  return pack_unalign_integer(bref, n, lb, ub);
}
template SRSASN_CODE pack_length<uint8_t>(bit_ref& bref, uint8_t n, uint8_t lb, uint8_t ub);
template SRSASN_CODE pack_length<uint16_t>(bit_ref& bref, uint16_t n, uint16_t lb, uint16_t ub);
template SRSASN_CODE pack_length<uint32_t>(bit_ref& bref, uint32_t n, uint32_t lb, uint32_t ub);
template SRSASN_CODE pack_length<uint64_t>(bit_ref& bref, uint64_t n, uint64_t lb, uint64_t ub);
template SRSASN_CODE pack_length<int8_t>(bit_ref& bref, int8_t n, int8_t lb, int8_t ub);
template SRSASN_CODE pack_length<int16_t>(bit_ref& bref, int16_t n, int16_t lb, int16_t ub);
template SRSASN_CODE pack_length<int32_t>(bit_ref& bref, int32_t n, int32_t lb, int32_t ub);
template SRSASN_CODE pack_length<int64_t>(bit_ref& bref, int64_t n, int64_t lb, int64_t ub);
template <typename IntType>
SRSASN_CODE unpack_length(IntType& n, bit_ref& bref, IntType lb, IntType ub)
{
  // TODO: FIXME if range higher than 64K
  return unpack_unalign_integer(n, bref, lb, ub);
}

template SRSASN_CODE unpack_length<uint8_t>(uint8_t& n, bit_ref& bref, uint8_t lb, uint8_t ub);
template SRSASN_CODE unpack_length<uint16_t>(uint16_t& n, bit_ref& bref, uint16_t lb, uint16_t ub);
template SRSASN_CODE unpack_length<uint32_t>(uint32_t& n, bit_ref& bref, uint32_t lb, uint32_t ub);
template SRSASN_CODE unpack_length<uint64_t>(uint64_t& n, bit_ref& bref, uint64_t lb, uint64_t ub);
template SRSASN_CODE unpack_length<int8_t>(int8_t& n, bit_ref& bref, int8_t lb, int8_t ub);
template SRSASN_CODE unpack_length<int16_t>(int16_t& n, bit_ref& bref, int16_t lb, int16_t ub);
template SRSASN_CODE unpack_length<int32_t>(int32_t& n, bit_ref& bref, int32_t lb, int32_t ub);
template SRSASN_CODE unpack_length<int64_t>(int64_t& n, bit_ref& bref, int64_t lb, int64_t ub);

SRSASN_CODE pack_length(bit_ref& bref, uint32_t val)
{
  if (val < 128) {
    HANDLE_CODE(bref.pack(0, 1));
    HANDLE_CODE(bref.pack(val, 7));
  } else if (val < ASN_16K) {
    HANDLE_CODE(bref.pack(1, 1));
    HANDLE_CODE(bref.pack(0, 1));
    HANDLE_CODE(bref.pack(val, 14));
  } else {
    srsasn_log_print(LOG_LEVEL_ERROR, "Not handling sizes longer than 16383 octets\n");
    return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

SRSASN_CODE unpack_length(uint32_t& val, bit_ref& bref)
{
  bool        ext;
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
      srsasn_log_print(LOG_LEVEL_ERROR, "Not handling octet strings longer than 16383 octets\n");
      val = 0;
      return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  return ret;
}

/************************
    common octstring
************************/

uint64_t octstring_to_number(const uint8_t* ptr, uint32_t nbytes)
{
  if (nbytes > 8) {
    srsasn_log_print(LOG_LEVEL_ERROR, "octstring of size=%d does not fit in an uint64_t\n", nbytes);
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
    srsasn_log_print(LOG_LEVEL_ERROR, "octstring of size=%d does not fit in an uint64_t\n", nbytes);
    return;
  }
  for (uint32_t i = 0; i < nbytes; ++i) {
    ptr[nbytes - 1 - i] = (number >> (uint64_t)(i * 8u)) & 0xFF;
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
    srsasn_log_print(LOG_LEVEL_WARN, "The provided hex string size=%zd is not a multiple of 2\n.", str.size());
  }
  char cstr[] = "\0\0\0";
  for (uint32_t i = 0; i < str.size(); i += 2) {
    memcpy(&cstr[0], &str[i], 2);
    ptr[i / 2] = strtoul(cstr, NULL, 16);
  }
}

/************************
     dyn_octstring
************************/

SRSASN_CODE dyn_octstring::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_length(bref, size()));
  for (uint32_t i = 0; i < size(); ++i) {
    HANDLE_CODE(bref.pack(octets_[i], 8));
  }
  return SRSASN_SUCCESS;
}

SRSASN_CODE dyn_octstring::unpack(bit_ref& bref)
{
  uint32_t len;
  HANDLE_CODE(unpack_length(len, bref));
  resize(len);
  for (uint32_t i = 0; i < size(); ++i) {
    HANDLE_CODE(bref.unpack(octets_[i], 8));
  }
  return SRSASN_SUCCESS;
}

std::string dyn_octstring::to_string() const
{
  return octstring_to_string(&octets_[0], size());
}

dyn_octstring& dyn_octstring::from_string(const std::string& hexstr)
{
  resize(hexstr.size() / 2);
  string_to_octstring(&octets_[0], hexstr);
  return *this;
}

/*********************
      bitstring
*********************/

SRSASN_CODE pack_common_bitstring(bit_ref& bref, const uint8_t* buf, uint32_t nbits)
{
  if (nbits == 0) {
    srsasn_log_print(LOG_LEVEL_ERROR, "Invalid bitstring size=%d\n", nbits);
    return SRSASN_ERROR_ENCODE_FAIL;
  }
  uint32_t n_octs = (uint32_t)ceilf(nbits / 8.0f);
  uint32_t offset = ((nbits - 1) % 8) + 1;
  HANDLE_CODE(bref.pack(buf[n_octs - 1], offset));
  for (uint32_t i = 1; i < n_octs; ++i) {
    HANDLE_CODE(bref.pack(buf[n_octs - 1 - i], 8));
  }
  return SRSASN_SUCCESS;
}

SRSASN_CODE unpack_common_bitstring(uint8_t* buf, bit_ref& bref, uint32_t nbits)
{
  if (nbits == 0) {
    srsasn_log_print(LOG_LEVEL_ERROR, "Invalid bitstring size=%d\n", nbits);
    return SRSASN_ERROR_DECODE_FAIL;
  }
  uint32_t n_octs = (uint32_t)ceilf(nbits / 8.0f);
  uint32_t offset = ((nbits - 1) % 8) + 1;
  HANDLE_CODE(bref.unpack(buf[n_octs - 1], offset));
  for (uint32_t i = 1; i < n_octs; ++i) {
    HANDLE_CODE(bref.unpack(buf[n_octs - 1 - i], 8));
  }
  return SRSASN_SUCCESS;
}

uint64_t bitstring_to_number(const uint8_t* ptr, uint32_t nbits)
{
  if (nbits > 64) {
    srsasn_log_print(LOG_LEVEL_ERROR, "bitstring of size=%d does not fit in an uint64_t\n", nbits);
    return 0;
  }
  uint64_t val       = 0;
  uint32_t nof_bytes = (uint32_t)ceilf(nbits / 8.0f);
  for (uint32_t i = 0; i < nof_bytes; ++i) {
    val += ptr[i] << (i * 8);
  }
  return val;
}
void number_to_bitstring(uint8_t* ptr, uint64_t number, uint32_t nbits)
{
  if (nbits > 64) {
    srsasn_log_print(LOG_LEVEL_ERROR, "bitstring of size=%d does not fit in an uint64_t\n", nbits);
    return;
  }
  uint32_t nof_bytes = (uint32_t)ceilf(nbits / 8.0f);
  for (uint32_t i = 0; i < nof_bytes; ++i) {
    ptr[i] = (number >> (i * 8)) & 0xFF;
  }
  uint32_t offset = nbits % 8; // clean up any extra set bit
  if (offset > 0) {
    ptr[nof_bytes - 1] &= (uint8_t)((1 << offset) - 1);
  }
}

std::string bitstring_to_string(const uint8_t* ptr, uint32_t nbits)
{
  std::string str(nbits, '\0');
  for (uint32_t i = 0; i < nbits; ++i) {
    str[i] = bitstring_get(ptr, nbits - 1 - i) ? '1' : '0';
  }
  return str;
}

/*********************
   fixed_bitstring
*********************/

SRSASN_CODE pack_fixed_bitstring(bit_ref& bref, const uint8_t* buf, uint32_t nbits)
{
  //  if(nbits > 16) { // X.691 Section 15
  //    bref.align_bytes_zero();
  //  }
  return pack_common_bitstring(bref, buf, nbits);
  //  if(nbits > 16) { // X.691 Section 15
  //    bref.align_bytes_zero();
  //  }
}

SRSASN_CODE pack_fixed_bitstring(bit_ref& bref, const uint8_t* buf, uint32_t nbits, bool ext)
{
  HANDLE_CODE(pack_unsupported_ext_flag(bref, ext));
  HANDLE_CODE(pack_fixed_bitstring(bref, buf, nbits));
  return SRSASN_SUCCESS;
}

SRSASN_CODE unpack_fixed_bitstring(uint8_t* buf, bit_ref& bref, uint32_t nbits)
{
  //  if(nbits > 16) { // X.691 Section 15
  //    bref.align_bytes_zero();
  //  }
  return unpack_common_bitstring(buf, bref, nbits);
  //  if(nbits > 16) { // X.691 Section 15
  //    bref.align_bytes_zero();
  //  }
}

SRSASN_CODE unpack_fixed_bitstring(uint8_t* buf, bool& ext, bit_ref& bref, uint32_t nbits)
{
  HANDLE_CODE(unpack_unsupported_ext_flag(ext, bref));
  HANDLE_CODE(unpack_fixed_bitstring(buf, bref, nbits));
  return SRSASN_SUCCESS;
}

/*********************
    dyn_bitstring
*********************/

dyn_bitstring::dyn_bitstring(uint32_t n_bits_) : n_bits(n_bits_)
{
  uint32_t n_bytes = nof_octets();
  octets_.resize(n_bytes);
  memset(&octets_[0], 0, n_bytes);
}

dyn_bitstring::dyn_bitstring(const char* s)
{
  n_bits           = (uint32_t)strlen(s);
  uint32_t n_bytes = nof_octets();
  octets_.resize(n_bytes);
  memset(&octets_[0], 0, n_bytes);
  for (uint32_t i = 0; i < n_bits; ++i)
    this->set(i, s[n_bits - i - 1] == '1');
}

bool dyn_bitstring::operator==(const char* other_str) const
{
  return strlen(other_str) == length() and (*this) == dyn_bitstring(other_str);
}

void dyn_bitstring::resize(uint32_t new_size)
{
  n_bits = new_size;
  octets_.resize(nof_octets());
  memset(&octets_[0], 0, nof_octets());
}

SRSASN_CODE dyn_bitstring::pack(bit_ref& bref, uint32_t lb, uint32_t ub) const
{
  uint32_t len = length();
  if (len < lb) {
    srsasn_log_print(LOG_LEVEL_ERROR, "dynamic bitstring length=%d is lower than set lower bound=%d\n", len, lb);
    return SRSASN_ERROR_ENCODE_FAIL;
  }
  if (ub > 0) {
    if (ub < len) {
      srsasn_log_print(
          LOG_LEVEL_ERROR, "asn1 error: dynamic bitstring length=%d is higher than set upper bound=%d\n", len, ub);
      return SRSASN_ERROR_ENCODE_FAIL;
    }
    uint32_t len_bits = ceilf(log2(ub - lb));
    HANDLE_CODE(bref.pack(len - lb, len_bits));
    HANDLE_CODE(bref.align_bytes_zero());
  } else {
    HANDLE_CODE(pack_length(bref, len));
  }
  HANDLE_CODE(pack_common_bitstring(bref, &octets_[0], len));
  //  bref.align_bytes_zero();
  return SRSASN_SUCCESS;
}

SRSASN_CODE dyn_bitstring::pack(bit_ref& bref, bool ext, uint32_t lb, uint32_t ub) const
{
  HANDLE_CODE(pack_unsupported_ext_flag(bref, ext));
  HANDLE_CODE(pack(bref, lb, ub));
  return SRSASN_SUCCESS;
}

SRSASN_CODE dyn_bitstring::unpack(bit_ref& bref, uint32_t lb, uint32_t ub)
{
  uint32_t len;
  if (ub > 0) {
    uint32_t len_bits = ceilf(log2f(ub - lb));
    HANDLE_CODE(bref.unpack(len, len_bits));
    len += lb;
    resize(len);
    HANDLE_CODE(bref.align_bytes_zero());
  } else {
    HANDLE_CODE(unpack_length(len, bref));
    resize(len);
  }
  HANDLE_CODE(unpack_common_bitstring(&octets_[0], bref, len));
  //  bref.align_bytes_zero();
  return SRSASN_SUCCESS;
}

SRSASN_CODE dyn_bitstring::unpack(bit_ref& bref, bool& ext, uint32_t lb, uint32_t ub)
{
  HANDLE_CODE(unpack_unsupported_ext_flag(ext, bref));
  HANDLE_CODE(unpack(bref, lb, ub));
  return SRSASN_SUCCESS;
}

/*********************
     choice utils
*********************/
void log_invalid_access_choice_id(uint32_t val, uint32_t choice_id)
{
  srsasn_log_print(LOG_LEVEL_ERROR, "The access choide id is invalid (%d!=%d)\n", val, choice_id);
}
void log_invalid_choice_id(uint32_t val, const char* choice_type)
{
  srsasn_log_print(LOG_LEVEL_ERROR, "Invalid choice id=%d for choice type %s\n", val, choice_type);
}

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
  HANDLE_CODE(pack_norm_small_integer(bref, nof_groups - 1));

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

SRSASN_CODE ext_groups_unpacker_guard::unpack(bit_ref& bref)
{
  bref_tracker = &bref;
  // unpack nof of ext groups
  HANDLE_CODE(unpack_norm_small_integer(nof_unpacked_groups, bref));
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

varlength_field_pack_guard::varlength_field_pack_guard(bit_ref& bref)
{
  brefstart    = bref;
  bref         = bit_ref(&buffer[0], sizeof(buffer));
  bref_tracker = &bref;
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
    srsasn_log_print(LOG_LEVEL_ERROR,
                     "The packed variable sized field is too long for the reserved buffer (%d > %zd)\n",
                     nof_bytes,
                     sizeof(buffer));
  }

  // go back in time to pack length
  pack_length(brefstart, nof_bytes);

  // pack encoded bytes
  for (uint32_t i = 0; i < nof_bytes; ++i) {
    brefstart.pack(buffer[i], 8);
  }
  *bref_tracker = brefstart;
}

varlength_field_unpack_guard::varlength_field_unpack_guard(bit_ref& bref)
{
  unpack_length(len, bref);
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
  if (fieldname.size() > 0) {
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
