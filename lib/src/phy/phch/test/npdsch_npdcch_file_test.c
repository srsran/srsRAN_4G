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

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

#include "srslte/phy/channel/ch_awgn.h"
#include "srslte/phy/io/filesource.h"
#include "srslte/phy/phch/npdsch.h"
#include "srslte/phy/ue/ue_dl_nbiot.h"
#include "srslte/phy/utils/debug.h"

#define DUMP_SIGNAL 0

char* input_file_name = NULL;

static srslte_nbiot_cell_t cell = {.base       = {.nof_prb = 1, .nof_ports = 1, .cp = SRSLTE_CP_NORM, .id = 0},
                                   .nbiot_prb  = 0,
                                   .n_id_ncell = 0,
                                   .nof_ports  = 1,
                                   .mode       = SRSLTE_NBIOT_MODE_STANDALONE,
                                   .is_r14     = true};

int flen;

uint16_t rnti = SRSLTE_SIRNTI;

int             sfn            = 0;
int             nof_frames     = 0;
int             max_frames     = 1;
uint32_t        sf_idx         = 0;
uint32_t        sched_info_tag = 0; // Value of schedulingInfoSIB1-NB-r13
srslte_mib_nb_t mib;
bool            decode_sib1 = false;
float           snr         = -1.0;

srslte_dci_format_t  dci_format = SRSLTE_DCI_FORMAT1;
srslte_filesource_t  fsrc;
srslte_nbiot_ue_dl_t ue_dl;
cf_t*                buff_ptrs[SRSLTE_MAX_PORTS] = {NULL, NULL, NULL, NULL};

