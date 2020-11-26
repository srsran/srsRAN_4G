/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */
#ifndef SRSLTE_RF_UHD_IMP_H_
#define SRSLTE_RF_UHD_IMP_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "srslte/config.h"
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/rf/rf.h"

#define DEVNAME_B200 "uhd_b200"
#define DEVNAME_X300 "uhd_x300"
#define DEVNAME_N300 "uhd_n300"
#define DEVNAME_E3X0 "uhd_e3x0"
#define DEVNAME_UNKNOWN "uhd_unknown"

SRSLTE_API int rf_uhd_open(char* args, void** handler);

SRSLTE_API int rf_uhd_open_multi(char* args, void** handler, uint32_t nof_channels);

SRSLTE_API const char* rf_uhd_devname(void* h);

SRSLTE_API int rf_uhd_close(void* h);

SRSLTE_API int rf_uhd_start_rx_stream(void* h, bool now);

SRSLTE_API int rf_uhd_stop_rx_stream(void* h);

SRSLTE_API void rf_uhd_flush_buffer(void* h);

SRSLTE_API bool rf_uhd_has_rssi(void* h);

SRSLTE_API float rf_uhd_get_rssi(void* h);

SRSLTE_API double rf_uhd_set_rx_srate(void* h, double freq);

SRSLTE_API int rf_uhd_set_rx_gain(void* h, double gain);

SRSLTE_API int rf_uhd_set_rx_gain_ch(void* h, uint32_t ch, double gain);

SRSLTE_API double rf_uhd_get_rx_gain(void* h);

SRSLTE_API double rf_uhd_get_tx_gain(void* h);

SRSLTE_API srslte_rf_info_t* rf_uhd_get_info(void* h);

SRSLTE_API void rf_uhd_suppress_stdout(void* h);

SRSLTE_API void rf_uhd_register_error_handler(void* h, srslte_rf_error_handler_t error_handler, void* arg);

SRSLTE_API double rf_uhd_set_rx_freq(void* h, uint32_t ch, double freq);

SRSLTE_API int
rf_uhd_recv_with_time(void* h, void* data, uint32_t nsamples, bool blocking, time_t* secs, double* frac_secs);

SRSLTE_API int
rf_uhd_recv_with_time_multi(void* h, void** data, uint32_t nsamples, bool blocking, time_t* secs, double* frac_secs);

SRSLTE_API double rf_uhd_set_tx_srate(void* h, double freq);

SRSLTE_API int rf_uhd_set_tx_gain(void* h, double gain);

SRSLTE_API int rf_uhd_set_tx_gain_ch(void* h, uint32_t ch, double gain);

SRSLTE_API double rf_uhd_set_tx_freq(void* h, uint32_t ch, double freq);

SRSLTE_API void rf_uhd_get_time(void* h, time_t* secs, double* frac_secs);

SRSLTE_API void rf_uhd_sync_pps(void* h);

SRSLTE_API int rf_uhd_send_timed(void*  h,
                                 void*  data,
                                 int    nsamples,
                                 time_t secs,
                                 double frac_secs,
                                 bool   has_time_spec,
                                 bool   blocking,
                                 bool   is_start_of_burst,
                                 bool   is_end_of_burst);

SRSLTE_API int rf_uhd_send_timed_multi(void*  h,
                                       void** data,
                                       int    nsamples,
                                       time_t secs,
                                       double frac_secs,
                                       bool   has_time_spec,
                                       bool   blocking,
                                       bool   is_start_of_burst,
                                       bool   is_end_of_burst);

#ifdef __cplusplus
}
#endif

#endif /* SRSLTE_RF_UHD_IMP_H_ */
