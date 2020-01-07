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

#ifndef SRSASN_COMMON_UTILS_H
#define SRSASN_COMMON_UTILS_H

#include "srslte/common/log.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>
#include <limits>
#include <map>
#include <sstream>
#include <stdarg.h> /* va_list, va_start, va_arg, va_end */
#include <stdint.h>
#include <string>
#include <vector>

namespace asn1 {

#define ASN_16K 16384
#define ASN_64K 65536

template <class Integer>
constexpr Integer ceil_frac(Integer n, Integer d)
{
  return (n + (d - 1)) / d;
}

/************************
        logging
************************/

using srsasn_logger_level_t = srslte::LOG_LEVEL_ENUM;
using srslte::LOG_LEVEL_DEBUG;
using srslte::LOG_LEVEL_ERROR;
using srslte::LOG_LEVEL_INFO;
using srslte::LOG_LEVEL_WARNING;

void vlog_print(srslte::log* log_ptr, srsasn_logger_level_t log_level, const char* format, va_list args);
void srsasn_log_register_handler(srslte::log* ctx);
void srsasn_log_print(srsasn_logger_level_t log_level, const char* format, ...);

/************************
     error handling
************************/

enum SRSASN_CODE { SRSASN_SUCCESS, SRSASN_ERROR_ENCODE_FAIL, SRSASN_ERROR_DECODE_FAIL };

void log_error_code(SRSASN_CODE code, const char* filename, int line);

#define HANDLE_CODE(ret)                                                                                               \
  do {                                                                                                                 \
    SRSASN_CODE macrocode = ((ret));                                                                                   \
    if (macrocode != SRSASN_SUCCESS) {                                                                                 \
      log_error_code(macrocode, __FILE__, __LINE__);                                                                   \
      return macrocode;                                                                                                \
    }                                                                                                                  \
  } while (0)

/************************
        bit_ref
************************/

struct ValOrError {
  uint32_t    val;
  SRSASN_CODE code;
  ValOrError() : val(0), code(SRSASN_SUCCESS) {}
  ValOrError(uint32_t val_, SRSASN_CODE code_) : val(val_), code(code_) {}
};
ValOrError unpack_bits(uint8_t*& ptr, uint8_t& offset, uint8_t* max_ptr, uint32_t n_bits);

class bit_ref
{
public:
  bit_ref() = default;
  bit_ref(uint8_t* start_ptr_, uint32_t max_size_) :
    ptr(start_ptr_),
    start_ptr(start_ptr_),
    max_ptr(max_size_ + start_ptr_)
  {
  }

  int distance(const bit_ref& other) const;
  int distance(uint8_t* ref_ptr) const;
  int distance() const;
  int distance_bytes(uint8_t* ref_ptr) const;
  int distance_bytes() const;

  SRSASN_CODE pack(uint32_t val, uint32_t n_bits);
  SRSASN_CODE pack_bytes(const uint8_t* buf, uint32_t n_bytes);
  template <class T>
  SRSASN_CODE unpack(T& val, uint32_t n_bits)
  {
    ValOrError ret = unpack_bits(ptr, offset, max_ptr, n_bits);
    val            = ret.val;
    return ret.code;
  }
  SRSASN_CODE unpack_bytes(uint8_t* buf, uint32_t n_bytes);
  SRSASN_CODE align_bytes();
  SRSASN_CODE align_bytes_zero();
  SRSASN_CODE advance_bits(uint32_t n_bits);
  void        set(uint8_t* start_ptr_, uint32_t max_size_);

private:
  uint8_t* ptr       = nullptr;
  uint8_t  offset    = 0;
  uint8_t* start_ptr = nullptr;
  uint8_t* max_ptr   = nullptr;
};

/*********************
  function helpers
*********************/
template <class T>
class dyn_array
{
public:
  typedef T item_type;
  using iterator       = T*;
  using const_iterator = const T*;

  dyn_array() = default;
  explicit dyn_array(uint32_t new_size) : size_(new_size), cap_(new_size) { data_ = new T[size_]; }
  dyn_array(const dyn_array<T>& other)
  {
    size_ = other.size_;
    cap_  = other.cap_;
    data_ = new T[cap_];
    std::copy(&other[0], &other[size_], data_);
  }
  ~dyn_array()
  {
    if (data_ != NULL) {
      delete[] data_;
    }
  }
  uint32_t      size() const { return size_; }
  uint32_t      capacity() const { return cap_; }
  T&            operator[](uint32_t idx) { return data_[idx]; }
  const T&      operator[](uint32_t idx) const { return data_[idx]; }
  dyn_array<T>& operator=(const dyn_array<T>& other)
  {
    if (this == &other) {
      return *this;
    }
    resize(other.size());
    std::copy(&other[0], &other[size_], data_);
    return *this;
  }
  void resize(uint32_t new_size, uint32_t new_cap = 0)
  {
    if (new_size == size_) {
      return;
    }
    if (cap_ >= new_size) {
      size_ = new_size;
      return;
    }
    T* old_data = data_;
    cap_        = new_size > new_cap ? new_size : new_cap;
    if (cap_ > 0) {
      data_ = new T[cap_];
      if (old_data != NULL) {
        std::copy(&old_data[0], &old_data[size_], data_);
      }
    } else {
      data_ = NULL;
    }
    size_ = new_size;
    if (old_data != NULL) {
      delete[] old_data;
    }
  }
  bool operator==(const dyn_array<T>& other) const
  {
    return size() == other.size() and std::equal(data_, data_ + size(), other.data_);
  }
  void push_back(const T& elem)
  {
    resize(size() + 1, size() * 2);
    data_[size() - 1] = elem;
  }
  T&             back() { return data_[size() - 1]; }
  const T&       back() const { return data_[size() - 1]; }
  T*             data() { return &data_[0]; }
  const T*       data() const { return &data_[0]; }
  iterator       begin() { return &data_[0]; }
  iterator       end() { return &data_[size()]; }
  const_iterator begin() const { return &data_[0]; }
  const_iterator end() const { return &data_[size()]; }

private:
  T*       data_ = nullptr;
  uint32_t size_ = 0;
  uint32_t cap_  = 0;
};

template <class T, uint32_t MAX_N>
class bounded_array
{
public:
  typedef T item_type;
  using iterator       = T*;
  using const_iterator = const T*;

