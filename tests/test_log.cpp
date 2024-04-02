//
// Created by zyxeeker on 2024/3/29.
//

#include <iostream>
#include <log.h>

int main() {
  LOG_D() << "HELLO " << "TEST#" << 0 << std::endl;
  LOG("HELLO %s#%d\n", "TEST", 1);
  tools::log::SetLevel(tools::log::level::INFO);
  LOG_D() << "HELLO " << "TEST#" << 2 << std::endl;
  LOG_W() << "HELLO " << "TEST#" << 3 << std::endl;
  LOG_E() << "HELLO " << "TEST#" << 4 << std::endl;
  LOG_F() << "HELLO " << "TEST#" << 5 << std::endl;
}