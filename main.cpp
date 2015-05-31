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
    std::cout << std::pow(i, 2) << std::endl;
  };

  std::vector<long> values(600);
  long i = 0;
  std::generate(values.begin(), values.end(), [&] { return i++; });

  for (long val : values) {
    wsharing.run(std::move(fun), std::move(val));
  }

  wsharing.shutdown();
}