  bounded_array(uint32_t size_ = 0) : current_size(size_) {}
  static uint32_t capacity() { return MAX_N; }
  uint32_t        size() const { return current_size; }
  T&              operator[](uint32_t idx) { return data_[idx]; }
  const T&        operator[](uint32_t idx) const { return data_[idx]; }
  bool            operator==(const bounded_array<T, MAX_N>& other) const
  {
    return size() == other.size() and std::equal(data_, data_ + size(), other.data_);
  }
  void resize(uint32_t new_size) { current_size = new_size; }
  void push_back(const T& elem)
  {
    if (current_size >= MAX_N) {
      srsasn_log_print(srslte::LOG_LEVEL_ERROR, "Maximum size %d achieved for bounded_array.\n", MAX_N);
    }
    data_[current_size++] = elem;
  }
  T&             back() { return data_[current_size - 1]; }
  const T&       back() const { return data_[current_size - 1]; }
  T*             data() { return &data_[0]; }
  const T*       data() const { return &data_[0]; }
  iterator       begin() { return &data_[0]; }
  iterator       end() { return &data_[size()]; }
  const_iterator begin() const { return &data_[0]; }
  const_iterator end() const { return &data_[size()]; }

private:
  T        data_[MAX_N];
  uint32_t current_size;
};

/**
 * This array does small buffer optimization. The array has a small stack (Nthres elements) to store elements. Once
 * the number of elements exceeds this stack, the array allocs on the heap.
 * @tparam T
 * @tparam Nthres number of elements T that can be stored in the stack
 */
template <class T, uint32_t Nthres = ceil_frac((size_t)16, sizeof(T))>
class ext_array
{
public:
  static const uint32_t small_buffer_size = Nthres;
  ext_array() : size_(0), head(&small_buffer.data[0]) {}
  explicit ext_array(uint32_t new_size) : ext_array() { resize(new_size); }
  ext_array(const ext_array<T, Nthres>& other) : ext_array(other.size_)
  {
    std::copy(other.head, other.head + other.size_, head);
  }
  ext_array(ext_array<T, Nthres>&& other) noexcept
  {
    size_ = other.size();
    if (other.is_in_small_buffer()) {
      head = &small_buffer.data[0];
      std::copy(other.data(), other.data() + other.size(), head);
    } else {
      head              = other.head;
      small_buffer.cap_ = other.small_buffer.cap_;
      other.head        = &other.small_buffer.data[0];
      other.size_       = 0;
    }
  }
  ~ext_array()
  {
    if (not is_in_small_buffer()) {
      delete[] head;
    }
  }
  ext_array<T, Nthres>& operator=(const ext_array<T, Nthres>& other)
  {
    if (this != &other) {
      resize(other.size());
      std::copy(other.data(), other.data() + other.size(), head);
    }
    return *this;
  }

  uint32_t size() const { return size_; }
  uint32_t capacity() const { return is_in_small_buffer() ? Nthres : small_buffer.cap_; }
  T&       operator[](uint32_t index) { return head[index]; }
  const T& operator[](uint32_t index) const { return head[index]; }
  T*       data() { return &head[0]; }
  const T* data() const { return &head[0]; }
  T&       back() { return head[size() - 1]; }
  const T& back() const { return head[size() - 1]; }
  bool     operator==(const ext_array<T, Nthres>& other) const
  {
    return other.size() == size() and std::equal(other.data(), other.data() + other.size(), data());
  }
  void push_back(const T& elem)
  {
    resize(size() + 1);
    head[size() - 1] = elem;
  }
  void resize(uint32_t new_size)
  {
    if (new_size == size_) {
      return;
    }
    if (capacity() >= new_size) {
      size_ = new_size;
      return;
    }
    T*       old_data = head;
    uint32_t newcap   = new_size + 5;
    head              = new T[newcap];
    std::copy(&small_buffer.data[0], &small_buffer.data[size_], head);
    size_ = new_size;
    if (old_data != &small_buffer.data[0]) {
      delete[] old_data;
    }
    small_buffer.cap_ = newcap;
  }
  bool is_in_small_buffer() const { return head == &small_buffer.data[0]; }

private:
  union {
    T        data[Nthres];
    uint32_t cap_;
  } small_buffer;
  uint32_t size_;
  T*       head;
};

/*********************
     ext packing
*********************/

SRSASN_CODE pack_unsupported_ext_flag(bit_ref& bref, bool ext);
SRSASN_CODE unpack_unsupported_ext_flag(bool& ext, bit_ref& bref);

/************************
    asn1 null packing
************************/

struct asn1_null_t {
  SRSASN_CODE pack(bit_ref& bref) const { return SRSASN_SUCCESS; }
  SRSASN_CODE unpack(bit_ref& bref) const { return SRSASN_SUCCESS; }
};

/************************
     enum packing
************************/

SRSASN_CODE pack_enum(bit_ref& bref, uint32_t enum_val, uint32_t nbits);
SRSASN_CODE pack_enum(bit_ref& bref, uint32_t enum_val, uint32_t nbits, uint32_t nof_noext);
SRSASN_CODE pack_enum(bit_ref& bref, uint32_t e, uint32_t nof_types, uint32_t nof_exts, bool has_ext);
ValOrError  unpack_enum(uint32_t nof_types, uint32_t nof_exts, bool has_ext, bit_ref& bref);
template <typename EnumType>
SRSASN_CODE pack_enum(bit_ref& bref, EnumType e)
{
  return pack_enum(bref, e, EnumType::nof_types, EnumType::nof_exts, EnumType::has_ext);
}
template <typename EnumType>
SRSASN_CODE unpack_enum(EnumType& e, bit_ref& bref)
{
  ValOrError ret = unpack_enum(EnumType::nof_types, EnumType::nof_exts, EnumType::has_ext, bref);
  e              = (typename EnumType::options)ret.val;
  return ret.code;
}

struct EnumPacker {
  template <class EnumType>
  SRSASN_CODE pack(bit_ref& bref, EnumType e)
  {
    return pack_enum(bref, e);
  }
  template <class EnumType>
  SRSASN_CODE unpack(EnumType& e, bit_ref& bref)
  {
    return unpack_enum(e, bref);
  }
};
template <class EnumType>
bool string_to_enum(EnumType& e, const std::string& s)
{
  for (uint32_t i = 0; i < EnumType::nof_types; ++i) {
    e = (typename EnumType::options)i;
    if (e.to_string() == s) {
      return true;
    }
  }
  return false;
}
template <class EnumType, class NumberType>
bool number_to_enum(EnumType& e, NumberType val)
{
  for (uint32_t i = 0; i < e.nof_types; ++i) {
    e = (typename EnumType::options)i;
    if (e.to_number() == val) {
      return true;
    }
  }
  return false;
}
template <class EnumType>
bool number_string_to_enum(EnumType& e, const std::string& val)
{
  for (uint32_t i = 0; i < e.nof_types; ++i) {
    e = (typename EnumType::options)i;
    if (e.to_number_string() == val) {
      return true;
    }
  }
  return false;
}

template <class EnumType, bool E = false, uint32_t M = 0>
class enumerated : public EnumType
{
public:
  static const uint32_t nof_types = EnumType::nulltype, nof_exts = M;
  static const bool     has_ext = E;

