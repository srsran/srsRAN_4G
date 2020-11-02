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

/*!
 * \file polar_decoder_ssc_c_avx2.c
 * \brief Definition of the SSC polar decoder inner functions working with
 * 8-bit integer-valued LLRs and AVX2 instructions.
 *
 * \author Jesus Gomez (CTTC)
 * \date 2020
 *
 * \copyright Software Radio Systems Limited
 *
 */

#include "polar_decoder_ssc_c_avx2.h"
#include "../utils_avx2.h"
#include "polar_decoder_vector_avx2.h"
#include "srslte/phy/fec/polar/polar_encoder.h"

#ifdef LV_HAVE_AVX2

/*!
 * \brief Describes the state of a AVX2 SSC polar decoder
 */
struct StateAVX2 {
  uint8_t  stage;   /*!< \brief Current stage [0 - code_size_log] of the decoding algorithm. */
  uint16_t bit_pos; /*!< \brief position of the next bit to be estimated in est_bit buffer. */
};

/*!
 * \brief Describes an SSC polar decoder (8-bit version).
 */
struct pSSC_c_avx2 {
  int8_t**                llr0;    /*!< \brief Pointers to the upper half of LLRs values at all stages. */
  int8_t**                llr1;    /*!< \brief Pointers to the lower half of LLRs values at all stages. */
  uint8_t*                est_bit; /*!< \brief Pointers to the temporary estimated bits. */
  struct Params*          param;   /*!< \brief Pointer to a Params structure. */
  struct StateAVX2*       state;   /*!< \brief Pointer to a State. */
  srslte_polar_encoder_t* enc;     /*!< \brief Pointer to a srslte_polar_encoder_t. */
  void (*f)(const int8_t* x, const int8_t* y, int8_t* z, const uint16_t len); /*!< \brief Pointer to the function-f. */
  void (*g)(const uint8_t* b,
            const int8_t*  x,
            const int8_t*  y,
            int8_t*        z,
            const uint16_t len); /*!< \brief Pointer to the function-g. */
  void (*xor)(const uint8_t* x,
              const uint8_t* y,
              uint8_t*       z,
              const uint16_t len);                                   /*!< \brief Pointer to the function-g. */
  void (*hard_bit)(const int8_t* x, uint8_t* z, const uint16_t len); /*!< \brief Pointer to the hard-bit function. */
};

/*!
 * max function
 */
static int max(int a, int b)
{
  return a > b ? a : b;
}

/*!
 * Switches between the different types of node (::RATE_1, ::RATE_0, ::RATE_R) for the SSC algorithm.
 * Nodes in the decoding tree at stage \f$ s\f$ get the \f$2^s\f$ LLRs from the parent node and
 * return the associated \f$2^s\f$ estimated bits.
 *
 * All decoded bits below a ::RATE_0 node are 0. The function updates the \a p->state->active_node_per_stage
 * pointer to point to the next active node. It is assumed that message bits are initialized to 0.
 *
 * ::RATE_1 nodes at stage \f$ s \f$ return the associated \f$2^s\f$ estimated bits by
 * making a hard decision on them.
 * ::RATE_1 nodes also update message bits vector.
 *
 * ::RATE_R nodes at stage \f$ s \f$ return the associated \f$2^s\f$ decoded bits by calling
 * the child nodes to the right and left of the decoding tree and then polar encoding (xor) their output.
 * At stage \f$ s \f$, this function runs function srslte_vec_function_f_fff() and srslte_vec_function_g_bfff()
 * with vector size \f$2^{ s - 1}\f$ and updates \a llr0 and \a llr1 memory space for stage \f$(s - 1)\f$.
 * This function also runs srslte_vec_xor_bbb() with vector size \f$2^{s-1}\f$ and
 * updates \a estbits memory space for stage \f$(s + 1)\f$.
 *
 */
static void simplified_node(struct pSSC_c_avx2* p);

void delete_polar_decoder_ssc_c_avx2(void* p)
{
  struct pSSC_c_avx2* pp = p;

  if (p != NULL) {
    free(pp->llr0[0]); // remove LLR buffer.
    free(pp->llr0);
    free(pp->llr1);
    free(pp->param->node_type[0]);
    free(pp->param->node_type);
    free(pp->est_bit); // remove estbits buffer.
    free(pp->param->code_stage_size);
    free(pp->param);
    free(pp->state);
    srslte_polar_encoder_free(pp->enc);
    free(pp->enc);
    free(pp);
  }
}

