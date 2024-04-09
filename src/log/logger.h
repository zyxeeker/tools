/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2024-04-07 18:19:54
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2024-04-09 17:42:20
 * @Description: 
 */

#ifndef TOOLS_LOG_LOGGER_H_
#define TOOLS_LOG_LOGGER_H_

#include <mutex>
#include <memory>
#include <unordered_map>

#include "log.h"
#include "formatter.h"
#include "io.h"

namespace tools {
namespace log {

/**
 * 日志消息体, 用于组合传递参数
 */
struct Msg {
  using Ptr = std::shared_ptr<Msg>;
  time_t time;          // 时间戳
  const char* file;     // 文件名
  const char* func;     // 函数名
  uint64_t line;        // 行号
  level::LEVEL level;   // 等级枚举
  std::string content;  // 内容
};

class Logger {
 public:
  using Ptr = std::shared_ptr<Logger>;
 public:
  explicit Logger(const Config& cfg);
  ~Logger();
  void operator()(const Msg::Ptr& msg);
  inline std::shared_ptr<Config> cfg() const {
    return cfg_;
  }
 private:
  std::shared_ptr<Config> cfg_;
  std::unique_ptr<Formatter> formatter_;
  std::unique_ptr<Outputter> outputter_;
};

/**
 * 日志管理器
 */
class LoggerMgr {
 public:
  static LoggerMgr& GetInstance() {
    return inst_;
  }
  /**
   * 注册日志器
   * @param cfg 日志器配置
   * @return 是否成功
   */
  bool Register(const Config& cfg);
  /**
   * 默认参数注册日志器
   * @param name 名称/Key(Unique)
   * @return 是否成功
   */
  bool Register(const std::string& name);
  /**
   * 反注册日志器
   * @param key 日志器key
   */
  void Unregister(const std::string& key);
  /**
   * 寻找指定日志器并调用输出
   * @param key 日志器名称
   * @param msg 日志消息
   */
  void Output(const std::string& key, Msg::Ptr msg);
  /**
   * 获取所有已注册日志器配置
   * @return 日志器配置数组
   */
  std::vector<std::weak_ptr<Config> > GetAllLoggers();
  /**
   * 设置全局输出最低等级
   * @param level 等级枚举
   */
  inline void set_level(level::LEVEL level) {
    level_ = level;
  }
  /**
   * 获取全局输出最低等级
   * @return 等级枚举
   */
  auto level() const {
    return level_;
  }

 private:
  LoggerMgr();
  LoggerMgr(const LoggerMgr&);
  LoggerMgr& operator=(const LoggerMgr&);

 private:
  static LoggerMgr inst_;
  std::mutex mutex_;
  /**
   * 全局输出最最低等级, 默认为 DEBUG
   */
  level::LEVEL level_ = level::DEBUG;
  /**
   * 日志器集合
   */
  std::unordered_map<std::string, Logger::Ptr> loggers_;
  /**
   * 默认日志器
   */
  Logger::Ptr def_logger_;
};

} // log
} // tools

#endif //TOOLS_LOG_LOGGER_H_
