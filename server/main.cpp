/**
 * @file main.cpp
 * @brief 游戏服务器 main
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-11-07
 */

#include <getopt.h>
#include <exception>
#include <iostream>
#include <string>

#include <asio.hpp>

#include "common/daemon.h"
#include "common/io_service_pool.h"
#include "common/log.h"
#include "common/logic_io.h"
#include "common/master_io.h"
#include "common/utility.h"
#include "gameserver.h"
#include "server_config.h"
#include "timer_manager.h"
#include "user_session_processor.h"

static void Tip() { std::cout << "usage: --config <path> [-d]" << std::endl; }

int main(int argc, char** argv) {
  int ch;
  bool daemon_status = false;
  std::string config_path;
  struct option opts[] = {{"config", 1, NULL, 'c'}, {NULL, 0, NULL, '?'}};
  while ((ch = getopt_long(argc, argv, "d", opts, NULL)) != -1) {
    switch (ch) {
      case 'd': {
        daemon_status = true;
        break;
      }
      case 'c': {
        config_path = optarg;
        break;
      }
      case '?': {
        Tip();
        return 1;
      }
      default: {
        Tip();
        return 1;
      }
    }
  }

  if (config_path.empty()) {
    Tip();
    return 1;
  }
  if (daemon_status) {
    common::CreateDaemon();
  }
  if (false == ServerConfig::Instance().LoadConfig(config_path)) {
    return -1;
  }
  try {
    common::log::Log::Instance().Init(
        ServerConfig::Instance().get_log().name,
        ServerConfig::Instance().get_log().path,
        ServerConfig::Instance().get_log().level,
        ServerConfig::Instance().get_log().flush, !daemon_status,
        ServerConfig::Instance().get_log().thread);
  } catch (const std::exception& e) {
    std::cerr << "Exception, what=" << e.what() << std::endl;
    return 1;
  }

  try {
    LOG_INFO("GameServer starting, ip={}, port={}",
             ServerConfig::Instance().get_attr().ip,
             ServerConfig::Instance().get_attr().port);
    common::IoServicePool::Instance().Init(
        ServerConfig::Instance().get_attr().thread);
    common::LogicIo::Instance().Init();

    common::LogicIo::Instance().get_io_service().post([]() {
      UserSessionProcessor::Instance().Init(
          common::LogicIo::Instance().get_io_service());

      TimerManager::Instance().Init(
          common::LogicIo::Instance().get_io_service());
    });

    GameServer gateserver(common::MasterIo::Instance().get_io_service());
    gateserver.Accept(ServerConfig::Instance().get_attr().ip,
                      ServerConfig::Instance().get_attr().port);

    // 所有初始化写在上面
    common::MasterIo::Instance().Run();
    // 这行之后的代码不会被执行，除非 MasterIo Stop

    common::LogicIo::Instance().Join();
    common::IoServicePool::Instance().Join();
  } catch (const std::exception& e) {
    LOG_CRITICAL("Exception, what={}", e.what());
    return 1;
  }

  return 0;
}
