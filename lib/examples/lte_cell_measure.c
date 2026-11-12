/**
 * cell_monitor.c - LTE Cell Monitor with Robust TDD Support
 *
 * IMPROVEMENTS for reliability:
 * 1. MIB decode retry logic (3 attempts before failing)
 * 2. Longer sync timeout and graceful degradation
 * 3. Optional measurement retry on failure
 * 4. Better error reporting for debugging
 * Version 2.0 Lane Messer 
 */

#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#include "srsran/common/crash_handler.h"
#include "srsran/phy/rf/rf_utils.h"
#include "srsran/srsran.h"

#ifndef DISABLE_RF
#include "srsran/phy/rf/rf.h"
#endif

#define MHZ 1000000
#define MAX_EARFCN 1000
#define MEAS_SUBFRAMES 20
#define B210_CAL_OFFSET -65.0f

#define DEFAULT_MIN_PSR 2.0f
#define FAST_MAX_FRAMES_PSS 8
#define FAST_NOF_VALID_PSS 2
#define NORMAL_MAX_FRAMES_PSS 20
#define NORMAL_NOF_VALID_PSS 4
#define RSSI_SKIP_THRESHOLD -60.0f

/* Reliability tuning parameters */
#define MIB_DECODE_RETRIES 3          /* Number of MIB decode attempts */
#define MIB_RETRY_DELAY_US 50000      /* Delay between MIB retries */
#define FDD_SYNC_TIMEOUT 80           /* Increased from 50 */
#define FDD_SYNC_RETRY_FRAMES 5       /* Extra frames on retry */
#define MEASUREMENT_RETRIES 2         /* Retry measurement on failure */
#define MIN_VALID_MEASUREMENTS 3      /* Minimum measurements for valid result */

/* Band 48 (CBRS) definitions */
#define BAND48_EARFCN_START 55240
#define BAND48_EARFCN_END   56739
#define BAND48_FREQ_START   3550.0
#define BAND48_FREQ_END     3700.0

static volatile sig_atomic_t go_exit = 0;
static srsran_rf_t* rf_ptr = NULL;

/* CLI args */
static int band = -1;
static int earfcn_start = -1;
static int earfcn_end = -1;
static int nof_prb_force = -1;
static float rf_gain = 60.0f;
static char* rf_args = "";
static char* rf_dev = "";
static int target_pci = -1;
static bool fast_mode = false;
static float min_psr = DEFAULT_MIN_PSR;
static bool skip_empty = false;
static int max_frames_pss_override = -1;
static bool force_tdd = false;
static double manual_freq_mhz = 0.0;
static int tdd_config = 2;
static int ss_config = 7;
static bool verbose_errors = false;  /* New: verbose error output */

static cell_search_cfg_t cell_detect_config = {
    .max_frames_pbch = 64,
    .max_frames_pss = NORMAL_MAX_FRAMES_PSS,
    .nof_valid_pss_frames = NORMAL_NOF_VALID_PSS,
    .init_agc = 0,
    .force_tdd = false,
};

/* TDD Subframe Configuration Table (3GPP TS 36.211 Table 4.2-2)
 * D=0, U=1, S=2 */
static const uint8_t tdd_sf_config[7][10] = {
    {0, 2, 1, 1, 1, 0, 2, 1, 1, 1},  /* Config 0 */
    {0, 2, 1, 1, 0, 0, 2, 1, 1, 0},  /* Config 1 */
    {0, 2, 1, 0, 0, 0, 2, 1, 0, 0},  /* Config 2 */
    {0, 2, 1, 1, 1, 0, 0, 0, 0, 0},  /* Config 3 */
    {0, 2, 1, 1, 0, 0, 0, 0, 0, 0},  /* Config 4 */
    {0, 2, 1, 0, 0, 0, 0, 0, 0, 0},  /* Config 5 */
    {0, 2, 1, 1, 1, 0, 2, 1, 1, 0},  /* Config 6 */
};

/* Special Subframe DwPTS lengths in symbols */
static const uint8_t ss_dwpts_symbols[10] = {3, 9, 10, 11, 12, 3, 9, 10, 11, 6};

static bool is_tdd_dl_subframe(int tdd_cfg, int ss_cfg, uint32_t sf_idx) {
    if (tdd_cfg < 0 || tdd_cfg > 6) tdd_cfg = 2;
    if (ss_cfg < 0 || ss_cfg > 9) ss_cfg = 7;
    
    uint8_t sf_type = tdd_sf_config[tdd_cfg][sf_idx % 10];
    
    if (sf_type == 0) return true;  /* DL subframe */
    if (sf_type == 2) return (ss_dwpts_symbols[ss_cfg] >= 9);  /* Special SF */
    return false;  /* UL subframe */
}

/* Count DL subframes in one frame for the given TDD config */
static int count_dl_subframes(int tdd_cfg, int ss_cfg) {
    int count = 0;
    for (int i = 0; i < 10; i++) {
        if (is_tdd_dl_subframe(tdd_cfg, ss_cfg, i)) count++;
    }
    return count;
}

/* Generate Band 48 EARFCNs */
static int generate_band48_earfcns(srsran_earfcn_t* channels, int max_channels) {
    int count = 0;
    double common_freqs[] = {
        3560.0, 3561.0, 3562.0, 3563.0, 3564.0,
        3565.0, 3566.0, 3567.0, 3568.0, 3569.0,
        3570.0, 3571.0, 3572.0, 3573.0, 3574.0,
        3575.0, 3576.0, 3577.0, 3578.0, 3579.0,
        3580.0, 3581.0, 3582.0, 3583.0, 3584.0, 3585.0,
        3586.0, 3587.0, 3588.0, 3589.0, 3590.0,
        3600.0, 3605.0, 3610.0, 3615.0, 3620.0,
        3625.0, 3630.0, 3635.0, 3640.0, 3645.0,
        3650.0, 3655.0, 3660.0, 3665.0, 3670.0, 3675.0,
        3680.0, 3685.0, 3690.0
    };
    int num_freqs = sizeof(common_freqs) / sizeof(common_freqs[0]);
    
    for (int i = 0; i < num_freqs && count < max_channels; i++) {
        double freq = common_freqs[i];
        int earfcn = BAND48_EARFCN_START + (int)((freq - BAND48_FREQ_START) / 0.1);
        channels[count].id = earfcn;
        channels[count].fd = freq;
        count++;
    }
    return count;
}

