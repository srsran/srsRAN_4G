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

#ifndef SRSLTE_VITERBI37_H_
#define SRSLTE_VITERBI37_H_

#include <stdbool.h>

void* create_viterbi37_port(int polys[3], uint32_t len);

int init_viterbi37_port(void* p, int starting_state);

int chainback_viterbi37_port(void* p, uint8_t* data, uint32_t nbits, uint32_t endstate);

void delete_viterbi37_port(void* p);

int update_viterbi37_blk_port(void* p, uint8_t* syms, uint32_t nbits, uint32_t* best_state);

void* create_viterbi37_sse(int polys[3], uint32_t len);

int init_viterbi37_sse(void* p, int starting_state);

void reset_blk_sse(void* p, int nbits);

int chainback_viterbi37_sse(void* p, uint8_t* data, uint32_t nbits, uint32_t endstate);

void delete_viterbi37_sse(void* p);

int update_viterbi37_blk_sse(void* p, uint8_t* syms, uint32_t nbits, uint32_t* best_state);

void* create_viterbi37_neon(int polys[3], uint32_t len);

int init_viterbi37_neon(void* p, int starting_state);

void reset_blk_neon(void* p, int nbits);

int chainback_viterbi37_neon(void* p, uint8_t* data, uint32_t nbits, uint32_t endstate);

void delete_viterbi37_neon(void* p);

int update_viterbi37_blk_neon(void* p, uint8_t* syms, uint32_t nbits, uint32_t* best_state);

void* create_viterbi37_avx2(int polys[3], uint32_t len);

int init_viterbi37_avx2(void* p, int starting_state);

void reset_blk_avx2(void* p, int nbits);

int chainback_viterbi37_avx2(void* p, uint8_t* data, uint32_t nbits, uint32_t endstate);

void delete_viterbi37_avx2(void* p);

int update_viterbi37_blk_avx2(void* p, uint8_t* syms, uint32_t nbits, uint32_t* best_state);

void* create_viterbi37_avx2_16bit(int polys[3], uint32_t len);

int init_viterbi37_avx2_16bit(void* p, int starting_state);

void reset_blk_avx2_16bit(void* p, int nbits);

int chainback_viterbi37_avx2_16bit(void* p, uint8_t* data, uint32_t nbits, uint32_t endstate);

void delete_viterbi37_avx2_16bit(void* p);

int update_viterbi37_blk_avx2_16bit(void* p, uint16_t* syms, uint32_t nbits, uint32_t* best_state);

#endif /* SRSLTE_VITERBI37_H_ */
