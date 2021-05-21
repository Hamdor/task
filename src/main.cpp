/*******************************************************************************
 *                            _            _    _                              *
 *                           | |          | |  (_)                             *
 *                           | |_ __ _ ___| | ___                              *
 *                           | __/ _` / __| |/ / |                             *
 *                           | || (_| \__ \   <| |                             *
 *                            \__\__,_|___/_|\_\_|                             *
 *                                                                             *
 * Copyright (C) 2015 - 2021                                                   *
 * Marian Triebe                                                               *
 *                                                                             *
 * Distributed under the terms and conditions of the BSD 3-Clause License      *
 * See accompanying file LICENSE.                                              *
 *                                                                             *
 * If you did not receive a copy of the license file, see                      *
 * http://opensource.org/licenses/BSD-3-Clause                                 *
 *******************************************************************************/

#include "taski/all.hpp"

#include <iostream>

#include <chrono>

int main() {
  auto t1 = std::chrono::high_resolution_clock::now();
  {
    taski::scheduler<taski::stealing, CPUS> scheduler;
#ifdef HEAVY
    size_t ImageHeight = 1080;
    size_t ImageWidth  = 1920;
    double MinRe = -2.0;
    double MaxRe = 1.0;
    double MinIm = -1.2;
    double MaxIm = MinIm + (MaxRe - MinRe) * ImageHeight / ImageWidth;
    double Re_factor = (MaxRe - MinRe) / (ImageWidth - 1);
    double Im_factor = (MaxIm - MinIm) / (ImageHeight - 1);
    size_t MaxIterations = 600;
    auto fun = [&](size_t x, size_t y) {
      double c_im = MaxIm - y * Im_factor;
      double c_re = MinRe + x * Re_factor;
      double Z_re = c_re, Z_im = c_im;
      for(size_t n = 0; n < MaxIterations; ++n) {
        double Z_re2 = Z_re*Z_re, Z_im2 = Z_im*Z_im;
        Z_im = 2*Z_re*Z_im + c_im;
        Z_re = Z_re2 - Z_im2 + c_re;
      }
      // Return result, even if its not in the mandelbrot...
      return std::make_pair(Z_im, Z_re);
    };
    for(size_t y = 0; y < ImageHeight; ++y)
      for(size_t x = 0; x < ImageWidth; ++x)
        scheduler.enqueue(fun, x, y);
#else
    auto f = [](int i) {
      return i << 2;
    };
    for (int i = 0; i < 10000000; ++i)
      scheduler.enqueue(f, i);
#endif
  }
  auto t2 = std::chrono::high_resolution_clock::now();

  std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << "ms" << std::endl;
}
