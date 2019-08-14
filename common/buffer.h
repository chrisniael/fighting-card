/**
 * @file buffer.h
 * @brief Buffer 类
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-09-12
 */

#ifndef COMMON_BUFFER_H_
#define COMMON_BUFFER_H_

#include <cstddef>
#include <cstdint>
#include <cstring>

#include "log.h"

namespace common {

template <class HeaderType, HeaderType kMaxBufferSize>
class Buffer {
 public:
  Buffer() { memset(buf_, 0, kMaxBufferSize); }

  inline char* get_buf() { return this->buf_; }
  inline const char* get_buf() const { return this->buf_; }
  inline HeaderType size() const {
    return this->get_header_size() + this->get_body_size();
  }
  inline HeaderType get_max_size() const { return kMaxBufferSize; }
  inline HeaderType get_header_size() const { return sizeof(HeaderType); }

  inline char* get_body() { return this->buf_ + sizeof(HeaderType); }
  inline const char* get_body() const {
    return this->buf_ + sizeof(HeaderType);
  }
  inline const HeaderType& get_body_size() const {
    return *reinterpret_cast<const HeaderType*>(this->buf_);
  }
  inline void set_body_size(const HeaderType& body_size) {
    *reinterpret_cast<HeaderType*>(this->buf_) = body_size;
  }

  inline bool SizeValid() const {
    const uint16_t& body_size = this->get_body_size();
    if (body_size > this->get_max_size()) {
      LOG_ERROR("body_size > max, body_size={}", body_size);
      return false;
    }
    if (body_size <= 0) {
      LOG_ERROR("body_size <= 0, body_size={}", body_size);
      return false;
    }
    return true;
  }

 private:
  char buf_[kMaxBufferSize];
};

}  // namespace common

#endif  // COMMON_BUFFER_H_