  enumerated() { EnumType::value = EnumType::nulltype; }
  enumerated(typename EnumType::options o) { EnumType::value = o; }
  SRSASN_CODE pack(bit_ref& bref) const { return pack_enum(bref, *this); }
  SRSASN_CODE unpack(bit_ref& bref) { return unpack_enum(*this, bref); }
  EnumType&   operator=(EnumType v)
  {
    EnumType::value = v;
    return *this;
  }
  operator typename EnumType::options() const { return EnumType::value; }
};

/************************
     PER encoding
************************/

/* X.691 - Section 10.5 - Constrained Whole Number */
template <class IntType>
SRSASN_CODE pack_constrained_whole_number(bit_ref& bref, IntType n, IntType lb, IntType ub, bool aligned);
template <class IntType>
SRSASN_CODE unpack_constrained_whole_number(IntType& n, bit_ref& bref, IntType lb, IntType ub, bool aligned);

/* X.691 - Section 10.6 - Normally small non-negative whole Number */
template <typename UintType>
SRSASN_CODE pack_norm_small_non_neg_whole_number(bit_ref& bref, UintType n);
template <typename UintType>
SRSASN_CODE unpack_norm_small_non_neg_whole_number(UintType& n, bit_ref& bref);

/* X.691 - Section 10.8 - Unconstrained Whole Number */
template <typename IntType>
SRSASN_CODE pack_unconstrained_whole_number(bit_ref& bref, IntType n, bool aligned);
template <typename IntType>
SRSASN_CODE unpack_unconstrained_whole_number(IntType& n, bit_ref& bref, bool aligned);

/************************
   length determinant
************************/

// Pack as whole constrained number
template <typename IntType>
SRSASN_CODE pack_length(bit_ref& bref, IntType n, IntType lb, IntType ub, bool aligned = false);
template <typename IntType>
SRSASN_CODE unpack_length(IntType& n, bit_ref& bref, IntType lb, IntType ub, bool aligned = false);

// Pack as a small non-negative whole number
SRSASN_CODE pack_length(bit_ref& ref, uint32_t val, bool aligned = false);
SRSASN_CODE unpack_length(uint32_t& val, bit_ref& ref, bool aligned = false);

/************************
        Integer
************************/

template <typename IntType>
SRSASN_CODE pack_integer(bit_ref& bref,
                         IntType  n,
                         IntType  lb      = std::numeric_limits<IntType>::min(),
                         IntType  ub      = std::numeric_limits<IntType>::max(),
                         bool     has_ext = false,
                         bool     aligned = false);
template <typename IntType>
SRSASN_CODE unpack_integer(IntType& n,
                           bit_ref& bref,
                           IntType  lb      = std::numeric_limits<IntType>::min(),
                           IntType  ub      = std::numeric_limits<IntType>::max(),
                           bool     has_ext = false,
                           bool     aligned = false);
// unconstrained case
template <typename IntType>
SRSASN_CODE pack_unconstrained_integer(bit_ref& bref, IntType n, bool has_ext = false, bool aligned = false);
template <typename IntType>
SRSASN_CODE unpack_unconstrained_integer(IntType& n, bit_ref& bref, bool has_ext = false, bool aligned = false);

template <class IntType>
struct integer_packer {
  integer_packer(IntType lb_, IntType ub_, bool has_ext_ = false, bool aligned_ = false);
  SRSASN_CODE pack(bit_ref& bref, IntType n);
  SRSASN_CODE unpack(IntType& n, bit_ref& bref);
  IntType     lb;
  IntType     ub;
  bool        has_ext;
  bool        aligned;
};

template <class IntType,
          IntType LB  = std::numeric_limits<IntType>::min(),
          IntType UB  = std::numeric_limits<IntType>::max(),
          bool    Ext = false,
          bool    Al  = false>
class integer
{
public:
  static const IntType ub = UB, lb = LB;
  static const bool    has_ext = Ext, is_aligned = Al;
  IntType              value;
  integer() = default;
  integer(IntType value_) : value(value_) {}
              operator IntType() { return value; }
  SRSASN_CODE pack(bit_ref& bref) const { return pack_integer(bref, value, lb, ub, has_ext, is_aligned); }
  SRSASN_CODE unpack(bit_ref& bref) { return unpack_integer(value, bref, lb, ub, has_ext, is_aligned); }
};

/************************
  General Packer/Unpacker
************************/

struct BitPacker {
  BitPacker(uint32_t nof_bits_) : nof_bits(nof_bits_) {}
  template <typename T>
  SRSASN_CODE pack(bit_ref& bref, const T& topack)
  {
    bref.pack(topack, nof_bits);
    return SRSASN_SUCCESS;
  }
  template <typename T>
  SRSASN_CODE unpack(T& tounpack, bit_ref& bref)
  {
    return bref.unpack(tounpack, nof_bits);
  }
  uint32_t nof_bits;
};

struct Packer {
  template <typename T>
  SRSASN_CODE pack(bit_ref& bref, const T& topack)
  {
    return topack.pack(bref);
  }
  template <typename T>
  SRSASN_CODE unpack(T& tounpack, bit_ref& bref)
  {
    return tounpack.unpack(bref);
  }
};

/*********************
 common octstring
*********************/

// helper functions common to all octstring implementations
uint64_t    octstring_to_number(const uint8_t* ptr, uint32_t nbytes);
void        number_to_octstring(uint8_t* ptr, uint64_t number, uint32_t nbytes);
std::string octstring_to_string(const uint8_t* ptr, uint32_t N);
void        string_to_octstring(uint8_t* ptr, const std::string& str);

/************************
    fixed_octstring
************************/

template <uint32_t N, bool aligned = false>
class fixed_octstring
{
public:
  const uint8_t& operator[](uint32_t idx) const { return octets_[idx]; }
  uint8_t&       operator[](uint32_t idx) { return octets_[idx]; }
  bool           operator==(const fixed_octstring<N>& other) const { return octets_ == other.octets_; }
  uint8_t*       data() { return &octets_[0]; }
  const uint8_t* data() const { return &octets_[0]; }

