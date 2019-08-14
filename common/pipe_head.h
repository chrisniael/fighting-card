/**
 * @file pipe_head.h
 * @brief pipe head
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-10-31
 */

#ifndef COMMON_PIPE_HEAD_H_
#define COMMON_PIPE_HEAD_H_

#include <string>

namespace common {
struct PipeHead {
  uint32_t session_id{0};
  std::string appid;
  std::string user_id;
};
}  // namespace common

#endif  // COMMON_PIPE_HEAD_H_
