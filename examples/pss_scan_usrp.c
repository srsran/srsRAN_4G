#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>

#include "lte.h"

#include "uhd.h"
#include "uhd_utils.h"

int nof_slots=1000;
int band, earfcn=-1;
float pss_threshold=15.0;
int earfcn_start, earfcn_end = -1;
float rssi_threshold = -42.0;

cf_t *input_buffer;
float *cfo_v;
int *idx_v;
float *p2a_v;
void *uhd;
int nof_bands;
int force_N_id_2;
float gain = 30.0;

#define MAX_EARFCN 1000
lte_earfcn_t channels[MAX_EARFCN];
float rssi[MAX_EARFCN];
float freqs[MAX_EARFCN];
float cfo[MAX_EARFCN];
float p2a[MAX_EARFCN];

#define MHZ 			1000000
#define SAMP_FREQ 		1920000
#define RSSI_FS			1000000
#define RSSI_NSAMP		50000
#define FLEN 			9600
#define FLEN_PERIOD		0.005

#define IS_SIGNAL(i) (10*log10f(rssi[i]) + 30 > rssi_threshold)

void print_to_matlab();

void usage(char *prog) {
	printf("Usage: %s [senvtr] -b band\n", prog);
	printf("\t-s earfcn_start [Default %d]\n", earfcn_start);
	printf("\t-e earfcn_end [Default All]\n");
	printf("\t-n number of frames [Default %d]\n", nof_slots);
	printf("\t-v [set verbose to debug, default none]\n");
	printf("\t-t pss_threshold [Default %.2f]\n", pss_threshold);
	printf("\t-r rssi_threshold [Default %.2f dBm]\n", rssi_threshold);
	printf("\t-f force_N_id_2 [Default no]\n");
	printf("\t-g gain [Default no %.2f dB]\n", gain);
}

