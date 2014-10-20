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
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>

#include "liblte/rrc/rrc.h"
#include "rrc_asn.h"

int main(int argc, char **argv) {
  //uint8_t buffer[18] = {0x40, 0x48, 0x50, 0x3, 0x2, 0xb, 0x14, 0x4a, 0x30, 0x18, 0x28, 0x20, 0x90, 0x81, 0x84, 0x79, 0x0, 0x0};

//  bzero(buffer, 18);
  uint8_t buffer[200];
  bcch_dlsch_sib1_pack(buffer, 144);

  //bcch_dlsch_sib1_unpack(buffer, 144);
}

