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

#ifdef CHECK_FUNCTION_EXISTS

char CHECK_FUNCTION_EXISTS();
#ifdef __CLASSIC_C__
int main()
{
  int   ac;
  char* av[];
#else
int main(int ac, char* av[])
{

#endif
  float ac2 = sqrtf(rand());
  CHECK_FUNCTION_EXISTS();
  if (ac2 * ac > 1000) {
    return *av[0];
  }
  return 0;
}

#else /* CHECK_FUNCTION_EXISTS */

#error "CHECK_FUNCTION_EXISTS has to specify the function"

#endif /* CHECK_FUNCTION_EXISTS */
