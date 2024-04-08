//
// Created by zyxeeker on 2024/3/29.
//

#include <iostream>
#include <log.h>

int main() {
  LOG_D() << "HELLO TEST#" << 0 << std::endl;
  LOG("HELLO %s#%d\n", "TEST", 1);
  tools::log::SetLevel(tools::log::level::INFO);
  LOG_D() << "HELLO TEST#" << 2 << std::endl;
  LOG_W() << "HELLO TEST#" << 3 << std::endl;
  LOG_E() << "HELLO TEST#" << 4 << std::endl;
  LOG_F() << "HELLO TEST#" << 5 << std::endl;

  tools::log::RegisterLogger({"test#1", "[%p](%f:%l@%c) %m"});
  tools::log::RegisterLogger({"test#2", "%d [%p](%f:%l) %m"});

  CLOG("test#1") << "test#1 TEST#" << 1 << std::endl;
  CLOG_D("test#1") << "test#1 TEST#" << 2 << std::endl;
  CLOG_W("test#1") << "test#1 TEST#" << 3 << std::endl;
  CLOG_E("test#1") << "test#1 TEST#" << 4 << std::endl;
  CLOG_F("test#1") << "test#1 TEST#" << 5 << std::endl;

  CLOG("test#2") << "test#2 TEST#" << 1 << std::endl;
  CLOG_D("test#2") << "test#2 TEST#" << 2 << std::endl;
  CLOG_W("test#2") << "test#2 TEST#" << 3 << std::endl;
  CLOG_E("test#2") << "test#2 TEST#" << 4 << std::endl;
  CLOG_F("test#2") << "test#2 TEST#" << 5 << std::endl;

}