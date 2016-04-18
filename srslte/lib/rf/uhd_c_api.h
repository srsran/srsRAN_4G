
#include <uhd.h>
#include "srslte/config.h"
#include "srslte/rf/rf.h"

/* Declare functions not currently provided by the C-API */
SRSLTE_API void rf_uhd_register_msg_handler_c(void (*new_handler)(const char*));
SRSLTE_API void uhd_tx_metadata_set_time_spec(uhd_tx_metadata_handle *md, time_t secs, double frac_secs);
SRSLTE_API void uhd_tx_metadata_set_start(uhd_tx_metadata_handle *md, bool is_start_of_burst);
SRSLTE_API void uhd_tx_metadata_set_end(uhd_tx_metadata_handle *md, bool is_end_of_burst);
SRSLTE_API void uhd_tx_metadata_add_time_spec(uhd_tx_metadata_handle *md, double frac_secs);
