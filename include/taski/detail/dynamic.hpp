#pragma once

#include <thread>
#include <utility>
#include <algorithm>

namespace taski::detail {

static constexpr int dynamic = -1;

template <int Workers>
constexpr size_t number_of_threads() {
  return Workers == dynamic ? std::max(std::thread::hardware_concurrency(), 2u)
                            : static_cast<size_t>(Workers);
}

} // namespace taski::detail
