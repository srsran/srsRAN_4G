
#ifndef DEFINE_H
#define DEFINE_H
#include <stdint.h>

typedef -typeinput- input_t;
typedef -typeoutput- output_t;

#define INPUT_MAX_SAMPLES 	-sizeinput-
#define OUTPUT_MAX_SAMPLES 	-sizeoutput-

#define NOF_INPUT_ITF		-numinputs-
#define NOF_OUTPUT_ITF		-numoutputs-

#endif
/**@} */

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
