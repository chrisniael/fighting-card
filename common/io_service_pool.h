/**
 * @file io_service_pool.h
 * @brief io_service pool
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-09-04
 */

#ifndef COMMON_IO_SERVICE_POOL_H_
#define COMMON_IO_SERVICE_POOL_H_

#include <atomic>
#include <thread>
#include <vector>

#include <asio.hpp>

namespace common {

class IoServicePool {
 public:
  using Work = asio::io_service::work;
  using WorkPtr = std::unique_ptr<Work>;

  IoServicePool() = default;
  IoServicePool(const IoServicePool&) = delete;
  ~IoServicePool() {}
  IoServicePool& operator=(const IoServicePool&) = delete;

  static IoServicePool& Instance();
  void Init(std::size_t pool_size = std::thread::hardware_concurrency() - 1);
  asio::io_service& GetIoService() const;

  void Join();
  void ClearWork();
  void Stop();

 private:
  std::vector<std::unique_ptr<asio::io_service>> io_services_;
  std::vector<WorkPtr> works_;
  std::vector<std::thread> threads_;
  mutable std::atomic_size_t next_io_service_;
};

}  // namespace common

#endif  // COMMON_IO_SERVICE_POOL_H_
