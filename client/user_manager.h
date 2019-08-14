/**
 * @file user_manager.h
 * @brief 用户管理类
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-12-12
 */

#ifndef GAMECLIENT_USER_MANAGER_H_
#define GAMECLIENT_USER_MANAGER_H_

#include <memory>
#include <set>

#include "user.h"

class UserManager {
 public:
  static UserManager& Instance();

  void AddUser(const std::shared_ptr<User>& user);

 private:
  UserManager() = default;

  std::set<std::shared_ptr<User>> users_;
};

#endif  // GAMECLIENT_USER_MANAGER_H_
