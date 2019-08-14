/**
 * @file pipe.h
 * @brief Pipe 类
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-09-21
 */

#ifndef COMMON_PIPE_H_
#define COMMON_PIPE_H_

#include <deque>
#include <functional>
#include <memory>

#include <asio.hpp>

#include "log.h"
#include "user_buffer.h"

namespace common {

template <class Buffer>
class Pipe : public std::enable_shared_from_this<Pipe<Buffer>> {
 public:
  Pipe(asio::io_service& io_service)
      : io_service_(io_service), socket_(io_service), resolver_(io_service) {}

  Pipe(const Pipe&) = delete;
  Pipe& operator=(const Pipe&) = delete;
  virtual ~Pipe() {}

  void Connect(const std::string& ip, const std::string& port,
               const std::function<void(const asio::error_code&)>& callback =
                   [](const asio::error_code&) {}) {
    auto endpoint_iterator = this->resolver_.resolve({ip, port});
    this->DoConnect(endpoint_iterator, callback);
  }

  void Close() {
    this->OnClose();
    auto self = this->shared_from_this();
    this->io_service_.post([this, self]() {
      this->socket_.close();
      this->OnClosed();
    });
  }

  void Write(const std::shared_ptr<Buffer>& buf) {
    auto self = this->shared_from_this();
    io_service_.post([this, self, buf]() {
      bool write_in_progress = !write_bufs_.empty();
      write_bufs_.push_back(std::move(buf));
      if (!write_in_progress) {
        DoWrite();
      }
    });
  }

 private:
  void DoConnect(asio::ip::tcp::resolver::iterator endpoint_iterator,
                 const std::function<void(const asio::error_code&)>& callback) {
    auto self = this->shared_from_this();
    asio::async_connect(
        this->socket_, endpoint_iterator,
        [this, self, callback](const asio::error_code& ec,
                               asio::ip::tcp::resolver::iterator) {
          if (!ec) {
            this->ConfigSocket(&this->socket_);
            callback(ec);
            this->DoReadHeader();
          } else {
            callback(ec);
          }
        });
  }

  void DoReadHeader() {
    auto self = this->shared_from_this();
    asio::async_read(
        this->socket_,
        asio::buffer(this->read_buf_.get_buf(),
                     this->read_buf_.get_header_size()),
        [this, self](const asio::error_code& ec, std::size_t /*length*/) {
          if (!ec) {
            if (this->OnReadHeader()) {
              this->DoReadBody();
            } else {
              this->ReadHeaderError();
            }
          } else if (ec != asio::error::operation_aborted) {
            this->RecvError(ec);
          }
        });
  }

  void DoReadBody() {
    auto self = this->shared_from_this();
    asio::async_read(
        this->socket_,
        asio::buffer(this->read_buf_.get_body(),
                     this->read_buf_.get_body_size()),
        [this, self](const asio::error_code& ec, std::size_t /*length*/) {
          if (!ec) {
            if (this->OnReadBody(&this->read_buf_)) {
              this->DoReadHeader();
            } else {
              this->ReadBodyError();
            }
          } else if (ec != asio::error::operation_aborted) {
            this->RecvError(ec);
          }
        });
  }

  void DoWrite() {
    auto self = this->shared_from_this();
    asio::async_write(socket_,
                      asio::buffer(write_bufs_.front()->get_buf(),
                                   write_bufs_.front()->size()),
                      [this, self](std::error_code ec, std::size_t /*length*/) {
                        if (!ec) {
                          write_bufs_.pop_front();
                          if (!write_bufs_.empty()) {
                            DoWrite();
                          }
                        } else if (ec != asio::error::operation_aborted) {
                          SendError(ec);
                        }
                      });
  }

  virtual void ReadHeaderError() {
    LOG_ERROR("ReadHeaderError.");
    this->Close();
  }

  virtual void ReadBodyError() {
    LOG_ERROR("ReadBodyError.");
    this->Close();
  }

  virtual void RecvError(std::error_code ec) {
    LOG_ERROR("RecvError, value={}, what={}", ec.value(), ec.message());
    this->Close();
  }

  virtual void SendError(std::error_code ec) {
    LOG_ERROR("SendError, value={}, what={}", ec.value(), ec.message());
    this->Close();
  }

  bool OnReadHeader() { return this->read_buf_.SizeValid(); }

  virtual void ConfigSocket(asio::ip::tcp::socket* socket) {}
  virtual void OnClose() {}
  virtual void OnClosed() {}
  virtual bool OnReadBody(Buffer* read_buf) { return true; }

  asio::io_service& io_service_;
  asio::ip::tcp::socket socket_;
  asio::ip::tcp::resolver resolver_;
  Buffer read_buf_;
  std::deque<std::shared_ptr<Buffer>> write_bufs_;
};

}  // namespace common

#endif  // COMMON_PIPE_H_
