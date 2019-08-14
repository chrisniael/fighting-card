/**
 * @file utility.h
 * @brief 工具函数
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-09-17
 */

#ifndef COMMON_UTILITY_H_
#define COMMON_UTILITY_H_

#include <cstdint>
#include <memory>
#include <string>

#include <google/protobuf/message.h>

namespace common {

std::shared_ptr<google::protobuf::Message> CreateMessage(
    const std::string& type_name);

template <class Message>
std::shared_ptr<Message> ProtoBufReflection(const std::string& type_name,
                                            const void* data, int size) {
  std::shared_ptr<google::protobuf::Message> proto_msg =
      CreateMessage(type_name);
  if (proto_msg) {
    proto_msg->ParseFromArray(data, size);
    return std::dynamic_pointer_cast<Message>(proto_msg);
  }
  return nullptr;
}

template <class Message>
bool MsgNameEqual(const std::string& msg_name) {
  return strcmp(msg_name.c_str(), Message::descriptor()->full_name().c_str()) ==
         0;
}

// return a hash code for the input, as Java hashCode()
// use horner's rule : s[0]*31^(n-1) + s[1]*31^(n-2) + ... + s[n-1]
inline uint32_t HashCode(const std::string& input, uint32_t mod) {
  uint32_t result = 0;
  uint32_t radis = 31;
  for (auto i = 0; i < input.length(); i++) {
    result = (radis * result + input[i]) % mod;
  }
  return result;
}

// MurmurHash2, by Austin Appleby
// Note - This code makes a few assumptions about how your machine behaves -
// And it has a few limitations -
// 1. It will not work incrementally.
// 2. It will not produce the same results on little-endian and big-endian
//    machines.

uint64_t MurmurHash64(const void* key, int len, unsigned int seed);

uint64_t Hash(const std::string& str);

uint64_t Hash(uint64_t n);

}  // namespace common

#endif  // COMMON_UTILITY_H_
