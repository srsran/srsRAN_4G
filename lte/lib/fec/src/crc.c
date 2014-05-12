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
#include "lte/utils/pack.h"

#define _WITHMALLOC

unsigned int cword;
char *cdata;
const unsigned long crcinit = 0x00000000; 	//initial CRC value
const unsigned long crcxor = 0x00000000;	//final XOR value
unsigned long crcmask;
unsigned long crchighbit;
unsigned long crctab[256]; 




void gen_crc_table(int lorder, unsigned long poly) {

	int i, j, ord=(lorder-8);
	unsigned long bit, crc;

	for (i=0; i<256; i++) {
		crc = ((unsigned long)i)<<ord;
		for (j=0; j<8; j++) {
			bit = crc & crchighbit;
			crc<<= 1;
			if (bit) crc^= poly;
		}			
		crctab[i] = crc & crcmask;
	}
}


unsigned long crctable (unsigned char* data, unsigned long length, int lorder) {

	// Polynom lorders of 8, 16, 24 or 32.
	unsigned long crc = crcinit;

	while (length--) crc = (crc << 8) ^ crctab[ ((crc >> (lorder-8)) & 0xff) ^ *data++];
	
	return((crc^crcxor)&crcmask);
}

unsigned long reversecrcbit(unsigned int crc, unsigned int polynom, int lorder, int nbits) {

	unsigned long m, rmask=0x1;

	for(m=0; m<nbits; m++){	
		if((rmask & crc) == 0x01 )crc = (crc ^ polynom)>>1;
		else crc = crc >> 1;
	}
	return((crc^crcxor)&crcmask);
}


int init_crc(int lorder, unsigned long polynom){

	unsigned long polyhighbit;

	// Compute bit masks for whole CRC and CRC high bit
	crcmask = ((((unsigned long)1<<(lorder-1))-1)<<1)|1;
	polyhighbit=0xFFFFFFFF ^ (crcmask+1);
	crchighbit = (unsigned long)1<<(lorder-1);

	// Eliminate highest bit in polynom word
	polynom=polynom & polyhighbit;  

	// check parameters
	if (lorder < 1 || lorder > 32) {
		printf("ERROR, invalid order, it must be between 1..32.\n");
		return(0);
	}
	if (lorder%8 != 0) {
		printf("ERROR, invalid order=%d, it must be 8, 16, 24 or 32.\n", lorder);
		return(0);
	}
	if (polynom != (polynom & crcmask)) {
		printf("ERROR, invalid polynom.\n");
		return(0);
	}
	if (crcinit != (crcinit & crcmask)) {
		printf("ERROR, invalid crcinit.\n");
		return(0);
	}
	if (crcxor != (crcxor & crcmask)) {
		printf("ERROR, invalid crcxor.\n");
		return(0);
	}
	// generate lookup table
	gen_crc_table(lorder, polynom);

	return(1);
}

#define MAX_LENGTH	8192

unsigned int crc(unsigned int crc, char *bufptr, int len,
		int long_crc, unsigned int poly, int paste_word) {

	int i, len8, res8, a;
#ifdef _WITHMALLOC
	char *data0, *pter;

	data0 = (char *)malloc(sizeof(char) * (len+long_crc)*2);
	if (!data0) {
		perror("malloc ERROR: Allocating memory for data pointer in crc() function");
		return(-1);
	}
#endif
#ifndef _WITHMALLOC
	char data0[MAX_LENGTH], *pter;	

	if((((len+long_crc)>>3) + 1) > MAX_LENGTH){
		printf("ERROR: Not enough memory allocated\n");
		return(-1);
	}
#endif
	//# Pack bits into bytes 
	len8=(len>>3);
	res8=8-(len - (len8<<3));
	if(res8>0)a=1;
	else a=0;

	// Zeroed additional bits
	memset((char *)(bufptr+len),0,(32)*sizeof(char));

	for(i=0; i<len8+a; i++){
		pter=(char *)(bufptr+8*i);
		data0[i]=(char)(unpack_bits(&pter, 8)&0xFF);
	}
	// Calculate CRC
	pter=data0;
	crc=crctable ((unsigned char *)pter, len8+a, long_crc);

	// Reverse CRC res8 positions
	if(a==1)crc=reversecrcbit(crc, poly, long_crc, res8);

	// Add CRC
	pter=(char *)(bufptr+len);
	pack_bits(crc, &pter, long_crc);

#ifdef _WITHMALLOC
	free(data0);
	data0=NULL;
#endif
	//Return CRC value
	return crc;
}

