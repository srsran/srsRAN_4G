#include "srsapps/ue/mac/mac_pdu.h"

namespace srslte {
  namespace ue {
mac_pdu::mac_subh* mac_pdu::get()
{

}
void mac_pdu::init(uint32_t pdu_len)
{

}
mac_pdu::mac_pdu(uint32_t max_subheaders)
{

}
mac_pdu::mac_subh::cetype mac_pdu::mac_subh::ce_type()
{

}
uint16_t mac_pdu::mac_subh::get_c_rnti()
{

}
uint64_t mac_pdu::mac_subh::get_con_res_id()
{

}
uint8_t mac_pdu::mac_subh::get_phd()
{

}
uint32_t mac_pdu::mac_subh::get_sdu_lcid()
{

}
uint32_t mac_pdu::mac_subh::get_sdu_nbytes()
{

}
uint8_t* mac_pdu::mac_subh::get_sdu_ptr()
{

}
uint8_t mac_pdu::mac_subh::get_ta_cmd()
{

}
bool mac_pdu::mac_subh::is_sdu()
{

}
bool mac_pdu::mac_subh::set_c_rnti(uint16_t crnti)
{

}
bool mac_pdu::mac_subh::set_con_res_id(uint64_t con_res_id)
{

}
bool mac_pdu::mac_subh::set_phd(uint8_t phd)
{

}
bool mac_pdu::mac_subh::set_sdu(uint8_t* ptr, uint32_t nof_bytes)
{

}
bool mac_pdu::mac_subh::set_ta_cmd(uint8_t ta_cmd)
{

}
void mac_pdu::parse_packet(uint8_t* ptr)
{

}
bool mac_pdu::read_next()
{

}
void mac_pdu::reset()
{

}
bool mac_pdu::write_next()
{

}
bool mac_pdu::write_packet(uint8_t* ptr)
{

}



mac_rar_pdu::mac_rar* mac_rar_pdu::get()
{

}
uint8_t mac_rar_pdu::get_backoff()
{

}
void mac_rar_pdu::init(uint32_t pdu_len)
{

}
bool mac_rar_pdu::is_backoff()
{

}
uint32_t mac_rar_pdu::mac_rar::get_rapid()
{

}
void mac_rar_pdu::mac_rar::get_sched_grant(uint8_t grant[])
{

}
uint32_t mac_rar_pdu::mac_rar::get_ta_cmd()
{

}
uint16_t mac_rar_pdu::mac_rar::get_temp_crnti()
{

}
void mac_rar_pdu::mac_rar::set_rapid(uint32_t rapid)
{

}
void mac_rar_pdu::mac_rar::set_sched_grant(uint8_t grant[])
{

}
void mac_rar_pdu::mac_rar::set_ta_cmd(uint32_t ta)
{

}
void mac_rar_pdu::mac_rar::set_temp_crnti(uint16_t temp_rnti)
{

}
mac_rar_pdu::mac_rar_pdu(uint32_t max_rars)
{

}
void mac_rar_pdu::parse_packet(uint8_t* ptr)
{

}
bool mac_rar_pdu::read_next()
{

}
void mac_rar_pdu::reset()
{

}
void mac_rar_pdu::set_backoff(uint8_t bi)
{

}
bool mac_rar_pdu::write_next()
{

}
bool mac_rar_pdu::write_packet(uint8_t* ptr)
{

}


  
    
  }
}

#ifdef kk
bool demux::lcid_is_lch(uint32_t lcid) {
  if (lcid <= LIBLTE_MAC_DLSCH_DCCH_LCID_END) {
    return true; 
  } else {
    return false; 
  }
}
#endif
