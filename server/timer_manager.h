/**
 * @file timer_manager.h
 * @brief 定时器
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-11-08
 */

#ifndef GAMESERVER_TIMER_MANAGER_H_

#include <chrono>
#include <memory>

#include <asio.hpp>

#include "common/asio_timer.h"

class TimerManager {
 public:
  TimerManager();

  static TimerManager& Instance();

  void Init(asio::io_service& io_service);

  void Timer1Sec();

  void Timer5Sec();

 private:
  std::shared_ptr<common::TimerManager<std::chrono::seconds>> timer_manager_;
};

#endif  // GAMESERVER_TIMER_MANAGER_H_
