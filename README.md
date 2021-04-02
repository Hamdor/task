# Taski
Simple task scheduler, taski has support for work-stealing and work-sharing.
Taski requires C++17, for an older version which only requires C++14, please use ff7c808.

Supported Compilers
-------------------
* GCC >= 7.2.0
* Clang with support for C++17

Example
-------------
```c++
// test.cpp
#include <iostream>
#include "taski.hpp"

int main() {
  // number of workers -------------+
  //                                |
  // scheduling policy ------+      |
  //                         |      |
  taski::scheduler<taski::stealing, 4> scheduler;
  auto future = scheduler.enqueue([](int i) -> int {
    return i + 1;
  }, int{42});
  future.wait();
  std::cout << "future: " << future.get() << std::endl;
}
```
Compile with: `g++ -std=c++17 test.cpp -Iinclude`

Taski's `enqueue` function returns a `std::future` if the given function has a return type. Otherwise `run` returns a `std::future<void>`.
