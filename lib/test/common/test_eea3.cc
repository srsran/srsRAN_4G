/*
 * Includes
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "srslte/common/liblte_security.h"
#include "srslte/srslte.h"

/*
 * Prototypes
 */

int32 arrcmp(uint8_t const* const a, uint8_t const* const b, uint32 len)
{
  uint32 i = 0;

  for (i = 0; i < len; i++) {
    if (a[i] != b[i]) {
      return a[i] - b[i];
    }
  }
  return 0;
}

/*
 * Tests
 *
 * Document Reference: 33.401 V13.1.0 Annex C.1
 */

void test_set_1()
{
  LIBLTE_ERROR_ENUM err_lte = LIBLTE_ERROR_INVALID_INPUTS;
  int32             err_cmp = 0;

  uint8_t  key[]     = {0x17, 0x3d, 0x14, 0xba, 0x50, 0x03, 0x73, 0x1d, 0x7a, 0x60, 0x04, 0x94, 0x70, 0xf0, 0x0a, 0x29};
  uint32_t count     = 0x66035492;
  uint8_t  bearer    = 0xf;
  uint8_t  direction = 0;
  uint32_t len_bits = 193;
  uint32_t len_bytes = (len_bits + 7) / 8;

  uint8_t msg[] = {0x6c, 0xf6, 0x53, 0x40, 0x73, 0x55, 0x52, 0xab, 0x0c, 0x97, 0x52, 0xfa, 0x6f, 0x90,
                   0x25, 0xfe, 0x0b, 0xd6, 0x75, 0xd9, 0x00, 0x58, 0x75, 0xb2, 0x00, 0x00, 0x00, 0x00};

  uint8_t ct[] = {0xa6, 0xc8, 0x5f, 0xc6, 0x6a, 0xfb, 0x85, 0x33, 0xaa, 0xfc, 0x25, 0x18, 0xdf, 0xe7,
                  0x84, 0x94, 0x0e, 0xe1, 0xe4, 0xb0, 0x30, 0x23, 0x8c, 0xc8, 0x00, 0x00, 0x00, 0x00};

  uint8_t* out = (uint8_t*)calloc(len_bytes, sizeof(uint8_t));

  // encryption
  err_lte = liblte_security_encryption_eea3(key, count, bearer, direction, msg, len_bits, out);
  assert(err_lte == LIBLTE_SUCCESS);

  // compare cipher text
  err_cmp = arrcmp(ct, out, len_bytes);
  for (int i = 0; i < len_bytes; i++){
      printf("%x ", out[i]);
  }

  printf("\n");
  if (err_cmp == 0) {
    printf("Success\n");
  } else {
    printf("Fail\n");
  }

  assert(err_cmp == 0);

  // decryption
  //   err_lte = liblte_security_decryption_eea3(key, count, bearer, direction, ct, len_bits, out);
  //   assert(err_lte == LIBLTE_SUCCESS);

  // compare cipher text
  //   err_cmp = arrcmp(msg, out, len_bytes);
  //   assert(err_cmp == 0);

  free(out);
}

int main(int argc, char* argv[])
{
  test_set_1();
}
