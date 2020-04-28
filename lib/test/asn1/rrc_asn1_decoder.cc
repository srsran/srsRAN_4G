/*
  Copyright 2013-2020 Software Radio Systems Limited

  This file is part of srsLTE

  srsLTE is free software: you can redistribute it and/or modify
  it under the terms of the GNU Affero General Public License as
  published by the Free Software Foundation, either version 3 of
  the License, or (at your option) any later version.

  srsLTE is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Affero General Public License for more details.

  A copy of the GNU Affero General Public License can be found in
  the LICENSE file in the top-level directory of this distribution
  and at http://www.gnu.org/licenses/.
*/

#include <getopt.h>
#include <iostream>

#include "srslte/asn1/asn1_utils.h"
#include "srslte/asn1/rrc_asn1.h"

using namespace std;

using namespace asn1;
using namespace asn1::rrc;

typedef enum {
  DL_CCCH = 0,
  DL_DCCH,
  UL_CCCH,
  UL_DCCH,
  BCCH_BCH,
  BCCH_DL_SCH,
  PCCH,
  MCCH,
  RRC_MSG_N_ITEMS,
} rrc_msg_types;

static const char rrc_msg_type_text[RRC_MSG_N_ITEMS][20] =
    {"dl_ccch", "dl_dcch", "ul_ccch", "ul_dcch", "bcch_bch", "bcch_dl_sch", "mcch", "pcch"};

int check_msg_type(char* input_msg_type)
{
  int i;
  for (i = 0; i < RRC_MSG_N_ITEMS; i++) {
    if (strcasecmp(input_msg_type, rrc_msg_type_text[i]) == 0)
      return i;
  }
  return -1;
}

void usage(char* prog)
{
  printf("Usage: %s [fph]\n", prog);
  printf("\t-f file\n");
  printf("\t-p json print\n");
  printf("\t-h show this message\n");
  printf("\t-t type: dl_ccch, dl_dcch, ul_ccch, ul_dcch, bcch_bch, bcch_dl_sch, mcch, pcch\n");
}

