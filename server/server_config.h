/*
 * file : server_config.h
 * author : caihanwu
 * date : 2018.11.21
 */

#ifndef GAMESERVER_SERVERCONFIG_H_
#define GAMESERVER_SERVERCONFIG_H_

#include <string>
#include <vector>

#include "pugixml.hpp"

struct Attr {
  int id;
  std::string ip;
  std::string port;
  int thread;
};

struct Log {
  std::string name;
  std::string path;
  std::string level;
  std::string flush;
  int thread;
};

struct Sdk {
  std::string url;
  std::string appid;
  uint32_t zone;
  unsigned int count;
};

class ServerConfig {
 public:
  const Log& get_log() const;
  const Attr& get_attr() const;
  const Sdk& get_sdk() const;
  bool LoadConfig(const std::string& path);
  static ServerConfig& Instance();

 private:
  ServerConfig(){};
  ServerConfig(const ServerConfig&) = delete;
  ServerConfig& operator=(const ServerConfig&) = delete;

  Attr attr_;
  Log log_;
  Sdk sdk_;
};

#endif  // GAMESERVER_SERVERCONFIG_H_
