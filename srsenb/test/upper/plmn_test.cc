/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2016 Software Radio Systems Limited
 *
 *
 */

#include <assert.h>
#include "srsenb/hdr/upper/common_enb.h"
#include "srslte/asn1/liblte_rrc.h"

void rrc_plmn_test()
{
  LIBLTE_RRC_PLMN_IDENTITY_STRUCT plmn_in, plmn_out;
  plmn_in.mcc = 0xF123;
  plmn_in.mnc = 0xFF45;

  // 2-digit MNC test
  uint8_t bit_buf[32];
  uint8_t *ie_ptr = bit_buf;

  liblte_rrc_pack_plmn_identity_ie(&plmn_in, &ie_ptr);
  uint8_t byte_buf[4];
  liblte_pack(bit_buf, 22, byte_buf);
  uint8_t ref[3] = {0x89, 0x19, 0x05};
  for(int i=0;i<3;i++) {
    assert(ref[i] == byte_buf[i]);
  }
  ie_ptr = bit_buf;
  liblte_rrc_unpack_plmn_identity_ie(&ie_ptr, &plmn_out);
  assert(plmn_in.mcc == plmn_out.mcc);
  assert(plmn_in.mnc == plmn_out.mnc);

  // 3-digit MNC test
  plmn_in.mnc = 0xF456;
  ie_ptr = bit_buf;
  liblte_rrc_pack_plmn_identity_ie(&plmn_in, &ie_ptr);
  liblte_pack(bit_buf, 26, byte_buf);
  uint8_t ref2[4] = {0x89, 0x1D, 0x15, 0x02};
  for(int i=0;i<3;i++) {
    assert(ref2[i] == byte_buf[i]);
  }
  ie_ptr = bit_buf;
  liblte_rrc_unpack_plmn_identity_ie(&ie_ptr, &plmn_out);
  assert(plmn_in.mcc == plmn_out.mcc);
  assert(plmn_in.mnc == plmn_out.mnc);
}

void s1ap_plmn_test()
{
  uint16_t mcc = 0xF123;
  uint16_t mnc = 0xFF45;
  uint32_t plmn;

  // 2-digit MNC test
  srsenb::s1ap_mccmnc_to_plmn(mcc, mnc, &plmn);
  assert(plmn == 0x21F354);
  srsenb::s1ap_plmn_to_mccmnc(plmn, &mcc, &mnc);
  assert(mcc == 0xF123);
  assert(mnc == 0xFF45);

  // 3-digit MNC test
  mnc = 0xF456;
  srsenb::s1ap_mccmnc_to_plmn(mcc, mnc, &plmn);
  assert(plmn == 0x216354);
  srsenb::s1ap_plmn_to_mccmnc(plmn, &mcc, &mnc);
  assert(mcc == 0xF123);
  assert(mnc == 0xF456);
}

int main(int argc, char **argv)
{
  rrc_plmn_test();
  s1ap_plmn_test();
}
