/**
 * @file user_buffer.h
 * @brief UserBuffer 类
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-09-12
 */

#ifndef COMMON_USER_BUFFER_H_
#define COMMON_USER_BUFFER_H_

#include "buffer.h"

#include <google/protobuf/message.h>

namespace common {

/*
 * 协议格式伪代码
struct ProtobufTransportFormat __attribute__((__packed__)) {
  uint16_t size;

  uint8_t name_size;
  char type_name[name_size];
  char protobuf_data[size - name_size - 5];
  int32_t check_sum;    // adler32 of nameLen, type_name and protobuf_data
};
*/

class UserBuffer : public common::Buffer<uint16_t, 16 * 1024> {
 public:
  UserBuffer();
  UserBuffer(const google::protobuf::Message& proto_msg);

  inline char* msg_name() const { return this->msg_name_; }
  inline char* msg_data() const { return this->msg_data_; }
  inline uint32_t* check_sum() const { return this->check_sum_; }
  inline std::size_t msg_data_size() const { return this->msg_data_size_; }

  bool Decode();
  void Encode(const google::protobuf::Message& proto_msg);

 private:
  uint8_t* msg_name_size_;
  char* msg_name_;
  char* msg_data_;
  uint32_t* check_sum_;
  std::size_t msg_data_size_;
};

}  // namespace common

#endif  // COMMON_USER_BUFFER_H_
