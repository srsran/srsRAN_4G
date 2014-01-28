
#ifndef DEFINE_H
#define DEFINE_H
#include <stdint.h>

typedef uint8_t output_t;

#define INPUT_MAX_SAMPLES 	0
#define OUTPUT_MAX_SAMPLES 	2048*14 

#define NOF_INPUT_ITF		0
#define NOF_OUTPUT_ITF		2

#endif
/**@} */

#define GENERATE_COMPLEX

#ifndef INCLUDE_DEFS_ONLY

/* Input and output buffer sizes (in number of samples) */
const int input_max_samples = INPUT_MAX_SAMPLES;
const int output_max_samples = OUTPUT_MAX_SAMPLES;

/* leave these two lines unmodified */
const int input_sample_sz = sizeof(input_t);
int output_sample_sz = sizeof(output_t);

/* Number of I/O interfaces. All have the same maximum size */
const int nof_input_itf = NOF_INPUT_ITF;
const int nof_output_itf = NOF_OUTPUT_ITF;

#endif
