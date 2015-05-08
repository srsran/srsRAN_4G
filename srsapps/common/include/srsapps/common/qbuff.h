
#include <stdint.h>
#include "srslte/config.h"

#ifndef QBUFF_H
#define QBUFF_H

/** Implementation of a lock-free single-producer single-consumer queue buffer
 * Communication can be pointer-based or stream based. 
 * Only 1 thread can read and only 1 thread can write. 
 * 
 * Writer: 
 *   - Call request, returns a pointer. 
 *   - Writes to memory, up to max_msg_size bytes
 *   - Call to push() passing message size 
 *  or 
 *   - use send()
 * 
 * Reader: 
 *   - Call to pop, receive pointer and message size
 *   - Read memory contents
 *   - Call to release() to release the message buffer
 *  or 
 *   - use recv()
 */

namespace srslte {

  class SRSLTE_API qbuff
  {
  public: 
    qbuff();
    ~qbuff();
    bool  init(uint32_t nof_messages, uint32_t max_msg_size);
    void* request();
    bool  push(uint32_t len); 
    void* pop(uint32_t *len, uint32_t idx);
    void* pop(uint32_t *len);
    void* pop();
    void  release();
    bool  isempty();
    bool  isfull();
    void  flush();
    bool  send(void *buffer, uint32_t msg_size); 
    int   recv(void* buffer, uint32_t buffer_size); 
    void  move_to(qbuff *dst);
    uint32_t pending_data(); 
  private:
    typedef struct {
      bool valid; 
      uint32_t len; 
      void *ptr; 
    } pkt_t; 
    
    uint32_t nof_messages; 
    uint32_t max_msg_size; 
    uint32_t rp, wp; 

    pkt_t   *packets; 
    uint8_t *buffer; 
    
  };

}

#endif