  static uint32_t              size() { return N; }
  std::string                  to_string() const { return octstring_to_string(&octets_[0], N); }
  fixed_octstring<N, aligned>& from_string(const std::string& hexstr)
  {
    if (hexstr.size() != 2 * N) {
      srsasn_log_print(
          srslte::LOG_LEVEL_ERROR, "The provided hex string size is not valid (%d!=2*%d).\n", hexstr.size(), N);
    } else {
      string_to_octstring(&octets_[0], hexstr);
    }
    return *this;
  }
  uint64_t                     to_number() const { return octstring_to_number(&octets_[0], size()); }
  fixed_octstring<N, aligned>& from_number(uint64_t val)
  {
    number_to_octstring(&octets_[0], val, size());
    return *this;
  }

  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);

private:
  std::array<uint8_t, N> octets_;
};

/**
 * X.691 Section 16 - Encoding the octetstring type
 * @tparam N - number of items
 * @tparam ext - aligned variant
 * @param bref
 * @return
 */
template <uint32_t N, bool aligned>
SRSASN_CODE fixed_octstring<N, aligned>::pack(bit_ref& bref) const
{
  if (aligned and N > 2) {
    bref.align_bytes_zero();
  }
  for (uint32_t i = 0; i < size(); ++i) {
    HANDLE_CODE(bref.pack(octets_[i], 8));
  }
  return SRSASN_SUCCESS;
}

template <uint32_t N, bool aligned>
SRSASN_CODE fixed_octstring<N, aligned>::unpack(bit_ref& bref)
{
  if (aligned and N > 2) {
    bref.align_bytes_zero();
  }
  for (uint32_t i = 0; i < size(); ++i) {
    HANDLE_CODE(bref.unpack(octets_[i], 8));
  }
  return SRSASN_SUCCESS;
}

/************************
     dyn_octstring
************************/

template <bool Al = false>
class unbounded_octstring
{
public:
  static const bool aligned = Al;

  unbounded_octstring() = default;
  explicit unbounded_octstring(uint32_t new_size) : octets_(new_size) {}

  const uint8_t& operator[](uint32_t idx) const { return octets_[idx]; }
  uint8_t&       operator[](uint32_t idx) { return octets_[idx]; }
  bool           operator==(const unbounded_octstring<Al>& other) const { return octets_ == other.octets_; }
  void           resize(uint32_t new_size) { octets_.resize(new_size); }
  uint32_t       size() const { return octets_.size(); }
  uint8_t*       data() { return &octets_[0]; }
  const uint8_t* data() const { return &octets_[0]; }

  SRSASN_CODE              pack(bit_ref& ie_ref) const;
  SRSASN_CODE              unpack(bit_ref& ie_ref);
  std::string              to_string() const;
  unbounded_octstring<Al>& from_string(const std::string& hexstr);
  uint64_t                 to_number() const { return octstring_to_number(&octets_[0], size()); }
  unbounded_octstring<Al>& from_number(uint64_t val)
  {
    number_to_octstring(&octets_[0], val, size());
    return *this;
  }

private:
  dyn_array<uint8_t> octets_;
};

using dyn_octstring = unbounded_octstring<false>;

/*********************
   common bitstring
*********************/

// helper functions common to all bitstring implementations
uint64_t    bitstring_to_number(const uint8_t* ptr, uint32_t nbits);
void        number_to_bitstring(uint8_t* ptr, uint64_t number, uint32_t nbits);
std::string bitstring_to_string(const uint8_t* ptr, uint32_t nbits);
inline bool bitstring_get(const uint8_t* ptr, uint32_t idx)
{
  uint32_t byte_idx = idx / 8;
  uint32_t offset   = idx % 8;
  return (ptr[byte_idx] & (1u << offset)) > 0;
}
inline void bitstring_set(uint8_t* ptr, uint32_t idx, bool value)
{
  uint32_t byte_idx = idx / 8;
  uint32_t offset   = idx % 8;
  if (value) {
    ptr[byte_idx] |= (1u << offset);
  } else {
    ptr[byte_idx] &= ((uint16_t)(1u << 8) - 1) - (1 << offset);
  }
}