static void usage(char* prog) {
  printf("Usage: %s -b <band> [options]\n", prog);
  printf("\nRequired:\n");
  printf("  -b <band>     LTE band number\n");
  printf("\nSearch options:\n");
  printf("  -p <pci>      Target specific PCI (faster)\n");
  printf("  -f            Fast mode\n");
  printf("  -n <frames>   PSS frames per channel\n");
  printf("  -t <psr>      Min PSR threshold (default: 2.0)\n");
  printf("  -k            Skip channels with low RSSI\n");
  printf("\nTDD options:\n");
  printf("  -T            Force TDD mode\n");
  printf("  -F <freq>     Manual frequency in MHz\n");
  printf("  -C <config>   TDD UL/DL config 0-6 (default: 2)\n");
  printf("  -S <config>   TDD special SF config 0-9 (default: 7)\n");
  printf("\nBandwidth:\n");
  printf("  -w <prb>      Force bandwidth: 6,15,25,50,75,100\n");
  printf("\nRF options:\n");
  printf("  -a <args>     Additional RF args\n");
  printf("  -d <device>   RF device name\n");
  printf("  -g <gain>     RX gain in dB (default: 60)\n");
  printf("\nRange:\n");
  printf("  -s <earfcn>   Start EARFCN\n");
  printf("  -e <earfcn>   End EARFCN\n");
  printf("\nDebug:\n");
  printf("  -V            Verbose error output\n");
  printf("\nExamples:\n");
  printf("  %s -b 2 -p 67                  # FDD Band 2\n", prog);
  printf("  %s -b 48 -p 1 -w 100           # TDD Band 48\n", prog);
  printf("  %s -b 48 -p 1 -w 100 -C 2 -S 7 # Explicit TDD config\n", prog);
}

static void parse_args(int argc, char** argv) {
  int opt;
  while ((opt = getopt(argc, argv, "a:b:C:d:e:fF:g:kn:p:s:S:t:Tw:vV")) != -1) {
    switch (opt) {
      case 'a': rf_args = optarg; break;
      case 'b': band = atoi(optarg); break;
      case 'C': tdd_config = atoi(optarg); break;
      case 'd': rf_dev = optarg; break;
      case 'e': earfcn_end = atoi(optarg); break;
      case 'f': fast_mode = true; break;
      case 'F': manual_freq_mhz = strtod(optarg, NULL); break;
      case 'g': rf_gain = strtof(optarg, NULL); break;
      case 'k': skip_empty = true; break;
      case 'n': max_frames_pss_override = atoi(optarg); break;
      case 'p': target_pci = atoi(optarg); break;
      case 's': earfcn_start = atoi(optarg); break;
      case 'S': ss_config = atoi(optarg); break;
      case 't': min_psr = strtof(optarg, NULL); break;
      case 'T': force_tdd = true; break;
      case 'w': nof_prb_force = atoi(optarg); break;
      case 'v': increase_srsran_verbose_level(); break;
      case 'V': verbose_errors = true; break;
      default: usage(argv[0]); exit(1);
    }
  }

  if (band < 0) { usage(argv[0]); exit(1); }

  if (tdd_config < 0 || tdd_config > 6) {
    fprintf(stderr, "Invalid TDD config %d\n", tdd_config);
    exit(1);
  }
  if (ss_config < 0 || ss_config > 9) {
    fprintf(stderr, "Invalid SS config %d\n", ss_config);
    exit(1);
  }

  /* Auto-enable TDD for known TDD bands */
  if (band == 38 || band == 40 || band == 41 || band == 42 || 
      band == 43 || band == 48) {
    force_tdd = true;
    printf("Auto-enabling TDD mode for band %d\n", band);
  }

  if (fast_mode) {
    cell_detect_config.max_frames_pss = FAST_MAX_FRAMES_PSS;
    cell_detect_config.nof_valid_pss_frames = FAST_NOF_VALID_PSS;
    cell_detect_config.max_frames_pbch = 32;
  }

  if (max_frames_pss_override > 0) {
    cell_detect_config.max_frames_pss = max_frames_pss_override;
  }

  cell_detect_config.force_tdd = force_tdd;

  if (nof_prb_force > 0) {
    if (nof_prb_force != 6 && nof_prb_force != 15 && nof_prb_force != 25 &&
        nof_prb_force != 50 && nof_prb_force != 75 && nof_prb_force != 100) {
      fprintf(stderr, "Invalid PRB count %d\n", nof_prb_force);
      exit(1);
    }
  }
}

static volatile sig_atomic_t signal_count = 0;

static void sig_handler(int signo) {
  if (signo == SIGINT || signo == SIGTERM || signo == SIGHUP) {
    signal_count++;
    go_exit = 1;
    
    if (signal_count == 1) {
      /* First signal - try graceful shutdown */
      fprintf(stderr, "\nSignal %d received, stopping gracefully...\n", signo);
      if (rf_ptr) { 
        srsran_rf_stop_rx_stream(rf_ptr); 
      }
    } else if (signal_count == 2) {
      /* Second signal - warn user */
      fprintf(stderr, "\nSecond signal received, forcing shutdown (send again for immediate exit)...\n");
      if (rf_ptr) { 
        srsran_rf_stop_rx_stream(rf_ptr); 
      }
    } else {
      /* Third signal - immediate exit */
      fprintf(stderr, "\nForced exit!\n");
      _exit(1);
    }
  }
}

static void setup_signal_handlers(void) {
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = sig_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGINT, &sa, NULL);
  sigaction(SIGTERM, &sa, NULL);
  sigaction(SIGHUP, &sa, NULL);
  signal(SIGPIPE, SIG_IGN);
}

static int rf_recv_wrapper(void* h, void* data, uint32_t nsamples,
                           srsran_timestamp_t* t) {
  (void)t;
  if (go_exit) return -1;
  
  /* Use timed receive to allow checking go_exit periodically */
  srsran_rf_t* rf = (srsran_rf_t*)h;
  uint32_t received = 0;
  int max_attempts = 10;  /* Prevent infinite loop */
  
  while (received < nsamples && !go_exit && max_attempts > 0) {
    int ret = srsran_rf_recv_with_time(rf, (cf_t*)data + received, 
                                        nsamples - received, 1, NULL, NULL);
    if (ret < 0) {
      if (go_exit) return -1;
      return ret;
    }
    received += ret;
    max_attempts--;
  }
  
  if (go_exit) return -1;
  return received;
}