void parse_args(int argc, char **argv) {
	int opt;
	while ((opt = getopt(argc, argv, "gfrtbsenv")) != -1) {
		switch(opt) {
		case 'g':
			gain = atof(argv[optind]);
			break;
		case 'f':
			force_N_id_2 = atoi(argv[optind]);
			break;
		case 't':
			pss_threshold = atof(argv[optind]);
			break;
		case 'r':
			rssi_threshold = -atof(argv[optind]);
			break;
		case 'b':
			band = atoi(argv[optind]);
			break;
		case 's':
			earfcn_start = atoi(argv[optind]);
			break;
		case 'e':
			earfcn_end = atoi(argv[optind]);
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
}

int base_init(int frame_length) {

	input_buffer = malloc(2 * frame_length * sizeof(cf_t));
	if (!input_buffer) {
		perror("malloc");
		exit(-1);
	}

	idx_v = malloc(nof_slots * sizeof(int));
	if (!idx_v) {
		perror("malloc");
		exit(-1);
	}
	cfo_v = malloc(nof_slots * sizeof(float));
	if (!cfo_v) {
		perror("malloc");
		exit(-1);
	}
	p2a_v = malloc(nof_slots * sizeof(float));
	if (!p2a_v) {
		perror("malloc");
		exit(-1);
	}

	bzero(cfo, sizeof(float) * MAX_EARFCN);
	bzero(p2a, sizeof(float) * MAX_EARFCN);

	/* open UHD device */
	printf("Opening UHD device...\n");
	if (uhd_open("",&uhd)) {
		fprintf(stderr, "Error opening uhd\n");
		exit(-1);
	}

	return 0;
}

void base_free() {

	uhd_close(&uhd);
	free(input_buffer);
	free(idx_v);
	free(cfo_v);
	free(p2a_v);
}

float mean_valid(int *idx_v, float *x, int nof_frames) {
	int i;
	float mean = 0;
	int n = 0;
	for (i=0;i<nof_frames;i++) {
		if (idx_v[i] != -1) {
			mean += x[i];
			n++;
		}
	}
	if (n > 0) {
		return mean/n;
	} else {
		return 0.0;
	}
}

int main(int argc, char **argv) {
	int frame_cnt;
	int i;
	int nsamples;
	int cell_id;
	sync_t synch;
	float max_peak_to_avg;
	float sfo;

	if (argc < 3) {
		usage(argv[0]);
		exit(-1);
	}

	parse_args(argc,argv);

	if (base_init(FLEN)) {
		fprintf(stderr, "Error initializing memory\n");
		exit(-1);
	}

	if (sync_init(&synch)) {
		fprintf(stderr, "Error initiating PSS/SSS\n");
		exit(-1);
	}
	sync_set_threshold(&synch, pss_threshold);
	sync_pss_det_peakmean(&synch);

	if (force_N_id_2 != -1) {
		sync_force_N_id_2(&synch, force_N_id_2);
	}

	nof_bands = lte_band_get_fd_band(band, channels, earfcn_start, earfcn_end, MAX_EARFCN);
	printf("RSSI scan: %d freqs in band %d\n", nof_bands, band);
	for (i=0;i<nof_bands;i++) {
		freqs[i] = channels[i].fd * MHZ;
	}


	if (uhd_rssi_scan(uhd, freqs, rssi, nof_bands, (double) RSSI_FS, RSSI_NSAMP)) {
		fprintf(stderr, "Error while doing RSSI scan\n");
		exit(-1);
	}

	printf("\nDone. Starting PSS search\n");
	usleep(500000);
	printf("Setting sampling frequency %.2f MHz\n", (float) SAMP_FREQ/MHZ);
	uhd_set_rx_srate(uhd, SAMP_FREQ);

	uhd_set_rx_gain(uhd, gain);

	print_to_matlab();

	int first = 1;
	for (i=0;i<nof_bands;i++) {
		/* scan only bands above rssi_threshold */
		if (IS_SIGNAL(i)) {
			uhd_set_rx_freq(uhd, (double) channels[i].fd * MHZ);
			uhd_rx_wait_lo_locked(uhd);

			if (first) {
				INFO("Starting receiver...\n",0);
				uhd_start_rx_stream(uhd);
				first = 0;
			}

			frame_cnt = 0;
			nsamples = 0;
			max_peak_to_avg = -99;
			nsamples += uhd_recv(uhd, input_buffer, FLEN, 1);
			cell_id = -1;
			while(frame_cnt < nof_slots) {
				if (frame_cnt) {
					nsamples += uhd_recv(uhd, &input_buffer[FLEN], FLEN, 1);
				}

				idx_v[frame_cnt] = sync_run(&synch, input_buffer, frame_cnt?FLEN:0);
				p2a_v[frame_cnt] = sync_get_peak_to_avg(&synch);
				if (idx_v[frame_cnt] != -1) {
					/* save cell id for the best peak-to-avg */
					if (p2a_v[frame_cnt] > max_peak_to_avg) {
						max_peak_to_avg = p2a_v[frame_cnt];
						cell_id = sync_get_cell_id(&synch);
					}
					cfo_v[frame_cnt] = sync_get_cfo(&synch);
				} else {
					cfo_v[frame_cnt] = 0.0;
				}
				if (frame_cnt) {
					memcpy(input_buffer, &input_buffer[FLEN], FLEN * sizeof(cf_t));
				}
				if (VERBOSE_ISINFO()) {
					printf("[%4d] - idx: %5d\tpeak-to-avg: %3.2f\tcfo=%.3f\r", frame_cnt,
						idx_v[frame_cnt], p2a_v[frame_cnt], cfo_v[frame_cnt]);
				}
				frame_cnt++;
			}

			cfo[i] = mean_valid(idx_v, cfo_v, nof_slots);
			p2a[i] = sum_r(p2a_v, nof_slots) / nof_slots;
			if (channels[i].id == 1900
					|| channels[i].id == 1901) {
				vec_fprint_i(stdout, idx_v, nof_slots);
			}

			sfo = sfo_estimate(idx_v, nof_slots, FLEN_PERIOD);
			if (VERBOSE_ISINFO()) {
				printf("\n");
			}

			printf("[%3d/%d]: EARFCN %d Freq. %.2f MHz, "
					"RSSI %3.2f dBm, PSS %2.2f dB, CFO=%+2.1f KHz, SFO=%+2.1f KHz, CELL_ID=%3d\n", i, nof_bands,
							channels[i].id, channels[i].fd, 10*log10f(rssi[i]) + 30,
							10*log10f(p2a[i]), cfo[i] * 15, sfo / 1000, cell_id);
			print_to_matlab();

		} else {
			INFO("[%3d/%d]: EARFCN %d Freq. %.2f MHz. RSSI below threshold (%3.2f < %3.2f dBm)\n",
					i, nof_bands, channels[i].id, channels[i].fd, 10*log10f(rssi[i]) + 30, rssi_threshold);
		}
	}

	print_to_matlab();

	sync_free(&synch);
	base_free();

	printf("Done\n");
	exit(0);
}

void print_to_matlab() {
	int i;

	FILE *f = fopen("output.m", "w");
	if (!f) {
		perror("fopen");
		exit(-1);
	}
	fprintf(f, "fd=[");
	for (i=0;i<nof_bands;i++) {
		fprintf(f, "%g, ", channels[i].fd);
	}
	fprintf(f, "];\n");

	fprintf(f, "rssi=[");
	for (i=0;i<nof_bands;i++) {
		fprintf(f, "%g, ", rssi[i]);
	}
	fprintf(f, "];\n");

	/*
	fprintf(f, "cfo=[");
	for (i=0;i<nof_bands;i++) {
		if (IS_SIGNAL(i)) {
			fprintf(f, "%g, ", cfo[i]);
		} else {
			fprintf(f, "NaN, ");
		}
	}
	fprintf(f, "];\n");
*/
	fprintf(f, "p2a=[");
	for (i=0;i<nof_bands;i++) {
		if (IS_SIGNAL(i)) {
			fprintf(f, "%g, ", p2a[i]);
		} else {
			fprintf(f, "0, ");
		}
	}
	fprintf(f, "];\n");
	fprintf(f, "clf;\n\n");
	fprintf(f, "subplot(1,2,1)\n");
	fprintf(f, "plot(fd, 10*log10(rssi)+30)\n");
	fprintf(f, "grid on; xlabel('f [Mhz]'); ylabel('RSSI [dBm]');\n");
	fprintf(f, "title('RSSI Estimation')\n");

	fprintf(f, "subplot(1,2,2)\n");
	fprintf(f, "plot(fd, p2a)\n");
	fprintf(f, "grid on; xlabel('f [Mhz]'); ylabel('Peak-to-Avg [dB]');\n");
	fprintf(f, "title('PSS Correlation')\n");
/*
	fprintf(f, "subplot(1,3,3)\n");
	fprintf(f, "plot(fd, cfo)\n");
	fprintf(f, "grid on; xlabel('f [Mhz]'); ylabel(''); axis([min(fd) max(fd) -0.5 0.5]);\n");
	fprintf(f, "title('CFO Estimation')\n");
	*/
	fprintf(f, "drawnow;\n");
	fclose(f);
}
