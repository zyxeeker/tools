/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2024-04-16 17:25:20
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2024-04-18 18:45:50
 * @Description: 
 */

#ifndef MEM_HPP_
#define MEM_HPP_

#include <atomic>
#include <array>

namespace tools {
namespace mem {

template <class T, size_t Size>
class RingBuffer {
  struct Item {
    T elem;
    struct Item* next;
  };
 public:
  explicit RingBuffer() {
    begin_ = nodes_.data();
    end_ = nodes_.data() + (capacity_ - 1);
    end_->next = begin_;
    for (auto i = 0; i < nodes_.size(); i++) {
      if (i + 1 == nodes_.size()) break;
      (nodes_.data() + i)->next = nodes_.data() + i + 1;
    }
    head_ = begin_;
    tail_ = begin_;
  }
  ~RingBuffer() = default;
  bool Put(const T& elem) {
    if (count() == capacity()) return false;
    tail_->elem = elem;
    tail_ = tail_->next;
    count_.fetch_add(1, std::memory_order_relaxed);
    return true;
  }
  bool Get(T& elem) {
    if (empty()) return false;
    elem = head_->elem;
    head_ = head_->next;
    count_.fetch_sub(1, std::memory_order_relaxed);
    return true;
  }
  inline bool empty() const {
    return !count_.load();
  }
  inline size_t capacity() const {
    return capacity_;
  }
  inline size_t count() const {
    return count_.load();
  }
 private:
  size_t capacity_ = Size;
  struct Item* begin_ = nullptr;
  struct Item* end_ = nullptr;
  struct Item* head_ = nullptr;
  struct Item* tail_ = nullptr;
  std::array<Item, Size> nodes_;
  std::atomic_size_t count_{0};
};

} // mem
} // tools

#endif //MEM_HPP_