static int rf_recv_wrapper_multi(void* h, cf_t* data[SRSRAN_MAX_CHANNELS],
                                 uint32_t nsamples, srsran_timestamp_t* t) {
  (void)t;
  if (go_exit) return -1;
  
  /* Use timed receive to allow checking go_exit periodically */
  srsran_rf_t* rf = (srsran_rf_t*)h;
  uint32_t received = 0;
  int max_attempts = 10;
  
  while (received < nsamples && !go_exit && max_attempts > 0) {
    int ret = srsran_rf_recv_with_time(rf, data[0] + received,
                                        nsamples - received, 1, NULL, NULL);
    if (ret < 0) {
      if (go_exit) return -1;
      return ret;
    }
    received += ret;
    max_attempts--;
  }
  
  if (go_exit) return -1;
  return received;
}

static long long now_ms(void) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (long long)tv.tv_sec * 1000LL + tv.tv_usec / 1000;
}

static void build_rf_args(char* buf, size_t buflen, const char* user_args) {
  snprintf(buf, buflen, "type=b200,num_recv_frames=512,recv_frame_size=8200");
  if (user_args && strlen(user_args) > 0) {
    size_t len = strlen(buf);
    snprintf(buf + len, buflen - len, ",%s", user_args);
  }
}

static float quick_rssi_check(srsran_rf_t* rf) {
  if (go_exit) return NAN;
  
  uint32_t nsamples = 1920 * 2;
  cf_t* buf = srsran_vec_cf_malloc(nsamples);
  if (!buf) return NAN;
  
  srsran_rf_start_rx_stream(rf, false);
  usleep(1000);
  
  if (go_exit) {
    free(buf);
    srsran_rf_stop_rx_stream(rf);
    return NAN;
  }
  
  int n = srsran_rf_recv_with_time(rf, buf, nsamples, 1, NULL, NULL);
  srsran_rf_stop_rx_stream(rf);
  
  if (n <= 0 || go_exit) {
    free(buf);
    return NAN;
  }
  
  float power = 0;
  for (uint32_t i = 0; i < (uint32_t)n; i++) {
    float re = crealf(buf[i]);
    float im = cimagf(buf[i]);
    power += re*re + im*im;
  }
  power /= n;
  free(buf);
  
  if (power <= 0) return -100.0f;
  return 10.0f * log10f(power);
}

static int search_single_nid2(srsran_rf_t* rf, int n_id_2, float* psr_out) {
  srsran_ue_cellsearch_t cs;
  srsran_ue_cellsearch_result_t result;
  int ret = -1;
  
  if (go_exit) return -2;
  
  if (srsran_ue_cellsearch_init(&cs, cell_detect_config.max_frames_pss,
                                rf_recv_wrapper, (void*)rf)) {
    return -2;
  }
  srsran_ue_cellsearch_set_nof_valid_frames(&cs, cell_detect_config.nof_valid_pss_frames);
  
  srsran_rf_start_rx_stream(rf, false);
  
  if (go_exit) {
    srsran_rf_stop_rx_stream(rf);
    srsran_ue_cellsearch_free(&cs);
    return -2;
  }
  
  int n = srsran_ue_cellsearch_scan_N_id_2(&cs, n_id_2, &result);
  srsran_rf_stop_rx_stream(rf);
  
  if (go_exit) {
    srsran_ue_cellsearch_free(&cs);
    return -2;
  }
  
  if (n > 0 && result.psr > min_psr) {
    ret = result.cell_id;
    if (psr_out) *psr_out = result.psr;
  }
  
  srsran_ue_cellsearch_free(&cs);
  return ret;
}

static int search_all_cells(srsran_rf_t* rf, srsran_ue_cellsearch_result_t results[3]) {
  srsran_ue_cellsearch_t cs;
  int ret = -1;
  
  if (go_exit) return -1;
  
  if (srsran_ue_cellsearch_init(&cs, cell_detect_config.max_frames_pss,
                                rf_recv_wrapper, (void*)rf)) {
    return -1;
  }
  srsran_ue_cellsearch_set_nof_valid_frames(&cs, cell_detect_config.nof_valid_pss_frames);
  
  srsran_rf_start_rx_stream(rf, false);
  
  if (go_exit) {
    srsran_rf_stop_rx_stream(rf);
    srsran_ue_cellsearch_free(&cs);
    return -1;
  }
  
  bzero(results, sizeof(srsran_ue_cellsearch_result_t) * 3);
  ret = srsran_ue_cellsearch_scan(&cs, results, NULL);
  
  srsran_rf_stop_rx_stream(rf);
  srsran_ue_cellsearch_free(&cs);
  
  if (go_exit) return -1;
  return ret;
}

/*
 * FDD measurement - uses ue_sync tracking (works reliably)
 * IMPROVED: Longer sync timeout, minimum measurement threshold
 */
