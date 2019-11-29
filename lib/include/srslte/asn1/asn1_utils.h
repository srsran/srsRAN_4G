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

#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>
#include <sstream>
#include <stdarg.h> /* va_list, va_start, va_arg, va_end */
#include <stdint.h>
#include <string>

namespace asn1 {

#define ASN_16K 16383

/************************
        logging
************************/

typedef enum { LOG_LEVEL_INFO, LOG_LEVEL_DEBUG, LOG_LEVEL_WARN, LOG_LEVEL_ERROR } srsasn_logger_level_t;

typedef void (*log_handler_t)(srsasn_logger_level_t log_level, void* ctx, const char* str);

void vlog_print(log_handler_t handler, void* ctx, srsasn_logger_level_t log_level, const char* format, va_list args);

void srsasn_log_register_handler(void* ctx, log_handler_t handler);

void srsasn_log_print(srsasn_logger_level_t log_level, const char* format, ...);

/************************
     error handling
************************/

enum SRSASN_CODE { SRSASN_SUCCESS, SRSASN_ERROR_ENCODE_FAIL, SRSASN_ERROR_DECODE_FAIL };

void log_error_code(SRSASN_CODE code, const char* filename, int line);

#define HANDLE_CODE(ret)                                                                                               \
  {                                                                                                                    \
    SRSASN_CODE macrocode = (ret);                                                                                     \
    if (macrocode != SRSASN_SUCCESS) {                                                                                 \
      log_error_code(macrocode, __FILE__, __LINE__);                                                                   \
      return macrocode;                                                                                                \
    }                                                                                                                  \
  }

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
  dyn_array() = default;
  dyn_array(uint32_t new_size) : size_(new_size), cap_(new_size) { data_ = new T[size_]; }
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
  T&       back() { return data_[size() - 1]; }
  const T& back() const { return data_[size() - 1]; }
  T*       data() { return &data_[0]; }
  const T* data() const { return &data_[0]; }

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
      srsasn_log_print(LOG_LEVEL_ERROR, "Maximum size %d achieved for bounded_array.\n", MAX_N);
    }
    data_[current_size++] = elem;
  }
  T&       back() { return data_[current_size - 1]; }
  const T& back() const { return data_[current_size - 1]; }
  T*       data() { return &data_[0]; }
  const T* data() const { return &data_[0]; }

private:
  T        data_[MAX_N];
  uint32_t current_size;
};

/*********************
     ext packing
*********************/

SRSASN_CODE pack_unsupported_ext_flag(bit_ref& bref, bool ext);
SRSASN_CODE unpack_unsupported_ext_flag(bool& ext, bit_ref& bref);

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
    integer packing
************************/

// Constrained Whole Number
template <class IntType>
SRSASN_CODE pack_unalign_integer(bit_ref& bref, IntType n, IntType lb, IntType ub);
template <class IntType>
SRSASN_CODE unpack_unalign_integer(IntType& n, bit_ref& bref, IntType lb, IntType ub);
template <class IntType>
struct UnalignedIntegerPacker {
  UnalignedIntegerPacker(IntType lb_, IntType ub_) : lb(lb_), ub(ub_) {}
  const IntType lb;
  const IntType ub;
  SRSASN_CODE   pack(bit_ref& bref, IntType n) const;
  SRSASN_CODE   unpack(IntType& n, bit_ref& bref) const;
};

template <class IntType, IntType lb, IntType ub>
struct unaligned_integer {
  IntType value;
  unaligned_integer() = default;
  unaligned_integer(IntType value_) : value(value_) {}
              operator IntType() { return value; }
  SRSASN_CODE pack(bit_ref& bref) const { return pack_unalign_integer(bref, value, lb, ub); }
  SRSASN_CODE unpack(bit_ref& bref) { return unpack_unalign_integer(value, bref, lb, ub); }
};

template <class IntType>
SRSASN_CODE pack_align_integer(bit_ref& bref, IntType n, IntType lb, IntType ub);
template <typename IntType>
SRSASN_CODE unpack_align_integer(IntType& intval, bit_ref& bref, IntType lb, IntType ub);
template <class IntType>
struct AlignedIntegerPacker {
  AlignedIntegerPacker(IntType lb_, IntType ub_);
  IntType     lb;
  IntType     ub;
  SRSASN_CODE pack(bit_ref& bref, IntType n);
  SRSASN_CODE unpack(IntType& n, bit_ref& bref);
};

// Normally Small non-negative whole number
template <typename UintType>
SRSASN_CODE pack_norm_small_integer(bit_ref& bref, UintType n);
template <typename UintType>
SRSASN_CODE unpack_norm_small_integer(UintType& n, bit_ref& bref);

