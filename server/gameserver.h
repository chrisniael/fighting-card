/**
 * @file gameserver.h
 * @brief 游戏服务器类
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-11-07
 */

#ifndef GAMESERVER_GAMESERVER_H_
#define GAMESERVER_GAMESERVER_H_

#include <memory>
#include <string>

#include <asio.hpp>

#include "common/server.h"
#include "user_session.h"

class GameServer : public common::Server<UserSession> {
 public:
  GameServer(asio::io_service& io_service);

 private:
  virtual void ConfigAcceptor(asio::ip::tcp::acceptor& acceptor);
  virtual asio::io_service& GetSessionIoService();
  virtual void OnAccept(const std::shared_ptr<UserSession>& user_session);
  virtual void OnAcceptError(const asio::error_code& ec);
};

#endif  // GAMESERVER_GAMESERVER_H_
