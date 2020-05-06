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

#include <math.h>
#include <stdlib.h>

float rand_gauss(void)
{
  float v1, v2, s;

  do {
    v1 = 2.0 * (rand() / (float)RAND_MAX) - 1;
    v2 = 2.0 * (rand() / (float)RAND_MAX) - 1;

    s = v1 * v1 + v2 * v2;
  } while (s >= 1.0 || s == 0.0);

  return (v1 * sqrtf(-2.0 * log(s) / s));
}
