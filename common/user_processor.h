/**
 * @file user_processor.h
 * @brief User 消息处理函数
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-10-31
 */

#ifndef COMMON_USER_PROCESSOR_H_
#define COMMON_USER_PROCESSOR_H_

#include <functional>
#include <map>
#include <memory>

#include <google/protobuf/message.h>
#include <asio.hpp>

#include "user_buffer.h"
#include "utility.h"

namespace common {
namespace user_processor {

enum ErrorCode {
  kOK = 0,
  kErrorMsgNotRegister,
  kErrorReflectionProtobufFailed,
  kErrorProtobufCastFailed
};

template <typename UserSession>
class UserHandlerInterface {
 public:
  virtual ~UserHandlerInterface() = default;
  virtual ErrorCode Process(asio::io_service& io_service,
                            const std::shared_ptr<UserSession>& user_session,
                            const void* proto_data,
                            const uint32_t& proto_size) = 0;
};

template <typename UserSession, typename ProtoMsg>
class UserHandler : public UserHandlerInterface<UserSession> {
 public:
  typedef std::function<void(const std::shared_ptr<UserSession>&,
                             const std::shared_ptr<ProtoMsg>&)>
      HandlerFunc;

  UserHandler(const HandlerFunc& f) : func_(f) {}

  virtual ErrorCode Process(asio::io_service& io_service,
                            const std::shared_ptr<UserSession>& user_session,
                            const void* proto_data,
                            const uint32_t& proto_size) {
    std::shared_ptr<google::protobuf::Message> msg =
        common::CreateMessage(ProtoMsg::descriptor()->full_name());
    if (!msg) {
      return kErrorReflectionProtobufFailed;
    }
    msg->ParseFromArray(proto_data, proto_size);

    std::shared_ptr<ProtoMsg> real_msg =
        std::dynamic_pointer_cast<ProtoMsg>(msg);
    if (!real_msg) {
      return kErrorProtobufCastFailed;
    }

    io_service.post([=]() { this->func_(user_session, real_msg); });

    return kOK;
  }

 private:
  HandlerFunc func_;
};

template <typename UserSession>
class UserProcessor {
 public:
  UserProcessor() : io_service_(nullptr) {}
  virtual ~UserProcessor() = default;

  void set_io_service(asio::io_service& io_service) {
    this->io_service_ = &io_service;
  }

  template <typename T, typename ProtoMsg>
  void AddHandler(T* obj,
                  void (T::*mem_func)(const std::shared_ptr<UserSession>&,
                                      const std::shared_ptr<ProtoMsg>&)) {
    assert(this->io_service_);
    auto pipe_handler = std::make_shared<UserHandler<UserSession, ProtoMsg>>(
        std::bind(mem_func, obj, std::placeholders::_1, std::placeholders::_2));
    auto insert_res = this->handlers_.insert(
        std::make_pair(ProtoMsg::descriptor()->full_name(), pipe_handler));
    assert(insert_res.second);
  }

  ErrorCode Dispatch(const common::UserBuffer& pipe_buffer,
                     const std::shared_ptr<UserSession>& user_session) {
    assert(this->io_service_);
    auto it = this->handlers_.find(pipe_buffer.msg_name());
    if (it == this->handlers_.end()) {
      return kErrorMsgNotRegister;
    }

    return it->second->Process(*(this->io_service_), user_session,
                               pipe_buffer.msg_data(),
                               pipe_buffer.msg_data_size());
  }

 private:
  asio::io_service* io_service_;
  std::map<std::string, std::shared_ptr<UserHandlerInterface<UserSession>>>
      handlers_;
};

}  // namespace user_processor
}  // namespace common

#endif  // COMMON_USER_PROCESSOR_H_
