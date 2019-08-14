/**
 * @file user.h
 * @brief 用户类
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-11-20
 */

#ifndef GAMECLIENT_USER_H_
#define GAMECLIENT_USER_H_

#include <memory>

#include "common/log.h"
#include "gameclient.h"
#include "queueclient.h"
#include "user_processor.h"

class User : public std::enable_shared_from_this<User> {
 public:
  User(asio::io_service& io_service);

  const std::string& get_appid() const;
  const uint32_t& get_zone() const;
  const std::string& get_uid() const;

  void Init(const std::string& appid, const uint32_t& zone,
            const std::string& uid);
  const UserProcessor& get_user_processor() const;
  void ConnectGame(const std::string& ip, const std::string& port,
                   const std::function<void()>& callback);
  void ConnectQueue(const std::string& ip, const std::string& port,
                    const std::function<void()>& callback);
  void SendGameMsg(const ::google::protobuf::Message& proto_msg);
  void SendQueueMsg(const ::google::protobuf::Message& proto_msg);
  void CloseGameClient();
  void CloseQueueClient();
  void SendPingMsg();

 private:
  std::weak_ptr<GameClient> gameclient_;
  std::weak_ptr<QueueClient> queueclient_;
  UserProcessor user_processor_;
  std::string appid_;
  uint32_t zone_;
  std::string uid_;
};

#endif  // GAMECLIENT_USER_H_
