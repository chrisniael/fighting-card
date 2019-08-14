/**
 * @file user_session_manager.cpp
 * @brief  UserSessionManager class
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-11-07
 */

#include "user_session_manager.h"

#include "common/log.h"

UserSessionManager& UserSessionManager::Instance() {
  static UserSessionManager instance;
  return instance;
}

void UserSessionManager::AddSession(
    const std::shared_ptr<UserSession>& user_session) {
  std::lock_guard<std::mutex> lock_guard(this->mutex_);
  static uint32_t session_id = 0;
  ++session_id;
  if (session_id == 0) {
    ++session_id;
  }
  user_session->set_session_id(session_id);
  std::weak_ptr<UserSession> user_session_wp = user_session;
  this->user_sessions_.insert(std::make_pair(session_id, user_session_wp));
  LOG_INFO("AddSession, session_id={}, session_num={}", session_id,
           this->user_sessions_.size());
}

void UserSessionManager::DelSession(const uint32_t& session_id) {
  std::lock_guard<std::mutex> lock_guard(this->mutex_);
  this->user_sessions_.erase(session_id);
  LOG_INFO("DelSession, session_id={}, session_num={}", session_id,
           this->user_sessions_.size());
}

std::shared_ptr<UserSession> UserSessionManager::GetSession(
    const uint32_t& session_id) const {
  std::lock_guard<std::mutex> lock_guard(this->mutex_);
  auto it = this->user_sessions_.find(session_id);
  if (it != this->user_sessions_.end()) {
    return it->second.lock();
  }

  return nullptr;
}
