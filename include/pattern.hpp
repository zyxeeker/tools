/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2024-04-24 15:00:06
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2024-04-24 15:41:10
 * @Description: 
 */

#ifndef PATTERN_HPP_
#define PATTERN_HPP_

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

} // pattern
} // tools

#endif //PATTERN_HPP_