static int measure_cell_fdd_internal(srsran_rf_t* rf, srsran_cell_t* cell, float* rsrp_dbm,
                            float* rsrq_db, float* snr_db, float* cfo_hz, int sync_timeout) {
  srsran_ue_sync_t ue_sync;
  srsran_ue_dl_t ue_dl;
  srsran_ue_dl_cfg_t ue_dl_cfg;
  srsran_dl_sf_cfg_t dl_sf;
  cf_t* buffer[SRSRAN_MAX_CHANNELS] = {NULL};
  int ret = -1;
  bool sync_inited = false;
  bool dl_inited = false;

  if (go_exit) return -1;

  srsran_rf_stop_rx_stream(rf);

  uint32_t sf_len = SRSRAN_SF_LEN_PRB(cell->nof_prb);
  uint32_t buf_len = sf_len * 3;

  buffer[0] = srsran_vec_cf_malloc(buf_len);
  if (!buffer[0]) {
    if (verbose_errors) fprintf(stderr, "Failed to allocate buffer\n");
    goto cleanup;
  }
  memset(buffer[0], 0, buf_len * sizeof(cf_t));

  double srate = srsran_sampling_freq_hz(cell->nof_prb);
  srsran_rf_set_rx_srate(rf, srate);
  usleep(10000);

  if (srsran_ue_sync_init_multi(&ue_sync, cell->nof_prb, false,
                                rf_recv_wrapper_multi, 1, rf)) {
    if (verbose_errors) fprintf(stderr, "ue_sync init failed\n");
    goto cleanup;
  }
  sync_inited = true;

  if (srsran_ue_sync_set_cell(&ue_sync, *cell)) {
    if (verbose_errors) fprintf(stderr, "ue_sync set_cell failed\n");
    goto cleanup;
  }

  if (srsran_ue_dl_init(&ue_dl, buffer, cell->nof_prb, 1)) {
    if (verbose_errors) fprintf(stderr, "ue_dl init failed\n");
    goto cleanup;
  }
  dl_inited = true;

  if (srsran_ue_dl_set_cell(&ue_dl, *cell)) {
    if (verbose_errors) fprintf(stderr, "ue_dl set_cell failed\n");
    goto cleanup;
  }

  bzero(&ue_dl_cfg, sizeof(ue_dl_cfg));
  ue_dl_cfg.chest_cfg.filter_coef[0] = 4;
  ue_dl_cfg.chest_cfg.filter_coef[1] = 1.0f;
  ue_dl_cfg.chest_cfg.filter_type = SRSRAN_CHEST_FILTER_GAUSS;
  ue_dl_cfg.chest_cfg.noise_alg = SRSRAN_NOISE_ALG_REFS;
  ue_dl_cfg.chest_cfg.rsrp_neighbour = false;
  ue_dl_cfg.chest_cfg.estimator_alg = SRSRAN_ESTIMATOR_ALG_AVERAGE;
  
  bzero(&dl_sf, sizeof(dl_sf));

  srsran_rf_start_rx_stream(rf, false);
  
  /* Flush initial samples */
  for (int i = 0; i < 3 && !go_exit; i++) {
    srsran_rf_recv_with_time(rf, buffer[0], sf_len, 1, NULL, NULL);
  }

  /* Wait for sync with configurable timeout */
  int sync_cnt = 0;
  while (!go_exit && sync_cnt < sync_timeout) {
    int n = srsran_ue_sync_zerocopy(&ue_sync, buffer, sf_len);
    if (n == 1) break;
    if (n < 0) {
      if (verbose_errors) fprintf(stderr, "ue_sync_zerocopy error: %d\n", n);
      goto cleanup;
    }
    sync_cnt++;
  }
  
  if (go_exit || sync_cnt >= sync_timeout) {
    if (verbose_errors && sync_cnt >= sync_timeout) {
      fprintf(stderr, "Sync timeout after %d attempts\n", sync_cnt);
    }
    goto cleanup;
  }

  float rsrp_acc = 0, rsrq_acc = 0, snr_acc = 0, cfo_acc = 0;
  int meas_cnt = 0;

  for (int i = 0; i < MEAS_SUBFRAMES && !go_exit; i++) {
    int n = srsran_ue_sync_zerocopy(&ue_sync, buffer, sf_len);
    if (n != 1) continue;

    uint32_t sfidx = srsran_ue_sync_get_sfidx(&ue_sync);
    dl_sf.tti = sfidx;

    if (srsran_ue_dl_decode_fft_estimate(&ue_dl, &dl_sf, &ue_dl_cfg) == 0) {
      if (!isnan(ue_dl.chest_res.rsrp_dbm) && !isinf(ue_dl.chest_res.rsrp_dbm)) {
        rsrp_acc += ue_dl.chest_res.rsrp_dbm;
        rsrq_acc += ue_dl.chest_res.rsrq_db;
        snr_acc += ue_dl.chest_res.snr_db;
        cfo_acc += srsran_ue_sync_get_cfo(&ue_sync);
        meas_cnt++;
      }
    }
  }

  srsran_rf_stop_rx_stream(rf);

  /* Require minimum number of valid measurements */
  if (meas_cnt >= MIN_VALID_MEASUREMENTS && !go_exit) {
    float rx_gain = srsran_rf_get_rx_gain(rf);
    *rsrp_dbm = (rsrp_acc / meas_cnt) - rx_gain + B210_CAL_OFFSET;
    *rsrq_db = rsrq_acc / meas_cnt;
    *snr_db = snr_acc / meas_cnt;
    *cfo_hz = cfo_acc / meas_cnt;
    ret = 0;
  } else if (verbose_errors && meas_cnt > 0) {
    fprintf(stderr, "Insufficient measurements: %d (need %d)\n", meas_cnt, MIN_VALID_MEASUREMENTS);
  }

cleanup:
  srsran_rf_stop_rx_stream(rf);
  if (dl_inited) srsran_ue_dl_free(&ue_dl);
  if (sync_inited) srsran_ue_sync_free(&ue_sync);
  if (buffer[0]) { free(buffer[0]); buffer[0] = NULL; }
  return ret;
}

/*
 * FDD measurement wrapper with retry logic
 */
static int measure_cell_fdd(srsran_rf_t* rf, srsran_cell_t* cell, float* rsrp_dbm,
                            float* rsrq_db, float* snr_db, float* cfo_hz) {
  int ret;
  
  /* First attempt with standard timeout */
  ret = measure_cell_fdd_internal(rf, cell, rsrp_dbm, rsrq_db, snr_db, cfo_hz, FDD_SYNC_TIMEOUT);
  
  if (ret == 0 || go_exit) return ret;
  
  /* Retry with longer timeout */
  if (verbose_errors) fprintf(stderr, "Measurement retry with extended timeout...\n");
  usleep(20000);  /* Brief pause before retry */
  ret = measure_cell_fdd_internal(rf, cell, rsrp_dbm, rsrq_db, snr_db, cfo_hz, 
                                  FDD_SYNC_TIMEOUT + FDD_SYNC_RETRY_FRAMES * 10);
  
  return ret;
}

/*
 * TDD measurement - replicates exact cell search approach
 */
static srsran_ue_sync_t tdd_ue_sync;
static srsran_ue_dl_t tdd_ue_dl;
static cf_t* tdd_sf_buffer[SRSRAN_MAX_CHANNELS] = {NULL};
static bool tdd_objects_inited = false;
static uint32_t tdd_last_cell_id = UINT32_MAX;

