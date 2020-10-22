/*
Copyright 2013-2017 Software Radio Systems Limited

This file is part of srsASN1

srsASN1 is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

srsASN1 is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

A copy of the GNU Affero General Public License can be found in
the LICENSE file in the top-level directory of this distribution
and at http://www.gnu.org/licenses/.
*/

#include "srslte/asn1/asn1_utils.h"
#include "srslte/common/test_common.h"
#include <cmath>
#include <numeric>
#include <random>

using namespace std;
using namespace asn1;

std::random_device rd;
std::mt19937       g(rd());

int test_arrays()
{
  /* Test Ext Array */
  ext_array<int> ext_ar;
  const int*     old_ptr = ext_ar.data();
  // Test Resize until goes out of small buffer
  for (uint32_t i = 0; i < ext_ar.small_buffer_size + 1; ++i) {
    TESTASSERT(ext_ar.is_in_small_buffer());
    TESTASSERT(ext_ar.capacity() == ext_ar.small_buffer_size);
    TESTASSERT(ext_ar.size() == i);
    ext_ar.push_back(i);
  }
  TESTASSERT(not ext_ar.is_in_small_buffer());
  TESTASSERT(ext_ar.size() == ext_ar.small_buffer_size + 1);
  TESTASSERT(old_ptr != ext_ar.data());
  TESTASSERT(ext_ar.capacity() > ext_ar.small_buffer_size);
  // Test Copy
  {
    ext_array<int> ext_ar2(ext_ar);
    TESTASSERT(ext_ar2 == ext_ar);
    TESTASSERT(not ext_ar.is_in_small_buffer());
  }
  TESTASSERT(ext_ar.size() == 5);
  // Test Move
  {
    ext_array<int> ext_ar2(std::move(ext_ar));
    TESTASSERT(ext_ar.is_in_small_buffer());
    TESTASSERT(ext_ar2.size() == 5);
    TESTASSERT(ext_ar2[2] == 2);
  }
  ext_ar = ext_array<int>(5);
  TESTASSERT(not ext_ar.is_in_small_buffer());
  TESTASSERT(ext_ar.size() == 5);
  ext_ar.resize(0);
  TESTASSERT(not ext_ar.is_in_small_buffer());

  return 0;
}

