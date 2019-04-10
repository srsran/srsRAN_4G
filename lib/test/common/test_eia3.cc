/*
 * Includes
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <sys/time.h>

#include "srslte/srslte.h"
#include "srslte/common/security.h"
#include "srslte/common/liblte_security.h"

/*
 * Tests
 *
 * Document Reference: 33.401 V14.6.0 Annex C.4
 *
 */

void test_set_1()
{
  uint8_t key[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  uint32_t count = 0x0;
  uint8_t bearer = 0x0;
  uint8_t direction = 0;
  uint32_t len_bits = 1;
  uint32_t len_bytes = (len_bits + 7) / 8;
  uint8_t  msg[] = {0x00, 0x00, 0x00, 0x00};
  uint8_t  mt[]  = {0xc8, 0xa9, 0x59, 0x5e};

  uint8_t mac[4];

  // gen mac
//   srslte::security_128_eia3(key, count, bearer, direction, msg, len_bytes, mac);
  liblte_security_128_eia3(key, count, bearer, direction, msg, len_bits, mac);
  int i;

  for(i=0; i<4; i++) {
    printf("%x ", mac[i]);
  }

  printf("\n");

  for(i=0; i<4; i++) {
    if(mac[i] != mt[i]){
      printf("Test Set 1: Failed\n");
    }
    assert(mac[i] == mt[i]);
  }

}



int main(int argc, char * argv[]) {
  test_set_1();
}