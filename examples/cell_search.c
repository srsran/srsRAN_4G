#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "lte.h"

char *input_file_name = NULL;
int nof_slots=100;
float corr_peak_threshold=2.5;
int ntime = 4;
int nfreq = 10;
int file_binary = 0;
int force_N_id_2=-1;

filesource_t fsrc;
cf_t *input_buffer, *fft_buffer;
pbch_t pbch;
dft_plan_t fft_plan;
chest_t chest;
sync_t synch;

void usage(char *prog) {
	printf("Usage: %s [onlt] -i input_file\n", prog);
	printf("\t-n number of frames [Default %d]\n", nof_slots);
	printf("\t-t correlation threshold [Default %g]\n", corr_peak_threshold);
	printf("\t-v [set verbose to debug, default none]\n");
	printf("\t-b Input files is binary [Default %s]\n", file_binary?"yes":"no");
	printf("\t-f force_N_id_2 [Default %d]\n", force_N_id_2);
}

void parse_args(int argc, char **argv) {
	int opt;
	while ((opt = getopt(argc, argv, "intvbf")) != -1) {
		switch(opt) {
		case 'i':
			input_file_name = argv[optind];
			break;
		case 'n':
			nof_slots = atoi(argv[optind]);
			break;
		case 't':
			corr_peak_threshold = atof(argv[optind]);
			break;
		case 'b':
			file_binary = 1;
			break;
		case 'v':
			verbose++;
			break;
		case 'f':
			force_N_id_2 = atoi(argv[optind]);
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

int base_init() {
	file_data_type_t type = file_binary?COMPLEX_FLOAT_BIN:COMPLEX_FLOAT;
	if (filesource_init(&fsrc, input_file_name, type)) {
		fprintf(stderr, "Error opening file %s\n", input_file_name);
		exit(-1);
	}

	input_buffer = malloc(4 * 960 * sizeof(cf_t));
	if (!input_buffer) {
		perror("malloc");
		exit(-1);
	}
	fft_buffer = malloc(CPNORM_NSYMB * 128 * sizeof(cf_t));
	if (!fft_buffer) {
		perror("malloc");
		return -1;
	}

	/* Init FFT plan */
	if (dft_plan_c2c(128, FORWARD, &fft_plan)) {
		fprintf(stderr, "Error initiating FFT plan\n");
		return -1;
	}
	fft_plan.options = DFT_DC_OFFSET | DFT_MIRROR_POS | DFT_NORMALIZE;

	DEBUG("Memory init OK\n",0);
	return 0;
}



int mib_decoder_init(int cell_id) {

	/*
	if (chest_LTEDL_init(&chest, ntime, nfreq, CPNORM_NSYMB, cell_id, 6)) {
		fprintf(stderr, "Error initiating LTE equalizer\n");
		return -1;
	}
	*/

	DEBUG("Channel estimation initiated ntime=%d nfreq=%d\n", ntime, nfreq);

	if (pbch_init(&pbch, cell_id, CPNORM)) {
		fprintf(stderr, "Error initiating PBCH\n");
		return -1;
	}
	DEBUG("PBCH initiated cell_id=%d\n", cell_id);
	return 0;
}

void fft_run_slot(dft_plan_t *fft_plan, cf_t *input, cf_t *output) {
	int i;
	for (i=0;i<CPNORM_NSYMB;i++) {
		DEBUG("Running FFT %d\n", i);
		input += CP_NORM(i, 128);
		dft_run_c2c(fft_plan, input, output);
		input += 128;
		output += 128;
	}
}

int mib_decoder_run(cf_t *input, pbch_mib_t *mib) {
	fft_run_slot(&fft_plan, input, fft_buffer);
	DEBUG("Decoding PBCH\n", 0);
	return pbch_decode(&pbch, fft_buffer, mib, 6, 1);
}

int get_samples(int length, int offset) {
	int n = 0;
	if (length != -1 && offset != -1) {
		while(n < length) {
			DEBUG("Reading %d samples offset=%d\n", length - n, offset + n);
			n = filesource_read(&fsrc, &input_buffer[offset + n], length - n);
			if (n == -1) {
				fprintf(stderr, "Error reading %d samples from file\n", length - n);
				break;
			} else if (n == 0) {
				printf("End of file\n");
				return -1;
			}
		}
		return n;
	} else {
		return -1;
	}
}

enum radio_state { DONE, SYNC, MIB};

int main(int argc, char **argv) {
	enum radio_state state;
	int sf_size, slot_start;
	int read_length, slot_idx;
	int mib_attempts;
	pbch_mib_t mib;
	int cell_id;
	int idx;
	int frame_cnt;
	int read_offset;
	float cfo;

	if (argc < 3) {
		usage(argv[0]);
		exit(-1);
	}

	parse_args(argc,argv);

	if (base_init()) {
		fprintf(stderr, "Error initializing memory\n");
		exit(-1);
	}

	if (sync_init(&synch, 960)) {
		fprintf(stderr, "Error initiating PSS/SSS\n");
		exit(-1);
	}

	sync_force_N_id_2(&synch, force_N_id_2);
	sync_set_threshold(&synch, corr_peak_threshold);

	state = SYNC;
	sf_size = 960;
	read_length = sf_size;
	slot_start = 0;
	slot_idx = 0;
	mib_attempts = 0;
	frame_cnt = -1;
	read_offset = 0;
	cfo = 0.0;

	printf("\n\n-- Initiating MIB search --\n\n");

	while(state != DONE && frame_cnt < nof_slots) {
		if (get_samples(read_length, read_offset) == -1) {
			fprintf(stderr, "Error reading %d samples sf_start=%d\n", read_length, slot_start);
			break;
		}
		if (read_length) {
			frame_cnt++;
			INFO("\n\tSlot idx=%d\n\n", slot_idx);
			INFO("Correcting CFO=%.4f\n", cfo);
			nco_cexp_f_direct(&input_buffer[read_offset], -cfo/128, read_length);
		}
		switch(state) {
		case SYNC:
			INFO("State Sync, Slot idx=%d\n", slot_idx);
			idx = sync_run(&synch, input_buffer, read_offset);
			if (idx != -1) {
				slot_start = read_offset + idx;
				read_length = idx;
				read_offset += 960;
				cell_id = sync_get_cell_id(&synch);
				cfo = sync_get_cfo(&synch);
				slot_idx = sync_get_slot_id(&synch);
				state = MIB;
				if (mib_decoder_init(cell_id)) {
					fprintf(stderr, "Error initiating MIB decoder\n");
					exit(-1);
				}
				INFO("SYNC done, cell_id=%d slot_start=%d\n", cell_id, slot_start);
			} else {
				read_offset = 960;
				memcpy(input_buffer, &input_buffer[960], 960 * sizeof(cf_t));
			}
			break;
		case MIB:
			read_length = 960;
			read_offset = slot_start;
			INFO("State MIB, Slot idx=%d\n", slot_idx);
			if (slot_idx == 1) {
				INFO("Trying to find MIB offset %d\n", slot_start);
				if (mib_decoder_run(&input_buffer[slot_start], &mib)) {
					INFO("MIB detected attempt=%d\n", mib_attempts);
					state = DONE;
				} else {
					INFO("MIB not detected attempt=%d\n", mib_attempts);
					if (mib_attempts >= 4) {
						state = SYNC;
					}
				}
				mib_attempts++;
			}
			break;
		case DONE:
			INFO("State Done, Slot idx=%d\n", slot_idx);
			pbch_mib_fprint(stdout, &mib);
			printf("Done\n");
			break;
		}

		if (read_length) {
			slot_idx++;
			if (slot_idx == 20) {
				slot_idx = 0;
			}
		}
	}

	sync_free(&synch);
	filesource_close(&fsrc);

	free(input_buffer);

	printf("Done\n");
	exit(0);
}