/*********************
   fixed_bitstring
*********************/

// fixed bitstring pack/unpack helpers
SRSASN_CODE pack_fixed_bitstring(bit_ref& bref, const uint8_t* buf, uint32_t nbits);
SRSASN_CODE pack_fixed_bitstring(bit_ref& bref, const uint8_t* buf, uint32_t nbits, bool ext);
SRSASN_CODE unpack_fixed_bitstring(uint8_t* buf, bit_ref& bref, uint32_t nbits);
SRSASN_CODE unpack_fixed_bitstring(uint8_t* buf, bool& ext, bit_ref& bref, uint32_t nbits);

template <uint32_t N, bool Ext = false, bool Al = false>
class fixed_bitstring
{
public:
  using type_t              = fixed_bitstring<N, Ext, Al>;
  static const bool has_ext = Ext, is_aligned = Al;

  fixed_bitstring() { memset(&octets_[0], 0, nof_octets()); }
  fixed_bitstring(const std::string& s)
  {
    if (s.size() != N) {
      srsasn_log_print(
          srslte::LOG_LEVEL_ERROR, "The provided string size=%zd does not match the bit string size=%d\n", s.size(), N);
    }
    memset(&octets_[0], 0, nof_octets());
    for (uint32_t i = 0; i < N; ++i)
      this->set(N - i - 1, s[i] == '1');
  }
  bool     get(uint32_t idx) const { return bitstring_get(&octets_[0], idx); }
  void     set(uint32_t idx, bool value) { bitstring_set(&octets_[0], idx, value); }
  bool     operator==(const type_t& other) const { return octets_ == other.octets_; }
  bool     operator==(const char* other_str) const { return strlen(other_str) == N and (*this) == type_t(other_str); }
  uint32_t nof_octets() const { return (uint32_t)ceilf(N / 8.0f); }
  uint32_t length() const { return N; }
  std::string to_string() const { return bitstring_to_string(&octets_[0], length()); }
  uint64_t    to_number() const { return bitstring_to_number(&octets_[0], length()); }
  type_t&     from_number(uint64_t val)
  {
    number_to_bitstring(&octets_[0], val, length());
    return *this;
  }
  uint8_t*       data() { return &octets_[0]; }
  const uint8_t* data() const { return &octets_[0]; }

  SRSASN_CODE pack(bit_ref& bref) const { return pack_fixed_bitstring(bref, data(), N); }
  SRSASN_CODE unpack(bit_ref& bref) { return unpack_fixed_bitstring(data(), bref, N); }

private:
  std::array<uint8_t, (uint32_t)((N + 7) / 8)> octets_; // ceil(N/8.0)
};

/*********************
  bounded_bitstring
*********************/

namespace bitstring_utils {
SRSASN_CODE
pack(bit_ref& bref, const uint8_t* data, uint32_t size, uint32_t lb, uint32_t ub, bool has_ext, bool is_aligned);
SRSASN_CODE unpack_length_prefix(uint32_t& len, bit_ref& bref, uint32_t lb, uint32_t ub, bool has_ext, bool is_aligned);
SRSASN_CODE unpack_bitfield(uint8_t* buf, bit_ref& bref, uint32_t n, uint32_t lb, uint32_t ub, bool is_aligned);
SRSASN_CODE
unpack_fixed_bitstring(uint8_t* buf, bit_ref& bref, uint32_t nof_bits, bool has_ext = false, bool is_aligned = false);
} // namespace bitstring_utils

template <typename BitStringType>
class base_bitstring
{
protected:
  using derived_t = BitStringType;

public:
  bool     get(uint32_t idx) const { return bitstring_get(derived()->data(), idx); }
  void     set(uint32_t idx, bool value) { bitstring_set(derived()->data(), idx, value); }
  uint32_t nof_octets() const { return ceil_frac(derived()->length(), 8u); }

  std::string to_string() const { return bitstring_to_string(derived()->data(), derived()->length()); }
  derived_t&  from_string(const std::string& s)
  {
    if (s.size() < derived_t::lb or s.size() > derived_t::ub) {
      srsasn_log_print(srslte::LOG_LEVEL_ERROR,
                       "The provided string size=%zd is not withing the bounds [%d, %d]\n",
                       s.size(),
                       derived_t::lb,
                       derived_t::ub);
    } else {
      derived()->resize(s.size());
      for (uint32_t i = 0; i < s.size(); ++i) {
        set(s.size() - i - 1, s[i] == '1');
      }
    }
    return *derived();
  }
  uint64_t   to_number() const { return bitstring_to_number(derived()->data(), derived()->length()); }
  derived_t& from_number(uint64_t val)
  {
    uint32_t nof_bits = (uint32_t)ceilf(log2(val));
    if (nof_bits > derived()->length()) {
      derived()->resize(nof_bits);
    }
    number_to_bitstring(derived()->data(), val, derived()->length());
    return *derived();
  }

  bool operator==(const base_bitstring<BitStringType>& other) const
  {
    return derived()->length() == other.derived()->length() and
           std::equal(derived()->data(), derived()->data() + derived()->nof_octets(), other.derived()->data());
  }
  bool operator==(const char* other_str) const
  {
    return strlen(other_str) == derived()->length() and (*this) == derived_t{}.from_string(other_str);
  }