int test_bit_ref()
{
  uint8_t buf[1024];

  // one bit at a time
  {
    uint32_t nof_bytes = 2;
    bit_ref  bref(&buf[0], nof_bytes);
    bit_ref  bref0 = bref;
    for (uint32_t i = 0; i < 8 * nof_bytes; ++i) {
      bref.pack(i, 1);
    }
    uint8_t a[] = {85, 85};
    TESTASSERT(bref.distance(bref0) / 8 == (int)nof_bytes);
    TESTASSERT(memcmp(a, buf, nof_bytes) == 0);
    bit_ref bref2(&buf[0], nof_bytes);
    for (uint32_t i = 0; i < 8 * nof_bytes; ++i) {
      bool val;
      bref2.unpack(val, 1);
      TESTASSERT((i & 1) == val);
    }
  }

  // three bits at a time
  {
    uint32_t nof_bytes = 3;
    bit_ref  bref(&buf[0], nof_bytes);
    bit_ref  bref0 = bref;
    for (uint32_t i = 0; i < 8 * nof_bytes / 3; ++i) {
      bref.pack(i, 3);
    }
    uint8_t a[] = {5, 57, 119};
    TESTASSERT(bref.distance(bref0) / 8 == (int)nof_bytes);
    TESTASSERT(memcmp(a, buf, nof_bytes) == 0);
    bit_ref bref2(&buf[0], nof_bytes);
    for (uint32_t i = 0; i < 8 * nof_bytes / 3; ++i) {
      uint32_t val;
      bref2.unpack(val, 3);
      TESTASSERT((i & 7) == val);
    }
  }

  // 16 bits at a time
  {
    uint32_t nof_bytes = 32, bitstride = 16;
    uint32_t start = 256, mask = (1u << bitstride) - 1u;
    bit_ref  bref(&buf[0], nof_bytes);
    for (uint32_t i = 0; i < 8 * nof_bytes / bitstride; ++i) {
      bref.pack(i + start, bitstride);
    }
    uint8_t a[] = {1, 0, 1, 1, 1, 2, 1, 3, 1, 4};
    TESTASSERT(bref.distance_bytes() == (int32_t)nof_bytes);
    TESTASSERT(memcmp(a, buf, sizeof(a)) == 0);
    bit_ref bref2(&buf[0], nof_bytes);
    for (uint32_t i = 0; i < 8 * nof_bytes / bitstride; ++i) {
      uint32_t val;
      bref2.unpack(val, bitstride);
      TESTASSERT(((i + start) & mask) == val);
    }
  }

  // pack bytes aligned
  {
    uint32_t nof_bytes = 512;
    uint32_t start     = 1;
    bit_ref  bref(&buf[0], sizeof(buf));
    uint8_t  buf2[1024];
    for (uint32_t i = 0; i < sizeof(buf2); ++i) {
      buf2[i] = start + i;
    }
    TESTASSERT(bref.pack_bytes(buf2, nof_bytes) == SRSASN_SUCCESS);
    TESTASSERT(bref.distance_bytes() == (int)nof_bytes);
    TESTASSERT(memcmp(buf2, buf, bref.distance_bytes()) == 0);
    bit_ref bref2(&buf[0], sizeof(buf));
    TESTASSERT(bref2.unpack_bytes(&buf2[0], nof_bytes) == SRSASN_SUCCESS);
    TESTASSERT(bref2.distance_bytes() == (int)nof_bytes);
    TESTASSERT(memcmp(buf2, buf, bref.distance_bytes()) == 0);
  }

  // pack bytes unaligned
  {
    uint32_t nof_bytes = 128;
    uint32_t start     = 1;
    bit_ref  bref(&buf[0], sizeof(buf));
    uint8_t  buf2[1024], buf3[1024];
    for (uint32_t i = 0; i < sizeof(buf2); ++i) {
      buf2[i] = start + i;
    }
    bzero(buf3, sizeof(buf3));
    // this unaligns
    TESTASSERT(bref.pack(0, 1) == SRSASN_SUCCESS);
    TESTASSERT(bref.pack_bytes(buf2, nof_bytes) == SRSASN_SUCCESS);
    TESTASSERT(bref.distance_bytes() == (int)nof_bytes + 1);
    uint8_t ar[] = {0, 129, 1, 130, 2, 131, 3, 132, 4, 133, 5};
    TESTASSERT(memcmp(ar, buf, sizeof(ar)) == 0);
    bit_ref  bref2(&buf[0], sizeof(buf));
    uint32_t val;
    TESTASSERT(bref2.unpack(val, 1) == SRSASN_SUCCESS);
    TESTASSERT(val == 0);
    TESTASSERT(bref2.unpack_bytes(&buf3[0], nof_bytes) == SRSASN_SUCCESS);
    TESTASSERT(bref2.distance_bytes() == (int)nof_bytes + 1);
    TESTASSERT(bref2.distance_bytes() == bref.distance_bytes());
    TESTASSERT(memcmp(buf2, buf3, nof_bytes) == 0);
  }

  // test advance bits
  {
    bit_ref bref(&buf[0], sizeof(buf));
    TESTASSERT(bref.advance_bits(4) == SRSASN_SUCCESS);
    TESTASSERT(bref.distance() == 4);
    TESTASSERT(bref.advance_bits(4) == SRSASN_SUCCESS);
    TESTASSERT(bref.distance() == 8);
    TESTASSERT(bref.advance_bits(3) == SRSASN_SUCCESS);
    TESTASSERT(bref.distance() == 11);
    TESTASSERT(bref.advance_bits(200) == SRSASN_SUCCESS);
    TESTASSERT(bref.distance() == 211);
    TESTASSERT(bref.advance_bits(5) == SRSASN_SUCCESS);
    TESTASSERT(bref.distance() == 216);
  }

  return 0;
}

