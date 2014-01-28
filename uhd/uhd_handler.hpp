#include <uhd/usrp/multi_usrp.hpp>

class uhd_handler {
public:
	uhd::usrp::multi_usrp::sptr usrp;
	uhd::rx_streamer::sptr rx_stream;
	bool rx_stream_enable;

};
