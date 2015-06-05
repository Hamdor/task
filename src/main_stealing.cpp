#include <cmath>
#include <mutex>
#include <vector>
#include <iostream>
#include <algorithm>

#include "work_stealing.hpp"

int main() {
  std::mutex mtx_cout;
  auto& wsharing = work_stealing::get_instance();
  wsharing.run([&]() {
    std::unique_lock<std::mutex> lock(mtx_cout);
    std::cout << "bla" << std::endl;
  });
  wsharing.run([&](int i) {
    std::unique_lock<std::mutex> lock(mtx_cout);
    std::cout << i << std::endl;
  }, int{6});

  auto fun = [](uint64_t i) {
    auto res = i;
    for (size_t j = 0; j < i * 2; ++j) {
      res += std::pow(i, 2) + std::sqrt(i);
    }
  };

  std::vector<long> values(200);
  long i = 0;
  std::generate(values.begin(), values.end(), [&] { return i++; });

  for (long val : values) {
    wsharing.run(fun, val);
  }

  for (long val : values) {
    wsharing.run(std::move(fun), std::move(val));
  }

  auto future = wsharing.run([](int i) -> int {
    std::cout << i << std::endl;
    return i + 1;
  }, int{42});

  future.wait();
  std::cout << "future: " << future.get() << std::endl;

  wsharing.shutdown();
}