static void cleanup_tdd_objects(void) {
  if (tdd_objects_inited) {
    srsran_ue_dl_free(&tdd_ue_dl);
    srsran_ue_sync_free(&tdd_ue_sync);
    if (tdd_sf_buffer[0]) {
      free(tdd_sf_buffer[0]);
      tdd_sf_buffer[0] = NULL;
    }
    tdd_objects_inited = false;
    tdd_last_cell_id = UINT32_MAX;
  }
}

static int init_tdd_objects(srsran_rf_t* rf, srsran_cell_t* cell) {
  const uint32_t meas_prb = 6;
  uint32_t sf_len = SRSRAN_SF_LEN_PRB(meas_prb);

  /* If already initialized for this cell, just reset */
  if (tdd_objects_inited && tdd_last_cell_id == cell->id) {
    srsran_ue_sync_reset(&tdd_ue_sync);
    return 0;
  }

  /* Clean up any existing objects */
  cleanup_tdd_objects();

  /* Allocate subframe buffer */
  tdd_sf_buffer[0] = srsran_vec_cf_malloc(sf_len * 5);
  if (!tdd_sf_buffer[0]) {
    return -1;
  }

  /* Initialize ue_sync in SEARCH mode */
  if (srsran_ue_sync_init(&tdd_ue_sync, meas_prb, true, rf_recv_wrapper, (void*)rf)) {
    free(tdd_sf_buffer[0]);
    tdd_sf_buffer[0] = NULL;
    return -1;
  }

  /* Set cell with TDD frame type */
  srsran_cell_t search_cell;
  bzero(&search_cell, sizeof(search_cell));
  search_cell.id = SRSRAN_CELL_ID_UNKNOWN;
  search_cell.nof_prb = meas_prb;
  search_cell.cp = SRSRAN_CP_NORM;
  search_cell.frame_type = SRSRAN_TDD;

  if (srsran_ue_sync_set_cell(&tdd_ue_sync, search_cell)) {
    srsran_ue_sync_free(&tdd_ue_sync);
    free(tdd_sf_buffer[0]);
    tdd_sf_buffer[0] = NULL;
    return -1;
  }

  /* Set N_id_2 and frame type */
  srsran_ue_sync_set_N_id_2(&tdd_ue_sync, cell->id % 3);
  srsran_ue_sync_set_frame_type(&tdd_ue_sync, SRSRAN_TDD);

  /* Create measurement cell for ue_dl */
  srsran_cell_t meas_cell;
  bzero(&meas_cell, sizeof(meas_cell));
  meas_cell.id = cell->id;
  meas_cell.nof_prb = meas_prb;
  meas_cell.nof_ports = cell->nof_ports;
  meas_cell.cp = SRSRAN_CP_NORM;
  meas_cell.frame_type = SRSRAN_TDD;
  meas_cell.phich_length = SRSRAN_PHICH_NORM;
  meas_cell.phich_resources = SRSRAN_PHICH_R_1;

  /* Initialize UE DL */
  if (srsran_ue_dl_init(&tdd_ue_dl, tdd_sf_buffer, meas_prb, 1)) {
    srsran_ue_sync_free(&tdd_ue_sync);
    free(tdd_sf_buffer[0]);
    tdd_sf_buffer[0] = NULL;
    return -1;
  }

  if (srsran_ue_dl_set_cell(&tdd_ue_dl, meas_cell)) {
    srsran_ue_dl_free(&tdd_ue_dl);
    srsran_ue_sync_free(&tdd_ue_sync);
    free(tdd_sf_buffer[0]);
    tdd_sf_buffer[0] = NULL;
    return -1;
  }

  tdd_objects_inited = true;
  tdd_last_cell_id = cell->id;
  return 0;
}

/*
 * TDD measurement internal - single attempt with configurable timeout
 */
static int measure_cell_tdd_internal(srsran_rf_t* rf, srsran_cell_t* cell, float* rsrp_dbm,
                                     float* rsrq_db, float* snr_db, float* cfo_hz, 
                                     int max_sync_attempts) {
  srsran_ue_dl_cfg_t ue_dl_cfg;
  srsran_dl_sf_cfg_t dl_sf;
  int ret = -1;

  if (go_exit) return -1;

  srsran_rf_stop_rx_stream(rf);

  const uint32_t meas_prb = 6;
  uint32_t sf_len = SRSRAN_SF_LEN_PRB(meas_prb);

  /* Set sample rate to cell search rate */
  srsran_rf_set_rx_srate(rf, SRSRAN_CS_SAMP_FREQ);
  usleep(10000);

  /* Initialize or reset TDD objects */
  if (init_tdd_objects(rf, cell) != 0) {
    return -1;
  }

  /* Configure channel estimator */
  bzero(&ue_dl_cfg, sizeof(ue_dl_cfg));
  ue_dl_cfg.chest_cfg.filter_coef[0] = 4;
  ue_dl_cfg.chest_cfg.filter_coef[1] = 1.0f;
  ue_dl_cfg.chest_cfg.filter_type = SRSRAN_CHEST_FILTER_GAUSS;
  ue_dl_cfg.chest_cfg.noise_alg = SRSRAN_NOISE_ALG_REFS;
  ue_dl_cfg.chest_cfg.rsrp_neighbour = false;
  ue_dl_cfg.chest_cfg.estimator_alg = SRSRAN_ESTIMATOR_ALG_AVERAGE;

  /* Configure TDD subframe */
  bzero(&dl_sf, sizeof(dl_sf));
  dl_sf.tdd_config.configured = true;
  dl_sf.tdd_config.sf_config = tdd_config;
  dl_sf.tdd_config.ss_config = ss_config;

  /* Start RF stream */
  srsran_rf_start_rx_stream(rf, false);

  float rsrp_acc = 0, rsrq_acc = 0, snr_acc = 0, cfo_acc = 0;
  int meas_cnt = 0;
  int sync_attempts = 0;

  while (!go_exit && meas_cnt < MEAS_SUBFRAMES && sync_attempts < max_sync_attempts) {
    int sync_ret = srsran_ue_sync_zerocopy(&tdd_ue_sync, tdd_sf_buffer, sf_len * 5);
    
    if (sync_ret < 0) {
      if (verbose_errors) fprintf(stderr, "TDD sync error: %d\n", sync_ret);
      break;
    } else if (sync_ret == 1) {
      uint32_t sf_idx = srsran_ue_sync_get_sfidx(&tdd_ue_sync);
      
      if (is_tdd_dl_subframe(tdd_config, ss_config, sf_idx)) {
        dl_sf.tti = sf_idx;

        if (srsran_ue_dl_decode_fft_estimate(&tdd_ue_dl, &dl_sf, &ue_dl_cfg) == 0) {
          float rsrp = tdd_ue_dl.chest_res.rsrp_dbm;
          float rsrq = tdd_ue_dl.chest_res.rsrq_db;
          float snr = tdd_ue_dl.chest_res.snr_db;
          
          if (!isnan(rsrp) && !isinf(rsrp)) {
            rsrp_acc += rsrp;
            rsrq_acc += rsrq;
            snr_acc += snr;
            cfo_acc += srsran_ue_sync_get_cfo(&tdd_ue_sync);
            meas_cnt++;
          }
        }
      }
    }
    sync_attempts++;
  }

  srsran_rf_stop_rx_stream(rf);

  /* Require minimum number of valid measurements */
  if (meas_cnt >= MIN_VALID_MEASUREMENTS && !go_exit) {
    float rx_gain = srsran_rf_get_rx_gain(rf);
    *rsrp_dbm = (rsrp_acc / meas_cnt) - rx_gain + B210_CAL_OFFSET;
    *rsrq_db = rsrq_acc / meas_cnt;
    *snr_db = snr_acc / meas_cnt;
    *cfo_hz = cfo_acc / meas_cnt;
    ret = 0;
  } else if (verbose_errors && meas_cnt > 0) {
    fprintf(stderr, "TDD: Insufficient measurements: %d (need %d)\n", meas_cnt, MIN_VALID_MEASUREMENTS);
  }

  return ret;
}

