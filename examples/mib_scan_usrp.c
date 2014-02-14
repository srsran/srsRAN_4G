#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>

#include "lte.h"

#define DISABLE_UHD

#ifndef DISABLE_UHD
#include "uhd.h"
#include "uhd_utils.h"
#endif

#define MHZ 			1000000
#define SAMP_FREQ 		1920000
#define RSSI_FS			1000000
#define FLEN 			9600
#define FLEN_PERIOD		0.005

#define RSSI_DECIM		20

#define IS_SIGNAL(i) (10*log10f(rssi[i]) + 30 > rssi_threshold)


int band, earfcn=-1;
float find_threshold = 40.0, track_threshold = 8.0;
int earfcn_start=-1, earfcn_end = -1;
float rssi_threshold = -30.0;
int max_track_lost=9;
int nof_frames_find=8, nof_frames_track=100, nof_samples_rssi=50000;
int track_len=500;
int nof_ports;

cf_t *input_buffer, *fft_buffer, *ce[MAX_PORTS];
pbch_t pbch;
lte_fft_t fft;
chest_t chest;
sync_t sfind, strack;

float *cfo_v;
int *idx_v, *idx_valid, *t;
float *p2a_v;
void *uhd;
int nof_bands;
float gain = 20.0;

#define MAX_EARFCN 1000
lte_earfcn_t channels[MAX_EARFCN];
float rssi[MAX_EARFCN];
float rssi_d[MAX_EARFCN/RSSI_DECIM];
float freqs[MAX_EARFCN];
float cfo[MAX_EARFCN];
float p2a[MAX_EARFCN];

enum sync_state {INIT, FIND, TRACK, MIB, DONE};

void print_to_matlab();

void usage(char *prog) {
	printf("Usage: %s [seRrFfTtgv] -b band\n", prog);
	printf("\t-s earfcn_start [Default All]\n");
	printf("\t-e earfcn_end [Default All]\n");
	printf("\t-R rssi_nof_samples [Default %d]\n", nof_samples_rssi);
	printf("\t-r rssi_threshold [Default %.2f dBm]\n", rssi_threshold);
	printf("\t-F pss_find_nof_frames [Default %d]\n", nof_frames_find);
	printf("\t-f pss_find_threshold [Default %.2f]\n", find_threshold);
	printf("\t-T pss_track_nof_frames [Default %d]\n", nof_frames_track);
	printf("\t-t pss_track_threshold [Default %.2f]\n", track_threshold);
	printf("\t-l pss_track_len [Default %d]\n", track_len);
	printf("\t-g gain [Default %.2f dB]\n", gain);
	printf("\t-v [set verbose to debug, default none]\n");
}

