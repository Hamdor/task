 #ifndef ATOMIC_QUEUE_HPP
 #define ATOMIC_QUEUE_HPP

 #include <atomic>

 #define CACHE_LINE_SIZE 64

template<typename T>
class atomic_queue {
 public:
  class node {
   public:
    T* value;
    std::atomic<node*> next;
    node(T* val) : value(val), next(nullptr) {
      // nop
    }
   private:
    static constexpr size_t payload_size =
      sizeof(T*) + sizeof(std::atomic<node*>);
    static constexpr size_t cline_size = CACHE_LINE_SIZE;
    static constexpr size_t pad_size =
      (cline_size * ((payload_size / cline_size) + 1)) - payload_size;
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

  void append(T* value) {
    node* tmp = new node(value);
    spinlock guard(m_tail_lock);
    m_tail.load()->next = tmp;
    m_tail = tmp;
  }

  T* take_head() {
    unique_node_ptr first;
    T* result = nullptr;
    { // lifetime scope of spinlock
      spinlock guard(m_head_lock);
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

  bool empty() const {
    return m_head == m_tail;
  }

  std::atomic<node*> m_head;
  char m_pad1[CACHE_LINE_SIZE - sizeof(node*)];
  std::atomic<node*> m_tail;
  char m_pad2[CACHE_LINE_SIZE - sizeof(node*)];
  std::atomic_flag m_head_lock;
  std::atomic_flag m_tail_lock;

  struct spinlock {
     spinlock(std::atomic_flag& lock) : m_lock(lock) {
      while (lock.test_and_set(std::memory_order_acquire)) {
        std::this_thread::yield();
      }
    }
    ~spinlock() {
      m_lock.clear(std::memory_order_release);
    }
    spinlock(const spinlock&) = delete;
   private:
    std::atomic_flag& m_lock;
  };

};

 #endif // ATOMIC_QUEUE_HPP
