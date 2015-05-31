#ifndef TUPLE_UNPACKING_HPP
#define TUPLE_UNPACKING_HPP

template<int...>
struct unpacked { };

template<int N, int... S>
struct unpack_tuple : unpack_tuple<N-1, N-1, S...> { };

template<int... S>
struct unpack_tuple<0, S...> {
  typedef unpacked<S...> type;
};

#endif // TUPLE_UNPACKING_HPP
