/**
 * @file main.cpp
 * @brief 游戏客户端 main
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-09-05
 */

#include <getopt.h>
#include <exception>
#include <iostream>
#include <string>
#include <thread>

#include <asio.hpp>

#include "client_config.h"
#include "common/daemon.h"
#include "common/io_service_pool.h"
#include "common/log.h"
#include "common/master_io.h"
#include "user.h"
#include "user_manager.h"

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

  if (false == ClientConfig::Instance().LoadConfig(config_path)) {
    return -1;
  }
  try {
    common::log::Log::Instance().Init(
        ClientConfig::Instance().get_log().name,
        ClientConfig::Instance().get_log().path,
        ClientConfig::Instance().get_log().level,
        ClientConfig::Instance().get_log().flush, !daemon_status,
        ClientConfig::Instance().get_log().thread);
  } catch (const std::exception& e) {
    std::cerr << "Exception, what=" << e.what() << std::endl;
    return 1;
  }

  try {
    LOG_INFO("GameClient starting, thread={}",
             ClientConfig::Instance().get_attr().thread);
    common::IoServicePool::Instance().Init(
        ClientConfig::Instance().get_attr().thread);

    for (auto uid = ClientConfig::Instance().get_usersinfo().uid_from;
         uid <= ClientConfig::Instance().get_usersinfo().uid_to; ++uid) {
      auto user = std::make_shared<User>(
          common::IoServicePool::Instance().GetIoService());
      user->Init(ClientConfig::Instance().get_usersinfo().appid,
                 ClientConfig::Instance().get_usersinfo().zone, std::to_string(uid));
      UserManager::Instance().AddUser(user);
      std::this_thread::sleep_for(std::chrono::milliseconds(800));
    }

    common::MasterIo::Instance().Run();
    common::IoServicePool::Instance().Join();
  } catch (const std::exception& e) {
    LOG_CRITICAL("Exception, what={}", e.what());
  }
}
