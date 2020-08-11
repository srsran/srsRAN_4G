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

#include "srslte/adt/span.h"
#include "srslte/common/test_common.h"

int test_span_access()
{
  std::array<int, 7> values{1, 2, 3, 4, 5, 6, 7};

  {
    srslte::span<int> view = values;

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

  return SRSLTE_SUCCESS;
}

int test_span_conversion()
{
  std::vector<int>   values{2, 3, 4, 5, 6, 7, 8};
  std::array<int, 7> values2{2, 3, 4, 5, 6, 7, 8};

  {
    // TEST: changing individual values
    srslte::span<int> v = values, v2 = values2;
    TESTASSERT(v == v2);

    v[0] = 3;
    TESTASSERT(v != v2);
  }
  TESTASSERT(values[0] == 3);
  TESTASSERT(values.size() == 7);
  TESTASSERT(values2.size() == 7);

  {
    // TEST: const context
    const srslte::span<int> v = values, v2 = values2;
    TESTASSERT(v != v2);
    TESTASSERT(v[0] == 3);
    TESTASSERT(v2[0] == 2);
    TESTASSERT(v.last(v.size() - 1) == v2.last(v2.size() - 1));
  }
  values[0] = 2;

  {
    // TEST: raw arrays
    int               carray[] = {2, 3, 4, 5, 6, 7, 8};
    srslte::span<int> v = values, v2 = carray;
    TESTASSERT(v == v2);
    TESTASSERT(v2.size() == v.size());
  }

  return SRSLTE_SUCCESS;
}

int test_byte_buffer_conversion()
{
  auto                  foo  = [](srslte::byte_span buffer) { return buffer.size() == 5 and buffer[4] == 4; };
  auto                  cfoo = [](const srslte::byte_span buffer) { return buffer.size() == 5 and buffer[4] == 4; };
  srslte::byte_buffer_t pdu;
  pdu.N_bytes = 5;
  pdu.msg[0]  = 0;
  pdu.msg[1]  = 1;
  pdu.msg[2]  = 2;
  pdu.msg[3]  = 3;
  pdu.msg[4]  = 4;

  {
    auto v = srslte::make_span(pdu);
    TESTASSERT(v.size() == 5);
    TESTASSERT(v[0] == 0);
    TESTASSERT(v[2] == 2);
    TESTASSERT(v[4] == 4);
  }

  const srslte::byte_buffer_t& pdu2 = pdu;
  {
    const auto v = srslte::make_span(pdu2);
    TESTASSERT(v.size() == 5);
    TESTASSERT(v[0] == 0);
    TESTASSERT(v[2] == 2);
    TESTASSERT(v[4] == 4);
  }

  TESTASSERT(foo(srslte::make_span(pdu)));
  TESTASSERT(cfoo(srslte::make_span(pdu)));

  return SRSLTE_SUCCESS;
}

int main()
{
  TESTASSERT(test_span_access() == SRSLTE_SUCCESS);
  TESTASSERT(test_span_conversion() == SRSLTE_SUCCESS);
  TESTASSERT(test_byte_buffer_conversion() == SRSLTE_SUCCESS);
  printf("Success\n");
  return SRSLTE_SUCCESS;
}
