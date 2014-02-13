/*
 * Copyright (c) 2013, Ismael Gomez-Miguelez <gomezi@tcd.ie>.
 * This file is part of OSLD-lib (http://https://github.com/ismagom/osld-lib)
 *
 * OSLD-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OSLD-lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with OSLD-lib.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef RM_CONV_
#define RM_CONV_

#define RX_NULL 10000

int rm_conv_rx(float *input, float *output, int in_len, int out_len);


/* High-level API */
typedef struct {
	struct rm_conv_init {
		int direction;
	} init;
	void *input;			// input type may be char or float depending on hard
	int in_len;
	struct rm_conv_ctrl_in {
		int E;
		int S;
	} ctrl_in;
	void *output;
	int out_len;
}rm_conv_hl;

int rm_conv_initialize(rm_conv_hl* h);
int rm_conv_work(rm_conv_hl* hl);
int rm_conv_stop(rm_conv_hl* hl);

#endif