/*
 * TDD measurement wrapper with retry logic (mirrors FDD approach)
 */
#define TDD_SYNC_TIMEOUT 40
#define TDD_SYNC_RETRY_EXTRA 20

static int measure_cell_tdd(srsran_rf_t* rf, srsran_cell_t* cell, float* rsrp_dbm,
                            float* rsrq_db, float* snr_db, float* cfo_hz) {
  int ret;
  
  /* First attempt with standard timeout */
  ret = measure_cell_tdd_internal(rf, cell, rsrp_dbm, rsrq_db, snr_db, cfo_hz, TDD_SYNC_TIMEOUT);
  
  if (ret == 0 || go_exit) return ret;
  
  /* Retry with longer timeout and fresh sync reset */
  if (verbose_errors) fprintf(stderr, "TDD measurement retry with extended timeout...\n");
  usleep(30000);  /* Longer pause for TDD to let frame timing settle */
  
  /* Force re-init of TDD objects to get fresh sync */
  tdd_last_cell_id = UINT32_MAX;
  
  ret = measure_cell_tdd_internal(rf, cell, rsrp_dbm, rsrq_db, snr_db, cfo_hz, 
                                  TDD_SYNC_TIMEOUT + TDD_SYNC_RETRY_EXTRA);
  
  return ret;
}

/* Wrapper that calls correct measurement function based on frame type */
static int measure_cell(srsran_rf_t* rf, srsran_cell_t* cell, float* rsrp_dbm,
                        float* rsrq_db, float* snr_db, float* cfo_hz) {
  if (cell->frame_type == SRSRAN_TDD) {
    return measure_cell_tdd(rf, cell, rsrp_dbm, rsrq_db, snr_db, cfo_hz);
  } else {
    return measure_cell_fdd(rf, cell, rsrp_dbm, rsrq_db, snr_db, cfo_hz);
  }
}

/*
 * MIB decode with retry logic
 * IMPROVED: Multiple attempts before giving up
 */
static int decode_mib_with_retry(srsran_rf_t* rf, srsran_cell_t* cell, 
                                  double freq_mhz, int max_retries) {
  for (int attempt = 0; attempt < max_retries && !go_exit; attempt++) {
    srsran_rf_set_rx_freq(rf, 0, freq_mhz * MHZ);
    
    if (attempt > 0) {
      usleep(MIB_RETRY_DELAY_US);
      if (verbose_errors) {
        fprintf(stderr, "  MIB retry %d/%d...\n", attempt + 1, max_retries);
      }
    }

    int mib_ret = rf_mib_decoder(rf, 1, &cell_detect_config, cell, NULL);
    srsran_rf_stop_rx_stream(rf);

    if (go_exit) return -1;

    bool valid_prb = (cell->nof_prb == 6 || cell->nof_prb == 15 || cell->nof_prb == 25 ||
                      cell->nof_prb == 50 || cell->nof_prb == 75 || cell->nof_prb == 100);

    if (mib_ret == SRSRAN_UE_MIB_FOUND && valid_prb) {
      return 0;  /* Success */
    }
  }
  
  return -1;  /* Failed after all retries */
}

typedef struct {
  int earfcn;
  double freq;
  int pci;
  int nof_prb;
  int nof_ports;
  srsran_cp_t cp;
  srsran_frame_type_t frame_type;
  bool valid;
  int consecutive_failures;  /* Track measurement failures */
} found_cell_t;

#define MAX_CELLS 64
#define MAX_CONSECUTIVE_FAILURES 5  /* Mark cell invalid after this many failures */

