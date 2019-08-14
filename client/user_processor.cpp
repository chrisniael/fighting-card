/**
 * @file user_processor.cpp
 * @brief 用户消息处理
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-11-20
 */

#include "user_processor.h"

#include "client_config.h"
#include "user.h"

UserProcessor::UserProcessor(asio::io_service& io_service) {
  this->set_io_service(io_service);
}

void UserProcessor::Init() {
  // Game
  this->AddHandler(this, &UserProcessor::GameLoginRspHandle);
}

void UserProcessor::GameLoginRspHandle(
    const std::shared_ptr<User>& user,
    const std::shared_ptr<game::LoginRsp>& queue_rsp_msg) {
  LOG_INFO("LoginRsp, appid={}, zone={}, uid={}, result={}, token={}",
           user->get_appid(), user->get_zone(), user->get_uid(),
           queue_rsp_msg->result(), queue_rsp_msg->token());
  if (queue_rsp_msg->result() == game::kLoginResSuccess) {
    LOG_INFO("LoginRsp, user login success, appid={}, zone={}, uid={}",
             user->get_appid(), user->get_zone(), user->get_uid());
  } else if (queue_rsp_msg->result() == game::kLoginResError) {
    LOG_ERROR("LoginRsp, user login error, appid={}, zone={}, uid={}",
              user->get_appid(), user->get_zone(), user->get_uid());
    user->CloseGameClient();
  } else if (queue_rsp_msg->result() == game::kLoginResQueue) {
    LOG_WARN("LoginRsp, user need to queue, appid={}, zone={}, uid={}",
             user->get_appid(), user->get_zone(), user->get_uid());

    user->CloseGameClient();
  } else if (queue_rsp_msg->result() == game::kLoginResLogined) {
    LOG_ERROR("LoginRsp, user has logined, appid={}, zone={}, uid={}",
              user->get_appid(), user->get_zone(), user->get_uid());
    user->CloseGameClient();
  }
}