int test_oct_string()
{
  uint8_t  buf[1024];
  bit_ref  b(&buf[0], sizeof(buf));
  cbit_ref b2(&buf[0], sizeof(buf));
  bit_ref  borig(b);

  std::string        hexstr = "014477aaff";
  fixed_octstring<5> statstr;
  dyn_octstring      dynstr;
  statstr.from_string(hexstr);
  dynstr.from_string(hexstr);

  TESTASSERT(sizeof(statstr) == statstr.size());
  TESTASSERT(statstr.size() == 5);
  TESTASSERT(dynstr.size() == 5);
  TESTASSERT(dynstr[0] == 1);
  TESTASSERT(statstr[4] == 255);
  TESTASSERT(statstr.to_string() == hexstr);
  TESTASSERT(statstr.to_string() == dynstr.to_string());
  TESTASSERT(statstr.to_number() == dynstr.to_number());

  // check endianess
  TESTASSERT(statstr.to_number() == 5443660543);
  TESTASSERT(dynstr.to_number() == 5443660543);
  dynstr.from_number(dynstr.to_number());
  TESTASSERT(dynstr.to_number() == 5443660543);

  statstr.pack(b);
  TESTASSERT(memcmp(&buf[0], &statstr[0], statstr.size()) == 0);
  TESTASSERT(b.distance(borig) == (int)statstr.size() * 8);
  fixed_octstring<5> statstr2;
  b2 = cbit_ref(&buf[0], sizeof(buf));
  statstr2.unpack(b2);
  TESTASSERT(statstr == statstr2);

  b = borig;
  dynstr.pack(b);
  TESTASSERT(buf[0] == dynstr.size()); // true for small strings
  TESTASSERT(memcmp(&buf[1], &dynstr[0], dynstr.size()) == 0);
  TESTASSERT(b.distance(borig) == (int)(dynstr.size() * 8 + 8));
  dyn_octstring dynstr2; // unpacker allocates automatically
  b2 = cbit_ref(&buf[0], sizeof(buf));
  dynstr2.unpack(b2);
  TESTASSERT(dynstr == dynstr2);

  // test copy ctor
  {
    fixed_octstring<5> statcopy(statstr);
    TESTASSERT(statcopy == statstr);
  }
  {
    dyn_octstring scopy(dynstr);
    TESTASSERT(scopy == dynstr);
  }

  b.pack(1, 1);
  TESTASSERT(b.distance(borig) == (int)(hexstr.size() * 8 / 2 + 9));
  b.align_bytes_zero();
  TESTASSERT(b.distance(borig) == (int)(hexstr.size() * 8 / 2 + 16));

  return 0;
}

