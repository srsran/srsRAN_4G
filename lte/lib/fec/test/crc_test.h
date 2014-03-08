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

#include <stdbool.h>

typedef struct {
	int n;
	int l;
	unsigned int p;
	unsigned int s;
	unsigned int word;
}expected_word_t;


static expected_word_t expected_words[] = {
		{5000, 24, 0x1864CFB, 1, 0x4D0836},		// LTE CRC24A (36.212 Sec 5.1.1)
		{5000, 24, 0X1800063, 1, 0x9B68F8},		// LTE CRC24B
		{5000, 16, 0x11021, 1, 0xBFFA},  		// LTE CRC16
		{5000, 8, 0x19B, 1, 0xF8},				// LTE CRC8

		{-1, -1, 0, 0, 0}
};

int get_expected_word(int n, int l, unsigned int p, unsigned int s, unsigned int *word) {
	int i;
	i=0;
	while(expected_words[i].n != -1) {
		if (expected_words[i].l == l
			&& expected_words[i].p == p
			&& expected_words[i].s == s) {
			break;
		} else {
			i++;
		}
	}
	if (expected_words[i].n == -1) {
		return -1;
	} else {
		if (word) {
			*word = expected_words[i].word;
		}
		return 0;
	}
}
