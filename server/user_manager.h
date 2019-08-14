/**
 * @file user_manager.h
 * @brief 用户管理类
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-12-13
 */

#ifndef GAMESERVER_USER_MANAGER_H_
#define GAMESERVER_USER_MANAGER_H_

#include <memory>
#include <unordered_map>

#include "user.h"

class UserManager {
 public:
  static UserManager& Instance();

  bool AddUser(const std::shared_ptr<User>& user);
  std::shared_ptr<User> FindUser(const std::string& uid);
  void DelUser(const std::string& uid);

 private:
  UserManager() = default;

  std::unordered_map<std::string, std::shared_ptr<User>> users_;
};

#endif  // QUEUESERVER_USER_MANAGER_H_
