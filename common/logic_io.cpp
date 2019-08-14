/**
 * @file logic_io.cpp
 * @brief 逻辑线程 io_service
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-10-30
 */

#include "logic_io.h"

namespace common {

LogicIo::LogicIo() : empty_work_(io_service_) {}

LogicIo& LogicIo::Instance() {
  static LogicIo instance;
  return instance;
}

asio::io_service& LogicIo::get_io_service() {
  return this->io_service_;
}

void LogicIo::Init() {
  this->thread_ = std::thread([&]() { this->io_service_.run(); });
}

void LogicIo::Join() { this->thread_.join(); }

void LogicIo::Stop() { this->io_service_.stop(); }

}  // namespace common
