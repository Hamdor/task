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

#pragma once

#include "taski/detail/spinlock.hpp"

#include <atomic>

namespace taski::detail {

/// Implementation of a cache line aware queue.
template <class T, size_t CacheLine=64>
class atomic_queue {
public:
  using value_ptr = std::unique_ptr<T>;

  /// Internal node of the queue.
  class node {
   public:
    value_ptr value;
    std::atomic<node*> next;
    explicit node(value_ptr val) : value{std::move(val)}, next{nullptr} {
      // nop
    }
   private:
    static constexpr size_t payload_size =
      sizeof(value_ptr) + sizeof(std::atomic<node*>);
    static constexpr size_t pad_size =
      (CacheLine * ((payload_size / CacheLine) + 1)) - payload_size;
    char pad[pad_size];
  };

  using unique_node_ptr = std::unique_ptr<node>;

  atomic_queue() {
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

  atomic_queue(const atomic_queue&) = delete;
  atomic_queue(atomic_queue&&) = delete;

  atomic_queue& operator=(const atomic_queue&) = delete;
  atomic_queue& operator=(atomic_queue&&) = delete;

  /// Appends the given 'value' to the queue.
  void append(value_ptr value) {
    auto tmp = new node(std::move(value));
    spinlock guard{m_tail_lock};
    m_tail.load()->next = tmp;
    m_tail = tmp;
  }

  /// @returns The first element of the queue, 'nullptr' if queue is empty.
  value_ptr take_head() {
    unique_node_ptr first;
    value_ptr result;
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
  std::atomic_bool m_head_lock = false;    /// CAS flag for head
  char m_pad3[CacheLine - sizeof(std::atomic_bool)];  /// Padding
  std::atomic_bool m_tail_lock = false;    /// CAS flag for tail
};

} // namespace taski::detail
