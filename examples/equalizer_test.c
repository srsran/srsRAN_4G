#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

#include "lte.h"

char *input_file_name;
int nof_slots=1;
int cell_id = 0;
int port_id = 0;
int nof_prb = 6;
lte_cp_t cp = CPNORM;
int file_binary = 0;

int in_slot_length() {
	if (CP_ISNORM(cp)) {
		return SLOT_LEN_CPNORM(lte_symbol_sz(nof_prb));
	} else {
		return SLOT_LEN_CPEXT(lte_symbol_sz(nof_prb));
	}
}

int slot_length() {
	return CP_NSYMB(cp)*lte_symbol_sz(nof_prb);
}


void usage(char *prog) {
	printf("Usage: %s [bncprev] -i input_file\n", prog);
	printf("\t-b input file is binary [Default no]\n");
	printf("\t-n number of slots [Default %d]\n", nof_slots);
	printf("\t-c cell_id [Default %d]\n", cell_id);
	printf("\t-p port_id [Default %d]\n", port_id);
	printf("\t-r nof_prb [Default %d]\n", nof_prb);
	printf("\t-e [extended cyclic prefix, Default normal]\n");
	printf("\t-v [set verbose to debug, default none]\n");
}

void parse_args(int argc, char **argv) {
	int opt;
	while ((opt = getopt(argc, argv, "bincprev")) != -1) {
		switch(opt) {
		case 'b':
			file_binary = 1;
			break;
		case 'i':
			input_file_name = argv[optind];
			break;
		case 'n':
			nof_slots = atoi(argv[optind]);
			break;
		case 'c':
			cell_id = atoi(argv[optind]);
			break;
		case 'p':
			port_id = atoi(argv[optind]);
			break;
		case 'r':
			nof_prb = atoi(argv[optind]);
			break;
		case 'e':
			cp = CPEXT;
			break;
		case 'v':
			PRINT_DEBUG;
			break;
		default:
			usage(argv[0]);
			exit(-1);
		}
	}
	if (!input_file_name) {
		usage(argv[0]);
		exit(-1);
	}
}

int main(int argc, char **argv) {
	filesource_t fsrc;
	lte_fft_t fft;
	FILE *f = NULL;
	chest_t eq;
	int slot_cnt;
	cf_t *input = NULL;
	cf_t *outfft = NULL;
	cf_t *ce = NULL;

	if (argc < 3) {
		usage(argv[0]);
		exit(-1);
	}

	parse_args(argc,argv);

	if (filesource_init(&fsrc, input_file_name, file_binary?COMPLEX_FLOAT_BIN:COMPLEX_FLOAT)) {
		fprintf(stderr, "Error opening file %s\n", input_file_name);
		goto do_exit;
	}
	f = fopen("output.m", "w");
	if (!f) {
		perror("fopen");
		goto do_exit;
	}

	input = malloc(in_slot_length()*sizeof(cf_t));
	if (!input) {
		perror("malloc");
		goto do_exit;
	}
	outfft = malloc(slot_length()*sizeof(cf_t));
	if (!outfft) {
		perror("malloc");
		goto do_exit;
	}
	ce = malloc(nof_prb * RE_X_RB * CP_NSYMB(cp) * sizeof(cf_t));
	if (!ce) {
		perror("malloc");
		goto do_exit;
	}

	if (lte_fft_init(&fft, cp, lte_symbol_sz(nof_prb))) {
		fprintf(stderr, "Error: initializing FFT\n");
		goto do_exit;
	}
	if (chest_init(&eq, cp, nof_prb, 1)) {
		fprintf(stderr, "Error initializing equalizer\n");
		goto do_exit;
	}
	if (chest_ref_LTEDL(&eq, cell_id)) {
		fprintf(stderr, "Error initializing reference signal\n");
		goto do_exit;
	}

	bzero(input, sizeof(cf_t) * in_slot_length());
	bzero(outfft, sizeof(cf_t) * slot_length());

	/* read all file or nof_slots */
	slot_cnt = 0;
	while (in_slot_length() == filesource_read(&fsrc, input, in_slot_length())
			&& (slot_cnt < nof_slots || nof_slots == -1)) {

		lte_fft_run(&fft, input, outfft);

		chest_ce_slot_port(&eq, outfft, ce, slot_cnt%20, 0);

		chest_fprint(&eq, f, slot_cnt%20, 0);

		fprintf(f, "ce=[");
		vec_fprint_c(f, ce, nof_prb * RE_X_RB * CP_NSYMB(cp));
		fprintf(f, "];\n");

		slot_cnt++;
	}

do_exit:
	chest_free(&eq);
	lte_fft_free(&fft);
	if (ce) {
		free(ce);
	}
	if (outfft) {
		free(outfft);
	}
	if (input) {
		free(input);
	}
	if (f) {
		fclose(f);
	}
	filesource_close(&fsrc);

	printf("Done\n");
	exit(0);
}
