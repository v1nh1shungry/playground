#ifndef MY_ANY_HPP
#define MY_ANY_HPP

#include <utility>

namespace my {
class any {
private:
  struct universal_base {
    virtual ~universal_base() = default;
    virtual universal_base *clone() const = 0;
  };
  template <class T> struct wrapper : universal_base {
    template <class U>
      requires(std::is_same_v<std::remove_cvref_t<U>, T>)
    wrapper(U &&value) : obj{std::forward<U>(value)} {}
    universal_base *clone() const override { return new wrapper{obj}; }

    T obj;
  };

public:
  any() = default;
  template <class T>
    requires(!std::is_same_v<std::remove_cvref_t<T>, any>)
  any(T &&value) : m_obj{new wrapper<T>{std::forward<T>(value)}} {}
  any(const any &other) {
    if (other.m_obj)
      m_obj = other.m_obj->clone();
  }
  any(any &&other) noexcept { swap(other); }
  ~any() {
    if (m_obj)
      delete m_obj;
  }

  template <class T>
    requires(!std::is_same_v<std::remove_cvref_t<T>, any>)
  any &operator=(T &&value) {
    any{std::forward<T>(value)}.swap(*this);
    return *this;
  }
  any &operator=(const any &other) {
    if (this != &other)
      any{other}.swap(*this);
    return *this;
  }
  any &operator=(any &&other) noexcept {
    if (this != &other)
      any{std::move(other)}.swap(*this);
    return *this;
  }

  void swap(any &other) noexcept { std::swap(m_obj, other.m_obj); }

  bool has_value() const noexcept { return m_obj; }
  template <class T> T &get() const & {
    return dynamic_cast<wrapper<T> *>(m_obj)->obj;
  }
  template <class T> T get() && {
    return std::move(dynamic_cast<wrapper<T> *>(m_obj)->obj);
  }

private:
  universal_base *m_obj = nullptr;
};
} // namespace my

#endif //! MY_ANY_HPP
