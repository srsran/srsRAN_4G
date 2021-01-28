/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsue/hdr/stack/upper/pcsc_usim.h"
#include <assert.h>
#include <iostream>

using namespace srsue;
using namespace std;

uint8_t rand_enb[] = {0xbc, 0x4c, 0xb0, 0x27, 0xb3, 0x4b, 0x7f, 0x51, 0x21, 0x5e, 0x56, 0x5f, 0x67, 0x3f, 0xde, 0x4f};
uint8_t autn_enb[] = {0x5a, 0x17, 0x77, 0x3c, 0x62, 0x57, 0x90, 0x01, 0xcf, 0x47, 0xf7, 0x6d, 0xb3, 0xa0, 0x19, 0x46};

int main(int argc, char** argv)
{
  srslog::basic_logger& logger = srslog::fetch_basic_logger("USIM", false);
  logger.set_level(srslog::basic_levels::debug);
  logger.set_hex_dump_max_size(100000);
  srslog::init();

  uint8_t res[16];
  int     res_len;
  uint8_t k_asme[32];
  uint16  mcc = 0;
  uint16  mnc = 0;

  usim_args_t args;
  args.pin  = "6129";
  args.imei = "353490069873319";

  srsue::pcsc_usim usim(logger);
  if (usim.init(&args)) {
    printf("Error initializing PC/SC USIM.\n");
    return SRSLTE_ERROR;
  };

  std::string imsi = usim.get_imsi_str();
  cout << "IMSI: " << imsi << endl;

  srslte::plmn_id_t home_plmn_id = {};
  if (usim.get_home_plmn_id(&home_plmn_id) == false) {
    printf("Error reading home PLMN\n");
    return SRSLTE_ERROR;
  }

  cout << "Home PLMN: " << home_plmn_id.to_string() << endl;

  auth_result_t result = usim.generate_authentication_response(rand_enb, autn_enb, mcc, mnc, res, &res_len, k_asme);

  return SRSLTE_SUCCESS;
}
