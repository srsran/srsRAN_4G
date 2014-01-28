#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#include "lte.h"

typedef _Complex float cf_t;

int frame_length=1000, nof_slots=128;
float ebno_db = 5.0;
unsigned int seed=0;
bool tail_biting = false;

char message[40] = {0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1,0,0,1,1,0,1,0,0,0,0,1};

void usage(char *prog) {
	printf("Usage: %s [nl]\n", prog);
	printf("\t-n nof_frames [Default %d]\n", nof_slots);
	printf("\t-l frame_length [Default %d]\n", frame_length);
	printf("\t-e ebno in dB [Default %.2f dB]\n", ebno_db);
	printf("\t-s seed [Default 0=time]\n");
	printf("\t-t tail_bitting [Default %s]\n", tail_biting?"yes":"no");
}

void parse_args(int argc, char **argv) {
	int opt;
	while ((opt = getopt(argc, argv, "nlste")) != -1) {
		switch(opt) {
		case 'n':
			nof_slots = atoi(argv[optind]);
			break;
		case 'l':
			frame_length = atoi(argv[optind]);
			break;
		case 'e':
			ebno_db = atof(argv[optind]);
			break;
		case 's':
			seed = atoi(argv[optind]);
			break;
		case 't':
			tail_biting = true;
			break;
		default:
			usage(argv[0]);
			exit(-1);
		}
	}
}

int main(int argc, char **argv) {
	viterbi_t dec;
	convcoder_t cod;
	modem_table_t modem;
	demod_soft_t demod;
	int frame_cnt;
	float *llr;
	char *data_tx, *data_rx, *symbols;
	cf_t *iq;
	int i;

	parse_args(argc,argv);

	if (!seed) {
		seed = time(NULL);
	}
	srand(seed);

	int coded_length = 3 * (frame_length + ((tail_biting)?0:6));

	printf("Convolutional Code 1/3 K=7 Test\n");
	printf("  Frame length: %d\n", frame_length);
	printf("  Codeword length: %d\n", coded_length);
	printf("  Tail bitting: %s\n", tail_biting?"yes":"no");
	printf("  EbNo: %.2f\n", ebno_db);

	data_tx = malloc(frame_length * sizeof(char));
	if (!data_tx) {
		perror("malloc");
		exit(-1);
	}

	data_rx = malloc(frame_length * sizeof(char));
	if (!data_rx) {
		perror("malloc");
		exit(-1);
	}

	symbols = malloc(coded_length * sizeof(char));
	if (!symbols) {
		perror("malloc");
		exit(-1);
	}
	llr = malloc(coded_length * sizeof(float));
	if (!llr) {
		perror("malloc");
		exit(-1);
	}

	iq = malloc(coded_length * sizeof(cf_t));
	if (!iq) {
		perror("malloc");
		exit(-1);
	}

	cod.K = 7;
	cod.R = 3;
	cod.tail_biting = tail_biting;
	cod.framelength = frame_length;
	cod.poly[0] = 0x6D;
	cod.poly[1] = 0x4F;
	cod.poly[2] = 0x57;

	float var = sqrt(pow(10,-ebno_db/10));

	modem_table_init(&modem);
	modem_table_std(&modem, LTE_QPSK, true);
	demod_soft_init(&demod);
	demod_soft_table_set(&demod, &modem);
	demod_soft_alg_set(&demod, APPROX);
	demod_soft_sigma_set(&demod, var);

	viterbi_init(&dec, CONVCODER_37, cod.poly, frame_length, tail_biting);

	/* read all file or nof_frames */
	frame_cnt = 0;
	unsigned int errors=0;
	while (frame_cnt < nof_slots) {

		/* generate data_tx */
		for (i=0;i<frame_length;i++) {
			data_tx[i] = message[i];
		}

		conv_encode(&cod, data_tx, symbols);

		bit_fprint(stdout, symbols, 120);

		mod_modulate(&modem, symbols, iq, coded_length);

		if (ebno_db < 100.0) {
			ch_awgn(iq, iq, var, coded_length/2);
		}

		demod_soft_demodulate(&demod, iq, llr, coded_length/2);

		viterbi_decode(&dec, llr, data_rx);

		errors += bit_diff(data_tx, data_rx, frame_length);
		frame_cnt++;
	}

	printf("BER:\t%g\t%u errors\n", (float) errors/(frame_cnt*frame_length), errors);

	viterbi_free(&dec);

	free(data_tx);
	free(symbols);
	free(iq);
	free(llr);
	free(data_rx);

	printf("Done\n");
	exit(0);
}
