/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2024-04-03 14:18:39
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2024-04-07 17:24:22
 * @Description: 
 */

#ifndef TOOLS_LOG_FORMATTER_H_
#define TOOLS_LOG_FORMATTER_H_

#include <vector>
#include <memory>
#include <string>

#include "log.h"

namespace tools {
namespace log {

/**
 * 日志格式器, 用于解析和储存格式顺序
 */
class Formatter {
 public:
  /**
   * 子模块, 作为各个参数处理的基础接口
   */
  class Item {
   public:
    using Ptr = std::shared_ptr<Item>;
    Item() = default;
    virtual ~Item() = default;
    /**
     * 以()运算符作为基本接口, 便于调用
     * @param msg 数据
     * @return 字符串
     */
    virtual std::string operator()(const struct Msg& msg) = 0;
  };
 public:
  Formatter();
  ~Formatter();

  /**
   * 解析格式
   * @param pattern 带解析的格式字符串
   * @return 是否正确解析
   */
  bool Parse(const std::string& pattern);

  inline const std::vector<Item::Ptr>& pattern() const {
    return pattern_;
  }

 private:
  std::vector<Item::Ptr> pattern_;
};

} // log
} // tools


#endif //TOOLS_LOG_FORMATTER_H_
