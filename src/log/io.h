/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2024-04-09 09:32:26
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2024-04-09 17:26:24
 * @Description: 
 */

#ifndef TOOLS_LOG_IO_H_
#define TOOLS_LOG_IO_H_

#include <string>
#include <memory>

#include <log.h>

namespace tools {
namespace log {

class Outputter {
 public:
  class Item {
   public:
    using Ptr = std::shared_ptr<Item>;
    virtual ~Item() = default;
    virtual void operator<<(const std::string& str) = 0;
  };
 public:
  explicit Outputter(const Config& cfg);
  ~Outputter();
  inline Item::Ptr file() const {
    return file_;
  }
  inline Item::Ptr console() const {
    return console_;
  }
 private:
  Item::Ptr file_;
  Item::Ptr console_;
};

} // log
} // tools

#endif //TOOLS_LOG_IO_H_
