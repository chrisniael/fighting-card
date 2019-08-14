/**
 * @file user_session.h
 * @brief UserSession Class
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-11-07
 */

#ifndef GAMESERVER_USER_SESSION_H_
#define GAMESERVER_USER_SESSION_H_

#include <atomic>
#include <chrono>
#include <cstring>
#include <memory>
#include <string>

#include <google/protobuf/message.h>
#include <asio.hpp>

#include "common/asio_timer.h"
#include "common/session.h"
#include "common/user_buffer.h"

class UserSession : public common::Session<common::UserBuffer> {
 public:
  UserSession(asio::ip::tcp::socket&& socket);

  void set_session_id(const uint32_t& session_id) {
    this->session_id_ = session_id;
  }
  const uint32_t& get_session_id() const { return this->session_id_; }
  void set_authed(const bool authed) { this->authed_ = authed; }
  bool get_authed() const { return this->authed_; }
  void set_user_id(const std::string& user_id) { this->user_id_ = user_id; }
  const std::string& get_user_id() const { return this->user_id_; }

  virtual void Init();
  void SendMsg(const ::google::protobuf::Message& proto_msg);

 protected:
  std::shared_ptr<UserSession> shared_from_this() {
    return std::static_pointer_cast<UserSession>(
        common::Session<common::UserBuffer>::shared_from_this());
  }
  std::shared_ptr<const UserSession> shared_from_this() const {
    return std::static_pointer_cast<const UserSession>(
        common::Session<common::UserBuffer>::shared_from_this());
  }

  void CheckAlive();

 private:
  virtual void OnClose();
  virtual void OnClosed();
  virtual bool OnReadBody(common::UserBuffer* read_buf);

  uint32_t session_id_;
  std::atomic_bool authed_;
  std::string user_id_;
  std::atomic<uint64_t> last_alive_time_;
  std::shared_ptr<common::TimerManager<std::chrono::seconds>> timer_;
};

#endif  // GAMESERVER_USER_SESSION_H_
