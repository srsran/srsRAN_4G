/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The libLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the libLTE library.
 *
 * libLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */


#include <stdio.h>
#include <stdlib.h>

#include "liblte/phy/phy.h"

int main(int argc, char **argv) {
  binsource_t bs;
  char* output;

  binsource_init(&bs);
  binsource_seed_time(&bs);

  output = malloc(100);

  if (binsource_generate(&bs,output,100)) {
    printf("Error generating bits\n");
    exit(-1);
  }
  printf("output: ");
  bit_fprint(stdout,output,100);
  printf("Done\n");
  exit(0);
}
