/**
 * @file master_io.cpp
 * @brief 主线程 io_service
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-09-20
 */

#include "master_io.h"

namespace common {

MasterIo& MasterIo::Instance() {
  static MasterIo instance;
  return instance;
}

void MasterIo::Run() { this->io_service_.run(); }

void MasterIo::Stop() { this->io_service_.stop(); }

}  // namespace common
