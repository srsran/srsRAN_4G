// Formatting library for C++
//
// Copyright (c) 2012 - 2016, Victor Zverovich
// All rights reserved.
//
// For the license information refer to format.h.

#include "fmt/format-inl.h"
#include <mutex>

FMT_BEGIN_NAMESPACE
namespace detail {

template <typename T>
int format_float(char* buf, std::size_t size, const char* format, int precision,
                 T value) {
#ifdef FMT_FUZZ
  if (precision > 100000)
    throw std::runtime_error(
        "fuzz mode - avoid large allocation inside snprintf");
#endif
  // Suppress the warning about nonliteral format string.
  int (*snprintf_ptr)(char*, size_t, const char*, ...) = FMT_SNPRINTF;
  return precision < 0 ? snprintf_ptr(buf, size, format, value)
                       : snprintf_ptr(buf, size, format, precision, value);
}

#define NODE_POOL_SIZE (10000u)
static constexpr uint8_t memory_heap_tag = 0xAA;
class dyn_node_pool
{
  /// The extra byte is used to store the memory tag at position 0 in the array.
  using type = std::array<uint8_t, dynamic_arg_list::max_pool_node_size + 1>;

public:
  dyn_node_pool() {
    pool.resize(NODE_POOL_SIZE);
    free_list.reserve(NODE_POOL_SIZE);
    for (auto& elem : pool) {
      free_list.push_back(elem.data());
    }
  }

  dyn_node_pool(const dyn_node_pool&) = delete;
  dyn_node_pool(dyn_node_pool&&) = delete;
  dyn_node_pool& operator=(dyn_node_pool&&) = delete;
  dyn_node_pool& operator=(const dyn_node_pool&) = delete;

  void* alloc(std::size_t sz) {
    assert(sz <= dynamic_arg_list::max_pool_node_size && "Object is too large to fit in the pool");

    std::lock_guard<std::mutex> lock(m);
    if (free_list.empty()) {
      // Tag that this allocation was performed by the heap.
      auto *p = new type;
      (*p)[0] = memory_heap_tag;
      return p->data() + 1;
    }

    auto* p = free_list.back();
    free_list.pop_back();

    // Tag that this allocation was performed by the pool.
    p[0] = 0;
    return p + 1;
  }

  void dealloc(void* p) {
    if (!p) {
      return;
    }

    std::lock_guard<std::mutex> lock(m);
    uint8_t* base_ptr = reinterpret_cast<uint8_t *>(p) - 1;
    if (*base_ptr == memory_heap_tag) {
      // This pointer was allocated using the heap.
      delete reinterpret_cast<type *>(base_ptr);
      return;
    }

    free_list.push_back(base_ptr);
  }

private:
  std::vector<type> pool;
  std::vector<uint8_t *> free_list;
  mutable std::mutex m;
};

static dyn_node_pool node_pool;

void *dynamic_arg_list::allocate_from_pool(std::size_t sz) {
  return node_pool.alloc(sz);
}

void dynamic_arg_list::free_from_pool(void *ptr) {
  return node_pool.dealloc(ptr);
}

}  // namespace detail

template struct FMT_INSTANTIATION_DEF_API detail::basic_data<void>;

// Workaround a bug in MSVC2013 that prevents instantiation of format_float.
int (*instantiate_format_float)(double, int, detail::float_specs,
                                detail::buffer<char>&) = detail::format_float;

#ifndef FMT_STATIC_THOUSANDS_SEPARATOR
template FMT_API detail::locale_ref::locale_ref(const std::locale& loc);
template FMT_API std::locale detail::locale_ref::get<std::locale>() const;
#endif

// Explicit instantiations for char.

template FMT_API std::string detail::grouping_impl<char>(locale_ref);
template FMT_API char detail::thousands_sep_impl(locale_ref);
template FMT_API char detail::decimal_point_impl(locale_ref);

template FMT_API void detail::buffer<char>::append(const char*, const char*);

template FMT_API FMT_BUFFER_CONTEXT(char)::iterator detail::vformat_to(
    detail::buffer<char>&, string_view,
    basic_format_args<FMT_BUFFER_CONTEXT(char)>);

template FMT_API int detail::snprintf_float(double, int, detail::float_specs,
                                            detail::buffer<char>&);
template FMT_API int detail::snprintf_float(long double, int,
                                            detail::float_specs,
                                            detail::buffer<char>&);
template FMT_API int detail::format_float(double, int, detail::float_specs,
                                          detail::buffer<char>&);
template FMT_API int detail::format_float(long double, int, detail::float_specs,
                                          detail::buffer<char>&);

// Explicit instantiations for wchar_t.

template FMT_API std::string detail::grouping_impl<wchar_t>(locale_ref);
template FMT_API wchar_t detail::thousands_sep_impl(locale_ref);
template FMT_API wchar_t detail::decimal_point_impl(locale_ref);

template FMT_API void detail::buffer<wchar_t>::append(const wchar_t*,
                                                      const wchar_t*);
FMT_END_NAMESPACE
