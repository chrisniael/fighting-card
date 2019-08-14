/**
 * @file asio_timer.h
 * @brief asio 定时器
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-11-08
 */

#ifndef COMMON_ASIO_TIMER_H_
#define COMMON_ASIO_TIMER_H_

#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

#include <asio.hpp>
#include <asio/steady_timer.hpp>

namespace common {

template <typename TimeUnit>
class TimerManager
    : public std::enable_shared_from_this<TimerManager<TimeUnit>> {
 public:
  struct TimerItem {
    TimerItem(asio::io_service& io_service, int seconds,
              const std::function<void()>& f)
        : timer(io_service), duration(seconds), func(f) {}
    asio::steady_timer timer;
    TimeUnit duration;
    std::function<void()> func;
  };

  TimerManager(asio::io_service& io_service) : io_service_(io_service) {}
  TimerManager(TimerManager&) = delete;
  TimerManager& operator=(const TimerManager&) = delete;

  template <typename T>
  void Add(T* obj, const unsigned int& duration, void (T::*mem_func)()) {
    std::lock_guard<std::mutex> lg(this->mt_);
    std::function<void()> func = std::bind(mem_func, obj);
    this->items_.push_back(
        std::make_shared<TimerItem>(this->io_service_, duration, func));
  }

  void Add(const unsigned int& duration, const std::function<void()>& func) {
    std::lock_guard<std::mutex> lg(this->mt_);
    this->items_.push_back(
        std::make_shared<TimerItem>(this->io_service_, duration, func));
  }

  void Run() {
    std::lock_guard<std::mutex> lg(this->mt_);
    for (auto& item : this->items_) {
      asio::steady_timer& timer = item->timer;
      const TimeUnit& duration = item->duration;
      const std::function<void()>& func = item->func;
      TimerLoop(timer, duration, func);
    }
  }

  void Stop() {
    std::lock_guard<std::mutex> lg(this->mt_);
    for (auto& item : this->items_) {
      asio::steady_timer& timer = item->timer;
      std::size_t sz = timer.cancel();
    }
  }

 protected:
  void TimerLoop(asio::steady_timer& timer, const TimeUnit& duration,
                 const std::function<void()>& func) {
    auto self = this->shared_from_this();
    timer.expires_from_now(duration);
    timer.async_wait(
        [self, &timer, duration, func](const asio::error_code& ec) {
          if (ec != asio::error::operation_aborted) {
            self->TimerLoop(timer, duration, func);
            func();
          }
        });
  }

 private:
  asio::io_service& io_service_;
  std::vector<std::shared_ptr<TimerItem>> items_;
  std::mutex mt_;
};

}  // namespace common

#endif  // COMMON_ASIO_TIMER_H_
