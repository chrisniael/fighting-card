/**
 * @file session.h
 * @brief 会话类
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-09-21
 */

#ifndef COMMON_SESSION_H_
#define COMMON_SESSION_H_

#include <cstring>
#include <deque>
#include <memory>

#include <google/protobuf/message.h>
#include <asio.hpp>

#include "log.h"
#include "pipe_buffer.h"

namespace common {

template <class Buffer>
class Session : public std::enable_shared_from_this<Session<Buffer>> {
 public:
  Session(asio::ip::tcp::socket&& socket)
      : io_service_(socket.get_io_service()), socket_(std::move(socket)) {}

  Session(const Session&) = delete;
  Session& operator=(const Session&) = delete;
  virtual ~Session() {}

  asio::io_service& get_io_service() const { return this->io_service_; }

  virtual void Init() {}

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

 private:
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
    asio::async_write(
        socket_,
        asio::buffer(write_bufs_.front()->get_buf(),
                     write_bufs_.front()->size()),
        [this, self](asio::error_code ec, std::size_t /*length*/) {
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

  virtual void RecvError(asio::error_code ec) {
    LOG_ERROR("RecvError, value={}, what={}", ec.value(), ec.message());
    this->Close();
  }

  virtual void SendError(asio::error_code ec) {
    LOG_ERROR("SendError, value={}, what={}", ec.value(), ec.message());
    this->Close();
  }

  bool OnReadHeader() { return this->read_buf_.SizeValid(); }

  virtual void OnClose() {}
  virtual void OnClosed() {}
  virtual bool OnReadBody(Buffer* read_buf) { return true; }

  asio::io_service& io_service_;
  asio::ip::tcp::socket socket_;
  Buffer read_buf_;
  std::deque<std::shared_ptr<Buffer>> write_bufs_;
};

}  // namespace common

#endif  // COMMON_SESSION_H_