void parse_args(int argc, char **argv) {
	int opt;
	while ((opt = getopt(argc, argv, "bseRrFfTtgv")) != -1) {
		switch(opt) {
		case 'b':
			band = atoi(argv[optind]);
			break;
		case 's':
			earfcn_start = atoi(argv[optind]);
			break;
		case 'e':
			earfcn_end = atoi(argv[optind]);
			break;
		case 'R':
			nof_samples_rssi = atoi(argv[optind]);
			break;
		case 'r':
			rssi_threshold = -atof(argv[optind]);
			break;
		case 'F':
			nof_frames_find = atoi(argv[optind]);
			break;
		case 'f':
			find_threshold = atof(argv[optind]);
			break;
		case 'T':
			nof_frames_track = atoi(argv[optind]);
			break;
		case 't':
			track_threshold = atof(argv[optind]);
			break;
		case 'g':
			gain = atof(argv[optind]);
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
	int i;

	input_buffer = malloc(2 * frame_length * sizeof(cf_t));
	if (!input_buffer) {
		perror("malloc");
		return -1;
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
	if (sync_init(&sfind, FLEN)) {
		fprintf(stderr, "Error initiating PSS/SSS\n");
		return -1;
	}
	if (sync_init(&strack, track_len)) {
		fprintf(stderr, "Error initiating PSS/SSS\n");
		return -1;
	}
	if (chest_init(&chest, LINEAR, CPNORM, 6, 1)) {
		fprintf(stderr, "Error initializing equalizer\n");
		return -1;
	}

	if (lte_fft_init(&fft, CPNORM, 6)) {
		fprintf(stderr, "Error initializing FFT\n");
		return -1;
	}

	idx_v = malloc(nof_frames_track * sizeof(int));
	if (!idx_v) {
		perror("malloc");
		return -1;
	}
	idx_valid = malloc(nof_frames_track * sizeof(int));
	if (!idx_valid) {
		perror("malloc");
		return -1;
	}
	t = malloc(nof_frames_track * sizeof(int));
	if (!t) {
		perror("malloc");
		return -1;
	}
	cfo_v = malloc(nof_frames_track * sizeof(float));
	if (!cfo_v) {
		perror("malloc");
		return -1;
	}
	p2a_v = malloc(nof_frames_track * sizeof(float));
	if (!p2a_v) {
		perror("malloc");
		return -1;
	}

	bzero(cfo, sizeof(float) * MAX_EARFCN);
	bzero(p2a, sizeof(float) * MAX_EARFCN);

	/* open UHD device */
#ifndef DISABLE_UHD
	printf("Opening UHD device...\n");
	if (uhd_open("",&uhd)) {
		fprintf(stderr, "Error opening uhd\n");
		return -1;
	}
#endif
	return 0;
}

void base_free() {
	int i;

#ifndef DISABLE_UHD
	uhd_close(&uhd);
#endif
	sync_free(&sfind);
	sync_free(&strack);
	lte_fft_free(&fft);
	chest_free(&chest);

	free(input_buffer);
	free(fft_buffer);
	for (i=0;i<nof_ports;i++) {
			free(ce[i]);
	}
	free(idx_v);
	free(idx_valid);
	free(t);
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

int preprocess_idx(int *in, int *out, int *period, int len) {
	int i, n;
	n=0;
	for (i=0;i<len;i++) {
		if (in[i] != -1) {
			out[n] = in[i];
			period[n] = i;
			n++;
		}
	}
	return n;
}

int rssi_scan() {
	int n=0;
	int i;

	if (nof_bands > 100) {
		/* scan every Mhz, that is 10 freqs */
		for (i=0;i<nof_bands;i+=RSSI_DECIM) {
			freqs[n] = channels[i].fd * MHZ;
			n++;
		}
#ifndef DISABLE_UHD
		if (uhd_rssi_scan(uhd, freqs, rssi_d, n, (double) RSSI_FS, nof_samples_rssi)) {
			fprintf(stderr, "Error while doing RSSI scan\n");
			return -1;
		}
#endif
		/* linearly interpolate the rssi vector */
		interp_linear_f(rssi_d, rssi, RSSI_DECIM, n);
	} else {
		for (i=0;i<nof_bands;i++) {
			freqs[i] = channels[i].fd * MHZ;
		}
#ifndef DISABLE_UHD
		if (uhd_rssi_scan(uhd, freqs, rssi, nof_bands, (double) RSSI_FS, nof_samples_rssi)) {
			fprintf(stderr, "Error while doing RSSI scan\n");
			return -1;
		}
#endif
		n = nof_bands;
	}

	return n;
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

int main(int argc, char **argv) {
	int frame_cnt, valid_frames;
	int freq;
	int cell_id;
	float max_peak_to_avg;
	float sfo;
	int find_idx, track_idx, last_found;
	enum sync_state state;
	int n;
	int mib_attempts;
	int nslot;
	pbch_mib_t mib;

	if (argc < 3) {
		usage(argv[0]);
		exit(-1);
	}

	parse_args(argc,argv);

	if (base_init(FLEN)) {
		fprintf(stderr, "Error initializing memory\n");
		exit(-1);
	}

	sync_pss_det_peakmean(&sfind);
	sync_pss_det_peakmean(&strack);

	nof_bands = lte_band_get_fd_band(band, channels, earfcn_start, earfcn_end, MAX_EARFCN);
	printf("RSSI scan: %d freqs in band %d, RSSI threshold %.2f dBm\n", nof_bands, band, rssi_threshold);

	n = rssi_scan();
	if (n == -1) {
		exit(-1);
	}

	printf("\nDone. Starting PSS search on %d channels\n", n);
	usleep(500000);
	INFO("Setting sampling frequency %.2f MHz\n", (float) SAMP_FREQ/MHZ);
#ifndef DISABLE_UHD
	uhd_set_rx_srate(uhd, SAMP_FREQ);

	uhd_set_rx_gain(uhd, gain);
#endif

	print_to_matlab();

	freq=0;
	state = INIT;
	nslot = 0;
	sfo = 0;
	find_idx = 0;
	frame_cnt = 0;
	while(freq<nof_bands) {
		/* scan only bands above rssi_threshold */
		if (!IS_SIGNAL(freq)) {
			INFO("[%3d/%d]: Skipping EARFCN %d %.2f MHz RSSI %.2f dB\n", freq, nof_bands,
								channels[freq].id, channels[freq].fd,10*log10f(rssi[freq]) + 30);
			freq++;
		} else {
			if (state != INIT && state != DONE) {
#ifndef DISABLE_UHD
				uhd_recv(uhd, &input_buffer[FLEN], FLEN, 1);
#endif
			}
			switch(state) {
			case INIT:
				DEBUG("Stopping receiver...\n",0);
#ifndef DISABLE_UHD
				uhd_stop_rx_stream(uhd);

				/* set freq */
				uhd_set_rx_freq(uhd, (double) channels[freq].fd * MHZ);
				uhd_rx_wait_lo_locked(uhd);
				DEBUG("Set freq to %.3f MHz\n", (double) channels[freq].fd);

				DEBUG("Starting receiver...\n",0);
				uhd_start_rx_stream(uhd);
#endif
				/* init variables */
				frame_cnt = 0;
				max_peak_to_avg = -99;
				cell_id = -1;

				/* receive first frame */
#ifndef DISABLE_UHD
				uhd_recv(uhd, input_buffer, FLEN, 1);
#endif
				/* set find_threshold and go to FIND state */
				sync_set_threshold(&sfind, find_threshold);
				sync_force_N_id_2(&sfind, -1);
				state = FIND;
				break;
			case FIND:
				/* find peak in all frame */
				find_idx = sync_run(&sfind, input_buffer, FLEN);
				DEBUG("[%3d/%d]: PAR=%.2f\n", freq, nof_bands, sync_get_peak_to_avg(&sfind));
				if (find_idx != -1) {
					/* if found peak, go to track and set lower threshold */
					frame_cnt = -1;
					last_found = 0;
					max_peak_to_avg = -1;
					sync_set_threshold(&strack, track_threshold);
					sync_force_N_id_2(&strack, sync_get_N_id_2(&sfind));
					cell_id = sync_get_cell_id(&strack);
					mib_decoder_init(cell_id);

					state = TRACK;
					INFO("[%3d/%d]: EARFCN %d Freq. %.2f MHz PSS found PAR %.2f dB\n", freq, nof_bands,
												channels[freq].id, channels[freq].fd,
												10*log10f(sync_get_peak_to_avg(&sfind)));
				} else {
					if (frame_cnt >= nof_frames_find) {
						state = INIT;
						freq++;
					}
				}
				break;
			case TRACK:
				INFO("Tracking PSS find_idx %d offset %d\n", find_idx, find_idx + track_len);

				track_idx = sync_run(&strack, input_buffer, FLEN + find_idx - track_len);
				p2a_v[frame_cnt] = sync_get_peak_to_avg(&strack);

				/* save cell id for the best peak-to-avg */
				if (p2a_v[frame_cnt] > max_peak_to_avg) {
					max_peak_to_avg = p2a_v[frame_cnt];
					cell_id = sync_get_cell_id(&strack);
				}
				if (track_idx != -1) {
					cfo_v[frame_cnt] = sync_get_cfo(&strack);
					last_found = frame_cnt;
					find_idx += track_idx - track_len;
					idx_v[frame_cnt] = find_idx;
					nslot = sync_get_slot_id(&strack);
				} else {
					idx_v[frame_cnt] = -1;
					cfo_v[frame_cnt] = 0.0;
				}
				/* if we missed to many PSS it is not a cell, next freq */
				if (frame_cnt - last_found > max_track_lost) {
					INFO("\n[%3d/%d]: EARFCN %d Freq. %.2f MHz %d frames lost\n", freq, nof_bands,
							channels[freq].id, channels[freq].fd, frame_cnt - last_found);

					state = INIT;
					freq++;
				} else if (frame_cnt >= nof_frames_track) {
					state = MIB;
					nslot=(nslot+10)%20;
				}
				break;
			case MIB:
				INFO("Finding MIB at freq %.2f Mhz\n", channels[freq].fd);
				cfo[freq] = mean_valid(idx_v, cfo_v, frame_cnt);
				p2a[freq] = mean_valid(idx_v, p2a_v, frame_cnt);
				valid_frames = preprocess_idx(idx_v, idx_valid, t, frame_cnt);
				sfo = sfo_estimate_period(idx_valid, t, valid_frames, FLEN_PERIOD);

				// TODO: Correct SFO

				// Correct CFO
				INFO("Correcting CFO=%.4f\n", cfo[freq]);
				nco_cexp_f_direct(&input_buffer[FLEN], -cfo[freq]/128, FLEN);

				if (nslot == 10) {
					if (mib_decoder_run(&input_buffer[FLEN+find_idx+FLEN/10], &mib)) {
						INFO("MIB detected attempt=%d\n", mib_attempts);
						state = DONE;
					} else {
						INFO("MIB not detected attempt=%d\n", mib_attempts);
						if (mib_attempts >= 20) {
							freq++;
							state = INIT;
						}
					}
					mib_attempts++;
				} else {
					nslot = (nslot+10)%20;
				}

				break;
			case DONE:
				printf("\n[%3d/%d]: FOUND EARFCN %d Freq. %.2f MHz. "
						"PAR %2.2f dB, CFO=%+.2f KHz, SFO=%+2.3f KHz, CELL_ID=%3d\n", freq, nof_bands,
								channels[freq].id, channels[freq].fd,
								10*log10f(p2a[freq]), cfo[freq] * 15, sfo / 1000, cell_id);
				pbch_mib_fprint(stdout, &mib);
				state = INIT;
				freq++;
				break;
			}

			/** FIXME: This is not necessary at all */
			if (state == TRACK || (state == FIND && frame_cnt)) {
				memcpy(input_buffer, &input_buffer[FLEN], FLEN * sizeof(cf_t));
			}
			frame_cnt++;
		}
	}

	print_to_matlab();

	base_free();

	printf("\n\nDone\n");
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


	fprintf(f, "rssi_d=[");
	for (i=0;i<nof_bands/RSSI_DECIM;i++) {
		fprintf(f, "%g, ", rssi_d[i]);
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
