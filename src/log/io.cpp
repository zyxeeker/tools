/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2024-04-09 09:32:26
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2024-04-09 17:26:32
 * @Description: 
 */

#include "io.h"

#if _WIN32
#include <io.h>
#define F_EXISTED     0
#elif __unix__
#include <unistd.h>
#define F_EXISTED     F_OK
#endif

#include <iostream>
#include <fstream>
#include <utility>

namespace tools {
namespace log {
namespace output {

class Console : public Outputter::Item {
 public:
  ~Console() = default;
  void operator<<(const std::string &str) override {
    std::cout << str;
  }
};

class File : public Outputter::Item {
  /**
   * 用于在不同OS下的即时同步
   */
  class Helper : public std::filebuf {
   public:
    inline int _fsync() {
#if _WIN32
      return sync();
#elif __unix__
      return fsync(_M_file.fd());
#endif
    }
  };
 public:
  explicit File(std::string path)
      : path_(std::move(path)),
        ofs_(std::move(std::ofstream(path_))) {}
  ~File() = default;
  void operator<<(const std::string &str) override {
    if (!Access() && !ReopenFile()) {
      // TODO: Throw Exception
      return;
    }
    ofs_ << str;
    FSync();
  }
 private:
  /**
   * 重新打开文件
   * @return 是否成功
   */
  bool ReopenFile() {
    ofs_ = std::move(std::ofstream(path_, std::ios::binary | std::ios::app | std::ios::out));
    return ofs_.is_open();
  }
  /**
   * 测试文件路径是否存在
   * @return 是否存在
   */
  bool Access() {
#if _WIN32
    return _access_s(path_.c_str(), F_EXISTED) == 0;
#elif __unix__
    return access(path.c_str(), F_EXISTED) == 0;
#endif
  }
  /**
   * 用于即时同步到磁盘中, 无需等待OS决定
   * @return
   */
  int FSync() {
    return reinterpret_cast<Helper *>(ofs_.rdbuf())->_fsync();
  }
 private:
  std::string path_;
  std::ofstream ofs_;
};

} // output

Outputter::Outputter(const Config& cfg) {
  if (cfg.toConsole) {
    console_ = std::move(Item::Ptr(new output::Console()));
  }
  if (cfg.toFile && cfg.fileName.empty()) {
    // TODO: Throw Exception
  } else {
    file_ = std::move(Item::Ptr(new output::File(cfg.fileName)));
  }
}

Outputter::~Outputter() = default;

} // log
} // tools