/**
 * @file logic_io.h
 * @brief 逻辑线程 io_service
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-10-30
 */

#ifndef COMMON_LOGIC_IO_H_
#define COMMON_LOGIC_IO_H_

#include <thread>

#include <asio.hpp>

namespace common {

class LogicIo {
 public:
  LogicIo();

  static LogicIo& Instance();

  asio::io_service& get_io_service();

  void Init();
  void Join();
  void Stop();

 private:
  asio::io_service io_service_;
  asio::io_service::work empty_work_;
  std::thread thread_;
};

}  // namespace common

#endif  // COMMON_LOGIC_IO_H_
