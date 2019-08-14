/**
 * @file gameserver_processor.h
 * @brief  游戏服务器消息处理类
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-11-07
 */

#ifndef GAMESERVER_USER_SESSION_PROCESSOR_H_
#define GAMESERVER_USER_SESSION_PROCESSOR_H_

#include <asio.hpp>

#include "common/user_processor.h"
#include "game.pb.h"
#include "user_session.h"

class UserSessionProcessor
    : public common::user_processor::UserProcessor<UserSession> {
 public:
  static UserSessionProcessor& Instance();

  void Init(asio::io_service& io_service);

  void LoginReqHandle(const std::shared_ptr<UserSession>&,
                      const std::shared_ptr<game::LoginReq>&);

  void PingReqHandle(const std::shared_ptr<UserSession>&,
                     const std::shared_ptr<game::Ping>&);
};

#endif  // GAMESERVER_USER_SESSION_PROCESSOR_H_
