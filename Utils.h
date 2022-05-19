#pragma once
#include <iostream>

namespace util {

#define inform(m) std::cout << m << std::endl

#define wait() system("pause")
#define wait_return() inform("Check log file"); wait(); return -1

}