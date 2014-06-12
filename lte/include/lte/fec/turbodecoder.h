#define RATE 3
#define TOTALTAIL 12

#define LOG18 -2.07944

#define NUMSTATES 8
#define NINPUTS 2
#define TAIL 3
#define TOTALTAIL 12

#define INF 9e4
#define ZERO 9e-4

#define MAX_LONG_CB     6114
#define MAX_LONG_CODED  (RATE*MAX_LONG_CB+TOTALTAIL)

typedef float llr_t;

typedef struct {
	int long_cb;
	llr_t *beta;
}map_gen_t;

typedef struct {
	int long_cb;

	map_gen_t dec;

	llr_t *llr1;
	llr_t *llr2;
	llr_t *w;
	llr_t *syst;
	llr_t *parity;

	tc_interl_t interleaver;
}tdec_t;

int tdec_init(tdec_t *h, int long_cb);
void tdec_free(tdec_t *h);

void tdec_reset(tdec_t *h);
void tdec_iteration(tdec_t *h, llr_t *input);
void tdec_decision(tdec_t *h, char *output);
void tdec_run_all(tdec_t *h, llr_t *input, char *output, int nof_iterations);
