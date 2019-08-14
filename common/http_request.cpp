/**
 * @file http_request.h
 * @brief curl 请求封装
 * @author caihanwu
 * @version
 * @date 2018-11-14
 */

#include "http_request.h"

namespace common {

static size_t WriteFunc(void* ptr, size_t size, size_t nmemb, void* data) {
  std::string* str = (std::string*)data;
  (*str).append((char*)ptr, size * nmemb);
  return size * nmemb;
};

bool PostJson(const std::string& url, const std::string& json_data,
              std::string* write_back_data) {
  if (curl_global_init(CURL_GLOBAL_ALL)) {
    return false;
  }

  CURL* curl = curl_easy_init();
  if (!curl) {
    curl_easy_cleanup(curl);
    return false;
  }

  struct curl_slist* headers = nullptr;
  headers = curl_slist_append(headers, "Content-Type: application/json");

  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_POST, 1);
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data.c_str());
  curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, json_data.length());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteFunc);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, write_back_data);
  // curl_easy_setopt(m_curl, CURLOPT_HEADER, 1);

  CURLcode res = curl_easy_perform(curl);
  curl_slist_free_all(headers);
  curl_easy_cleanup(curl);
  curl_global_cleanup();
  return res == CURLE_OK;
}

}  // namespace common
