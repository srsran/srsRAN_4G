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

#ifndef SRSLTE_LIBLTE_SSL_H
#define SRSLTE_LIBLTE_SSL_H

#ifdef HAVE_POLARSSL

#include "polarssl/aes.h"
#include "polarssl/sha256.h"

void sha256(const unsigned char* key,
            size_t               keylen,
            const unsigned char* input,
            size_t               ilen,
            unsigned char        output[32],
            int                  is224)
{
  sha256_hmac(key, keylen, input, ilen, output, is224);
}

#endif // HAVE_POLARSSL

#ifdef HAVE_MBEDTLS

#include "mbedtls/aes.h"
#include "mbedtls/md.h"

typedef mbedtls_aes_context aes_context;

#define AES_ENCRYPT 1
#define AES_DECRYPT 0

int aes_setkey_enc(aes_context* ctx, const unsigned char* key, unsigned int keysize)
{
  return mbedtls_aes_setkey_enc(ctx, key, keysize);
}

int aes_crypt_ecb(aes_context* ctx, int mode, const unsigned char input[16], unsigned char output[16])
{
  return mbedtls_aes_crypt_ecb(ctx, mode, input, output);
}

int aes_crypt_ctr(aes_context*         ctx,
                  size_t               length,
                  size_t*              nc_off,
                  unsigned char        nonce_counter[16],
                  unsigned char        stream_block[16],
                  const unsigned char* input,
                  unsigned char*       output)
{
  return mbedtls_aes_crypt_ctr(ctx, length, nc_off, nonce_counter, stream_block, input, output);
}

void sha256(const unsigned char* key,
            size_t               keylen,
            const unsigned char* input,
            size_t               ilen,
            unsigned char        output[32],
            int                  is224)
{
  mbedtls_md_hmac(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), key, keylen, input, ilen, output);
}

#endif // HAVE_MBEDTLS

#endif // SRSLTE_LIBLTE_SSL_H
