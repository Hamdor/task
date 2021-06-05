
#include "taski/detail/xorwow_engine.hpp"

#include <random>
#include <chrono>
#include <iostream>

using namespace taski;

#define ITERATIONS 2000000000

int main() {
  detail::xorwow xor_engine;
  std::mt19937 mt_engine;
  std::random_device df_engine;
  // Measure time
  auto tp1 = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < ITERATIONS; ++i) {
    static_cast<void>(xor_engine());
  }
  auto tp2 = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < ITERATIONS; ++i) {
    static_cast<void>(mt_engine());
  }
  auto tp3 = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < ITERATIONS; ++i) {
    static_cast<void>(df_engine());
  }
  auto tp4 = std::chrono::high_resolution_clock::now();

  std::uniform_int_distribution<size_t> dist1(0, 100);
  std::uniform_int_distribution<size_t> dist2(0, 100);
  std::uniform_int_distribution<size_t> dist3(0, 100);
  // Check distribution, expect near 50
  double xor_mean = 0.0;
  for (int i = 0; i < ITERATIONS; ++i) {
    xor_mean += dist1(xor_engine);
  }
  xor_mean /= ITERATIONS;
  double mt_mean = 0.0;
  for (int i = 0; i < ITERATIONS; ++i) {
    mt_mean += dist2(mt_engine);
  }
  mt_mean /= ITERATIONS;
  double df_mean = 0.0;
  for (int i = 0; i < ITERATIONS; ++i) {
    df_mean += dist3(df_engine);
  }
  df_mean /= ITERATIONS;

  std::cout << "xorwow:   " << std::chrono::duration_cast<std::chrono::microseconds>(tp2 - tp1).count() << "µs\n";
  std::cout << "mt19937:  " << std::chrono::duration_cast<std::chrono::microseconds>(tp3 - tp2).count() << "µs\n";
  std::cout << "default:  " << std::chrono::duration_cast<std::chrono::microseconds>(tp4 - tp3).count() << "µs" << std::endl;

  std::cout << "xor_mean: " << xor_mean << "\n";
  std::cout << "mt_mean:  " << mt_mean << "\n";
  std::cout << "df_mean: " << df_mean << std::endl;

  std::cout << xor_engine() << std::endl;
}

