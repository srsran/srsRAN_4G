/**
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srsran/adt/bounded_bitset.h"
#include "srsran/common/test_common.h"

void test_bit_operations()
{
  TESTASSERT(0 == srsran::mask_lsb_ones<uint8_t>(0));
  TESTASSERT(0 == srsran::mask_msb_ones<uint8_t>(0));
  TESTASSERT(0 == srsran::mask_lsb_ones<uint32_t>(0));
  TESTASSERT(0 == srsran::mask_msb_ones<uint32_t>(0));
  TESTASSERT(0b11111111 == srsran::mask_lsb_zeros<uint8_t>(0));
  TESTASSERT(0b11111111 == srsran::mask_msb_zeros<uint8_t>(0));
  TESTASSERT((uint32_t)-1 == srsran::mask_lsb_zeros<uint32_t>(0));
  TESTASSERT((uint32_t)-1 == srsran::mask_msb_zeros<uint32_t>(0));

  TESTASSERT(0b11 == srsran::mask_lsb_ones<uint8_t>(2));
  TESTASSERT(0b11000000 == srsran::mask_msb_ones<uint8_t>(2));
  TESTASSERT(0b11111100 == srsran::mask_lsb_zeros<uint8_t>(2));
  TESTASSERT(0b00111111 == srsran::mask_msb_zeros<uint8_t>(2));

  TESTASSERT(0b11111111 == srsran::mask_lsb_ones<uint8_t>(8));
  TESTASSERT(0b1111 == srsran::mask_lsb_ones<uint8_t>(4));

  TESTASSERT(srsran::find_first_lsb_one<uint8_t>(0) == std::numeric_limits<uint8_t>::digits);
  TESTASSERT(srsran::find_first_msb_one<uint8_t>(0) == std::numeric_limits<uint8_t>::digits);
  TESTASSERT(srsran::find_first_lsb_one<uint8_t>(1) == 0);
  TESTASSERT(srsran::find_first_msb_one<uint8_t>(1) == 0);
  TESTASSERT(srsran::find_first_lsb_one<uint8_t>(0b11) == 0);
  TESTASSERT(srsran::find_first_lsb_one<uint8_t>(0b10) == 1);
  TESTASSERT(srsran::find_first_msb_one<uint8_t>(0b11) == 1);
  TESTASSERT(srsran::find_first_msb_one<uint8_t>(0b10) == 1);
  TESTASSERT(srsran::find_first_lsb_one<uint32_t>(0b11) == 0);
  TESTASSERT(srsran::find_first_lsb_one<uint32_t>(0b10) == 1);
  TESTASSERT(srsran::find_first_msb_one<uint32_t>(0b11) == 1);
  TESTASSERT(srsran::find_first_msb_one<uint32_t>(0b10) == 1);
}

int test_zero_bitset()
{
  srsran::bounded_bitset<25> mask;
  srsran::bounded_bitset<25> mask2{23};

  TESTASSERT(mask.max_size() == 25);
  TESTASSERT(mask.size() == 0);
  TESTASSERT(mask.count() == 0);
  TESTASSERT(mask.none());
  TESTASSERT(not mask.any());
  TESTASSERT(mask.all());
  TESTASSERT(mask != mask2);

  TESTASSERT(mask2.max_size() == 25);
  TESTASSERT(mask2.size() == 23);
  TESTASSERT(mask2.count() == 0);
  TESTASSERT(mask2.none());
  TESTASSERT(not mask2.any());
  TESTASSERT(not mask2.all());

  mask = mask2;
  TESTASSERT(mask == mask2);

  return SRSRAN_SUCCESS;
}

int test_ones_bitset()
{
  srsran::bounded_bitset<25> mask;

  // Flipping empty bitset is noop
  TESTASSERT(mask.none() and mask.all() and not mask.any());
  mask.flip();
  TESTASSERT(mask.none() and mask.all() and not mask.any());

  // Flipping zeros bitset with size>0 will set all bits to one
  mask.resize(23);
  TESTASSERT(mask.none() and not mask.all() and not mask.any());
  mask.flip();
  TESTASSERT(not mask.none() and mask.all() and mask.any());

  return SRSRAN_SUCCESS;
}

int test_bitset_set()
{
  srsran::bounded_bitset<25> mask{23};
  mask.set(10);

  TESTASSERT(mask.any());
  TESTASSERT(not mask.all());
  TESTASSERT(not mask.test(0));
  TESTASSERT(mask.test(10));
  mask.flip();
  TESTASSERT(not mask.test(10));
  TESTASSERT(mask.test(0));

  return SRSRAN_SUCCESS;
}

int test_bitset_bitwise_oper()
{
  srsran::bounded_bitset<25> mask{23};
  srsran::bounded_bitset<25> mask2{23};

  mask.set(10);
  TESTASSERT(mask != mask2);
  mask2 |= mask;
  TESTASSERT(mask == mask2);

  mask.set(11);
  mask2 &= mask;
  TESTASSERT(mask != mask2);
  TESTASSERT(mask2.test(10) and not mask2.test(11));

#if EXCEPTIONS_ENABLED
  bool caught = false;
  mask2.resize(24);
  try {
    mask2 |= mask;
  } catch (srsran::bad_type_access& c) {
    printf("Received exception \"%s\" (as expected)\n", c.what());
    caught = true;
  }
  TESTASSERT(caught);
#endif

  return SRSRAN_SUCCESS;
}

int test_bitset_print()
{
  {
    srsran::bounded_bitset<100> bitset(100);
    bitset.set(0);
    bitset.set(5);

    TESTASSERT(fmt::format("{:x}", bitset) == "0000000000000000000000021");
    TESTASSERT(fmt::format("{:b}", bitset) ==
               "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000100001");

    bitset.set(99);
    TESTASSERT(fmt::format("{:x}", bitset) == "8000000000000000000000021");
    TESTASSERT(fmt::format("{:b}", bitset) ==
               "1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000100001");
  }

  {
    srsran::bounded_bitset<100> bitset(25);
    bitset.set(0);
    bitset.set(4);

    TESTASSERT(fmt::format("{:x}", bitset) == "0000011");
    TESTASSERT(fmt::format("{:b}", bitset) == "0000000000000000000010001");

    bitset.set(24);
    TESTASSERT(fmt::format("{:x}", bitset) == "1000011");
    TESTASSERT(fmt::format("{:b}", bitset) == "1000000000000000000010001");
  }

  return SRSRAN_SUCCESS;
}

int test_bitset_resize()
{
  {
    srsran::bounded_bitset<100> bitset;
    TESTASSERT(bitset.none() and bitset.size() == 0);

    bitset.resize(100);
    TESTASSERT(bitset.none() and bitset.size() == 100);
    bitset.fill(0, 100);
    TESTASSERT(bitset.all() and bitset.size() == 100);

    bitset.resize(25);
    TESTASSERT(bitset.to_uint64() == 0x1ffffff);
    TESTASSERT(bitset.all() and bitset.size() == 25); // keeps the data it had for the non-erased bits

    bitset.resize(100);
    TESTASSERT(bitset.count() == 25 and bitset.size() == 100);
  }

  {
    // TEST: Reverse case
    srsran::bounded_bitset<100, true> bitset;
    TESTASSERT(bitset.none() and bitset.size() == 0);

    bitset.resize(100);
    TESTASSERT(bitset.none() and bitset.size() == 100);
    bitset.fill(0, 100);
    TESTASSERT(bitset.all() and bitset.size() == 100);

    bitset.resize(25);
    TESTASSERT(bitset.to_uint64() == 0x1ffffff);
    TESTASSERT(bitset.all() and bitset.size() == 25); // keeps the data it had for the non-erased bits

    bitset.resize(100);
    TESTASSERT(bitset.count() == 25 and bitset.size() == 100);
  }

  return SRSRAN_SUCCESS;
}

template <bool reversed>
void test_bitset_find()
{
  {
    srsran::bounded_bitset<25, reversed> bitset(6);

    // 0b000000
    TESTASSERT(bitset.find_lowest(0, bitset.size(), false) == 0);
    TESTASSERT(bitset.find_lowest(0, bitset.size(), true) == -1);

    // 0b000100
    bitset.set(2);
    TESTASSERT(bitset.find_lowest(0, 6) == 2);
    TESTASSERT(bitset.find_lowest(0, 6, false) == 0);
    TESTASSERT(bitset.find_lowest(3, 6) == -1);
    TESTASSERT(bitset.find_lowest(3, 6, false) == 3);

    // 0b000101
    bitset.set(0);
    TESTASSERT(bitset.find_lowest(0, 6) == 0);
    TESTASSERT(bitset.find_lowest(0, 6, false) == 1);
    TESTASSERT(bitset.find_lowest(3, 6) == -1);
    TESTASSERT(bitset.find_lowest(3, 6, false) == 3);

    // 0b100101
    bitset.set(5);
    TESTASSERT(bitset.find_lowest(0, 6) == 0);
    TESTASSERT(bitset.find_lowest(0, 6, false) == 1);
    TESTASSERT(bitset.find_lowest(3, 6) == 5);

    // 0b111111
    bitset.fill(0, 6);
    TESTASSERT(bitset.find_lowest(0, 6) == 0);
    TESTASSERT(bitset.find_lowest(0, 6, false) == -1);
    TESTASSERT(bitset.find_lowest(3, 6, false) == -1);
  }
  {
    srsran::bounded_bitset<100, reversed> bitset(95);

    // 0b0...0
    TESTASSERT(bitset.find_lowest(0, bitset.size()) == -1);

    // 0b1000...
    bitset.set(94);
    TESTASSERT(bitset.find_lowest(0, 93) == -1);
    TESTASSERT(bitset.find_lowest(0, bitset.size()) == 94);

    // 0b1000...010
    bitset.set(1);
    TESTASSERT(bitset.find_lowest(0, bitset.size()) == 1);
    TESTASSERT(bitset.find_lowest(1, bitset.size()) == 1);
    TESTASSERT(bitset.find_lowest(2, bitset.size()) == 94);

    // 0b11..11
    bitset.fill(0, bitset.size());
    TESTASSERT(bitset.find_lowest(0, bitset.size()) == 0);
    TESTASSERT(bitset.find_lowest(5, bitset.size()) == 5);
  }
}

int main()
{
  test_bit_operations();
  TESTASSERT(test_zero_bitset() == SRSRAN_SUCCESS);
  TESTASSERT(test_ones_bitset() == SRSRAN_SUCCESS);
  TESTASSERT(test_bitset_set() == SRSRAN_SUCCESS);
  TESTASSERT(test_bitset_bitwise_oper() == SRSRAN_SUCCESS);
  TESTASSERT(test_bitset_print() == SRSRAN_SUCCESS);
  TESTASSERT(test_bitset_resize() == SRSRAN_SUCCESS);
  test_bitset_find<false>();
  test_bitset_find<true>();
  printf("Success\n");
  return 0;
}
