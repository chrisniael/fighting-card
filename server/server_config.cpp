/*
 * file : server_config.cpp
 * author : caihanwu
 * date : 2018.11.21
 *
 */

#include "server_config.h"

#include <iostream>

#include "pugixml.hpp"

const Log& ServerConfig::get_log() const { return this->log_; }

const Attr& ServerConfig::get_attr() const { return this->attr_; }

const Sdk& ServerConfig::get_sdk() const { return this->sdk_; }

bool ServerConfig::LoadConfig(const std::string& path) {
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(path.c_str());
  if (!result) {
    std::cerr << "load file " << path << " failed!" << std::endl;
    return false;
  }

  auto root = doc.child("gameserver");
  if (!root) {
    std::cerr << "root is nil -> gameserver" << std::endl;
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
  attr_.ip = root.attribute("ip").as_string();
  attr_.port = root.attribute("port").as_string();
  attr_.thread = root.attribute("thread").as_int();
  std::cout << "id:" << attr_.id << " ip:" << attr_.ip << " port:" << attr_.port
            << " thread:" << attr_.thread << std::endl;

  auto sdk = root.child("sdk");
  sdk_.url = sdk.child("url").text().as_string();
  sdk_.appid = sdk.child("appid").text().as_string();
  sdk_.zone = sdk.child("zone").text().as_uint();
  sdk_.count = sdk.child("count").text().as_uint();
  std::cout << "url:" << sdk_.url << " appid:" << sdk_.appid
            << " zone:" << sdk_.zone << " count:" << sdk_.count << std::endl;
  return true;
}

ServerConfig& ServerConfig::Instance() {
  static ServerConfig instance;
  return instance;
}
