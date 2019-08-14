/**
 * @file user_session_manager.h
 * @brief  UserSessionManager class
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-11-07
 */

#ifndef GAMESERVER_USER_SESSION_MANAGER_H_
#define GAMESERVER_USER_SESSION_MANAGER_H_

#include <map>
#include <memory>
#include <mutex>

#include "user_session.h"

class UserSessionManager {
 public:
  static UserSessionManager& Instance();
  void AddSession(const std::shared_ptr<UserSession>& user_session);
  void DelSession(const uint32_t& session_id);
  std::shared_ptr<UserSession> GetSession(const uint32_t& session_id) const;

 private:
  std::map<uint32_t, std::weak_ptr<UserSession>> user_sessions_;
  mutable std::mutex mutex_;
};

#endif  // GAMESERVER_USER_SESSION_MANAGER_H_