int main(int argc, char** argv) {
  srsran_rf_t rf;
  srsran_ue_cellsearch_result_t search_results[3];
  srsran_earfcn_t channels[MAX_EARFCN];
  found_cell_t cells[MAX_CELLS];
  int nof_freqs;
  int nof_cells = 0;
  bool target_found = false;
  char args_buf[512];
  long long scan_start_ms;

  srsran_debug_handle_crash(argc, argv);
  parse_args(argc, argv);
  setup_signal_handlers();

  printf("=== LTE Cell Monitor ===\n");
  printf("Band: %d%s\n", band, force_tdd ? " (TDD)" : " (FDD)");
  if (force_tdd) {
    printf("TDD UL/DL Config: %d, Special SF Config: %d\n", tdd_config, ss_config);
    printf("DL subframes per frame: %d\n", count_dl_subframes(tdd_config, ss_config));
  }
  if (target_pci >= 0) {
    printf("Target PCI: %d (N_id_2 = %d)\n", target_pci, target_pci % 3);
  }
  printf("Mode: %s\n", fast_mode ? "FAST" : "NORMAL");
  printf("PSS frames/channel: %d\n", cell_detect_config.max_frames_pss);
  printf("Min PSR: %.1f\n", min_psr);
  if (nof_prb_force > 0) {
    printf("Forced BW: %d PRB\n", nof_prb_force);
  }
  printf("\n");

  printf("Opening RF device...\n");
  
  build_rf_args(args_buf, sizeof(args_buf), rf_args);
  
  if (srsran_rf_open_devname(&rf, rf_dev, args_buf, 1)) {
    fprintf(stderr, "ERROR: Failed to open RF device\n");
    return -1;
  }

  rf_ptr = &rf;
  srsran_rf_set_rx_gain(&rf, rf_gain);
  printf("RF device '%s' successfully opened\n", srsran_rf_name(&rf));

  /* Get EARFCN list for band */
  if (manual_freq_mhz > 0) {
    channels[0].id = 0;
    channels[0].fd = manual_freq_mhz;
    nof_freqs = 1;
    printf("Using manual frequency: %.2f MHz\n", manual_freq_mhz);
  } else if (band == 48) {
    nof_freqs = generate_band48_earfcns(channels, MAX_EARFCN);
    printf("Generated %d Band 48 (CBRS) channels\n", nof_freqs);
  } else {
    nof_freqs = srsran_band_get_fd_band(band, channels, earfcn_start, earfcn_end, MAX_EARFCN);
  }
  
  if (nof_freqs <= 0) {
    fprintf(stderr, "ERROR: No EARFCNs for band %d\n", band);
    srsran_rf_close(&rf);
    return -1;
  }

  printf("Scanning %d channels...\n", nof_freqs);
  scan_start_ms = now_ms();

  /* Main frequency scan loop */
  for (int i = 0; i < nof_freqs && !go_exit && !target_found; i++) {
    int earfcn = channels[i].id;
    double freq = channels[i].fd;

    printf("\r[%d/%d] EARFCN %d (%.2f MHz)...", i+1, nof_freqs, earfcn, freq);
    fflush(stdout);

    srsran_rf_set_rx_freq(&rf, 0, freq * MHZ);
    srsran_rf_set_rx_srate(&rf, SRSRAN_CS_SAMP_FREQ);

    if (skip_empty && !go_exit) {
      float rssi = quick_rssi_check(&rf);
      if (!isnan(rssi) && rssi < RSSI_SKIP_THRESHOLD) {
        continue;
      }
    }

    if (go_exit) break;

    int found_pci = -1;
    float found_psr = 0;
    srsran_cp_t found_cp = SRSRAN_CP_NORM;
    srsran_frame_type_t found_ft = force_tdd ? SRSRAN_TDD : SRSRAN_FDD;

    if (target_pci >= 0) {
      int n_id_2 = target_pci % 3;
      int pci = search_single_nid2(&rf, n_id_2, &found_psr);
      
      if (pci == target_pci) {
        found_pci = pci;
      }
    } else {
      int ret = search_all_cells(&rf, search_results);
      
      if (ret > 0) {
        int best = -1;
        float best_psr = 0;
        for (int j = 0; j < 3; j++) {
          if (search_results[j].psr > best_psr && search_results[j].psr > min_psr) {
            best_psr = search_results[j].psr;
            best = j;
          }
        }

        if (best >= 0) {
          found_pci = search_results[best].cell_id;
          found_psr = search_results[best].psr;
          found_cp = search_results[best].cp;
          found_ft = search_results[best].frame_type;
        }
      }
    }

    if (found_pci >= 0 && nof_cells < MAX_CELLS) {
      bool dup = false;
      for (int k = 0; k < nof_cells; k++) {
        if (cells[k].pci == found_pci && abs(cells[k].earfcn - earfcn) < 10) {
          dup = true;
          break;
        }
      }

      if (!dup) {
        cells[nof_cells].earfcn = earfcn;
        cells[nof_cells].freq = freq;
        cells[nof_cells].pci = found_pci;
        cells[nof_cells].cp = found_cp;
        cells[nof_cells].frame_type = force_tdd ? SRSRAN_TDD : found_ft;
        cells[nof_cells].nof_prb = 0;
        cells[nof_cells].nof_ports = 1;
        cells[nof_cells].valid = true;
        cells[nof_cells].consecutive_failures = 0;

        printf("\n  >> Found PCI %d, PSR=%.2f (%s)\n", found_pci, found_psr,
               cells[nof_cells].frame_type == SRSRAN_TDD ? "TDD" : "FDD");
        nof_cells++;

        if (target_pci >= 0 && found_pci == target_pci) {
          printf("  >> Target PCI found! Stopping scan.\n");
          target_found = true;
        }
      }
    }
  }

  long long scan_time_ms = now_ms() - scan_start_ms;
  printf("\n\nScan complete in %.1f seconds\n", scan_time_ms / 1000.0);

  if (go_exit) {
    printf("Scan interrupted.\n");
    srsran_rf_close(&rf);
    return 0;
  }

  if (nof_cells == 0) {
    printf("No cells found on band %d\n", band);
    if (target_pci >= 0) {
      printf("Target PCI %d was not detected.\n", target_pci);
    }
    srsran_rf_close(&rf);
    return 0;
  }

  printf("Found %d cell(s).\n\n", nof_cells);

  /* Apply bandwidth */
  if (nof_prb_force > 0) {
    printf("Using forced bandwidth: %d PRB\n", nof_prb_force);
    for (int i = 0; i < nof_cells; i++) {
      cells[i].nof_prb = nof_prb_force;
      cells[i].nof_ports = 2;  /* Assume 2 ports for TDD CBRS */
    }
  } else {
    printf("Decoding MIB for each cell...\n");
    for (int i = 0; i < nof_cells && !go_exit; i++) {
      srsran_cell_t cell;
      bzero(&cell, sizeof(cell));
      cell.id = cells[i].pci;
      cell.cp = cells[i].cp;
      cell.frame_type = cells[i].frame_type;
      cell.phich_length = SRSRAN_PHICH_NORM;
      cell.phich_resources = SRSRAN_PHICH_R_1;
      cell.nof_ports = 1;
      cell.nof_prb = 6;

      printf("  PCI %d: ", cells[i].pci);
      fflush(stdout);

      /* Use retry logic for MIB decode */
      if (decode_mib_with_retry(&rf, &cell, cells[i].freq, MIB_DECODE_RETRIES) == 0) {
        cells[i].nof_prb = cell.nof_prb;
        cells[i].nof_ports = cell.nof_ports;
        cells[i].cp = cell.cp;
        cells[i].frame_type = cell.frame_type;
        printf("%d PRB, %d ports (%s)\n", cells[i].nof_prb, cells[i].nof_ports,
               cells[i].frame_type == SRSRAN_TDD ? "TDD" : "FDD");
      } else {
        printf("MIB failed after %d retries (use -w to force bandwidth)\n", MIB_DECODE_RETRIES);
        cells[i].valid = false;
      }
    }
  }

  if (go_exit) {
    printf("\nInterrupted during MIB decode.\n");
    srsran_rf_close(&rf);
    return 0;
  }

  /* Filter out invalid cells */
  int valid_count = 0;
  for (int i = 0; i < nof_cells; i++) {
    if (cells[i].valid && cells[i].nof_prb > 0) {
      if (i != valid_count) { cells[valid_count] = cells[i]; }
      valid_count++;
    }
  }
  
  if (valid_count == 0) {
    printf("\nNo valid cells to monitor.\n");
    printf("Try using -w <prb> to force bandwidth.\n");
    srsran_rf_close(&rf);
    return -1;
  }
  
  nof_cells = valid_count;
  printf("\n=== Monitoring %d cell(s) ===\n", nof_cells);
  printf("Press Ctrl+C to stop (twice to force, three times for immediate exit).\n\n");

  /* Main monitoring loop - unified for both FDD and TDD */
  long long last_successful_ms = now_ms();
  const long long WATCHDOG_TIMEOUT_MS = 30000;  /* 30 second watchdog */
  
  while (!go_exit) {
    for (int i = 0; i < nof_cells && !go_exit; i++) {
      /* Skip cells that have failed too many times in a row */
      if (cells[i].consecutive_failures >= MAX_CONSECUTIVE_FAILURES) {
        continue;
      }

      long long t_ms = now_ms();
      
      /* Watchdog: if we haven't had a successful measurement in too long, 
       * try resetting the RF device */
      if (t_ms - last_successful_ms > WATCHDOG_TIMEOUT_MS) {
        fprintf(stderr, "Warning: No successful measurements for %.1f seconds, resetting RF...\n",
                (t_ms - last_successful_ms) / 1000.0);
        srsran_rf_stop_rx_stream(&rf);
        usleep(100000);  /* 100ms pause */
        last_successful_ms = t_ms;  /* Reset watchdog */
        
        /* Also reset TDD objects if applicable */
        if (force_tdd) {
          tdd_last_cell_id = UINT32_MAX;
        }
      }

      srsran_rf_set_rx_freq(&rf, 0, cells[i].freq * MHZ);

      srsran_cell_t cell;
      bzero(&cell, sizeof(cell));
      cell.id = cells[i].pci;
      cell.cp = cells[i].cp;
      cell.frame_type = cells[i].frame_type;
      cell.phich_length = SRSRAN_PHICH_NORM;
      cell.phich_resources = SRSRAN_PHICH_R_1;
      cell.nof_prb = cells[i].nof_prb;
      cell.nof_ports = cells[i].nof_ports;

      float rsrp_dbm = NAN, rsrq_db = NAN, snr_db = NAN, cfo_hz = NAN;

      if (measure_cell(&rf, &cell, &rsrp_dbm, &rsrq_db, &snr_db, &cfo_hz) == 0) {
        cells[i].consecutive_failures = 0;  /* Reset on success */
        last_successful_ms = now_ms();      /* Update watchdog */
        printf("{\"t_ms\":%lld,\"band\":%d,\"earfcn\":%d,\"freq_mhz\":%.2f,"
               "\"pci\":%d,\"n_prb\":%d,\"ports\":%d,"
               "\"rsrp_dbm\":%.1f,\"rsrq_db\":%.1f,\"snr_db\":%.1f,\"cfo_hz\":%.1f,"
               "\"frame_type\":\"%s\"}\n",
               t_ms, band, cells[i].earfcn, cells[i].freq, cells[i].pci,
               cells[i].nof_prb, cells[i].nof_ports, rsrp_dbm, rsrq_db, snr_db,
               cfo_hz, cells[i].frame_type == SRSRAN_TDD ? "TDD" : "FDD");
      } else if (!go_exit) {
        cells[i].consecutive_failures++;
        printf("{\"t_ms\":%lld,\"band\":%d,\"earfcn\":%d,\"freq_mhz\":%.2f,"
               "\"pci\":%d,\"n_prb\":%d,\"ports\":%d,\"error\":\"measure_failed\","
               "\"failures\":%d,\"frame_type\":\"%s\"}\n",
               t_ms, band, cells[i].earfcn, cells[i].freq, cells[i].pci,
               cells[i].nof_prb, cells[i].nof_ports, cells[i].consecutive_failures,
               cells[i].frame_type == SRSRAN_TDD ? "TDD" : "FDD");
        
        if (cells[i].consecutive_failures >= MAX_CONSECUTIVE_FAILURES) {
          fprintf(stderr, "Warning: PCI %d disabled after %d consecutive failures\n",
                  cells[i].pci, MAX_CONSECUTIVE_FAILURES);
        }
      }

      fflush(stdout);
    }

    /* Check if all cells are disabled */
    bool any_active = false;
    for (int i = 0; i < nof_cells; i++) {
      if (cells[i].consecutive_failures < MAX_CONSECUTIVE_FAILURES) {
        any_active = true;
        break;
      }
    }
    if (!any_active && !go_exit) {
      fprintf(stderr, "Error: All cells have failed. Exiting.\n");
      break;
    }

    /* Sleep between measurement cycles - check go_exit more frequently */
    for (int s = 0; s < 5 && !go_exit; s++) {
      usleep(100 * 1000);
    }
  }

  printf("\nShutting down...\n");
  cleanup_tdd_objects();  /* Clean up static TDD objects */
  srsran_rf_close(&rf);
  printf("Done.\n");
  return 0;
}