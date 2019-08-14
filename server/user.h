/**
 * @file user.h
 * @brief 用户类
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-12-13
 */

#ifndef GAMESERVER_USER_H_
#define GAMESERVER_USER_H_

#include <chrono>
#include <memory>
#include <string>

#include <google/protobuf/message.h>

class User : public std::enable_shared_from_this<User> {
 public:
  User(const std::string& uid);

  const std::string& get_uid() const { return this->uid_; }

 private:
  std::string uid_;
};

#endif  // GAMESERVER_USER_H_
