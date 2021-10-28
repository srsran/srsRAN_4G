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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "srsran/srsran.h"

// Enable to measure execution time
#define NOF_CE_SYMBOLS SRSRAN_NOF_RE(cell)

static srsran_cell_t cell = {
    6,                  // nof_prb
    1,                  // nof_ports
    0,                  // cell_id
    SRSRAN_CP_NORM,     // cyclic prefix
    SRSRAN_PHICH_NORM,  // PHICH length
    SRSRAN_PHICH_R_1_6, // PHICH resources
    SRSRAN_FDD,

};

static srsran_tm_t tm                           = SRSRAN_TM1;
static uint32_t    cfi                          = 1;
static uint32_t    mcs[SRSRAN_MAX_CODEWORDS]    = {0, 0};
static uint32_t    subframe                     = 1;
static int         rv_idx[SRSRAN_MAX_CODEWORDS] = {0, 1};
static uint16_t    rnti                         = 1234;
static uint32_t    nof_rx_antennas              = 1;
static bool        tb_cw_swap                   = false;
static bool        enable_coworker              = false;
static uint32_t    pmi                          = 0;
static char*       input_file                   = NULL;
static int         M                            = 1;
static bool        enable_256qam                = false;
static bool        use_8_bit                    = false;

void usage(char* prog)
{
  printf("Usage: %s [fmMbcsrtRFpnwav] \n", prog);
  printf("\t-f read signal from file [Default generate it with pdsch_encode()]\n");
  printf("\t-m MCS [Default %d]\n", mcs[0]);
  printf("\t-M MCS2 [Default %d]\n", mcs[1]);
  printf("\t-c cell id [Default %d]\n", cell.id);
  printf("\t-b Use 8-bit LLR [Default 16-bit]\n");
  printf("\t-s subframe [Default %d]\n", subframe);
  printf("\t-r rv_idx [Default %d]\n", rv_idx[0]);
  printf("\t-t rv_idx2 [Default %d]\n", rv_idx[1]);
  printf("\t-R rnti [Default %d]\n", rnti);
  printf("\t-F cfi [Default %d]\n", cfi);
  printf("\t-X Number of repetitions for time measurement [Default %d]\n", M);
  printf("\t-x Transmission mode [1 to 4] [Default %d]\n", tm + 1);
  printf("\t-n cell.nof_prb [Default %d]\n", cell.nof_prb);
  printf("\t-a nof_rx_antennas [Default %d]\n", nof_rx_antennas);
  printf("\t-p pmi (multiplex only)  [Default %d]\n", pmi);
  printf("\t-w Swap Transport Blocks\n");
  printf("\t-j Enable PDSCH decoder coworker\n");
  printf("\t-v [set srsran_verbose to debug, default none]\n");
  printf("\t-q Enable/Disable 256QAM modulation (default %s)\n", enable_256qam ? "enabled" : "disabled");
}

