/**
 * @file io_service_pool.cpp
 * @brief io_service pool
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-09-04
 */

#include "io_service_pool.h"

namespace common {

IoServicePool& IoServicePool::Instance() {
  static IoServicePool instance;
  return instance;
}

void IoServicePool::Init(std::size_t pool_size) {
  next_io_service_ = 0;

  for (size_t i = 0; i < pool_size; ++i) {
    io_services_.emplace_back(new asio::io_service);
  }

  for (size_t i = 0; i < io_services_.size(); ++i) {
    works_.emplace_back(new Work(*(io_services_[i])));
  }

  for (size_t i = 0; i < io_services_.size(); ++i) {
    threads_.emplace_back([this, i]() { io_services_[i]->run(); });
  }
}

asio::io_service& IoServicePool::GetIoService() const {
  auto& service = io_services_[next_io_service_++ % io_services_.size()];
  return *service.get();
}

void IoServicePool::Join() {
  for (auto& t : threads_) {
    t.join();
  }
}

void IoServicePool::ClearWork() {
  for (auto& work : this->works_) {
    work.reset();
  }
}

void IoServicePool::Stop() {
  for (auto& io_service : this->io_services_) {
    io_service->stop();
  }
}

}  // namespace common
