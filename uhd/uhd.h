

#ifdef __cplusplus
extern "C" {
#endif
#include <stdbool.h>

int uhd_open(char *args, void **handler);
int uhd_close(void *h);
int uhd_start_rx_stream(void *h);
int uhd_start_rx_stream_nsamples(void *h, int nsamples);
int uhd_stop_rx_stream(void *h);

bool uhd_rx_wait_lo_locked(void *h);
double uhd_set_rx_srate(void *h, double freq);
double uhd_set_rx_gain(void *h, double gain);

double uhd_set_rx_freq(void *h, double freq);

int uhd_recv(void *h, void *data, int nsamples, int blocking);


#ifdef __cplusplus
}
#endif
