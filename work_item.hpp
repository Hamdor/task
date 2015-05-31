
#ifndef WORK_ITEM_HPP
#define WORK_ITEM_HPP

#include <tuple>

struct storeable {
  virtual ~storeable() {
    // nop
  }
  virtual void exec() = 0;
};

template<typename T, typename... Xs>
struct work_item : public storeable {
  work_item(T&& t, Xs&&... xs)
    : m_fun(t)
    , m_args(std::make_tuple(std::forward<Xs>(xs)...)) {
    // nop
  }

  T m_fun;
  std::tuple<Xs...> m_args;

  template<int...>
  struct seq { };

  template<int N, int... S>
  struct gens : gens<N-1, N-1, S...> { };

  template<int... S>
  struct gens<0, S...> {
    typedef seq<S...> type;
  };

  virtual void exec() {
    call_unpack(typename gens<sizeof...(Xs)>::type());
  }

  template<int... S>
  void call_unpack(seq<S...>) {
    m_fun(std::get<S>(m_args)...);
  }
};

#endif // WORK_ITEM_HPP
