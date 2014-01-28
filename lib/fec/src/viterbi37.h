#include <stdbool.h>

void *create_viterbi37_port(int polys[3], int len, bool tail_biting);
int init_viterbi37_port(void *p, int starting_state);
int chainback_viterbi37_port(void *p, char *data, unsigned int nbits, unsigned int endstate);
void delete_viterbi37_port(void *p);
int update_viterbi37_blk_port(void *p, float *syms, int nbits, float amp, int framebits);
