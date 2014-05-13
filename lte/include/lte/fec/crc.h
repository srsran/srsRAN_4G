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



#ifndef CRC_
#define CRC_

#define LTE_CRC24A	0x1864CFB
#define LTE_CRC24B	0X1800063
#define LTE_CRC16	0x11021
#define LTE_CRC8	0x19B


#define _WITHMALLOC

#ifndef _WITHMALLOC
#define MAX_LENGTH	1024*16
#endif

typedef struct {
	unsigned long table[256];
#ifdef _WITHMALLOC
	unsigned char *data0;
#else
	unsigned char data0[MAX_LENGTH];
#endif
	int polynom;
	int order;
	unsigned long crcinit; 
	unsigned long crcxor; 
	unsigned long crcmask;
	unsigned long crchighbit;
	unsigned int crc_out;
} crc_t;


int crc_init(crc_t *crc_par);
unsigned int crc_attach(char *bufptr, int len, crc_t *crc_params);

#endif
