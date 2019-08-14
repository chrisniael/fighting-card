/**
 * @file pipe_buffer.h
 * @brief PipeBuffer 类
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-09-12
 */

#ifndef COMMON_PIPE_BUFFER_H_
#define COMMON_PIPE_BUFFER_H_

#include <string>

#include <google/protobuf/message.h>

#include "buffer.h"
#include "user_buffer.h"

namespace common {

/*
 * 协议格式伪代码
struct ProtobufTransportFormat __attribute__((__packed__)) {
  uint32_t size;    // not include self

  uint32_t session_id;
  uint8_t appid_size;
  char appid[appid_size];
  uint8_t user_id_size;
  char user_id[user_id_size];

  uint8_t name_size;
  char type_name[name_size];
  char protobuf_data[size - name_size - 5];
  int32_t check_sum;    // adler32 of nameLen, type_name and protobuf_data
};
*/

/**
 * @brief PipeBuffer 类
 */
class PipeBuffer : public common::Buffer<uint32_t, 512 * 1024> {
 public:
  PipeBuffer();
  PipeBuffer(const google::protobuf::Message& proto_msg);
  PipeBuffer(const uint32_t& session_id,
             const google::protobuf::Message& proto_msg);
  PipeBuffer(const uint32_t& session_id, const std::string& appid,
             const std::string& userid,
             const google::protobuf::Message& proto_msg);
  PipeBuffer(const uint32_t& session_id, const common::UserBuffer& user_buf);
  PipeBuffer(const uint32_t& session_id, const std::string& appid,
             const std::string& user_id, const common::UserBuffer& user_buf);

  inline uint32_t* sesseion_id() const { return this->session_id_; }
  // appid 和 user_id 需要判断是否为 nullptr, 才能使用
  inline char* appid() const { return this->appid_; }
  inline char* user_id() const { return this->user_id_; }

  inline uint8_t* msg_name_size() const { return this->msg_name_size_; }
  inline char* msg_name() const { return this->msg_name_; }
  inline char* msg_data() const { return this->msg_data_; }
  inline uint32_t* check_sum() const { return this->check_sum_; }
  inline std::size_t msg_data_size() const { return this->msg_data_size_; }

  bool Decode();
  bool Decode(UserBuffer* user_buf);

  void Encode(const uint32_t& session_id, const std::string& appid,
              const std::string& user_id,
              const google::protobuf::Message& proto_msg);
  void Encode(const uint32_t& session_id,
              const google::protobuf::Message& proto_msg);
  void Encode(const google::protobuf::Message& proto_msg);

  void Encode(const uint32_t& session_id, const std::string& appid,
              const std::string& userid, const common::UserBuffer& user_buf);
  void Encode(const uint32_t& session_id, const common::UserBuffer& user_buf);

 private:
  uint32_t* session_id_;
  char* appid_;
  char* user_id_;

  uint8_t* msg_name_size_;
  char* msg_name_;
  char* msg_data_;
  uint32_t* check_sum_;

  std::size_t msg_data_size_;
};

}  // namespace common

#endif  // COMMON_PIPE_BUFFER_H_
