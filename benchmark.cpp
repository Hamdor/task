#include <tuple>
#include <cstdlib>

#ifdef WORK_STEALING
#include "work_stealing.hpp"
#endif

#ifdef WORK_SHARING
#include "work_sharing.hpp"
#endif

int main() {
  #ifdef WORK_STEALING
    auto& scheduler = work_stealing::get_instance();
  #endif
  #ifdef WORK_SHARING
    auto& scheduler = work_sharing::get_instance();
  #endif

  size_t ImageHeight = 1080;
  size_t ImageWidth  = 1920;
  double MinRe = -2.0;
  double MaxRe = 1.0;
  double MinIm = -1.2;
  double MaxIm = MinIm + (MaxRe - MinRe) * ImageHeight / ImageWidth;
  double Re_factor = (MaxRe - MinRe) / (ImageWidth - 1);
  double Im_factor = (MaxIm - MinIm) / (ImageHeight - 1);
  size_t MaxIterations = 400;

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

  for(size_t y = 0; y < ImageHeight; ++y) {
    for(size_t x = 0; x < ImageWidth; ++x) {
#ifdef SINGLE_CORE
      fun(x, y);
#endif

#if defined(WORK_STEALING) || defined(WORK_SHARING)
      scheduler.run(fun, x, y);
#endif
    }
  }

#if defined(WORK_STEALING) || defined(WORK_SHARING)
  scheduler.shutdown();
#endif
}
