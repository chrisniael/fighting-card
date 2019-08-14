/**
 * @file user_processor.h
 * @brief 用户消息处理
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-11-20
 */

#ifndef GAMECLIENT_USER_PROCESSOR_H_
#define GAMECLIENT_USER_PROCESSOR_H_

#include <asio.hpp>

#include "common/user_processor.h"
#include "game.pb.h"

class User;

class UserProcessor : public common::user_processor::UserProcessor<User> {
 public:
  UserProcessor(asio::io_service& io_service);

  void Init();

  // Game 消息
  void GameLoginRspHandle(const std::shared_ptr<User>&,
                          const std::shared_ptr<game::LoginRsp>&);
};

#endif  // GAMECLIENT_USER_PROCESSOR_H_
