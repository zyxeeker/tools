/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2024-03-29 14:44:17
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2024-04-02 18:16:06
 * @Description: 
 */

#include "log.h"

#include <cstdarg>
#include <iostream>
#include <vector>

namespace tools {
namespace log {

/**
 * 全局输出最最低等级, 默认为 DEBUG
 */
level::LEVEL g_level_ = level::DEBUG;

const char* level::ToString(level::LEVEL level) {
  switch (level) {
#define XX(LVL) \
    case LEVEL::LVL: \
      return #LVL;

    XX(DEBUG)
    XX(INFO)
    XX(WARN)
    XX(ERROR)
    XX(FATAL)
#undef XX
    default:
      return "UNKNOWN";
  }
}

Logger::Logger(const char* file, const char* function, uint64_t line, level::LEVEL level)
    : file_(file),
      function_(function),
      line_(line),
      level_(level) {}

Logger::~Logger() {
  // 小于最低等级时忽略输出
  if (level_ < g_level_) {
    return;
  }
  std::cout << "(" << level::ToString(level_) << ")"
            << "[" << file_ << ":" << line_ << "(" << function_  << ")]"
            << oss_.str();
}

std::string Logger::CStringToStdString(const char* format, ...) {
  // 设置 format 默认为空, 若为空, 返回空字符串
  if (!format) {
    return "";
  }
  va_list args;
      va_start(args, format);
  // 通过 vsnprintf 计算出所需的字符串长度
  auto len = vsnprintf(nullptr, 0, format, args);
  // 使用 vector 创建 buffer, 并初始化为 0
  std::vector<char> buf(len + 1, 0);
  // 使用 vsnprintf 填入到目标 buffer 内
  vsnprintf(buf.data(), buf.size(), format, args);
      va_end(args);
  return buf.data();
}

void SetLevel(level::LEVEL level) {
  g_level_ = level >= level::NUM_LEVEL ? level::INFO : level;
}

level::LEVEL GetLevel() {
  return g_level_;
}

} // log
} // tools