/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2024-04-24 15:00:06
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2024-04-24 17:29:47
 * @Description: 
 */

#ifndef PATTERN_HPP_
#define PATTERN_HPP_

#include <map>
#include <memory>

namespace tools {
namespace pattern {

#define DISALLOW_COPY_AND_ASSIGN(CLASS_NAME)  \
  CLASS_NAME(const CLASS_NAME&);              \
  CLASS_NAME& operator=(const CLASS_NAME&);

template <class T>
class HungrySingleton {
  DISALLOW_COPY_AND_ASSIGN(HungrySingleton)
 public:
  static T& GetInstance() {
    return *inst_;
  }
 private:
  static std::unique_ptr<T> inst_;
};

template <class T>
std::unique_ptr<T> HungrySingleton<T>::inst_ = std::make_unique<T>();

template <class T>
class LazySingleton {
  DISALLOW_COPY_AND_ASSIGN(LazySingleton)
 public:
  static T& GetInstance() {
    static std::unique_ptr<T> inst = std::make_unique<T>();
    return *inst;
  }
};

template <typename T>
class Observer {
 public:
  class Listener {
   public:
    using Id = size_t;
    using WPtr = std::weak_ptr<Listener>;
    Listener() = default;
    virtual ~Listener() = default;
    virtual void Handle(const T& val) = 0;
  };
 public:
  Observer() = default;
  virtual ~Observer() = default;
  inline typename Listener::Id RegisterListener(typename Listener::WPtr listener) {
    static typename Listener::Id id = 0;
    listeners_.insert({id++, std::move(listener)});
    return id;
  }
  inline void UnregisterListener(typename Listener::Id id) {
    listeners_.erase(id);
  }
  void DispatchData(const T& val) {
    for (auto& i : listeners_) {
      if (i.second.expired()) continue;
      i.second.lock()->Handle(val);
    }
  }
 private:
  std::map<typename Listener::Id, typename Listener::WPtr> listeners_;
};

} // pattern
} // tools

#endif //PATTERN_HPP_
