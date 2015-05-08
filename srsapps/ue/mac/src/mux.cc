
#include "srsapps/ue/mac/mux.h"
#include "srsapps/ue/mac/mac.h"

namespace srslte {
namespace ue {

#define IO_IDX(lch)     (lch + mac_io::MAC_LCH_CCCH_UL)
#define UL_IDX(lch)     (lch - mac_io::MAC_LCH_CCCH_UL)

mux::mux() : pdu_msg(20)
{
  msg3_buff.init(1, MSG3_BUFF_SZ);
  pdu_buff.init(1, PDU_BUFF_SZ);
  bzero(nof_tx_pkts, sizeof(uint32_t) * mac_io::NOF_UL_LCH);
  pthread_mutex_init(&mutex, NULL);
  
  for (int i=0;i<mac_io::NOF_UL_LCH;i++) {
   priority[i]        = 1; 
   priority_sorted[i] = 1; 
   PBR[i]             = -1; // -1 is infinite 
   BSD[i]             = 10;
   lchid_sorted[i]      = i; 
  }  
}

void mux::init(log *log_h_, mac_io *mac_io_h_)
{
  log_h      = log_h_;
  mac_io_h   = mac_io_h_;
}

void mux::reset()
{
  for (int i=0;i<mac_io::NOF_UL_LCH;i++) {
    Bj[i] = 0; 
  }
}

void mux::set_priority(uint32_t lch_id, uint32_t set_priority, int set_PBR, uint32_t set_BSD)
{
  pthread_mutex_lock(&mutex);
  if (lch_id < mac_io::NOF_UL_LCH) {
    priority[lch_id] = set_priority;
    PBR[lch_id]      = set_PBR;
    BSD[lch_id]      = set_BSD; 
    
    // Insert priority in sorted idx array
    int new_index = 0; 
    while(set_priority > priority_sorted[new_index] && new_index < mac_io::NOF_UL_LCH) {
      new_index++; 
    }
    int old_index = 0; 
    while(lch_id != lchid_sorted[old_index] && new_index < mac_io::NOF_UL_LCH) {
      old_index++;
    }
    if (new_index ==  mac_io::NOF_UL_LCH) {
      Error("Can't find LchID=%d in sorted list\n", lch_id);
      return;
    }
    // Replace goes in one direction or the other 
    int add=new_index>old_index?1:-1;
    for (int i=old_index;i!=new_index;i+=add) {
      priority_sorted[i] = priority_sorted[i+add];
      lchid_sorted[i]    = lchid_sorted[i+add];
    }
    priority_sorted[new_index] = set_priority;
    lchid_sorted[new_index]    = lch_id; 
  }
  pthread_mutex_unlock(&mutex);
}

void mux::pdu_release()
{
  pdu_buff.release();
}

bool mux::pdu_move_to_msg3(uint32_t tti_, uint32_t pdu_sz)
{
  tti = tti_; 
  if (pdu_buff.isempty()) {
    if (assemble_pdu(pdu_sz)) {
      if (pdu_buff.pending_data() < MSG3_BUFF_SZ) {
        pdu_buff.move_to(&msg3_buff);        
        return true; 
      } else {
        pdu_buff.release();
        Error("Assembled PDU size exceeds Msg3 buffer size\n");
        return false; 
      }
    } else {
      Error("Assembling PDU\n");
      return false; 
    }    
  } else {
    Error("Generating PDU: PDU pending in buffer for transmission\n");
    return false; 
  }  
}

// Multiplexing and logical channel priorization as defined in Section 5.4.3
uint8_t* mux::pdu_pop(uint32_t tti_, uint32_t pdu_sz)
{
  tti = tti_; 
  if (pdu_buff.isempty()) {
    if (assemble_pdu(pdu_sz)) {
      return (uint8_t*) pdu_buff.pop();
    } else {
      return NULL; 
    }    
  } else {
    Error("Generating PDU: PDU pending in buffer for transmission\n");
    return NULL; 
  }
}

void mux::append_crnti_ce_next_tx(uint16_t crnti) {
  pending_crnti_ce = crnti; 
}

bool mux::assemble_pdu(uint32_t pdu_sz) {

  uint8_t *buff = (uint8_t*) pdu_buff.request();
  if (!buff) {
    Error("Assembling PDU: Buffer is not available\n");
    return false; 
  }
  
  // Make sure pdu_sz is byte-aligned
  pdu_sz = 8*(pdu_sz/8);
  
  // Acquire mutex. Cannot change priorities, PBR or BSD after assemble finishes
  pthread_mutex_lock(&mutex); 
  
  // Update Bj
  for (int i=0;i=mac_io::NOF_UL_LCH;i++) {    
    // Add PRB unless it's infinity 
    if (PBR[i] >= 0) {
      Bj[i] += PBR[i];
    }
    if (Bj[i] >= BSD[i]) {
      Bj[i] = BSD[i]; 
    }    
  }
  
// Logical Channel Procedure
  
  uint32_t sdu_sz   = 0; 
 
  pdu_msg.init(pdu_sz);
  
  // MAC control element for C-RNTI or data from UL-CCCH
  if (!allocate_sdu(UL_IDX(mac_io::MAC_LCH_CCCH_UL), &pdu_msg)) {
    if (pending_crnti_ce) {
      if (pdu_msg.write_next()) {
        if (!pdu_msg.get()->set_c_rnti(pending_crnti_ce)) {
          Warning("Pending C-RNTI CE could not be inserted in MAC PDU\n");
        }
      }
    }
  }
  pending_crnti_ce = 0; 
  
  // MAC control element for BSR, with exception of BSR included for padding;
     // TODO
  // MAC control element for PHR
     // TODO
     
  // data from any Logical Channel, except data from UL-CCCH;  
  // first only those with positive Bj
  for (int i=0;i<mac_io::NOF_UL_LCH;i++) {
    bool res = true; 
    while ((Bj[i] > 0 || PBR[i] < 0) && res) {
      res = allocate_sdu(lchid_sorted[i], &pdu_msg, &sdu_sz);
      if (res && PBR[i] >= 0) {
        Bj[i] -= sdu_sz;         
      }
    }
  }
  
  // If resources remain, allocate regardless of their Bj value
  for (int i=0;i<mac_io::NOF_UL_LCH;i++) {
    while (allocate_sdu(lchid_sorted[i], &pdu_msg));   
  }
  
  // MAC control element for BSR included for padding.
     // TODO

  pthread_mutex_unlock(&mutex);

  /* Release SDUs */
  for (int i=0;i<mac_io::NOF_UL_LCH;i++) {
    while(nof_tx_pkts[i] > 0) {
      mac_io_h->get(IO_IDX(i))->release();      
      nof_tx_pkts[i]--;
    }
  }

  /* Generate MAC PDU and save to buffer */
  if (pdu_msg.write_packet(buff)) {
    pdu_buff.push(pdu_sz);
  } else {
    Error("Writing PDU message to packet\n");
    return false; 
  }  
  return true; 
}


bool mux::allocate_sdu(uint32_t lcid, mac_pdu *pdu) 
{
  return allocate_sdu(lcid, pdu, NULL);
}

bool mux::allocate_sdu(uint32_t lcid, mac_pdu *pdu, uint32_t *sdu_sz) 
{
  
  // Get n-th pending SDU pointer and length
  uint32_t buff_len; 
  uint8_t *buff_ptr = (uint8_t*) mac_io_h->get(lcid)->pop(&buff_len, nof_tx_pkts[lcid]);  

  if (buff_ptr) { // there is pending SDU to allocate
    if (pdu->write_next()) { // there is space for a new subheader
      if (pdu->get()->set_sdu(buff_ptr, buff_len)) { // new SDU could be added
        // Increase number of pop'ed packets from queue
        nof_tx_pkts[lcid]++;      
        return true;               
      }
    } 
  }
  return false; 
}



void mux::msg3_flush()
{
  msg3_buff.flush();
}

bool mux::msg3_isempty()
{
  return msg3_buff.isempty();
}

/* Returns a pointer to the Msg3 buffer */
uint8_t* mux::msg3_pop(uint32_t tti, uint32_t TB_size)
{
  uint32_t len; 
  uint8_t *msg3 = (uint8_t*) msg3_buff.pop(&len);
  if (len < TB_size) {
    // Pad with zeros without exceeding maximum buffer size 
    if (TB_size <= MSG3_BUFF_SZ) {
      bzero(&msg3[len], (TB_size-len)*sizeof(uint8_t));
    } else {
      Error("Requested TB size from Msg3 buffer exceeds buffer size (%d>%d)\n", TB_size, MSG3_BUFF_SZ);
      return NULL; 
    }
  }  
  return msg3;
}

void mux::msg3_release()
{
  msg3_buff.release();
}


  
}
}
