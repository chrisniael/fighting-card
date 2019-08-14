/**
 * @file consistent_map.h
 * @brief 一致性 HashMap
 * @author shenyu, shenyu@shenyu.me
 * @version
 * @date 2018-09-20
 */

#ifndef COMMON_CONSISTENT_MAP_H_
#define COMMON_CONSISTENT_MAP_H_

#include <map>

namespace common {
template <typename K, typename V>
class ConsistentMap {
 public:
  ConsistentMap() = default;
  virtual ~ConsistentMap() = default;
  ConsistentMap(const ConsistentMap<K, V>&) = delete;
  ConsistentMap<K, V>& operator=(const ConsistentMap<K, V>&) = delete;

  inline bool insert(const K& hash, const V& node) {
    return data_.insert(std::make_pair(hash, node)).second;
  }

  inline void update(const K& hash, const V& node) { data_[hash] = node; }

  inline bool find(const K& hash, V* node) {
    if (data_.empty()) {
      return false;
    }

    auto it = data_.lower_bound(hash);

    if (it == data_.end()) {
      it = data_.begin();
    }

    *node = it->second;
    return true;
  }

  inline size_t erase(uint32_t hash) { return data_.erase(hash); }

  inline bool empty() { return data_.empty(); }

  inline size_t size() { return data_.size(); }

 private:
  std::map<K, V> data_;
};

}  // namespace common

#endif  // COMMON_CONSISTENT_MAP_H_