void* create_polar_decoder_ssc_c_avx2(uint16_t* frozen_set, const uint8_t code_size_log, const uint16_t frozen_set_size)
{
  struct pSSC_c_avx2* pp = NULL; // pointer to the polar decoder instance
  // allocate memory to the polar decoder instance
  if ((pp = malloc(sizeof(struct pSSC_c_avx2))) == NULL) {
    return NULL;
  }

  // set functions
  pp->f        = srslte_vec_function_f_ccc_avx2;
  pp->g        = srslte_vec_function_g_bccc_avx2;
  pp->xor      = srslte_vec_xor_bbb_avx2;
  pp->hard_bit = srslte_vec_hard_bit_cc_avx2;

  // encoder of maximum size
  if ((pp->enc = malloc(sizeof(srslte_polar_encoder_t))) == NULL) {
    free(pp);
    return NULL;
  }

  srslte_polar_encoder_init(pp->enc, SRSLTE_POLAR_ENCODER_AVX2, code_size_log);

  // algorithm constants/parameters
  if ((pp->param = malloc(sizeof(struct Params))) == NULL) {
    free(pp->enc);
    free(pp);
    return NULL;
  }

  if ((pp->param->code_stage_size = malloc((code_size_log + 1) * sizeof(uint16_t))) == NULL) {
    free(pp->param);
    free(pp->enc);
    free(pp);
    return NULL;
  }

  pp->param->code_stage_size[0] = 1;
  for (uint8_t i = 1; i < code_size_log + 1; i++) {
    pp->param->code_stage_size[i] = 2 * pp->param->code_stage_size[i - 1];
  }

  pp->param->code_size_log = code_size_log;

  // state  -- initialized in polar_decoder_ssc_init
  if ((pp->state = malloc(sizeof(struct StateAVX2))) == NULL) {
    free(pp->param->code_stage_size);
    free(pp->param);
    free(pp->enc);
    free(pp);
    return NULL;
  }

  // allocates memory for estimated bits per stage
  // allocates extra SRSLTE_AVX2_B_SIZE bytes to allow store the output of 256-bit instructions
  int est_bit_size = pp->param->code_stage_size[code_size_log] + SRSLTE_AVX2_B_SIZE;

  pp->est_bit = aligned_alloc(SRSLTE_AVX2_B_SIZE, est_bit_size); // every 32 chars are aligned

  // allocate memory for LLR pointers.
  pp->llr0 = malloc((code_size_log + 1) * sizeof(int8_t*));
  pp->llr1 = malloc((code_size_log + 1) * sizeof(int8_t*));

  // LLR MEMORY NOT ALIGNED FOR LLR_BUFFERS_SIZE < SRSLTE_SIMB_LLR_ALIGNED

  // We do not align the memory at lower stages, as if done, after each function f and function g
  // operation, the second half of the output vector needs to be moved to the next
  // aligned position. This extra operation may incur more overhead that the gain of aligned memory.

  uint8_t  n_llr_all_stages = code_size_log + 1; // there are 2^(n_llr_all_stages) - 1 LLR values summing up all stages.
  uint16_t llr_all_stages   = 1U << n_llr_all_stages;

  // Reserve at least SRSLTE_AVX2_B_SIZE bytes for each stage, so that there is space for the output
  // of the 32-bytes mm256 vectorized functions.
  // llr1 (second half) of lower stages is not aligned.

  uint16_t llr_all_stages_avx2 = llr_all_stages;
  if (code_size_log >= 5) {
    llr_all_stages_avx2 += SRSLTE_AVX2_B_SIZE * 5;
  } else {
    llr_all_stages_avx2 += (code_size_log + 1) * SRSLTE_AVX2_B_SIZE;
  }

  // add extra SRSLTE_AVX2_B_SIZE llrs positions for hard_bit functions on the last bits have
  // access to allocated memory
  llr_all_stages_avx2 += SRSLTE_AVX2_B_SIZE;

  pp->llr0[0] = aligned_alloc(SRSLTE_AVX2_B_SIZE, llr_all_stages_avx2 * sizeof(int8_t)); // 32*8=256

  // allocate memory to the polar decoder instance
  if (pp->llr0[0] == NULL) {
    free(pp->est_bit);
    free(pp->state);
    free(pp->param->code_stage_size);
    free(pp->param);
    free(pp->enc);
    free(pp);
    return NULL;
  }

  pp->llr1[0] = pp->llr0[0] + 1;
  for (uint8_t s = 1; s < code_size_log + 1; s++) {
    pp->llr0[s] = pp->llr0[s - 1] + max(SRSLTE_AVX2_B_SIZE, pp->param->code_stage_size[s - 1]);
    pp->llr1[s] = pp->llr0[s] + pp->param->code_stage_size[s - 1];
  }

  // allocate memory for node type pointers, one per stage.
  pp->param->frozen_set_size = frozen_set_size;
  pp->param->node_type       = malloc((code_size_log + 1) * sizeof(uint8_t*));

  // allocate memory to node_type_ssc. Stage s has 2^(N-s) nodes s=0,...,N.
  // Thus, same size as LLRs all stages.
  pp->param->node_type[0] = aligned_alloc(SRSLTE_AVX2_B_SIZE, llr_all_stages * sizeof(uint8_t)); // 32*8=256

  if (pp->param->node_type[0] == NULL) {
    free(pp->param->node_type);
    free(pp->est_bit);
    free(pp->state);
    free(pp->param->code_stage_size);
    free(pp->param);
    free(pp->enc);
    free(pp);
    return NULL;
  }

  // initialize all node type pointers. (stage 0 is the first, opposite to LLRs)
  for (uint8_t s = 1; s < code_size_log + 1; s++) {
    pp->param->node_type[s] = pp->param->node_type[s - 1] + pp->param->code_stage_size[code_size_log - s + 1];
  }

  init_node_type(frozen_set, pp->param);

  return pp;
}

