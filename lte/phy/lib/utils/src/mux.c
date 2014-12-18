/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The libLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the libLTE library.
 *
 * libLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */



#include <stdint.h>
#include <string.h>

/**
 * Multiplexes a signal from nof_inputs interfaces.
 * Copies output_lengths[i] samples to the i-th interfaces, ignores output_padding_pre[i] samples
 * from the beginning each input interface.
 */
void mux(void **input, void *output, int *input_lengths, int *input_padding_pre, int nof_inputs,
    int sample_sz) {
  int i,r;
  uint8_t *out = (uint8_t*) output;
  uint8_t **in = (uint8_t**) input;

  r=0;
  for (i=0;i<nof_inputs;i++) {
    memcpy(&out[r*sample_sz],&in[i][sample_sz*input_padding_pre[i]],sample_sz*input_lengths[i]);
    r+=input_lengths[i];
  }
}

/**
 * De-multiplexes a signal to nof_outputs interfaces.
 * Copies output_lengths[i] samples to the i-th interfaces, adds output_padding_pre[i] zeros
 * to the beginning and output_padding_post[i] zeros to the end.
 */
void demux(void *input, void **output, int *output_lengths,
    int *output_padding_pre, int *output_padding_post, int nof_outputs,
    int sample_sz) {
  int i,r;
  uint8_t **out = (uint8_t**) output;
  uint8_t *in = (uint8_t*) input;

  r=0;
  for (i=0;i<nof_outputs;i++) {
    memset(&out[i][0],0,sample_sz*output_padding_pre[i]);
    memcpy(&out[i][sample_sz*output_padding_pre[i]],&in[r*sample_sz],sample_sz*output_lengths[i]);
    memset(&out[i][sample_sz*(output_padding_pre[i]+output_lengths[i])],0,sample_sz*output_padding_post[i]);
    r+=output_lengths[i]+output_padding_post[i];
  }
}
