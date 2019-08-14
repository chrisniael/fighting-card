/**
 * @file user_manager.cpp
 * @brief 用户管理类
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-12-12
 */

#include "user_manager.h"

#include "common/log.h"
#include "game.pb.h"

#define PING_SEND_FREQUENCY 2

UserManager& UserManager::Instance() {
  static UserManager instance;
  return instance;
}

void UserManager::AddUser(const std::shared_ptr<User>& user) {
  this->users_.insert(user);
}
