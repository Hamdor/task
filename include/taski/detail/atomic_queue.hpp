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

#ifndef TASKI_DETAIL_ATOMIC_QUEUE_HPP
#define TASKI_DETAIL_ATOMIC_QUEUE_HPP

#include <atomic>

namespace taski {

namespace detail {

///
template <class T, size_t CacheLine=64>
class atomic_queue {
public:
  ///
  class node {
   public:
    T* value;
    std::atomic<node*> next;
    node(T* val) : value{val}, next{nullptr} {
      // nop
    }
   private:
    static constexpr size_t payload_size =
      sizeof(T*) + sizeof(std::atomic<node*>);
    static constexpr size_t pad_size =
      (CacheLine * ((payload_size / CacheLine) + 1)) - payload_size;
    char pad[pad_size];
  };

  using unique_node_ptr = std::unique_ptr<node>;

  atomic_queue() {
    m_head_lock.clear();
    m_tail_lock.clear();
    auto ptr = new node(nullptr);
    m_head = ptr;
    m_tail = ptr;
  }

  ~atomic_queue() {
    auto ptr = m_head.load();
    while (ptr) {
      unique_node_ptr tmp{ptr};
      ptr = tmp->next.load();
    }
  }

  ///
  void append(T* value) {
    node* tmp = new node(value);
    spinlock guard{m_tail_lock};
    m_tail.load()->next = tmp;
    m_tail = tmp;
  }

  ///
  T* take_head() {
    unique_node_ptr first;
    T* result = nullptr;
    { // lifetime scope of spinlock
      spinlock guard{m_head_lock};
      first.reset(m_head.load());
      node* next = first->next;
      if (next == nullptr) {
        first.release();
        return nullptr;
      }
      std::swap(result, next->value);
      m_head = next;
    }
    return result;
  }

  /// @returns `true` if queue is empty, `false` otherwise.
  inline bool empty() const { return m_head == m_tail; }

  std::atomic<node*> m_head;               /// Head of queue
  char m_pad1[CacheLine - sizeof(node*)];  /// Padding
  std::atomic<node*> m_tail;               /// Tail of queue
  char m_pad2[CacheLine - sizeof(node*)];  /// Padding
  std::atomic_flag m_head_lock;            /// CAS flag for head
  std::atomic_flag m_tail_lock;            /// CAS flag for tail

  /// Spinlock implementation.
  struct spinlock {
    spinlock(std::atomic_flag& lock) : m_lock{lock} {
      while (lock.test_and_set(std::memory_order_acquire))
        std::this_thread::yield();
    }
    ~spinlock() {
      m_lock.clear(std::memory_order_release);
    }
    spinlock(const spinlock&) = delete;
  private:
    std::atomic_flag& m_lock;   /// Reference to CAS flag to spin.
  };
};

} // namespace detail

} // namespace taski

#endif // TASKI_DETAIL_ATOMIC_QUEUE_HPP
