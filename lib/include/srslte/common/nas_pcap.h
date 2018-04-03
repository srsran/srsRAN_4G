#ifndef SRSLTE_NAS_PCAP_H
#define SRSLTE_NAS_PCAP_H

#include "srslte/common/pcap.h"

namespace srslte {

class nas_pcap
{
public:
    nas_pcap() {enable_write=false; ue_id=0; pcap_file = NULL; }
    void enable();
    void open(const char *filename, uint32_t ue_id=0);
    void close();
    void write_nas(uint8_t *pdu, uint32_t pdu_len_bytes);
private:
    bool enable_write;
    FILE *pcap_file;
    uint32_t ue_id;
    void pack_and_write(uint8_t* pdu, uint32_t pdu_len_bytes);
};

} //namespace srslte

#endif // SRSLTE_NAS_PCAP_H
