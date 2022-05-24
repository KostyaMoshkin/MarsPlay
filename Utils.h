#pragma once
#include <iostream>

namespace util {

#define to_cout(m) std::cout << m << std::endl

#define wait() system("pause")
#define wait_return() to_cout("Check log file"); wait(); return -1

}