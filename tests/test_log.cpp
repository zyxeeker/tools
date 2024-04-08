//
// Created by zyxeeker on 2024/3/29.
//

#include <iostream>
#include <log.h>

#define LOG(...)    T_LOG("", __VA_ARGS__)
#define LOG_D(...)  T_LOG_D("", __VA_ARGS__)
#define LOG_W(...)  T_LOG_W("", __VA_ARGS__)
#define LOG_E(...)  T_LOG_E("", __VA_ARGS__)
#define LOG_F(...)  T_LOG_F("", __VA_ARGS__)

int main() {
  LOG_D() << "HELLO TEST#" << 0 << std::endl;
  LOG("HELLO %s#%d\n", "TEST", 1);
  tools::log::SetLevel(tools::log::level::INFO);
  LOG_D() << "HELLO TEST#" << 2 << std::endl;
  LOG_W() << "HELLO TEST#" << 3 << std::endl;
  LOG_E() << "HELLO TEST#" << 4 << std::endl;
  LOG_F() << "HELLO TEST#" << 5 << std::endl;
}