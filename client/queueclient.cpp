/**
 * @file queueclient.cpp
 * @brief 排队客户端类
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-11-08
 */

#include "queueclient.h"

#include "common/log.h"
#include "common/utility.h"
#include "game.pb.h"
#include "user.h"

#define PING_SEND_FREQUENCY 2

QueueClient::QueueClient(asio::io_service& io_service,
                         const std::shared_ptr<User>& user)
    : common::Pipe<common::UserBuffer>(io_service),
      user_(user),
      timer_(std::make_shared<common::TimerManager<std::chrono::seconds>>(
          io_service)) {}

void QueueClient::Connect(
    const std::string& ip, const std::string& port,
    const std::function<void(const asio::error_code& ec)>& callback) {
  this->timer_->Add(PING_SEND_FREQUENCY, std::bind(&QueueClient::SendPingMsg,
                                                   this->shared_from_this()));

  auto self = this->shared_from_this();
  common::Pipe<common::UserBuffer>::Connect(
      ip, port, [self, callback](const asio::error_code& ec) {
        if (!ec) {
          self->timer_->Run();
        }
        callback(ec);
      });
}

void QueueClient::ConfigSocket(asio::ip::tcp::socket* socket) {
  socket->set_option(asio::ip::tcp::no_delay(true));
  socket->non_blocking(true);
  asio::socket_base::send_buffer_size SNDBUF(32 * 1024);
  asio::socket_base::receive_buffer_size RCVBUF(32 * 1024);
  socket->set_option(SNDBUF);
  socket->set_option(RCVBUF);
}

void QueueClient::OnClose() {
  LOG_INFO("OnClose.");
  this->timer_->Stop();
}

bool QueueClient::OnReadBody(common::UserBuffer* read_buf) {
  if (!read_buf->Decode()) {
    LOG_ERROR("Decode message fail.");
    return false;
  }
  LOG_INFO("Recv msg, msg_name={}", read_buf->msg_name());

  std::shared_ptr<User> user = this->user_.lock();
  if (!user) {
    LOG_ERROR("User disappear");
    return false;
  }
  auto user_processor = user->get_user_processor();
  common::user_processor::ErrorCode ret_value =
      user_processor.Dispatch(*read_buf, user);

  if (ret_value == common::user_processor::kErrorMsgNotRegister) {
    LOG_ERROR("Msg not register, msg_name={}", read_buf->msg_name());
  } else if (ret_value != common::user_processor::kOK) {
    LOG_ERROR("Msg dispatch error, ret_value={}", ret_value);
    return false;
  }

  return true;
}

void QueueClient::Write(const std::shared_ptr<common::UserBuffer>& buf) {
  this->last_write_time_ =
      std::chrono::duration_cast<std::chrono::seconds>(
          std::chrono::system_clock::now().time_since_epoch())
          .count();
  common::Pipe<common::UserBuffer>::Write(buf);
}

void QueueClient::SendMsg(const ::google::protobuf::Message& proto_msg) const {
  auto user_buf = std::make_shared<common::UserBuffer>(proto_msg);
  const_cast<QueueClient*>(this)->Write(user_buf);
}

void QueueClient::SendPingMsg() {
  uint64_t now_seconds =
      std::chrono::duration_cast<std::chrono::seconds>(
          std::chrono::system_clock::now().time_since_epoch())
          .count();
  if (now_seconds > this->last_write_time_ &&
      now_seconds - this->last_write_time_ >= PING_SEND_FREQUENCY) {
    game::Ping msg;
    this->SendMsg(msg);
    auto user = this->user_.lock();
    if (user) {
      LOG_TRACE("Send ping to gameserver, appid={}, zone={}, uid={}",
                user->get_appid(), user->get_zone(), user->get_uid());
    } else {
      LOG_TRACE("Send ping to gameserver");
    }
  }
}