// Unconstrained Whole Number
// FIXME: Implement
inline SRSASN_CODE pack_unconstrained_integer(bit_ref& bref, int64_t n)
{
  // TODO
  srsasn_log_print(LOG_LEVEL_ERROR, "Not implemented\n");
  return SRSASN_SUCCESS;
}
inline SRSASN_CODE unpack_unconstrained_integer(int64_t& n, bit_ref& bref)
{
  // TODO
  srsasn_log_print(LOG_LEVEL_ERROR, "Not implemented\n");
  return SRSASN_SUCCESS;
}

/************************
   length determinant
************************/

// Pack as whole constrained number
template <typename IntType>
SRSASN_CODE pack_length(bit_ref& bref, IntType n, IntType lb, IntType ub);
template <typename IntType>
SRSASN_CODE unpack_length(IntType& n, bit_ref& bref, IntType lb, IntType ub);

// Pack as a small non-negative whole number
SRSASN_CODE pack_length(bit_ref& ref, uint32_t val);
SRSASN_CODE unpack_length(uint32_t& val, bit_ref& ref);

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

template <uint32_t N>
class fixed_octstring
{
public:
  const uint8_t& operator[](uint32_t idx) const { return octets_[idx]; }
  uint8_t&       operator[](uint32_t idx) { return octets_[idx]; }
  bool           operator==(const fixed_octstring<N>& other) const { return octets_ == other.octets_; }
  uint8_t*       data() { return &octets_[0]; }
  const uint8_t* data() const { return &octets_[0]; }

  static uint32_t     size() { return N; }
  std::string         to_string() const { return octstring_to_string(&octets_[0], N); }
  fixed_octstring<N>& from_string(const std::string& hexstr)
  {
    if (hexstr.size() != 2 * N) {
      srsasn_log_print(LOG_LEVEL_ERROR, "The provided hex string size is not valid (%d!=2*%d).\n", hexstr.size(), N);
    } else {
      string_to_octstring(&octets_[0], hexstr);
    }
    return *this;
  }
  uint64_t            to_number() const { return octstring_to_number(&octets_[0], size()); }
  fixed_octstring<N>& from_number(uint64_t val)
  {
    number_to_octstring(&octets_[0], val, size());
    return *this;
  }

  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(bit_ref& bref);

private:
  std::array<uint8_t, N> octets_;
};

template <uint32_t N>
SRSASN_CODE fixed_octstring<N>::pack(bit_ref& bref) const
{
  //  if(N > 2) { // X.691 Sec.16
  //    bref.align_bytes_zero();
  //  }
  for (uint32_t i = 0; i < size(); ++i) {
    bref.pack(octets_[i], 8);
  }
  return SRSASN_SUCCESS;
}

template <uint32_t N>
SRSASN_CODE fixed_octstring<N>::unpack(bit_ref& bref)
{
  //  if(N > 2) { // X.691 Sec.16
  //    bref.align_bytes_zero();
  //  }
  for (uint32_t i = 0; i < size(); ++i) {
    HANDLE_CODE(bref.unpack(octets_[i], 8));
  }
  return SRSASN_SUCCESS;
}

/************************
     dyn_octstring
************************/

class dyn_octstring
{
public:
  dyn_octstring() = default;
  dyn_octstring(uint32_t new_size) : octets_(new_size) {}

  const uint8_t& operator[](uint32_t idx) const { return octets_[idx]; }
  uint8_t&       operator[](uint32_t idx) { return octets_[idx]; }
  bool           operator==(const dyn_octstring& other) const { return octets_ == other.octets_; }
  void           resize(uint32_t new_size) { octets_.resize(new_size); }
  uint32_t       size() const { return octets_.size(); }
  uint8_t*       data() { return &octets_[0]; }
  const uint8_t* data() const { return &octets_[0]; }

