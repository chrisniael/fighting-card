/**
 * @file http_request.h
 * @brief curl 请求封装
 * @author caihanwu
 * @version
 * @date 2018-11-14
 */

#ifndef COMMON_HTTP_REQUEST_H_
#define COMMON_HTTP_REQUEST_H_

#include <cstring>
#include <string>

#include <curl/curl.h>
#include <curl/easy.h>

namespace common {

bool PostJson(const std::string& url, const std::string& json_data,
              std::string* write_back_data);

}  // namespace common

#endif  // COMMON_HTTP_REQUEST_H_