void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "fmMcsbrtRFpnqawvXxj")) != -1) {
    switch (opt) {
      case 'f':
        input_file = argv[optind];
        break;
      case 'm':
        mcs[0] = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'b':
        use_8_bit = true;
        break;
      case 'M':
        mcs[1] = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 's':
        subframe = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'X':
        M = (int)strtol(argv[optind], NULL, 10);
        break;
      case 'r':
        rv_idx[0] = (int)strtol(argv[optind], NULL, 10);
        break;
      case 't':
        rv_idx[1] = (int)strtol(argv[optind], NULL, 10);
        break;
      case 'R':
        rnti = (uint16_t)strtol(argv[optind], NULL, 10);
        break;
      case 'F':
        cfi = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'x':
        tm = (srsran_tm_t)(strtol(argv[optind], NULL, 10) - 1);
        break;
      case 'p':
        pmi = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'n':
        cell.nof_prb = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'c':
        cell.id = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'a':
        nof_rx_antennas = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'w':
        tb_cw_swap = true;
        break;
      case 'j':
        enable_coworker = true;
        break;
      case 'v':
        increase_srsran_verbose_level();
        break;
      case 'q':
        enable_256qam ^= true;
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
}

static int check_softbits(srsran_pdsch_t*     pdsch_enb,
                          srsran_pdsch_t*     pdsch_ue,
                          srsran_pdsch_cfg_t* pdsch_cfg,
                          uint32_t            sf_idx,
                          int                 tb)
{
  int ret = SRSRAN_SUCCESS;

  if (!pdsch_ue->llr_is_8bit && !tb_cw_swap) {
    // Scramble
    srsran_sequence_pdsch_apply_s(pdsch_ue->e[tb],
                                  pdsch_ue->e[tb],
                                  rnti,
                                  pdsch_cfg->grant.tb[tb].cw_idx,
                                  2 * (sf_idx % 10),
                                  cell.id,
                                  pdsch_cfg->grant.tb[tb].nof_bits);

    int16_t* rx       = pdsch_ue->e[tb];
    uint8_t* rx_bytes = pdsch_ue->e[tb];
    for (int i = 0, k = 0; i < pdsch_cfg->grant.tb[tb].nof_bits / 8; i++) {
      uint8_t w = 0;
      for (int j = 0; j < 8; j++, k++) {
        w |= (rx[k] > 0) ? (1 << (7 - j)) : 0;
      }
      rx_bytes[i] = w;
    }
    if (memcmp(pdsch_ue->e[tb], pdsch_enb->e[tb], pdsch_cfg->grant.tb[tb].nof_bits / 8) != 0) {
      printf("tx=");
      srsran_vec_fprint_byte(stdout, pdsch_enb->e[tb], pdsch_cfg->grant.tb[tb].nof_bits / 8);
      printf("rx=");
      srsran_vec_fprint_byte(stdout, pdsch_ue->e[tb], pdsch_cfg->grant.tb[tb].nof_bits / 8);
      ret = SRSRAN_ERROR;
    }
  }
  return ret;
}

int main(int argc, char** argv)
{
  int                     ret  = -1;
  struct timeval          t[3] = {};
  srsran_softbuffer_tx_t* softbuffers_tx[SRSRAN_MAX_CODEWORDS];
  bool                    acks[SRSRAN_MAX_CODEWORDS] = {false};

  uint8_t*                data_tx[SRSRAN_MAX_CODEWORDS] = {NULL};
  uint8_t*                data_rx[SRSRAN_MAX_CODEWORDS] = {NULL};
  srsran_softbuffer_rx_t* softbuffers_rx[SRSRAN_MAX_CODEWORDS];
  srsran_pdsch_cfg_t      pdsch_cfg;
  srsran_dl_sf_cfg_t      dl_sf;
  cf_t*                   tx_slot_symbols[SRSRAN_MAX_PORTS];
  cf_t*                   rx_slot_symbols[SRSRAN_MAX_PORTS];
  srsran_pdsch_t          pdsch_tx, pdsch_rx;
  srsran_ofdm_t           ofdm_tx[SRSRAN_MAX_PORTS];
  srsran_ofdm_t           ofdm_rx[SRSRAN_MAX_PORTS];
  srsran_chest_dl_t       chest;
  srsran_chest_dl_res_t   chest_res;
  srsran_pdsch_res_t      pdsch_res[SRSRAN_MAX_CODEWORDS];
  srsran_random_t         random_gen = srsran_random_init(0x1234);
  srsran_crc_t            crc_tb;

  /* Initialise to zeros */
  ZERO_OBJECT(softbuffers_tx);
  ZERO_OBJECT(data_tx);
  ZERO_OBJECT(data_rx);
  ZERO_OBJECT(softbuffers_rx);
  ZERO_OBJECT(pdsch_cfg);
  ZERO_OBJECT(dl_sf);
  ZERO_OBJECT(tx_slot_symbols);
  ZERO_OBJECT(rx_slot_symbols);
  ZERO_OBJECT(pdsch_tx);
  ZERO_OBJECT(pdsch_rx);
  ZERO_OBJECT(ofdm_tx);
  ZERO_OBJECT(ofdm_rx);
  ZERO_OBJECT(chest);
  ZERO_OBJECT(chest_res);
  ZERO_OBJECT(pdsch_res);
  ZERO_OBJECT(crc_tb);

  parse_args(argc, argv);

  if (tm == SRSRAN_TM1) {
    cell.nof_ports = 1;
    mcs[1]         = 0;
    rv_idx[1]      = 1;
  } else {
    cell.nof_ports = 2;
  }

  srsran_dci_dl_t dci;
  ZERO_OBJECT(dci);

  switch (tm) {
    case SRSRAN_TM1:
    case SRSRAN_TM2:
      dci.format = SRSRAN_DCI_FORMAT1A;
      break;
    case SRSRAN_TM3:
      dci.format = SRSRAN_DCI_FORMAT2A;
      break;
    case SRSRAN_TM4:
      dci.format = SRSRAN_DCI_FORMAT2;
      break;
    default:
      fprintf(stderr, "Error unsupported tm=%d\n", tm);
      exit(-1);
  }
  dci.rnti                    = rnti;
  dci.type0_alloc.rbg_bitmask = 0xffffffff;

  /* If transport block 0 is enabled */
  uint32_t nof_tb = 0;
  for (int i = 0; i < SRSRAN_MAX_CODEWORDS; i++) {
    dci.tb[i].mcs_idx = mcs[i];
    dci.tb[i].rv      = rv_idx[i];
    if (SRSRAN_DCI_IS_TB_EN(dci.tb[i])) {
      nof_tb++;
    }
  }

  for (int i = 0; i < SRSRAN_MAX_CODEWORDS; i++) {
    dci.tb[i].cw_idx = (((tb_cw_swap) ? 1 : 0) + i) % nof_tb;
  }

  ZERO_OBJECT(dl_sf);
  dl_sf.tti = subframe;
  dl_sf.cfi = cfi;

  // Enable power allocation
  pdsch_cfg.power_scale = true;
  pdsch_cfg.p_a         = 0.0f;                      // 0 dB
  pdsch_cfg.p_b         = (tm > SRSRAN_TM1) ? 1 : 0; // 0 dB

  /* Generate dci from DCI */
  if (srsran_ra_dl_dci_to_grant(&cell, &dl_sf, tm, enable_256qam, &dci, &pdsch_cfg.grant)) {
    ERROR("Error computing resource allocation");
    return ret;
  }

  srsran_chest_dl_res_init(&chest_res, cell.nof_prb);
  srsran_chest_dl_res_set_identity(&chest_res);

  /* init memory */
  for (uint32_t i = 0; i < SRSRAN_MAX_PORTS; i++) {
    rx_slot_symbols[i] = srsran_vec_cf_malloc(SRSRAN_NOF_RE(cell));
    if (!rx_slot_symbols[i]) {
      perror("srsran_vec_malloc");
      goto quit;
    }
  }

  for (uint32_t i = 0; i < cell.nof_ports; i++) {
    tx_slot_symbols[i] = calloc(SRSRAN_NOF_RE(cell), sizeof(cf_t));
    if (!tx_slot_symbols[i]) {
      perror("srsran_vec_malloc");
      goto quit;
    }
  }

  for (uint32_t i = 0; i < SRSRAN_MAX_TB; i++) {
    if (pdsch_cfg.grant.tb[i].enabled) {
      data_tx[i] = srsran_vec_u8_malloc(pdsch_cfg.grant.tb[i].tbs);
      if (!data_tx[i]) {
        perror("srsran_vec_malloc");
        goto quit;
      }
      bzero(data_tx[i], sizeof(uint8_t) * pdsch_cfg.grant.tb[i].tbs);

      data_rx[i] = srsran_vec_u8_malloc(pdsch_cfg.grant.tb[i].tbs);
      if (!data_rx[i]) {
        perror("srsran_vec_malloc");
        goto quit;
      }
      bzero(data_rx[i], sizeof(uint8_t) * pdsch_cfg.grant.tb[i].tbs);

    } else {
      data_tx[i] = NULL;
      data_rx[i] = NULL;
    }
  }

  if (srsran_pdsch_init_ue(&pdsch_rx, cell.nof_prb, nof_rx_antennas)) {
    ERROR("Error creating PDSCH object");
    goto quit;
  }
  if (srsran_pdsch_set_cell(&pdsch_rx, cell)) {
    ERROR("Error creating PDSCH object");
    goto quit;
  }

  pdsch_rx.llr_is_8bit        = use_8_bit;
  pdsch_rx.dl_sch.llr_is_8bit = use_8_bit;

  for (uint32_t i = 0; i < SRSRAN_MAX_CODEWORDS; i++) {
    softbuffers_rx[i] = calloc(sizeof(srsran_softbuffer_rx_t), 1);
    if (!softbuffers_rx[i]) {
      ERROR("Error allocating RX soft buffer");
      goto quit;
    }

    if (srsran_softbuffer_rx_init(softbuffers_rx[i], cell.nof_prb)) {
      ERROR("Error initiating RX soft buffer");
      goto quit;
    }

    srsran_softbuffer_rx_reset(softbuffers_rx[i]);
  }

  if (input_file) {
    srsran_filesource_t fsrc;
    if (srsran_filesource_init(&fsrc, input_file, SRSRAN_COMPLEX_FLOAT_BIN)) {
      ERROR("Error opening file %s", input_file);
      goto quit;
    }

    if (srsran_chest_dl_init(&chest, cell.nof_prb, 1)) {
      ERROR("Error initializing equalizer");
      exit(-1);
    }
    if (srsran_chest_dl_set_cell(&chest, cell)) {
      printf("Error initializing equalizer\n");
      exit(-1);
    }

    srsran_chest_dl_estimate(&chest, &dl_sf, rx_slot_symbols, &chest_res);
    srsran_chest_dl_free(&chest);

    srsran_filesource_free(&fsrc);
  } else {
    if (srsran_pdsch_init_enb(&pdsch_tx, cell.nof_prb)) {
      ERROR("Error creating PDSCH object");
      goto quit;
    }
    if (srsran_pdsch_set_cell(&pdsch_tx, cell)) {
      ERROR("Error creating PDSCH object");
      goto quit;
    }

    for (uint32_t i = 0; i < SRSRAN_MAX_CODEWORDS; i++) {
      softbuffers_tx[i] = calloc(sizeof(srsran_softbuffer_tx_t), 1);
      if (!softbuffers_tx[i]) {
        ERROR("Error allocating TX soft buffer");
      }

      if (srsran_softbuffer_tx_init(softbuffers_tx[i], cell.nof_prb)) {
        ERROR("Error initiating TX soft buffer");
        goto quit;
      }
    }

    if (srsran_crc_init(&crc_tb, SRSRAN_LTE_CRC24A, 24) < SRSRAN_SUCCESS) {
      ERROR("Error initiating CRC24A");
      goto quit;
    }

    // Generate random data
    for (int tb = 0; tb < SRSRAN_MAX_CODEWORDS; tb++) {
      if (pdsch_cfg.grant.tb[tb].enabled) {
        for (int byte = 0; byte < pdsch_cfg.grant.tb[tb].tbs / 8; byte++) {
          data_tx[tb][byte] = (uint8_t)srsran_random_uniform_int_dist(random_gen, 0, 255);
        }
        // Attach CRC for making sure TB with 0 CRC are detected
        srsran_crc_attach_byte(&crc_tb, data_tx[tb], pdsch_cfg.grant.tb[tb].tbs - 24);
      }
    }

    /*uint8_t databit[100000];
    srsran_bit_unpack_vector(data, databit, dci.mcs.tbs);
    srsran_vec_save_file("data_in", databit, dci.mcs.tbs);*/

    pdsch_cfg.rnti              = rnti;
    pdsch_cfg.softbuffers.tx[0] = softbuffers_tx[0];
    pdsch_cfg.softbuffers.tx[1] = softbuffers_tx[1];
    if (rv_idx[0] != 0) {
      for (int i = 0; i < SRSRAN_MAX_CODEWORDS; i++) {
        pdsch_cfg.grant.tb[i].rv = 0;
      }
      /* Do 1st transmission for rv_idx!=0 */
      if (srsran_pdsch_encode(&pdsch_tx, &dl_sf, &pdsch_cfg, data_tx, tx_slot_symbols)) {
        ERROR("Error encoding PDSCH");
        goto quit;
      }
    }
    gettimeofday(&t[1], NULL);
    for (int i = 0; i < SRSRAN_MAX_CODEWORDS; i++) {
      pdsch_cfg.grant.tb[i].rv = rv_idx[i];
    }
    for (uint32_t k = 0; k < M; k++) {
      if (srsran_pdsch_encode(&pdsch_tx, &dl_sf, &pdsch_cfg, data_tx, tx_slot_symbols)) {
        ERROR("Error encoding PDSCH");
        goto quit;
      }
    }
    gettimeofday(&t[2], NULL);
    get_time_interval(t);
    printf("ENCODED in %.2f (PHY bitrate=%.2f Mbps. Processing bitrate=%.2f Mbps)\n",
           (float)t[0].tv_usec / M,
           (float)(pdsch_cfg.grant.tb[0].tbs + pdsch_cfg.grant.tb[1].tbs) / 1000.0f,
           (float)(pdsch_cfg.grant.tb[0].tbs + pdsch_cfg.grant.tb[1].tbs) * M / t[0].tv_usec);

    /* combine outputs */
    for (uint32_t j = 0; j < nof_rx_antennas; j++) {
      for (uint32_t k = 0; k < SRSRAN_NOF_RE(cell); k++) {
        rx_slot_symbols[j][k] = 0.0f;
        for (uint32_t i = 0; i < cell.nof_ports; i++) {
          rx_slot_symbols[j][k] += tx_slot_symbols[i][k] * chest_res.ce[i][j][k];
        }
      }
    }
  }

  INFO(" Global:");
  INFO("         nof_prb=%d", cell.nof_prb);
  INFO("       nof_ports=%d", cell.nof_ports);
  INFO("              id=%d", cell.id);
  INFO("              cp=%s", srsran_cp_string(cell.cp));
  INFO("    phich_length=%d", (int)cell.phich_length);
  INFO(" phich_resources=%d", (int)cell.phich_resources);
  INFO("         nof_prb=%d", pdsch_cfg.grant.nof_prb);
  INFO("          sf_idx=%d", dl_sf.tti);
  INFO("          nof_tb=%d", pdsch_cfg.grant.nof_tb);
  for (uint32_t i = 0; i < SRSRAN_MAX_CODEWORDS; i++) {
    INFO(" Tranport block index %d:", i);
    INFO("         enabled=%d", pdsch_cfg.grant.tb[i].enabled);
    INFO("         mcs.idx=%d", pdsch_cfg.grant.tb[i].mcs_idx);
    INFO("         mcs.tbs=%d", pdsch_cfg.grant.tb[i].tbs);
    INFO("         mcs.mod=%s", srsran_mod_string(pdsch_cfg.grant.tb[i].mod));
    INFO("              rv=%d", pdsch_cfg.grant.tb[i].rv);
    INFO("        nof_bits=%d", pdsch_cfg.grant.tb[i].nof_bits);
    INFO("          nof_re=%d", pdsch_cfg.grant.nof_re);
  }

  int r = 0;
  if (enable_coworker) {
    srsran_pdsch_enable_coworker(&pdsch_rx);
  }

  for (uint32_t i = 0; i < SRSRAN_MAX_CODEWORDS; i++) {
    pdsch_cfg.softbuffers.rx[i] = softbuffers_rx[i];
    pdsch_res[i].payload        = data_rx[i];
  }

  gettimeofday(&t[1], NULL);
  for (uint32_t k = 0; k < M; k++) {
    for (uint32_t i = 0; i < SRSRAN_MAX_CODEWORDS; i++) {
      if (pdsch_cfg.grant.tb[i].enabled) {
        srsran_softbuffer_rx_reset_tbs(softbuffers_rx[i], (uint32_t)pdsch_cfg.grant.tb[i].tbs);
      }
    }

    /* Set ACKs to zero, otherwise will not decode if there are positive ACKs*/
    bzero(acks, sizeof(acks));

    r = srsran_pdsch_decode(&pdsch_rx, &dl_sf, &pdsch_cfg, &chest_res, rx_slot_symbols, pdsch_res);
  }
  gettimeofday(&t[2], NULL);
  get_time_interval(t);
  printf("DECODED %s in %.2f (PHY bitrate=%.2f Mbps. Processing bitrate=%.2f Mbps)\n",
         r ? "Error" : "OK",
         (float)t[0].tv_usec / M,
         (float)(pdsch_cfg.grant.tb[0].tbs + pdsch_cfg.grant.tb[1].tbs) / 1000.0f,
         (float)(pdsch_cfg.grant.tb[0].tbs + pdsch_cfg.grant.tb[1].tbs) * M / t[0].tv_usec);

  /* If there is an error in PDSCH decode */
  if (r) {
    ret = -1;
    goto quit;
  }

  /* Check Tx and Rx bytes */
  for (int tb = 0; tb < SRSRAN_MAX_CODEWORDS; tb++) {
    if (pdsch_cfg.grant.tb[tb].enabled) {
      if (check_softbits(&pdsch_tx, &pdsch_rx, &pdsch_cfg, subframe, tb)) {
        ERROR("TB%d: The received softbits in subframe %d DO NOT match the encoded bits (crc=%d)\n",
              tb,
              subframe,
              pdsch_res[tb].crc);
        ret = SRSRAN_ERROR;
        goto quit;
      } else {
        for (int byte = 0; byte < pdsch_cfg.grant.tb[tb].tbs / 8; byte++) {
          if (data_tx[tb][byte] != data_rx[tb][byte]) {
            ERROR("Found BYTE (%d) error in TB %d (%02X != %02X), quitting...",
                  byte,
                  tb,
                  data_tx[tb][byte],
                  data_rx[tb][byte]);
            ret = SRSRAN_ERROR;
            goto quit;
          }
        }
      }
    }
  }

  /* Check all transport blocks have been decoded OK */
  for (int tb = 0; tb < SRSRAN_MAX_CODEWORDS; tb++) {
    if (pdsch_cfg.grant.tb[tb].enabled) {
      ret |= (acks[tb]) ? SRSRAN_SUCCESS : SRSRAN_ERROR;
    }
  }

  ret = SRSRAN_SUCCESS;

quit:
  for (uint32_t i = 0; i < cell.nof_ports; i++) {
    srsran_ofdm_tx_free(&ofdm_tx[i]);
  }
  for (uint32_t i = 0; i < nof_rx_antennas; i++) {
    srsran_ofdm_rx_free(&ofdm_rx[i]);
  }
  srsran_chest_dl_res_free(&chest_res);
  srsran_chest_dl_free(&chest);
  srsran_pdsch_free(&pdsch_tx);
  srsran_pdsch_free(&pdsch_rx);
  for (uint32_t i = 0; i < SRSRAN_MAX_CODEWORDS; i++) {
    srsran_softbuffer_tx_free(softbuffers_tx[i]);
    if (softbuffers_tx[i]) {
      free(softbuffers_tx[i]);
    }

    srsran_softbuffer_rx_free(softbuffers_rx[i]);
    if (softbuffers_rx[i]) {
      free(softbuffers_rx[i]);
    }

    if (data_tx[i]) {
      free(data_tx[i]);
    }

    if (data_rx[i]) {
      free(data_rx[i]);
    }
  }

  for (uint32_t i = 0; i < SRSRAN_MAX_PORTS; i++) {
    if (tx_slot_symbols[i]) {
      free(tx_slot_symbols[i]);
    }
    if (rx_slot_symbols[i]) {
      free(rx_slot_symbols[i]);
    }
  }
  srsran_random_free(random_gen);
  if (ret) {
    printf("Error\n");
  } else {
    printf("Ok\n");
  }
  exit(ret);
}
