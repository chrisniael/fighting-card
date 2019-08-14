/**
 * @file gameserver.cpp
 * @brief 游戏服务器类
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-11-07
 */

#include "gameserver.h"

#include "common/io_service_pool.h"
#include "common/log.h"
#include "common/logic_io.h"
#include "user_session.h"
#include "user_session_manager.h"

GameServer::GameServer(asio::io_service& io_service)
    : common::Server<UserSession>(io_service) {}

void GameServer::ConfigAcceptor(asio::ip::tcp::acceptor& acceptor) {
  // this->acceptor_.set_option(asio::ip::tcp::no_delay(true));

  asio::socket_base::send_buffer_size SNDBUF(32 * 1024);
  asio::socket_base::receive_buffer_size RCVBUF(32 * 1024);
  acceptor.set_option(SNDBUF);
  acceptor.set_option(RCVBUF);
}

asio::io_service& GameServer::GetSessionIoService() {
  return common::IoServicePool::Instance().GetIoService();
}

void GameServer::OnAccept(const std::shared_ptr<UserSession>& user_session) {
  LOG_INFO("OnAccept.");
  // 分配 session id，添加到管理器
  common::LogicIo::Instance().get_io_service().post(
      [=]() { UserSessionManager::Instance().AddSession(user_session); });
}

void GameServer::OnAcceptError(const asio::error_code& ec) {
  LOG_ERROR("AcceptError, value={}, what={}", ec.value(), ec.message());
}
