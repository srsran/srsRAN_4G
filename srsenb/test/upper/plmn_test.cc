/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2016 Software Radio Systems Limited
 *
 *
 */

#include "srsenb/hdr/upper/common_enb.h"
#include "srslte/asn1/rrc_asn1.h"
#include "srslte/common/bcd_helpers.h"
#include <iostream>

using namespace asn1::rrc;

#define TESTASSERT(cond)                                                                                               \
  {                                                                                                                    \
    if (!(cond)) {                                                                                                     \
      std::cout << "[" << __FUNCTION__ << "][Line " << __LINE__ << "]: FAIL at " << (#cond) << std::endl;              \
      return -1;                                                                                                       \
    }                                                                                                                  \
  }

int rrc_plmn_test()
{
  plmn_id_s plmn_in, plmn_out;
  uint8_t   ref[3] = {0x89, 0x19, 0x14};
  uint8_t   byte_buf[4];

  // 2-digit MNC test
  asn1::bit_ref bref_out(&ref[0], sizeof(ref));
  asn1::bit_ref bref_out0(&ref[0], sizeof(ref));

  plmn_out.unpack(bref_out);

  TESTASSERT(plmn_out.mcc_present);
  uint16_t mcc, mnc;
  srslte::bytes_to_mcc(&plmn_out.mcc[0], &mcc);
  srslte::bytes_to_mnc(&plmn_out.mnc[0], &mnc, plmn_out.mnc.size());
  TESTASSERT(mcc == 0xF123);
  TESTASSERT(mnc == 0xFF45);

  // Test MCC/MNC --> vector
  plmn_in.mcc_present = plmn_out.mcc_present;
  TESTASSERT(srslte::mcc_to_bytes(mcc, &plmn_in.mcc[0]));
  TESTASSERT(srslte::mnc_to_bytes(mnc, plmn_in.mnc));
  TESTASSERT(plmn_in.mcc_present == plmn_out.mcc_present);
  TESTASSERT(plmn_in.mcc == plmn_out.mcc);
  TESTASSERT(plmn_in.mnc == plmn_out.mnc);

  // Test plmn --> string
  std::string mccmnc_str = srslte::plmn_id_to_string(plmn_in);
  TESTASSERT(mccmnc_str == "12345");

  asn1::bit_ref bref_in(&byte_buf[0], sizeof(byte_buf));
  asn1::bit_ref bref_in0(&byte_buf[0], sizeof(byte_buf));
  plmn_out.pack(bref_in);

  TESTASSERT(bref_in.distance(&byte_buf[0]) == bref_out.distance(bref_out0));
  TESTASSERT(memcmp(&ref[0], &byte_buf[0], sizeof(ref)) == 0);

  // 3-digit MNC test
  TESTASSERT(srslte::mnc_to_bytes(0xF456, plmn_in.mnc));
  bref_in = asn1::bit_ref(&byte_buf[0], sizeof(byte_buf));
  plmn_in.pack(bref_in);
  uint8_t ref2[4] = {0x89, 0x1D, 0x15, 0x80};
  TESTASSERT(bref_in.distance(bref_in0) == (1 + 3 * 4 + 1 + 3 * 4));
  TESTASSERT(memcmp(&byte_buf[0], &ref2[0], sizeof(ref)) == 0);

  bref_out = asn1::bit_ref(&ref2[0], sizeof(ref2));
  plmn_out.unpack(bref_out);
  TESTASSERT(plmn_in.mcc_present == plmn_out.mcc_present);
  TESTASSERT(plmn_in.mcc == plmn_out.mcc);
  TESTASSERT(plmn_in.mnc == plmn_out.mnc);

  return 0;
}

int s1ap_plmn_test()
{
  uint16_t mcc = 0xF123;
  uint16_t mnc = 0xFF45;
  uint32_t plmn;

  // 2-digit MNC test
  srslte::s1ap_mccmnc_to_plmn(mcc, mnc, &plmn);
  TESTASSERT(plmn == 0x21F354);
  srslte::s1ap_plmn_to_mccmnc(plmn, &mcc, &mnc);
  TESTASSERT(mcc == 0xF123);
  TESTASSERT(mnc == 0xFF45);

  // 3-digit MNC test
  mnc = 0xF456;
  srslte::s1ap_mccmnc_to_plmn(mcc, mnc, &plmn);
  TESTASSERT(plmn == 0x216354);
  srslte::s1ap_plmn_to_mccmnc(plmn, &mcc, &mnc);
  TESTASSERT(mcc == 0xF123);
  TESTASSERT(mnc == 0xF456);
  return 0;
}

int main(int argc, char **argv)
{
  TESTASSERT(rrc_plmn_test() == 0);
  TESTASSERT(s1ap_plmn_test() == 0);
  return 0;
}
