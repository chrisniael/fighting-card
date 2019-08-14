/**
 * @file pipe_buffer.cpp
 * @brief PipeBuffer 类
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-09-12
 */

#include "pipe_buffer.h"

#include "log.h"

namespace common {

PipeBuffer::PipeBuffer() {}

PipeBuffer::PipeBuffer(const google::protobuf::Message& proto_msg) {
  this->Encode(proto_msg);
}

PipeBuffer::PipeBuffer(const uint32_t& session_id,
                       const google::protobuf::Message& proto_msg) {
  this->Encode(session_id, proto_msg);
}
PipeBuffer::PipeBuffer(const uint32_t& session_id, const std::string& appid,
                       const std::string& userid,
                       const google::protobuf::Message& proto_msg) {
  this->Encode(session_id, appid, userid, proto_msg);
}

PipeBuffer::PipeBuffer(const uint32_t& session_id,
                       const common::UserBuffer& user_buf) {
  this->Encode(session_id, "", "", user_buf);
}

PipeBuffer::PipeBuffer(const uint32_t& session_id, const std::string& appid,
                       const std::string& user_id,
                       const common::UserBuffer& user_buf) {
  this->Encode(session_id, appid, user_id, user_buf);
}

bool PipeBuffer::Decode(UserBuffer* user_buf) {
  // 解析 session_id
  this->session_id_ = reinterpret_cast<uint32_t*>(this->get_body());
  LOG_DEBUG("session_id={}", *this->session_id_);

  // 解析 appid_size
  uint8_t* appid_size =
      reinterpret_cast<uint8_t*>(this->get_body() + sizeof(*this->session_id_));
  LOG_DEBUG("appid_size={}", static_cast<uint32_t>(*appid_size));

  // 解析 appid
  if (*appid_size == 0) {
    this->appid_ = nullptr;
  } else {
    this->appid_ = reinterpret_cast<char*>(appid_size) + sizeof(*appid_size);
    if (*(this->appid_ + *appid_size - 1) != '\0') {
      return false;
    }
    LOG_DEBUG("appid={}", this->appid_);
  }

  // 解析 user_id_size
  uint8_t* user_id_size =
      reinterpret_cast<uint8_t*>(this->get_body() + sizeof(*this->session_id_) +
                                 sizeof(*appid_size) + *appid_size);
  LOG_DEBUG("user_id_size=", static_cast<uint32_t>(*user_id_size));

  // 解析 user_id
  if (*user_id_size == 0) {
    this->user_id_ = nullptr;
  } else {
    this->user_id_ =
        reinterpret_cast<char*>(user_id_size) + sizeof(*user_id_size);
    if (*(this->user_id_ + *user_id_size - 1) != '\0') {
      return false;
    }
    LOG_DEBUG("user_id={}", this->user_id_);
  }

  char* msg_name_size = this->get_body() + sizeof(*this->session_id_) +
                        sizeof(*appid_size) + *appid_size +
                        sizeof(*user_id_size) + *user_id_size;

  uint32_t user_buf_size = this->get_body_size() - sizeof(*this->session_id_) -
                           sizeof(*appid_size) - *appid_size -
                           sizeof(*user_id_size) - *user_id_size;
  memcpy(user_buf->get_body(), msg_name_size, user_buf_size);
  user_buf->set_body_size(user_buf_size);

  return true;
}

bool PipeBuffer::Decode() {
  // 解析 session_id
  this->session_id_ = reinterpret_cast<uint32_t*>(this->get_body());
  LOG_DEBUG("session_id={}", *this->session_id_);

  // 解析 appid_size
  uint8_t* appid_size =
      reinterpret_cast<uint8_t*>(this->get_body() + sizeof(*this->session_id_));
  LOG_DEBUG("appid_size={}", static_cast<uint32_t>(*appid_size));

  // 解析 appid
  if (*appid_size == 0) {
    this->appid_ = nullptr;
  } else {
    this->appid_ = reinterpret_cast<char*>(appid_size) + sizeof(*appid_size);
    if (*(this->appid_ + *appid_size - 1) != '\0') {
      return false;
    }
    LOG_DEBUG("appid={}", this->appid_);
  }

  // 解析 user_id_size
  uint8_t* user_id_size =
      reinterpret_cast<uint8_t*>(this->get_body() + sizeof(*this->session_id_) +
                                 sizeof(*appid_size) + *appid_size);
  LOG_DEBUG("user_id_size={}", static_cast<uint32_t>(*user_id_size));

  // 解析 user_id
  if (*user_id_size == 0) {
    this->user_id_ = nullptr;
  } else {
    this->user_id_ =
        reinterpret_cast<char*>(user_id_size) + sizeof(*user_id_size);
    if (*(this->user_id_ + *user_id_size - 1) != '\0') {
      return false;
    }
    LOG_DEBUG("user_id={}", this->user_id_);
  }

  // 解析消息名长度
  this->msg_name_size_ = reinterpret_cast<uint8_t*>(
      this->get_body() + sizeof(*this->session_id_) + sizeof(*appid_size) +
      *appid_size + sizeof(*user_id_size) + *user_id_size);
  LOG_DEBUG("msg_name_size={}",
            static_cast<std::size_t>(*this->msg_name_size_));
  if (*this->msg_name_size_ < 2) {
    LOG_ERROR("msg name size < 2, msg_name_size={}", *this->msg_name_size_);
    return false;
  }

  // 解析消息名
  this->msg_name_ = reinterpret_cast<char*>(this->msg_name_size_) +
                    sizeof(*this->msg_name_size_);
  if (*(this->msg_name_ + *this->msg_name_size_ - 1) != '\0') {
    LOG_ERROR("msg name has no \\0");
    return false;
  }
  LOG_DEBUG("msg_name={}", this->msg_name_);

  // 解析消息内容
  this->msg_data_ = this->msg_name_ + *this->msg_name_size_;
  this->msg_data_size_ = this->get_body_size() - sizeof(*this->msg_name_size_) -
                         *this->msg_name_size_ - sizeof(*this->check_sum_);

  // 消息名长度验证，不能过长

  return true;
}

void PipeBuffer::Encode(const uint32_t& session_id, const std::string& appid,
                        const std::string& user_id,
                        const google::protobuf::Message& proto_msg) {
  // session_id
  this->session_id_ = reinterpret_cast<uint32_t*>(this->get_body());
  *this->session_id_ = session_id;

  // appid_size
  uint8_t* appid_size =
      reinterpret_cast<uint8_t*>(this->get_body() + sizeof(*this->session_id_));

  // appid
  if (appid.size() == 0) {
    *appid_size = appid.size();
    this->appid_ = nullptr;
  } else {
    *appid_size = appid.size() + 1;
    this->appid_ = reinterpret_cast<char*>(appid_size) + sizeof(*appid_size);
    memcpy(this->appid_, appid.c_str(), appid.size());
    *(this->appid_ + *appid_size - 1) = '\0';
  }

  // user_id_size
  uint8_t* user_id_size =
      reinterpret_cast<uint8_t*>(this->get_body() + sizeof(*this->session_id_) +
                                 sizeof(*appid_size) + *appid_size);

  // user_id
  if (user_id.size() == 0) {
    *user_id_size = user_id.size();
    this->user_id_ = nullptr;
  } else {
    *user_id_size = user_id.size() + 1;
    this->user_id_ =
        reinterpret_cast<char*>(user_id_size) + sizeof(*user_id_size);
    memcpy(this->user_id_, user_id.c_str(), user_id.size());
    *(this->user_id_ + *user_id_size - 1) = '\0';
  }

  // msg_name_size
  this->msg_name_size_ = reinterpret_cast<uint8_t*>(
      this->get_body() + sizeof(*this->session_id_) + sizeof(*appid_size) +
      *appid_size + sizeof(*user_id_size) + *user_id_size);
  size_t msg_name_size = proto_msg.GetTypeName().size();
  if (msg_name_size != 0) {
    msg_name_size += 1;  // 包含 '\0'
  }
  *this->msg_name_size_ = msg_name_size;

  // msg_name
  this->msg_name_ = reinterpret_cast<char*>(this->msg_name_size_) +
                    sizeof(*this->msg_name_size_);
  if (*this->msg_name_size_ != 0) {
    memcpy(this->msg_name_, proto_msg.GetTypeName().c_str(),
           *this->msg_name_size_);
    *(this->msg_name_ + *this->msg_name_size_ - 1) = '\0';
  }

  // msg_data
  this->msg_data_ = this->msg_name_ + *this->msg_name_size_;
  proto_msg.SerializeToArray(this->msg_data_, proto_msg.ByteSize());

  uint32_t body_size = sizeof(*this->session_id_) + sizeof(*appid_size) +
                       *appid_size + sizeof(*user_id_size) + *user_id_size +
                       sizeof(*this->msg_name_size_) + *this->msg_name_size_ +
                       proto_msg.ByteSize() + sizeof(*this->check_sum_);
  LOG_DEBUG("Send body_size={}", body_size);
  this->set_body_size(body_size);
}

void PipeBuffer::Encode(const uint32_t& session_id,
                        const google::protobuf::Message& proto_msg) {
  this->Encode(session_id, "", "", proto_msg);
}

void PipeBuffer::Encode(const google::protobuf::Message& proto_msg) {
  this->Encode(0, "", "", proto_msg);
}

void PipeBuffer::Encode(const uint32_t& session_id, const std::string& appid,
                        const std::string& user_id,
                        const common::UserBuffer& user_buf) {
  // session_id
  this->session_id_ = reinterpret_cast<uint32_t*>(this->get_body());
  *this->session_id_ = session_id;

  // appid_size
  uint8_t* appid_size =
      reinterpret_cast<uint8_t*>(this->get_body() + sizeof(*this->session_id_));

  // appid
  if (appid.size() == 0) {
    *appid_size = appid.size();
    this->appid_ = nullptr;
  } else {
    *appid_size = appid.size() + 1;
    this->appid_ = reinterpret_cast<char*>(appid_size) + sizeof(*appid_size);
    memcpy(this->appid_, appid.c_str(), appid.size());
    *(this->appid_ + *appid_size - 1) = '\0';
  }

  // user_id_size
  uint8_t* user_id_size =
      reinterpret_cast<uint8_t*>(this->get_body() + sizeof(*this->session_id_) +
                                 sizeof(*appid_size) + *appid_size);

  // user_id
  if (user_id.size() == 0) {
    *user_id_size = user_id.size();
    this->user_id_ = nullptr;
  } else {
    *user_id_size = user_id.size() + 1;
    this->user_id_ =
        reinterpret_cast<char*>(user_id_size) + sizeof(*user_id_size);
    memcpy(this->user_id_, user_id.c_str(), user_id.size());
    *(this->user_id_ + *user_id_size - 1) = '\0';
  }

  char* buf = this->get_body() + sizeof(*this->session_id_) +
              sizeof(*appid_size) + *appid_size + sizeof(*user_id_size) +
              *user_id_size;
  memcpy(buf, user_buf.get_body(), user_buf.get_body_size());

  this->set_body_size(sizeof(*this->session_id_) + sizeof(*appid_size) +
                      *appid_size + sizeof(*user_id_size) + *user_id_size +
                      user_buf.get_body_size());
}

void PipeBuffer::Encode(const uint32_t& session_id,
                        const common::UserBuffer& user_buf) {
  this->Encode(session_id, "", "", user_buf);
}

}  // namespace common
