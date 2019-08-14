/**
 * @file master_io.h
 * @brief 主线程 io_service
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-09-20
 */

#ifndef COMMON_MASTER_IO_H_
#define COMMON_MASTER_IO_H_

#include <asio.hpp>

namespace common {

class MasterIo {
 public:
  static MasterIo& Instance();

  inline asio::io_service& get_io_service() { return this->io_service_; }

  void Run();
  void Stop();

 private:
  asio::io_service io_service_;
};

}  // namespace common

#endif  // COMMON_MASTER_IO_H_
