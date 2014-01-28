#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

#include "lte.h"

char *input_file_name;
int frame_length=1920, symbol_sz=128, nof_slots=1;

void usage(char *prog) {
	printf("Usage: %s [ls] -i input_file\n", prog);
	printf("\t-l frame_length [Default %d]\n", frame_length);
	printf("\t-s symbol_sz [Default %d]\n", symbol_sz);
	printf("\t-n nof_frames [Default %d]\n", nof_slots);
	printf("\t-v [set verbose to debug, default none]\n");
}

void parse_args(int argc, char **argv) {
	int opt;
	while ((opt = getopt(argc, argv, "ilsnv")) != -1) {
		switch(opt) {
		case 'i':
			input_file_name = argv[optind];
			break;
		case 'l':
			frame_length = atoi(argv[optind]);
			break;
		case 's':
			symbol_sz = atoi(argv[optind]);
			break;
		case 'n':
			nof_slots = atoi(argv[optind]);
			break;
		case 'v':
			verbose++;
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

void fft_run_slot(dft_plan_t *fft_plan, cf_t *input, cf_t *output) {
	int i;
	for (i=0;i<7;i++) {
		input += CP_NORM(i, symbol_sz);
		dft_run_c2c(fft_plan, input, output);
		input += symbol_sz;
		output += symbol_sz;
	}
}

int main(int argc, char **argv) {
	filesource_t fsrc;
	pbch_mib_t pbch_data;
	pbch_t pbch;
	dft_plan_t fft_plan;

	int frame_cnt;
	cf_t *input, *outfft;

	if (argc < 3) {
		usage(argv[0]);
		exit(-1);
	}

	parse_args(argc,argv);

	if (filesource_init(&fsrc, input_file_name, COMPLEX_FLOAT)) {
		fprintf(stderr, "Error opening file %s\n", input_file_name);
		exit(-1);
	}

	input = malloc(frame_length*sizeof(cf_t));
	if (!input) {
		perror("malloc");
		exit(-1);
	}
	outfft = malloc(7*symbol_sz*sizeof(cf_t));
	if (!outfft) {
		perror("malloc");
		exit(-1);
	}

	/* Init FFT plan */
	if (dft_plan_c2c(symbol_sz, FORWARD, &fft_plan)) {
		fprintf(stderr, "Error initiating FFT plan\n");
		exit(-1);
	}
	fft_plan.options = DFT_DC_OFFSET | DFT_MIRROR_POS | DFT_NORMALIZE;

	if (pbch_init(&pbch, 0, CPNORM)) {
		fprintf(stderr, "Error initiating PBCH\n");
		exit(-1);
	}
	int res = 0;
	frame_cnt = 0;
	while (frame_length == filesource_read(&fsrc, input, frame_length)
			&& frame_cnt < nof_slots
			&& res == 0) {

		fft_run_slot(&fft_plan, &input[960], outfft);

		res = pbch_decode(&pbch, outfft, &pbch_data, 6, 1);
		if (res == -1) {
			fprintf(stderr, "Error decoding PBCH\n");
			break;
		}
		frame_cnt++;
	}

	if (res == 1) {
		printf("MIB found\n");
	} else {
		printf("MIB not found after %d frames\n", frame_cnt);
	}

	pbch_free(&pbch);
	free(input);
	free(outfft);

	printf("Done\n");
	exit(0);
}
