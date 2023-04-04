#ifndef MY_SHARED_PTR_HPP
#define MY_SHARED_PTR_HPP

#include <utility>

namespace my {
template <class T> class shared_ptr {
public:
  shared_ptr(std::nullptr_t = nullptr) {}
  shared_ptr(T *ptr) : m_cb{new control_block{ptr, 1}} {}
  shared_ptr(const shared_ptr &other) : m_cb{other.m_cb} {
    if (m_cb)
      m_cb->use_count += 1;
  }
  shared_ptr(shared_ptr &&other) noexcept { swap(other); }
  ~shared_ptr() { reset(); }

  shared_ptr &operator=(const shared_ptr &other) {
    if (this != &other)
      shared_ptr{other}.swap(*this);
    return *this;
  }
  shared_ptr &operator=(shared_ptr &&other) noexcept {
    if (this != &other)
      shared_ptr(std::move(other)).swap(*this);
    return *this;
  }

  void swap(shared_ptr &other) noexcept { std::swap(m_cb, other.m_cb); }
  void reset() {
    if (m_cb) {
      m_cb->use_count -= 1;
      if (m_cb->use_count == 0) {
        delete m_cb->ptr;
        delete m_cb;
      }
    }
    m_cb = nullptr;
  }
  void reset(T *ptr) { shared_ptr{ptr}.swap(*this); }

  std::size_t use_count() const noexcept { return m_cb ? m_cb->use_count : 0; }
  T *get() const noexcept { return m_cb ? m_cb->ptr : nullptr; }
  T *operator->() const noexcept { return m_cb ? m_cb->ptr : nullptr; }
  T &operator*() const noexcept { return *m_cb->ptr; }

private:
  struct control_block {
    T *ptr = nullptr;
    std::size_t use_count = 0;
  } *m_cb = nullptr;
};
} // namespace my

#endif //! MY_SHARED_PTR_HPP
