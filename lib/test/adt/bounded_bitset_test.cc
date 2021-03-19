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

#include "srsran/adt/bounded_bitset.h"
#include "srsran/common/test_common.h"

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

int main()
{
  TESTASSERT(test_zero_bitset() == SRSRAN_SUCCESS);
  TESTASSERT(test_ones_bitset() == SRSRAN_SUCCESS);
  TESTASSERT(test_bitset_set() == SRSRAN_SUCCESS);
  TESTASSERT(test_bitset_bitwise_oper() == SRSRAN_SUCCESS);
  TESTASSERT(test_bitset_print() == SRSRAN_SUCCESS);
  TESTASSERT(test_bitset_resize() == SRSRAN_SUCCESS);
  printf("Success\n");
  return 0;
}
