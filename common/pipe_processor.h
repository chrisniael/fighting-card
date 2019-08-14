/**
 * @file pipe_processor.h
 * @brief Pipe 消息处理器
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-10-31
 */

#ifndef COMMON_PIPE_PROCESSOR_H_
#define COMMON_PIPE_PROCESSOR_H_

#include <functional>
#include <map>
#include <memory>

#include <google/protobuf/message.h>
#include <asio.hpp>

#include "log.h"
#include "pipe_buffer.h"
#include "pipe_head.h"
#include "utility.h"

namespace common {
namespace pipe_processor {

enum ErrorCode {
  kOK = 0,
  kErrorMsgNotRegister,
  kErrorReflectionProtobufFailed,
  kErrorProtobufCastFailed
};

template <typename PipeSession>
class PipeHandlerInterface {
 public:
  virtual ~PipeHandlerInterface() = default;
  virtual ErrorCode Process(asio::io_service& io_service,
                            const std::shared_ptr<PipeSession>& pipe_session,
                            const std::shared_ptr<common::PipeHead>& pipe_head,
                            const void* proto_data,
                            const uint32_t& proto_size) = 0;
};

template <typename PipeSession, typename ProtoMsg>
class PipeHandler : public PipeHandlerInterface<PipeSession> {
 public:
  typedef std::function<void(const std::shared_ptr<PipeSession>&,
                             const std::shared_ptr<common::PipeHead>&,
                             const std::shared_ptr<ProtoMsg>&)>
      HandlerFunc;

  PipeHandler(const HandlerFunc& f) : func_(f) {}

  virtual ErrorCode Process(asio::io_service& io_service,
                            const std::shared_ptr<PipeSession>& pipe_session,
                            const std::shared_ptr<common::PipeHead>& pipe_head,
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

    io_service.post([=]() { this->func_(pipe_session, pipe_head, real_msg); });

    return kOK;
  }

 private:
  HandlerFunc func_;
};

template <typename PipeSession>
class PipeProcessor {
 public:
  PipeProcessor() : io_service_(nullptr) {}
  virtual ~PipeProcessor() = default;

  void set_io_service(asio::io_service& io_service) {
    this->io_service_ = &io_service;
  }

  template <typename T, typename ProtoMsg>
  void AddHandler(T* obj,
                  void (T::*mem_func)(const std::shared_ptr<PipeSession>&,
                                      const std::shared_ptr<common::PipeHead>&,
                                      const std::shared_ptr<ProtoMsg>&)) {
    assert(this->io_service_);
    auto pipe_handler = std::make_shared<PipeHandler<PipeSession, ProtoMsg>>(
        std::bind(mem_func, obj, std::placeholders::_1, std::placeholders::_2,
                  std::placeholders::_3));
    auto insert_res = this->handlers_.insert(
        std::make_pair(ProtoMsg::descriptor()->full_name(), pipe_handler));
    assert(insert_res.second);
  }

  ErrorCode Dispatch(const common::PipeBuffer& pipe_buffer,
                     const std::shared_ptr<PipeSession>& pipe_session) {
    assert(this->io_service_);
    auto it = this->handlers_.find(pipe_buffer.msg_name());
    if (it == this->handlers_.end()) {
      return kErrorMsgNotRegister;
    }

    auto pipe_head = std::make_shared<common::PipeHead>();
    const uint32_t& session_id = *pipe_buffer.sesseion_id();
    pipe_head->session_id = session_id;

    if (pipe_buffer.appid() && pipe_buffer.user_id()) {
      const auto& appid = pipe_buffer.appid();
      const auto& user_id = pipe_buffer.user_id();
      LOG_DEBUG("appid={}, user_id={}", appid, user_id);
      pipe_head->appid = appid;
      pipe_head->user_id = user_id;
    } else {
      LOG_DEBUG("No appid and user_id.");
    }

    return it->second->Process(*(this->io_service_), pipe_session, pipe_head,
                               pipe_buffer.msg_data(),
                               pipe_buffer.msg_data_size());
  }

 private:
  asio::io_service* io_service_;
  std::map<std::string, std::shared_ptr<PipeHandlerInterface<PipeSession>>>
      handlers_;
};

}  // namespace pipe_processor
}  // namespace common

#endif  // COMMON_PIPE_PROCESSOR_H_
