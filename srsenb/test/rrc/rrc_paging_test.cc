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

#include "srsenb/hdr/stack/rrc/rrc_paging.h"
#include "srsran/common/test_common.h"

using namespace srsenb;

void test_paging()
{
  unsigned       paging_cycle = 32;
  float          nb           = 1;
  paging_manager pcch_manager{paging_cycle, nb};

  unsigned ue_id    = 4780;
  unsigned mmec     = 10;
  uint8_t  m_tmsi[] = {0x64, 0x04, 0x00, 0x02};

  pcch_manager.add_tmsi_paging(ue_id, mmec, m_tmsi);

  // \remark: See TS 36.304, section 7.1.
  unsigned N   = std::min(paging_cycle, (unsigned)std::round(nb * paging_cycle));
  unsigned Ns  = std::max(1, (int)nb);
  unsigned i_s = (ue_id / N) % Ns;
  TESTASSERT_EQ(0, i_s);
  tti_point t{0};
  for (unsigned count = 0; count < 1024 * 10; ++count, ++t) {
    if (pcch_manager.pending_pcch_bytes(t) > 0) {
      fmt::print("[{}]\n", t);
      TESTASSERT_EQ((paging_cycle / N) * (ue_id % N), (t.sfn() % paging_cycle));
      TESTASSERT_EQ(9, t.sf_idx()); // PO when i_s == 0.
    }
  }
}

int main()
{
  test_paging();
}