  SRSASN_CODE pack(bit_ref& bref) const
  {
    return bitstring_utils::pack(bref,
                                 derived()->data(),
                                 derived()->length(),
                                 derived_t::lb,
                                 derived_t::ub,
                                 derived_t::has_ext,
                                 derived_t::is_aligned);
  }
  SRSASN_CODE unpack(bit_ref& bref)
  {
    // X.691, subclause 15.11
    uint32_t nbits;
    HANDLE_CODE(bitstring_utils::unpack_length_prefix(
        nbits, bref, derived_t::lb, derived_t::ub, derived_t::has_ext, derived_t::is_aligned));
    derived()->resize(nbits);
    return bitstring_utils::unpack_bitfield(
        derived()->data(), bref, nbits, derived_t::lb, derived_t::ub, derived_t::is_aligned);
  }

private:
  derived_t*       derived() { return static_cast<derived_t*>(this); }
  const derived_t* derived() const { return static_cast<const derived_t*>(this); }
};

template <uint32_t LB, uint32_t UB, bool ext = false, bool aligned = false>
class bounded_bitstring : public base_bitstring<bounded_bitstring<LB, UB, ext, aligned> >
{
  using base_t = base_bitstring<bounded_bitstring<LB, UB> >;

public:
  static const uint32_t lb = LB, ub = UB;
  static const bool     has_ext = ext, is_aligned = aligned;

  explicit bounded_bitstring(uint32_t siz_ = 0) { resize(siz_); }

  const uint8_t* data() const { return &octets_[0]; }
  uint8_t*       data() { return &octets_[0]; }
  uint32_t       length() const { return nof_bits; }
  void           resize(uint32_t new_size)
  {
    nof_bits = new_size;
    octets_.resize(this->nof_octets());
    memset(data(), 0, this->nof_octets());
  }

private:
  bounded_array<uint8_t, ceil_frac(ub, 8u)> octets_;
  uint32_t                                  nof_bits = 0;
};

/*********************
    dyn_bitstring
*********************/

template <bool Ext = false, bool Al = false>
class unbounded_bitstring : public base_bitstring<unbounded_bitstring<Ext, Al> >
{
  using base_t = base_bitstring<unbounded_bitstring<Ext, Al> >;

public:
  static const uint32_t lb = 0, ub = std::numeric_limits<uint32_t>::max();
  static const bool     has_ext = Ext, is_aligned = Al;

  explicit unbounded_bitstring(uint32_t siz_ = 0) { resize(siz_); }

  const uint8_t* data() const { return &octets_[0]; }
  uint8_t*       data() { return &octets_[0]; }
  uint32_t       length() const { return n_bits; }
  void           resize(uint32_t new_size)
  {
    n_bits = new_size;
    octets_.resize(this->nof_octets());
    memset(data(), 0, this->nof_octets()); // resize always resets content
  }

private:
  dyn_array<uint8_t> octets_;
  uint32_t           n_bits;
};

using dyn_bitstring = unbounded_bitstring<false, false>;

/*********************
  fixed sequence of
*********************/

// packers/unpackers for fixed_length sequence-of
template <class T, class ItemPacker>
SRSASN_CODE pack_fixed_seq_of(bit_ref& bref, const T* item_array, uint32_t nof_items, ItemPacker packer)
{
  for (uint32_t i = 0; i < nof_items; ++i) {
    HANDLE_CODE(packer.pack(bref, item_array[i]));
  }
  return SRSASN_SUCCESS;
}
template <class T>
SRSASN_CODE pack_fixed_seq_of(bit_ref& bref, const T* item_array, uint32_t nof_items)
{
  for (uint32_t i = 0; i < nof_items; ++i) {
    HANDLE_CODE(item_array[i].pack(bref));
  }
  return SRSASN_SUCCESS;
}
template <class T, class ItemUnpacker>
SRSASN_CODE unpack_fixed_seq_of(T* item_array, bit_ref& bref, uint32_t nof_items, ItemUnpacker unpacker)
{
  for (uint32_t i = 0; i < nof_items; ++i) {
    HANDLE_CODE(unpacker.unpack(item_array[i], bref));
  }
  return SRSASN_SUCCESS;
}
template <class T>
SRSASN_CODE unpack_fixed_seq_of(T* item_array, bit_ref& bref, uint32_t nof_items)
{
  for (uint32_t i = 0; i < nof_items; ++i) {
    HANDLE_CODE(item_array[i].unpack(bref));
  }
  return SRSASN_SUCCESS;
}

template <class ItemPacker>
struct FixedSeqOfPacker {
  FixedSeqOfPacker(uint32_t nof_items_, ItemPacker packer_) : nof_items(nof_items_), packer(packer_) {}
  FixedSeqOfPacker(uint32_t nof_items_) : nof_items(nof_items_), packer(Packer()) {}
  template <typename T>
  SRSASN_CODE pack(bit_ref& bref, const T* topack)
  {
    return pack_fixed_seq_of(bref, topack, nof_items, packer);
  }
  template <typename T>
  SRSASN_CODE unpack(T* tounpack, bit_ref& bref)
  {
    return unpack_fixed_seq_of(tounpack, bref, nof_items, packer);
  }
  uint32_t   nof_items;
  ItemPacker packer;
};

/*********************
   dyn sequence of
*********************/

template <class ArrayType, class ItemPacker>
SRSASN_CODE pack_dyn_seq_of(bit_ref&         bref,
                            const ArrayType& seqof,
                            uint32_t         lb,
                            uint32_t         ub,
                            ItemPacker       packer,
                            bool             aligned = false)
{
  HANDLE_CODE(pack_length(bref, seqof.size(), lb, ub, aligned));
  for (uint32_t i = 0; i < seqof.size(); ++i) {
    HANDLE_CODE(packer.pack(bref, seqof[i]));
  }
  return SRSASN_SUCCESS;
}

template <class ArrayType>
SRSASN_CODE pack_dyn_seq_of(bit_ref& bref, const ArrayType& seqof, uint32_t lb, uint32_t ub, bool aligned = false)
{
  HANDLE_CODE(pack_length(bref, seqof.size(), lb, ub, aligned));
  for (uint32_t i = 0; i < seqof.size(); ++i) {
    HANDLE_CODE(seqof[i].pack(bref));
  }
  return SRSASN_SUCCESS;
}

