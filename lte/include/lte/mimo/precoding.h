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


#ifndef PRECODING_H_
#define PRECODING_H_

typedef _Complex float cf_t;

/** The precoder takes as input nlayers vectors "x" from the
 * layer mapping and generates nports vectors "y" to be mapped onto
 * resources on each of the antenna ports.
 */

/* Estimates the vector "x" based on the received signal "y" and the channel estimates "ce"
 */
void precoding_decode(cf_t *y[MAX_PORTS], cf_t *ce[MAX_PORTS],
		cf_t *x[MAX_LAYERS], int nof_ports, int nof_symbols,
		mimo_type_t type);

/* Generates the vector "y" from the input vector "x"
 */
void precoding_encode(cf_t *x[MAX_LAYERS], cf_t *y[MAX_PORTS], int nof_ports,
		int nof_symbols, mimo_type_t type);

#endif /* PRECODING_H_ */
