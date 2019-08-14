/**
 * @file user_buffer.cpp
 * @brief UserBuffer 类
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-09-12
 */

#include "user_buffer.h"

#include "log.h"

namespace common {

UserBuffer::UserBuffer() {}

UserBuffer::UserBuffer(const google::protobuf::Message& proto_msg) {
  this->Encode(proto_msg);
}

bool UserBuffer::Decode() {
  // 解析消息名长度
  uint8_t* msg_name_size = reinterpret_cast<uint8_t*>(this->get_body());
  LOG_DEBUG("msg_name_size={}", static_cast<std::size_t>(*msg_name_size));
  if (*msg_name_size < 2) {
    LOG_ERROR("msg name size < 2, msg_name_size={}", *msg_name_size);
    return false;
  }

  // 解析消息名
  this->msg_name_ =
      reinterpret_cast<char*>(msg_name_size) + sizeof(*msg_name_size);
  if (*(this->msg_name_ + *msg_name_size - 1) != '\0') {
    LOG_ERROR("msg name has no \\0");
    return false;
  }
  LOG_DEBUG("msg_name={}, msg_name_size={}", this->msg_name_,
            static_cast<uint32_t>(*msg_name_size));

  // 解析消息内容
  this->msg_data_ = this->msg_name_ + *msg_name_size;
  this->msg_data_size_ = this->get_body_size() - 1 - *msg_name_size - 4;

  // 解析校验码
  this->check_sum_ =
      reinterpret_cast<uint32_t*>(this->msg_data_ + this->msg_data_size_);

  // 消息名长度验证，不能过长

  // 验证 CheckSum

  return true;
}

void UserBuffer::Encode(const google::protobuf::Message& proto_msg) {
  const std::string& msg_name = proto_msg.GetTypeName();
  LOG_DEBUG("msg_name={}", msg_name);

  uint8_t msg_name_size = msg_name.size() + 1;  // 包含 '\0'
  uint16_t msg_data_size = proto_msg.ByteSize();
  uint16_t body_size = 1 + msg_name_size + msg_data_size + 4;

  char* buf_msg_name = this->get_body() + sizeof(msg_name_size);
  char* buf_msg_data = buf_msg_name + msg_name_size;
  char* buf_check_sum = buf_msg_data + msg_data_size;

  // size
  memcpy(this->get_buf(), &body_size, this->get_header_size());

  // msg name size
  memcpy(this->get_body(), &msg_name_size, sizeof(msg_name_size));

  // msg name
  memcpy(buf_msg_name, const_cast<char*>(msg_name.c_str()), msg_name_size);

  // msg data
  proto_msg.SerializeToArray(buf_msg_data, msg_data_size);

  // chec sum
  // memcpy(buf_check_sum, "", 0);

  this->set_body_size(body_size);
}

}  // namespace common