template <class ArrayType, class ItemUnpacker>
SRSASN_CODE unpack_dyn_seq_of(ArrayType&   seqof,
                              bit_ref&     bref,
                              uint32_t     lb,
                              uint32_t     ub,
                              ItemUnpacker unpacker,
                              bool         aligned = false)
{
  uint32_t nof_items;
  HANDLE_CODE(unpack_length(nof_items, bref, lb, ub, aligned));
  seqof.resize(nof_items);
  for (uint32_t i = 0; i < nof_items; ++i) {
    HANDLE_CODE(unpacker.unpack(seqof[i], bref));
  }
  return SRSASN_SUCCESS;
}

template <class ArrayType>
SRSASN_CODE unpack_dyn_seq_of(ArrayType& seqof, bit_ref& bref, uint32_t lb, uint32_t ub, bool aligned = false)
{
  uint32_t nof_items;
  HANDLE_CODE(unpack_length(nof_items, bref, lb, ub, aligned));
  seqof.resize(nof_items);
  for (uint32_t i = 0; i < nof_items; ++i) {
    HANDLE_CODE(seqof[i].unpack(bref));
  }
  return SRSASN_SUCCESS;
}

template <class InnerPacker>
struct SeqOfPacker {
  SeqOfPacker(uint32_t lb_, uint32_t ub_, InnerPacker packer_) : lb(lb_), ub(ub_), packer(packer_) {}
  template <typename T>
  SRSASN_CODE pack(bit_ref& bref, const T& topack) const
  {
    return pack_dyn_seq_of(bref, topack, lb, ub, packer);
  }
  template <typename T>
  SRSASN_CODE unpack(T& tounpack, bit_ref& bref)
  {
    return unpack_dyn_seq_of(tounpack, bref, lb, ub, packer);
  }
  InnerPacker packer;
  uint32_t    lb;
  uint32_t    ub;
};

template <class ItemType, uint32_t lb, uint32_t ub, bool aligned = false>
struct dyn_seq_of : public dyn_array<ItemType> {
  dyn_seq_of() = default;
  dyn_seq_of(const dyn_array<ItemType>& other) : dyn_array<ItemType>(other) {}
  SRSASN_CODE pack(bit_ref& bref) const { return pack_dyn_seq_of(bref, *this, lb, ub, aligned); }
  SRSASN_CODE unpack(bit_ref& bref) { return unpack_dyn_seq_of(*this, bref, lb, ub, aligned); }
};

/*********************
   printable string
*********************/

/* X.691 - Section 27 - Character Restricted String */
namespace asn_string_utils {
SRSASN_CODE
pack(bit_ref& bref, const std::string& s, size_t lb, size_t ub, size_t alb, size_t aub, bool ext, bool aligned);
SRSASN_CODE unpack(std::string& s, bit_ref& bref, size_t lb, size_t ub, size_t alb, size_t aub, bool ext, bool aligned);
} // namespace asn_string_utils

template <uint32_t LB,
          uint32_t UB,
          uint32_t ALB     = 0,
          uint32_t AUB     = std::numeric_limits<uint32_t>::max(),
          bool     ext     = false,
          bool     aligned = false>
class asn_string
{
public:
  SRSASN_CODE pack(bit_ref& bref) const { return asn_string_utils::pack(bref, str, LB, UB, ALB, AUB, ext, aligned); }
  SRSASN_CODE unpack(bit_ref& bref) { return asn_string_utils::unpack(str, bref, LB, UB, ALB, AUB, ext, aligned); }
  char&       operator[](std::size_t idx) { return str[idx]; }
  const char& operator[](std::size_t idx) const { return str[idx]; }
  void        resize(std::size_t newsize) { str.resize(newsize); }
  std::size_t size() const { return str.size(); }
  std::string to_string() const { return str; }
  void        from_string(std::string s) { str = std::move(s); }

private:
  std::string str;
};

template <uint32_t ALB = 0, uint32_t AUB = std::numeric_limits<uint32_t>::max(), bool ext = false, bool aligned = false>
using printable_string = asn_string<32, 122, ALB, AUB, ext, aligned>;

/*********************
      copy_ptr
*********************/

template <class T>
class copy_ptr
{
public:
  explicit copy_ptr(T* ptr_ = nullptr) :
    ptr(ptr_) {} // it takes hold of the pointer (including destruction). You should use make_copy_ptr() in most cases
  // instead of this ctor
  copy_ptr(const copy_ptr<T>& other) { ptr = (other.ptr == nullptr) ? nullptr : new T(*other.ptr); }
  ~copy_ptr() { destroy_(); }
  copy_ptr<T>& operator=(const copy_ptr<T>& other)
  {
    if (this != &other) {
      reset((other.ptr == nullptr) ? nullptr : new T(*other.ptr));
    }
    return *this;
  }
  bool     operator==(const copy_ptr<T>& other) const { return *ptr == *other; }
  T*       operator->() { return ptr; }
  const T* operator->() const { return ptr; }
  T&       operator*() { return *ptr; }       // like pointers, don't call this if ptr==NULL
  const T& operator*() const { return *ptr; } // like pointers, don't call this if ptr==NULL
  T*       get() { return ptr; }
  const T* get() const { return ptr; }
  T*       release()
  {
    T* ret = ptr;
    ptr    = nullptr;
    return ret;
  }
  void reset(T* ptr_ = nullptr)
  {
    destroy_();
    ptr = ptr_;
  }
  void set_present(bool flag = true)
  {
    if (flag) {
      reset(new T());
    } else {
      reset();
    }
  }
  bool is_present() const { return get() != nullptr; }

private:
  void destroy_()
  {
    if (ptr != NULL) {
      delete ptr;
    }
  }
  T* ptr;
};

template <class T>
copy_ptr<T> make_copy_ptr(const T& t)
{
  return copy_ptr<T>(new T(t));
}

/*********************
     choice utils
*********************/

