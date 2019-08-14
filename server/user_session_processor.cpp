/**
 * @file gameserver_processor.cpp
 * @brief  游戏服务器消息处理类
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-11-07
 */

#include "user_session_processor.h"

#include <chrono>
#include <map>
#include <mutex>

#include "common/log.h"
#include "game.pb.h"
#include "server_config.h"
#include "user.h"
#include "user_manager.h"

static std::map<long, size_t> login_req_freq;
static std::mutex login_req_freq_mt;

UserSessionProcessor& UserSessionProcessor::Instance() {
  static UserSessionProcessor instance;
  return instance;
}

void UserSessionProcessor::Init(asio::io_service& io_service) {
  this->set_io_service(io_service);

  this->AddHandler(this, &UserSessionProcessor::LoginReqHandle);
  this->AddHandler(this, &UserSessionProcessor::PingReqHandle);
}

void UserSessionProcessor::LoginReqHandle(
    const std::shared_ptr<UserSession>& user_session,
    const std::shared_ptr<game::LoginReq>& login_req_msg) {
  LOG_INFO("LoginReq, type={}, uid={}, token={}", login_req_msg->type(),
           login_req_msg->uid(), login_req_msg->token());

  if (login_req_msg->type() == game::kLoginTypeNormal) {
    const std::string& uid = login_req_msg->uid();
    // 这里验证身份的步骤省略

    auto user = UserManager::Instance().FindUser(uid);
    if (user) {
      game::LoginRsp login_rsp_msg;
      login_rsp_msg.set_result(game::kLoginResLogined);
      user_session->SendMsg(login_rsp_msg);
      LOG_ERROR("User has logined, uid={}", uid);
      return;
    }

    auto new_user = std::make_shared<User>(uid);
    UserManager::Instance().AddUser(new_user);
  } else {
  }

  // 身份验证通过
  user_session->set_user_id(login_req_msg->uid());
  user_session->set_authed(true);

  game::LoginRsp login_rsp_msg;
  login_rsp_msg.set_result(game::kLoginResSuccess);
  user_session->SendMsg(login_rsp_msg);
  LOG_INFO("LoginRsp, uid={}, result={}", login_req_msg->uid(),
           login_rsp_msg.result());
}

void UserSessionProcessor::PingReqHandle(
    const std::shared_ptr<UserSession>& session,
    const std::shared_ptr<game::Ping>& ping) {
  LOG_TRACE("uid={}, session_id={}", session->get_user_id(),
            session->get_session_id());
}