int main(int argc, char** argv)
{
  FILE*       fd       = NULL;
  SRSASN_CODE err_code = SRSASN_ERROR_DECODE_FAIL;

  uint8_t* rrc_msg = NULL;
  int      verbose = 0;
  char*    file    = NULL;
  int      type = -1;

  if (argc < 2) {
    printf("Please only call me with one parameter\n");
    usage(argv[0]);
    exit(-1);
  }
  int opt;

  while ((opt = getopt(argc, argv, "f:pht:")) != -1) {
    switch (opt) {
      case 'f':
        file = optarg;
        break;
      case 'p':
        verbose++;
        break;
      case 't':
        type = check_msg_type(optarg);
        if (type == -1) {
          printf("[Error] Invalid message type %s\n", optarg);
          usage(argv[0]);
        }
        break;
      case 'h':
      default:
        usage(argv[0]);
        exit(-1);
    }
  }

  fd = fopen(file, "rb");
  if (fd == NULL) {
    printf("Error failed to open file %s\n", file);
    exit(-1);
  }

  fseek(fd, 0, SEEK_END);
  long fsize = ftell(fd);
  fseek(fd, 0, SEEK_SET); /* same as rewind(f); */

  rrc_msg = new uint8_t[fsize];

  size_t read_size = fread(rrc_msg, 1, fsize, fd);
  fclose(fd);
  if (read_size != (size_t)fsize) {
    printf("read_size != fsize\n");
    return -1;
  }

  asn1::cbit_ref bref(rrc_msg, fsize);
  // TODO do other rrc messages and type

  switch (type) {
    case DL_CCCH: {
      dl_ccch_msg_s dl_ccch_msg;
      err_code = dl_ccch_msg.unpack(bref);
      if (err_code == SRSASN_SUCCESS) {
        printf("Decoding Success\n");
        if (verbose > 0) {
          json_writer jw;
          dl_ccch_msg.to_json(jw);
          printf("RRC DL CCCH Message:\n");
          printf("%s\n", jw.to_string().c_str());
        }
      } else {
        printf("Decoding Error\n");
      }
      break;
    }
    case DL_DCCH: {
      dl_dcch_msg_s dl_dcch_msg;
      err_code = dl_dcch_msg.unpack(bref);
      if (err_code == SRSASN_SUCCESS) {
        printf("Decoding Success\n");
        if (verbose > 0) {
          json_writer jw;
          dl_dcch_msg.to_json(jw);
          printf("RRC DL DCCH Message:\n");
          printf("%s\n", jw.to_string().c_str());
        }
      } else {
        printf("Decoding Error\n");
      }
      break;
    }
    case UL_CCCH: {
      ul_ccch_msg_s ul_ccch_msg;
      err_code = ul_ccch_msg.unpack(bref);
      if (err_code == SRSASN_SUCCESS) {
        printf("Decoding Success\n");
        if (verbose > 0) {
          json_writer jw;
          ul_ccch_msg.to_json(jw);
          printf("RRC UL CCCH Message:\n");
          printf("%s\n", jw.to_string().c_str());
        }
      } else {
        printf("Decoding Error\n");
      }
      break;
    }
    case UL_DCCH: {
      ul_dcch_msg_s ul_dcch_msg;
      err_code = ul_dcch_msg.unpack(bref);
      if (err_code == SRSASN_SUCCESS) {
        printf("Decoding Success\n");
        if (verbose > 0) {
          json_writer jw;
          ul_dcch_msg.to_json(jw);
          printf("RRC UL DCCH Message:\n");
          printf("%s\n", jw.to_string().c_str());
        }
      } else {
        printf("Decoding Error\n");
      }
      break;
    }
    case BCCH_BCH: {
      bcch_bch_msg_s bcch_bch_msg;
      err_code = bcch_bch_msg.unpack(bref);
      if (err_code == SRSASN_SUCCESS) {
        printf("Decoding Success\n");
        if (verbose > 0) {
          json_writer jw;
          bcch_bch_msg.to_json(jw);
          printf("RRC BCCH BCH Message:\n");
          printf("%s\n", jw.to_string().c_str());
        }
      } else {
        printf("Decoding Error\n");
      }
      break;
    }
    case BCCH_DL_SCH: {
      bcch_dl_sch_msg_s bcch_dl_sch_msg;
      err_code = bcch_dl_sch_msg.unpack(bref);
      if (err_code == SRSASN_SUCCESS) {
        printf("Decoding Success\n");
        if (verbose > 0) {
          json_writer jw;
          bcch_dl_sch_msg.to_json(jw);
          printf("RRC SBCCH SL BCH Message:\n");
          printf("%s\n", jw.to_string().c_str());
        }
      } else {
        printf("Decoding Error\n");
      }
      break;
    }
    case MCCH: {
      mcch_msg_s mcch_msg;
      err_code = mcch_msg.unpack(bref);
      if (err_code == SRSASN_SUCCESS) {
        printf("Decoding Success\n");
        if (verbose > 0) {
          json_writer jw;
          mcch_msg.to_json(jw);
          printf("RRC MCCH Message:\n");
          printf("%s\n", jw.to_string().c_str());
        }
      } else {
        printf("Decoding Error\n");
      }
      break;
    }
    case PCCH: {
      pcch_msg_s pcch_msg;
      err_code = pcch_msg.unpack(bref);
      if (err_code == SRSASN_SUCCESS) {
        printf("Decoding Success\n");
        if (verbose > 0) {
          json_writer jw;
          pcch_msg.to_json(jw);
          printf("RRC PCCH Message:\n");
          printf("%s\n", jw.to_string().c_str());
        }
      } else {
        printf("Decoding Error\n");
      }
      break;
    }
    default: {
      printf("[Error] type not supported\n");
      break;
    }
  }

  delete[] rrc_msg;

  return 0;
}
