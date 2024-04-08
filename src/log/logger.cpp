/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2024-04-07 18:19:54
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2024-04-08 17:19:15
 * @Description: 
 */

#include "logger.h"

#include <sstream>
#include <iostream>

namespace tools {
namespace log {

Logger::Logger(const Config& cfg)
    : cfg_(new Config(cfg)),
      formatter_(new Formatter()) {
  formatter_->Parse(cfg_->pattern);
}

Logger::~Logger() = default;

void Logger::operator()(const Msg::Ptr& msg) {
  // 格式器不存在时 OR 小于最低等级时忽略输出
  if (!formatter_ || msg->level < LoggerMgr::GetInstance().level()) return;
  std::ostringstream oss;
  for (auto& i : formatter_->pattern()) {
    oss << (*i)(*msg);
  }
  std::cout << oss.str();
}

LoggerMgr LoggerMgr::inst_;

LoggerMgr::LoggerMgr()
  : def_logger_(new Logger({"root", "%d [%p](%f:%l@%c) %m"})) {}

bool LoggerMgr::Register(const Config& cfg) {
  std::lock_guard<std::mutex> lk(mutex_);
  if (loggers_.count(cfg.name)) {
    return false;
  }
  auto logger = std::make_shared<Logger>(cfg);
  loggers_.insert({cfg.name, logger});
  return true;
}

void LoggerMgr::Unregister(const std::string &key) {
  std::lock_guard<std::mutex> lk(mutex_);
  loggers_.erase(key);
}

void LoggerMgr::Output(const std::string& key, Msg::Ptr msg) {
  auto res = loggers_.find(key);
  // 未找到交予默认日志器输出
  if (res == loggers_.end()) {
    (*def_logger_)(msg);
  } else {
    (*(res->second))(msg);
  }
}

std::vector<std::weak_ptr<Config> > LoggerMgr::GetAllLoggers() {
  std::lock_guard<std::mutex> lk(mutex_);
  std::vector<std::weak_ptr<Config> > cfgs;
  cfgs.reserve(loggers_.size());
  for (auto& i : loggers_) {
    cfgs.push_back(i.second->cfg());
  }
  return cfgs;
}

} // log
} // tools