int test_bitstring()
{
  fixed_bitstring<10>      bstr1;
  bounded_bitstring<5, 15> bound_bstr1(10);
  dyn_bitstring            dyn_bstr1(10);
  bstr1.set(7, true);
  bstr1.set(9, true);
  bound_bstr1.set(7, true);
  bound_bstr1.set(9, true);
  dyn_bstr1.set(7, true);
  dyn_bstr1.set(9, true);

  /* Test BitSet and BitGet and Length */
  // fixed
  TESTASSERT(bstr1.length() == 10);
  TESTASSERT(bstr1.get(7));
  TESTASSERT(bstr1.get(9));
  TESTASSERT(not bstr1.get(8));
  TESTASSERT(not bstr1.get(0));
  // bounded
  TESTASSERT(bound_bstr1.length() == 10);
  TESTASSERT(bound_bstr1.get(7));
  TESTASSERT(bound_bstr1.get(9));
  TESTASSERT(not bound_bstr1.get(8));
  TESTASSERT(not bound_bstr1.get(0));
  // dyn
  TESTASSERT(dyn_bstr1.length() == 10);
  TESTASSERT(dyn_bstr1.get(7));
  TESTASSERT(dyn_bstr1.get(9));
  TESTASSERT(not dyn_bstr1.get(8));
  TESTASSERT(not dyn_bstr1.get(0));

  /* Test Copy, Dtor, and Equal Comparison */
  {
    // fixed
    fixed_bitstring<10> bstr2;
    bstr2 = bstr1;
    TESTASSERT(bstr2 == bstr1);
    // bound
    bounded_bitstring<5, 15> bound_bstr2;
    bound_bstr2 = bound_bstr1;
    TESTASSERT(bound_bstr2 == bound_bstr1);
    // dyn
    dyn_bitstring dyn_bstr2(10);
    dyn_bstr2 = dyn_bstr1;
    TESTASSERT(dyn_bstr2 == dyn_bstr1);
    // dtors called
  }

  /* Test Number and String conversion */
  // fixed
  TESTASSERT(bstr1.to_string() == "1010000000");
  TESTASSERT(bstr1.to_number() == 640);
  bstr1.set(1, true);
  TESTASSERT(bstr1.to_string() == "1010000010");
  TESTASSERT(bstr1.to_number() == 642);
  TESTASSERT(bstr1 == fixed_bitstring<10>().from_number(642));
  // bounded
  TESTASSERT(bound_bstr1.to_string() == "1010000000");
  TESTASSERT(bound_bstr1.to_number() == 640);
  bound_bstr1.set(1, true);
  TESTASSERT(bound_bstr1.to_string() == "1010000010");
  TESTASSERT(bound_bstr1.to_number() == 642);
  TESTASSERT((bound_bstr1 == bounded_bitstring<5, 15>().from_number(642)));
  TESTASSERT((bound_bstr1 == bounded_bitstring<5, 15>().from_string("1010000010")));
  // dyn
  TESTASSERT(dyn_bstr1.to_string() == "1010000000");
  TESTASSERT(dyn_bstr1.to_number() == 640);
  dyn_bstr1.set(1, true);
  TESTASSERT(dyn_bstr1.to_string() == "1010000010");
  TESTASSERT(dyn_bstr1.to_number() == 642);
  TESTASSERT(dyn_bstr1 == dyn_bitstring().from_number(642));
  TESTASSERT(dyn_bstr1 == dyn_bitstring().from_string("1010000010"));
  // zero case
  TESTASSERT(bound_bstr1.from_number(0) == "00000");

  /* Test Resize */
  bound_bstr1.resize(11);
  TESTASSERT(not bound_bstr1.get(9)); // resets content
  TESTASSERT(bound_bstr1.length() == 11);
  bound_bstr1.set(9, true);
  dyn_bstr1.resize(11);
  TESTASSERT(not dyn_bstr1.get(9));
  TESTASSERT(dyn_bstr1.length() == 11);
  dyn_bstr1.set(9, true);
  TESTASSERT(dyn_bstr1.get(9));
  TESTASSERT(not dyn_bstr1.get(10));

  /* Test Packing/Unpacking */
  uint8_t  buffer[1024];
  bit_ref  bref(&buffer[0], sizeof(buffer));
  cbit_ref bref2(&buffer[0], sizeof(buffer));
  // fixed
  TESTASSERT(bstr1.pack(bref) == SRSASN_SUCCESS);
  fixed_bitstring<10> bstr2;
  TESTASSERT(bstr2.length() == 10);
  TESTASSERT(bstr2.unpack(bref2) == SRSASN_SUCCESS);
  TESTASSERT(bstr2 == bstr1);
  TESTASSERT(bref.distance() == 10 and bref.distance() == bref2.distance());
  // bounded
  bref  = bit_ref(&buffer[0], sizeof(buffer));
  bref2 = cbit_ref(&buffer[0], sizeof(buffer));
  TESTASSERT(bound_bstr1.pack(bref) == SRSASN_SUCCESS);
  bounded_bitstring<5, 15> bound_bstr2(dyn_bstr1.length());
  TESTASSERT(bound_bstr2.length() == 11);
  TESTASSERT(bound_bstr2.unpack(bref2) == SRSASN_SUCCESS);
  TESTASSERT(bound_bstr2 == bound_bstr1);
  TESTASSERT(bref.distance() == (11 + 4) and bref.distance() == bref2.distance());
  // dyn
  bref  = bit_ref(&buffer[0], sizeof(buffer));
  bref2 = cbit_ref(&buffer[0], sizeof(buffer));
  TESTASSERT(dyn_bstr1.pack(bref) == SRSASN_SUCCESS);
  dyn_bitstring dyn_bstr2(dyn_bstr1.length());
  TESTASSERT(dyn_bstr2.length() == 11);
  TESTASSERT(dyn_bstr2.unpack(bref2) == SRSASN_SUCCESS);
  TESTASSERT(dyn_bstr2 == dyn_bstr1);
  //  printf("%s==%s\n", dyn_bstr1.to_string().c_str(), dyn_bstr2.to_string().c_str());

  // disable temporarily the prints to check failures
  //  srslte::nullsink_log null_log("NULL");
  //  bit_ref bref3(&buffer[0], sizeof(buffer));
  //  TESTASSERT(dyn_bstr1.pack(bref3, false, 5, 10)==SRSASN_ERROR_ENCODE_FAIL);

  /* Test Pack/Unpack 2 */
  buffer[0] = 0;
  buffer[1] = 7;
  bref2     = cbit_ref(&buffer[0], sizeof(buffer));
  fixed_bitstring<16> bstr3;
  bstr3.unpack(bref2);
  TESTASSERT(bstr3 == "0000000000000111");
  TESTASSERT(bstr3.to_string() == "0000000000000111");

  return 0;
}

