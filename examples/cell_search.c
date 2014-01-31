#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "lte.h"

char *input_file_name = NULL;
int nof_slots=100;
float corr_peak_threshold=15;
int file_binary = 0;
int force_N_id_2=-1;
int nof_ports = 1;


#define FLEN	9600

filesource_t fsrc;
cf_t *input_buffer, *fft_buffer, *ce[MAX_PORTS];
pbch_t pbch;
lte_fft_t fft;
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
	int i;

	file_data_type_t type = file_binary?COMPLEX_FLOAT_BIN:COMPLEX_FLOAT;
	if (filesource_init(&fsrc, input_file_name, type)) {
		fprintf(stderr, "Error opening file %s\n", input_file_name);
		exit(-1);
	}

	input_buffer = malloc(4 * FLEN * sizeof(cf_t));
	if (!input_buffer) {
		perror("malloc");
		exit(-1);
	}

	fft_buffer = malloc(CPNORM_NSYMB * 72 * sizeof(cf_t));
	if (!fft_buffer) {
		perror("malloc");
		return -1;
	}

	for (i=0;i<nof_ports;i++) {
		ce[i] = malloc(CPNORM_NSYMB * 72 * sizeof(cf_t));
		if (!ce[i]) {
			perror("malloc");
			return -1;
		}
	}

	if (chest_init(&chest, CPNORM, 6, 1)) {
		fprintf(stderr, "Error initializing equalizer\n");
		return -1;
	}

	if (lte_fft_init(&fft, CPNORM, 6)) {
		fprintf(stderr, "Error initializing FFT\n");
		return -1;
	}

	DEBUG("Memory init OK\n",0);
	return 0;
}



int mib_decoder_init(int cell_id) {

	if (chest_ref_LTEDL(&chest, cell_id)) {
		fprintf(stderr, "Error initializing reference signal\n");
		return -1;
	}

	if (pbch_init(&pbch, cell_id, CPNORM)) {
		fprintf(stderr, "Error initiating PBCH\n");
		return -1;
	}
	DEBUG("PBCH initiated cell_id=%d\n", cell_id);
	return 0;
}

int mib_decoder_run(cf_t *input, pbch_mib_t *mib) {
	int i;
	lte_fft_run(&fft, input, fft_buffer);

	/* Get channel estimates for each port */
	for (i=0;i<nof_ports;i++) {
		chest_ce_slot_port(&chest, fft_buffer, ce[i], 1, 0);
	}

	DEBUG("Decoding PBCH\n", 0);
	return pbch_decode(&pbch, fft_buffer, ce, nof_ports, 6, 1, mib);
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
	int read_length, frame_idx;
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

	if (sync_init(&synch, FLEN)) {
		fprintf(stderr, "Error initiating PSS/SSS\n");
		exit(-1);
	}

	sync_force_N_id_2(&synch, force_N_id_2);
	sync_set_threshold(&synch, corr_peak_threshold);
	sync_pss_det_peakmean(&synch);

	state = SYNC;
	sf_size = FLEN;
	read_length = sf_size;
	slot_start = 0;
	frame_idx = 0;
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
			INFO("\nFrame idx=%d\n\n", frame_idx);
			INFO("Correcting CFO=%.4f\n", cfo);
			nco_cexp_f_direct(&input_buffer[read_offset], -cfo/128, read_length);
		}
		switch(state) {
		case SYNC:
			INFO("State Sync, Slot idx=%d\n", frame_idx);
			idx = sync_run(&synch, input_buffer, read_offset);
			if (idx != -1) {
				slot_start = read_offset + idx;
				read_length = idx;
				read_offset += FLEN;
				cell_id = sync_get_cell_id(&synch);
				cfo = sync_get_cfo(&synch);
				frame_idx = sync_get_slot_id(&synch)?1:0;
				state = MIB;
				if (mib_decoder_init(cell_id)) {
					fprintf(stderr, "Error initiating MIB decoder\n");
					exit(-1);
				}
				INFO("SYNC done, cell_id=%d slot_start=%d frame_idx=%d\n", cell_id, slot_start, frame_idx);
			} else {
				read_offset = FLEN;
				memcpy(input_buffer, &input_buffer[FLEN], FLEN * sizeof(cf_t));
			}
			break;
		case MIB:
			read_length = FLEN;
			read_offset = slot_start;
			INFO("State MIB, frame idx=%d\n", frame_idx);
			if (frame_idx == 0) {
				INFO("Trying to find MIB offset %d\n", slot_start);
				if (mib_decoder_run(&input_buffer[slot_start+FLEN/10], &mib)) {
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
			INFO("State Done, Slot idx=%d\n", frame_idx);
			pbch_mib_fprint(stdout, &mib);
			printf("Done\n");
			break;
		}

		if (read_length) {
			frame_idx++;
			if (frame_idx == 2) {
				frame_idx = 0;
			}
		}
	}

	sync_free(&synch);
	filesource_close(&fsrc);

	free(input_buffer);

	printf("Done\n");
	exit(0);
}
