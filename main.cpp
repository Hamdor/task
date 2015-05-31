#include <iostream>
#include <mutex>
#include <vector>
#include <algorithm>
#include <cmath>
#include "work_sharing.hpp"

int main() {
  std::mutex mtx_cout;
  auto& wsharing = work_sharing::get_instance();
  wsharing.run([&]() {
    std::unique_lock<std::mutex> lock(mtx_cout);
    std::cout << "bla" << std::endl;
  });
  wsharing.run([&](int i) {
    std::unique_lock<std::mutex> lock(mtx_cout);
    std::cout << i << std::endl;
  }, int{6});

  auto fun = [](uint64_t i) {
    auto res = std::pow(i, 2);
  };

  std::vector<long> values(6000);
  long i = 0;
  std::generate(values.begin(), values.end(), [&] { return i++; });

  for (long val : values) {
    wsharing.run(std::move(fun), std::move(val));
  }

  wsharing.run([](int i) -> int {
    std::cout << i << std::endl;
    return i;
  }, int{6});

  wsharing.shutdown();
}
