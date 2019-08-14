/*
 * file : client_config.h
 * author : caihanwu
 * date : 2018.11.21
 */

#ifndef GAMECLIENT_CLIENTCONFIG_H_
#define GAMECLIENT_CLIENTCONFIG_H_

#include <string>
#include <vector>

#include "pugixml.hpp"

struct Attr {
  int id;
  int thread;
};

struct Log {
  std::string name;
  std::string path;
  std::string level;
  std::string flush;
  int thread;
};

struct ServerNode {
  std::string ip;
  std::string port;
};

struct UsersInfo {
  std::string appid;
  uint32_t zone;
  int uid_from;
  int uid_to;
};

class ClientConfig {
 public:
  const Log& get_log() const;
  const Attr& get_attr() const;
  const UsersInfo& get_usersinfo() const;
  const ServerNode& get_queueserver() const;
  const ServerNode& get_gameserver() const;
  bool LoadConfig(const std::string& path);
  static ClientConfig& Instance();

 private:
  ClientConfig(){};
  ClientConfig(const ClientConfig&) = delete;
  ClientConfig& operator=(const ClientConfig&) = delete;

  Attr attr_;
  Log log_;
  UsersInfo usersinfo_;
  ServerNode queueserver_;
  ServerNode gameserver_;
};

#endif  // GAMECLIENT_CLIENTCONFIG_H_
