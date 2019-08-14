/**
 * @file user.cpp
 * @brief 用户类
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-11-20
 */

#include "user.h"
#include <chrono>
#include <functional>
#include <memory>

#include <asio.hpp>

#include "client_config.h"
#include "common/io_service_pool.h"
#include "gameclient.h"
#include "queueclient.h"
#include "user_processor.h"

User::User(asio::io_service& io_service) : user_processor_(io_service) {}

const std::string& User::get_appid() const { return this->appid_; }

const uint32_t& User::get_zone() const { return this->zone_; }

const std::string& User::get_uid() const { return this->uid_; }

void User::Init(const std::string& appid, const uint32_t& zone,
                const std::string& uid) {
  this->appid_ = appid;
  this->zone_ = zone;
  this->uid_ = uid;

  this->user_processor_.Init();

  std::string game_ip(ClientConfig::Instance().get_gameserver().ip);
  std::string game_port(ClientConfig::Instance().get_gameserver().port);
  auto self = this->shared_from_this();
  this->ConnectGame(game_ip, game_port, [=]() {
    LOG_INFO("Game connected, ip={}, port={}", game_ip, game_port);
    game::LoginReq proto_msg;
    proto_msg.set_type(game::kLoginTypeNormal);
    proto_msg.set_uid(self->get_uid());
    self->SendGameMsg(proto_msg);
    LOG_INFO("LoginReq, type={}, uid={}", proto_msg.type(), proto_msg.uid());
  });
}

const UserProcessor& User::get_user_processor() const {
  return this->user_processor_;
}

void User::ConnectGame(const std::string& ip, const std::string& port,
                       const std::function<void()>& callback) {
  auto gameclient = std::make_shared<GameClient>(
      common::IoServicePool::Instance().GetIoService(), shared_from_this());
  if (!gameclient) {
    return;
  }
  auto self = shared_from_this();
  gameclient->Connect(ip, port, [=](const asio::error_code& ec) {
    if (!ec) {
      self->gameclient_ = gameclient;
      callback();
    } else {
      LOG_ERROR("Connect game error, ip={}, port={}", ip, port);
    }
  });
}

void User::ConnectQueue(const std::string& ip, const std::string& port,
                        const std::function<void()>& callback) {
  auto queueclient = std::make_shared<QueueClient>(
      common::IoServicePool::Instance().GetIoService(), shared_from_this());
  if (!queueclient) {
    return;
  }
  auto self = shared_from_this();
  queueclient->Connect(ip, port, [=](const asio::error_code& ec) {
    if (!ec) {
      self->queueclient_ = queueclient;
      callback();
    } else {
      LOG_ERROR("Connect queue error, ip={}, port={}", ip, port);
    }

  });
  this->queueclient_ = queueclient;
}

void User::SendGameMsg(const ::google::protobuf::Message& proto_msg) {
  auto gameclient = this->gameclient_.lock();
  if (!gameclient) {
    LOG_ERROR("Gameclient disappear.");
    return;
  }
  gameclient->SendMsg(proto_msg);
}

void User::SendQueueMsg(const ::google::protobuf::Message& proto_msg) {
  auto queueclient = this->queueclient_.lock();
  if (!queueclient) {
    LOG_ERROR("Queueclient disappear.");
    return;
  }
  queueclient->SendMsg(proto_msg);
}

void User::CloseGameClient() {
  auto gameclient = this->gameclient_.lock();
  if (!gameclient) {
    LOG_ERROR("Gameserver disappear.");
  }

  gameclient->Close();
}

void User::CloseQueueClient() {
  auto queueclient = this->queueclient_.lock();
  if (!queueclient) {
    LOG_ERROR("Queueclient disappear.");
    return;
  }

  queueclient->Close();
}