union alignment_t {
  char        c;
  float       f;
  uint32_t    i;
  uint64_t    i2;
  double      d;
  long double d2;
  uint32_t*   ptr;
};

template <std::size_t arg1, std::size_t... others>
struct static_max;

template <std::size_t arg>
struct static_max<arg> {
  static const std::size_t value = arg;
};

template <std::size_t arg1, std::size_t arg2, std::size_t... others>
struct static_max<arg1, arg2, others...> {
  static const std::size_t value =
      arg1 >= arg2 ? static_max<arg1, others...>::value : static_max<arg2, others...>::value;
};

template <std::size_t Size, std::size_t Align>
struct choice_buffer_base_t {
  static const std::size_t data_size  = Size;
  static const std::size_t data_align = Align;
  using buffer_t                      = typename std::aligned_storage<data_size, data_align>::type;
  buffer_t buffer;

  template <typename T>
  T& get()
  {
    return *(reinterpret_cast<T*>(&buffer));
  }
  template <typename T>
  const T& get() const
  {
    return *(reinterpret_cast<const T*>(&buffer));
  }
  template <typename T>
  void destroy()
  {
    get<T>().~T();
  }
  template <typename T>
  void init()
  {
    new (&buffer) T();
  }
  template <typename T>
  void init(const T& other)
  {
    new (&buffer) T(other);
  }
  template <typename T>
  void set(const T& other)
  {
    get<T>() = other;
  }
};

template <typename... Ts>
struct choice_buffer_t : public choice_buffer_base_t<static_max<sizeof(alignment_t), sizeof(Ts)...>::value,
                                                     static_max<alignof(alignment_t), alignof(Ts)...>::value> {
};

using pod_choice_buffer_t = choice_buffer_t<>;

/*********************
      ext group
*********************/

class ext_groups_packer_guard
{
public:
  bool&       operator[](uint32_t idx);
  SRSASN_CODE pack(bit_ref& bref) const;

private:
  ext_array<bool> groups;
};

class ext_groups_unpacker_guard
{
public:
  explicit ext_groups_unpacker_guard(uint32_t nof_supported_groups_);
  ~ext_groups_unpacker_guard();

  void        resize(uint32_t new_size);
  bool&       operator[](uint32_t idx);
  SRSASN_CODE unpack(bit_ref& bref);

private:
  ext_array<bool> groups;
  const uint32_t  nof_supported_groups;
  uint32_t        nof_unpacked_groups = 0;
  bit_ref*        bref_tracker        = nullptr;
};

/*********************
   Var Length Field
*********************/

class varlength_field_pack_guard
{
public:
  explicit varlength_field_pack_guard(bit_ref& bref, bool align_ = false);
  ~varlength_field_pack_guard();

private:
  bit_ref brefstart;
  //  bit_ref  bref0;
  bit_ref* bref_tracker;
  uint8_t  buffer[1024];
  bool     align;
};

class varlength_field_unpack_guard
{
public:
  explicit varlength_field_unpack_guard(bit_ref& bref, bool align = false);
  ~varlength_field_unpack_guard();

private:
  bit_ref  bref0;
  bit_ref* bref_tracker = nullptr;
  uint32_t len          = 0;
};

/*******************
    JsonWriter
*******************/

class json_writer
{
public:
  json_writer();
  void        write_fieldname(const std::string& fieldname);
  void        write_str(const std::string& fieldname, const std::string& value);
  void        write_str(const std::string& value);
  void        write_int(const std::string& fieldname, int64_t value);
  void        write_int(int64_t value);
  void        write_bool(const std::string& fieldname, bool value);
  void        write_bool(bool value);
  void        write_null(const std::string& fieldname);
  void        write_null();
  void        start_obj(const std::string& fieldname = "");
  void        end_obj();
  void        start_array(const std::string& fieldname = "");
  void        end_array();
  std::string to_string() const;

private:
  std::stringstream ss;
  std::string       ident;
  enum separator_t { COMMA, NEWLINE, NONE };
  separator_t sep;
};

/*******************
  Test pack/unpack
*******************/

template <class Msg>
int test_pack_unpack_consistency(const Msg& msg)
{
  uint8_t buf[2048], buf2[2048];
  bzero(buf, sizeof(buf));
  bzero(buf2, sizeof(buf2));
  Msg           msg2;
  asn1::bit_ref bref(&buf[0], sizeof(buf)), bref2(&buf[0], sizeof(buf)), bref3(&buf2[0], sizeof(buf2));

  if (msg.pack(bref) != asn1::SRSASN_SUCCESS) {
    log_error_code(SRSASN_ERROR_ENCODE_FAIL, __FILE__, __LINE__);
    return -1;
  }
  if (msg2.unpack(bref2) != asn1::SRSASN_SUCCESS) {
    log_error_code(SRSASN_ERROR_DECODE_FAIL, __FILE__, __LINE__);
    return -1;
  }
  if (msg2.pack(bref3) != asn1::SRSASN_SUCCESS) {
    log_error_code(SRSASN_ERROR_ENCODE_FAIL, __FILE__, __LINE__);
    return -1;
  }

  // unpack and last pack done for the same number of bits
  if (bref3.distance() != bref2.distance()) {
    srsasn_log_print(LOG_LEVEL_ERROR, "[%s][%d] .\n", __FILE__, __LINE__);
    return -1;
  }

  // ensure packed messages are the same
  if (bref3.distance() != bref.distance()) {
    srsasn_log_print(LOG_LEVEL_ERROR, "[%s][%d] .\n", __FILE__, __LINE__);
    return -1;
  }
  if (memcmp(buf, buf2, bref.distance_bytes()) != 0) {
    srsasn_log_print(LOG_LEVEL_ERROR, "[%s][%d] .\n", __FILE__, __LINE__);
    return -1;
  }
  return SRSASN_SUCCESS;
}

} // namespace asn1

#endif // SRSASN_COMMON_UTILS_H
