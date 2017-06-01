

#ifndef ENB_MAC_METRICS_H
#define ENB_MAC_METRICS_H


namespace srsenb {

// MAC metrics per user   
  
struct mac_metrics_t
{
  uint16_t rnti; 
  int tx_pkts;
  int tx_errors;
  int tx_brate;
  int rx_pkts;
  int rx_errors;
  int rx_brate;
  int ul_buffer;
  int dl_buffer;
  float phr; 
};

} // namespace srsenb

#endif // ENB_MAC_METRICS_H
