/*
 * Copyright (c) 2012, Ismael Gomez-Miguelez <ismael.gomez@tsc.upc.edu>.
 * This file is part of ALOE++ (http://flexnets.upc.edu/)
 *
 * ALOE++ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ALOE++ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with ALOE++.  If not, see <http://www.gnu.org/licenses/>.
 */


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


