/*
 * file : client_config.cpp
 * author : caihanwu
 * date : 2018.11.21
 *
 */

#include "client_config.h"

#include <iostream>

const Log& ClientConfig::get_log() const { return this->log_; }

const Attr& ClientConfig::get_attr() const { return this->attr_; }

const UsersInfo& ClientConfig::get_usersinfo() const {
  return this->usersinfo_;
}

const ServerNode& ClientConfig::get_queueserver() const {
  return this->queueserver_;
}

const ServerNode& ClientConfig::get_gameserver() const {
  return this->gameserver_;
}

bool ClientConfig::LoadConfig(const std::string& path) {
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(path.c_str());
  if (!result) {
    std::cerr << "load file " << path << " failed!" << std::endl;
    return false;
  }

  auto root = doc.child("gameclient");
  if (!root) {
    std::cerr << "root is nil -> gameclient" << std::endl;
    return false;
  }

  auto log = root.child("log");
  log_.name = log.child("name").text().as_string();
  log_.path = log.child("path").text().as_string();
  log_.level = log.child("level").text().as_string();
  log_.flush = log.child("flush").text().as_string();
  log_.thread = log.child("thread").text().as_int();
  std::cout << "name:" << log_.name << " path:" << log_.path
            << " level:" << log_.level << " flush:" << log_.flush << std::endl;

  attr_.id = root.attribute("id").as_int();
  attr_.thread = root.attribute("thread").as_int();
  std::cout << "id:" << attr_.id << " ip:"
            << " thread:" << attr_.thread << std::endl;

  auto users_info = root.child("users_info");
  usersinfo_.appid = users_info.child("appid").text().as_string();
  usersinfo_.zone = users_info.child("zone").text().as_int();
  usersinfo_.uid_from = users_info.child("uid_from").text().as_int();
  usersinfo_.uid_to = users_info.child("uid_to").text().as_int();
  std::cout << "appid:" << usersinfo_.appid << " zone:" << usersinfo_.zone
            << " uid_from:" << usersinfo_.uid_from
            << " uid_to:" << usersinfo_.uid_to << std::endl;

  auto gameserver_node = root.child("gameserver");
  this->gameserver_.ip = gameserver_node.attribute("ip").as_string();
  this->gameserver_.port = gameserver_node.attribute("port").as_string();
  std::cout << "game ip:" << this->gameserver_.ip
            << " port:" << this->gameserver_.port << std::endl;

  auto queueserver_node = root.child("queueserver");
  this->queueserver_.ip = queueserver_node.attribute("ip").as_string();
  this->queueserver_.port = queueserver_node.attribute("port").as_string();
  std::cout << "queue ip:" << this->queueserver_.ip
            << " port:" << this->queueserver_.port << std::endl;

  return true;
}

ClientConfig& ClientConfig::Instance() {
  static ClientConfig instance;
  return instance;
}
