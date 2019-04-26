/*
 * Copyright 2013-2019 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

/* This file implements a few features not currently provided by the UHD C-API */
#include <uhd.h>
#include <uhd/usrp/multi_usrp.hpp>

extern "C" {
#include "srslte/phy/rf/rf.h"
#include "uhd_c_api.h"
}

#if UHD_VERSION < 31100
static void (*handler)(const char*);

void translate_handler(uhd::msg::type_t type, const std::string & msg)
{
  if(handler)
    handler(msg.c_str());
}
#endif

void rf_uhd_register_msg_handler_c(void (*new_handler)(const char*)) 
{
#if UHD_VERSION < 31100
  handler = new_handler;
  uhd::msg::register_handler(translate_handler);
#endif
}

void uhd_tx_metadata_set_time_spec(uhd_tx_metadata_handle *md, time_t secs, double frac_secs)
{
  (*md)->tx_metadata_cpp.time_spec = uhd::time_spec_t(secs, frac_secs);
  (*md)->tx_metadata_cpp.has_time_spec = true; 
}

void uhd_tx_metadata_set_start(uhd_tx_metadata_handle *md, bool is_start_of_burst)
{
  (*md)->tx_metadata_cpp.start_of_burst = is_start_of_burst;
}

void uhd_tx_metadata_set_has_time_spec(uhd_tx_metadata_handle *md, bool has_time_spec)
{
  (*md)->tx_metadata_cpp.has_time_spec = has_time_spec;
}


void uhd_tx_metadata_set_end(uhd_tx_metadata_handle *md, bool is_end_of_burst)
{
  (*md)->tx_metadata_cpp.end_of_burst = is_end_of_burst;
}

void uhd_tx_metadata_add_time_spec(uhd_tx_metadata_handle *md, double frac_secs)
{
  (*md)->tx_metadata_cpp.time_spec += frac_secs;
}

