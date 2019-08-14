/**
 * @file user_manager.cpp
 * @brief 用户管理类
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-12-13
 */

#include "user_manager.h"

UserManager& UserManager::Instance() {
  static UserManager instance;
  return instance;
}

bool UserManager::AddUser(const std::shared_ptr<User>& user) {
  auto it = this->users_.find(user->get_uid());
  if (it != this->users_.end()) {
    return false;
  }
  this->users_.insert(std::make_pair(user->get_uid(), user));
  return true;
}

std::shared_ptr<User> UserManager::FindUser(const std::string& uid) {
  auto it = this->users_.find(uid);
  if (it == this->users_.end()) {
    return nullptr;
  }

  return it->second;
}

void UserManager::DelUser(const std::string& uid) { this->users_.erase(uid); }
