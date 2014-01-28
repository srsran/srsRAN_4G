#include <uhd/usrp/multi_usrp.hpp>
#include <iostream>
#include <complex>
#include <cstdio>
#include <uhd/utils/msg.hpp>

void my_handler(uhd::msg::type_t type, const std::string &msg){
    //handle the message...
}

#include "uhd_handler.hpp"
#include "uhd.h"

typedef _Complex float complex_t;

#define SAMPLE_SZ sizeof(complex_t)

void uhd_rx_stream(void *h);

bool isLocked(void *h)
{
   uhd_handler* handler = static_cast<uhd_handler*>(h);
   return handler->usrp->get_rx_sensor("lo_locked", 0).to_bool();
}

bool uhd_rx_wait_lo_locked(void *h)
{

     double report = 0.0;
     while(isLocked(h) && report < 3.0)
     {
	   report += 0.1;
       usleep(1000);
     }
     return isLocked(h);
}

int uhd_start_rx_stream(void *h) {
	uhd_handler* handler = static_cast<uhd_handler*>(h);
    uhd::stream_cmd_t cmd(uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS);
    cmd.time_spec = handler->usrp->get_time_now();
    cmd.stream_now = true;
    handler->usrp->issue_stream_cmd(cmd);
    return 0;
}

int uhd_stop_rx_stream(void *h) {
	uhd_handler* handler = static_cast<uhd_handler*>(h);
    uhd::stream_cmd_t cmd(uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS);
    cmd.time_spec = handler->usrp->get_time_now();
    cmd.stream_now = true;
    handler->usrp->issue_stream_cmd(cmd);
    return 0;
}

int uhd_start_rx_stream_nsamples(void *h, int nsamples) {
	uhd_handler* handler = static_cast<uhd_handler*>(h);
    uhd::stream_cmd_t cmd(uhd::stream_cmd_t::STREAM_MODE_NUM_SAMPS_AND_MORE);
    cmd.time_spec = handler->usrp->get_time_now();
    cmd.stream_now = true;
    cmd.num_samps = nsamples;
    handler->usrp->issue_stream_cmd(cmd);
    return 0;
}



int uhd_open(char *args, void **h) {
	uhd_handler* handler = new uhd_handler();
	std::string _args=std::string(args);
	handler->usrp = uhd::usrp::multi_usrp::make(_args);

	uhd::msg::register_handler(&my_handler);

	std::string otw, cpu;
	otw="sc16";
	cpu="fc32";

	handler->usrp->set_clock_source("internal");

	uhd::stream_args_t stream_args(cpu, otw);
//	stream_args.channels.push_back(0);
//	stream_args.args["noclear"] = "1";

	handler->rx_stream = handler->usrp->get_rx_stream(stream_args);
	*h = handler;

	int size = 10000*handler->rx_stream->get_max_num_samps();

	return 0;
}

int uhd_close(void *h) {
	uhd_handler* handler = static_cast<uhd_handler*>(h);
	return 0;
}


double uhd_set_rx_srate(void *h, double freq) {
	uhd_handler* handler = static_cast<uhd_handler*>(h);
	handler->usrp->set_rx_rate(freq);
	double ret = handler->usrp->get_rx_rate();
	return ret;
}

double uhd_set_rx_gain(void *h, double gain) {
	uhd_handler* handler = static_cast<uhd_handler*>(h);
	handler->usrp->set_rx_gain(gain);
	return handler->usrp->get_rx_gain();
}

float uhd_get_rx_srate(void *h) {
	uhd_handler* handler = static_cast<uhd_handler*>(h);
	return handler->usrp->get_tx_rate();
}

double uhd_set_rx_freq(void *h, double freq) {
	uhd_handler* handler = static_cast<uhd_handler*>(h);
	handler->usrp->set_rx_freq(freq);
	return handler->usrp->get_rx_freq();
}

int uhd_recv(void *h, void *data, int nsamples, int blocking) {
	uhd_handler* handler = static_cast<uhd_handler*>(h);
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