int test_seq_of()
{
  uint8_t buf[1024];
  memset(buf, 0, 1024);

  bit_ref  b(&buf[0], sizeof(buf));
  bit_ref  borig = b;
  cbit_ref borig2(&buf[0], sizeof(buf));

  uint32_t                 fixed_list_size = 33;
  std::array<uint32_t, 33> fixed_list;
  for (uint32_t i = 0; i < fixed_list_size; ++i) {
    fixed_list[i] = i;
  }
  {
    // test copy and ==
    std::array<uint32_t, 33> fixed_list2 = fixed_list;
    TESTASSERT(fixed_list == fixed_list2);
  }

  int      lb = 0, ub = 40;
  uint32_t n_bits = ceil(log2(ub - lb + 1));

  pack_fixed_seq_of(b, &fixed_list[0], fixed_list.size(), integer_packer<uint32_t>(lb, ub, false));
  TESTASSERT(b.distance(borig) == (int)(fixed_list_size * n_bits));
  cbit_ref                 b2(&buf[0], sizeof(buf));
  std::array<uint32_t, 33> fixed_list2;
  unpack_fixed_seq_of(&fixed_list2[0], b2, fixed_list.size(), integer_packer<uint32_t>(lb, ub, false));
  TESTASSERT(fixed_list == fixed_list2);

  // bounded seq_of
  bounded_array<uint32_t, 33> bseq;
  TESTASSERT(bseq.size() == 0);
  bseq.resize(fixed_list_size);
  TESTASSERT(bseq.size() == fixed_list_size);
  memcpy(&bseq[0], &fixed_list[0], fixed_list_size * sizeof(uint32_t));
  b = borig;
  pack_dyn_seq_of(b, bseq, 0, 33, integer_packer<uint32_t>(lb, ub, false));
  TESTASSERT(b.distance(borig) == (int)((fixed_list_size + 1) * n_bits)); // unaligned
  //  TESTASSERT(b.distance(borig)==fixed_list_size*n_bits+8); // aligned
  bounded_array<uint32_t, 33> bseq2;
  bseq2.resize(fixed_list_size);
  b2 = borig2;
  unpack_dyn_seq_of(bseq2, b2, 0, 33, integer_packer<uint32_t>(lb, ub, false));
  TESTASSERT(bseq2 == bseq);
  TESTASSERT(std::equal(&bseq2[0], &bseq2[fixed_list_size], &fixed_list[0]));

  {
    bounded_array<uint32_t, 33> bseq3;
    bseq3 = bseq;
    TESTASSERT(bseq == bseq3);
    // call dtor
  }
  TESTASSERT(bseq2 == bseq);

  // dynamic array
  dyn_array<int> vec, vec2(33);
  std::iota(vec2.begin(), vec2.end(), 0);
  std::shuffle(vec2.begin(), vec2.end(), g);
  TESTASSERT(vec2.size() == 33);
  TESTASSERT(vec.size() == 0);
  vec.resize(32);
  std::iota(vec.begin(), vec.end(), 0);
  std::shuffle(vec.begin(), vec.end(), g);
  TESTASSERT(vec.size() == 32);
  vec[5] = 5;
  vec.push_back(33);
  TESTASSERT(vec.size() == 33);
  TESTASSERT(vec[5] == 5); // resize does not affect content
  int* pos = &vec[5];
  vec.resize(10);
  TESTASSERT(vec.size() == 10); // reduction in size does not cause realloc
  TESTASSERT(pos == &vec[5]);
  {
    dyn_array<int> vec3 = vec2;
    TESTASSERT(vec3.size() == vec2.size());
    TESTASSERT(std::equal(vec3.begin(), vec3.end(), vec2.begin()));
    vec3 = vec;
    TESTASSERT(vec3.size() == vec.size());
    TESTASSERT(std::equal(vec3.begin(), vec3.end(), vec.begin()));
  }
  TESTASSERT(vec[5] == 5);

  return 0;
}

int test_copy_ptr()
{
  typedef fixed_octstring<10> TestType;
  char                        buffer[1024];
  uint32_t                    N = 10;
  for (uint32_t i = 0; i < N; ++i) {
    buffer[i] = i;
  }

  TestType*          s = new fixed_octstring<10>();
  copy_ptr<TestType> cptr(s);
  copy_ptr<TestType> cptr2;

  memcpy(&(*cptr)[0], buffer, N);

  TESTASSERT(cptr2.get() == NULL);
  TESTASSERT(memcmp(&(*s)[0], buffer, s->size()) == 0);
  TESTASSERT(*cptr == *s);
  TESTASSERT(cptr.get() == s);
  TESTASSERT(memcmp(&(*cptr)[0], buffer, cptr->size()) == 0);
  TESTASSERT(cptr2.get() == NULL);
  TESTASSERT(cptr.get() != NULL);

  {
    copy_ptr<TestType> cptr3 = cptr;
    TESTASSERT(cptr3 == cptr);
    TESTASSERT(*cptr3 == *s);
    TESTASSERT(cptr3.get() != cptr.get()); // different addresses
    TESTASSERT(memcmp(&(*cptr3)[0], buffer, cptr3->size()) == 0);
    // call dtor
  }
  TESTASSERT(*cptr == *s);

  // Test make_copy_ptr() function. It has the potential to hide all news from the code.
  TestType           s2(*s);
  copy_ptr<TestType> cptr3 = make_copy_ptr(s2);
  TESTASSERT(cptr3.get() != &s2);
  TESTASSERT(cptr3 == cptr);

  // get raw pointer and delete manually
  TestType* s3 = cptr.release();
  TESTASSERT(s3 == s); // same address
  delete s3;           // it should *not* double free

  return 0;
}

