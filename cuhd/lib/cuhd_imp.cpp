/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The libLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the libLTE library.
 *
 * libLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include <uhd/usrp/multi_usrp.hpp>
#include <iostream>
#include <complex>
#include <cstdio>
#include <uhd/utils/msg.hpp>

#include "cuhd_handler.hpp"
#include "liblte/cuhd/cuhd.h"


void my_handler(uhd::msg::type_t type, const std::string &msg){
    //handle the message...
}

typedef _Complex float complex_t;

#define SAMPLE_SZ sizeof(complex_t)

bool isLocked(void *h)
{
   cuhd_handler* handler = static_cast<cuhd_handler*>(h);
   std::vector<std::string> mb_sensors = handler->usrp->get_mboard_sensor_names();
   std::vector<std::string> rx_sensors = handler->usrp->get_rx_sensor_names(0);
   if(std::find(rx_sensors.begin(), rx_sensors.end(), "lo_locked") != rx_sensors.end()) {
     return handler->usrp->get_rx_sensor("lo_locked", 0).to_bool();
   }
   else if(std::find(mb_sensors.begin(), mb_sensors.end(), "ref_locked") != mb_sensors.end()) {
     return handler->usrp->get_mboard_sensor("ref_locked", 0).to_bool();
   }
   else {
     usleep(500);
     return true;
   }
}

bool cuhd_rx_wait_lo_locked(void *h)
{

     double report = 0.0;
     while(isLocked(h) && report < 3.0)
     {
	   report += 0.1;
       usleep(1000);
     }
     return isLocked(h);
}

int cuhd_start_rx_stream(void *h) {
	cuhd_handler* handler = static_cast<cuhd_handler*>(h);
    uhd::stream_cmd_t cmd(uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS);
    cmd.time_spec = handler->usrp->get_time_now();
    cmd.stream_now = true;
    handler->usrp->issue_stream_cmd(cmd);
    return 0;
}

int cuhd_stop_rx_stream(void *h) {
	cuhd_handler* handler = static_cast<cuhd_handler*>(h);
    uhd::stream_cmd_t cmd(uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS);
    cmd.time_spec = handler->usrp->get_time_now();
    cmd.stream_now = true;
    handler->usrp->issue_stream_cmd(cmd);
    return 0;
}

int cuhd_start_rx_stream_nsamples(void *h, int nsamples) {
	cuhd_handler* handler = static_cast<cuhd_handler*>(h);
    uhd::stream_cmd_t cmd(uhd::stream_cmd_t::STREAM_MODE_NUM_SAMPS_AND_MORE);
    cmd.time_spec = handler->usrp->get_time_now();
    cmd.stream_now = true;
    cmd.num_samps = nsamples;
    handler->usrp->issue_stream_cmd(cmd);
    return 0;
}

int cuhd_open(char *args, void **h) {
	cuhd_handler* handler = new cuhd_handler();
	std::string _args=std::string(args);
	handler->usrp = uhd::usrp::multi_usrp::make(_args);

	// Try to set LTE clock
	handler->usrp->set_master_clock_rate(30720000);

	handler->usrp->set_clock_source("internal");

	std::string otw, cpu;
	otw="sc16";
	cpu="fc32";
	uhd::stream_args_t stream_args(cpu, otw);
	handler->rx_stream = handler->usrp->get_rx_stream(stream_args);
	handler->tx_stream = handler->usrp->get_tx_stream(stream_args);

	*h = handler;

	return 0;
}

int cuhd_close(void *h) {
	cuhd_stop_rx_stream(h);
	/** Something else to close the USRP?? */
	return 0;
}


double cuhd_set_rx_srate(void *h, double freq) {
	cuhd_handler* handler = static_cast<cuhd_handler*>(h);
	handler->usrp->set_rx_rate(freq);
	double ret = handler->usrp->get_rx_rate();
	return ret;
}

double cuhd_set_rx_gain(void *h, double gain) {
	cuhd_handler* handler = static_cast<cuhd_handler*>(h);
	handler->usrp->set_rx_gain(gain);
	return handler->usrp->get_rx_gain();
}

double cuhd_set_rx_freq(void *h, double freq) {
	cuhd_handler* handler = static_cast<cuhd_handler*>(h);
	handler->usrp->set_rx_freq(freq);
	return handler->usrp->get_rx_freq();
}

int cuhd_recv(void *h, void *data, int nsamples, int blocking) {
	cuhd_handler* handler = static_cast<cuhd_handler*>(h);
	uhd::rx_metadata_t md;
	if (blocking) {
		int n=0,p;
		complex_t *data_c = (complex_t*) data;
		do {
			p=handler->rx_stream->recv(&data_c[n], nsamples-n, md);
			if (p == -1) {
				return -1;
			}
			n+=p;
		} while(n<nsamples);
		return nsamples;
	} else {
		return handler->rx_stream->recv(data, nsamples, md, 0.0);
	}
}

double cuhd_set_tx_gain(void *h, double gain) {
	cuhd_handler* handler = static_cast<cuhd_handler*>(h);
	handler->usrp->set_tx_gain(gain);
	return handler->usrp->get_tx_gain();
}

double cuhd_set_tx_srate(void *h, double freq) {
	cuhd_handler* handler = static_cast<cuhd_handler*>(h);
	handler->usrp->set_tx_rate(freq);
	return handler->usrp->get_tx_rate();
}

double cuhd_set_tx_freq(void *h, double freq) {
	cuhd_handler* handler = static_cast<cuhd_handler*>(h);
	handler->usrp->set_tx_freq(freq);
	return handler->usrp->get_tx_freq();
}

int cuhd_send(void *h, void *data, int nsamples, int blocking) {
	cuhd_handler* handler = static_cast<cuhd_handler*>(h);
	uhd::tx_metadata_t md;
	if (blocking) {
		int n=0,p;
		complex_t *data_c = (complex_t*) data;
		do {
			p=handler->tx_stream->send(&data_c[n], nsamples-n, md);
			if (p == -1) {
				return -1;
			}
			n+=p;
		} while(n<nsamples);
		return nsamples;
	} else {
		return handler->tx_stream->send(data, nsamples, md, 0.0);
	}
}