  SRSASN_CODE    pack(bit_ref& ie_ref) const;
  SRSASN_CODE    unpack(bit_ref& ie_ref);
  std::string    to_string() const;
  dyn_octstring& from_string(const std::string& hexstr);
  uint64_t       to_number() const { return octstring_to_number(&octets_[0], size()); }
  dyn_octstring& from_number(uint64_t val)
  {
    number_to_octstring(&octets_[0], val, size());
    return *this;
  }

private:
  dyn_array<uint8_t> octets_;
};

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
  return (ptr[byte_idx] & (1 << offset)) > 0;
}
inline void bitstring_set(uint8_t* ptr, uint32_t idx, bool value)
{
  uint32_t byte_idx = idx / 8;
  uint32_t offset   = idx % 8;
  if (value) {
    ptr[byte_idx] |= (1 << offset);
  } else {
    ptr[byte_idx] &= ((uint16_t)(1 << 8) - 1) - (1 << offset);
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

template <uint32_t N>
class fixed_bitstring
{
public:
  fixed_bitstring() { memset(&octets_[0], 0, nof_octets()); }
  fixed_bitstring(const std::string& s)
  {
    if (s.size() != N) {
      srsasn_log_print(
          LOG_LEVEL_ERROR, "The provided string size=%zd does not match the bit string size=%d\n", s.size(), N);
    }
    memset(&octets_[0], 0, nof_octets());
    for (uint32_t i = 0; i < N; ++i)
      this->set(N - i - 1, s[i] == '1');
  }
  bool get(uint32_t idx) const { return bitstring_get(&octets_[0], idx); }
  void set(uint32_t idx, bool value) { bitstring_set(&octets_[0], idx, value); }
  bool operator==(const fixed_bitstring<N>& other) const { return octets_ == other.octets_; }
  bool operator==(const char* other_str) const
  {
    return strlen(other_str) == N and (*this) == fixed_bitstring<N>(other_str);
  }
  uint32_t            nof_octets() const { return (uint32_t)ceilf(N / 8.0f); }
  uint32_t            length() const { return N; }
  std::string         to_string() const { return bitstring_to_string(&octets_[0], length()); }
  uint64_t            to_number() const { return bitstring_to_number(&octets_[0], length()); }
  fixed_bitstring<N>& from_number(uint64_t val)
  {
    number_to_bitstring(&octets_[0], val, length());
    return *this;
  }
  uint8_t*       data() { return &octets_[0]; }
  const uint8_t* data() const { return &octets_[0]; }

  SRSASN_CODE pack(bit_ref& bref) const { return pack_fixed_bitstring(bref, data(), N); }
  SRSASN_CODE pack(bit_ref& bref, bool ext) const { return pack_fixed_bitstring(bref, data(), N, ext); }
  SRSASN_CODE unpack(bit_ref& bref) { return unpack_fixed_bitstring(data(), bref, N); }
  SRSASN_CODE unpack(bit_ref& bref, bool& ext) { return unpack_fixed_bitstring(data(), ext, bref, N); }

private:
  std::array<uint8_t, (uint32_t)((N + 7) / 8)> octets_; // ceil(N/8.0)
};

/*********************
    dyn_bitstring
*********************/

class dyn_bitstring
{
public:
  dyn_bitstring() : n_bits(0) {}
  dyn_bitstring(uint32_t n_bits_);
  dyn_bitstring(const char* s);

  bool operator==(const dyn_bitstring& other) const { return octets_ == other.octets_; }
  bool operator==(const char* other_str) const;
  bool get(uint32_t idx) const { return bitstring_get(&octets_[0], idx); }
  void set(uint32_t idx, bool value) { bitstring_set(&octets_[0], idx, value); }

  void           resize(uint32_t new_size);
  uint32_t       length() const { return n_bits; }
  uint32_t       nof_octets() const { return (uint32_t)ceilf(length() / 8.0f); }
  std::string    to_string() const { return bitstring_to_string(&octets_[0], length()); }
  uint64_t       to_number() const { return bitstring_to_number(&octets_[0], length()); }
  dyn_bitstring& from_number(uint64_t val)
  {
    number_to_bitstring(&octets_[0], val, length());
    return *this;
  }
  const uint8_t* data() const { return &octets_[0]; }
  uint8_t*       data() { return &octets_[0]; }

  SRSASN_CODE pack(bit_ref& bref, uint32_t lb = 0, uint32_t ub = 0) const;
  SRSASN_CODE pack(bit_ref& bref, bool ext, uint32_t lb = 0, uint32_t ub = 0) const;
  SRSASN_CODE unpack(bit_ref& bref, uint32_t lb = 0, uint32_t ub = 0);
  SRSASN_CODE unpack(bit_ref& bref, bool& ext, uint32_t lb = 0, uint32_t ub = 0);

private:
  dyn_array<uint8_t> octets_;
  uint32_t           n_bits;
};

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
SRSASN_CODE pack_dyn_seq_of(bit_ref& bref, const ArrayType& seqof, uint32_t lb, uint32_t ub, ItemPacker packer)
{
  HANDLE_CODE(pack_length(bref, seqof.size(), lb, ub));
  for (uint32_t i = 0; i < seqof.size(); ++i) {
    HANDLE_CODE(packer.pack(bref, seqof[i]));
  }
  return SRSASN_SUCCESS;
}

template <class ArrayType>
SRSASN_CODE pack_dyn_seq_of(bit_ref& bref, const ArrayType& seqof, uint32_t lb, uint32_t ub)
{
  HANDLE_CODE(pack_length(bref, seqof.size(), lb, ub));
  for (uint32_t i = 0; i < seqof.size(); ++i) {
    HANDLE_CODE(seqof[i].pack(bref));
  }
  return SRSASN_SUCCESS;
}

template <class ArrayType, class ItemUnpacker>
SRSASN_CODE unpack_dyn_seq_of(ArrayType& seqof, bit_ref& bref, uint32_t lb, uint32_t ub, ItemUnpacker unpacker)
{
  uint32_t nof_items;
  HANDLE_CODE(unpack_length(nof_items, bref, lb, ub));
  seqof.resize(nof_items);
  for (uint32_t i = 0; i < nof_items; ++i) {
    HANDLE_CODE(unpacker.unpack(seqof[i], bref));
  }
  return SRSASN_SUCCESS;
}

template <class ArrayType>
SRSASN_CODE unpack_dyn_seq_of(ArrayType& seqof, bit_ref& bref, uint32_t lb, uint32_t ub)
{
  uint32_t nof_items;
  HANDLE_CODE(unpack_length(nof_items, bref, lb, ub));
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

template <class ItemType, uint32_t lb, uint32_t ub>
struct dyn_seq_of : public dyn_array<ItemType> {
  SeqOfPacker<Packer> packer;
  dyn_seq_of() : packer(lb, ub, Packer()) {}
  dyn_seq_of(const dyn_array<ItemType>& other) : dyn_array<ItemType>(other), packer(lb, ub, Packer()) {}
  SRSASN_CODE pack(bit_ref& bref) const { return packer.pack(bref, *this); }
  SRSASN_CODE unpack(bit_ref& bref) { return packer.unpack(*this, bref); }
};

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
#define MAX2(a, b) ((a) > (b)) ? (a) : (b)
#define MAX4(a, b, c, d) MAX2((MAX2(a, b)), MAX2(c, d))
#define MAX8(a, b, c, d, e, f, g, h) MAX2((MAX4(a, b, c, d)), (MAX4(e, f, g, h)))
#define MAX12(a, b, c, d, e, f, g, h, i, j, k, l) MAX2((MAX8(a, b, c, d, e, f, g, h)), (MAX4(i, j, k, l)))
#define MAX16(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p)                                                          \
  MAX2((MAX8(a, b, c, d, e, f, g, h)), (MAX8(i, j, k, l, m, n, o, p)))
#define MAX32(                                                                                                         \
    a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, a1, b1, c1, d1, e1, f1, g1, h1, i1, j1, k1, l1, m1, n1, o1, p1)    \
  MAX2((MAX16(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p)),                                                        \
       (MAX16(a1, b1, c1, d1, e1, f1, g1, h1, i1, j1, k1, l1, m1, n1, o1, p1)))

template <size_t SIZE>
class choice_buffer_t
{
public:
  struct __attribute__((__may_alias__)) bytes {
    uint8_t        buf_[MAX2(SIZE, 8)];
    uint8_t*       data() { return &buf_[0]; }
    const uint8_t* data() const { return &buf_[0]; }
  };

  template <typename T>
  T& get()
  {
    return *((T*)buffer.buf_.data());
  }
  template <typename T>
  const T& get() const
  {
    return *((T*)buffer.buf_.data());
  }
  template <typename T>
  void destroy()
  {
    ((T*)buffer.buf_.data())->~T();
  }
  template <typename T>
  void init()
  {
    new (buffer.buf_.data()) T();
  }
  template <typename T>
  void init(const T& other)
  {
    new (buffer.buf_.data()) T(other);
  }
  template <typename T>
  void set(const T& other)
  {
    get<T>() = other;
  }

private:
  union {
    alignment_t a_;
    bytes       buf_;
  } buffer;
};

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
      ext group
*********************/

class ext_groups_packer_guard
{
public:
  bool&       operator[](uint32_t idx);
  SRSASN_CODE pack(bit_ref& bref) const;

private:
  bounded_array<bool, 20> groups;
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
  bounded_array<bool, 20> groups;
  const uint32_t          nof_supported_groups;
  uint32_t                nof_unpacked_groups = 0;
  bit_ref*                bref_tracker        = nullptr;
};

/*********************
   Var Length Field
*********************/

class varlength_field_pack_guard
{
public:
  varlength_field_pack_guard(bit_ref& bref);
  ~varlength_field_pack_guard();

private:
  bit_ref brefstart;
  //  bit_ref  bref0;
  bit_ref* bref_tracker;
  uint8_t  buffer[1024];
};

class varlength_field_unpack_guard
{
public:
  varlength_field_unpack_guard(bit_ref& bref);
  ~varlength_field_unpack_guard();

private:
  bit_ref  bref0;
  bit_ref* bref_tracker;
  uint32_t len;
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

} // namespace asn1

#endif // SRSASN_COMMON_UTILS_H
