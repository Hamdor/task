# Taski
[![Reliability Rating](https://sonarcloud.io/api/project_badges/measure?project=Hamdor_task&metric=reliability_rating)](https://sonarcloud.io/dashboard?id=Hamdor_task)
[![Maintainability Rating](https://sonarcloud.io/api/project_badges/measure?project=Hamdor_task&metric=sqale_rating)](https://sonarcloud.io/dashboard?id=Hamdor_task)
[![Security Rating](https://sonarcloud.io/api/project_badges/measure?project=Hamdor_task&metric=security_rating)](https://sonarcloud.io/dashboard?id=Hamdor_task)
[![Coverage](https://sonarcloud.io/api/project_badges/measure?project=Hamdor_task&metric=coverage)](https://sonarcloud.io/dashboard?id=Hamdor_task)

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
#include "taski/all.hpp"

int main() {
  // number of workers -------------+
  //                                |
  // scheduling policy ------+      |
  //                         |      |
  taski::scheduler<taski::stealing, 4> scheduler;
  auto future = scheduler.enqueue([](auto i) {
    return i + 1;
  }, 42);
  std::cout << "future: " << future.get() << std::endl;
}
```
Compile with: `g++ -std=c++17 -Iinclude test.cpp`

Taski's `enqueue` function returns a `std::future` if the given function has a return type. Otherwise `run` returns a `std::future<void>`.
