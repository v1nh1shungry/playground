#ifndef MY_STRING_HPP
#define MY_STRING_HPP

#include <algorithm>
#include <cstring>
#include <stdexcept>

namespace my {
class string {
public:
  string() { m_sso_buffer[0] = '\0'; }
  string(const char *str) : m_size{strlen(str)} {
    if (m_size < SSO_BUFFER_SIZE) {
      std::copy(str, str + m_size + 1, m_sso_buffer);
    } else {
      m_buffer_size = m_size * 2;
      m_ptr = new char[m_buffer_size];
      std::copy(str, str + m_size + 1, m_ptr);
    }
  }
  string(const string &other)
      : m_buffer_size{other.m_buffer_size}, m_size{other.m_size} {
    if (m_buffer_size == SSO_BUFFER_SIZE) {
      std::copy(other.m_sso_buffer, other.m_sso_buffer + m_size + 1,
                m_sso_buffer);
    } else {
      m_ptr = new char[m_buffer_size];
      std::copy(other.m_ptr, other.m_ptr + m_size + 1, m_ptr);
    }
  }
  string(string &&other) noexcept {
    m_sso_buffer[0] = '\0';
    swap(other);
  }
  ~string() {
    if (m_buffer_size != SSO_BUFFER_SIZE)
      delete[] m_ptr;
  }

  string &operator=(const char *str) {
    string{str}.swap(*this);
    return *this;
  }
  string &operator=(const string &other) {
    if (this != &other)
      string{other}.swap(*this);
    return *this;
  }
  string &operator=(string &&other) noexcept {
    if (this != &other)
      string{std::move(other)}.swap(*this);
    return *this;
  }

  void swap(string &other) noexcept {
    std::swap(m_sso_buffer, other.m_sso_buffer);
    std::swap(m_buffer_size, other.m_buffer_size);
    std::swap(m_size, other.m_size);
  }

  bool empty() const noexcept { return m_size == 0; }
  std::size_t size() const noexcept { return m_size; }
  std::size_t capacity() const noexcept { return m_buffer_size - 1; }
  const char *c_str() const noexcept {
    return m_buffer_size == SSO_BUFFER_SIZE ? m_sso_buffer : m_ptr;
  }
  char operator[](std::size_t index) const {
    return m_buffer_size == SSO_BUFFER_SIZE ? m_sso_buffer[index]
                                            : m_ptr[index];
  }
  char &operator[](std::size_t index) {
    return m_buffer_size == SSO_BUFFER_SIZE ? m_sso_buffer[index]
                                            : m_ptr[index];
  }

  void push_back(char c) {
    char *buffer = m_buffer_size == SSO_BUFFER_SIZE ? m_sso_buffer : m_ptr;
    if (m_size >= m_buffer_size - 1) {
      auto new_buffer_size = m_buffer_size * 2;
      char *new_buffer = new char[new_buffer_size];
      std::copy(buffer, buffer + m_size, new_buffer);
      if (m_buffer_size != SSO_BUFFER_SIZE)
        delete[] m_ptr;
      m_ptr = buffer = new_buffer;
      m_buffer_size = new_buffer_size;
    }
    buffer[m_size] = c;
    m_size += 1;
    buffer[m_size] = '\0';
  }
  void pop_back() {
    if (m_size == 0)
      throw std::out_of_range{"my::string::pop_back(): empty string"};
    m_size -= 1;
    if (m_buffer_size == SSO_BUFFER_SIZE)
      m_sso_buffer[m_size] = '\0';
    else
      m_ptr[m_size] = '\0';
  }

private:
  constexpr static std::size_t SSO_BUFFER_SIZE = 16;

private:
  union {
    char m_sso_buffer[SSO_BUFFER_SIZE];
    char *m_ptr;
  };
  std::size_t m_buffer_size = SSO_BUFFER_SIZE;
  std::size_t m_size = 0;
};
} // namespace my

#endif //! MY_STRING_HPP
