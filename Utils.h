#pragma once
#include <iostream>

namespace util {

#define message(m) std::cout << m << std::endl

#define wait() system("pause")
#define wait_return() system("pause"); return -1

}