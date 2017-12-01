#include <stdint.h>
#include "srslte/srslte.h"
#include "srslte/common/pcap.h"
#include "srslte/common/nas_pcap.h"


namespace srslte {

void nas_pcap::enable()
{
  enable_write = true;
}
void nas_pcap::open(const char* filename, uint32_t ue_id)
{
  pcap_file = LTE_PCAP_Open(NAS_LTE_DLT, filename);
  ue_id = ue_id;
  enable_write = true;
}
void nas_pcap::close()
{
  fprintf(stdout, "Saving NAS PCAP file\n");
  LTE_PCAP_Close(pcap_file);
}

void nas_pcap::write_nas(uint8_t *pdu, uint32_t pdu_len_bytes)
{
    if (enable_write) {
      NAS_Context_Info_t  context;
      if (pdu) {
        LTE_PCAP_NAS_WritePDU(pcap_file, &context, pdu, pdu_len_bytes);
      }
    }
}

}
