/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef GEN_MCH_TALBES_H
#define GEN_MCH_TALBES_H

/******************************************************************************
 * Common mch table generation - used in phch_common of UE and ENB for MBMS
 *****************************************************************************/
#include <pthread.h>
#include <string.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


void generate_frame_mch_table(uint8_t *table, uint8_t alloc);
void generate_mch_table(uint8_t *table, uint32_t sf_alloc, uint8_t num_frames);
void generate_mcch_table(uint8_t *table, uint32_t sf_alloc);

#ifdef __cplusplus
}
#endif // __cplusplus


#endif // SECURITY_H
