/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2024-04-03 12:03:35
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2024-04-07 17:24:29
 * @Description: 
 */

#ifndef TOOLS_LOG_H_
#define TOOLS_LOG_H_

#include <memory>
#include <vector>

#include <log.h>

namespace tools {
namespace log {

/**
 * 日志消息体, 用于组合传递参数
 */
struct Msg {
  time_t time;          // 时间戳
  const char* file;     // 文件名
  const char* func;     // 函数名
  uint64_t line;        // 行号
  level::LEVEL level;   // 等级枚举
  std::string content;  // 内容
};

} // log
} // tools

#endif //TOOLS_LOG_H_
