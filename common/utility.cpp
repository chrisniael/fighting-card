/**
 * @file utility.cpp
 * @brief 工具函数
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-09-17
 */

#include "utility.h"

#include <cstdint>
#include <memory>
#include <string>

#include <google/protobuf/message.h>

namespace common {

std::shared_ptr<google::protobuf::Message> CreateMessage(
    const std::string& type_name) {
  const google::protobuf::Descriptor* descriptor =
      google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(
          type_name);
  if (descriptor) {
    const google::protobuf::Message* prototype =
        google::protobuf::MessageFactory::generated_factory()->GetPrototype(
            descriptor);
    if (prototype) {
      return std::shared_ptr<google::protobuf::Message>(prototype->New());
    }
  }
  return nullptr;
}

uint64_t MurmurHash64(const void* key, int len, unsigned int seed) {
  const uint64_t m = 0xc6a4a7935bd1e995;
  const int r = 47;

  uint64_t h = seed ^ (len * m);

  const uint64_t* data = (const uint64_t*)key;
  const uint64_t* end = data + (len / 8);

  while (data != end) {
    uint64_t k = *data++;

    k *= m;
    k ^= k >> r;
    k *= m;

    h ^= k;
    h *= m;
  }

  const unsigned char* data2 = (const unsigned char*)data;

  switch (len & 7) {
    case 7:
      h ^= uint64_t(data2[6]) << 48;
    case 6:
      h ^= uint64_t(data2[5]) << 40;
    case 5:
      h ^= uint64_t(data2[4]) << 32;
    case 4:
      h ^= uint64_t(data2[3]) << 24;
    case 3:
      h ^= uint64_t(data2[2]) << 16;
    case 2:
      h ^= uint64_t(data2[1]) << 8;
    case 1:
      h ^= uint64_t(data2[0]);
      h *= m;
  };

  h ^= h >> r;
  h *= m;
  h ^= h >> r;

  return h;
}

uint64_t Hash(const std::string& str) {
  uint64_t seed = 0xEE6B27EB;
  return MurmurHash64(str.data(), str.size(), seed);
}

uint64_t Hash(uint64_t n) {
  uint64_t seed = 0xEE6B27EB;
  return MurmurHash64(&n, sizeof(n), seed);
}

}  // namespace common
