
#ifndef WORK_ITEM_HPP
#define WORK_ITEM_HPP

#include <tuple>
#include <future>
#include <type_traits>

#include "storeable.hpp"
#include "tuple_unpacking.hpp"

template<typename T, typename... Xs>
struct work_item : public storeable {
  work_item(T&& t, Xs&&... xs)
    : m_fun(t)
    , m_args(std::make_tuple(std::forward<Xs>(xs)...)) {
    // nop
  }

  //using ret_type = typename std::result_of<T(Xs...)>::type;
  //ret_type m_ret;
  //std::future<ret_type> m_future;

  T m_fun;
  std::tuple<Xs...> m_args;

  virtual void exec() {
    call_unpack(typename unpack_tuple<sizeof...(Xs)>::type());
  }

  /*template<int... S>
  typename std::enable_if<!std::is_void<ret_type>::value>::type
  call_unpack(unpacked<S...>) {
    m_ret = m_fun(std::get<S>(m_args)...);
  }*/

  template<int... S>
  void call_unpack(unpacked<S...>) {
    m_fun(std::get<S>(m_args)...);
  }
};

#endif // WORK_ITEM_HPP
