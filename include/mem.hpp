/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2024-04-16 17:25:20
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2024-04-19 18:08:36
 * @Description: 
 */

#ifndef MEM_HPP_
#define MEM_HPP_

#include <atomic>
#include <array>
#include <map>
#include <vector>
#include <queue>

namespace tools {
namespace mem {

class FixedArena {
  class Node {
    struct Block {
      inline explicit Block(const size_t& size)
          : addr(nullptr) { addr = new int8_t[size]; }
      inline ~Block() {
        delete [] addr;
      }
      int8_t* addr;
    };
   public:
    using UPtr = std::unique_ptr<Node>;
    explicit Node(const size_t& size)
        : block_size_(size) {
      AllocateNewBlock();
    }
    ~Node() {
      for (auto& i : blocks_) {
        delete i;
      }
    }
    void Allocate(int8_t*& ptr) {
      auto _block = free_blocks_.empty() ? AllocateNewBlock() : free_blocks_.front();
      ptr = _block->addr;
      used_blocks_.insert({ _block->addr, _block });
      free_blocks_.pop();
    }
    void Deallocate(int8_t*& ptr) {
      auto res = used_blocks_.find(ptr);
      if (res == used_blocks_.end()) return;
      free_blocks_.push(res->second);
      used_blocks_.erase(res);
      ptr = nullptr;
    }
    inline size_t GetTotalSize() const { return blocks_.size() * block_size_; }
    inline size_t GetInUsedSize() const { return used_blocks_.size() * block_size_; }
    inline size_t GetFreeSize() const { return GetTotalSize() - GetInUsedSize(); }
   private:
    Block* AllocateNewBlock() {
      auto _block = new Block(block_size_);;
      blocks_.push_back(_block);
      free_blocks_.push(_block);
      return _block;
    }
   private:
    std::vector<Block*> blocks_;
    std::queue<Block*> free_blocks_;
    std::map<int8_t*, Block*> used_blocks_;
    size_t block_size_;
  };
 public:
  FixedArena(std::initializer_list<size_t> size_list) {
    for (auto size :size_list)
      nodes_.insert({size, std::make_unique<Node>(size)});
  }
  ~FixedArena() = default;
  void Allocate(size_t size, int8_t*& dst) {
    // TODO: Aligned
    auto res = nodes_.lower_bound(size);
    if (res != nodes_.end()) {
      res->second->Allocate(dst);
      return;
    }
    auto ptr = std::make_unique<Node>(size);
    ptr->Allocate(dst);
    nodes_.insert({size, std::move(ptr)});
  }
  void Deallocate(int8_t*& dst) {
    if (!dst) return;
    for (auto& i : nodes_) {
      i.second->Deallocate(dst);
    }
  }
  inline size_t GetTotalSize() {
    size_t size = 0;
    for (auto &i : nodes_) {
      size += i.second->GetTotalSize();
    }
    return size;
  }
  inline size_t GetInUsedSize() {
    size_t size = 0;
    for (auto &i : nodes_) {
      size += i.second->GetInUsedSize();
    }
    return size;
  }
  inline size_t GetFreeSize() {
    size_t size = 0;
    for (auto &i : nodes_) {
      size += i.second->GetFreeSize();
    }
    return size;
  }
  inline size_t GetNodeCount() {
    return nodes_.size();
  }
 private:
  std::map<size_t, Node::UPtr> nodes_;
};

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
