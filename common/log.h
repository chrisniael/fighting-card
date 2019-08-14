/**
 * @file log.h
 * @brief 日志
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-11-01
 */

#ifndef COMMON_LOG_H_
#define COMMON_LOG_H_

#include <memory>
#include <string>

#include <spdlog/async.h>
#include <spdlog/spdlog.h>

namespace common {
namespace log {

class Log {
 public:
  Log();

  static Log& Instance();

  std::shared_ptr<spdlog::async_logger> get_logger();

  void Init(const std::string& log_name, const std::string& log_path,
            const spdlog::level::level_enum& log_level = spdlog::level::trace,
            const spdlog::level::level_enum& flush_level = spdlog::level::info,
            const bool& stdout_switch = false, const uint16_t& thread_num = 1);
  void Init(const std::string& log_name, const std::string& log_path,
            const std::string& log_level_name,
            const std::string& flush_level_name,
            const bool& stdout_switch = false, const uint16_t& thread_num = 1);

 protected:
  void set_logger(const std::shared_ptr<spdlog::async_logger>& logger);

 private:
  std::shared_ptr<spdlog::async_logger> logger_;
};

#define LOG_TRACE(fmt, ...)                         \
  common::log::Log::Instance().get_logger()->trace( \
      "[{}:{}:{}] " fmt, __FILENAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...)                         \
  common::log::Log::Instance().get_logger()->debug( \
      "[{}:{}:{}] " fmt, __FILENAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)                         \
  common::log::Log::Instance().get_logger()->info( \
      "[{}:{}:{}] " fmt, __FILENAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)                         \
  common::log::Log::Instance().get_logger()->warn( \
      "[{}:{}:{}] " fmt, __FILENAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...)                         \
  common::log::Log::Instance().get_logger()->error( \
      "[{}:{}:{}] " fmt, __FILENAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG_CRITICAL(fmt, ...)                         \
  common::log::Log::Instance().get_logger()->critical( \
      "[{}:{}:{}] " fmt, __FILENAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__)

}  // namespace log
}  // namespace common

#endif  // COMMON_LOG_H_