int init_polar_decoder_ssc_c_avx2(void* p, const int8_t* input_llr, uint8_t* data_decoded)
{
  struct pSSC_c_avx2* pp = p;

  if (p == NULL) {
    return -1;
  }

  uint8_t code_size_log  = pp->param->code_size_log;
  int16_t code_size      = pp->param->code_stage_size[code_size_log];
  int16_t code_half_size = pp->param->code_stage_size[code_size_log - 1];

  // Initializes the data_decoded_vector to all zeros
  memset(data_decoded, 0, code_size);

  // Initialize est_bit vector to all zeros
  int est_bit_size = pp->param->code_stage_size[code_size_log] + SRSLTE_AVX2_B_SIZE;
  memset(pp->est_bit, 0, est_bit_size);

  // Initializes LLR buffer for the last stage/level with the input LLRs values
  memcpy(&pp->llr0[code_size_log][0], &input_llr[0], code_half_size * sizeof(int8_t));
  memcpy(&pp->llr1[code_size_log][0], &input_llr[code_half_size], code_half_size * sizeof(int8_t));

  // Initializes the state of the decoding tree
  pp->state->stage   = code_size_log + 1; // start from the only one node at the last stage + 1.
  pp->state->bit_pos = 0;

  return 0;
}

int polar_decoder_ssc_c_avx2(void* p, uint8_t* data_decoded)
{

  if (p == NULL) {
    return -1;
  }

  struct pSSC_c_avx2* pp = p;

  simplified_node(pp);

  // est_bit contains the coded bits. To obtain the message, we call the encoder
  srslte_polar_encoder_encode(pp->enc, pp->est_bit, data_decoded, pp->param->code_size_log);

  // transform {0,-128} into {0, 1}
  srslte_vec_sign_to_bit_c_avx2(data_decoded, 1U << pp->param->code_size_log);
  return 0;
}

static void simplified_node(struct pSSC_c_avx2* p)
{

  struct pSSC_c_avx2* pp = p;

  pp->state->stage--; // to child node.

  uint8_t  stage    = pp->state->stage;
  uint16_t bit_pos  = pp->state->bit_pos >> stage;
  uint8_t* estbits0 = NULL;
  uint8_t* estbits1 = NULL;

  uint16_t stage_size      = pp->param->code_stage_size[stage];
  uint16_t stage_half_size = pp->param->code_stage_size[stage - 1];

  switch (pp->param->node_type[stage][bit_pos]) {

    case RATE_1:
      pp->hard_bit(pp->llr0[stage], pp->est_bit + pp->state->bit_pos, stage_size);

      pp->state->bit_pos = pp->state->bit_pos + stage_size;
      break;

    case RATE_0:
      pp->state->bit_pos = pp->state->bit_pos + stage_size;
      break;

    case RATE_R:

      pp->f(pp->llr0[stage], pp->llr1[stage], pp->llr0[stage - 1], stage_half_size);

      // move to the child node to the left (up) of the tree.
      simplified_node(pp);

      estbits0 = pp->est_bit + pp->state->bit_pos - stage_half_size;
      pp->g(estbits0, pp->llr0[stage], pp->llr1[stage], pp->llr0[stage - 1], stage_half_size);

      // move to the child node to the right (down) of the tree.
      simplified_node(pp);

      estbits0 = pp->est_bit + pp->state->bit_pos - stage_size;
      estbits1 = pp->est_bit + pp->state->bit_pos - stage_size + stage_half_size;
      pp->xor (estbits0, estbits1, estbits0, stage_half_size);

      break;

    default:
      printf("ERROR: wrong node type %d\n", pp->param->node_type[stage][bit_pos]);
      exit(-1);
      break;
  }

  pp->state->stage++; // to parent node.
}

#endif // LV_HAVE_AVX2
