/**
 * @file queueclient.h
 * @brief 排队客户端类
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-11-08
 */

#ifndef GAMECLIENT_QUEUECLIENT_H_
#define GAMECLIENT_QUEUECLIENT_H_

#include <atomic>
#include <chrono>
#include <memory>
#include <string>

#include <google/protobuf/message.h>
#include <asio.hpp>

#include "common/asio_timer.h"
#include "common/pipe.h"
#include "common/user_buffer.h"

class User;

class QueueClient : public common::Pipe<common::UserBuffer> {
 public:
  QueueClient(asio::io_service& io_service, const std::shared_ptr<User>& user);

  void Connect(const std::string& ip, const std::string& port,
               const std::function<void(const asio::error_code& ec)>& callback);
  void Write(const std::shared_ptr<common::UserBuffer>& buf);
  void SendMsg(const ::google::protobuf::Message& proto_msg) const;

 protected:
  std::shared_ptr<QueueClient> shared_from_this() {
    return std::static_pointer_cast<QueueClient>(
        common::Pipe<common::UserBuffer>::shared_from_this());
  }

  std::shared_ptr<const QueueClient> shared_from_this() const {
    return std::static_pointer_cast<const QueueClient>(
        common::Pipe<common::UserBuffer>::shared_from_this());
  }

  void SendPingMsg();

 private:
  virtual void ConfigSocket(asio::ip::tcp::socket* socket);
  virtual void OnClose();
  virtual bool OnReadBody(common::UserBuffer* read_buf);

  std::weak_ptr<User> user_;
  std::atomic<uint64_t> last_write_time_;
  std::shared_ptr<common::TimerManager<std::chrono::seconds>> timer_;
};

#endif  // GAMECLIENT_QUEUECLIENT_H_
