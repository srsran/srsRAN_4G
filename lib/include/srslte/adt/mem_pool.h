/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSLTE_MEM_POOL_H
#define SRSLTE_MEM_POOL_H

namespace srslte {

/// Stores provided mem blocks in a stack in an non-owning manner. Not thread-safe
class memblock_stack
{
public:
  memblock_stack() = default;

  memblock_stack(const memblock_stack&) = delete;

  memblock_stack(memblock_stack&& other) noexcept : head(other.head) { other.head = nullptr; }

  memblock_stack& operator=(const memblock_stack&) = delete;

  memblock_stack& operator=(memblock_stack&& other) noexcept
  {
    head       = other.head;
    other.head = nullptr;
    return *this;
  }

  void push(uint8_t* block) noexcept
  {
    // printf("head: %ld\n", (long)head);
    node* next = ::new (block) node(head);
    head       = next;
    count++;
  }

  uint8_t* try_pop() noexcept
  {
    if (is_empty()) {
      return nullptr;
    }
    node* last_head = head;
    head            = head->prev;
    count--;
    return (uint8_t*)last_head;
  }

  bool is_empty() const { return head == nullptr; }

  size_t size() const { return count; }

  void clear() { head = nullptr; }

private:
  struct node {
    node* prev;

    explicit node(node* prev_) : prev(prev_) {}
  };

  node*  head  = nullptr;
  size_t count = 0;
};

/// memblock stack that mutexes pushing/popping
class mutexed_memblock_stack
{
public:
  mutexed_memblock_stack() = default;

  mutexed_memblock_stack(const mutexed_memblock_stack&) = delete;

  mutexed_memblock_stack(mutexed_memblock_stack&& other) noexcept
  {
    std::unique_lock<std::mutex> lk1(other.mutex, std::defer_lock);
    std::unique_lock<std::mutex> lk2(mutex, std::defer_lock);
    std::lock(lk1, lk2);
    stack = std::move(other.stack);
  }

  mutexed_memblock_stack& operator=(const mutexed_memblock_stack&) = delete;

  mutexed_memblock_stack& operator=(mutexed_memblock_stack&& other) noexcept
  {
    std::unique_lock<std::mutex> lk1(other.mutex, std::defer_lock);
    std::unique_lock<std::mutex> lk2(mutex, std::defer_lock);
    std::lock(lk1, lk2);
    stack = std::move(other.stack);
    return *this;
  }

  void push(uint8_t* block) noexcept
  {
    // auto t = time_prof(push_telapsed);
    std::lock_guard<std::mutex> lock(mutex);
    stack.push(block);
  }

  uint8_t* try_pop() noexcept
  {
    // auto t = time_prof(pop_telapsed);
    std::lock_guard<std::mutex> lock(mutex);
    uint8_t*                    block = stack.try_pop();
    return block;
  }

  bool is_empty() const noexcept { return stack.is_empty(); }

  void clear()
  {
    std::lock_guard<std::mutex> lock(mutex);
    stack.clear();
  }

private:
  memblock_stack stack;
  std::mutex     mutex;
};

template <typename T>
class single_thread_obj_pool
{
public:
  /// single-thread obj pool deleter
  struct obj_deleter {
    explicit obj_deleter(single_thread_obj_pool<T>* pool_) : pool(pool_) {}
    void                       operator()(void* block) { pool->stack.push(static_cast<uint8_t*>(block)); }
    single_thread_obj_pool<T>* pool;
  };
  using obj_ptr = std::unique_ptr<T, obj_deleter>;

  ~single_thread_obj_pool()
  {
    uint8_t* block = stack.try_pop();
    while (block != nullptr) {
      delete[] block;
      block = stack.try_pop();
    }
  }

  /// allocate object
  template <typename... Args>
  obj_ptr make(Args&&... args)
  {
    uint8_t* block = stack.try_pop();
    if (block == nullptr) {
      block = new uint8_t[sizeof(T)];
    }
    new (block) T(std::forward<Args>(args)...);
    return obj_ptr(reinterpret_cast<T*>(block), obj_deleter(this));
  }

  void reserve(size_t N)
  {
    for (size_t i = 0; i < N; ++i) {
      stack.push(new uint8_t[sizeof(T)]);
    }
  }

  size_t capacity() const { return stack.size(); }

private:
  memblock_stack stack;
};
template <typename T>
using unique_pool_obj = typename single_thread_obj_pool<T>::obj_ptr;

} // namespace srslte

#endif // SRSLTE_MEM_POOL_H
