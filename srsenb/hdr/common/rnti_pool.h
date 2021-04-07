/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSRAN_RNTI_POOL_H
#define SRSRAN_RNTI_POOL_H

#include <memory>

namespace srsenb {

// Allocation of Objects in UE dedicated memory pool
void  reserve_rnti_memblocks(size_t nof_blocks);
void* allocate_rnti_dedicated_mem(uint16_t rnti, std::size_t size, std::size_t align);
void  deallocate_rnti_dedicated_mem(uint16_t rnti, void* p);

template <typename T>
struct rnti_obj_deleter {
  uint16_t rnti;
  explicit rnti_obj_deleter(uint16_t rnti_ = 0) : rnti(rnti_) {}
  void operator()(void* p)
  {
    static_cast<T*>(p)->~T();
    deallocate_rnti_dedicated_mem(rnti, p);
  }
};
template <typename T>
using unique_rnti_ptr = std::unique_ptr<T, rnti_obj_deleter<T> >;

template <typename T, typename... Args>
unique_rnti_ptr<T> make_rnti_obj(uint16_t rnti, Args&&... args)
{
  void* block = allocate_rnti_dedicated_mem(rnti, sizeof(T), alignof(T));
  new (block) T(std::forward<Args>(args)...);
  return unique_rnti_ptr<T>(static_cast<T*>(block), rnti_obj_deleter<T>(rnti));
}

} // namespace srsenb

#endif // SRSRAN_RNTI_POOL_H