void usage(char* prog)
{
  printf("Usage: %s [rovcnwmpstRx] -i input_file\n", prog);
  printf("\t-o DCI format [Default %s]\n", srslte_dci_format_string(dci_format));
  printf("\t-c n_id_ncell [Default %d]\n", cell.n_id_ncell);
  printf("\t-s Start subframe_idx [Default %d]\n", sf_idx);
  printf("\t-w Start SFN [Default %d]\n", nof_frames);
  printf("\t-t Value of schedulingInfoSIB1-NB-r13 [Default %d]\n", sched_info_tag);
  printf("\t-k Decode SIB1 [Default %s]\n", decode_sib1 ? "Yes" : "No");
  printf("\t-r rnti [Default 0x%x]\n", rnti);
  printf("\t-p cell.nof_ports [Default %d]\n", cell.base.nof_ports);
  printf("\t-n cell.nof_prb [Default %d]\n", cell.base.nof_prb);
  printf("\t-m max_frames [Default %d]\n", max_frames);
  printf("\t-R Is R14 cell [Default %s]\n", cell.is_r14 ? "Yes" : "No");
  printf("\t-x SNR-10 (apply noise to input file) [Default %f]\n", snr);
  printf("\t-v [set srslte_verbose to debug, default none]\n");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "irovcnmwpkstx")) != -1) {
    switch (opt) {
      case 'i':
        input_file_name = argv[optind];
        break;
      case 'c':
        cell.n_id_ncell = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'R':
        cell.is_r14 = !cell.is_r14;
        break;
      case 's':
        sf_idx = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 't':
        sched_info_tag = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'r':
        rnti = strtoul(argv[optind], NULL, 0);
        break;
      case 'w':
        sfn = strtoul(argv[optind], NULL, 0);
        break;
      case 'm':
        max_frames = strtoul(argv[optind], NULL, 0);
        break;
      case 'n':
        cell.base.nof_prb = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'k':
        decode_sib1 = true;
        break;
      case 'p':
        cell.base.nof_ports = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'o':
        dci_format = srslte_dci_format_from_string(argv[optind]);
        if (dci_format == SRSLTE_DCI_NOF_FORMATS) {
          fprintf(stderr, "Error unsupported format %s\n", argv[optind]);
          exit(-1);
        }
        break;
      case 'x':
        snr = strtof(argv[optind], NULL);
        break;
      case 'v':
        srslte_verbose++;
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
  if (!input_file_name) {
    usage(argv[0]);
    exit(-1);
  }
}

int base_init()
{
  if (srslte_filesource_init(&fsrc, input_file_name, SRSLTE_COMPLEX_FLOAT_BIN)) {
    fprintf(stderr, "Error opening file %s\n", input_file_name);
    exit(-1);
  }

  flen         = 2 * (SRSLTE_SLOT_LEN(srslte_symbol_sz(cell.base.nof_prb)));
  buff_ptrs[0] = srslte_vec_cf_malloc(flen);
  if (!buff_ptrs[0]) {
    perror("malloc");
    exit(-1);
  }

  if (srslte_nbiot_ue_dl_init(&ue_dl, buff_ptrs, SRSLTE_NBIOT_MAX_PRB, SRSLTE_NBIOT_NUM_RX_ANTENNAS)) {
    fprintf(stderr, "Error initializing UE DL\n");
    return -1;
  }

  if (srslte_nbiot_ue_dl_set_cell(&ue_dl, cell)) {
    fprintf(stderr, "Configuring cell in UE DL\n");
    return -1;
  }

  mib.sched_info_sib1 = sched_info_tag;
  srslte_nbiot_ue_dl_set_mib(&ue_dl, mib);
  srslte_nbiot_ue_dl_set_rnti(&ue_dl, rnti);
  // srslte_mib_nb_printf(stdout, 0, &mib);

  DEBUG("Memory init OK\n");
  return 0;
}

void base_free()
{
  srslte_filesource_free(&fsrc);
  srslte_nbiot_ue_dl_free(&ue_dl);

  for (int i = 0; i < SRSLTE_MAX_PORTS; i++) {
    if (buff_ptrs[i] != NULL) {
      free(buff_ptrs[i]);
    }
  }
}

int main(int argc, char** argv)
{
  int                 dci_formatN0_detected = 0;
  int                 dci_formatN1_detected = 0;
  srslte_dci_format_t last_dci_format       = SRSLTE_DCI_FORMAT0;

  if (argc < 3) {
    usage(argv[0]);
    exit(-1);
  }
  parse_args(argc, argv);

  if (base_init()) {
    fprintf(stderr, "Error initializing memory\n");
    exit(-1);
  }

  // adjust SNR input to allow for negative values
  if (snr != -1.0) {
    snr -= 10.0;
    printf("Target SNR: %.2fdB\n", snr);
  }

  uint8_t* data = malloc(100000);

  int nread = 0;

  if (decode_sib1) {
    srslte_nbiot_ue_dl_decode_sib1(&ue_dl, sfn);
  }

  do {
    nread = srslte_filesource_read(&fsrc, buff_ptrs[0], flen);
    if (nread > 0) {
      DEBUG("%d.%d: Reading %d samples.\n", sfn, sf_idx, nread);

      // add some noise to the signal
      if (snr != -1.0) {
        float nstd = powf(10.0f, -snr / 20.0f);
        srslte_ch_awgn_c(buff_ptrs[0], buff_ptrs[0], nstd, nread);
      }

      // try to decode
      if (srslte_ra_nbiot_is_valid_dl_sf(sfn * 10 + sf_idx)) {
        int n;
        if (srslte_nbiot_ue_dl_has_grant(&ue_dl)) {
          // attempt to decode NPDSCH
          n = srslte_nbiot_ue_dl_decode_npdsch(&ue_dl, buff_ptrs[0], data, sfn, sf_idx, rnti);
          if (n == SRSLTE_SUCCESS) {
            INFO("NPDSCH decoded ok.\n");

            if (decode_sib1) {
              srslte_nbiot_ue_dl_decode_sib1(&ue_dl, sfn);
            }
          } else if (n == SRSLTE_ERROR && decode_sib1) {
            // reactivate SIB1 reception
            srslte_nbiot_ue_dl_decode_sib1(&ue_dl, sfn);
          }
        } else {
          // decode NPDCCH
          srslte_dci_msg_t dci_msg;
          n = srslte_nbiot_ue_dl_decode_npdcch(&ue_dl, buff_ptrs[0], sfn, sf_idx, rnti, &dci_msg);
          if (n == SRSLTE_NBIOT_UE_DL_FOUND_DCI) {
            INFO("Found %s DCI for RNTI=0x%x\n", srslte_dci_format_string(dci_msg.format), rnti);
            last_dci_format = dci_msg.format;

            if (dci_msg.format == SRSLTE_DCI_FORMATN0) {
              // try to convert to UL grant
              srslte_ra_nbiot_ul_dci_t   dci_unpacked;
              srslte_ra_nbiot_ul_grant_t grant;
              // Creates the UL NPUSCH resource allocation grant from a DCI format N0 message
              if (srslte_nbiot_dci_msg_to_ul_grant(
                      &dci_msg, &dci_unpacked, &grant, sfn * 10 + sf_idx, SRSLTE_NPUSCH_SC_SPACING_15000)) {
                fprintf(stderr, "Error unpacking DCI\n");
                return SRSLTE_ERROR;
              }
              dci_formatN0_detected++;

              // we don't use UL grants any further
            } else if (dci_msg.format == SRSLTE_DCI_FORMATN1) {
              // try to convert DCI to DL grant
              srslte_ra_nbiot_dl_dci_t   dci_unpacked;
              srslte_ra_nbiot_dl_grant_t grant;
              if (srslte_nbiot_dci_msg_to_dl_grant(
                      &dci_msg, rnti, &dci_unpacked, &grant, sfn, sf_idx, 64 /* fixme: remove */, cell.mode)) {
                fprintf(stderr, "Error unpacking DCI\n");
                return SRSLTE_ERROR;
              }

              dci_formatN1_detected++;

              // activate grant
              srslte_nbiot_ue_dl_set_grant(&ue_dl, &grant);
            } else {
              fprintf(stderr, "Unsupported DCI format.\n");
              return SRSLTE_ERROR;
            }
          }
        }
      }

      sf_idx++;
      if (sf_idx == 10) {
        sfn = (sfn + 1) % 1024;
        nof_frames++;
        sf_idx = 0;
      }
    }
  } while (nread > 0 && nof_frames <= max_frames);

  uint32_t pkts_ok = ue_dl.pkts_ok;
  printf("pkt_total=%d\n", ue_dl.pkts_total);
  printf("pkt_ok=%d\n", pkts_ok);
  printf("pkt_errors=%d\n", ue_dl.pkt_errors);
  printf("bler=%.2f\n", ue_dl.pkts_total ? (float)100 * ue_dl.pkt_errors / ue_dl.pkts_total : 0);
  printf("rate=%.2f\n", ((ue_dl.bits_total / ((nof_frames * 10 + sf_idx) / 1000.0)) / 1000.0));
  printf("dci_detected=%d\n", ue_dl.nof_detected);
  printf("dci_formatN0_detected=%d\n", dci_formatN0_detected);
  printf("dci_formatN1_detected=%d\n", dci_formatN1_detected);
  printf("num_frames=%d\n", nof_frames);

#if DUMP_SIGNAL
  printf("Saving signal...\n");
  srslte_nb_ue_dl_save_signal(&ue_dl, &ue_dl.softbuffer, sf_idx);
#endif

  base_free();
  if (data != NULL) {
    free(data);
  }

  // exit value depends on actual test

  // check if we are testing for correct DCI deconding
  if (dci_format == SRSLTE_DCI_FORMATN0 || dci_format == SRSLTE_DCI_FORMATN1 || dci_format == SRSLTE_DCI_FORMATN2) {
    // this is a DCI test, check if last DCI matches requested type
    if (last_dci_format == dci_format) {
      return SRSLTE_SUCCESS;
    } else {
      return SRSLTE_ERROR;
    }
  }

  // success if at least one NPDSCH could be decoded
  if (pkts_ok > 0)
    return SRSLTE_SUCCESS;

  // return error by default
  return SRSLTE_ERROR;
}
