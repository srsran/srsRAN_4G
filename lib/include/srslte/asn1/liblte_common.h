/*******************************************************************************

    Copyright 2012-2014 Ben Wojtowicz

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*******************************************************************************

    File: liblte_common.h

    Description: Contains all the common definitions for the LTE library.

    Revision History
    ----------    -------------    --------------------------------------------
    02/26/2012    Ben Wojtowicz    Created file.
    07/21/2013    Ben Wojtowicz    Added a common message structure.
    06/15/2014    Ben Wojtowicz    Split LIBLTE_MSG_STRUCT into bit and byte
                                   aligned messages.
    08/03/2014    Ben Wojtowicz    Commonized value_2_bits and bits_2_value.
    11/29/2014    Ben Wojtowicz    Added liblte prefix to value_2_bits and
                                   bits_2_value.

*******************************************************************************/

#ifndef SRSLTE_LIBLTE_COMMON_H
#define SRSLTE_LIBLTE_COMMON_H

/*******************************************************************************
                              INCLUDES
*******************************************************************************/

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/*******************************************************************************
                              DEFINES
*******************************************************************************/

// FIXME: This was chosen arbitrarily
#define LIBLTE_ASN1_OID_MAXSUBIDS 128
#define LIBLTE_MAX_MSG_SIZE_BITS  102048
#define LIBLTE_MAX_MSG_SIZE_BYTES 12756
#define LIBLTE_MSG_HEADER_OFFSET  1024

/*******************************************************************************
                              TYPEDEFS
*******************************************************************************/

typedef int8_t    int8;
typedef uint8_t   uint8;
typedef int16_t   int16;
typedef uint16_t  uint16;
typedef int32_t   int32;
typedef uint32_t  uint32;
typedef int64_t   int64;
typedef uint64_t  uint64;

typedef enum{
  LIBLTE_SUCCESS = 0,
  LIBLTE_ERROR_INVALID_INPUTS,
  LIBLTE_ERROR_ENCODE_FAIL,
  LIBLTE_ERROR_DECODE_FAIL,
  LIBLTE_ERROR_INVALID_CRC,
  LIBLTE_ERROR_N_ITEMS
}LIBLTE_ERROR_ENUM;
static const char liblte_error_text[LIBLTE_ERROR_N_ITEMS][64] = {
  "Invalid inputs",
  "Encode failure",
  "Decode failure",
};

typedef void* LIBLTE_ASN1_OPEN_TYPE_STRUCT;

typedef struct {
   uint32_t numids;                             // number of subidentifiers
   uint32_t subid[LIBLTE_ASN1_OID_MAXSUBIDS];   // subidentifier values
} LIBLTE_ASN1_OID_STRUCT;

typedef struct{
    bool   data_valid;
    bool   data;
}LIBLTE_BOOL_MSG_STRUCT;

typedef struct{
    uint32 N_bits;
    uint8  msg[LIBLTE_MAX_MSG_SIZE_BITS];
}LIBLTE_SIMPLE_BIT_MSG_STRUCT;

typedef struct{
    uint32 N_bytes;
    uint8  msg[LIBLTE_MAX_MSG_SIZE_BYTES];
}LIBLTE_SIMPLE_BYTE_MSG_STRUCT;


struct LIBLTE_BYTE_MSG_STRUCT{
    uint32  N_bytes;
    uint8   buffer[LIBLTE_MAX_MSG_SIZE_BYTES];
    uint8  *msg;

    LIBLTE_BYTE_MSG_STRUCT():N_bytes(0)
    {
      msg = &buffer[LIBLTE_MSG_HEADER_OFFSET];
    }
    LIBLTE_BYTE_MSG_STRUCT(const LIBLTE_BYTE_MSG_STRUCT& buf)
    {
      N_bytes = buf.N_bytes;
      memcpy(msg, buf.msg, N_bytes);
    }
    LIBLTE_BYTE_MSG_STRUCT & operator= (const LIBLTE_BYTE_MSG_STRUCT & buf)
    {
      // avoid self assignment
      if (&buf == this)
        return *this;
      N_bytes = buf.N_bytes;
      memcpy(msg, buf.msg, N_bytes);
      return *this;
    }
    uint32 get_headroom()
    {
      return msg-buffer;
    }
    void reset()
    {
      N_bytes = 0;
      msg = &buffer[LIBLTE_MSG_HEADER_OFFSET];
    }
};

struct LIBLTE_BIT_MSG_STRUCT{
    uint32  N_bits;
    uint8   buffer[LIBLTE_MAX_MSG_SIZE_BITS];
    uint8  *msg;

    LIBLTE_BIT_MSG_STRUCT():N_bits(0)
    {
      msg = &buffer[LIBLTE_MSG_HEADER_OFFSET];
      while( (uint64_t)(msg) % 8 > 0) {
        msg++;
      }
    }
    LIBLTE_BIT_MSG_STRUCT(const LIBLTE_BIT_MSG_STRUCT& buf){
      N_bits = buf.N_bits;
      memcpy(msg, buf.msg, N_bits);
    }
    LIBLTE_BIT_MSG_STRUCT & operator= (const LIBLTE_BIT_MSG_STRUCT & buf){
      // avoid self assignment
      if (&buf == this)
        return *this;
      N_bits = buf.N_bits;
      memcpy(msg, buf.msg, N_bits);
      return *this;
    }
    uint32 get_headroom()
    {
      return msg-buffer;
    }
    void reset()
    {
      N_bits = 0;
      msg = &buffer[LIBLTE_MSG_HEADER_OFFSET];
      while( (uint64_t)(msg) % 8 > 0) {
        msg++;
      }
    }
};


/*******************************************************************************
                              DECLARATIONS
*******************************************************************************/

/*********************************************************************
    Name: liblte_value_2_bits

    Description: Converts a value to a bit string
*********************************************************************/
void liblte_value_2_bits(uint32   value,
                         uint8  **bits,
                         uint32   N_bits);

/*********************************************************************
    Name: liblte_bits_2_value

    Description: Converts a bit string to a value
*********************************************************************/
uint32 liblte_bits_2_value(uint8  **bits,
                           uint32   N_bits);

/*********************************************************************
    Name: liblte_pack

    Description: Pack a bit array into a byte array
*********************************************************************/
void liblte_pack(LIBLTE_BIT_MSG_STRUCT  *bits,
                 LIBLTE_BYTE_MSG_STRUCT *bytes);

/*********************************************************************
    Name: liblte_unpack

    Description: Unpack a byte array into a bit array
*********************************************************************/
void liblte_unpack(LIBLTE_BYTE_MSG_STRUCT *bytes,
                   LIBLTE_BIT_MSG_STRUCT  *bits);

/*********************************************************************
    Name: liblte_pack

    Description: Pack a bit array into a byte array
*********************************************************************/
void liblte_pack(uint8_t *bits,  uint32_t n_bits, uint8_t *bytes);

/*********************************************************************
    Name: liblte_unpack

    Description: Unpack a byte array into a bit array
*********************************************************************/
void liblte_unpack(uint8_t *bytes, uint32_t n_bytes, uint8_t *bits);

/*********************************************************************
    Name: liblte_align_up

    Description: Aligns a pointer to a multibyte boundary
*********************************************************************/
void liblte_align_up(uint8_t **ptr, uint32_t align);

/*********************************************************************
    Name: liblte_align_up_zero

    Description:  Aligns a pointer to a multibyte boundary and zeros
                  bytes skipped
*********************************************************************/
void liblte_align_up_zero(uint8_t **ptr, uint32_t align);

#endif // SRSLTE_LIBLTE_COMMON_H
