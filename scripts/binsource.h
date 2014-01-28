#include <stdint.h>

/* Low-level API */
typedef struct {
	unsigned int seed;
	uint32_t *seq_buff;
	int seq_buff_nwords;
	int seq_cache_nbits;
	int seq_cache_rp;
}binsource_t;

void binsource_init(binsource_t* q);
void binsource_destroy(binsource_t* q);
void binsource_seed_set(binsource_t* q, unsigned int seed);
void binsource_seed_time(binsource_t *q);
int binsource_cache_gen(binsource_t* q, int nbits);
void binsource_cache_cpy(binsource_t* q, uint8_t *bits, int nbits);
int binsource_generate(binsource_t* q, uint8_t *bits, int nbits);


/* High-level API */
typedef struct {
	binsource_t obj;
	struct binsource_init {
		int cache_seq_nbits; /* default=2 */
		int seed;
	} init;
	struct binsource_ctrl_in {
		int nbits;
	} ctrl_in;
	uint8_t* output[2];		/* size=2048*14 */
	int* out_len;
}binsource_hl;

int binsource_initialize(binsource_hl* h);
int binsource_work(	binsource_hl* hl);
