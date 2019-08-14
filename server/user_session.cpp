/**
 * @file user_session.cpp
 * @brief UserSession Class
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-11-07
 */

#include "user_session.h"

#include "common/log.h"
#include "common/logic_io.h"
#include "common/master_io.h"
#include "common/utility.h"
#include "user_manager.h"
#include "user_session_manager.h"
#include "user_session_processor.h"

#define CHECK_ALIVE_TIME 5

UserSession::UserSession(asio::ip::tcp::socket&& socket)
    : common::Session<common::UserBuffer>(std::move(socket)),
      session_id_(0),
      authed_(false),
      last_alive_time_(std::chrono::duration_cast<std::chrono::seconds>(
                           std::chrono::system_clock::now().time_since_epoch())
                           .count()) {}
void UserSession::Init() {
  this->timer_ = std::make_shared<common::TimerManager<std::chrono::seconds>>(
      this->get_io_service());
  this->timer_->Add(CHECK_ALIVE_TIME, std::bind(&UserSession::CheckAlive,
                                                this->shared_from_this()));
  this->timer_->Run();
}

void UserSession::SendMsg(const ::google::protobuf::Message& proto_msg) {
  auto user_buf = std::make_shared<common::UserBuffer>(proto_msg);
  this->Write(user_buf);
}

void UserSession::CheckAlive() {
  uint64_t now_seconds =
      std::chrono::duration_cast<std::chrono::seconds>(
          std::chrono::system_clock::now().time_since_epoch())
          .count();
  if (now_seconds > this->last_alive_time_ &&
      now_seconds - this->last_alive_time_ >= CHECK_ALIVE_TIME) {
    LOG_WARN("User Session not alive, session_id={}", this->session_id_);
    this->Close();
  }
}

void UserSession::OnClose() {
  this->timer_->Stop();

  LOG_INFO("OnClose, session_id={}", this->get_session_id());
  auto self = this->shared_from_this();
  common::LogicIo::Instance().get_io_service().post([self, this]() {
    UserManager::Instance().DelUser(this->get_user_id());
    UserSessionManager::Instance().DelSession(this->get_session_id());
  });
}

void UserSession::OnClosed() {
  LOG_INFO("OnClosed, session_id={}", this->get_session_id());
  auto self = this->shared_from_this();
  common::LogicIo::Instance().get_io_service().post([self, this]() {
    if (this->get_authed()) {
      LOG_INFO("User offline, uid={}", this->get_user_id());
    }
  });
}

bool UserSession::OnReadBody(common::UserBuffer* read_buf) {
  if (!read_buf->Decode()) {
    LOG_ERROR("Decode message fail.");
    return false;
  }

  uint32_t session_id = this->get_session_id();
  LOG_INFO("Recv msg, msg_name={}, session_id={}", read_buf->msg_name(),
           session_id);

  this->last_alive_time_ =
      std::chrono::duration_cast<std::chrono::seconds>(
          std::chrono::system_clock::now().time_since_epoch())
          .count();
  common::user_processor::ErrorCode ret_value =
      UserSessionProcessor::Instance().Dispatch(*read_buf,
                                                this->shared_from_this());
  if (ret_value == common::user_processor::kErrorMsgNotRegister) {
    LOG_ERROR("Msg not register, msg_name={}", read_buf->msg_name());
    return false;
  } else if (ret_value != common::user_processor::kOK) {
    LOG_ERROR("Msg dispatch error, ret_value={}", ret_value);
    return false;
  }

  return true;
}
