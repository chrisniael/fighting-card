/**
 * @file server.h
 * @brief 服务器类
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-09-23
 */

#ifndef COMMON_SERVER_H_
#define COMMON_SERVER_H_

#include <memory>
#include <string>

#include <asio.hpp>

namespace common {

template <class Session>
class Server {
 public:
  Server(asio::io_service& io_service) : acceptor_(io_service) {}
  virtual ~Server() = default;

  Server& operator=(const Server&) = delete;
  Server& operator=(const Server&&) = delete;

  void Accept(const std::string& address, const std::string& port) {
    asio::ip::tcp::resolver resolver(this->acceptor_.get_io_service());
    asio::ip::tcp::endpoint endpoint = *resolver.resolve(address, port).begin();
    this->acceptor_.open(endpoint.protocol());
    this->acceptor_.set_option(asio::socket_base::reuse_address(true));

    this->ConfigAcceptor(this->acceptor_);

    this->acceptor_.bind(endpoint);
    this->acceptor_.listen();
    this->DoAccept();
  }

 private:
  void DoAccept() {
    acceptor_.async_accept(
        this->GetSessionIoService(),
        [=](const asio::error_code& ec, asio::ip::tcp::socket session_socket) {
          if (!ec) {
            auto session = std::make_shared<Session>(std::move(session_socket));
            session->Init();
            this->OnAccept(session);
            session->DoReadHeader();
            DoAccept();
          } else {
            this->OnAcceptError(ec);
          }
        });
  }

  virtual void ConfigAcceptor(asio::ip::tcp::acceptor& acceptor) {}
  virtual asio::io_service& GetSessionIoService() = 0;
  virtual void OnAccept(const std::shared_ptr<Session>& session) {}
  virtual void OnAcceptError(const asio::error_code& ec) {}

  asio::ip::tcp::acceptor acceptor_;
};

}  // namespace common

#endif  // COMMON_SERVER_H_
