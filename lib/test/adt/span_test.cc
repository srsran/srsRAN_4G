/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#include "srsran/adt/span.h"
#include "srsran/common/byte_buffer.h"
#include "srsran/common/test_common.h"

int test_span_access()
{
  std::array<int, 7> values{1, 2, 3, 4, 5, 6, 7};

  {
    srsran::span<int> view = values;

    // access operators
    TESTASSERT(view.size() == 7);
    TESTASSERT(view[0] == 1);
    TESTASSERT(view.front() == 1);
    TESTASSERT(view.back() == 7);
    TESTASSERT(*view.begin() == 1);
    TESTASSERT(*view.rbegin() == 7);

    // slicing
    TESTASSERT(view.first(7) == view);
    TESTASSERT(view.last(7) == view);
    TESTASSERT(view.first(4).size());
    TESTASSERT(view.first(4)[0] == 1);
    TESTASSERT(view.last(4)[0] == 4);
  }
  TESTASSERT(values.size() == 7);

  return SRSRAN_SUCCESS;
}

int test_span_conversion()
{
  std::vector<int>   values{2, 3, 4, 5, 6, 7, 8};
  std::array<int, 7> values2{2, 3, 4, 5, 6, 7, 8};

  {
    // TEST: changing individual values
    srsran::span<int> v = values, v2 = values2;
    TESTASSERT(v == v2);

    v[0] = 3;
    TESTASSERT(v != v2);
  }
  TESTASSERT(values[0] == 3);
  TESTASSERT(values.size() == 7);
  TESTASSERT(values2.size() == 7);

  {
    // TEST: const context
    const srsran::span<int> v = values, v2 = values2;
    TESTASSERT(v != v2);
    TESTASSERT(v[0] == 3);
    TESTASSERT(v2[0] == 2);
    TESTASSERT(v.last(v.size() - 1) == v2.last(v2.size() - 1));
  }
  values[0] = 2;

  {
    // TEST: raw arrays
    int               carray[] = {2, 3, 4, 5, 6, 7, 8};
    srsran::span<int> v = values, v2 = carray;
    TESTASSERT(v == v2);
    TESTASSERT(v2.size() == v.size());
  }

  return SRSRAN_SUCCESS;
}

int test_byte_buffer_conversion()
{
  auto                  foo  = [](srsran::byte_span buffer) { return buffer.size() == 5 and buffer[4] == 4; };
  auto                  cfoo = [](const srsran::byte_span buffer) { return buffer.size() == 5 and buffer[4] == 4; };
  srsran::byte_buffer_t pdu;
  pdu.N_bytes = 5;
  pdu.msg[0]  = 0;
  pdu.msg[1]  = 1;
  pdu.msg[2]  = 2;
  pdu.msg[3]  = 3;
  pdu.msg[4]  = 4;

  {
    auto v = srsran::make_span(pdu);
    TESTASSERT(v.size() == 5);
    TESTASSERT(v[0] == 0);
    TESTASSERT(v[2] == 2);
    TESTASSERT(v[4] == 4);
  }

  const srsran::byte_buffer_t& pdu2 = pdu;
  {
    const auto v = srsran::make_span(pdu2);
    TESTASSERT(v.size() == 5);
    TESTASSERT(v[0] == 0);
    TESTASSERT(v[2] == 2);
    TESTASSERT(v[4] == 4);
  }

  TESTASSERT(foo(srsran::make_span(pdu)));
  TESTASSERT(cfoo(srsran::make_span(pdu)));

  return SRSRAN_SUCCESS;
}

int main()
{
  TESTASSERT(test_span_access() == SRSRAN_SUCCESS);
  TESTASSERT(test_span_conversion() == SRSRAN_SUCCESS);
  TESTASSERT(test_byte_buffer_conversion() == SRSRAN_SUCCESS);
  printf("Success\n");
  return SRSRAN_SUCCESS;
}
