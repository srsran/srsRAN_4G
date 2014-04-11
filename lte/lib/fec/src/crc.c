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

unsigned int cword;

unsigned int icrc1(unsigned int crc, unsigned short onech,int long_crc,
		int left_shift,unsigned int poly)
{
  int i;
  unsigned int tmp=(unsigned int) (crc ^ (onech << (long_crc >> 1) ));

  for (i=0;i<left_shift;i++) {
    if (tmp & (0x1<<(long_crc-1)))
      tmp=(tmp<<1)^poly;
    else
      tmp <<= 1;
  }

  return tmp;
}

unsigned int crc(unsigned int crc, char *bufptr, int len,
		int long_crc,unsigned int poly, int paste_word) {

  int i,k;
  unsigned int data;
  int stop;
  unsigned int ret;

  cword=crc;

  k=0;
  stop=0;
  while(!stop) {
    data=0;
    for (i=0;i<long_crc/2;i++) {
      if (bufptr[k] && k<len)
        data|=(0x1<<(long_crc/2-1-i));
      k++;
      if (k==len) {
        stop=1;
        i++;
        break;
      }
    }

    cword=(unsigned int) (icrc1((unsigned int) (cword<<long_crc>>long_crc),
    		data,long_crc,i,poly)<<long_crc)>>long_crc;
  }

  ret=cword;
  if (paste_word) {
    cword<<=32-long_crc;
    for (i=0;i<long_crc;i++) {
       bufptr[i+len]=((cword&(0x1<<31))>>31);
       cword<<=1;
    }
  }
  return (ret);
}


