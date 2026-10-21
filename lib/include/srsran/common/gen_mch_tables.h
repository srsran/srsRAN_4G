// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

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
