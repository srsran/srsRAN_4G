/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef GEN_MCH_TALBES_H
#define GEN_MCH_TALBES_H

/******************************************************************************
 * Common mch table generation - used in phch_common of UE and ENB for MBMS
 *****************************************************************************/
#include <pthread.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void generate_frame_mch_table(uint8_t* table, uint8_t alloc);
void generate_mch_table(uint8_t* table, uint32_t sf_alloc, uint8_t num_frames);
void generate_mcch_table(uint8_t* table, uint32_t sf_alloc);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // SECURITY_H
