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

/*!
 * \file polar_sets.c
 * \brief Definition of the auxiliary function that reads polar index sets from a file.
 * \author Jesus Gomez
 * \date 2020
 *
 * \copyright Software Radio Systems Limited
 *
 * The message and parity check sets provided by this functions are needed by
 * the subchannel allocation block.
 * The frozen bit set provided by this function is used by the polar decoder.
 *
 */

#include "polar_sets.h"

#include <inttypes.h>
#include <srsran/phy/utils/vector.h>
#include <stdio.h>
#include <stdlib.h> //exit
#include <string.h>

void srsran_polar_code_sets_free(srsran_polar_sets_t* c)
{
  if (c != NULL) {
    free(c->frozen_set);
    free(c->info_set);
    free(c->message_set);
    free(c->parity_set);
  }
}

int srsran_polar_code_sets_read(srsran_polar_sets_t* c,
                                const uint16_t       message_size,
                                const uint8_t        code_size_log,
                                const uint16_t       rate_matching_size,
                                const uint8_t        parity_set_size,
                                const uint8_t        nWmPC)
{
  FILE* fptr = NULL;
  char  filename[50];

  uint16_t code_size = 1U << code_size_log;

  c->frozen_set_size  = code_size - message_size - parity_set_size;
  c->parity_set_size  = parity_set_size;
  c->info_set_size    = message_size + parity_set_size;
  c->message_set_size = message_size;

  c->frozen_set = srsran_vec_u16_malloc(c->frozen_set_size);
  if (!c->frozen_set) {
    perror("malloc");
    exit(-1);
  }

  c->info_set = srsran_vec_u16_malloc(c->info_set_size);
  if (!c->info_set) {
    perror("malloc");
    exit(-1);
  }

  c->message_set = srsran_vec_u16_malloc(c->message_set_size);
  if (!c->message_set) {
    perror("malloc");
    exit(-1);
  }

  c->parity_set = srsran_vec_u16_malloc(parity_set_size);
  if (!c->parity_set) {
    perror("malloc");
    exit(-1);
  }

  sprintf(filename,
          "frozensets/polar_code_sets_%hu_%hu_%hu_%hu_%u.bin",
          code_size,
          rate_matching_size,
          c->message_set_size,
          c->parity_set_size,
          nWmPC);

  fptr = fopen(filename, "rbe");

  if (fptr == NULL) {
    printf("Error! file: %s does not exit. Probably, the polar set file is missing in folder "
           "/frozensets for the provided code parameters.\n",
           filename);
    exit(1);
  }

  fread(c->info_set, sizeof(uint16_t), c->info_set_size, fptr);
  fread(c->message_set, sizeof(uint16_t), c->message_set_size, fptr);
  fread(c->parity_set, sizeof(uint16_t), c->parity_set_size, fptr);
  fread(c->frozen_set, sizeof(uint16_t), c->frozen_set_size, fptr);

  fclose(fptr);
  return 0;
}
