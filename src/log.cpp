/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2024-03-29 14:44:17
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2024-04-24 15:40:54
 * @Description: 
 */

#include "log.h"

#include <ctime>
#include <cstdarg>
#include <iostream>
#include <vector>

#include "log/logger.h"

namespace tools {
namespace log {

void SetLevel(level::LEVEL level) {
  Mgr::GetInstance().set_level(level >= level::NUM_LEVEL ? level::INFO : level);
}

level::LEVEL GetLevel() {
  return Mgr::GetInstance().level();
}

bool RegisterLogger(const std::string& name) {
  return Mgr::GetInstance().Register(name);
}

bool RegisterLogger(const Config &cfg) {
  return Mgr::GetInstance().Register(cfg);
}

void UnregisterLogger(const std::string &name) {
  return Mgr::GetInstance().Unregister(name);
}

std::vector<std::weak_ptr<Config> > GetAllLoggers() {
  return Mgr::GetInstance().GetAllLoggers();
}

class Log::Impl {
 public:
  inline Impl(time_t time,
       const char *file,
       const char *function,
       uint64_t line,
       level::LEVEL level,
       const char *name)
    : name_(name),
      msg_(new Msg{time, file, function, line, level, ""}) {}
  inline ~Impl() {
    Mgr::GetInstance().Output(name_, std::move(msg_));
  }
  inline void UpdateContent(const std::ostringstream& oss) {
    msg_->content = std::move(oss.str());
  }
 private:
  /**
   * 日志器名
   */
  const char* name_;
  /**
   * 消息体
   */
  std::shared_ptr<Msg> msg_;
};

Log::Log(const char* file,
         const char* function,
         uint64_t line,
         level::LEVEL level,
         const char* name)
    : impl_(new Impl(std::time(nullptr), file, function, line, level, name)) {}

Log::~Log() {
  impl_->UpdateContent(oss_);
}

std::string Log::CStringToStdString(const char *format, ...) {
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

} // log
} // tools