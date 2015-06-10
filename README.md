# Taski
Simple task scheduler, taski has support for work-stealing and work-sharing.

Supported Compilers
-------------------

* GCC >= 4.9.2
* Clang >= 3.5.2

(Only tested with these two compilers)

Example
-------------
```c++
// test.cpp
#include <iostream>
#include "work_stealing.hpp"

auto main() -> int {
  auto& wstealing = work_stealing::get_instance();
  auto future = wstealing.run([](int i) -> int {
    return i + 1;
  }, int{42});
  future.wait();
  std::cout << "future: " << future.get() << std::endl;
  wstealing.shutdown();
}
```
Compile with: `g++ -std=c++14 -pthread src/work_stealing.cpp test.cpp -I include`

Taski's `run` function returns a `std::future` if the given function has a return type. Otherwise `run` returns a `std::future<void>`.
Be careful with `std::future<void>`, `get()` will throw a `std::future_error` (broken promise exception).

