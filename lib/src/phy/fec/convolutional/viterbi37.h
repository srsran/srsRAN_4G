// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

#ifndef SRSRAN_VITERBI37_H_
#define SRSRAN_VITERBI37_H_

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

#endif /* SRSRAN_VITERBI37_H_ */
