/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2024-03-29 14:36:59
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2024-04-02 18:13:31
 * @Description: 
 */

#include <sstream>

#ifndef LOG_H
#define LOG_H

#define LOG_IMPL(LVL, ...) \
  tools::log::Logger(__FILE__, __FUNCTION__, __LINE__, \
    tools::log::level::LVL) << tools::log::Logger::CStringToStdString(__VA_ARGS__)
#define LOG(...)    LOG_IMPL(INFO, __VA_ARGS__)
#define LOG_D(...)  LOG_IMPL(DEBUG, __VA_ARGS__)
#define LOG_W(...)  LOG_IMPL(WARN, __VA_ARGS__)
#define LOG_E(...)  LOG_IMPL(ERROR, __VA_ARGS__)
#define LOG_F(...)  LOG_IMPL(FATAL, __VA_ARGS__)

namespace tools {
namespace log {

namespace level {
/**
 * 等级枚举
 */
enum LEVEL : uint8_t {
  DEBUG = 0,
  INFO,
  WARN,
  ERROR,
  FATAL,
  NUM_LEVEL
};

/**
 * 用于字符转换
 * @param level 等级枚举
 * @return 等级字符串, 若未处于枚举范围则返回 UNKNOWN
 */
const char* ToString(LEVEL level);

} // level

class Logger {
 public:
  /**
   * 构建流式输出
   * @param file 文件名
   * @param function 函数名
   * @param line 行号
   * @param level 等级
   */
  Logger(const char* file,
         const char* function,
         uint64_t line,
         level::LEVEL level);
  /**
   * 在对象析构时处理缓存的字符串
   */
  ~Logger();
  /**
   * 用于转换C风格输入为C++标准字符串
   * @param format 输出格式
   * @param ... 输出的参数
   * @return 标准字符串
   */
  static std::string CStringToStdString(const char* format = nullptr, ...);
  /**
   * 流输出方式 重写输入运算符
   * @tparam T 模板参数来适应不同类型的参数传递
   * @param value 需要打印的参数
   * @return 返回自身引用用于多次 << 调用, 比如 << ... << ...
   */
  template <typename T>
  Logger& operator<<(const T& value) {
    oss_ << value;
    return *this;
  }
  /**
   * 用于适配 std::endl, std::ends, std::flush
   * @param func 函数指针
   * @return 返回自身引用
   */
  Logger& operator<<(std::ostream& (*func)(std::ostream&)) {
    oss_ << func;
    return *this;
  }

 private:
  /**
   * 用于存储缓冲字符串
   */
  std::ostringstream oss_;
  /**
   * 文件名
   */
  const char* file_;
  /**
   * 函数名
   */
  const char* function_;
  /**
   * 行号
   */
  uint64_t line_;
  /**
   * 等级
   */
  level::LEVEL level_;
};

/**
 * 设置最低输出等级
 * @param level 等级枚举
 */
void SetLevel(level::LEVEL level);

/**
 * 获取最低输出等级
 * @return 等级枚举
 */
level::LEVEL GetLevel();

} // log
} // tools

#endif //LOG_H
