/**
 * @file timer_manager.cpp
 * @brief 定时器
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-11-08
 */

#include "timer_manager.h"

#include <memory>

#include "common/log.h"
#include "user_session_manager.h"

TimerManager::TimerManager() : timer_manager_(nullptr) {}

TimerManager& TimerManager::Instance() {
  static TimerManager instance;
  return instance;
}

void TimerManager::Init(asio::io_service& io_service) {
  this->timer_manager_ =
      std::make_shared<common::TimerManager<std::chrono::seconds>>(io_service);
  this->timer_manager_->Add(this, 1, &TimerManager::Timer1Sec);
  this->timer_manager_->Run();
}

void TimerManager::Timer1Sec() {
  LOG_TRACE("Timer1Sec.");
}