class EnumTest
{
public:
  enum options { test5, test10, test20, nulltype };
  options               value;
  static const uint32_t nof_types = 3, nof_exts = 0;
  static const bool     has_ext = false;
  EnumTest() {}
  EnumTest(options v) : value(v) {}
  EnumTest& operator=(options v)
  {
    value = v;
    return *this;
  }
              operator uint8_t() { return (uint8_t)value; }
  std::string to_string() const
  {
    switch (value) {
      case test5:
        return "test5";
      case test10:
        return "test10";
      case test20:
        return "test20";
      default:
        printf("invalid value\n");
    }
    return "";
  }
  int32_t to_number() const
  {
    switch (value) {
      case test5:
        return 5;
      case test10:
        return 10;
      case test20:
        return 20;
      default:
        printf("invalid value\n");
    }
    return -1;
  }
};

int test_enum()
{
  EnumTest e;
  EnumTest e2;
  e = EnumTest::test10;
  TESTASSERT(e.nof_types == 3);
  TESTASSERT(e.to_string() == "test10");
  TESTASSERT(e.to_number() == 10);
  TESTASSERT(string_to_enum<EnumTest>(e2, "test10"));
  TESTASSERT(e2 == e);
  TESTASSERT(number_to_enum<EnumTest>(e2, 10));
  TESTASSERT(e2 == e);

  uint8_t buffer[1024];
  bit_ref bref(&buffer[0], sizeof(buffer));
  bit_ref borig = bref;
  TESTASSERT(pack_enum(bref, e) == SRSASN_SUCCESS);
  TESTASSERT(bref.distance(borig) == (int)(floor(log2(e.nof_types)) + 1));

  cbit_ref bref2(&buffer[0], sizeof(buffer));
  TESTASSERT(unpack_enum(e2, bref2) == SRSASN_SUCCESS);
  TESTASSERT(bref2.distance() == (int)bref.distance());
  TESTASSERT(e == e2);

  // Test fail path
  srslte::scoped_log<srslte::nullsink_log> null_log("ASN1");
  bref  = bit_ref(&buffer[0], sizeof(buffer));
  bref2 = cbit_ref(&buffer[0], sizeof(buffer));
  e     = EnumTest::nulltype;
  TESTASSERT(pack_enum(bref, e) == SRSASN_ERROR_ENCODE_FAIL);
  buffer[0] = 255;
  TESTASSERT(unpack_enum(e, bref2) == SRSASN_ERROR_DECODE_FAIL);

  return 0;
}

int test_json_writer()
{
  json_writer writer;

  writer.start_array("");
  writer.start_obj("");
  writer.start_obj("obj1");
  writer.write_str("field1", "01010101");
  writer.write_int("field2", 5);
  writer.write_bool("field3", true);
  writer.start_obj("obj2");
  writer.write_int("field4", 1);
  writer.start_obj("obj3");
  writer.end_obj();
  writer.start_array("array1");
  writer.end_array();
  writer.end_obj();
  writer.end_obj();
  writer.end_obj();
  writer.end_array();

  printf("%s\n", writer.to_string().c_str());
  return 0;
}

int main()
{
  srslte::logmap::set_default_log_level(srslte::LOG_LEVEL_DEBUG);
  TESTASSERT(test_arrays() == 0);
  TESTASSERT(test_bit_ref() == 0);
  TESTASSERT(test_oct_string() == 0);
  TESTASSERT(test_bitstring() == 0);
  TESTASSERT(test_seq_of() == 0);
  TESTASSERT(test_copy_ptr() == 0);
  TESTASSERT(test_enum() == 0);
  //  TESTASSERT(test_json_writer()==0);
  printf("Success\n");
}
